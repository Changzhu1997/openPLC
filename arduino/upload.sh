set -e

case $1 in
    Uno)    FQBN="arduino:avr:uno" ;;
    Due)    FQBN="arduino:sam:arduino_due_x_dbg" ;;
    *)      echo "Unkonwn."; exit 1 ;;
esac

arduino-cli compile --fqbn $FQBN $2
arduino-cli upload -p /dev/ttyACM0 --fqbn $FQBN $2
