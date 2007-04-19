#!/bin/bash

. ./setupOnline.sh

${gaudi_exe}  -main=${GAUDIONLINEROOT}/options/MBMinit.opts -opt=${GAUDIONLINEROOT}/options/Daemon.opts &
