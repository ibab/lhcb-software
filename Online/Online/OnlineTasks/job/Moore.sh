#!/bin/bash

. ./setupOnline.sh
${gaudi_exe}  -opt=${GAUDIONLINEROOT}/options/ReadMBM.opts -main=${GAUDIONLINEROOT}/options/Main.opts &

