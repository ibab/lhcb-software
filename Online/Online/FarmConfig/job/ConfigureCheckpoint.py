import os, sys, imp, traceback

global restore_args
restore_args = ''

sep = '/'

checkpoint_zip = True
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'
checkpoint_expand_area = '/dev/shm'
checkpoint_local_area = '/localdisk/checkpoints'
if os.environ.has_key('LOCAL_CHECKPOINT_DIR'):
  checkpoint_local_area = os.environ['LOCAL_CHECKPOINT_DIR']

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
    # This is sort of a hack for producing checkpoints for Brunel:
    # If the brunel version cannot be obtained from the run-info, 
    # we simply assume the 'default' version called 'OnlineBrunel'
    if not hasattr(self,'OnlineBrunelVersion'):
      self.OnlineBrunelVersion = 'OnlineBrunel'

class Checkpoint:
  def __init__(self, runinfo, config_brunel):
    global checkpoint_local_area
    self.runinfo = runinfo
    self.config_brunel = config_brunel
    
    ##print 'echo "[INFO] '+str(isinstance(self.config_brunel,str))+'";'
    if isinstance(self.config_brunel,str):
      self.task_type = self.config_brunel
    elif self.config_brunel:
      self.task_type = 'OnlineBrunel' # FixME: runinfo.HLTType
    elif runinfo.MooreOnlineVersion == "" or runinfo.MooreOnlineVersion == "DEFAULT":
      self.task_type = 'PassThrough'
    else:
      self.task_type = 'Moore1' # FixME: runinfo.HLTType

    self.torrent_file   = self._getTorrentFile()
    self.checkpoint_bin = os.environ['CHECKPOINTING_BIN']
    self.restore        = self.checkpoint_bin + '/bin/restore.exe'
    if self.torrent_file:
      if checkpoint_zip:
        self.target_path = checkpoint_expand_area+sep+self.task_type+sep+"Checkpoint.data"
      else:
        self.target_path = checkpoint_local_area+self.checkpointRelativeDir()+sep+self.torrent_file
      self.lib_dir = os.path.dirname(self.target_path)+'/lib'

  def _getTorrentFile(self):
    try:
      if (self.config_brunel and self.runinfo.RecoStartupMode > 1) or self.runinfo.MooreStartupMode > 1:
        directory = checkpoint_dir + self.checkpointRelativeDir()
        for i in os.listdir(directory):
          idx=i.find('.data')
          if idx>0 and idx==len(i)-5:
            return i
    except Exception,X:
      print 'echo "[ERROR] '+str(X)+'";'
    return None

  def checkpointRelativeDir(self):
    ri = self.runinfo
    if self.config_brunel:
      Mapping = os.path.basename(ri.ConditionsMapping)
      checkpoint_reloc = sep+self.task_type+sep+ri.OnlineBrunelVersion+sep+\
          ri.HLTType+sep+ri.CondDBTag+sep+ri.DDDBTag+sep+Mapping+\
          '' #sep+self.task_type
    elif ri.MooreOnlineVersion == "" or ri.MooreOnlineVersion == "DEFAULT":
      checkpoint_reloc = sep+ri.HltArchitecture+sep+ri.OnlineVersion+sep+ri.HLTType
    else:
      Mapping = os.path.basename(ri.ConditionsMapping)
      checkpoint_reloc = sep+'Moore'+sep+ri.MooreOnlineVersion+sep+\
          ri.HLTType+sep+ri.CondDBTag+sep+ri.DDDBTag+sep+\
          Mapping+sep+self.task_type
    return checkpoint_reloc

  def torrentFile(self):
    return self.torrent_file

  def output(self, message):
    print message
    ## print 'echo [INFO] ',message
    return self

  def initProcess(self):
    self.output('unset CHECKPOINT_DIR;') \
        .output('unset CHECKPOINT_FILE;') \
        .output('unset LD_PRELOAD;')
    if self.config_brunel:
      self.output('export PYTHONPATH=/group/online/dataflow/options/'+self.runinfo.PartitionName+'/RECONSTRUCTION:${PYTHONPATH};')
    else:
      self.output('export PYTHONPATH=/group/online/dataflow/options/'+self.runinfo.PartitionName+'/HLT:${PYTHONPATH};')
    return self

  def addSlaves(self):
    return self.output('export NUM_CORES=${NBOFSLAVES};')

  def setupNormal(self):
    return self.initProcess().output('export APP_STARTUP_OPTS=-normal;') \
        .output('export CHECKPOINTING_BIN='+self.checkpoint_bin+';') \
        .output('export LD_PRELOAD='+self.checkpoint_bin+'/lib/libCheckpointing.so;') \
        .output('export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;')

  #=======================================================================================
  def setupForking(self):
    return self.initProcess().addSlaves() \
        .output('export APP_STARTUP_OPTS=-forking;') \
        .output('export CHECKPOINTING_BIN='+self.checkpoint_bin+';') \
        .output('export LD_PRELOAD='+self.checkpoint_bin+'/lib/libCheckpointing.so;')

  #=======================================================================================
  def setupCheckpointFile(self):
    #self.output('export CHECKPOINT_DIR='+os.path.dirname(self.target_path)+';') \
    #    .output('export CHECKPOINT_FILE='+self.target_path+';') 
    self.output('export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;') \
        .output('export CHECKPOINT_RESTART_OPTIONS=${FARMCONFIGROOT}/options/CheckpointRestart.opts;')
    return self

  #=======================================================================================
  def extractLibs(self):
    self.output(self.restore + ' -p 4 -x -C -i '+self.target_path+' -l '+self.lib_dir+'/;') 
    self.output('if test ! -d "'+self.lib_dir+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT LIBS DIRECTORY '+self.lib_dir+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    return self

  #=======================================================================================
  def setupRestoreEnv(self):
    return self.initProcess().addSlaves() \
        .output('export APP_STARTUP_OPTS=-restore;') \
        .output('export CHECKPOINTING_BIN='+self.checkpoint_bin+';')

  #=======================================================================================
  def setupRestore(self, extract=True):
    ldir = self.checkpointRelativeDir()
    relocate_path = ldir+sep+self.torrent_file
    if extract: e_opt = '-x'
    else:       e_opt = '-X'
    self.initProcess().setupCheckpointFile() \
        .output('echo "[DEBUG] Setup restore process for %s";'%(self.task_type,)) \
        .output('export APP_STARTUP_OPTS=-restore;') \
        .output('RESTORE_CMD="exec -a ${UTGID} '+self.restore+' -p 4 -e '+e_opt+' -l '+\
                  self.lib_dir+' -i '+self.target_path+'";') \
        .output('if test ! -f "'+self.target_path+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT FILE '+self.target_path+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    return self.addSlaves()  # Normal running

  #=======================================================================================
  def copyTorrent(self):
    ldir = self.checkpointRelativeDir()
    relocate_path = self.checkpointRelativeDir()+sep+self.torrent_file
    expand_path   = self.target_path
    self.setupCheckpointFile() \
        .output('echo "[INFO] Copy checkpoint:'+relocate_path+'";')
    if self.config_brunel:
      self.output('bash '+checkpoint_dir+'/cmds/copy_data '+relocate_path+' '+self.task_type+';')
    else:
      self.output('bash '+checkpoint_dir+'/cmds/copy_torrent.zipped '+relocate_path+' '+self.task_type+';')
    self.output('if test ! -f "'+self.target_path+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT FILE '+self.target_path+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    return self

#=========================================================================================
def torrent_restore(cp, extract=True):
  torrent_file = cp.torrentFile()
  if torrent_file:
    return cp.setupRestore(extract)
  print 'echo "[ERROR] Checkpointing '+cp.checkpointRelativeDir()+' requested, but no checkpoint present.";'
  print 'exit 1;'
  return None

#=========================================================================================
def torrent_copy(cp,copy,extract):
  torrent_file = cp.torrentFile()
  if torrent_file:
    res=None
    if copy: res = cp.copyTorrent()
    if (res and copy and extract) or extract: res = cp.extractLibs()
    if res: return 1
  print 'echo "[ERROR] Checkpointing '+cp.checkpointRelativeDir()+' requested, but no checkpoint present.";'
  print 'exit 1;'
  return None

#=========================================================================================
def setupEnviron(runinfo, config_brunel):
  ri = RunInfo(runinfo)
  ##print 'echo "[ERROR] Configure CHECKPOINT environment. Reco:%s/%d Moore:%d";'%(str(config_brunel),ri.RecoStartupMode,ri.MooreStartupMode,)
  cp = Checkpoint(ri,config_brunel)

  if   config_brunel and ri.RecoStartupMode == 0:
    return cp.setupNormal()
  elif config_brunel and ri.RecoStartupMode == 1:
    return cp.setupForking()
  elif config_brunel and ri.RecoStartupMode == 2:
    return cp.setupRestoreEnv()
  elif ri.MooreStartupMode == 0:
    return cp.setupNormal()
  elif ri.MooreStartupMode == 1:
    return cp.setupForking()
  elif ri.MooreStartupMode == 2:
    return cp.setupRestoreEnv()
  else:
    print 'echo "[ERROR] Undefined CHECKPOINT startup mode in runinfo. Exiting";'
  print 'exit 1;'
  return None

#=========================================================================================
def configureForRunning(runinfo, config_brunel, extract=True):
  ri = RunInfo(runinfo)
  ##print 'echo "[ERROR] Configure CHECKPOINT for running. Reco:%s/%d Moore:%d";'%(str(config_brunel),ri.RecoStartupMode,ri.MooreStartupMode,)
  cp = Checkpoint(ri,config_brunel)

  if   config_brunel and ri.RecoStartupMode == 0:
    return cp.setupNormal()
  elif config_brunel and ri.RecoStartupMode == 1:
    return cp.setupForking()
  elif config_brunel and ri.RecoStartupMode == 2:
    return torrent_restore(cp, extract)
  elif ri.MooreStartupMode == 0:
    return cp.setupNormal()
  elif ri.MooreStartupMode == 1:
    return cp.setupForking()
  elif ri.MooreStartupMode == 2:
    return torrent_restore(cp, extract)
  else:
    print 'echo "[ERROR] Undefined CHECKPOINT startup mode in runinfo. Exiting";'
  print 'exit 1;'
  return None

#=========================================================================================
def extractLibraries(runinfo,copy,extract,config_brunel):
  ri = RunInfo(runinfo)
  cp = Checkpoint(ri,config_brunel)
  if config_brunel and (ri.RecoStartupMode == 0 or ri.RecoStartupMode == 1):
    print 'echo "[DEBUG] No checkpoints to be manipulated for BRUNEL startup mode:'+str(ri.RecoStartupMode)+'.";'
    return 1
  elif config_brunel and ri.RecoStartupMode == 2:
    return torrent_copy(cp,copy,extract)
  elif ri.MooreStartupMode == 0 or ri.MooreStartupMode == 1:
    #print 'echo "[DEBUG] No checkpoints to be manipulated for MOORE startup mode:'+str(ri.MooreStartupMode)+'.";'
    return 1
  elif ri.MooreStartupMode == 2:
    return torrent_copy(cp,copy,extract)
  else:
    print 'echo "[ERROR] Undefined MOORE startup mode in runinfo. Exiting";'
  print 'exit 1;'
  return None

#=========================================================================================
def configureForCheckpoint(runinfo):
  print 'export APP_STARTUP_OPTS="-checkpoint -auto";'
  chkpt_dir = os.path.dirname(os.path.dirname(os.environ['CHECKPOINTINGROOT']))
  print 'export CHECKPOINTING_BIN='+chkpt_dir+'/InstallArea/'+os.environ['CMTCONFIG']+';'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'
  print 'export MBM_SETUP_OPTIONS='+checkpoint_dir+'/cmds/MBM_setup.opts;'
  print 'export CHECKPOINT_SETUP_OPTIONS='+checkpoint_dir+'/cmds/Checkpoint.opts;'
  print 'echo "[ERROR] '+line+'";'
  print 'echo "[ERROR] == Running in checkkpoint PRODUCTION mode....";'
  print 'echo "[ERROR] == File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "[ERROR] == Producing CHECKPOINT file......Please be patient. RunInfo:'+runinfo+'";'
  print 'echo "[ERROR] == LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so";'
  print 'echo "[ERROR] '+line+'";'
  # Note: This is the VERY LAST statement. Afterwards the executable MUST run!
  print 'export LD_PRELOAD=${CHECKPOINTING_BIN}/lib/libCheckpointing.so;'

#=========================================================================================
def configureForTest(runinfo):
  print 'echo "[ERROR] Running in checkpoint TESTING mode....";'
  print 'export APP_STARTUP_OPTS=-restore;'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'
  print 'mkdir -p /dev/shm/checkpoint;'
  print 'RESTORE_CMD="exec -a ${UTGID} restore.exe -p 4 -e -l /dev/shm/checkpoint -i ${CHECKPOINT_FILE}";'
  print 'echo "'+line+'";'
  print 'echo "== File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "== Command ${RESTORE_CMD}";'
  print 'echo "== Testing CHECKPOINT file......Please be patient. RunInfo:'+runinfo+'";'
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
  parser.add_option("-e","--environ",dest='environ',default=False,action='store_true',
                    help="Setup environment from run info",metavar="<boolean>")
  parser.add_option("-l","--libs",dest='libs',default=False,action='store_true',
                    help="Extract libraries from existing checkpoint file",metavar="<directory>")
  parser.add_option("-x","--test",dest='test',default=False,action='store_true',
                    help="Test existing checkpoint file",metavar="<boolean>")
  parser.add_option("-f","--create",dest='create',default=False,action='store_true',
                    help="Create checkpoint file checkpoint file",metavar="<boolean>")
  parser.add_option("-s","--start",dest='start',default=False,action='store_true',
                    help="Start process according to runinfo",metavar="<boolean>")
  parser.add_option("-b","--brunel",dest='brunel',default=False,action='store_true',
                    help="Configure checkpointing for brunel",metavar="<boolean>")
  parser.add_option("-t","--task_type",dest='task_type',default=None,
                    help="Set special task type for brunel configuration",metavar="<string>")

  try:
    (opts, args) = parser.parse_args()

    ##print 'echo [ERROR] Flags: ',opts.copy,opts.libs,opts.test,opts.create,';'
    if os.environ.has_key('TEST_CHECKPOINT') or opts.test:
      configureForTest(opts.runinfo)
      return
    elif os.environ.has_key('CREATE_CHECKPOINT') or opts.create:
      configureForCheckpoint(opts.runinfo)
      return
    elif opts.runinfo is None:
      parser.format_help()
      return

    config_brunel = opts.brunel
    if opts.brunel and opts.task_type:
      config_brunel = opts.task_type
      
    if opts.environ:
      setupEnviron(opts.runinfo,config_brunel=config_brunel)
    if opts.copy and opts.libs:
      extractLibraries(opts.runinfo,copy=True,extract=True,config_brunel=config_brunel)
    elif opts.copy:
      extractLibraries(opts.runinfo,copy=True,extract=False,config_brunel=config_brunel)
    elif opts.libs:
      extractLibraries(opts.runinfo,copy=False,extract=True,config_brunel=config_brunel)
    if opts.start:
      configureForRunning(opts.runinfo,config_brunel=config_brunel,extract=opts.libs)

  except Exception,X:
    traceback.print_stack()
    print 'echo "[ERROR] Exception(ConfigureCheckpoint): Checkpoint production mode:'+str(X)+'";'
    print 'exit 1;'

#=========================================================================================
if __name__ == '__main__':
  doIt()
