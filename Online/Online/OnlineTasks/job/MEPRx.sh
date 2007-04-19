#!/bin/bash

. ./setupOnline.sh

${gaudi_exe}  -opt=${ONLINETASKSROOT}/options/MEPRxSvc.opts -main=${GAUDIONLINEROOT}/options/Main.opts &
