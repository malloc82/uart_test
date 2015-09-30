#!/bin/bash

curr_path=$(readlink -m $(dirname $0))
exec_path=../bin
header=${curr_path}/../include/PDD_settings.h

if ! [ -f ${exec_path}/uart ]; then
    echo "Executable $(readlink -m ${exec_path}) not found, quit."
    exit 1
fi

DEVICE=${1^^} # convert to upper case
RANGE=${2}
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
        echo "   $(basename $0) <DEVICE> <RANGE>
        echo ""
        exit 0
        ;;
    *) echo "Unknown device ${DEVICE}. Valid devices are: GRD, SDU, MDU. quit"
       exit 1
       ;;
esac

