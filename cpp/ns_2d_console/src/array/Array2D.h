/*
 * Copyright 2025 Ilya Tsivilskiy @Kei https://github.com/kurono
 * C++23 version
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace ns2d {

/**
 * 2D array of scalar values (row-major).
 */
class Array2D {
public:
    using value_type = double;

    /** Default-constructs an empty 2D array. */
    Array2D() = default;

    /** Constructs a rows x cols array filled with fill. */
    Array2D(std::size_t rows, std::size_t cols, value_type fill = 0.0)
        : rows_{rows}, cols_{cols}, data_(rows * cols, fill) {}

    /** Returns the number of rows. */
    [[nodiscard]] std::size_t rows() const noexcept { return rows_; }

    /** Returns the number of columns. */
    [[nodiscard]] std::size_t cols() const noexcept { return cols_; }

    /** Element access at (iy, ix), row-major. */
    [[nodiscard]] value_type& operator()(std::size_t iy, std::size_t ix) noexcept {
        return data_[iy * cols_ + ix];
    }

    /** Element access at (iy, ix), row-major. */
    [[nodiscard]] value_type operator()(std::size_t iy, std::size_t ix) const noexcept {
        return data_[iy * cols_ + ix];
    }

    /** Fills the entire array with v. */
    void fill(value_type v) { std::ranges::fill(data_, v); }

    /** Returns the minimum value in the array. */
    [[nodiscard]] value_type min_value() const {
        return *std::ranges::min_element(data_);
    }

    /** Returns the maximum value in the array. */
    [[nodiscard]] value_type max_value() const {
        return *std::ranges::max_element(data_);
    }

    /** Pointer to the underlying buffer (mutable). */
    [[nodiscard]] value_type* data() noexcept { return data_.data(); }

    /** Pointer to the underlying buffer (const). */
    [[nodiscard]] const value_type* data() const noexcept { return data_.data(); }

private:
    std::size_t rows_{0};
    std::size_t cols_{0};
    std::vector<value_type> data_;
};

} // namespace ns2d
