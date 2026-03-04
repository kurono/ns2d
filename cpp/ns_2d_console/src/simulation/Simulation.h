/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include "../array/Array2D.h"
#include "../boundary/BoundaryConditions.h"
#include "../solver/NavierStokesSolver.h"
#include <cstddef>

namespace ns2d {

/**
 * Main Navier–Stokes simulation state and parameters.
 * Chorin projection (advect → diffuse → pressure Poisson → apply grad p).
 */
class Simulation {
public:
    static constexpr double EPS = 1.0e-09;

    // Mesh
    std::size_t res_{40};
    double dsize_{0.1};
    double h_{dsize_ / (res_ - 1.0)};

    // Time
    double t{0.0};
    double tmax{10.0};
    double dt{1.0e-5};
    std::size_t maxit{20};

    // Material
    double rho{1000.0};
    double mu{100.0};
    double nu_{mu / rho};

    // Top boundary velocity [m/s]
    double umax{0.01};

    // Fields
    Array2D u_, v_, p_;
    Array2D umag_, uprev_, vprev_;

    /** Default constructor; uses default resolution and parameters. */
    Simulation();

    /** Constructs simulation with the given grid resolution. */
    explicit Simulation(std::size_t resolution);

    /** Allocates and initializes u, v, p, and prev/umag fields. */
    void init_fields();

    /** Computes velocity magnitude sqrt(u^2 + v^2) into umag_. */
    void compute_umag();

    /** Performs one Chorin step: advect, diffuse, Poisson, apply grad p; applies BCs with u_top on lid. */
    void solve(double u_top);

    /** Returns the current velocity magnitude field. */
    [[nodiscard]] const Array2D& velocity_magnitude() const noexcept { return umag_; }

    /** Returns the grid resolution (number of points per side). */
    [[nodiscard]] std::size_t resolution() const noexcept { return res_; }

    /** Returns the cell size h. */
    [[nodiscard]] double cell_size() const noexcept { return h_; }

private:
    /** Copies field f into fprev. */
    void current_to_prev(const Array2D& f, Array2D& fprev);

    /** Applies Dirichlet velocity BCs (lid u_top) to u_ and v_. */
    void apply_velocity_bcs(double u_top);
};

} // namespace ns2d
