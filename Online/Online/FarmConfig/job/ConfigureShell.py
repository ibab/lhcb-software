import os, sys, imp

sep = '/'
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'
checkpoint_local_area = '/dev/shm'
checkpoint_local_area = '/localdisk/checkpoints'

#=========================================================================================
def configureForRunning():
  runinfo = os.environ['RUNINFO']
  dir,fname = os.path.split(runinfo)
  nam,ext = os.path.splitext(fname)
  sys.path.insert(1,dir)
  mod = imp.find_module(nam,None)
  Online=imp.load_module(nam,mod[0],mod[1],mod[2])
  
  OnlVSN = Online.OnlineVersion
  CFG    = Online.HltArchitecture
  DD     = Online.DDDBTag
  COND   = Online.CondDBTag
  App    = Online.HLTType
  Mode   = Online.MooreStartupMode

  if Online.MooreVersion == "":
    checkpoint_reloc = sep+Online.HltArchitecture+sep+Online.OnlineVersion+sep+Online.HLTType
  else:
    Mapping= os.path.basename(Online.ConditionsMapping)
    app_name = os.environ['TASK_TYPE']
    checkpoint_reloc = sep+'Moore'+sep+Online.MooreOnlineVersion+sep+\
                       Online.HLTType+sep+Online.CondDBTag+sep+Online.DDDBTag+sep+\
                       Mapping+sep+app_name

  directory  = checkpoint_dir+checkpoint_reloc
  checkpoint_torrent = None
  ##Mode = 0
  if Mode >= 1:
    try:
      for i in os.listdir(directory):
        idx=i.find('.data')
        if idx>0 and idx==len(i)-5:
          checkpoint_reloc   = checkpoint_reloc+sep+i
          checkpoint_path    = checkpoint_local_area+checkpoint_reloc
          checkpoint_torrent = checkpoint_dir+checkpoint_reloc
          break
    except Exception,X:
      if Mode > 1:
        Mode = 1
        print 'echo "[ERROR] Checkpointing '+checkpoint_reloc+' requested, but no checkpoint present.";'
        print 'exit 1;'

  startup    = "-normal"

  if Mode==0:
    print 'export APP_STARTUP_OPTS=-normal;'
    print 'unset CHECKPOINT_DIR;'
    print 'unset CHECKPOINT_FILE;'
    print 'unset MOORESTARTUP_MODE;'
  elif Mode==1:
    print 'export APP_STARTUP_OPTS=-forking;'
    print 'unset CHECKPOINT_DIR;'
    print 'unset CHECKPOINT_FILE;'
    print 'unset MOORESTARTUP_MODE;'
    # Note: This is the VERY LAST statement. Afterwards the executable MUST run!
    print 'export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;'
  elif Mode==2:
    print 'export APP_STARTUP_OPTS=-restore;'


  print 'export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;'
  print 'export CHECKPOINT_RESTART_OPTIONS=${FARMCONFIGROOT}/options/CheckpointRestart.opts;'
  print 'export PYTHONPATH=/group/online/dataflow/options/'+Online.PartitionName+'/HLT:${PYTHONPATH};'
  
  if Mode > 0:
    print 'export NUM_CORES='+str(os.environ['NBOFSLAVES'])+';'
  if Mode > 1:
    print 'export CHECKPOINT_DIR='+os.path.dirname(checkpoint_path)+';'
    print 'export CHECKPOINT_FILE='+checkpoint_path+';'
    print 'RESTORE_CMD="exec -a ${UTGID} ${CHECKPOINTING_BIN}/bin/restore.exe -p 4 -e -l ${CHECKPOINT_DIR} -i ${CHECKPOINT_FILE}";'
    if not os.environ.has_key('TEST_CHECKPOINT'):
      print 'echo "[INFO] Copy checkpoint:'+checkpoint_path+'";'
      print 'bash '+checkpoint_dir+'/cmds/copy_torrent '+checkpoint_reloc+';'
    print 'if test ! -f "${CHECKPOINT_FILE}";then'
    print '  echo "[FATAL] =====================================================================================";'
    print '  echo "[FATAL] == CHECKPOINT FILE ${CHECKPOINT_FILE} DOES NOT EXIST!";'
    print '  echo "[FATAL] =====================================================================================";'
    print '  exit 1;'
    print 'fi;'

#=========================================================================================
def configureForCheckpoint():
  print 'export APP_STARTUP_OPTS="-checkpoint -auto";'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'
  print 'export MBM_SETUP_OPTIONS='+checkpoint_dir+'/cmds/MBM_setup.opts;'
  print 'export CHECKPOINT_SETUP_OPTIONS='+checkpoint_dir+'/cmds/Checkpoint.opts;'
  print 'echo "[ERROR] =====================================================================================";'
  print 'echo "[ERROR] == Running in checkkpoint PRODUCTION mode....";'
  print 'echo "[ERROR] == File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "[ERROR] == Producing CHECKPOINT file......Please be patient.";'
  print 'echo "[ERROR] == LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so";'
  print 'echo "[ERROR] =====================================================================================";'
  # Note: This is the VERY LAST statement. Afterwards the executable MUST run!
  print 'export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;'

#=========================================================================================
def configureForTest():
  print 'echo "[ERROR] Running in checkpoint TESTING mode....";'
  print 'export APP_STARTUP_OPTS=-restore;'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'
  print 'mkdir -p /dev/shm/checkpoint;'
  print 'RESTORE_CMD="exec -a ${UTGID} restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE}";'
  print 'echo "=====================================================================================";'
  print 'echo "== File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "== Command ${RESTORE_CMD}";'
  print 'echo "== Testing CHECKPOINT file......Please be patient.";'
  print 'echo "=====================================================================================";'
  print 'if test ! -f "${CHECKPOINT_FILE}";then'
  print '  echo "[FATAL] =====================================================================================";'
  print '  echo "[FATAL] == CHECKPOINT FILE ${CHECKPOINT_FILE} DOES NOT EXIST!";'
  print '  echo "[FATAL] =====================================================================================";'
  print '  exit 1;'
  print 'fi;'
  
#=========================================================================================
def doIt():
  try:
    mode = os.environ['MOORESTARTUP_MODE']
    print 'export CHECKPOINTING_BIN=${CHECKPOINTINGROOT}/${CMTCONFIG};'
    print 'export CHECKPOINTING_BIN=${CHECKPOINTINGROOT}/../../InstallArea/${CMTCONFIG};'
    if os.environ.has_key('TEST_CHECKPOINT'):
      configureForTest()
    elif os.environ.has_key('CREATE_CHECKPOINT'):
      configureForCheckpoint()
    else:  # Running in production mode in the HLT
      configureForRunning()
    print 'echo "=====================================================================================";'
    print 'echo "== STARTUP MODE = '+mode+' Opts:${APP_STARTUP_OPTS} Directory:${CHECKPOINT_DIR} ";'
    print 'echo "=====================================================================================";'

  except Exception,X:
    print 'echo "[ERROR] Exception(ConfigureShell): Checkpoint production mode:'+str(X)+'";'
    print 'exit 1;'

#=========================================================================================
if __name__ == '__main__':
  doIt()
