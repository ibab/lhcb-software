import os, errno, time, threading, subprocess
import Online.Utils as Utils
import Online.AllocatorControl as Control
import Online.PVSSSystems as Systems
import Online.SetupParams as Params
import Online.PVSS as PVSS

DataPoint = PVSS.DataPoint
error     = PVSS.error
log       = Utils.log
##error     = Utils.error

CONDITIONS_DIR = '/group/online/hlt/conditions'
MOORE_RELEASES = '/group/hlt/MOORE/Moore_%s/InstallArea/TCK/manifest'
CHECKPOINT_DIR = '/group/online/dataflow/cmtuser/checkpoints'
MOORE_STARTUP  = '/group/hlt/MOORE/%s/Hlt/Moore/job/runMooreOnline_EFF.sh'
MOORE_STARTUP  = '/group/online/dataflow/cmtuser/TEST/Moore_checkpoint.sh'

# ===========================================================================
def getCONDBtags():
  condb_tags = []
  lines = os.listdir(CONDITIONS_DIR)
  for l in lines:
    if l[:9]=='LHCBCOND_':
      tag = l[9:l.rfind('.')]
      if tag[0]=='h': condb_tags.append(tag)
  return condb_tags

# ===========================================================================
def getDDDBtags():
  dddb_tags = []
  lines = os.listdir(CONDITIONS_DIR)
  for l in lines:
    if l[:5]=='DDDB_':
      tag = l[5:l.rfind('.')]
      if tag[0]=='h': dddb_tags.append(tag)
  return dddb_tags

# ===========================================================================
def getTCKs(moore_version):
  fname = MOORE_RELEASES%(moore_version,)
  lines = open(fname).readlines()
  families = {}
  for i in xrange(len(lines)):
    l = lines[i]
    lines[i] = l[:-1].split(' : ')
    fam = lines[i][0]
    if not families.has_key(fam): families[fam] = []
    families[fam].append(lines[i][3])
  return families

# ===========================================================================
def _mgr(name):
  "Access PVSS controls manager by name"
  return Systems.controlsMgr(name)

# ===========================================================================
def runInfo(runinfo_dp):
  import Online.RunInfoClasses.General as RunInfo
  idx = runinfo_dp.find(':')
  nam = runinfo_dp[:idx]
  part = runinfo_dp[idx+1:]
  part = part[:part.find('_')]
  print '---> runinfo_dp:',runinfo_dp,nam,part
  mgr = _mgr(nam)
  info = RunInfo.General(mgr,name=part,complete=1)
  info.load()
  return info

# ===========================================================================
def test(moore_version='v10r2'):
  condb_tags = getCONDBtags()
  dddb_tags  = getDDDBtags()
  families   = getTCKs(moore_version)

  families   = sorted(families.keys(), key=str.lower, reverse=True)
  condb_tags = sorted(condb_tags, key=str.lower, reverse=True)
  dddb_tags  = sorted(dddb_tags, key=str.lower, reverse=True)
  print 120*'='
  print families
  print condb_tags
  print dddb_tags
  print 120*'='
  for t in families:
    for c in condb_tags[:-1]:
      ct = int(c[c.find('-')+1:])
      cp = c[:c.find('-')]
      for d in dddb_tags[:-2]:
        dt = int(d[d.find('-')+1:])
        dp = d[:d.find('-')]
        if cp == dp and dt==ct:
          print '%-40s %-12s %-12s'%(t,c,d,)
        #else:
        #  print 'SKIP %-40s %-12s %-12s'%(t,c,d,)
      
# ===========================================================================
class CheckpointThread(threading.Thread):
  # =========================================================================
  def __init__(self, name, wd, args, env, mgr):
    threading.Thread.__init__(self,name=name)
    self.start_time = time.time()
    self.args  = args
    self.env   = env
    self.wd    = wd
    self.proc  = None
    self.manager = mgr
    self.messages = DataPoint(mgr,DataPoint.original('CheckpointService.Messages'))
    self.writer   = mgr.devWriter()
    self.writer.add(self.messages)

  # =========================================================================
  def run(self):
    self.start_time = time.time()
    script = self.args[0]
    part = self.args[3]
    hlt  = self.args[1]
    os.chdir(self.wd)
    log('Starting checkpointing process.....')
    p = subprocess.Popen(self.args,stdout=subprocess.PIPE,stderr=subprocess.PIPE,env=self.env)
    self.proc = p
    msg = self.messages
    while 1:
      line = p.stdout.readline()
      if not line: break
      text = line[:-1]
      print text
      msg.data = text
      self.writer.execute(1,1)
    p.stdout.close()
    ret = p.wait()
    answer = '/CheckpointService/'+part+'/'+hlt
    if ret == 255:
      answer = 'ERROR'+answer
      log('Checkpoint creation failed! Return code:%d -> %s'%(ret,answer,))
    elif ret == 0:
      log('Checkpoint creation SUCCEEDED. Return code:%d -> %s'%(ret,answer,))
      answer = 'SUCCESS'+answer
    self.writer.clear()
    status = DataPoint(self.manager,DataPoint.original('CheckpointService.Status'))
    self.writer.add(status)
    self.writer.execute()

# ===========================================================================
class ThreadWatcher(threading.Thread):
  # =========================================================================
  def __init__(self,mgr):
    threading.Thread.__init__(self,name='CheckpointWatchdog')
    self.manager = mgr
    
  # =========================================================================
  def run(self):
    m = self.manager
    while m.run:
      cnt = 0
      for k in m.threads.keys():
        t = m.threads[k]
        if not t.isAlive():
          t.join()
          del m.threads[k]
          log('Joined Checkpoint thread %s'%(k,))
          break
        else:
          cnt = cnt + 1
      time.sleep(10)
      if cnt > 0:
        log('Found %d active checkpoint threads.'%(cnt,),timestamp=1)
    log('Checkpoint manager shutting down....')

# ===========================================================================
class CheckpointManager:
  # =========================================================================
  def __init__(self, manager, name):
    self.run = True
    self.threads={}
    self.runInfos = {}
    self.dp_name = 'STORAGE:StorageAlloc'
    self.manager = manager
    self.name = name
    self.watcher = ThreadWatcher(self)
    self.watcher.start()
    self.messages = DataPoint(self.manager,DataPoint.original('CheckpointService.Messages'))

  # ===========================================================================
  def get(self,name):
    dp = self.dp_name+'.'+name
    if name == 'State':
      dp = 'STORAGE:CheckpointService.State'
    nam = DataPoint.original(dp)
    # log('Access datapoint:'+nam,timestamp=1)
    return DataPoint(self.manager,nam)

  # ===========================================================================
  def allocate(self, rundp_name, partition):
    self.runInfos[partition] = rundp_name
    return 'SUCCESS'

  # ===========================================================================
  def configure(self, rundp_name, partition):
    try:
      PVSS.info('Create checkpoint: Read RunInfo from '+rundp_name+' for '+partition,timestamp=1)
      if partition == 'LHCb':
        res = self.create(rundp_name)
        return res
      return 'SUCCESS'
    except Exception,X:
      print 'Exception:',X
    error('Run Information for partition '+partition+' does not exist!',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # =========================================================================
  def stop(self):
    self.run = False
    self.watcher.join()
    
  # =========================================================================
  def create(self,runinfo_dp):
    info = runInfo(runinfo_dp)
    info.show()
    info.showTrigger()

    application   = 'Moore'
    onlineversion = None
    hltversion    = None
    hltType       = None
    condDBtag     = None
    dimdns        = 'storectl01'
    partition     = info.partitionName()
    runtype       = info.runType()

    if info.mooreVersion is not None:
      hltversion = info.mooreVersion.data
    if info.condDBtag is not None:
      condDBtag = info.condDBtag.data
    if info.hltType is not None:
      hltType = info.hltType.data
    if info.onlineVersion is not None:
      onlineversion = info.onlineVersion.data

    if application and hltversion and hltType and condDBtag:
      dir_name = CHECKPOINT_DIR+os.sep+application+os.sep+hltversion+os.sep+hltType+os.sep+condDBtag
      fname = dir_name+os.sep+'Checkpoint.data'

      if self.threads.has_key(fname):
        log('The checkpoint file %s is already under construction. --> Nothing to do.'%(fname,))
        return 'SUCCESS'

      if hltversion != 'Moore_v11r2dev':
        log('Checkpointing is only upported for %s. --> Nothing to do.'%(hltversion,))
        return None

      try:
        os.stat(fname)
        log('The checkpoint file '+fname+' already exists. --> Nothing to do.')
        ##return 'SUCCESS'
      except OSError,e:
        if e.errno==errno.ENOENT:
          pass

      try:
        os.makedirs(dir_name)
      except OSError,e:
        if e.errno != errno.EEXIST:
          raise e
        log('Target directory:%s exists already.'%dir_name)
      
      log('Create checkpoint file: %s'%(fname,))
      script = MOORE_STARTUP  ###%(hltversion,)
      args = [script,hltversion,dimdns,partition,runtype]
      log('Starting script: %s'%(str(args),))
      env = {}
      env['UTGID']           = partition+'_'+hltversion
      env['LOGFIFO']         = '/tmp/logGaudi.fifo'
      env['DIM_DNS_NODE']    = dimdns
      env['GAUDITASK_OPTS']  = '-checkpoint'
      env['CHECKPOINT_FILE'] = fname
      env['CMTCONFIG']       = 'x86_64-slc5-gcc43-dbg'

      t = CheckpointThread(name=fname,wd=os.path.dirname(script),args=args,env=env,mgr=self.manager)
      self.threads[fname] = t
      t.start()
      return 'SUCCESS'
    return None

def runCheckpointManager(name='CHKPT'):
  """
  Execute job options writer for the HLT farm.

   Arguments:
   @param name        System name
   @param sim         (Dummy)

   @author M.Frank
  """
  mgr    = _mgr(Params.storage_system_name)
  chkpt  = CheckpointManager(mgr,'CheckpointSvc')
  ctrl   = Control.Control(mgr,'Storage','Alloc',[chkpt])
  ctrl.run()
  wait = 1
  if wait != 0:
    if not PVSS.batchMode(): print 'Hit CTRL-C to quit!'
    ctrl.sleep()
    print '\n\nDone...\n\n'
    return 1
  return None

# ===========================================================================
def testCheckpointThread():
  t = CheckpointThread(name='test',wd='/',args=['/bin/ls','-l'],env={})
  t.start()
  t.join()
  log('Test finished.')
  
###testCheckpointThread()

def testCheckpointThread():
  mgr = _mgr(Params.storage_system_name)
  m = CheckpointManager(mgr,'Storage')
  time.sleep(2)
  m.create('ECS:LHCb_RunInfo')
  m.stop()

runCheckpointManager()
