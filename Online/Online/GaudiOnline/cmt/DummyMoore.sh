#!/bin/bash

. ./setupOnline.sh
export MSGSVC=MessageSvc
${gaudi_exe} ${gaudi_exe_args} -opt=../options/ReadMBM.opts\
    -msgsvc=${MSGSVC} &
