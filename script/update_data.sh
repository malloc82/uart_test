#!/bin/bash

UPLOAD_FOLDER=/media/sf_PDD_test_data
rsync -avr MDU_DATA ${UPLOAD_FOLDER}
# rsync -avr SDU_DATA ${UPLOAD_FOLDER}
