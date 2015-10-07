#!/bin/bash

DEVICE=${1^^}
curr_path=$(readlink -m $(dirname $0))
exec_path=../bin

header=${curr_path}/../include/PDD_settings.h
PORT=$(grep -i port ${header} | awk -F"[\"]" '{print $2}')
BAUD=$(grep -i baudrate ${header} | awk '{print $3}' | cut -b 2-)

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

input_command=F${idx}
${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}
