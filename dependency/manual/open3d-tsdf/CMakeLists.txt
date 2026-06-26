cmake_minimum_required(VERSION 3.20)
project(open3d_tsdf VERSION 0.19.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Eigen ─────────────────────────────────────────────────────────────────────
# Conan provides Eigen via find_package when the caller runs `conan install`.
# Fallback: find it in the system / toolchain.
find_package(Eigen3 REQUIRED NO_MODULE)

# ── Source files ──────────────────────────────────────────────────────────────
# Only the TSDF integration algorithm and the Image type it reads from.
# All other Open3D geometry / utility headers are minimal stubs in include/.
set(SOURCES
    src/pipelines/integration/UniformTSDFVolume.cpp
    src/pipelines/integration/ScalableTSDFVolume.cpp
    src/geometry/Image.cpp
    src/geometry/ImageFactory.cpp
    src/geometry/PointCloudFactory.cpp
)

add_library(open3d_tsdf STATIC ${SOURCES})

target_include_directories(open3d_tsdf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(open3d_tsdf PUBLIC Eigen3::Eigen)

# Silence warnings originating in the verbatim Open3D source files.
target_compile_options(open3d_tsdf PRIVATE
    -Wno-unused-parameter
    -Wno-sign-compare
)

# ── Install ───────────────────────────────────────────────────────────────────
include(GNUInstallDirs)

install(TARGETS open3d_tsdf
    EXPORT  open3d_tsdf_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT open3d_tsdf_targets
    FILE        open3d_tsdtTargets.cmake
    NAMESPACE   open3d_tsdf::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/open3d_tsdf
)

# ── Package config ────────────────────────────────────────────────────────────
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/open3d_tsdfConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/open3d_tsdfConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/open3d_tsdf
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/open3d_tsdfConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/open3d_tsdfConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/open3d_tsdfConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/open3d_tsdf
)
