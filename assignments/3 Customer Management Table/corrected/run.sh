#!/bin/sh
set -eu
ASSIGNMENTS_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
exec "$ASSIGNMENTS_DIR/.venv/bin/python" "$ASSIGNMENTS_DIR/tools/run.py" assignment 3 "$@"
