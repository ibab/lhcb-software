import os, sys, imp, traceback

global restore_args
restore_args = ''

sep = '/'
checkpoint_zip = True
checkpoint_dir = '/group/online/dataflow/cmtuser/checkpoints'
checkpoint_expand_area = '/dev/shm/Torrents'
checkpoint_local_area  = '/localdisk/checkpoints'
checkpoint_expand_area = checkpoint_local_area+'/Torrents'

if os.environ.has_key('LOCAL_CHECKPOINT_DIR'):
  checkpoint_local_area = os.environ['LOCAL_CHECKPOINT_DIR']

line = "====================================================================================="


#=========================================================================================
class RunInfo:
  """

     \author  M.Frank
     \version 1.0
  """
  #=======================================================================================
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

#=========================================================================================
class Checkpoint:
  """
     \author  M.Frank
     \version 1.0
  """

  #=======================================================================================
  def __init__(self, runinfo, config_task_type, config_task_family=None):
    global checkpoint_local_area
    self.runinfo = runinfo
    self.config_task_type = config_task_type
    self.config_task_family = config_task_family
    ##print 'echo "[INFO] '+str(isinstance(self.config_task_type,str))+'";'
    if isinstance(self.config_task_type,str):
      self.task_type = self.config_task_type
    elif self.config_task_type:
      self.task_type = 'OnlineBrunel' # FixME: runinfo.HLTType
    elif runinfo.MooreOnlineVersion == "" or runinfo.MooreOnlineVersion == "DEFAULT":
      self.task_type = 'PassThrough'
    else:
      self.task_type = 'Moore1' # FixME: runinfo.HLTType

    self.torrent_file   = self._getTorrentFile()
    self.checkpoint_bin = os.environ['CHECKPOINTING_BIN']
    self.restore        = 'restore.exe' ##self.checkpoint_bin + '/bin/restore.exe'
    if self.torrent_file:
      if checkpoint_zip:
        self.target_path = checkpoint_expand_area+sep+self.task_type+sep+"Checkpoint.data"
      else:
        self.target_path = checkpoint_local_area+self.checkpointRelativeDir()+sep+self.torrent_file
      self.lib_dir = os.path.dirname(self.target_path)+'/lib'

  #=======================================================================================
  def _getTorrentFile(self):
    try:
      if (self.config_task_family=='REC' and self.runinfo.RecoStartupMode > 1):
        directory = checkpoint_dir + self.checkpointRelativeDir()
        for i in os.listdir(directory):
          idx=i.find('.data')
          if idx>0 and idx==len(i)-5:
            return i
      elif (self.config_task_family=='HLT' and self.runinfo.MooreStartupMode > 1):
        directory = checkpoint_dir + self.checkpointRelativeDir()
        for i in os.listdir(directory):
          idx=i.find('.data')
          if idx>0 and idx==len(i)-5:
            return i
    except Exception,X:
      print 'echo "[ERROR] Check torrent file: '+str(X)+'";'
    return None

  #=======================================================================================
  def checkpointRelativeDir(self):
    ri = self.runinfo
    if self.config_task_family == 'REC' and self.config_task_type:
      Mapping = os.path.basename(ri.ConditionsMapping)
      checkpoint_reloc = sep+self.task_type+sep+ri.OnlineBrunelVersion+sep+\
          ri.CondDBTag+sep+ri.DDDBTag+sep+Mapping
    elif ri.MooreOnlineVersion == "" or ri.MooreOnlineVersion == "DEFAULT":
      checkpoint_reloc = sep+ri.HltArchitecture+sep+ri.OnlineVersion+sep+ri.HLTType
    else:
      Mapping = os.path.basename(ri.ConditionsMapping)
      checkpoint_reloc = sep+'Moore'+sep+ri.MooreOnlineVersion+sep+\
          ri.HLTType+sep+ri.CondDBTag+sep+ri.DDDBTag+sep+\
          Mapping+sep+self.task_type
    return checkpoint_reloc

  #=======================================================================================
  def torrentFile(self):
    return self.torrent_file

  #=======================================================================================
  def output(self, message):
    print message
    ## print 'echo [INFO] ',message
    return self

  #=======================================================================================
  def initProcess(self):
    self.output('unset CHECKPOINT_DIR;') \
        .output('unset CHECKPOINT_FILE;') \
        .output('unset LD_PRELOAD;') \
        .output('export CHECKPOINTING_BIN='+self.checkpoint_bin+';')

    if self.config_task_family=='REC':
      self.output('export PYTHONPATH=/group/online/dataflow/options/'+self.runinfo.PartitionName+'/RECONSTRUCTION:${PYTHONPATH};')
    elif self.config_task_family=='DQ':
      self.output('export PYTHONPATH=/group/online/dataflow/options/'+self.runinfo.PartitionName+':${PYTHONPATH};')
    else:
      self.output('export PYTHONPATH=/group/online/dataflow/options/'+self.runinfo.PartitionName+'/HLT:${PYTHONPATH};')
    return self

  #=======================================================================================
  def addSlaves(self):
    return self.output('export NUM_CORES=${NBOFSLAVES};')

  #=======================================================================================
  def preload(self):
    return self.output('export LD_PRELOAD=libCheckpointing.so;')

  #=======================================================================================
  def setupNormal(self):
    return self.initProcess().output('export APP_STARTUP_OPTS=-normal;').preload() \
	       .output('export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Empty.opts;')

  #=======================================================================================
  def setupForking(self):
    return self.initProcess().addSlaves().preload() \
        .output('export APP_STARTUP_OPTS=-forking;')

  #=======================================================================================
  def setupCheckpointFile(self):
    #self.output('export CHECKPOINT_DIR='+os.path.dirname(self.target_path)+';') \
    #    .output('export CHECKPOINT_FILE='+self.target_path+';') 
    self.output('export CHECKPOINT_SETUP_OPTIONS=${FARMCONFIGROOT}/options/Checkpoint.opts;') \
        .output('export CHECKPOINT_RESTART_OPTIONS=${FARMCONFIGROOT}/options/CheckpointRestart.opts;')
    return self

  #=======================================================================================
  def extractLibs(self):
    """
    Note:
    There is quite some optimization here by checking the modifcation date
    of the libs directory against the modification date of the checkpoint file.
    """
    lib_md5 = self.target_path+'.libs.md5'
    tar_md5 = checkpoint_dir + self.checkpointRelativeDir()+os.sep+self.torrent_file+'.libs.md5'
    self.output('if test ! -d "'+self.lib_dir+'"; then') \
        .output('  need_extract=1;')\
        .output('elif test ! -f "'+lib_md5+'"; then') \
        .output('  need_extract=1;')\
        .output('elif test -d "'+self.lib_dir+'"; then')
    #self.output('  echo md5sum '+tar_md5+';')\
    #    .output('  echo md5sum '+lib_md5+';')
    self.output('  torrent_stat=`md5sum '+tar_md5+' | cut -b 1-32`;')\
        .output('  libsdir_stat=`md5sum '+lib_md5+' | cut -b 1-32`;')\
        .output('  if test "${libsdir_stat}" != "${torrent_stat}"; then')\
        .output('    need_extract=1;')\
        .output('  fi;') \
        .output('fi;')

    self.output('if test -n "${need_extract}"; then')\
        .output('  echo "[INFO] Need to expand libraries from checkpoint before start [Check-sum].";') \
        .output('  mkdir -p '+self.lib_dir+';') \
        .output('  rm -f '+self.lib_dir+'/*;') \
        .output(   self.restore + ' -p 4 -x -C -i '+self.target_path+' -l '+self.lib_dir+'/;') \
        .output('  cd '+self.lib_dir+'; md5sum `ls -S -1 . | grep -v passwd` > '+lib_md5+'; cd -;') \
        .output('else') \
        .output('  echo "[INFO] MD5 of libs[${libsdir_stat}] fits torrent[${torrent_stat}]  Nothing to expand before start [Checksum-OK].";') \
        .output('fi;')

    self.output('if test ! -d "'+self.lib_dir+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT LIBS DIRECTORY '+self.lib_dir+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    self.output('if test ! -f "'+lib_md5+'"; then') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  echo "[FATAL] == CHECKPOINT LIBS MD5 '+lib_md5+' DOES NOT EXIST!";') \
        .output('  echo "[FATAL] '+line+'";') \
        .output('  exit 1;') \
        .output('fi;')
    return self

  #=======================================================================================
  def extractLibs_old(self):
    """
    Note:
    There is quite some optimization here by checking the modifcation date
    of the libs directory against the modification date of the checkpoint file.
    """
    self.output('if test ! -d "'+self.lib_dir+'"; then') \
        .output('  need_extract=1;')\
        .output('fi;')
    self.output('if test -d "'+self.lib_dir+'"; then') \
        .output('  torrent_stat=`stat -c "%Y" '+self.target_path+'`;')\
        .output('  libsdir_stat=`stat -c "%Y" '+self.lib_dir+'`;')\
        .output('  if test ${libsdir_stat} -le ${torrent_stat};then')\
        .output('    need_extract=1;')\
        .output('  fi;') \
        .output('fi;')

    self.output('if test -n "${need_extract}"; then')\
        .output('  echo "[INFO] Need to expand libraries from checkpoint before start [Time-stams].";') \
        .output(   self.restore + ' -p 4 -x -C -i '+self.target_path+' -l '+self.lib_dir+'/;') \
        .output('else') \
        .output('  echo "[INFO] lib[${libsdir_stat}] is younger than torrent[${torrent_stat}]  Nothing to expand before start [Time-stamps-OK].";') \
        .output('fi;')

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
        .output('export APP_STARTUP_OPTS=-restore;')

  #=======================================================================================
  def setupRestore(self, extract=True):
    ldir = self.checkpointRelativeDir()
    relocate_path = ldir+sep+self.torrent_file
    if extract: extract_libs_opt = '-x'
    else:       extract_libs_opt = '-X'
    local_libs_opt = ' -l '+self.lib_dir

    restore_cmd = 'exec -a ${UTGID} '+self.restore+' -p 4 -e '+\
                  extract_libs_opt+local_libs_opt+' -i '+self.target_path

    print 'echo "[INFO] Checkpointing Restore from torrent: '+restore_cmd+'";'
    self.initProcess().setupCheckpointFile() \
        .output('echo "[DEBUG] Setup restore process for %s";'%(self.task_type,)) \
        .output('export APP_STARTUP_OPTS=-restore;') \
        .output('sleep 3;') \
        .output('RESTORE_CMD="'+restore_cmd+'";') \
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
    if self.config_task_family=='REC':
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
def setupEnviron(runinfo, config_task_family, config_task_type):
  ri = RunInfo(runinfo)
  ##print 'echo "[ERROR] Configure CHECKPOINT environment. Reco:%s/%d Moore:%d";'%(str(config_task_type),ri.RecoStartupMode,ri.MooreStartupMode,)
  cp = Checkpoint(ri,config_task_type,config_task_family)

  if   config_task_family=='REC' and ri.RecoStartupMode == 0:
    return cp.setupNormal()
  elif config_task_family=='REC' and ri.RecoStartupMode == 1:
    return cp.setupForking()
  elif config_task_family=='REC' and ri.RecoStartupMode == 2:
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
def configureForRunning(runinfo, config_task_family, config_task_type, extract=True):
  ri = RunInfo(runinfo)
  ##print 'echo "[ERROR] Configure CHECKPOINT for running. Reco:%s/%d Moore:%d";'%(str(config_task_type),ri.RecoStartupMode,ri.MooreStartupMode,)
  cp = Checkpoint(ri,config_task_type,config_task_family)

  if   config_task_family=='REC' and ri.RecoStartupMode == 0:
    return cp.setupNormal()
  elif config_task_family=='REC' and ri.RecoStartupMode == 1:
    return cp.setupForking()
  elif config_task_family=='REC' and ri.RecoStartupMode == 2:
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
def extractLibraries(runinfo,copy,extract,config_task_family,config_task_type):
  ri = RunInfo(runinfo)
  cp = Checkpoint(ri,config_task_type,config_task_family)
  if config_task_family=='REC' and (ri.RecoStartupMode == 0 or ri.RecoStartupMode == 1):
    print 'echo "[DEBUG] No checkpoints to be manipulated for BRUNEL startup mode:'+str(ri.RecoStartupMode)+'.";'
    return 1
  elif config_task_family=='REC' and ri.RecoStartupMode == 2:
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
  print 'echo "[ERROR] == LD_PRELOAD=libCheckpointing.so";'
  print 'echo "[ERROR] '+line+'";'
  # Note: This is the VERY LAST statement. Afterwards the executable MUST run!
  print 'export LD_PRELOAD=libCheckpointing.so;'

#=========================================================================================
def configureForTest(runinfo):
  print 'echo "[ALWAYS] Running in checkpoint TESTING mode....";'
  print 'export APP_STARTUP_OPTS=-restore;'
  print 'export CHECKPOINT_DIR; export CHECKPOINT_FILE;'
  print 'export PYTHONPATH=${CHECKPOINT_DIR}:${PYTHONPATH};'
  print 'export CHECKPOINT_EXPANSION_DIR=/dev/shm/checkpoint;'
  print 'export CHECKPOINT_EXPANSION_DIR=/localdisk/checkpoints/test;'
  print 'rm -rf ${CHECKPOINT_EXPANSION_DIR}/*;'
  print 'mkdir -p ${CHECKPOINT_EXPANSION_DIR};'
  print 'RESTORE_CMD="exec -a ${UTGID} restore.exe -p 4 -e -l ${CHECKPOINT_EXPANSION_DIR} -i ${CHECKPOINT_FILE}";'
  print 'echo "[ALWAYS] '+line+'";'
  print 'echo "[ALWAYS] == File:  ${CHECKPOINT_FILE} MBM setup:${MBM_SETUP_OPTIONS}";'
  print 'echo "[ALWAYS] == Command ${RESTORE_CMD}";'
  print 'echo "[ALWAYS] == Testing CHECKPOINT file......Please be patient. RunInfo:'+runinfo+'";'
  print 'echo "[ALWAYS] == DO NOT CLOSE THE PANEL YET UNLESS CHECKPOINT TEST IS OVER !!!!!";'
  print 'echo "[ALWAYS] '+line+'";'
  print 'if test ! -f "${CHECKPOINT_FILE}";then'
  print '  echo "[FATAL] '+line+'";'
  print '  echo "[FATAL] == CHECKPOINT FILE ${CHECKPOINT_FILE} DOES NOT EXIST!";'
  print '  echo "[FATAL] '+line+'";'
  print '  exit 2;'
  print 'fi;'
  print 'echo "[ALWAYS] '+line+'";'
  print 'echo "[ALWAYS] == Executing second restore to extract libraries and computing lib checksum. RunInfo:'+runinfo+'";'
  print 'echo "[ALWAYS] == DO NOT CLOSE THE PANEL YET UNLESS CHECKPOINT TEST IS OVER !!!!!";'
  print 'echo "[ALWAYS] '+line+'";'
  print 'restore.exe -p 4 -x -C -l ${CHECKPOINT_EXPANSION_DIR} -i ${CHECKPOINT_FILE};'
  print 'CURR=`pwd`;cd ${CHECKPOINT_EXPANSION_DIR}; md5sum `ls -S . | grep -v passwd` > ${CHECKPOINT_FILE}.libs.md5; cd ${CURR};'
  print 'if test ! -f "${CHECKPOINT_FILE}.libs.md5";then'
  print '  echo "[FATAL] '+line+'";'
  print '  echo "[FATAL] == CHECKPOINT library checksum FILE ${CHECKPOINT_FILE}.libs.md5 DOES NOT EXIST!";'
  print '  echo "[FATAL] '+line+'";'
  print '  exit 2;'
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

    task_family = 'HLT'
    config_task_type = 'Moore1'
    if opts.brunel and not opts.task_type:
      config_task_type = 'OnlineBrunel'
      task_family = 'REC'
    elif opts.brunel and opts.task_type:
      config_task_type = opts.task_type
      task_family = 'REC'
    elif opts.task_type:
      config_task_type = opts.task_type
    else:
      config_task_type = 'Moore1'
      
    if opts.environ:
      setupEnviron(opts.runinfo,config_task_family=task_family,config_task_type=config_task_type)
    if opts.copy and opts.libs:
      extractLibraries(opts.runinfo,copy=True,extract=True,config_task_family=task_family,config_task_type=config_task_type)
    elif opts.copy:
      extractLibraries(opts.runinfo,copy=True,extract=False,config_task_family=task_family,config_task_type=config_task_type)
    elif opts.libs:
      extractLibraries(opts.runinfo,copy=False,extract=True,config_task_family=task_family,config_task_type=config_task_type)
    if opts.start:
      configureForRunning(opts.runinfo,config_task_family=task_family,config_task_type=config_task_type,extract=opts.libs)

  except Exception,X:
    traceback.print_stack()
    print 'echo "[ERROR] Exception(ConfigureCheckpoint): Checkpoint production mode:'+str(X)+'";'
    print 'exit 1;'

#=========================================================================================
if __name__ == '__main__':
  doIt()
