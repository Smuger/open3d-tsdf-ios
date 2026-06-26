// Stub TriangleMesh.h — only the fields accessed by TSDF integration
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "open3d/geometry/MeshBase.h"

namespace open3d {
namespace geometry {

class TriangleMesh : public MeshBase {
public:
    TriangleMesh()
        : MeshBase(Geometry::GeometryType::TriangleMesh) {}
    ~TriangleMesh() override = default;

    TriangleMesh& Clear() override {
        MeshBase::Clear();
        triangles_.clear();
        triangle_normals_.clear();
        triangle_uvs_.clear();
        return *this;
    }

    bool HasTriangles() const {
        return !vertices_.empty() && !triangles_.empty();
    }
    bool HasTriangleNormals() const {
        return HasTriangles() &&
               triangle_normals_.size() == triangles_.size();
    }
    bool HasTriangleUvs() const {
        return HasTriangles() && triangle_uvs_.size() == 3 * triangles_.size();
    }

public:
    std::vector<Eigen::Vector3i> triangles_;
    std::vector<Eigen::Vector3d> triangle_normals_;
    std::vector<Eigen::Vector2d> triangle_uvs_;
};

}  // namespace geometry
}  // namespace open3d
