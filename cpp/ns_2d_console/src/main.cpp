/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include "simulation/Simulation.h"
#include "utils/Utils.h"
#include "visualization/ConsoleRenderer.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
    constexpr double w = 2e2;  // velocity direction change frequency [1/s]
}

/** Entry point: runs lid-driven cavity simulation with console visualization and FPS counter. */
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "Start\n";

    ns2d::Simulation sim;
    ns2d::ConsoleRenderer renderer(ns2d::ConsoleRenderer::default_con_h,
                                   ns2d::ConsoleRenderer::default_aspect);
    ns2d::Utils fps_counter;

    while (sim.t < sim.tmax) {
        sim.compute_umag();
        renderer.draw_field(sim.velocity_magnitude(), "u [m/s]", sim.t,
                           ns2d::ConsoleRenderer::default_eps, &sim.u_, &sim.v_, fps_counter.fps());

        const double u_posy = sim.umax * std::cos(w * sim.t);
        sim.solve(u_posy);
        sim.t += sim.dt;

        fps_counter.tick();
    }

    std::cout << "End\n";
    return 0;
}
