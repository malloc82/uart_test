#!/bin/bash

curr_path=$(readlink -m $(dirname $0))
exec_path=../bin
header=${curr_path}/../include/PDD_settings.h

if ! [ -f ${exec_path}/uart ]; then
    echo "Executable $(readlink -m ${exec_path}) not found, quit."
    exit 1
fi

DEVICE=${1^^}
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
    *) echo "Unknown device ${DEVICE}. Valid devices are: GRD, SDU, MDU. quit"
       exit 1
       ;;
esac

if [ "$2" -gt "4095" -o "$2" -lt "0" ]; then
    echo "range value is out of range [0, 4095], quit."
    exit 1
fi

input_command=4${idx}$(printf "%03s" $(echo "obase=16; $2" | bc))0
${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}
