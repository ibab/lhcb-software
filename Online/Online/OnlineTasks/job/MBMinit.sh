#!/bin/bash

. ./setupOnline.sh
export MSGSVC=MessageSvc

${gaudi_exe} ${gaudi_exe_args} -main=../options/MBMinit.opts\
    -opt=../options/Daemon.opts -msgsvc=${MSGSVC} &
