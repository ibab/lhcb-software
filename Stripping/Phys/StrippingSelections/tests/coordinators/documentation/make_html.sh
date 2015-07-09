#!/bin/sh

export TCK='0x32211202'
export STRIP='stripping23'

python utils.py ${TCK}
rm -f config.tar.lock
python utils.py ${TCK} ${STRIP}
rm -f config.tar.lock
