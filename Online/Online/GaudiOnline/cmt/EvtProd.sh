#!/bin/bash

. ./setupOnline.sh
export MSGSVC=MessageSvc
${gaudi_exe} ${gaudi_exe_args} -opt=../options/MEPConverter.opts\
    -msgsvc=${MSGSVC} &
