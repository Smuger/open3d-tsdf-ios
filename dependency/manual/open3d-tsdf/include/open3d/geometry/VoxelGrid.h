// Stub VoxelGrid.h — class declaration only; ExtractVoxelGrid() is a debug method
// not used in production scanning.
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include <memory>
#include <unordered_map>
#include <vector>
#include "open3d/geometry/Geometry3D.h"

namespace open3d {
namespace geometry {

struct Voxel {
    Voxel() {}
    explicit Voxel(const Eigen::Vector3i& grid_index)
        : grid_index_(grid_index) {}
    Voxel(const Eigen::Vector3i& grid_index, const Eigen::Vector3d& color)
        : grid_index_(grid_index), color_(color) {}
    Eigen::Vector3i grid_index_ = Eigen::Vector3i::Zero();
    Eigen::Vector3d color_ = Eigen::Vector3d::Zero();
};

class VoxelGrid : public Geometry3D {
public:
    VoxelGrid() : Geometry3D(Geometry::GeometryType::VoxelGrid) {}
    ~VoxelGrid() override = default;

    VoxelGrid& Clear() override {
        voxels_.clear();
        return *this;
    }
    bool IsEmpty() const override { return voxels_.empty(); }
    Eigen::Vector3d GetMinBound() const override {
        return origin_;
    }
    Eigen::Vector3d GetMaxBound() const override {
        return origin_;
    }
    VoxelGrid& Transform(const Eigen::Matrix4d&) override { return *this; }

    void AddVoxel(const Voxel& voxel) {
        voxels_[voxel.grid_index_] = voxel;
    }

public:
    double voxel_size_ = 0.0;
    Eigen::Vector3d origin_ = Eigen::Vector3d::Zero();
    struct VoxelHasher {
        size_t operator()(const Eigen::Vector3i& v) const {
            return std::hash<int>()(v[0]) ^ (std::hash<int>()(v[1]) << 1) ^
                   (std::hash<int>()(v[2]) << 2);
        }
    };
    std::unordered_map<Eigen::Vector3i, Voxel, VoxelHasher> voxels_;
};

}  // namespace geometry
}  // namespace open3d
