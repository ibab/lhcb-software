#!/usr/bin/env python

import os, os.path
import re
import sys
import subprocess
import string
from os import pathsep, listdir 
from os.path import exists, isdir, realpath, islink
from zipfile import is_zipfile
import fnmatch


def StripPath(path,predicate=None):
    collected = []
    for p in path.split(pathsep):
        rp = realpath(p)
        if exists(rp) :
            if ( isdir(rp) and len(listdir(rp))!=0 ) or is_zipfile(rp) :
                if p not in collected and ( not predicate or predicate(p) ): collected.append(p)
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
version = sys.argv[2]
env   = 'SetupMoore_'+version+'.sh'
setup = '$LHCBSCRIPTS/' + env
moore = '$MOOREROOT'
if len(sys.argv)>3:
    moore = os.environ['MOOREROOT']
    setup = os.path.join(moore,'job',env)
    print 'generating ' + setup
    #ret = subprocess.call(['python',os.path.join(os.environ['LHCBPYTHON'],'SetupProject.py'),'--dev-dir=/home/online/ONLINE:/group/hlt/GAUDI','--shell=sh','--output='+setup,'Moore',version])
    from LbConfiguration.SetupProject import SetupProject
    SetupProject().main(['--dev-dir=/home/online/ONLINE','--use','OnlineSys','--runtime-project','Online'] + sys.argv[4:] + [ '--shell=sh','--output='+setup,'Moore',version])

    print 'removing use of StripPath.sh'
    # remove call to StripPath.sh from generated SetupProject.sh
    f = open(setup, 'r')
    txt = f.read()
    f.close()
    f = open(setup, 'w')
    for input in txt.splitlines():
        line = input.replace('echo ','#echo ')
        line = re.sub('test -f [^ ]*StripPath.sh','false', line)
        m = re.match('export ([^=]+)="([^"]+)"',line)
        if m :
            (name,value) = m.groups()
            if name == 'LD_LIBRARY_PATH':
                # value = StripPath(value,lambda x: not re.search('lcg/external/Grid',x))
                value = StripPath(value,lambda x: ContainsFNmatch(x,['*.so']))
            if name == 'PYTHONPATH' :  # TODO: deal with python.zip files...
                value = StripPath(value)
                #value = StripPath(value,lambda x: ContainsFNmatch(x,['*.py','*.pyc']))
            line = 'export %s="%s"'%(name,value)
        if input!=line : f.write('#ORIG:# %s\n'%input)
        f.write(line+'\n')        
    f.close()



print 'using ' + setup
print 'generating '+ sys.argv[1]
file = open(sys.argv[1],'w+')
file.write( """#!/bin/sh
#!/bin/sh
ulimit -v 3221225472
export PARENT=$1
export PARTNAME=$2
if test -n "$3" ;
   then export NBOFSLAVES=$3
   else export NBOFSLAVES=0
fi
   
# remove the args because they interfere with the cmt scripts
export HOME=/home/$(/usr/bin/whoami)
# pick up 'our' setup... 
source %(setup)s

####echo ${UTGID} Running as $(/usr/bin/whoami) with DIM_DNS_NODE $DIM_DNS_NODE and home $HOME , cmtconfig $CMTCONFIG

export LOGFIFO=/tmp/logGaudi.fifo

# pick up partition specific OnlineEnv module
export PYTHONPATH=/group/online/dataflow/options/${PARTNAME}/HLT:${PYTHONPATH}
# and go for it!
renice 19 -p $$>>/dev/null

# define the task
export gaudi_exe="exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe"

# Setup the environment to checkpoint, restore, run normally etc. by interpreting the vars from OnlineEnv
eval `python -c "import Moore.ConfigureShell as c; c.doIt()"`

export CHECKPOINT_BIN=${CHECKPOINTINGROOT}/${CMTCONFIG};

if test "${APP_STARTUP_OPTS}" = "-normal";         ## RunInfo flag=0
    then
    unset  APP_STARTUP_OPTS;
    unset  CHECKPOINT_DIR;
    unset  CHECKPOINT_FILE;
elif test "${APP_STARTUP_OPTS}" = "-forking";      ## RunInfo flag=1
    then
    unset  APP_STARTUP_OPTS;
    unset  CHECKPOINT_DIR;
    unset  CHECKPOINT_FILE;
    export gaudi_exe="exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/GaudiCheckpoint.exe";
    ####echo   "FORKING: gaudi_exe: ${gaudi_exe}"
    export LD_PRELOAD=${CHECKPOINT_BIN}/libCheckpointing.so;
elif test "${APP_STARTUP_OPTS}" = "-checkpoint";   ## Not handled by RunInfo. Used to create checkpoints.
    then
    export gaudi_exe="exec -a ${UTGID} ${GAUDIONLINEROOT}/${CMTCONFIG}/GaudiCheckpoint.exe";
    export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};
    export LD_PRELOAD=${CHECKPOINT_BIN}/libCheckpointing.so;
    echo "=============================================================================";
    echo "== File:  ${CHECKPOINT_FILE}"
    echo "== Producing CHECKPOINT file......Please be patient.";
    echo "=============================================================================";
    python -c "import Moore.ConfigureShell as c; c.doIt()";
    echo "=============================================================================";
elif test "${APP_STARTUP_OPTS}" = "-restore";      ## RunInfo flag=2
    then
    unset  APP_STARTUP_OPTS;
    unset  LD_PRELOAD;
    if test -f "${CHECKPOINT_FILE}";
	then
        export ONLINETASKS=/group/online/dataflow/templates
        export PARTITIONOPTS=/group/online/dataflow/options/${PARTNAME}/${PARTNAME}_Info.opts
	if test -n "${TEST_CHECKPOINT}";
	    then
	    echo "=============================================================================";
	    echo "== File:  ${CHECKPOINT_FILE}";
	    echo "== Testing CHECKPOINT file......Please be patient.";
	    echo "=============================================================================";
	    python ${MOOREROOT}/python/Moore/ConfigureFromCheckpoint.py;
	    echo exec -a ${UTGID} ${CHECKPOINT_BIN}/restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE};
	    echo "=============================================================================";
	fi;
	python ${MOOREROOT}/python/Moore/ConfigureFromCheckpoint.py | \\
        exec -a ${UTGID} ${CHECKPOINT_BIN}/restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE};
    else
	echo " [FATAL] =============================================================================";
	echo " [FATAL] == File:  ${CHECKPOINT_FILE}";
	echo " [FATAL] == CHECKPOINT FILE DOES NOT EXIST!";
	echo " [FATAL] =============================================================================";
    fi;
    exit 0;
fi;

${gaudi_exe} ${GAUDIONLINEROOT}/${CMTCONFIG}/libGaudiOnline.so \\
    OnlineTask  \\
    -msgsvc=LHCb::FmcMessageSvc \\
    -tasktype=LHCb::Class1Task  \\
    -main=/group/online/dataflow/templates/options/Main.opts \\
    -opt=command="import Moore.runOnline; Moore.runOnline.start("${NBOFSLAVES}")" \\
 ${APP_STARTUP_OPTS};

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
