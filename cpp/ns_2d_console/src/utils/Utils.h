/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include <chrono>

namespace ns2d {

/**
 * Utility helpers (e.g. FPS counting).
 */
class Utils {
public:
    /** Initializes the FPS counter (fps() returns 0 until the first tick()). */
    Utils() : t_prev_{std::chrono::steady_clock::now()}, fps_{0.0} {}

    /** Call at the end of each frame; updates FPS from elapsed time since last tick. */
    void tick() {
        const auto t_now = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(t_now - t_prev_).count();
        fps_ = (elapsed > 0.0) ? (1.0 / elapsed) : 0.0;
        t_prev_ = t_now;
    }

    /** Current FPS (from the last completed frame; 0 before first tick). */
    [[nodiscard]] double fps() const noexcept { return fps_; }

private:
    std::chrono::steady_clock::time_point t_prev_;
    double fps_;
};

} // namespace ns2d
