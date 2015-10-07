#!/bin/bash

curr_path=$(readlink -m $(dirname $0))
exec_path=../bin

${curr_path}/../bin/GHD
