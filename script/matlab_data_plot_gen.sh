#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Need to specify a filename ... quit";
    exit 1
fi

if ! [ -f ${1} ]; then
    echo "File does not exist ... quit.";
    exit 2
fi

DATA_FILE=${1}

if [ $# -ge 2 ]; then
    MATLAB_FILE=${2}
else
    MATLAB_FILE=data_plot.m
fi

if [ $# -ge 3 ]; then
    TITLE=${3}
fi

echo ${MATLAB_FILE}
echo ${DATA_FILE}

echo -n "data = [" > ${MATLAB_FILE}
cat ${DATA_FILE}  >> ${MATLAB_FILE}
echo "];"         >> ${MATLAB_FILE}

echo "figure, plot(data);" >> ${MATLAB_FILE}
if [ -n "${TITLE}" ]; then 
    echo "title('${TITLE}')" >> ${MATLAB_FILE}
fi

