// Stub Optional.h — delegates to C++17 std::optional
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <optional>
namespace open3d {
namespace utility {
template <typename T>
using optional = std::optional<T>;
}  // namespace utility
}  // namespace open3d
