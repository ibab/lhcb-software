#!/bin/bash

. ./setupOnline.sh
echo $LD_LIBRARY_PATH
${gaudi_exe}  -main=${ONLINETASKSROOT}/options/MEPInit.opts -opt=${GAUDIONLINEROOT}/options/Daemon.opts &
