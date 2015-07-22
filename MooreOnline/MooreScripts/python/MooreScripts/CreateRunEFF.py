
def CreateRunMooreOnline_EFF( output, split='' ) :#setup, split='' ) :
    from os import makedirs,chmod
    from os.path import exists, dirname

    #print '# INFO: using ' + setup
    print '# INFO: generating '+ output
    target_dir = dirname( output )
    if not exists( target_dir ) : makedirs( target_dir )
    file = open(output,'w+')
    file.write( """#!/bin/sh
ulimit -v 3221225472
export UTGID;
export DIM_DNS_NODE=$1
export PARTITION_NAME=$2
export NBOFSLAVES=${3:-0}
# remove the args because they interfere with the cmt scripts
export HOME=/home/$(/usr/bin/whoami)
#
. /group/hlt/MOORE/${MOOREONLINE_VERSION}/InstallArea/${CMTCONFIG}/setupMoore.sh;
#
# Enable next line for Debug printout
#python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s;
eval `python ${FARMCONFIGROOT}/job/ConfigureCheckpoint.py -r ${RUNINFO} -s %(WhichMoore)s`;
renice -n %(Priority)s -p $$>>/dev/null
#
if test "${MOORESTARTUP_MODE}" = "RESTORE";      ## RunInfo flag=2
    then
    # Enable next line for Debug printout
    #python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py;
    #echo "RESORE command: ${RESTORE_CMD}";
    eval "python ${FARMCONFIGROOT}/job/ConfigureFromCheckpoint.py | ${RESTORE_CMD}";
else
    exec -a ${UTGID} GaudiCheckpoint.exe libGaudiOnline.so OnlineTask \\
	-msgsvc=LHCb::FmcMessageSvc \\
        -tasktype=LHCb::Class1Task \\
	-main=/group/online/dataflow/templates/options/Main.opts \\
	-opt=command="import MooreScripts.runOnline; MooreScripts.runOnline.start(NbOfSlaves = "${NBOFSLAVES}", Split = '%(split)s', %(checkOdin)s WriterRequires = %(WriterRequires)s  )" \\
	${APP_STARTUP_OPTS};
fi;
    """%({'split': split,
          'Priority' : { 'Hlt1' : '10', 'Hlt2' : '19' },
          'WhichMoore' : { 'Hlt1' : '', 'Hlt2' : '-t Moore2' },
          'checkOdin': { 'Hlt1' : '' , 'Hlt2' : 'CheckOdin = False, ' }.get( split, "" ),
          'WriterRequires': { 'Hlt1' : "[ 'Hlt1' ]" , 'Hlt2' : "[ 'Hlt2' ]" }.get(split, "['HltDecisionSequence']")}))

    from stat import S_IRUSR, S_IRGRP, S_IROTH, S_IWUSR, S_IWGRP, S_IXUSR, S_IXGRP, S_IXOTH
    import os
    orig = os.stat(output)[0]
    rwxrwxrx = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH
    if orig|rwxrwxrx != orig :
        print '%s has permissions %d -- want %d instead' % (output,orig,orig|rwxrwxrx)
        try :
            chmod(output,rwxrwxrx)
            print 'updated permission of %s'%(output)
        except :
            print 'WARNING: could not update permissions of %s -- please make sure it is executable!' % output
