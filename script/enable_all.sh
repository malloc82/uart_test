#!/bin/bash

DEVICE=${1^^}
curr_path=$(readlink -m $(dirname $0))
exec_path=../bin

header=${curr_path}/../include/PDD_settings.h
PORT=$(grep -i port ${header} | awk -F"[\"]" '{print $2}')
BAUD=$(grep -i baudrate ${header} | awk '{print $3}' | cut -b 2-)

input_command=87
${exec_path}/uart -d ${PORT} -B ${BAUD} -L 1 -i ${input_command}
