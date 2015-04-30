#!/bin/bash

if [ $# -eq 0 ]; then
    MATLAB_FILE=MDU_data_plot.m
else
    MATLAB_FILE=$1
fi

echo -n "MDU_data = [" > ${MATLAB_FILE};
for i in $(seq 0 8)
do
    cat MDU_${i}.txt >> ${MATLAB_FILE};
    echo ";"         >> ${MATLAB_FILE};
done

echo "];" >> ${MATLAB_FILE};

# cat ${HOME}/dev/uart_test/script/matlab_data_plot.m >> ${MATLAB_FILE};

echo "figure
for i=1:9
    % fprintf('index = %d\n', i);
    subplot(3, 3, i);
    plot((0:length(MDU_data(i, :))-1), MDU_data(i, :));
    title(sprintf('MDU %d data', i));
end
" >> ${MATLAB_FILE}

