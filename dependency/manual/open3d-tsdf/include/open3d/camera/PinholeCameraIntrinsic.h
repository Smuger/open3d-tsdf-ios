// Stub PinholeCameraIntrinsic.h — standalone, no jsoncpp / fmt dependency.
// Preserves all fields and methods accessed by the TSDF integration code.
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <Eigen/Core>
#include <utility>

namespace open3d {
namespace camera {

enum class PinholeCameraIntrinsicParameters {
    PrimeSenseDefault         = 0,
    Kinect2DepthCameraDefault = 1,
    Kinect2ColorCameraDefault = 2,
};

class PinholeCameraIntrinsic {
public:
    PinholeCameraIntrinsic() = default;

    PinholeCameraIntrinsic(int width, int height, const Eigen::Matrix3d& m)
        : width_(width), height_(height), intrinsic_matrix_(m) {}

    PinholeCameraIntrinsic(int width,
                           int height,
                           double fx,
                           double fy,
                           double cx,
                           double cy)
        : width_(width), height_(height) {
        SetIntrinsics(width, height, fx, fy, cx, cy);
    }

    void SetIntrinsics(int width,
                       int height,
                       double fx,
                       double fy,
                       double cx,
                       double cy) {
        width_ = width;
        height_ = height;
        intrinsic_matrix_ = Eigen::Matrix3d::Identity();
        intrinsic_matrix_(0, 0) = fx;
        intrinsic_matrix_(1, 1) = fy;
        intrinsic_matrix_(0, 2) = cx;
        intrinsic_matrix_(1, 2) = cy;
    }

    std::pair<double, double> GetFocalLength() const {
        return {intrinsic_matrix_(0, 0), intrinsic_matrix_(1, 1)};
    }
    std::pair<double, double> GetPrincipalPoint() const {
        return {intrinsic_matrix_(0, 2), intrinsic_matrix_(1, 2)};
    }
    double GetSkew() const { return intrinsic_matrix_(0, 1); }
    bool IsValid() const { return width_ > 0 && height_ > 0; }

public:
    int width_ = -1;
    int height_ = -1;
    Eigen::Matrix3d intrinsic_matrix_ = Eigen::Matrix3d::Identity();
};

}  // namespace camera
}  // namespace open3d
