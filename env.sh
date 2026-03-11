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

# ------------------------------------------------------------------------------
# Database (PostgreSQL)
# ------------------------------------------------------------------------------
export SMITTY_DB_HOST="${SMITTY_DB_HOST:-localhost}"
export SMITTY_DB_PORT="${SMITTY_DB_PORT:-5432}"
export SMITTY_DB_NAME="${SMITTY_DB_NAME:-smitty_services}"
export SMITTY_DB_USER="${SMITTY_DB_USER:-smitty}"
export SMITTY_DB_PASS="${SMITTY_DB_PASS:-smitty_secret}"
export SMITTY_DB_URL="${SMITTY_DB_URL:-postgresql://${SMITTY_DB_USER}:${SMITTY_DB_PASS}@${SMITTY_DB_HOST}:${SMITTY_DB_PORT}/${SMITTY_DB_NAME}}"

# ------------------------------------------------------------------------------
# Docker Configuration
# ------------------------------------------------------------------------------
export SMITTY_DOCKER_NETWORK="${SMITTY_DOCKER_NETWORK:-smitty-network}"
export SMITTY_DOCKER_PREFIX="${SMITTY_DOCKER_PREFIX:-smitty}"

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
    echo "  Database:  ${SMITTY_DB_HOST}:${SMITTY_DB_PORT}/${SMITTY_DB_NAME}"
    echo "  Build Dir: ${SMITTY_BUILD_DIR}"
    echo "===================================="
fi
