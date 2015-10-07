#!/bin/bash

BIN_DIR=${HOME}/dev/uart_test/bin
DATA_DIR=SDU_DATA/$(date "+%Y-%m-%d_%H%M%S")
mkdir -p ${DATA_DIR}

${BIN_DIR}/GSD a ${DATA_DIR}/SDU.txt

