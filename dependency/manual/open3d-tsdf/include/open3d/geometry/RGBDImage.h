// Stub RGBDImage.h — minimal class matching the fields accessed by TSDF integration
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once
#include <memory>
#include <vector>
#include "open3d/geometry/Geometry2D.h"
#include "open3d/geometry/Image.h"

namespace open3d {
namespace geometry {

class RGBDImage;

typedef std::vector<std::shared_ptr<RGBDImage>> RGBDImagePyramid;

class RGBDImage : public Geometry2D {
public:
    RGBDImage() : Geometry2D(Geometry::GeometryType::RGBDImage) {}
    RGBDImage(const Image& color, const Image& depth)
        : Geometry2D(Geometry::GeometryType::RGBDImage),
          color_(color),
          depth_(depth) {}
    ~RGBDImage() override {
        color_.Clear();
        depth_.Clear();
    }

    RGBDImage& Clear() override {
        color_.Clear();
        depth_.Clear();
        return *this;
    }
    bool IsEmpty() const override {
        return color_.IsEmpty() && depth_.IsEmpty();
    }
    Eigen::Vector2d GetMinBound() const override {
        return Eigen::Vector2d(0.0, 0.0);
    }
    Eigen::Vector2d GetMaxBound() const override {
        return Eigen::Vector2d(color_.width_, color_.height_);
    }

public:
    Image color_;
    Image depth_;
};

}  // namespace geometry
}  // namespace open3d
