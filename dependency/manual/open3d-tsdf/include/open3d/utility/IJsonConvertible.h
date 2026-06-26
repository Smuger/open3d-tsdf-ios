// Stub IJsonConvertible.h — empty abstract base, no jsoncpp or fmt dependency
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once

// Minimal Json::Value forward declaration so that the virtual interface
// compiles without pulling in jsoncpp headers.
namespace Json { class Value; }

namespace open3d {
namespace utility {

class IJsonConvertible {
public:
    virtual ~IJsonConvertible() = default;
    virtual bool ConvertToJsonValue(Json::Value& value) const { return false; }
    virtual bool ConvertFromJsonValue(const Json::Value& value) { return false; }
};

}  // namespace utility
}  // namespace open3d
