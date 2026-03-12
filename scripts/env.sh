#!/usr/bin/env bash
# ==============================================================================
# Smitty Services - Environment Configuration
# ==============================================================================
# Source this file to configure the runtime environment for all tiers.
# Override any variable before sourcing, or pass as environment variables.
#
# Usage:
#   source env.sh                        # Use all defaults
#   SMITTY_APP_PORT=9090 source env.sh   # Override frontend port
# ==============================================================================

# ------------------------------------------------------------------------------
# Frontend (C++ Wt Application)
# ------------------------------------------------------------------------------
export SMITTY_APP_HOST="${SMITTY_APP_HOST:-0.0.0.0}"
export SMITTY_APP_PORT="${SMITTY_APP_PORT:-8080}"
export SMITTY_DOCROOT="${SMITTY_DOCROOT:-./resources}"

# ------------------------------------------------------------------------------
# Backend API (ApiLogicServer)
# ------------------------------------------------------------------------------
export SMITTY_API_HOST="${SMITTY_API_HOST:-localhost}"
export SMITTY_API_PORT="${SMITTY_API_PORT:-5656}"
export SMITTY_API_ENDPOINT="${SMITTY_API_ENDPOINT:-http://${SMITTY_API_HOST}:${SMITTY_API_PORT}/api}"
export ALS_API_URL="${ALS_API_URL:-${SMITTY_API_ENDPOINT}}"

# ------------------------------------------------------------------------------
# Build Configuration
# ------------------------------------------------------------------------------
export SMITTY_BUILD_DIR="${SMITTY_BUILD_DIR:-build}"
export SMITTY_EXECUTABLE="${SMITTY_EXECUTABLE:-smitty_services}"

# ------------------------------------------------------------------------------
# Print active configuration
# ------------------------------------------------------------------------------
if [ "${SMITTY_VERBOSE:-0}" = "1" ]; then
    echo "=== Smitty Services Environment ==="
    echo "  Frontend:  http://${SMITTY_APP_HOST}:${SMITTY_APP_PORT}"
    echo "  Backend:   ${SMITTY_API_ENDPOINT}"
    echo "  Build Dir: ${SMITTY_BUILD_DIR}"
    echo "===================================="
fi
