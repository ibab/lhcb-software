import os, sys, imp, traceback

global restore_args
restore_args = ''

sep = '/'
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'
checkpoint_local_area = '/dev/shm'
checkpoint_local_area = '/localdisk/checkpoints'
line = "====================================================================================="

class RunInfo:
  def __init__(self, runinfo):
    dir, fname = os.path.split(runinfo)
    nam, ext = os.path.splitext(fname)
    if dir not in sys.path: sys.path.insert(1,dir)
    mod = imp.find_module(nam,None)
    Online = imp.load_module(nam,mod[0],mod[1],mod[2])
    for i in Online.__dict__:
      if i[:2] != '__':
        setattr(self,i,getattr(Online,i))

class Checkpoint:
  def __init__(self, runinfo):
    self.runinfo = runinfo
    self.task_type = runinfo.HLTType
    self.torrent_file = self._getTorrentFile()
    self.checkpoint_bin = os.environ['CHECKPOINTING_BIN']
    self.restore = self.checkpoint_bin + '/bin/restore.exe'
    if self.torrent_file:
      self.target_path = self.checkpointTargetDir()+sep+self.torrent_file
      self.lib_dir = os.path.dirname(self.target_path)+'/libs'

  def _getTorrentFile(self):
    if self.runinfo.MooreStartupMode > 1:
      try:
        directory = self.checkpointLocationDir() 
        for i in os.listdir(directory):
          idx=i.find('.data')
          if idx>0 and idx==len(i)-5:
            return i
      except Exception,X:
        print X
        return None
    return None

  def checkpointRelativeDir(self):
    ri = self.runinfo
    if ri.MooreOnlineVersion == "" or ri.MooreOnlineVersion == "DEFAULT":
      checkpoint_reloc = sep+ri.HltArchitecture+sep+ri.OnlineVersion+sep+ri.HLTType
    else:
      Mapping = os.path.basename(ri.ConditionsMapping)
      checkpoint_reloc = sep+'Moore'+sep+ri.MooreOnlineVersion+sep+\
          ri.HLTType+sep+ri.CondDBTag+sep+ri.DDDBTag+sep+\
          Mapping+sep+self.task_type
    return checkpoint_reloc

  def checkpointLocationDir(self):
    return checkpoint_dir + self.checkpointRelativeDir()

  def checkpointTargetDir(self):
    return checkpoint_local_area + self.checkpointRelativeDir()

  def torrentFile(self):
    return self.torrent_file

  def output(self, message):
    print message
    return self

  def initProcess(self):
    self.output('unset CHECKPOINT_DIR;') \
        .output('unset CHECKPOINT_FILE;') \
        .output('unset LD_PRELOAD;') \
        .output('export PYTHONPATH=/group/online/dataflow/options/'+self.runinfo.PartitionName+'/HLT:${PYTHONPATH};')
    return self

  def addSlaves(self):
    return self.output('export NUM_CORES=${NBOFSLAVES};')

  def setupNormal(self):
    return self.initProcess().output('export APP_STARTUP_OPTS=-normal;') \
        .output('export CHECKPOINTING_BIN='+self.checkpoint_bin+';') \
        .output('export LD_PRELOAD='+self.checkpoint_bin+'/lib/libCheckpointing.so;') \
        .output('export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;')

  def setupForking(self):
    return self.initProcess().addSlaves() \
        .output('export APP_STARTUP_OPTS=-forking;') \
        .output('export CHECKPOINTING_BIN='+self.checkpoint_bin+';') \
        .output('export LD_PRELOAD='+self.checkpoint_bin+'/lib/libCheckpointing.so;')

  def setupCheckpointFile(self):
    self.output('export CHECKPOINT_DIR='+os.path.dirname(self.target_path)+';') \
        .output('export CHECKPOINT_FILE='+self.target_path+';') \
        .output('export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;') \
        .output('export CHECKPOINT_RESTART_OPTIONS=${FARMCONFIGROOT}/options/CheckpointRestart.opts;')
    return self

  def extractLibs(self):
    self.output(self.restore + ' -p 4 -x -C -i '+self.target_path+' -l '+self.lib_dir+'/;') 
    self.output('if test ! -d "'+self.lib_dir+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT LIBS DIRECTORY '+self.lib_dir+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    return self

  def setupRestore(self):
    ldir = self.checkpointRelativeDir()
    relocate_path = ldir+sep+self.torrent_file
    self.initProcess().setupCheckpointFile() \
        .output('export APP_STARTUP_OPTS=-restore;') \
        .output('RESTORE_CMD="exec -a ${UTGID} '+self.restore+' -p 4 -e -l '+\
                  self.lib_dir+' -i '+self.target_path+'";') \
        .output('if test ! -f "'+self.target_path+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT FILE '+self.target_path+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    return self.addSlaves()  # Normal running

  def copyTorrent(self):
    ldir = self.checkpointRelativeDir()
    relocate_path = self.checkpointRelativeDir()+sep+self.torrent_file
    return self.setupCheckpointFile() \
        .output('echo "[INFO] Copy checkpoint:'+self.target_path+'";') \
        .output('bash '+checkpoint_dir+'/cmds/copy_torrent '+relocate_path+';') \
        .output('if test ! -f "'+self.target_path+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT FILE '+self.target_path+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')

#=========================================================================================
def configureForRunning(runinfo):
  ri = RunInfo(runinfo)
  cp = Checkpoint(ri)

  if ri.MooreStartupMode == 0:
    return cp.setupNormal()
  elif ri.MooreStartupMode == 1:
    return cp.setupForking()
  elif ri.MooreStartupMode == 2:
    torrent_file = cp.torrentFile()
    if torrent_file:
      return cp.setupRestore()
    print 'echo "[ERROR] Checkpointing '+cp.checkpointRelativeDir()+' requested, but no checkpoint present.";'
  else:
    print 'echo "[ERROR] Undefined MOORE startup mode in runinfo. Exiting";'
  print 'exit 1;'

#=========================================================================================
def extractLibraries(runinfo,copy=True,extract=True):
  ri = RunInfo(runinfo)
  cp = Checkpoint(ri)
  if ri.MooreStartupMode == 0 or ri.MooreStartupMode == 1:
    #print 'echo "[DEBUG] No checkpoints to be manipulated for startup mode:'+str(ri.MooreStartupMode)+'.";'
    return 1
  elif ri.MooreStartupMode == 2:
    torrent_file = cp.torrentFile()
    if torrent_file:
      res=None
      if copy: res = cp.copyTorrent()
      if (res and copy and extract) or extract: res = cp.extractLibs()
      if res: return 1
    print 'echo "[ERROR] Checkpointing '+cp.checkpointRelativeDir()+' requested, but no checkpoint present.";'
  else:
    print 'echo "[ERROR] Undefined MOORE startup mode in runinfo. Exiting";'
  print 'exit 1;'

#=========================================================================================
def configureForCheckpoint():
  print 'export APP_STARTUP_OPTS="-checkpoint -auto";'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'
  print 'export MBM_SETUP_OPTIONS='+checkpoint_dir+'/cmds/MBM_setup.opts;'
  print 'export CHECKPOINT_SETUP_OPTIONS='+checkpoint_dir+'/cmds/Checkpoint.opts;'
  print 'echo "[ERROR] '+line+'";'
  print 'echo "[ERROR] == Running in checkkpoint PRODUCTION mode....";'
  print 'echo "[ERROR] == File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "[ERROR] == Producing CHECKPOINT file......Please be patient.";'
  print 'echo "[ERROR] == LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so";'
  print 'echo "[ERROR] '+line+'";'
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
  print 'echo "'+line+'";'
  print 'echo "== File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "== Command ${RESTORE_CMD}";'
  print 'echo "== Testing CHECKPOINT file......Please be patient.";'
  print 'echo "'+line+'";'
  print 'if test ! -f "${CHECKPOINT_FILE}";then'
  print '  echo "[FATAL] '+line+'";'
  print '  echo "[FATAL] == CHECKPOINT FILE ${CHECKPOINT_FILE} DOES NOT EXIST!";'
  print '  echo "[FATAL] '+line+'";'
  print '  exit 1;'
  print 'fi;'
  
#=========================================================================================
def doIt():
  import optparse
  parser = optparse.OptionParser()
  parser.description = 'Helper to start and configure checkpointing actions'
  parser.add_option("-r","--runinfo",dest='runinfo',default=None,
                    help="Set RunInfo file name",metavar="<FILE>")
  parser.add_option("-c","--copy",dest='copy',default=False,action='store_true',
                    help="Flag to copy checkpoint to target",metavar="<boolean>")
  parser.add_option("-l","--libs",dest='libs',default=False,action='store_true',
                    help="Extract libraries from existing checkpoint file",metavar="<directory>")
  parser.add_option("-x","--test",dest='test',default=False,action='store_true',
                    help="Test existing checkpoint file",metavar="<boolean>")
  parser.add_option("-f","--create",dest='create',default=False,action='store_true',
                    help="Create checkpoint file checkpoint file",metavar="<boolean>")
  parser.add_option("-s","--start",dest='start',default=False,action='store_true',
                    help="Start process according to runinfo",metavar="<boolean>")

  try:
    (opts, args) = parser.parse_args()
    if os.environ.has_key('TEST_CHECKPOINT') or opts.test:
      configureForTest()
      return1111
    elif os.environ.has_key('CREATE_CHECKPOINT') or opts.create:
      configureForCheckpoint()
      return
    elif opts.runinfo is None:
      parser.format_help()
      return

    if opts.copy:    extractLibraries(opts.runinfo,True,False)
    if opts.libs:    extractLibraries(opts.runinfo,False,True)
    if opts.start:   configureForRunning(opts.runinfo)

  except Exception,X:
    traceback.print_exc()
    print 'echo "[ERROR] Exception(ConfigureShell): Checkpoint production mode:'+str(X)+'";'
    print 'exit 1;'

#=========================================================================================
if __name__ == '__main__':
  doIt()
