/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include "../array/Array2D.h"
#include "../boundary/BoundaryConditions.h"
#include <cstddef>

namespace ns2d {

/**
 * Explicit diffusion step: d f / dt = nu * laplacian(f).
 * @param f Scalar field to diffuse (modified in place).
 * @param h Cell size.
 * @param dt Time step.
 * @param nu Kinematic viscosity.
 */
inline void diffuse(Array2D& f, double h, double dt, double nu) {
    const std::size_t res = f.rows();
    const double hh = h * h;
    for (std::size_t iy = 1; iy + 1 < res; ++iy) {
        for (std::size_t ix = 1; ix + 1 < res; ++ix) {
            const double d2fdx2 = (f(iy, ix + 1) + f(iy, ix - 1) - 2.0 * f(iy, ix)) / hh;
            const double d2fdy2 = (f(iy + 1, ix) + f(iy - 1, ix) - 2.0 * f(iy, ix)) / hh;
            const double lapf = d2fdx2 + d2fdy2;
            f(iy, ix) += dt * nu * lapf;
        }
    }
}

/**
 * Explicit advection: d f / dt = -(u df/dx + v df/dy).
 * @param f Scalar field to advect (modified in place).
 * @param u x-component of velocity.
 * @param v y-component of velocity.
 * @param h Cell size.
 * @param dt Time step.
 */
inline void advect(Array2D& f, const Array2D& u, const Array2D& v, double h, double dt) {
    const std::size_t res = f.rows();
    const double h2 = h * 2.0;
    for (std::size_t iy = 1; iy + 1 < res; ++iy) {
        for (std::size_t ix = 1; ix + 1 < res; ++ix) {
            const double dfdx = (f(iy, ix + 1) - f(iy, ix - 1)) / h2;
            const double dfdy = (f(iy + 1, ix) - f(iy - 1, ix)) / h2;
            const double unablaf = u(iy, ix) * dfdx + v(iy, ix) * dfdy;
            f(iy, ix) -= dt * unablaf;
        }
    }
}

/**
 * Solves the Poisson equation for pressure (Gauss–Seidel) with Neumann BCs.
 * @param u x-component of velocity.
 * @param v y-component of velocity.
 * @param p Pressure field (modified in place).
 * @param h Cell size.
 * @param dt Time step.
 * @param rho Density.
 * @param maxit Maximum Gauss–Seidel iterations.
 */
inline void implicit_poisson(const Array2D& u, const Array2D& v, Array2D& p,
                             double h, double dt, double rho, std::size_t maxit) {
    const std::size_t res = u.rows();
    const double hh = h * h;
    const double h2 = h * 2.0;
    for (std::size_t it = 0; it < maxit; ++it) {
        for (std::size_t iy = 1; iy + 1 < res; ++iy) {
            for (std::size_t ix = 1; ix + 1 < res; ++ix) {
                const double dudx = (u(iy, ix + 1) - u(iy, ix - 1)) / h2;
                const double dvdy = (v(iy + 1, ix) - v(iy - 1, ix)) / h2;
                const double divu = dudx + dvdy;
                const double src = (rho / dt) * divu;
                const double sum4 = p(iy + 1, ix) + p(iy - 1, ix) + p(iy, ix + 1) + p(iy, ix - 1);
                p(iy, ix) = 0.25 * (sum4 - hh * src);
            }
        }
        bound_neumann(p);
    }
}

/**
 * Applies the pressure gradient to velocity: u -= dt * (grad p) / rho.
 * @param u x-component of velocity (modified in place).
 * @param v y-component of velocity (modified in place).
 * @param p Pressure field.
 * @param h Cell size.
 * @param dt Time step.
 * @param rho Density.
 */
inline void apply_gradp(Array2D& u, Array2D& v, const Array2D& p,
                        double h, double dt, double rho) {
    const std::size_t res = u.rows();
    const double h2 = h * 2.0;
    for (std::size_t iy = 1; iy + 1 < res; ++iy) {
        for (std::size_t ix = 1; ix + 1 < res; ++ix) {
            const double dpdx = (p(iy, ix + 1) - p(iy, ix - 1)) / h2;
            const double dpdy = (p(iy + 1, ix) - p(iy - 1, ix)) / h2;
            u(iy, ix) -= dt * (dpdx / rho);
            v(iy, ix) -= dt * (dpdy / rho);
        }
    }
}

} // namespace ns2d
