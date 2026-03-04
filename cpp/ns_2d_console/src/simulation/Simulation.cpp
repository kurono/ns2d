/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 */

#include "Simulation.h"
#include <cmath>

namespace ns2d {

/** Default constructor; uses default resolution and parameters. */
Simulation::Simulation() {
    h_ = dsize_ / static_cast<double>(res_ - 1);
    nu_ = mu / rho;
    init_fields();
}

/** Constructs simulation with the given grid resolution. */
Simulation::Simulation(std::size_t resolution) : res_{resolution} {
    h_ = dsize_ / static_cast<double>(res_ - 1);
    nu_ = mu / rho;
    init_fields();
}

/** Allocates and initializes u, v, p, and prev/umag fields. */
void Simulation::init_fields() {
    u_ = Array2D(res_, res_);
    v_ = Array2D(res_, res_);
    p_ = Array2D(res_, res_);
    uprev_ = Array2D(res_, res_);
    vprev_ = Array2D(res_, res_);
    umag_ = Array2D(res_, res_);
}

/** Computes velocity magnitude sqrt(u^2 + v^2) into umag_. */
void Simulation::compute_umag() {
    for (std::size_t iy = 0; iy < res_; ++iy) {
        for (std::size_t ix = 0; ix < res_; ++ix) {
            const double u = u_(iy, ix);
            const double v = v_(iy, ix);
            umag_(iy, ix) = std::sqrt(u * u + v * v);
        }
    }
}

/** Copies field f into fprev. */
void Simulation::current_to_prev(const Array2D& f, Array2D& fprev) {
    for (std::size_t iy = 0; iy < res_; ++iy) {
        for (std::size_t ix = 0; ix < res_; ++ix) {
            fprev(iy, ix) = f(iy, ix);
        }
    }
}

/** Applies Dirichlet velocity BCs (lid u_top) to u_ and v_. */
void Simulation::apply_velocity_bcs(double u_top) {
    bound_dirichlet(u_, 0.0, 0.0, 0.0, u_top);
    bound_dirichlet(v_, 0.0, 0.0, 0.0, 0.0);
}

/** Performs one Chorin step: advect, diffuse, Poisson, apply grad p; applies BCs with u_top on lid. */
void Simulation::solve(double u_top) {
    // 1: Advect velocity through itself (using previous step)
    advect(u_, uprev_, vprev_, h_, dt);
    advect(v_, uprev_, vprev_, h_, dt);
    apply_velocity_bcs(u_top);

    // 2: Diffuse velocity
    diffuse(u_, h_, dt, nu_);
    diffuse(v_, h_, dt, nu_);
    apply_velocity_bcs(u_top);

    // 3: Solve for pressure (Poisson)
    implicit_poisson(u_, v_, p_, h_, dt, rho, maxit);

    // 4: Apply pressure gradient
    apply_gradp(u_, v_, p_, h_, dt, rho);
    apply_velocity_bcs(u_top);

    // Update previous velocity for next step
    current_to_prev(u_, uprev_);
    current_to_prev(v_, vprev_);
}

} // namespace ns2d
