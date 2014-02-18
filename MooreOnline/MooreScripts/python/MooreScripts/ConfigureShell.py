import os

sep = '/'
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'
shared_memory = '/dev/shm'

def copyNewFile(checkpoint_loc,checkpoint_file):
  checkpoint = checkpoint_loc+sep+checkpoint_file
  print 'echo "  [INFO] Copy new checkpoint:'+checkpoint+'.gz to RAM.";'
  print checkpoint_dir+'/cmds/copy_torrent '+checkpoint+';'
  print 'export CHECKPOINT_FILE='+shared_memory+sep+checkpoint_file+';'
  print 'export CHECKPOINT_TORRENT='+checkpoint_file+'.gz.torrent;'

#=========================================================================================
def configureForRunning():
  import OnlineEnvBase as Online
  CFG   = Online.HLTType
  DD    = Online.DDDBTag
  COND  = Online.CondDBTag
  App   = Online.MooreVersion
  Mode  = Online.MooreStartupMode

  directory  = checkpoint_dir+sep+'Moore'+sep+App+sep+CFG+sep+COND+sep+DD
  checkpoint_loc  = None
  checkpoint_file = None
  if Mode > 1:
    try:
      for i in os.listdir(directory):
        idx=i.find('.data')
        if idx>0 and idx==len(i)-5:
          checkpoint_loc  = directory
          checkpoint_file = i
          break
    except Exception,X:
      ###print 'echo "[ERROR] Checkpointing requested:'+str(X)+'";'
      if Mode > 1:
        Mode = 1
        print 'echo "[ERROR] Checkpointing requested, but no checkpoint present.";'

  startup    = "-normal"

  if Mode==0:
    startup="-normal"
  elif Mode==1:
    startup="-forking"
  elif Mode==2:
    startup="-restore"
  elif Mode==100:
    startup="-checkpoint"

  if os.environ.has_key('CREATE_CHECKPOINT'):
    startup="-checkpoint"
    Mode = 100
  elif os.environ.has_key('TEST_CHECKPOINT'):
    print 'echo "[ERROR] Startup mode:TEST_CHECKPOINT='+str(os.environ['TEST_CHECKPOINT'])+' - '+str(Mode)+'";'
    startup="-restore"

  print 'echo "[INFO] Startup mode:'+str(startup)+' - '+str(Mode)+'";'
  if Mode==2 and not os.environ.has_key('TEST_CHECKPOINT'):
    md5 = ''
    shm_md5 = ''
    try:
      md5 = open(checkpoint_loc+sep+checkpoint_file+".md5","r").read()
      shm_md5 = open(''+shared_memory+sep+checkpoint_file+'.md5').read()
      if shm_md5 == md5:
        print 'echo "  [INFO] Checkpoint file is up to date";'
      else:
        copyNewFile(checkpoint_loc,checkpoint_file)
    except Exception,X:
      copyNewFile(checkpoint_loc,checkpoint_file)
    print 'export CHECKPOINT_DIR=/dev/shm;'
    print 'export CHECKPOINT_FILE='+shared_memory+sep+checkpoint_file+';'
  elif Mode > 1:
    print 'export CHECKPOINT_DIR='+checkpoint_loc+';'
    print 'export CHECKPOINT_FILE='+checkpoint_loc+sep+checkpoint_file+';'

  print 'export NUM_CORES='+str(os.environ['NBOFSLAVES'])+';'
  print 'export APP_STARTUP_OPTS='+startup+';'

#=========================================================================================
def configureForCheckpoint():
  print 'echo "Running in checkkpoint PRODUCTION mode....";'
  print 'export APP_STARTUP_OPTS=-checkpoint;'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'

#=========================================================================================
def configureForTest():
  print 'echo "Running in checkkpoint TESTING mode....";'
  print 'export APP_STARTUP_OPTS=-restore;'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'

#=========================================================================================
def doIt():
  try:
    if os.environ.has_key('TEST_CHECKPOINT'):
      configureForTest()
    elif os.environ.has_key('CREATE_CHECKPOINT'):
      configureForCheckpoint()
    else:
      # Running in production mode in the HLT
      configureForRunning()

  except Exception,X:
    print 'echo "Checkpoint production mode:'+str(X)+'";'
    configureForCheckpoint()

#=========================================================================================
if __name__ == '__main__':
  doIt()
