#!/bin/bash

. ./setupOnline.sh
${gaudi_exe}  -main=${GAUDIONLINEROOT}/options/ErrorSrv.opts -opt=${GAUDIONLINEROOT}/options/Daemon.opts &

