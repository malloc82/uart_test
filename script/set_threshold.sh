#!/bin/bash

curr_path=$(readlink -m $(dirname $0))
exec_path=../bin
header=${curr_path}/../include/PDD_settings.h

if ! [ -f ${exec_path}/uart ]; then
    echo "Executable $(readlink -m ${exec_path}) not found, quit."
    exit 1
fi
OPCODE=4
DEVICE=${1^^}  # convert to upper case
RANGE=${2^^}
VOLTAGE_mv=${3}
PORT=$(grep -i port ${header} | awk -F"[\"]" '{print $2}')
BAUD=$(grep -i baudrate ${header} | awk '{print $3}' | cut -b 2-)

echo "exec path = ${exec_path}"
echo "device    = ${DEVICE}"
echo "port      = ${PORT}"
echo "baudrate  = ${BAUD}"

idx=0
case ${DEVICE} in
    GRD) idx=4
         ;;
    SDU) idx=2
         ;;
    MDU) idx=1
         ;;
    HELP)
        echo ""
        echo "Usage:"
        echo "   $(basename $0) <DEVICE> <RANGE> <voltage in mv>"
        echo ""
        echo "Valid device      Valid ranges       Voltage range (mv)"
        echo "   MDU             +/-2.5v            0-2500"
        echo "   SDU             +/-5v              0-5000"
        echo "   GRD             +/-10v             0-10000"
        echo ""
        exit 0
        ;;
    *) echo "Unknown device ${DEVICE}. Valid devices are: GRD, SDU, MDU. quit"
       exit 1
       ;;
esac

range_idx=10
case ${RANGE} in
    +/-2.5V) range_idx=0
             ;;
    +/-5V) range_idx=1
             ;;
    +/-10V) range_idx=2
             ;;
    *) 
        echo "Invalid range. Valid ranges are:"
        echo "   +/-2.5v"
        echo "   +/-5v"
        echo "   +/-10v"
        exit 1
        ;;
esac

threshold=0
case ${range_idx} in
    0) if [ "${VOLTAGE_mv}" -gt "2500" -o "${VOLTAGE_mv}" -lt "0" ]; then
            echo "threshold value is out of range [0, 2500], quit."
            exit 1
        fi
        threshold=$(echo "${VOLTAGE_mv} / (2500.0  / 4096)" | bc -l)
       ;;
    1) if [ "${VOLTAGE_mv}" -gt "5000" -o "${VOLTAGE_mv}" -lt "0" ]; then
            echo "threshold value is out of range [0, 5000], quit."
            exit 1
        fi
       threshold=$(echo "${VOLTAGE_mv} / (5000.0  / 4096)" | bc -l)
       ;;
    2) if [ "${VOLTAGE_mv}" -gt "10000" -o "${VOLTAGE_mv}" -lt "0" ]; then
            echo "threshold value is out of range [0, 10000], quit."
            exit 1
       fi
       threshold=$(echo "${VOLTAGE_mv} / (10000.0 / 4096)" | bc -l)
       ;;
    *) echo "Invalid index : ${range_idx}"
       exit 1
       ;;
esac

# printf "threshold = %.0f\n" ${threshold}
THRESHOLD_HEX=$(printf "%03x0" $(printf "%.0f" ${threshold}))
input_command=${OPCODE}${idx}${THRESHOLD_HEX}

echo "${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}"
# ${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}

