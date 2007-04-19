#!/bin/bash

. ./setupOnline.sh

${gaudi_exe}  -opts=${GAUDIONLINEROOT}/options/DiskWR.opts -main=${GAUDIONLINEROOT}/options/Main.opts -loop &

