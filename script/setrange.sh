#!/bin/bash

curr_path=$(readlink -m $(dirname $0))
exec_path=../bin/
header=${curr_path}/../include/PDD_settings.h

if ! [ -f ${exec_path}/uart ]; then
    echo "Executable $(readlink -m ${exec_path}) not found, quit."
    exit 1
fi

echo $curr_path
echo $exec_path

DEVICE=${1^^}
PORT=$(grep -i port ${header} | awk -F"[\"]" '{print $2}')
BAUD=$(grep -i baudrate ${header} | awk '{print $3}' | cut -b 2-)
echo "device   = ${DEVICE}"
echo "port     = ${PORT}"
echo "baudrate = ${BAUD}"

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

${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i "4${idx}$(echo "obase=16 $2" | bc)0"

