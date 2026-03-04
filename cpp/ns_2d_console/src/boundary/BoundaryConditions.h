/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include "../array/Array2D.h"
#include <cstddef>

namespace ns2d {

/**
 * Applies Dirichlet boundary conditions: sets boundary values explicitly.
 * @param f 2D field to modify.
 * @param negx Value at x = 0 (left).
 * @param posx Value at x = right.
 * @param negy Value at y = 0 (bottom).
 * @param posy Value at y = top.
 */
inline void bound_dirichlet(Array2D& f, double negx, double posx, double negy, double posy) {
    const std::size_t res = f.rows();
    // negy (iy = 0)
    for (std::size_t ix = 0; ix < res; ++ix)
        f(0, ix) = negy;
    // posy (iy = res-1)
    for (std::size_t ix = 0; ix < res; ++ix)
        f(res - 1, ix) = posy;
    // negx (ix = 0)
    for (std::size_t iy = 0; iy < res; ++iy)
        f(iy, 0) = negx;
    // posx (ix = res-1)
    for (std::size_t iy = 0; iy < res; ++iy)
        f(iy, res - 1) = posx;
}

/**
 * Applies Neumann boundary conditions (zero normal gradient) by copying from the interior.
 * @param f 2D field to modify.
 */
inline void bound_neumann(Array2D& f) {
    const std::size_t res = f.rows();
    // negy
    for (std::size_t ix = 0; ix < res; ++ix)
        f(0, ix) = f(1, ix);
    // posy
    for (std::size_t ix = 0; ix < res; ++ix)
        f(res - 1, ix) = f(res - 2, ix);
    // negx
    for (std::size_t iy = 0; iy < res; ++iy)
        f(iy, 0) = f(iy, 1);
    // posx
    for (std::size_t iy = 0; iy < res; ++iy)
        f(iy, res - 1) = f(iy, res - 2);
}

} // namespace ns2d
