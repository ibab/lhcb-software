import Online.PVSS    as PVSS
import Online.Utils   as Utils
import Online.SetupParams as Params
import Online.Streaming.Allocator as Alloc
import Online.Streaming.PartitionInfo as PartitionInfo
import Online.Streaming.StreamingDescriptor as StreamingDescriptor

FSMmanip = Alloc.FSMmanip
PartInfo = PartitionInfo.PartitionInfo
Descriptor = StreamingDescriptor.StreamingDescriptor

def _mgr(name):
  "Access PVSS controls manager by name"
  import Online.PVSSSystems as Systems
  return Systems.controlsMgr(name)

TASK_NODE  = 0
TASK_UTGID = 1
TASK_NAME  = 2
TASK_TYPE  = 3
TASK_CLASS = 4
TASK_DNS   = 5
TASK_SYS   = 6
TASK_SLICE = 7
TASK_OPTS  = 8
#1234567890123456789012345678901234567890123456789012345678901234567890123456789
#-------------------------------------------------------------------------------
class Task(list):
  """
  class Task(list)
  
  @author M.Frank
  """
  def __init__(self,items):    list.__init__(self,items)
  def name(self):              return self[TASK_NAME]
  def node(self):              return self[TASK_NODE]
  def utgid(self):             return self[TASK_UTGID]
  def type(self):              return self[TASK_TYPE]
  def clazz(self):             return self[TASK_CLASS]
  def dimdns(self):            return self[TASK_DNS]
  def system(self):            return self[TASK_SYS]
  def slice(self):             return self[TASK_SLICE]
  def options(self):           return self[TASK_OPTS]

#-------------------------------------------------------------------------------
class Mickey:
  #-----------------------------------------------------------------------------
  def __init__(self, manager, name):
    self.manager = manager
    self.reader = manager.devReader()
    self.name = name
    self.dsc = Descriptor(self.manager,self.name)

  #-----------------------------------------------------------------------------
  def _add(self,tasks,newtasks):
    for i in xrange(newtasks.size()):
      tasks.append(Task(newtasks[i].split('/')))
    return tasks
  
  #-----------------------------------------------------------------------------
  def load(self,partition):
    "Load the partition slice information."
    self.dsc.load()
    slice = self.dsc.getPartition(partition)
    if slice:
      dp, nam, id = slice
      return PartInfo(self.manager,nam).load(self.reader)
    return None

  #-----------------------------------------------------------------------------
  def getTasks(self,partition):
    "Retrieve the tasks content in the system's streaming layer."
    part_info = self.load(partition)
    if part_info:
      tasks = self._add([],part_info.recvInfrastructure())
      tasks = self._add(tasks,part_info.dataSources())
      tasks = self._add(tasks,part_info.recvReceivers())
      tasks = self._add(tasks,part_info.recvSenders())
      tasks = self._add(tasks,part_info.streamInfrastructure())
      tasks = self._add(tasks,part_info.streamReceivers())
      tasks = self._add(tasks,part_info.streamSenders())
      return tasks
    return None
  
  #-----------------------------------------------------------------------------
  def loadTasks(self,partition):
    part_info = self.load(partition)
    fsm = FSMmanip(part_info,'_FwFsmDevice',match='*')
    rdr = self.manager.devReader()
    typ = self.manager.typeMgr().type('FSM_DimTask')
    fsm.__dict__['commands']    = fsm._tskLookup('Command',typ)
    fsm.__dict__['states']      = fsm._tskLookup('State',typ)
    fsm.__dict__['activeTasks'] = {}
    rdr.add(fsm.enabled.container)
    rdr.add(fsm.fsmtypes.container)
    rdr.add(fsm.names.container)
    rdr.add(fsm.inUse.container)
    rdr.add(fsm.nodes.container)
    rdr.add(fsm.commands.container)
    rdr.add(fsm.states.container)
    if rdr.execute():
      tasks,parents = fsm.collectTaskSlots()
      for node in tasks.keys():
        fsm.activeTasks[node] = {}
        items = tasks[node]
        slots = fsm.taskSlots[node]
        for l in items:
          tsk = slots[l]
          if fsm.inUse.container[tsk].data>0:
            fsm.activeTasks[node][fsm.names.container[tsk].data] = tsk
      return fsm
    return None
  
  #-----------------------------------------------------------------------------
  def showPartition(self,partition):
    part_info = self.load(partition)
    self.dsc.show(extended=1)
    PartitionInfo.showMonitoring(part_info,extended=1)

  #-----------------------------------------------------------------------------
  def test(self,partition):
    part_info = self.load(partition)
    fsm = FSMmanip(part_info,'_FwFsmDevice',match='*')
    rdr = self.manager.devReader()
    typ = self.manager.typeMgr().type('FSM_DimTask')
    commands = fsm._tskLookup('Command',typ)
    states = fsm._tskLookup('State',typ)
    rdr.add(fsm.enabled.container)
    rdr.add(fsm.fsmtypes.container)
    rdr.add(fsm.names.container)
    rdr.add(fsm.inUse.container)
    rdr.add(fsm.nodes.container)
    rdr.add(commands.container)
    rdr.add(states.container)
    if rdr.execute():
      tasks,parents = fsm.collectTaskSlots()
      print 'Tasks:',tasks
      for node in tasks.keys():
        items = tasks[node]
        slots = fsm.taskSlots[node]
        for l in items:
          tsk = slots[l]
          if fsm.inUse.container[tsk].data>0:
            print 'Task:',fsm.names.container[tsk].data,commands.container[tsk].data,states.container[tsk].data
            #fsm.cmds.container[tsk].data
      print 'Parents',parents


def setup(sys=Params.monitor_system_name):
  mgr      = _mgr(sys)
  sys = sys[0]+sys[1:].lower()
  mickey = Mickey(mgr,sys)
  return mickey
  
if __name__ == "__main__":
  sys = Params.monitor_system_name
  print 'Main program executing.....',sys
  mickey = setup(sys)
  #mickey.showPartition('LHCb')
  #tasks = mickey.getTasks('LHCb')
  #for i in tasks:
  #  print str(i[:7])
  #mickey.test('LHCb') 
  fsm = mickey.loadTasks('LHCb')
  for n in fsm.activeTasks.keys():
    tasks = fsm.activeTasks[n]
    for t in tasks.keys():
      tsk = tasks[t]
      print n, t, tsk, fsm.states.container[tsk].data, fsm.commands.container[tsk].data
