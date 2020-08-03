#!bin/sh
set -e

# FQBN="arduino:avr:uno"
FQBN="arduino:sam:arduino_due_x_dbg"

arduino-cli compile --fqbn $FQBN $1
arduino-cli upload -p /dev/ttyACM0 --fqbn $FQBN $1
