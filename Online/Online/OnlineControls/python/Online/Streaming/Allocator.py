import copy, time
import Online.PVSS    as PVSS
import Online.Utils   as Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.Streaming.PartitionInfo as PartitionInfo
from   Online.Streaming.SlotPolicy import SlotPolicy as SlotPolicy
from   Online.Streaming.StreamingDescriptor import StreamingDescriptor as StreamingDescriptor

log            = Utils.log
error          = PVSS.error
warning        = Utils.warning
printSlots     = Utils.printSlots
std            = PVSS.gbl.std

DataPoint = PVSS.DataPoint
debug = 0

def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()

# =============================================================================
class FSMmanip:
  """

    fwTree_getNodeUserData(node, udata, exInfo);
    udata[1] = visi;
    udata[2] = 1;
    udata[3] = label;
    udata[4] = panel;

    Nodes:  
    dist_2:TestStorage_Slice00|TestStorage_Slice00_lbstorcv1.mode.enabled  1
    dist_2:TestStorage_Slice00|FSM_Node_FWDM.fsm.sendCommand:_original.._value
    ENABLE/DEVICE(S)=TESTSTORAGE_SLICE00_LBSTORCV1

    Class 0:
    dist_2:TestStorage_Slice00|TestStorage_Slice00_lbstorcv1_Class0.mode.enabled  0  
    dist_2:TestStorage_Slice00|TestStorage_Slice00_lbstorcv1FSM_Class0_FWDM.fsm.sendCommand:_original.._value
    DISABLE/DEVICE(S)=TESTSTORAGE_SLICE00_LBSTORCV1_CLASS0
    
    Tasks:
    dist_2:TestStorage_Slice00|TestStorage_Slice00_lbstorcv1_000.mode.enabled  0
    dist_2:TestStorage_Slice00|TestStorage_Slice00_lbstorcv1_Class0FSM_DimTask_FWDM.fsm.sendCommand:_original.._value
    DISABLE/DEVICE(S)=TESTSTORAGE_SLICE00_LBSTORCV1_000

  """
  # ===========================================================================
  def __init__(self,info,type,match='*'):
    self.info    = info
    self.name    = info.name
    self.match   = match
    self.manager = info.manager
    self.reader  = self.manager.devReader()
    self.writer  = self.manager.devWriter()
    typ          = self.manager.typeMgr().type(type)
    tsk_typ      = self.manager.typeMgr().type('FSM_DimTask')
    self.enabled = self._fsmLookup('mode.enabled',typ)
    self.labels  = self._fsmLookup('ui.label',typ)
    self.visible = self._fsmLookup('ui.visible',typ)
    self.fsmtypes= self._fsmLookup('type',typ)
    self.tnode   = self._fsmLookup('tnode',typ)
    self.names   = self._tskLookup('Name',tsk_typ)
    self.inUse   = self._tskLookup('InUse',tsk_typ)
    self.nodes   = self._tskLookup('Node',tsk_typ)
    self.prio    = self._tskLookup('Priority',tsk_typ)
    self.types   = self._tskLookup('Type',tsk_typ)
    self.cmds    = self._tskLookup('FMC_Start',tsk_typ)
    self.sysName = self._tskLookup('SysName',tsk_typ)
    self.dimDns  = self._tskLookup('DimDNS',tsk_typ)
    self.slots = {}
    self.startupInfo       = self._startupInfo
    self.optionsFile       = self._optsFile
    self.setupTask         = self._setupTask
    self.configureTask     = self._configureTask
    self.allocateProcesses = self._allocateProcesses
    self.fifoName          = self._fifoName
    for i in xrange(self.names.container.size()):
      nam = self.names.container[i].name()
      node = nam[nam.find(':')+1:]
      itms = node.split('_')
      node = itms[2]
      if len(itms)>2 and itms[3][:3]=="MON":
        node = node+'_'+itms[3]
      if not self.slots.has_key(node): self.slots[node] = []
      self.slots[node].append(i)
    self.tasks = {}
    self.nodeParents = {}
    rdr = self.manager.devReader()
    rdr.add(self.fsmtypes.container)
    if not rdr.execute():
      print 'Failed to read FSM info.',self.manager.name()
      print self.name+'_'+match,self.names.container.size(),self.slots

  # ===========================================================================
  def _optsFile(self,name,type):
    return name+'.opts'
  # ===========================================================================
  def _fifoName(self,task):
    return '/tmp/'+task[7]+'.fifo'
  # ===========================================================================
  def _fsmLookup(self,dp,type):
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookupOriginal(self.name+'|'+self.name+'_'+self.match+'.'+dp,type)
    return obj
  
  # ===========================================================================
  def _tskLookup(self,dp,type):
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookupOriginal(self.name+'_'+self.match+'.'+dp,type)
    return obj
  
  # ===========================================================================
  def _makeTaskMap(self,tasks):
    m = {}
    nam = self.name+'_'
    tag = self.name+'|'+nam
    for i in tasks.keys():
      if debug: log(self.name+'> Task map:'+tag+' '+i+' '+i[5:9],timestamp=1)
      dpn = self.name+'|FSM_Tasks_FWDM.fsm.sendCommand'
      if not m.has_key(dpn): m[dpn] = []
      m[dpn].append(str(nam+i).upper())
    for i in tasks.keys():
      for j in tasks[i]:
        dp = tag+i+'FSM_DimTask_FWDM.fsm.sendCommand'
        if not m.has_key(dp): m[dp] = []
        val = self.enabled.container[j].name()
        val = val[val.find('|')+1:val.find('.')]
        m[dp].append(val.upper())
    if debug: print 'Taskmap:',m
    return m
  
  # ===========================================================================
  def _commitFSM(self, dpMap, action):
    dpv = PVSS.DataPointVector()
    length = 0
    for i in dpMap.keys():
      vals = ''
      for j in dpMap[i]: vals = vals+j.upper()+'|'
      if len(vals)>0:
        dpv.push_back(DataPoint(self.manager,DataPoint.original(i)))
        dpv.back().data = action+'/DEVICES(S)='+vals[:-1]
        if debug: log('%-12s %s'%(action,dpv.back().data))
        #log('%s>>>>>>> %-12s %s'%(i,action,dpv.back().data))
    self.writer.add(dpv)
    length = self.writer.length()
    if not self.writer.execute():
      log(self.name+'> PVSS commit failed! [%d datapoints]'%(length,),timestamp=1)
      return None
    return self
 
  # ===========================================================================
  def _commitFSM_OLD(self, dpMap, action):
    dpv = PVSS.DataPointVector()
    count = 0
    commit = 0
    length = 0
    while(len(dpMap)>0):
      for i in dpMap.keys():
        dpv.push_back(DataPoint(self.manager,DataPoint.original(i)))
        dpv.back().data = action+'/DEVICE(S)='+dpMap[i][0].upper()
        #print 'Send command:',dpv.back().name(),'cmd:',dpv.back().data
        count = count + 1
        if debug: log('%-12s %3d %3d %s'%(action, commit, count, dpv.back().data))
        #log('%-12s %3d %3d %s'%(action, commit, count, dpv.back().data))
        del dpMap[i][0]
        if len(dpMap[i])==0: del dpMap[i]
      self.writer.add(dpv)
      length = self.writer.length()
      if not self.writer.execute():
        log(self.name+'> PVSS commit failed! [%d datapoints]'%(length,),timestamp=1)
        return None
      commit = commit + 1
      dpv.clear()
    return self

  # ===========================================================================
  def printSlots(self):
    for i in self.tasks.keys():
      log('Task: %s -- %s'%(str(i),str(self.tasks[i])),timestamp=None)
      for j in self.tasks[i]:
        log('    Item: %s'%(self.enabled.container[j].name(),),timestamp=None)
    for i in self.taskSlots.keys():
      log('Slot: %s -- %s'%(str(i),str(self.taskSlots[i])),timestamp=None)
      for j in self.taskSlots[i].keys():
        log('    Item: %s'%(self.enabled.container[j].name(),),timestamp=None)
 
  # ===========================================================================
  def addNodeObject(self,name):
    self.enabled.container.push_back(DataPoint(self.manager,DataPoint.original(name+'.mode.enabled')))
    self.visible.container.push_back(DataPoint(self.manager,DataPoint.original(name+'.ui.visible')))
    self.labels.container.push_back(DataPoint(self.manager,DataPoint.original(name+'.ui.label')))
                                     
  # ===========================================================================
  def collectTaskSlots(self):
    self.tasks = {}
    self.taskSlots = {}
    self.nodeParents = {}
    nam = self.name+'|'+self.name+'_'
    num = self.enabled.container.size()
    tag = self.name+'_FWDM.mode.enabled'
    cnt_0 = 0
    cnt_1 = 1
    if debug: log('Entries:'+str(num)+' '+str(self.fsmtypes.container.size()))
    for i in xrange(num):
      if self.fsmtypes.container[i].data != "FSM_DimTask":
        if debug: log('skip:'+str(self.enabled.container[i].name()))
        continue
      node0 = self.enabled.container[i].name()
      key = self.name+'|'+self.name
      node1 = node0[node0.find(key)+len(key)+1:]
      node2 = node1[:node1.find('_')]
      if debug: log('node2:'+node2+' '+node0)
      if node0.find(tag) > 0: continue
      item = node1[node1.find('_')+1:]
      if item[:3]=="MON":
        node2 = node1[:node1.find('_')+4]
        item = item[4:]
      task_id = int(item[:3])
      if not self.tasks.has_key(node2):
        self.tasks[node2] = []
      #log('use['+i+']:'+str(self.enabled.container[i].name()))
      self.tasks[node2].append(i)
      if not self.taskSlots.has_key(node2):
        self.taskSlots[node2] = {}
      self.taskSlots[node2][i] = self.slots[node2][task_id]
      if not self.nodeParents.has_key(node2):
        if debug: log('Add node:'+node2)
        self.nodeParents[node2] = self.enabled.container.size()
        self.addNodeObject(nam+node2)
    if debug: log('Task nodes:'+str(self.tasks.keys())+' '+str(self.tasks))
    return (self.tasks,self.nodeParents)
    
  # ===========================================================================
  def enableObject(self, i, node, enabled, visible, label, type="NONE"):
    if debug:
      print 'Enable slot for:', i, i.__class__, enabled, visible, label
      print 'Enable:', self.enabled.container[i].name()
    self.enabled.container[i].data = int(enabled)
    self.writer.add(self.enabled.container[i])
    self.labels.container[i].data = label
    self.writer.add(self.labels.container[i])
    self.visible.container[i].data = int(visible)
    self.writer.add(self.visible.container[i])
        
  # ===========================================================================
  def _setupTask(self,which,node,name,type,inUse,prio,cmd,sysname,dimdns,nodename=None):
    if debug:
      print 'setupTask:',which,node,name,type,inUse,prio,dimdns
    if nodename is None:
      nodename = node
    i = self.taskSlots[node][which]
    self.names.container[i].data = name
    self.inUse.container[i].data = inUse
    self.nodes.container[i].data = nodename
    self.prio.container[i].data  = prio
    self.types.container[i].data = type
    self.sysName.container[i].data = sysname
    self.dimDns.container[i].data = dimdns
    self.cmds.container[i].data  = cmd
    self.writer.add(self.names.container[i])
    self.writer.add(self.inUse.container[i])
    self.writer.add(self.nodes.container[i])
    self.writer.add(self.prio.container[i])
    self.writer.add(self.types.container[i])
    self.writer.add(self.cmds.container[i])
    self.writer.add(self.dimDns.container[i])
    self.writer.add(self.sysName.container[i])

  # ===========================================================================
  def disableTasks(self,tasks_parents):
    tasks = tasks_parents[0]
    parents = tasks_parents[1]
    for i in parents.keys():
      self.enableObject(parents[i],i,-1,0,'Disabled')
    for i in tasks.keys():
      for j in tasks[i]:
        self.enableObject(j,i,-1,0,'Disabled')
        self.setupTask(j,node=i,name="NONE",type="NONE",inUse=0,prio=0,cmd="NONE",dimdns="NONE",sysname="NONE")
    return self

  # ===========================================================================
  def reset(self,tasks_parents):
    if tasks_parents:
      self.disableTasks(tasks_parents)
      return self._commitFSM(self._makeTaskMap(tasks_parents[0]),'DISABLE')
    return None

  # ===========================================================================
  def _allocateProcesses(self,processes,tasks_parents):
    if not tasks_parents:
      error(self.name+': No task list present!',timestamp=1)
      return None

    unused_tasks = copy.deepcopy(tasks_parents[0])
    parents = copy.deepcopy(tasks_parents[1])
    if debug: print 'tasks_parents[0]:',len(tasks_parents[0]),tasks_parents[0]
    if debug: print 'tasks_parents[1]:',len(tasks_parents[1]),tasks_parents[1]
    used_tasks = {}
    for n in processes.keys():
      if debug: print '--->',len(processes[n]),' tasks running on node:',n
      
    for (n,task_list) in processes.items():
      if debug: print 'allocateProcesses:',n,task_list
      if parents.has_key(n) and len(task_list) > 0:
        itm1 = parents[n]
        self.enableObject(itm1,n,1,1,n)
        del parents[n]
      used_tasks[n] = []
      for task in task_list:
        if unused_tasks.has_key(n) and len(unused_tasks[n]) > 0:
          i = unused_tasks[n][0]
          if debug: print 'Task:',task
          self.enableObject(i,n,1,1,task[2])
          self.configureTask(n,i,task)
          used_tasks[n].append(i)
          del unused_tasks[n][0]
          #print 'Task:',len(used_tasks[n]),task
        else:
          error(self.name+': No free task slots present for node '+str(n)+' [FSM_DimTask device units]!',timestamp=1)
          error(self.name+': The number of FSM_DimTask device units per node is a hard limit.',timestamp=1)
          error(self.name+': However: Is it possible a node in this system is disabled?',timestamp=1)
          if not unused_tasks.has_key(n):
            error(self.name+': Node '+str(n)+' cannot be found in the list and seems to have no device slots.',timestamp=1)
          else:
            error(self.name+': No more slots/device units for node '+str(n)+' present.',timestamp=1)
          if debug: print 'Used   tasks:', len(used_tasks), used_tasks
          if debug: print 'Unused tasks:', len(unused_tasks), unused_tasks
          return None
    #self.nodeParents = parents
    #self.tasks = used_tasks
    return (used_tasks,unused_tasks)

  # ===========================================================================
  def _collectScriptPath(self,mgr):
    typ = mgr.typeMgr().type('JobOptionsTaskType')
    scripts = []
    if typ:
      o1 = PVSS.DpVectorActor(mgr)
      o2 = PVSS.DpVectorActor(mgr)
      o3 = PVSS.DpVectorActor(mgr)
      o1.lookupOriginal('JobOptionsTaskType_*.ScriptPath',typ)
      o2.lookupOriginal('JobOptionsTaskType_*.RunAs',typ)
      o3.lookupOriginal('JobOptionsTaskType_*.Detector',typ)
      w = mgr.devReader()
      w.add(o1.container)
      w.add(o2.container)
      w.add(o3.container)
      w.execute()
      for idx in xrange(len(o1.container)):
        i = o1.container[idx]
        j = o2.container[idx]
        k = o3.container[idx]
        n = i.name()[i.name().find('_')+1:]
        scripts.append((n[:n.find('.')],i.data,j.data,k.data))
    return scripts
    
  # ===========================================================================
  def _startupInfo(self,task_typ):
    " Return startup script of a given task type."
    script  = Params.gauditask_startscript
    account = 'online'
    detector = 'ANY'
    if not hasattr(self,'startupInfoData'):
      self.startupInfoData = {}
      # first look for defaults
      info = self._collectScriptPath(Systems.controlsMgr(Params.jobopts_system_name))
      for name,path,account,detector in info: self.startupInfoData[name] = (path,account,detector)
      # now look in project specific values/overrides
      info = self._collectScriptPath(self.manager)
      for name,path.account,detector in info: self.startupInfoData[name] = (path,account,detector)
    if self.startupInfoData.has_key(task_typ):
      return self.startupInfoData[task_typ]
    return (script,account,detector)

  # ===========================================================================
  def setStartupInfoData(self,info):
    self.startupInfoData = info
    
  # ===========================================================================
  def _configureTask(self, fsm_node, item, task):
    print "Configure single task object",task
    node    = task[0]
    utgid   = task[1]
    sysname = task[6]
    dimdns  = task[5]
    type    = task[3]
    clazz   = task[4]
    slice   = task[7]
    script,account,detector = self.startupInfo(type)
    if len(account) == 0: account = 'online'
    opts    = self.optionsFile(utgid,type)
    fifo = '/tmp/logSrv.fifo'
    # cmd = sysname+'#-E /tmp/logGaudi.fifo -O /tmp/logGaudi.fifo '+\
    fifo = self.fifoName(task)
    #fifo = '/tmp/'+slice+'.fifo'
    #cmd = sysname+'#-e -o'+\
    cmd = sysname+'#-E '+fifo+' -O '+fifo+\
          ' -c -u '+utgid+' -n '+account+\
          ' -D LOGFIFO='+fifo+\
          ' -D TASKTYPE='+type+\
          ' -D TASKCLASS='+clazz+\
          ' -D PARTITION='+self.info.detectorName()
    if len(detector)>0: cmd = cmd + ' -D DETECTOR='+detector
    for t in task:
      if t[:11] == 'NBOFSLAVES=':
        cmd = cmd + ' -D ' + t
      if t[:7] == 'FORKEE=':
        cmd = 'FORKEE'
    if cmd != 'FORKEE':
      cmd = cmd + ' '+script+' '+opts+' '+clazz+' '+type+' '+dimdns
    self.setupTask(item,node=fsm_node,name=utgid,type=type,inUse=1,prio=0,cmd=cmd,sysname=sysname,dimdns=dimdns,nodename=node)
    return self

  # ===========================================================================
  def commit(self,tasks):
    "Commit all the writer's datapoints."
    if not self.writer.execute():
      log(self.name+': PVSS commit failed!',timestamp=1)
      return None
    used = tasks[0]
    unused = tasks[1]
    if self._commitFSM(self._makeTaskMap(unused),'DISABLE'):
      return self._commitFSM(self._makeTaskMap(used),'ENABLE')
    return None
  
# =============================================================================
#
# CLASS: BlockSlotPolicy
#
# =============================================================================
class BlockSlotPolicy(SlotPolicy):
  """ Class to implement the block slot allocation policy.

      The block allocation policy works as follows:
      1) get the node with the least occupation
      2) allocate a maximum of per_node slots on this node
      3) goto 1 until the required number of slots is
         allocated.

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, debug=1):
    SlotPolicy.__init__(self, debug)

  # ===========================================================================
  def allocateSlices(self,number,per_node,slices):
    freeSlots = self.collectFreeSlots(slices)
    if self.debug: print 'Free slots:',freeSlots
    num = 0
    alloc_slots = {}
    for i in xrange(number):
      node = self.leastOccupied(freeSlots)
      if node is not None:
        slots = freeSlots[node]
        for j in xrange(per_node):
          if num<number and len(slots) > 0:
            if not alloc_slots.has_key(node): alloc_slots[node] = []
            alloc_slots[node].append(slots.pop())
            num = num + 1
    if self.debug:
      print 'Allocated %d slots on %d nodes:'%(num,len(alloc_slots))
      for node,slots in alloc_slots.items():
        print 'Node:%-12s  Slots:%s'%(node,slots)
    return (num,alloc_slots)

# =============================================================================
#
# CLASS: RoundRobinSlotPolicy
#
# =============================================================================
class RoundRobinSlotPolicy(SlotPolicy):
  """ Class to implement the block slot allocation policy.

      The block allocation policy works as follows:
      1) get the first node and allocate a slice
      2) get the next node and allocate a slice ....
      3) if only one node is left, allocate the rest on this node

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, debug=1):
    SlotPolicy.__init__(self, debug)

  # ===========================================================================
  def allocateSlices(self,number,per_node,slices):
    freeSlots = self.collectFreeSlots(slices)
    if self.debug: print 'Free slots:',freeSlots
    num = 0
    alloc_slots = {}
    last_node = ''
    for i in xrange(number):
      for node,slots in freeSlots.items():
        if node != last_node:
          last_node = node
          if not alloc_slots.has_key(node): alloc_slots[node] = []
          print 'Debug:',node,slots
          alloc_slots[node].append(slots.pop())
          num = num + 1
          break
        
    for i in xrange(number-num):
      node = self.leastOccupied(freeSlots)
      if node is not None:
        slots = freeSlots[node]
        for j in xrange(per_node):
          if num<number and len(slots) > 0:
            if not alloc_slots.has_key(node): alloc_slots[node] = []
            alloc_slots[node].append(slots.pop())
            num = num + 1

    self.debug = True
    if self.debug:
      print 'RoundRobinSlotPolicy> Allocated %d slots on %d nodes:'%(num,len(alloc_slots))
      for node,slots in alloc_slots.items():
        print 'Node:%-12s  Slots:%s'%(node,slots)
    return (num,alloc_slots)

# =============================================================================
#
# CLASS: AllSlotPolicy
#
# =============================================================================
class AllSlotPolicy(SlotPolicy):
  """ Class to implement the block slot allocation policy.

      The block allocation policy works as follows:
      1) get the node with the minimal occupation
      2) allocate a maximum of slots on this node
      3) goto 1 until a node with sufficient slots is found

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, debug=1):
    SlotPolicy.__init__(self, debug)

  # ===========================================================================
  def allocateSlices(self,number,per_node,slices):
    freeSlots = self.collectFreeSlots(slices)
    if self.debug: print 'Free slots:',freeSlots
    occ = self.occupation(freeSlots)
    print freeSlots
    if len(occ) > 0:
      for size,node in occ.items():
        print 'allocateSlices:',node,size
      for size,node in occ.items():
        if size>=number:
          slots = freeSlots[node]
          alloc_slots = {node: []}
          for i in xrange(number):
            alloc_slots[node].append(slots.pop())
          return (number,alloc_slots)
    return (0,{})

# =============================================================================
#
# CLASS: Allocator
#
# =============================================================================
class Allocator(StreamingDescriptor):
  """ @class Control

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, info_interface, policy=(BlockSlotPolicy,BlockSlotPolicy)):
    """ Default constructor
        @param  manager         Reference to PVSS ControlManager
        @param  name            Name of the Streaming control datapoint
        @param  info_interface  Creator for RunInfo interpreter
        @param  policy          Slot llocation policy object

        @return reference to initialized object
    """
    StreamingDescriptor.__init__(self,manager,name)
    self.recvAllocationPolicy = policy[0]
    self.strmAllocationPolicy = policy[1]
    self.infoInterface        = info_interface
    self.fsmManip             = FSMmanip
    self.recv_slots_per_node  = 2
    self.strm_slots_per_node  = 1

  # ===========================================================================
  def showSetup(self):
    log('%s> system:             %s'%(self.name,self.manager.name()),timestamp=1)
    log('%s> recv_slots_per_node:%s'%(self.name,self.recv_slots_per_node),timestamp=1)
    log('%s> strm_slots_per_node:%s'%(self.name,self.strm_slots_per_node),timestamp=1)
    return self
  # ===========================================================================
  def _selectSlices(self, slices, all_slices):
    """Select slices from free pool
       @param slices     allocaed slices
       @param all_slices POOL of free slices

       @return Tuple (slots,nodes,remaining) returning std.vector('std::string')
               with used slots, used nodes, remaining free slots
    """
    tmp  = []
    nod = {}
    slots = std.vector('std::string')()
    nodes = std.vector('std::string')()
    remaining = std.vector('std::string')()
    for n,s in slices.items():
      for j in s:
        tmp.append(j)
        nod[n] = 1
        slots.push_back(j)
    for i in all_slices:
      used = None
      for k in tmp:
        if k == i:
          used = 1
          tmp.remove(k)
          break
      if used is None:
        remaining.push_back(i)
    for i in nod.keys():
      nodes.push_back(i)
    return (slots,nodes,remaining)

  # ===========================================================================
  def allocPartition(self,rundp_name,partition):
    p = self.inUse.data
    n = self.active.data
    for i in xrange(len(p)):
      if p[i] == 0:
        v0 = std.vector('int')()
        for j in xrange(len(p)):
          if i==j: v0.push_back(1)
          else:    v0.push_back(p[j])
        self.inUse.data = v0
        n0 = std.vector('std::string')()
        for j in xrange(len(n)):
          if i==j: n0.push_back(partition)
          else:    n0.push_back(n[j])
        slice = self.partitionName(i)
        self.active.data = n0
        self.writer.add(self.inUse)
        self.writer.add(self.active)
        self.active.data.push_back(slice)
        info = PartitionInfo.PartitionInfo(self.manager,slice).load()
        info.initialize(rundp_name,partition,self.writer)
        if self.writer.execute():
          return info
        error('Failed to allocate partition:'+datapoint+' for '+partition)
        return None
    error('Failed to load storage configuration:'+self.name)
    return None

  # ===========================================================================
  def freePartition(self, partition):
    if partition is not None:
      dp, nam, id = partition
      dp.data = ''
      self.writer.add(dp)
      use = DataPoint(self.manager,DataPoint.original(nam+'.InUse'))
      use.data = 0
      self.writer.add(use)
      old_active = self.active.data
      old_inuse = self.inUse.data
      inuse = std.vector('int')()
      active = std.vector('std::string')()
      for j in xrange(len(old_inuse)):
        if j == id:
          inuse.push_back(0)
        else:
          inuse.push_back(old_inuse[j])
      for j in xrange(len(old_active)):
        if old_active[j] != nam:
          active.push_back(old_active[j])
      self.inUse.data = inuse
      self.active.data = active
      self.writer.add(self.active)
      self.writer.add(self.inUse)
      if self.writer.execute():
        return self
      error('Failed to update partition information for:'+nam)
    return None

  # ===========================================================================
  def allocateSlots(self,rundp_name,partition,num_recv_slots,recv_slots_per_node,num_strm_slots,strm_slots_per_node):
    """
    Allocate slots in the receiving and streaming layer of the storage system.
    Note: All input data must already be read.

    @param  partition           Partition name
    @param  num_recv_slots      Number of slots in the receiving layer to be allocated
    @param  recv_slots_per_node Number of slots to be allocated at once per node (if possible)
    @param  num_strm_slots      Number of slots in the streaming layer to be allocated
    @param  strm_slots_per_node Number of slots to be allocated at once per node (if possible)

    @return None on failure, on Succes tuple (all_recv_slots,all_strm_slots) with allocated slots
    """
    policy = self.recvAllocationPolicy(False)
    recvSlices = self.recvSlices.data
    num,slices = policy.allocateSlices(num_recv_slots,recv_slots_per_node,recvSlices)
    if num < num_recv_slots:
      error('Failed to allocate sufficient RECEIVE slots. Got only %s slots, but requested %d.'%(num,num_recv_slots),timestamp=1,type=PVSS.OUTOFUSERRANGE)
      return None
    all_recv_slots,all_recv_nodes,rem_recv_slices = self._selectSlices(slices,recvSlices)

    policy = self.strmAllocationPolicy(False)
    strmSlices = self.strmSlices.data
    num,slices = policy.allocateSlices(num_strm_slots,strm_slots_per_node,strmSlices)
    if num < num_strm_slots:
      msg = 'Failed to allocate sufficient STREAMER slots. Got only %s slots, but requested %d.'
      error(msg%(num,num_strm_slots),timestamp=1,type=PVSS.OUTOFUSERRANGE)
      return None
    all_strm_slots,all_strm_nodes,rem_strm_slices = self._selectSlices(slices,strmSlices)

    part_info = self.allocPartition(rundp_name,partition)
    if part_info is not None:
      self.recvSlices.data = rem_recv_slices
      self.strmSlices.data = rem_strm_slices
      part_info.datapoints[PartitionInfo.RECVNODES].data  = all_recv_nodes   # RecvNodes
      part_info.datapoints[PartitionInfo.RECVSLICES].data = all_recv_slots   # RecvSlices
      part_info.datapoints[PartitionInfo.STRMNODES].data  = all_strm_nodes   # StreamNodes
      part_info.datapoints[PartitionInfo.STRMSLICES].data = all_strm_slots   # StreamSlices
      self.writer.add(self.recvSlices)
      self.writer.add(self.strmSlices)
      self.writer.add(part_info.datapoints[PartitionInfo.RECVNODES])
      self.writer.add(part_info.datapoints[PartitionInfo.RECVSLICES])
      self.writer.add(part_info.datapoints[PartitionInfo.STRMNODES])
      self.writer.add(part_info.datapoints[PartitionInfo.STRMSLICES])
      if self.writer.execute():
        return part_info
    error('Failed to allocate partition:'+partition+' RunDP:'+rundp_name,timestamp=1)
    return None
  
  # ===========================================================================
  def allocate(self,rundp_name,partition,recv_slots_per_node=None,strm_slots_per_node=None):
    """
    Allocate slots in the receiving and streaming layer of the storage system.
    Note: All input data must already be read.

    @param  partition           Partition name
    @param  recv_slots_per_node Number of receiving layer slots to be
                                allocated at once per node (if possible)
    @param  strm_slots_per_node Number of streaming layer slots to be
                                allocated at once per node (if possible)

    @return None on failure, on Succes tuple (all_recv_slots,all_strm_slots) with allocated slots
    """
    if self.recv_slots_per_node is not None:
      recv_slots_per_node = self.recv_slots_per_node
    if self.strm_slots_per_node is not None:
      strm_slots_per_node = self.strm_slots_per_node

    start = time.time()
    if debug: print 'Starting action:',start,' recv_slots_per_node:',recv_slots_per_node,' strm_slots_per_node:',strm_slots_per_node
    print 'Starting action:',start,
    existing = self.getPartition(partition)
    if existing is not None:
      error('[Partition already allocated] Cannot allocate partition for:'+partition)
      msg = '[WAS_ALRDY_ALLOCATED] Ignore Error on allocate on request from Clara'
      PVSS.error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
      return 'WAS_ALRDY_ALLOCATED' # None    # Do not return error. Allocator must stay READY
    info_obj = self.infoInterface.create(rundp_name,partition).load()
    if not info_obj.doStreaming():
      warning('Use flag is not set. No need to allocate resources for partition:'+partition)
      msg = '[NO_STREAMFLG_SET] Ignore Error on allocate on request from Clara'
      PVSS.error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
      return 'NO_STREAMFLG_SET' # None    # Do not return error. Allocator must stay READY
    
    self.load()
    nLayer1Slots = info_obj.numLayer1Slots()
    nLayer2Slots = info_obj.numLayer2Slots()
    print 'allocate: nLayer1Slots:',nLayer1Slots,' nLayer2Slots:',nLayer2Slots
    part_info = self.allocateSlots(rundp_name,partition,nLayer1Slots,recv_slots_per_node,nLayer2Slots,strm_slots_per_node)

    if part_info is not None:   # Allocation was successful: Now update Info table+tasks
      pinfo = PartitionInfo.PartitionInfo(self.manager,part_info.name).load()
      fsm_manip = self.fsmManip(pinfo,'_FwFsmDevice',match='*')
      slots = fsm_manip.collectTaskSlots()
      if slots:
        log('Cleaning task slots for '+partition,timestamp=1)
        fsm_manip.reset(slots)
      return part_info.name
    self.free(rundp_name,partition)
    error('Failed to allocate slots of type:'+self.name+' for partition:'+partition,timestamp=1)
    msg = '[FAIL_ALLOC_SLOTS] Ignore Error on allocate on request from Clara'
    PVSS.error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
    return 'FAIL_ALLOC_SLOTS' # None    # Do not return error. Allocator must stay READY

  # ===========================================================================
  def configure(self,rundp_name,partition):
    "Configure partition content after all information is set."
    existing = self.getPartition(partition)
    if existing is None:
      error('[Partition not allocated] Cannot find partition for:'+partition)
      return None
    info_obj = self.infoInterface.create(rundp_name,partition).load()
    if not info_obj.doStreaming():
      warning('Use flag is not set. No need to allocate resources for partition:'+partition)
      return None
    dp, slice, id = existing
    part_info = PartitionInfo.PartitionInfo(self.manager,slice).load()
    tasks = info_obj.defineTasks(part_info)
    if tasks:
      fsm_manip = self.fsmManip(part_info,'_FwFsmDevice',match='*')
      res = fsm_manip.collectTaskSlots()
      if res:
        fsm_manip.disableTasks(res)
        used_tasks = {}
        for node in tasks.keys():
          for task in tasks[node]:
            script,account,detector = fsm_manip.startupInfo(task[3])
            if info_obj.isDetectorUsed(detector):
              if not used_tasks.has_key(node): used_tasks[node]=[]
              used_tasks[node].append(task)
        #
        #
        res = fsm_manip.allocateProcesses(used_tasks,res)
        #
        # Things went wrong
        if res is None:
          return None
        #
        # Now commit the transaction
        if fsm_manip.commit(res) is None:
          return None
        #
        # Dump output
        self.infoInterface.showPartition(partition=part_info,extended=1)
        #
        # All OK
        return part_info.name
      error('No task slots found of type:'+self.name+' for partition:'+partition,timestamp=1)
      return None
    error('Failed to configure slots of type:'+self.name+' for partition:'+partition,timestamp=1)
    return None
  
  # ===========================================================================
  def free(self, rundp_name, partition):
    "Free partition and all related content."
    res = self.getPartition(partition)
    if res is not None:
      dp, nam, id = res
      part_info = PartitionInfo.PartitionInfo(self.manager,nam).load(self.reader)
      self.reader.add(self.inUse)
      self.reader.add(self.active)
      self.reader.add(self.recvSlices)
      self.reader.add(self.strmSlices)
      if self.reader.execute():
        #print 'Partition name:',nam, slice
        fsm_manip = self.fsmManip(part_info,'_FwFsmDevice',match='*')
        slots = fsm_manip.collectTaskSlots()
        if slots:
          log('Cleaning task slots for '+partition,timestamp=1)
          fsm_manip.reset(slots)
        slice = part_info.fsmSlice()
        for i in part_info.datapoints[4].data:
          self.recvSlices.data.push_back(i)
        for i in part_info.datapoints[6].data:
          self.strmSlices.data.push_back(i)
        part_info.clear()
        part_info.saveAll(self.writer)
        self.writer.add(self.recvSlices)
        self.writer.add(self.strmSlices)
        if self.freePartition(res):
          return slice
        error('[Cannot commit] Cannot free partition for '+partition,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
        return None
    error('[Partition not allocated] Cannot free partition for '+partition,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
    return 'WAS_NOT_ALLOCATED'

  # ===========================================================================
  def recover(self, rundp_name, partition):
    return self.free(rundp_name, partition)
  
  # ===========================================================================
  def recover_slice(self, rundp_name, partition):
    log('Recover '+partition+' runDP:'+rundp_name)
    return 'SUCCESS'  # self.free(rundp_name, partition)
  
  # ===========================================================================
  def showSummary(self, extended=None):
    actor = PVSS.DpVectorActor(self.manager)
    typ   = self.manager.typeMgr().type('StreamPartition')
    actor.lookupOriginal(self.name+'_Slice*.Name',typ)
    partitions = []
    log('Stream Control contains a total of '+str(len(actor.container))+' partitions',timestamp=1)
    for i in xrange(len(actor.container)):
      nam = actor.container[i].name()
      nam = nam[nam.find(':')+1:nam.find('.')]
      partitions.append(PartitionInfo.PartitionInfo(self.manager,nam))
    for i in partitions:
      self.reader.add(i.datapoints)
    self.load()
    StreamingDescriptor.show(self,extended)
    return partitions
  
  # ===========================================================================
  def show(self, extended=None):
    import Online.RunInfoClasses.Storage as Storage
    partitions = self.showSummary(1)
    info_obj = None
    for i in partitions:
      if extended and i.inUse():
        info_obj = self.infoInterface.create('LBECS:'+i.detectorName()+'_RunInfo',i.detectorName())
      else: info_obj = None
      self.infoInterface.showPartition(info_obj,i)
