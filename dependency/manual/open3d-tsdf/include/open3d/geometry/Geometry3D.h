// Stub Geometry3D.h — 3-D abstract base matching Open3D's interface
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>
#include "open3d/geometry/Geometry.h"
#include "open3d/utility/Eigen.h"

namespace open3d {
namespace geometry {

class Geometry3D : public Geometry {
public:
    explicit Geometry3D(Geometry::GeometryType type) : Geometry(type, 3) {}
    virtual ~Geometry3D() = default;

    virtual Eigen::Vector3d GetMinBound() const = 0;
    virtual Eigen::Vector3d GetMaxBound() const = 0;
    virtual Geometry3D& Transform(const Eigen::Matrix4d& transformation) = 0;
};

}  // namespace geometry
}  // namespace open3d
