#!/usr/bin/env bash
# extract_open3d_tsdf.sh — re-extract verbatim Open3D TSDF files for a new version pin.
#
# Usage:
#   scripts/extract_open3d_tsdf.sh [--version X.Y.Z] [--sha256 <hash>] [--commit <sha>] [--dry-run]
#
# With no arguments: prints the currently pinned version and exits.
# With --version:    downloads that Open3D release, verifies SHA-256, copies verbatim
#                    files, and updates deps.json + CMakeLists.txt + the workflow artifact name.
#
# Stub headers are NEVER overwritten — they must be reviewed manually if the TSDF
# algorithm changes significantly between Open3D versions.
#
# After running:
#   1. git diff dependency/manual/open3d-tsdf/   — review API changes in verbatim files
#   2. Check stubs listed at the end of output against upstream headers for any new deps
#   3. git add dependency/manual/open3d-tsdf/ deps.json
#   4. git commit -m "chore: update open3d-tsdf to vX.Y.Z"
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
TSDF_DIR="${REPO_ROOT}/dependency/manual/open3d-tsdf"
DEPS_JSON="${REPO_ROOT}/deps.json"
WORKFLOW_YML="${REPO_ROOT}/.github/workflows/build-open3d-ios.yml"
OPEN3D_REPO="isl-org/Open3D"

# ── Usage ─────────────────────────────────────────────────────────────────────
usage() {
    cat <<EOF
Usage: $(basename "$0") --version X.Y.Z [--sha256 <hash>] [--commit <sha>] [--dry-run]

Re-extracts verbatim Open3D TSDF source and headers into dependency/manual/open3d-tsdf/
for a new Open3D version. Also updates deps.json, CMakeLists.txt, and the workflow
artifact name. Stub headers are never overwritten.

Options:
  --version X.Y.Z   Open3D release tag to extract from (required to make changes)
  --sha256 <hash>   Expected SHA-256 of the source archive (verified if supplied;
                    computed and reported if omitted — add to deps.json for future runs)
  --commit <sha>    Full commit SHA of the release tag (fetched from GitHub if omitted)
  --dry-run         Show what would be copied without modifying any file

Run with no arguments to print the currently pinned version.
EOF
}

# ── Argument parsing ──────────────────────────────────────────────────────────
VERSION=""
EXPECTED_SHA256=""
EXPECTED_COMMIT=""
DRY_RUN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --version)   VERSION="$2";           shift 2 ;;
        --sha256)    EXPECTED_SHA256="$2";   shift 2 ;;
        --commit)    EXPECTED_COMMIT="$2";   shift 2 ;;
        --dry-run)   DRY_RUN=true;           shift   ;;
        -h|--help)   usage; exit 0            ;;
        *) printf 'Unknown flag: %s\n' "$1" >&2; usage; exit 1 ;;
    esac
done

# ── No-args: print current pin and exit ───────────────────────────────────────
if [[ -z "$VERSION" ]]; then
    current=$(python3 -c "
import json
d = json.load(open('${DEPS_JSON}'))
t = d['cpp_deps']['open3d_tsdf']
print(f\"Open3D TSDF currently pinned to v{t['version']} (commit {t.get('commit','?')})\")
")
    printf '%s\n' "$current"
    printf 'Run with --version X.Y.Z to update.\n'
    exit 0
fi

# ── File manifest ─────────────────────────────────────────────────────────────
# Each entry: "open3d_relative_path:local_relative_path"
# open3d_relative_path is under <archive>/cpp/open3d/
# local_relative_path  is under dependency/manual/open3d-tsdf/

VERBATIM_SOURCES=(
    "pipelines/integration/UniformTSDFVolume.cpp:src/pipelines/integration/UniformTSDFVolume.cpp"
    "pipelines/integration/ScalableTSDFVolume.cpp:src/pipelines/integration/ScalableTSDFVolume.cpp"
    "geometry/Image.cpp:src/geometry/Image.cpp"
    "geometry/ImageFactory.cpp:src/geometry/ImageFactory.cpp"
    "geometry/PointCloudFactory.cpp:src/geometry/PointCloudFactory.cpp"
)

VERBATIM_HEADERS=(
    "geometry/Image.h:include/open3d/geometry/Image.h"
    "pipelines/integration/MarchingCubesConst.h:include/open3d/pipelines/integration/MarchingCubesConst.h"
    "pipelines/integration/ScalableTSDFVolume.h:include/open3d/pipelines/integration/ScalableTSDFVolume.h"
    "pipelines/integration/TSDFVolume.h:include/open3d/pipelines/integration/TSDFVolume.h"
    "pipelines/integration/UniformTSDFVolume.h:include/open3d/pipelines/integration/UniformTSDFVolume.h"
    "utility/Eigen.h:include/open3d/utility/Eigen.h"
    "utility/Helper.h:include/open3d/utility/Helper.h"
)

# These are custom stubs that replace Open3D's heavy dependencies (fmt, jsoncpp,
# tbb). They are NEVER overwritten — review them manually if the TSDF algorithm
# gains new dependencies in the upstream version you are moving to.
STUB_HEADERS=(
    "include/open3d/Macro.h"
    "include/open3d/camera/PinholeCameraIntrinsic.h"
    "include/open3d/geometry/Geometry.h"
    "include/open3d/geometry/Geometry2D.h"
    "include/open3d/geometry/Geometry3D.h"
    "include/open3d/geometry/KDTreeSearchParam.h"
    "include/open3d/geometry/MeshBase.h"
    "include/open3d/geometry/PointCloud.h"
    "include/open3d/geometry/RGBDImage.h"
    "include/open3d/geometry/TriangleMesh.h"
    "include/open3d/geometry/VoxelGrid.h"
    "include/open3d/utility/IJsonConvertible.h"
    "include/open3d/utility/Logging.h"
    "include/open3d/utility/Optional.h"
    "include/open3d/utility/Parallel.h"
)

ARCHIVE_URL="https://github.com/${OPEN3D_REPO}/archive/refs/tags/v${VERSION}.tar.gz"
ARCHIVE_CACHE="/tmp/open3d-v${VERSION}-source.tar.gz"
EXTRACT_DIR="/tmp/open3d-extract-${VERSION}-$$"

# ── Resolve commit SHA for the tag ───────────────────────────────────────────
if [[ -z "$EXPECTED_COMMIT" ]]; then
    printf '=== Resolving tag commit via GitHub API ===\n'
    ref_json=$(curl -fsSL "https://api.github.com/repos/${OPEN3D_REPO}/git/ref/tags/v${VERSION}")
    obj_type=$(printf '%s' "$ref_json" | python3 -c "import json,sys; print(json.load(sys.stdin)['object']['type'])")
    obj_sha=$(printf '%s'  "$ref_json" | python3 -c "import json,sys; print(json.load(sys.stdin)['object']['sha'])")
    if [[ "$obj_type" == "tag" ]]; then
        # Annotated tag — dereference tag object to the underlying commit
        EXPECTED_COMMIT=$(curl -fsSL "https://api.github.com/repos/${OPEN3D_REPO}/git/tags/${obj_sha}" \
            | python3 -c "import json,sys; print(json.load(sys.stdin)['object']['sha'])")
    else
        EXPECTED_COMMIT="$obj_sha"
    fi
    printf '  commit: %s\n' "$EXPECTED_COMMIT"
fi

# ── Download ──────────────────────────────────────────────────────────────────
printf '\n=== Downloading Open3D v%s ===\n' "$VERSION"
if [[ -f "$ARCHIVE_CACHE" ]]; then
    printf '  using cached archive: %s\n' "$ARCHIVE_CACHE"
else
    printf '  %s\n' "$ARCHIVE_URL"
    curl -fsSL "$ARCHIVE_URL" -o "$ARCHIVE_CACHE"
    printf '  saved to: %s\n' "$ARCHIVE_CACHE"
fi

# ── Verify SHA-256 ────────────────────────────────────────────────────────────
printf '\n=== Verifying archive ===\n'
ACTUAL_SHA256=$(shasum -a 256 "$ARCHIVE_CACHE" | awk '{print $1}')
if [[ -n "$EXPECTED_SHA256" ]]; then
    if [[ "$ACTUAL_SHA256" != "$EXPECTED_SHA256" ]]; then
        printf 'ERROR: SHA-256 mismatch\n  expected: %s\n  actual:   %s\n' \
            "$EXPECTED_SHA256" "$ACTUAL_SHA256" >&2
        rm -f "$ARCHIVE_CACHE"
        exit 1
    fi
    printf '  SHA-256 OK: %s\n' "$ACTUAL_SHA256"
else
    printf '  SHA-256 (unverified — supply --sha256 next time): %s\n' "$ACTUAL_SHA256"
fi

if $DRY_RUN; then
    printf '\n[dry-run] would extract and copy files — skipping\n'
else
    # ── Extract ───────────────────────────────────────────────────────────────
    printf '\n=== Extracting archive ===\n'
    mkdir -p "$EXTRACT_DIR"
    trap 'rm -rf "$EXTRACT_DIR"' EXIT
    tar -xf "$ARCHIVE_CACHE" -C "$EXTRACT_DIR" --strip-components=1
    OPEN3D_CPP="${EXTRACT_DIR}/cpp/open3d"
fi

# ── Helper: copy one file ─────────────────────────────────────────────────────
copy_file() {
    local src_rel="$1" dst_rel="$2"
    if $DRY_RUN; then
        printf '  [dry-run] %s\n' "$dst_rel"
        return
    fi
    local src="${OPEN3D_CPP}/${src_rel}"
    local dst="${TSDF_DIR}/${dst_rel}"
    if [[ ! -f "$src" ]]; then
        printf 'ERROR: source file not found in archive: cpp/open3d/%s\n' "$src_rel" >&2
        exit 1
    fi
    mkdir -p "$(dirname "$dst")"
    cp "$src" "$dst"
    printf '  %s\n' "$dst_rel"
}

# ── Copy verbatim source files ────────────────────────────────────────────────
printf '\n=== Verbatim source files ===\n'
for entry in "${VERBATIM_SOURCES[@]}"; do
    copy_file "${entry%%:*}" "${entry##*:}"
done

# ── Copy verbatim headers ─────────────────────────────────────────────────────
printf '\n=== Verbatim headers ===\n'
for entry in "${VERBATIM_HEADERS[@]}"; do
    copy_file "${entry%%:*}" "${entry##*:}"
done

# ── Stub headers — not touched ────────────────────────────────────────────────
printf '\n=== Stub headers (not overwritten) ===\n'
for stub in "${STUB_HEADERS[@]}"; do
    printf '  kept  %s\n' "$stub"
done

# ── Update deps.json ──────────────────────────────────────────────────────────
if ! $DRY_RUN; then
    printf '\n=== Updating deps.json ===\n'
    python3 - "$DEPS_JSON" "$VERSION" "$EXPECTED_COMMIT" "$ACTUAL_SHA256" "$ARCHIVE_URL" <<'PYEOF'
import json, sys
path, version, commit, sha256, url = sys.argv[1:]
with open(path) as f:
    d = json.load(f)
d["cpp_deps"]["open3d_tsdf"].update({
    "version":  version,
    "commit":   commit[:7],
    "url":      url,
    "sha256":   sha256,
    "note":     f"TSDF subsystem extracted from Open3D v{version}",
})
with open(path, "w") as f:
    json.dump(d, f, indent=2)
    f.write("\n")
PYEOF
    printf '  updated\n'

    # ── Update CMakeLists.txt project VERSION ─────────────────────────────────
    printf '\n=== Updating CMakeLists.txt ===\n'
    sed -i '' \
        "s/project(open3d_tsdf VERSION [0-9][0-9.]*  *LANGUAGES CXX)/project(open3d_tsdf VERSION ${VERSION} LANGUAGES CXX)/" \
        "${TSDF_DIR}/CMakeLists.txt"
    printf '  project version → %s\n' "$VERSION"

    # ── Update workflow artifact name ─────────────────────────────────────────
    if [[ -f "$WORKFLOW_YML" ]]; then
        printf '\n=== Updating workflow artifact name ===\n'
        sed -i '' \
            "s/name: open3d-tsdf-iOS-arm64-v[0-9][0-9.]*/name: open3d-tsdf-iOS-arm64-v${VERSION}/" \
            "$WORKFLOW_YML"
        printf '  artifact name → open3d-tsdf-iOS-arm64-v%s\n' "$VERSION"
    fi
fi

# ── Next steps ────────────────────────────────────────────────────────────────
printf '\n=== Done%s ===\n' "$(${DRY_RUN} && printf ' (dry-run)' || printf '')"
printf '\nNext steps:\n'
if ! $DRY_RUN; then
    printf '  1. Review verbatim file changes:\n'
    printf '       git diff dependency/manual/open3d-tsdf/\n'
    printf '  2. Review stubs against upstream headers for new #include dependencies;\n'
    printf '     upstream headers are at cpp/open3d/ in the Open3D source tree.\n'
    printf '     Stubs that may need updating:\n'
    for stub in "${STUB_HEADERS[@]}"; do
        printf '       %s\n' "$stub"
    done
    if [[ -z "$EXPECTED_SHA256" ]]; then
        printf '  3. Record the SHA-256 in deps.json (already written) or pass --sha256\n'
        printf '     next time to verify the download:\n'
        printf '       %s\n' "$ACTUAL_SHA256"
    fi
    printf '  4. Commit:\n'
    printf '       git add dependency/manual/open3d-tsdf/ deps.json\n'
    if [[ -f "$WORKFLOW_YML" ]]; then
        printf '       git add .github/workflows/build-open3d-ios.yml\n'
    fi
    printf '       git commit -m "chore: update open3d-tsdf to v%s"\n' "$VERSION"
else
    printf '  Re-run without --dry-run to apply changes.\n'
fi
