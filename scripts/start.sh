#!/usr/bin/env bash
# ==============================================================================
# Smitty Services - Start Script
# ==============================================================================
# Builds (if needed) and launches the Smitty Services application.
#
# Usage:
#   ./start.sh                           # Build & run with defaults
#   ./start.sh --port 9090               # Override frontend port
#   ./start.sh --api-port 5657           # Override backend API port
#   ./start.sh --build-only              # Build without running
#   ./start.sh --run-only               # Run without building
#   ./start.sh --clean                   # Clean build then start
#   ./start.sh --help                    # Show usage
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# ------------------------------------------------------------------------------
# Parse command-line arguments (override env.sh defaults)
# ------------------------------------------------------------------------------
BUILD=true
RUN=true
CLEAN=false

usage() {
    cat <<USAGE
Usage: $(basename "$0") [OPTIONS]

Options:
  --port PORT          Frontend application port (default: 8080)
  --host HOST          Frontend bind address (default: 0.0.0.0)
  --api-port PORT      Backend API port (default: 5656)
  --api-host HOST      Backend API host (default: localhost)
  --db-port PORT       Database port (default: 5432)
  --db-host HOST       Database host (default: localhost)
  --db-name NAME       Database name (default: smitty_services)
  --db-user USER       Database user (default: smitty)
  --db-pass PASS       Database password (default: smitty_secret)
  --docroot PATH       Static resources directory (default: ./resources)
  --build-dir DIR      Build output directory (default: build)
  --build-only         Build without running
  --run-only           Run without building (executable must exist)
  --clean              Clean build directory before building
  --verbose            Print environment configuration
  --help               Show this help message

Environment:
  All options can also be set via environment variables before running.
  See env.sh for the full list of SMITTY_* variables.

Examples:
  ./start.sh                                    # Default startup
  ./start.sh --port 9090 --api-port 5657        # Custom ports
  SMITTY_APP_PORT=9090 ./start.sh               # Via environment
  ./start.sh --clean --verbose                  # Clean rebuild with output
USAGE
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --port)         export SMITTY_APP_PORT="$2"; shift 2 ;;
        --host)         export SMITTY_APP_HOST="$2"; shift 2 ;;
        --api-port)     export SMITTY_API_PORT="$2"; shift 2 ;;
        --api-host)     export SMITTY_API_HOST="$2"; shift 2 ;;
        --db-port)      export SMITTY_DB_PORT="$2"; shift 2 ;;
        --db-host)      export SMITTY_DB_HOST="$2"; shift 2 ;;
        --db-name)      export SMITTY_DB_NAME="$2"; shift 2 ;;
        --db-user)      export SMITTY_DB_USER="$2"; shift 2 ;;
        --db-pass)      export SMITTY_DB_PASS="$2"; shift 2 ;;
        --docroot)      export SMITTY_DOCROOT="$2"; shift 2 ;;
        --build-dir)    export SMITTY_BUILD_DIR="$2"; shift 2 ;;
        --build-only)   BUILD=true; RUN=false; shift ;;
        --run-only)     BUILD=false; RUN=true; shift ;;
        --clean)        CLEAN=true; shift ;;
        --verbose)      export SMITTY_VERBOSE=1; shift ;;
        --help|-h)      usage ;;
        *)              echo "Unknown option: $1"; usage ;;
    esac
done

# ------------------------------------------------------------------------------
# Source environment configuration
# ------------------------------------------------------------------------------
source "${SCRIPT_DIR}/env.sh"

echo "============================================"
echo "  Smitty Services"
echo "============================================"
echo "  Frontend:  http://${SMITTY_APP_HOST}:${SMITTY_APP_PORT}"
echo "  Backend:   ${SMITTY_API_ENDPOINT}"
echo "  Database:  ${SMITTY_DB_HOST}:${SMITTY_DB_PORT}/${SMITTY_DB_NAME}"
echo "============================================"

# ------------------------------------------------------------------------------
# Build
# ------------------------------------------------------------------------------
if [ "$BUILD" = true ]; then
    if [ "$CLEAN" = true ] && [ -d "${PROJECT_DIR}/${SMITTY_BUILD_DIR}" ]; then
        echo "[build] Cleaning ${SMITTY_BUILD_DIR}..."
        rm -rf "${PROJECT_DIR}/${SMITTY_BUILD_DIR}"
    fi

    echo "[build] Configuring with CMake..."
    mkdir -p "${PROJECT_DIR}/${SMITTY_BUILD_DIR}"
    cmake -S "${PROJECT_DIR}" -B "${PROJECT_DIR}/${SMITTY_BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release

    echo "[build] Compiling..."
    cmake --build "${PROJECT_DIR}/${SMITTY_BUILD_DIR}" --parallel "$(nproc 2>/dev/null || echo 4)"

    echo "[build] Build complete."
fi

# ------------------------------------------------------------------------------
# Run
# ------------------------------------------------------------------------------
if [ "$RUN" = true ]; then
    EXEC_PATH="${PROJECT_DIR}/${SMITTY_BUILD_DIR}/${SMITTY_EXECUTABLE}"

    if [ ! -f "$EXEC_PATH" ]; then
        echo "[error] Executable not found: ${EXEC_PATH}"
        echo "[error] Run without --run-only to build first."
        exit 1
    fi

    echo "[run] Starting ${SMITTY_EXECUTABLE} on port ${SMITTY_APP_PORT}..."
    exec "$EXEC_PATH" \
        --docroot "${SMITTY_DOCROOT}" \
        --http-listen "${SMITTY_APP_HOST}:${SMITTY_APP_PORT}"
fi
