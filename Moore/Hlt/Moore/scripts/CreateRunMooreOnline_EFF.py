#!/usr/bin/env python

import os, os.path
import sys
import subprocess
import string

#
cmt = subprocess.Popen(['cmt','show','version'],stdout=subprocess.PIPE)
version = string.strip(cmt.communicate()[0],'\n')

env   = 'SetupMoore_'+version+'.sh'
setup = '$LHCBSCRIPTS/' + env
moore = '$MOOREROOT'
if len(sys.argv)>2:
    moore = os.environ['MOOREROOT']
    setup = os.path.join(moore,'job',env)
    print 'generating ' + setup
    ret = subprocess.call(['python',os.path.join(os.environ['LHCBPYTHON'],'SetupProject.py'),'--dev-dir=/home/online','--shell=sh','--output='+setup,'Moore'])

print 'using ' + setup

print 'generating '+ sys.argv[1]
file = open(sys.argv[1],'w+')
file.write( """#!/bin/sh
# pick up the online setup...
source ./setupOnline.sh $*
# pick up 'our' setup... (which defines $MOOREROOT!)
source %(setup)s
# and the options to be used
export USEROPTS=%(moore)s/options/DEFAULT${IS_TAE_RUN}.opts
# and run them!
echo exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
exec -a ${UTGID} ${CLASS1_TASK} -opt=${USEROPTS}
      """%({'setup': setup,'moore':moore}) )

from stat import *
rwxrwxrx = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH 
os.chmod(sys.argv[1],rwxrwxrx )
