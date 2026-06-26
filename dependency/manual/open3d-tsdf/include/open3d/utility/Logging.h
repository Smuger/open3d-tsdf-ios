// Stub Logging.h — maps Open3D logging to stderr without the fmt dependency.
// The Open3D source calls LogError with {}-style format strings.  Rather than
// pulling in fmt, we print only the format string as-is (args dropped) which
// is safe and readable for diagnostic purposes.
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <cstdio>
#include <string>

namespace open3d {
namespace utility {

// Variadic overload: prints the format string unchanged (args dropped).
// Open3D passes {}-style fmt placeholders which are not printf-compatible,
// so we deliberately ignore the args rather than pass them to fprintf.
template <typename... Args>
inline void LogError(const char* msg, Args&&...) {
    fprintf(stderr, "[Open3D-TSDF ERROR] %s\n", msg);
}

template <typename... Args>
inline void LogWarning(const char* msg, Args&&...) {
    fprintf(stderr, "[Open3D-TSDF WARNING] %s\n", msg);
}

template <typename... Args>
inline void LogInfo(const char*, Args&&...) {}

template <typename... Args>
inline void LogDebug(const char*, Args&&...) {}

// Single-string overloads.
inline void LogError(const std::string& msg) {
    fprintf(stderr, "[Open3D-TSDF ERROR] %s\n", msg.c_str());
}
inline void LogWarning(const std::string& msg) {
    fprintf(stderr, "[Open3D-TSDF WARNING] %s\n", msg.c_str());
}
inline void LogInfo(const std::string&) {}
inline void LogDebug(const std::string&) {}

}  // namespace utility
}  // namespace open3d
