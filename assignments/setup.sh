#!/bin/sh
set -eu
ASSIGNMENTS_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PYTHON=${PYTHON:-python3}
command -v cc >/dev/null 2>&1 || { echo 'A C compiler (cc) is required.' >&2; exit 1; }
command -v clang >/dev/null 2>&1 || { echo 'Clang is required for IA-32 assembly.' >&2; exit 1; }
if [ ! -x "$ASSIGNMENTS_DIR/.venv/bin/python" ]; then
    "$PYTHON" -m venv "$ASSIGNMENTS_DIR/.venv"
fi
"$ASSIGNMENTS_DIR/.venv/bin/python" -m pip --disable-pip-version-check --no-cache-dir install -r "$ASSIGNMENTS_DIR/requirements.txt"
ln -sf ../../bin/ee209 "$ASSIGNMENTS_DIR/.venv/bin/ee209"
ln -sf ../../bin/gcc209 "$ASSIGNMENTS_DIR/.venv/bin/gcc209"
printf '\nReady. Activate with: . "%s/.venv/bin/activate"\n' "$ASSIGNMENTS_DIR"
printf 'Run all assignments: "%s/run_all.sh"\n' "$ASSIGNMENTS_DIR"
