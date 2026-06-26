// Stub Geometry.h — minimal abstract base class matching Open3D's interface
// Original: Copyright (c) 2018-2024 www.open3d.org  SPDX-License-Identifier: MIT
#pragma once

namespace open3d {
namespace geometry {

class Geometry {
public:
    enum class GeometryType {
        Unspecified            = 0,
        PointCloud             = 1,
        VoxelGrid              = 2,
        Octree                 = 3,
        LineSet                = 4,
        MeshBase               = 5,
        TriangleMesh           = 6,
        HalfEdgeTriangleMesh   = 7,
        Image                  = 8,
        RGBDImage              = 9,
        TetraMesh              = 10,
        OrientedBoundingBox    = 11,
        AxisAlignedBoundingBox = 12,
    };

    explicit Geometry(GeometryType type, int dimension)
        : geometry_type_(type), dimension_(dimension) {}
    virtual ~Geometry() = default;

    virtual Geometry& Clear() = 0;
    virtual bool IsEmpty() const = 0;

    GeometryType GetGeometryType() const { return geometry_type_; }
    int Dimension() const { return dimension_; }

protected:
    GeometryType geometry_type_ = GeometryType::Unspecified;
    int dimension_ = 0;
};

}  // namespace geometry
}  // namespace open3d
