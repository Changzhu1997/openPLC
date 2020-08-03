#/bin/sh
set -e

# FQBN="arduino:avr:uno"
FQBN="arduino:sam:arduino_due_x_dbg"

arduino-cli compile --fqbn $FQBN $1
