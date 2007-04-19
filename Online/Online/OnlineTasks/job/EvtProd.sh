#!/bin/bash

. ./setupOnline.sh

${gaudi_exe}  -opts=${GAUDIONLINEROOT}/options/MEPConverter.opts -main=${GAUDIONLINEROOT}/options/Main.opts &

