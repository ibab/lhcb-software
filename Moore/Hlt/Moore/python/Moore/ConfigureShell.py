import os

sep = '/'
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'

#=========================================================================================
def configureForRunning():
  import OnlineEnvBase as Online
  CFG   = Online.HLTType
  DD    = Online.DDDBTag
  COND  = Online.CondDBTag
  App   = Online.MooreVersion
  Mode  = Online.MooreStartupMode

  directory  = checkpoint_dir+sep+'Moore'+sep+App+sep+CFG+sep+COND+sep+DD
  checkpoint = directory+sep+'Checkpoint.data'
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

  if Mode != 0:
    print 'export CHECKPOINT_DIR='+directory+';'
    print 'export CHECKPOINT_FILE='+checkpoint+';'
    
  # Number of CPUs online:
  # os.sysconf('SC_NPROCESSORS_ONLN')
  #
  # Number of processors known to operating system:
  # os.sysconf('SC_NPROCESSORS_CONF')
  print 'export NUM_CORES='+str(os.sysconf('SC_NPROCESSORS_ONLN'))+';'
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
