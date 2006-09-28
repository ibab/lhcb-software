#!/bin/bash

. ./setupOnline.sh
${gaudi_exe} ${gaudi_exe_args} -opt=../options/ErrorSrv.opts
#    -msgsvc=${MSGSVC} &
