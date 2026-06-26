// Stub Parallel.h — single-threaded stubs sufficient for iOS TSDF extraction
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
namespace open3d {
namespace utility {
inline int EstimateMaxThreads() { return 1; }
inline bool InParallel() { return false; }
}  // namespace utility
}  // namespace open3d
