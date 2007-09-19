"""
Online Joboptions installation

import Online.JobOptions.Installer as JOI
JOI.install()

JOI.uninstall()

"""

import Online.JobOptions.JobOptions as JobOptions
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.RunInfoClasses.Storage as StorageInfo
import Online.Utils as Utils
import Online.PVSS as PVSS

DataPoint = PVSS.DataPoint
log = Utils.log
mgr = Systems.controlsMgr(Params.jobopts_system_name)
JobOptions.debug = 1

# =============================================================================
class Task(JobOptions.TaskType):
  def __init__(self, name, opts, defs=1, tell1=0):
    JobOptions.TaskType.__init__(self,mgr,name)
    if not self.exists():
      self.create()
    self.options.data = opts
    self.defaults.data = defs
    self.tell1s.data = tell1
    self.save()

# =============================================================================
class RunType(JobOptions.Activity):
  def __init__(self, name, tasks):
    JobOptions.Activity.__init__(self,mgr,name)
    if not self.exists():
      self.create()
      self.load()
    self.tasks.data.clear()
    for t in tasks:
      nam = t.datapoint().name()
      log(self.name+'> Add task:'+nam+' ['+str(DataPoint.exists(nam))+']',timestamp=1)
      self.tasks.data.push_back(t.datapoint().id())
    self.save()

# =============================================================================
def deleteDevices(type,match):
  actor = JobOptions._getDevices(type,match,mgr,0)
  m = mgr.deviceMgr()
  for i in actor.container:
    if m.exists(i.name()):
      result = m.deleteDevice(i.name())
      if result is None:
        log('Failed to delete datapoint:'+i.name()+' [Internal error]',timestamp=1)
      else:
        log('Deleted datapoint:'+i.name()+' ['+type+']',timestamp=1)

# =============================================================================
class TaskMap(dict):
  def __init__(self):
    dict.__init__(self)
  def add(self, task):
    self[task.name] = task

tasks = TaskMap()
moni_streams = ['Stream1', 'Stream2', 'Stream3']
moni_types   = ['MonBBinc','MonVELO','MonECAL','MonHCAL','MonHCAL','MonRICH','MonTT','MonIT','MonOT','MonIT','MonMUON']

lhcb_streams = ['Higgs',   'B2JPsi',  'B2Dstar', 'B2Charm', 'BBinc']
sd_streams   = ['Events']


# =============================================================================
def installDAQTasks():
  streams = []
  for i in sd_streams: streams.append(i)
  for i in lhcb_streams: streams.append(i)
  # Generic tasks
  opts = '#include "$ONLINETASKS/options/ErrorServer.opts"'
  tasks.add(Task('ErrSrv',opts,1,0))

  # HLT Farm tasks
  opts = '#include "$ONLINETASKS/options/EbSetup.opts"'
  tasks.add(Task('EventBuilder',opts,1,1))
  opts = '#include "$ONLINETASKS/options/HLTMBM.opts"'
  tasks.add(Task('HLTMBM',opts,1,0))
  opts = '#include "$ONLINETASKS/options/Moore.opts"'
  tasks.add(Task('HLTMoore',opts,1,0))
  opts = '#include "$ONLINETASKS/options/HLTSend.opts"'
  tasks.add(Task('HLTSend',opts,1,0))

  # Storage receiving layer
  opts = '#include "$ONLINETASKS/options/RecvMBM.opts"'
  tasks.add(Task('RecvMBM',opts,1,0))
  opts = '#include "$ONLINETASKS/options/HLTRec.opts"'
  tasks.add(Task('HLTRec',opts,1,0))
  for s in streams:
    opts = '#include "$ONLINETASKS/options/SND'+s+'.opts"'
    tasks.add(Task('SND'+s,opts,1,0))

  # Storage streaming layer
  opts = '#include "$ONLINETASKS/options/StrmMBM.opts"'
  tasks.add(Task('StrmMBM',opts,1,0))
  for s in streams:
    opts = '#include "$ONLINETASKS/options/RCV'+s+'.opts"'
    tasks.add(Task('RCV'+s,opts,1,0))
    opts = '#include "$ONLINETASKS/options/WRT'+s+'.opts"'
    tasks.add(Task('WRT'+s,opts,1,0))

# =============================================================================
def installMonTasks():
  streams = moni_streams
  opts = '#include "$ONLINETASKS/options/MBMRelay.opts"'
  tasks.add(Task('MBMRelay',opts,1,0))
  for s in streams:
    opts = '#include "$ONLINETASKS/options/SND'+s+'.opts"'
    tasks.add(Task('SND'+s,opts,1,0))
    opts = '#include "$ONLINETASKS/options/RCV'+s+'.opts"'
    tasks.add(Task('RCV'+s,opts,1,0))

  opts = '#include "$ONLINETASKS/options/RCVMon.opts"'
  tasks.add(Task('RCVMon',opts,1,0))
  opts = '#include "$ONLINETASKS/options/MBMMon.opts"'
  tasks.add(Task('MBMMon',opts,1,0))
  streams = moni_types
  for s in streams:
    opts = '#include "$ONLINETASKS/options/'+s+'.opts"'
    tasks.add(Task(s,opts,1,0))

# =============================================================================
def installPartitions():
  actor = PVSS.DpVectorActor(mgr)
  actor = JobOptions._getDevices('RunInfo','',mgr,0)
  actor.lookup('*_RunInfo',mgr.typeMgr().type('RunInfo'))
  for i in actor.container:
    name = i.name()
    name = name[name.find(':')+1:]
    name = name[:name.find('_')]
    log('Partition:'+name,timestamp=1)
    run = StorageInfo.StorageInfo(mgr,name)
    run.load()
    p = JobOptions.Partition(mgr,name)
    if not p.exists():
      p.create()
      p.load()
    p.activity.data  = run.runTyp.id()
    p.partID.data    = run.partID.id()
    p.Tell1.data     = run.tell1Boards.id()
    p.save()

# =============================================================================
def installSDData():
  task_set = [tasks['ErrSrv'],
              tasks['HLTMBM'],
              tasks['EventBuilder'],
              tasks['HLTMoore'],
              tasks['HLTSend']]
  hlt_physics = RunType('HLT_SDET',task_set)
  task_set = [tasks['ErrSrv'],
              tasks['RecvMBM'],
              tasks['HLTRec']]
  for s in sd_streams: task_set.append(tasks['SND'+s])
  storage_recv = RunType('RECV_SDET',task_set)
  task_set = [tasks['ErrSrv'],tasks['StrmMBM']]
  for s in sd_streams:
    task_set.append(tasks['RCV'+s])
    task_set.append(tasks['WRT'+s])
  storage_stream = RunType('STREAM_SDET',task_set)

# =============================================================================
def installDAQPhysics():
  task_set = [tasks['ErrSrv'],
              tasks['HLTMBM'],
              tasks['EventBuilder'],
              tasks['HLTMoore'],
              tasks['HLTSend']]
  hlt_physics = RunType('HLT_PHYSICS',task_set)
  task_set = [tasks['ErrSrv'],
              tasks['RecvMBM'],
              tasks['HLTRec']]
  for s in lhcb_streams: task_set.append(tasks['SND'+s])
  storage_recv = RunType('RECV_PHYSICS',task_set)
  task_set = [tasks['ErrSrv'],tasks['StrmMBM']]
  for s in lhcb_streams:
    task_set.append(tasks['RCV'+s])
    task_set.append(tasks['WRT'+s])
  storage_stream = RunType('STREAM_PHYSICS',task_set)

# =============================================================================
def installMonPhysics():
  task_set = [tasks['ErrSrv'], tasks['MBMRelay']]
  for s in moni_streams:
    task_set.append(tasks['RCV'+s])
    task_set.append(tasks['SND'+s])
  rt = RunType('MONRELAY_PHYSICS',task_set)
  
  task_set = [tasks['ErrSrv'],
              tasks['MBMMon'],
              tasks['RCVMon'],
              tasks['MonBBinc'],
              tasks['MonVELO'],
              tasks['MonECAL'],
              tasks['MonHCAL'],
              tasks['MonRICH'],
              tasks['MonMUON']]
  rt = RunType('MONI_PHYSICS',task_set)

# =============================================================================
def install():
  installPartitions()
  installDAQTasks()
  installDAQPhysics()
  installSDData()
  installMonTasks()
  installMonPhysics()

# =============================================================================
def uninstall():
  deleteDevices(JobOptions.Partition_t,'')
  deleteDevices(JobOptions.Activity_t,'')
  deleteDevices(JobOptions.TaskType_t,'')

