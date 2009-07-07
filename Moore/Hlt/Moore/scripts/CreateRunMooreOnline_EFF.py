#!/usr/bin/env python

import os, os.path
import re
import sys
import subprocess
import string
from os import pathsep, listdir 
from os.path import exists, isdir, realpath, islink
import fnmatch


def StripPath(path,predicate=None):
    collected = []
    for p in path.split(pathsep):
        rp = realpath(p)
        if exists(rp) and isdir(rp):
            if len(listdir(rp))!=0 and p not in collected and ( not predicate or predicate(p) ): 
                collected.append(p)
    return pathsep.join(collected)
#
def ContainsFNmatch(dir,matches) :
    for root,dirs,files in os.walk( dir ) :
        for m in matches :
            if fnmatch.filter(files,m)  : return True
        dirs.extend([ f for f in files if islink(f) and isdir(f)])
    return False


# link all files found in path to 'target' dir, checking
# that entry does not already exist
# return path to target
def AmalgatePath(path,target):
    return None
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
    #ret = subprocess.call(['python',os.path.join(os.environ['LHCBPYTHON'],'SetupProject.py'),'--dev-dir=/home/online/ONLINE:/group/hlt/GAUDI','--shell=sh','--output='+setup,'Moore',version])
    from LbConfiguration.SetupProject import SetupProject
    SetupProject().main(['--dev-dir=/home/online/ONLINE:/group/hlt/GAUDI','--shell=sh','--output='+setup,'Moore',version])

    print 'removing use of StripPath.sh'
    # remove call to StripPath.sh from generated SetupProject.sh
    f = open(setup, 'r')
    txt = f.read()
    f.close()
    print txt
    f = open(setup, 'w')
    for input in txt.splitlines():
        line = input
        line = re.sub('test -f [^ ]*StripPath.sh','false', line)
        m = re.match('export ([^=]+)="([^"]+)"',line)
        if m :
            (name,value) = m.groups()
            if name == 'LD_LIBRARY_PATH':
                value = StripPath(value,lambda x: not re.search('lcg/external/Grid',x))
                value = StripPath(value,lambda x: ContainsFNmatch(x,['*.so']))
            if name == 'PYTHONPATH' : 
                value = StripPath(value,lambda x: not re.search('Hlt/HltPython/python',x))
                value = StripPath(value,lambda x: ContainsFNmatch(x,['*.py','*.pyc']))
            line = 'export %s="%s"'%(name,value)
        if input!=line : f.write('#ORIG:# %s\n'%input)
        f.write(line+'\n')        
    f.close()



print 'using ' + setup

print 'generating '+ sys.argv[1]
file = open(sys.argv[1],'w+')
file.write( """#!/bin/sh
# pick up the online setup...
source ./setupOnline.sh $*
# pick up 'our' setup... 
source %(setup)s
# pick up partition specific OnlineEnv module
export PYTHONPATH=/group/online/dataflow/options/${PARTNAME}/HLT:${PYTHONPATH}
# and go for it!
exec -a ${UTGID} ${CLASS1_TASK} -opt=command="import Moore.runOnline; Moore.runOnline.start()"

"""%({'setup': setup,'moore':moore}) )


from stat import *
orig = os.stat(sys.argv[1])[0]
rwxrwxrx = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH 
if orig|rwxrwxrx != orig :
    print '%s has permissions %d -- want %d instead' % (sys.argv[1],orig,orig|rwxrwxrx)
    try :
        os.chmod(sys.argv[1],rwxrwxrx)
        print 'updated permission of %s'%(sys.argv[1])
    except :
        print 'WARNING: could not update permissions of %s -- please make sure it is executable!' % sys.argv[1]
