#!/bin/bash

BIN_DIR=${HOME}/dev/uart_test/bin
DATA_DIR=SDU_DATA/$(date "+%Y-%m-%d_%H%M%S")
mkdir -p ${DATA_DIR}

${BIN_DIR}/GSD 9 ${DATA_DIR}/GRID_sensor_9.txt
${BIN_DIR}/GSD b ${DATA_DIR}/GRID_sensor_b.txt

