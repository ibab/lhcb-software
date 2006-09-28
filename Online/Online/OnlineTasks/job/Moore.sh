#!/bin/bash

. ./setupOnline.sh
${gaudi_exe} ${gaudi_exe_args} -opt=../options/ReadMBM.opts\
    -msgsvc=${MSGSVC} &
