// Stub PointCloud.h — only the fields accessed by TSDF integration
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include <memory>
#include <vector>
#include "open3d/geometry/Geometry3D.h"
#include "open3d/geometry/Image.h"
#include "open3d/utility/Eigen.h"

// Forward-declare camera type to avoid circular includes
namespace open3d { namespace camera { class PinholeCameraIntrinsic; } }

namespace open3d {
namespace geometry {

class PointCloud : public Geometry3D {
public:
    PointCloud() : Geometry3D(Geometry::GeometryType::PointCloud) {}
    ~PointCloud() override = default;

    PointCloud& Clear() override {
        points_.clear();
        normals_.clear();
        colors_.clear();
        return *this;
    }
    bool IsEmpty() const override { return points_.empty(); }
    Eigen::Vector3d GetMinBound() const override {
        return Eigen::Vector3d::Zero();
    }
    Eigen::Vector3d GetMaxBound() const override {
        return Eigen::Vector3d::Zero();
    }
    PointCloud& Transform(const Eigen::Matrix4d&) override { return *this; }

    bool HasPoints() const { return !points_.empty(); }
    bool HasNormals() const {
        return !normals_.empty() && normals_.size() == points_.size();
    }
    bool HasColors() const {
        return !colors_.empty() && colors_.size() == points_.size();
    }

    // Factory method used by ScalableTSDFVolume — implemented in PointCloudFactory.cpp
    static std::shared_ptr<PointCloud> CreateFromDepthImage(
            const Image& depth,
            const camera::PinholeCameraIntrinsic& intrinsic,
            const Eigen::Matrix4d& extrinsic = Eigen::Matrix4d::Identity(),
            double depth_scale = 1000.0,
            double depth_trunc = 1000.0,
            int stride = 1,
            bool project_valid_depth_only = true);

public:
    std::vector<Eigen::Vector3d> points_;
    std::vector<Eigen::Vector3d> normals_;
    std::vector<Eigen::Vector3d> colors_;
};

}  // namespace geometry
}  // namespace open3d
