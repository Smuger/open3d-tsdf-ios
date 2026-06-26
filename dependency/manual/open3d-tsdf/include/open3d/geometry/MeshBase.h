// Stub MeshBase.h — minimal abstract base for triangle meshes
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "open3d/geometry/Geometry3D.h"

namespace open3d {
namespace geometry {

class MeshBase : public Geometry3D {
public:
    explicit MeshBase(Geometry::GeometryType type)
        : Geometry3D(type) {}
    ~MeshBase() override = default;

    MeshBase& Clear() override {
        vertices_.clear();
        vertex_normals_.clear();
        vertex_colors_.clear();
        return *this;
    }
    bool IsEmpty() const override { return vertices_.empty(); }
    Eigen::Vector3d GetMinBound() const override {
        return Eigen::Vector3d::Zero();
    }
    Eigen::Vector3d GetMaxBound() const override {
        return Eigen::Vector3d::Zero();
    }
    MeshBase& Transform(const Eigen::Matrix4d&) override { return *this; }

    bool HasVertices() const { return !vertices_.empty(); }
    bool HasVertexNormals() const {
        return !vertices_.empty() &&
               vertex_normals_.size() == vertices_.size();
    }
    bool HasVertexColors() const {
        return !vertices_.empty() &&
               vertex_colors_.size() == vertices_.size();
    }

public:
    std::vector<Eigen::Vector3d> vertices_;
    std::vector<Eigen::Vector3d> vertex_normals_;
    std::vector<Eigen::Vector3d> vertex_colors_;
};

}  // namespace geometry
}  // namespace open3d
