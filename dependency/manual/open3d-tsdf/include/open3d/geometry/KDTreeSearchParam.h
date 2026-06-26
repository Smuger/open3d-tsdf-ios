// Stub KDTreeSearchParam.h — forward declaration only; TSDF does not use KD-tree
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once

namespace open3d {
namespace geometry {

class KDTreeSearchParam {
public:
    enum class SearchType { Knn = 0, Radius = 1, Hybrid = 2 };
    virtual ~KDTreeSearchParam() = default;
    virtual SearchType GetSearchType() const = 0;
};

}  // namespace geometry
}  // namespace open3d
