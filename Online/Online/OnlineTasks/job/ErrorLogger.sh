#!/bin/bash

. ./setupOnline.sh

${gaudi_exe2}  -opts=${GAUDIONLINEROOT}/options/ErrorLogger.opts -main=${GAUDIONLINEROOT}/options/Remap.opts &
