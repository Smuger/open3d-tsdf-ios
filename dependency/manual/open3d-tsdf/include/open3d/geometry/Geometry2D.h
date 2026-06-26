// Stub Geometry2D.h — 2-D abstract base matching Open3D's interface
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include "open3d/geometry/Geometry.h"

namespace open3d {
namespace geometry {

class Geometry2D : public Geometry {
public:
    explicit Geometry2D(Geometry::GeometryType type) : Geometry(type, 2) {}
    virtual ~Geometry2D() = default;

    virtual Eigen::Vector2d GetMinBound() const = 0;
    virtual Eigen::Vector2d GetMaxBound() const = 0;
};

}  // namespace geometry
}  // namespace open3d
