/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#pragma once

#include "../array/Array2D.h"
#include <cmath>
#include <cstdio>
#include <format>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace ns2d {

/**
 * Pseudo-colormap visualization of a 2D scalar field to the console (Unicode blocks like ns2djs).
 * Uses cursor-home overwrite (no cls) to avoid flicker and one bulk write for speed.
 * Console character aspect: height = 2*width; default_aspect = 2 keeps the magnitude grid square.
 */
class ConsoleRenderer {
public:
    static constexpr int default_con_h = 20;
    static constexpr int default_aspect = 2;  // con_w = con_h * aspect so physical aspect is square (2:1 char cells)

    /** Constructs renderer with given console height and aspect ratio (con_w = con_h * aspect_ratio). */
    explicit ConsoleRenderer(int con_h = default_con_h, int aspect_ratio = default_aspect)
        : con_h_{con_h}, con_w_{con_h * aspect_ratio} {}

    static constexpr double default_eps = 1.0e-9;

    /**
     * Draws a 2D scalar field to the console (Unicode block colormap and optional velocity arrows).
     * @param f 2D scalar field to draw (e.g. velocity magnitude).
     * @param title Title for the color bar axis.
     * @param sim_time Simulation time in seconds.
     * @param eps Small value added to denominator to avoid division by zero.
     * @param u Optional x-component of velocity for arrow panel; if non-null with v, vectors are drawn.
     * @param v Optional y-component of velocity for arrow panel.
     * @param fps Frames per second to show in header; if < 0, FPS line is omitted.
     */
    void draw_field(const Array2D& f, std::string_view title, double sim_time,
                    double eps = default_eps,
                    const Array2D* u = nullptr, const Array2D* v = nullptr,
                    double fps = -1.0) const {
        const std::size_t res = f.rows();
        const double val_min = f.min_value();
        const double val_max = f.max_value();
        const double denom = (val_max - val_min) + eps;

        // Unicode block colormap (same as ns2djs: light → full block)
        static constexpr const char* cmap[] = {" ", "\u2591", "\u2592", "\u2593", "\u2588"};
        constexpr std::size_t cmap_size = std::size(cmap) - 1;

        // 8-direction arrows (E, NE, N, NW, W, SW, S, SE) + dot for near-zero
        static constexpr const char* arrows[] = {
            "\u2192", "\u2197", "\u2191", "\u2196", "\u2190", "\u2199", "\u2193", "\u2198"
        };
        constexpr double arrow_eps = 1.0e-12;

        const bool draw_vectors = (u != nullptr && v != nullptr && u->rows() == res && v->rows() == res);
        // Match magnitude grid size so aspect ratio is correct (char height = 2*width)
        const int arrow_w = draw_vectors ? static_cast<int>(con_w_) : 0;

        std::string out;
        out.reserve((2 + static_cast<std::size_t>(con_h_)) * (static_cast<std::size_t>(con_w_) + 80 + static_cast<std::size_t>(arrow_w)));

        if (fps >= 0.0) {
            out += std::format("t = {:e} [s], FPS = {:.1f}\n\n", sim_time, fps);
        } else {
            out += std::format("t = {:e} [s]\n\n", sim_time);
        }

        int icb = static_cast<int>(cmap_size);
        bool odd_line = false;

        for (int py = 0; py < con_h_; ++py) {
            const double ny = (static_cast<double>(con_h_ - 1 - py) / (con_h_ - 1));

            // Velocity vectors panel (left)
            if (draw_vectors && arrow_w > 0) {
                const double inv_aw = (arrow_w > 1) ? 1.0 / (arrow_w - 1) : 1.0;
                for (int pa = 0; pa < arrow_w; ++pa) {
                    const double nx = static_cast<double>(pa) * inv_aw;
                    const std::size_t iy = static_cast<std::size_t>(
                        std::round((static_cast<double>(res) - 1.0) * ny));
                    const std::size_t ix = static_cast<std::size_t>(
                        std::round((static_cast<double>(res) - 1.0) * nx));
                    const std::size_t iy_c = (iy >= res) ? res - 1 : iy;
                    const std::size_t ix_c = (ix >= res) ? res - 1 : ix;
                    const double uval = (*u)(iy_c, ix_c);
                    const double vval = (*v)(iy_c, ix_c);
                    const double mag = uval * uval + vval * vval;
                    if (mag < arrow_eps * arrow_eps) {
                        out += "\u00B7";  // middle dot for zero
                    } else {
                        double angle = std::atan2(vval, uval);
                        if (angle < 0) angle += 6.283185307179586;  // 2*pi
                        constexpr double pi_4 = 0.785398163397448;
                        constexpr double pi_8 = 0.392699081698724;
                        int sector = static_cast<int>((angle + pi_8) / pi_4) % 8;
                        if (sector < 0) sector += 8;
                        out += arrows[static_cast<std::size_t>(sector)];
                    }
                }
                out += " | ";
            }

            // Magnitude field (right of vectors)
            for (int px = 0; px < con_w_; ++px) {
                const double nx = static_cast<double>(px) / (con_w_ - 1);
                const std::size_t iy = static_cast<std::size_t>(
                    std::round((static_cast<double>(res) - 1.0) * ny));
                const std::size_t ix = static_cast<std::size_t>(
                    std::round((static_cast<double>(res) - 1.0) * nx));
                const std::size_t iy_c = (iy >= res) ? res - 1 : iy;
                const std::size_t ix_c = (ix >= res) ? res - 1 : ix;
                const double fn = (f(iy_c, ix_c) - val_min) / denom;
                std::size_t ic = static_cast<std::size_t>(
                    std::round(static_cast<double>(cmap_size) * fn));
                if (ic > cmap_size) ic = cmap_size;
                out += cmap[ic];
            }
            // Vertical color bar (right of magnitude)
            if (odd_line) {
                if (icb >= 0) {
                    const double data_val = (cmap_size > 1)
                        ? (val_min + (val_max - val_min) * static_cast<double>(icb) / (cmap_size - 1.0))
                        : val_min;
                    out += std::format(" |{}| {:e}", cmap[static_cast<std::size_t>(icb)], data_val);
                } else {
                    out += " | |\t";
                }
                --icb;
            } else {
                if (py == 0) {
                    out += " ";
                    out += title;
                } else {
                    out += " | |\t";
                }
            }

            odd_line = !odd_line;
            out += '\n';
        }

#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            COORD top = {0, 0};
            SetConsoleCursorPosition(hOut, top);
            DWORD written = 0;
            WriteConsoleA(hOut, out.data(), static_cast<DWORD>(out.size()), &written, nullptr);
        }
#else
        std::fwrite("\033[H", 1, 3, stdout);  // ANSI cursor home
        std::fwrite(out.data(), 1, out.size(), stdout);
        std::fflush(stdout);
#endif
    }

private:
    int con_h_;
    int con_w_;
};

} // namespace ns2d
