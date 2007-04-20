import Online.JobOptions.JobOptions as JobOptions
import Online.RunInfo as RunInfo
import Online.Utils as Utils
import Online.PVSS as PVSS

DataPoint = PVSS.DataPoint
log = Utils.log
mgr = PVSS.controlsMgr()
JobOptions.debug = 1

class Task(JobOptions.TaskType):
  def __init__(self, name, opts, defs=1, tell1=0):
    JobOptions.TaskType.__init__(self,mgr,name)
    if not self.exists():
      self.create()
    self.options.data = opts
    self.defaults.data = defs
    self.tell1s.data = tell1
    print name,self.defaults.data,self.tell1s.data 
    self.save()
      
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

class TaskMap(dict):
  def __init__(self):
    dict.__init__(self)
  def add(self, task):
    self[task.name] = task

tasks = TaskMap()
lhcb_streams = ['Higgs','B2JPsi','B2Dstar','B2Charm','BBinclusive']
sd_streams = ['Events']
def installTasks():
  streams = []
  for i in sd_streams: streams.append(i)
  for i in lhcb_streams: streams.append(i)
  # Generic tasks
  opts = '#include "$ONLINETASKS/options/ErrorServer.opts"'
  tasks.add(Task('ErrSrv',opts,1,0))

  # HLT Farm tasks
  opts = '#include "$ONLINETASKS/options/EbSetup.opts"'
  tasks.add(Task('EventBuilder',opts,1,1))
  opts = '#include "$ONLINETASKS/options/HLT_MBM.opts"'
  tasks.add(Task('HLT_MBM',opts,1,0))
  opts = '#include "$ONLINETASKS/options/Moore.opts"'
  tasks.add(Task('HLT_Moore',opts,1,0))
  opts = '#include "$ONLINETASKS/options/HLT_Sender.opts"'
  tasks.add(Task('HLT_Sender',opts,1,0))

  # Storage receiving layer
  opts = '#include "$ONLINETASKS/options/StoreRecv_MBM.opts"'
  tasks.add(Task('StoreRecv_MBM',opts,1,0))
  opts = '#include "$ONLINETASKS/options/HLT_Receiver.opts"'
  tasks.add(Task('HLT_Receiver',opts,1,0))
  for s in streams:
    opts = '#include "$ONLINETASKS/options/SND_'+s+'.opts"'
    tasks.add(Task('SND_'+s,opts,1,0))

  # Storage streaming layer
  opts = '#include "$ONLINETASKS/options/StoreStream_MBM.opts"'
  tasks.add(Task('StoreStream_MBM',opts,1,0))
  for s in streams:
    opts = '#include "$ONLINETASKS/options/RCV_'+s+'.opts"'
    tasks.add(Task('RCV_'+s,opts,1,0))
    opts = '#include "$ONLINETASKS/options/WRT_'+s+'.opts"'
    tasks.add(Task('WRT_'+s,opts,1,0))

def installPartitions():
  actor = PVSS.DpVectorActor(mgr)
  actor = JobOptions._getDevices('RunInfo','',mgr,0)
  actor.lookup('*_RunInfo',mgr.typeMgr().type('RunInfo'))
  for i in actor.container:
    name = i.name()
    name = name[name.find(':')+1:]
    name = name[:name.find('_')]
    log('Partition:'+name,timestamp=1)
    run = RunInfo.RunInfo(mgr,name)
    run.load()
    p = JobOptions.Partition(mgr,name)
    if not p.exists():
      p.create()
      p.load()
    p.activity.data  = run.runTyp.id()
    p.partID.data    = run.partID.id()
    p.Tell1.data     = run.tell1Boards.id()
    p.save()

def install():
  installPartitions()
  installTasks()
  task_set = [tasks['EventBuilder'],
              tasks['ErrSrv'],
              tasks['HLT_MBM'],
              tasks['HLT_Moore'],
              tasks['HLT_Sender']]
  hlt_physics = RunType('HLT_PHYSICS',task_set)
  task_set = [tasks['StoreRecv_MBM'],
              tasks['ErrSrv'],
              tasks['HLT_Receiver']]
  for s in lhcb_streams: task_set.append(tasks['SND_'+s])
  storage_recv = RunType('RECV_PHYSICS',task_set)
  task_set = [tasks['StoreStream_MBM'],
              tasks['ErrSrv']]
  for s in lhcb_streams:
    task_set.append(tasks['RCV_'+s])
    task_set.append(tasks['WRT_'+s])
  storage_stream = RunType('STREAM_PHYSICS',task_set)

def uninstall():
  deleteDevices(JobOptions.Partition_t,'')
  deleteDevices(JobOptions.Activity_t,'')
  deleteDevices(JobOptions.TaskType_t,'')

