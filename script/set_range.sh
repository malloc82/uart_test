#!/bin/bash

curr_path=$(readlink -m $(dirname $0))
exec_path=../bin
header=${curr_path}/../include/PDD_settings.h

if ! [ -f ${exec_path}/uart ]; then
    echo "Executable $(readlink -m ${exec_path}) not found, quit."
    exit 1
fi

OPCODE=5
DEVICE=${1^^} # convert to upper case
RANGE=${2^^}
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
        echo "   $(basename $0) <DEVICE> <RANGE>"
        echo ""
        exit 0
        ;;
    *) echo "Unknown device ${DEVICE}. Valid devices are: GRD, SDU, MDU. quit"
       exit 1
       ;;
esac

RANGE_VAL=EE
case ${RANGE} in
    +/-2.5V) 
        RANGE_VAL=00
        ;;
    +/-5V) 
        RANGE_VAL=01
        ;;
    +/-10V) 
        RANGE_VAL=02
        ;;
    *)
        echo ""
        echo "Invalid range. Valid ranges are:"
        echo "   +/-2.5v"
        echo "   +/-5v"
        echo "   +/-10v"
        echo ""
        exit 1
        ;;
esac

input_command=${OPCODE}${idx}${RANGE_VAL}
echo "${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}"
${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}
