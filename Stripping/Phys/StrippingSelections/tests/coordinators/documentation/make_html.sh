#!/bin/sh

export TCK='0x39102111'
export STRIP='stripping21r1p1'

python utils.py ${TCK}
rm -f config.tar.lock
python utils.py ${TCK} ${STRIP}
rm -f config.tar.lock
