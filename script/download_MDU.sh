#!/bin/bash

BIN_DIR=${HOME}/dev/uart_test/bin

for i in $(seq 0 8)
do
    ${BIN_DIR}/GSD ${i} MDU_${i}.txt
done
