#!/bin/bash

BIN_DIR=${HOME}/dev/uart_test/bin
DATA_DIR=MDU_DATA/$(date "+%Y-%m-%d_%H%M%S")
mkdir -p ${DATA_DIR}

for i in $(seq 0 8)
do
    ${BIN_DIR}/GSD ${i} ${DATA_DIR}/MDU_${i}.txt
done
