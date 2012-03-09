import os

sep = '/'
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'
shared_memory = '/dev/shm'

def copyNewFile(checkpoint_loc,checkpoint_file):
  checkpoint = checkpoint_loc+sep+checkpoint_file
  print 'echo "  [INFO] Copy new checkpoint:'+checkpoint+'.gz to RAM.";'
  print 'rm -f '+shared_memory+sep+'Checkpoint.20*;'
  print checkpoint_dir+'/cmds/get_torrent '+checkpoint+'.gz.torrent;'
  print 'export CHECKPOINT_FILE='+shared_memory+sep+checkpoint_file+';'
  print '/bin/gunzip '+shared_memory+sep+checkpoint_file+'.gz;'
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
  for i in os.listdir(directory):
    idx=i.find('.data')
    if idx>0 and idx==len(i)-5:
      checkpoint_loc  = directory
      checkpoint_file = i
      break

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
  elif os.environ.has_key('TEST_CHECKPOINT'):
    startup="-restore"

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
  elif Mode != 0:
    print 'export CHECKPOINT_DIR='+checkpoint_loc+';'
    print 'export CHECKPOINT_FILE='+checkpoint_loc+sep+checkpoint_file+';'

  # Number of CPUs online:
  # os.sysconf('SC_NPROCESSORS_ONLN')
  #
  # Number of processors known to operating system:
  # os.sysconf('SC_NPROCESSORS_CONF')
  ##print 'export NUM_CORES='+str(os.sysconf('SC_NPROCESSORS_ONLN'))+';'
  print 'export NUM_CORES='+str(os.environ['NBOFSLAVES'])+';'
  print 'export APP_STARTUP_OPTS='+startup+';'

#=========================================================================================
def configureForCheckpoint():
  print 'echo "Running in checkkpoint production mode....";'
  print 'export APP_STARTUP_OPTS=-checkpoint;'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'

#=========================================================================================
def doIt():
  try:
    import OnlineEnvBase as Online
    if os.environ.has_key('CREATE_CHECKPOINT'):
      configureForCheckpoint()
    else:
      configureForRunning()

  except ImportError,X:
    configureForCheckpoint()

#=========================================================================================
if __name__ == '__main__':
  doIt()
