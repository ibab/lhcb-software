#!/bin/bash
cmd=`python <<EE_EOF 
import os
args='HOST UTGID PATH LD_LIBRARY_PATH $*'.split(' ')
for n in os.environ.keys():
  remove = 1
  for i in args:
    if i == n: 
      remove=0
      break
  if remove: print 'unset '+n+';'
EE_EOF`
eval ${cmd}
