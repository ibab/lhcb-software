import copy, time
import Online.PVSS    as PVSS
import Online.Utils   as Utils
from   Online.Streaming.SlotPolicy import SlotPolicy as SlotPolicy
from   Online.Streaming.PartitionInfo import PartitionInfo as PartitionInfo
from   Online.Streaming.StreamingDescriptor import StreamingDescriptor as StreamingDescriptor

log            = Utils.log
error          = PVSS.error
warning        = Utils.warning
printSlots     = Utils.printSlots
std            = PVSS.gbl.std
DeviceSensor   = PVSS.DeviceSensor
DeviceListener = PVSS.PyDeviceListener
PVSS.logPrefix = 'PVSS Control '

DataPoint = PVSS.DataPoint
debug = 0
NUM_INFRASTRUCTURE_TASKS = 5

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
  def __init__(self,partition,name,manager,type):
    self.partition = partition
    self.name    = name
    self.manager = manager
    self.reader  = self.manager.devReader()
    self.writer  = self.manager.devWriter()
    typ          = self.manager.typeMgr().type(type)
    tsk_typ      = self.manager.typeMgr().type('FSM_DimTask')
    self.enabled = self._fsmLookup('mode.enabled',typ)
    self.labels  = self._fsmLookup('ui.label',typ)
    self.visible = self._fsmLookup('ui.visible',typ)
    self.types   = self._fsmLookup('type',typ)
    self.tnode   = self._fsmLookup('tnode',typ)
    self.names   = self._tskLookup('Name',tsk_typ)
    self.inUse   = self._tskLookup('InUse',tsk_typ)
    self.nodes   = self._tskLookup('Node',tsk_typ)
    self.prio    = self._tskLookup('Priority',tsk_typ)
    self.types   = self._tskLookup('Type',tsk_typ)
    self.cmds    = self._tskLookup('FMC_Start',tsk_typ)
    self.slots = {}
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookup(DataPoint.original(self.name+'_*'),tsk_typ)
    for i in xrange(self.names.container.size()):
      nam = self.names.container[i].name()
      node = nam[nam.find(':')+1:]
      node = node.split('_')[2]
      if not self.slots.has_key(node): self.slots[node] = []
      self.slots[node].append(i)
    self.class0Tasks = {}
    self.class1Tasks = {}
    self.class0Parents = {}
    self.class1Parents = {}
    self.nodeParents = {}
    
  # ===========================================================================
  def _fsmLookup(self,dp,type):
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookup(DataPoint.original(self.name+'|'+self.name+'_*.'+dp),type)
    return obj
  
  # ===========================================================================
  def _tskLookup(self,dp,type):
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookup(DataPoint.original(self.name+'_*.'+dp),type)
    return obj
  
  # ===========================================================================
  def _makeTaskMap(self):
    m = {}
    nam = self.name+'_'
    tag = self.name+'|'+nam
    for i in self.class0Tasks.keys():
      if debug: log(self.name+'> Task map:'+tag+' '+i+' '+i[5:9],timestamp=1)
      if i[5:9]=="strm":
        dpn = self.name+'|FSM_StrmNode_FWDM.fsm.sendCommand'
      else:
        dpn = self.name+'|FSM_RecvNode_FWDM.fsm.sendCommand'
      if not m.has_key(dpn): m[dpn] = []
      m[dpn].append(str(nam+i).upper())
      dp0 = tag+i+'FSM_Class0_FWDM.fsm.sendCommand'
      if not m.has_key(dp0): m[dp0] = []
      m[dp0].append(str(nam+i).upper()+'_CLASS0')
      dp1 = tag+i+'FSM_Class1_FWDM.fsm.sendCommand'
      if not m.has_key(dp1): m[dp1] = []
      m[dp1].append(str(nam+i).upper()+'_CLASS1')
    for i in self.class0Tasks.keys():
      for j in self.class0Tasks[i]:
        dp = tag+i+'_Class0FSM_DimTask_FWDM.fsm.sendCommand'
        if not m.has_key(dp): m[dp] = []
        val = self.enabled.container[j].name()
        val = val[val.find('|')+1:val.find('.')]
        m[dp].append(val.upper())
    for i in self.class1Tasks.keys():
      for j in self.class1Tasks[i]:
        dp = tag+i+'_Class1FSM_DimTask_FWDM.fsm.sendCommand'
        if not m.has_key(dp): m[dp] = []
        val = self.enabled.container[j].name()
        val = val[val.find('|')+1:val.find('.')]
        m[dp].append(val.upper())
    return m
  
  # ===========================================================================
  def _commitFSM(self, dpMap, action):
    dpv = PVSS.DataPointVector()
    count = 0
    commit = 0
    while(len(dpMap)>0):
      for i in dpMap.keys():
        dpv.push_back(DataPoint(self.manager,DataPoint.original(i)))
        dpv.back().data = action+'/DEVICE(S)='+dpMap[i][0].upper()
        count = count + 1
        if debug: log('%3d %3d %s'%(commit, count, dpv.back().data))
        del dpMap[i][0]
        if len(dpMap[i])==0: del dpMap[i]
      self.writer.add(dpv)
      if not self.writer.execute():
        log(self.name+'> PVSS commit failed!',timestamp=1)
        return None
      commit = commit + 1
      dpv.clear()
    return self
  
  # ===========================================================================
  def addNodeObject(self,name):
    self.enabled.container.push_back(DataPoint(self.manager,DataPoint.original(name+'.mode.enabled')))
    self.visible.container.push_back(DataPoint(self.manager,DataPoint.original(name+'.ui.visible')))
    self.labels.container.push_back(DataPoint(self.manager,DataPoint.original(name+'.ui.label')))
                                     
  # ===========================================================================
  def collectTaskSlots(self):
    self.class0Tasks = {}
    self.class1Tasks = {}
    self.taskSlots = {}
    self.class0Parents = {}
    self.class1Parents = {}
    self.nodeParents = {}
    nam = self.name+'|'+self.name+'_'
    num = self.enabled.container.size()
    tag = self.name+'_FWDM.mode.enabled'
    lag_len = len(tag)
    cnt_0 = 0
    cnt_1 = 1
    for i in xrange(num):
      node0 = self.enabled.container[i].name()
      key = self.name+'|'+self.name
      node1 = node0[node0.find(key)+len(key)+1:]
      node2 = node1[:node1.find('_')]
      if debug: log('node2:'+node2+' '+node0)
      if node0.find(tag) > 0: continue
      item = node1[node1.find('_')+1:]
      task_id = int(item[:3])
      if task_id < NUM_INFRASTRUCTURE_TASKS:
        if not self.class0Tasks.has_key(node2):
          self.class0Tasks[node2] = []
        self.class0Tasks[node2].append(i)
      else:
        if not self.class1Tasks.has_key(node2):
          self.class1Tasks[node2] = []
        self.class1Tasks[node2].append(i)
      if not self.taskSlots.has_key(node2):
        self.taskSlots[node2] = {}
      self.taskSlots[node2][i] = self.slots[node2][task_id]
      if not self.nodeParents.has_key(node2):
        if debug: log('Add node:'+node2)
        self.nodeParents[node2] = self.enabled.container.size()
        self.addNodeObject(nam+node2)
        self.class0Parents[node2] = self.enabled.container.size()
        self.addNodeObject(nam+node2+'_Class0')
        self.class1Parents[node2] = self.enabled.container.size()
        self.addNodeObject(nam+node2+'_Class1')
      
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
  def setupTask(self,which,node,name,type,inUse,prio,cmd):
    if debug:
      print 'setupTask:',which,node,name,type,inUse,prio
    i = self.taskSlots[node][which]
    self.names.container[i].data = name
    self.inUse.container[i].data = inUse
    self.nodes.container[i].data = node
    self.prio.container[i].data  = prio
    self.types.container[i].data = type
    self.cmds.container[i].data  = cmd
    self.writer.add(self.names.container[i])
    self.writer.add(self.inUse.container[i])
    self.writer.add(self.nodes.container[i])
    self.writer.add(self.prio.container[i])
    self.writer.add(self.types.container[i])
    self.writer.add(self.cmds.container[i])
    
  # ===========================================================================
  def reset(self):
    for i in self.nodeParents.keys():
      self.enableObject(self.nodeParents[i],i,-1,0,'Disabled')
    for i in self.class0Parents.keys():
      self.enableObject(self.class0Parents[i],i,-1,0,'Disabled')
    for i in self.class1Parents.keys():
      self.enableObject(self.class1Parents[i],i,-1,0,'Disabled')
    for i in self.class0Tasks.keys():
      for j in self.class0Tasks[i]:
        self.enableObject(j,i,-1,0,'Disabled')
        self.setupTask(j,node=i,name="NONE",type="NONE",inUse=0,prio=0,cmd="NONE")
    for i in self.class1Tasks.keys():
      for j in self.class1Tasks[i]:
        self.enableObject(j,i,-1,0,'Disabled')
        self.setupTask(j,node=i,name="NONE",type="NONE",inUse=0,prio=0,cmd="NONE")
    res = self.writer.execute()
    return self._commitFSM(self._makeTaskMap(),'DISABLE')

  # ===========================================================================
  def allocateInfrastructure(self,processes):
    cl0 = copy.deepcopy(self.class0Tasks)
    self.class0Tasks = {}
    # print cl0
    for n in self.nodeParents.keys():
      print 'Node parent:',n
    for n in processes.keys():
      if debug: print '--->',len(processes[n]),' Class 0 tasks running on node:',n
    for (n,task_list) in processes.items():
      if debug: print 'allocateInfrastructure:',n
      if self.nodeParents.has_key(n):
        if len(task_list) > 0:
          itm1 = self.nodeParents[n]
          itm2 = self.class0Parents[n]
          if self.enabled.container[itm1].data != 1:
            self.enableObject(itm1,n,1,1,n)
          if self.enabled.container[itm2].data != 1:
            self.enableObject(itm2,n,1,1,'Class0')
      self.class0Tasks[n] = []
      for task in task_list:
        if cl0.has_key(n) and len(cl0[n]) > 0:
          i = cl0[n][0]
          if debug: print 'Task:',i,task
          self.enableObject(i,n,1,1,task[2])
          cmd = '-e -o -u '+task[1]+' -D PARTITION='+self.partition+' /home/frankm/runGaudi.sh '+task[1]+'.opts Class0 '+task[3]
          self.setupTask(i,node=n,name=task[1],type=task[3],inUse=1,prio=0,cmd=cmd)
          self.class0Tasks[n].append(i)
          del cl0[n][0]
        else:
          error(self.name+': No task slots for Class 0 present!',timestamp=1)
          if not cl0.has_key(n): error(self.name+': Key '+str(n)+' canot be found.',timestamp=1)
          else:                  error(self.name+': No slots for key '+str(n)+' present.',timestamp=1)
          return None
          
    return self

  # ===========================================================================
  def allocateProcesses(self,processes):
    cl1 = copy.deepcopy(self.class1Tasks)
    self.class1Tasks = {}
    for n in processes.keys():
      if debug: print '--->',len(processes[n]),' Class 1 tasks running on node:',n
    for (n,task_list) in processes.items():
      if debug: print 'allocateProcesses:',n,task_list
      if self.nodeParents.has_key(n):
        if len(task_list) > 0:
          itm1 = self.nodeParents[n]
          itm2 = self.class1Parents[n]
          if self.enabled.container[itm1].data != 1:
            self.enableObject(itm1,n,1,1,n)
          if self.enabled.container[itm2].data != 1:
            self.enableObject(itm2,n,1,1,'Class1')
      if self.nodeParents.has_key(n):
        if len(task_list) > 0:
          itm1 = self.nodeParents[n]
          itm2 = self.class1Parents[n]
          if self.enabled.container[itm1].data != 1:
            self.enableObject(itm1,n,1,1,n)
          if self.enabled.container[itm2].data != 1:
            self.enableObject(itm2,n,1,1,'Class1')


      self.class1Tasks[n] = []
      for task in task_list:
        if cl1.has_key(n) and len(cl1[n]) > 0:
          i = cl1[n][0]
          if debug: print 'Task:',task
          self.enableObject(i,n,1,1,task[2])
          cmd = '-e -o -u '+task[1]+' -D PARTITION='+self.partition+' /home/frankm/runGaudi.sh '+task[1]+'.opts Class1 '+task[3]
          self.setupTask(i,node=n,name=task[1],type=task[3],inUse=1,prio=0,cmd=cmd)
          self.class1Tasks[n].append(i)
          del cl1[n][0]
        else:
          error(self.name+': No task slots for Class 1 present!',timestamp=1)
          if not cl1.has_key(n): error(self.name+': Key '+str(n)+' canot be found.',timestamp=1)
          else:                  error(self.name+': No slots for key '+str(n)+' present.',timestamp=1)
          return None
    return self

  # ===========================================================================
  def commit(self):
    if not self.writer.execute():
      log(self.name+': PVSS commit failed!',timestamp=1)
      return None
    return self._commitFSM(self._makeTaskMap(),'ENABLE')

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
# CLASS: Control
#
# =============================================================================
class Control(StreamingDescriptor,DeviceListener):
  """ @class Control

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, info_interface):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Streaming control datapoint

        @return reference to initialized object
    """
    StreamingDescriptor.__init__(self,manager,name)
    DeviceListener.__init__(self,self,manager)
    self.control  = self.get('Command')
    self.state  = self.get('State')
    PVSS.info(name+': Listen to '+self.control.name(),1)
    PVSS.info(name+': Answer to '+self.state.name(),1)
    self.sensor = DeviceSensor(manager,self.control)
    self.recvAllocationPolicy = BlockSlotPolicy
    self.strmAllocationPolicy = BlockSlotPolicy
    self.infoInterface        = info_interface

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
  def _usePartition(self,datapoint,partition):
    log('Using partition: '+partition,timestamp=1)
    use = DataPoint(self.manager,DataPoint.original(datapoint+'.InUse'))
    use.data = 1
    self.writer.add(use)
    nam = DataPoint(self.manager,DataPoint.original(datapoint+'.Name'))
    nam.data = partition
    self.writer.add(nam)
    fsm = DataPoint(self.manager,DataPoint.original(datapoint+'.FSMSlice'))
    fsm.data = self.name+"_Slice"+datapoint[len(datapoint)-2:];
    self.writer.add(fsm)
    
    self.active.data.push_back(datapoint)
    self.writer.add(self.active)
    if self.writer.execute():
      return datapoint
    error('Failed to allocate partition:'+datapoint+' for '+partition)
    return None

  # ===========================================================================
  def allocPartition(self,partition):
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
        self.active.data = n0
        self.writer.add(self.inUse)
        self.writer.add(self.active)
        return self._usePartition(self.partitionName(i),partition)
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
  def allocateSlots(self,partition,num_recv_slots,recv_slots_per_node,num_strm_slots,strm_slots_per_node):
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

    dp = self.allocPartition(partition)
    if dp is not None:
      part_info = PartitionInfo(self.manager,dp)
      self.recvSlices.data = rem_recv_slices
      self.strmSlices.data = rem_strm_slices
      part_info.datapoints[3].data = all_recv_nodes   # RecvNodes
      part_info.datapoints[4].data = all_recv_slots   # RecvSlices
      part_info.datapoints[5].data = all_strm_nodes   # StreamNodes
      part_info.datapoints[6].data = all_strm_slots   # StreamSlices
      self.writer.add(self.recvSlices)
      self.writer.add(self.strmSlices)
      self.writer.add(part_info.datapoints[3])
      self.writer.add(part_info.datapoints[4])
      self.writer.add(part_info.datapoints[5])
      self.writer.add(part_info.datapoints[6])
      if self.writer.execute():
        return (dp,all_recv_nodes,all_recv_slots,all_strm_nodes,all_strm_slots)
    return None
  
  # ===========================================================================
  def allocate(self,rundp_name,partition,recv_slots_per_node,strm_slots_per_node):
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
    start = time.time()
    if debug: print 'Starting action:',start
    existing = self.getPartition(partition)
    if existing is not None:
      error('[Partition already allocated] Cannot allocate partition for:'+partition)
      return None
    info_obj = self.infoInterface.create(rundp_name,partition).load()
    if info_obj.flag.data == 0:
      warning('Use flag is not set. No need to allocate resources for partition:'+partition)
      return None
    nLayer1Slots = info_obj.numLayer1Slots()
    nLayer2Slots = info_obj.numLayer2Slots()
    res = self.allocateSlots(partition,nLayer1Slots,recv_slots_per_node,nLayer2Slots,strm_slots_per_node)

    # This in configure:
    if res is not None:
      # Allocation was successful: Now update Info table+tasks
      dp,recv_nodes,recv_slots,strm_nodes,strm_slots = res
      log('allocateSlots took:'+str(time.time()-start)+' seconds')
      info_obj.clearTasks()
      log('clearTasks took:'+str(time.time()-start)+' seconds')
      if info_obj.defineTasks(recv_slots,strm_slots):
        slice = 'Slice'+dp[len(self.name+'_Partition_'):]
        log('info_obj.defineTasks(recv_slots,strm_slots) took:'+str(time.time()-start)+' seconds')
        class0_tasks,class1_tasks = info_obj.collectTasks({},{})
        for i,j in class0_tasks.items(): log('-->Class 0:'+str(i)+' with '+str(len(j))+' tasks')
        for i,j in class1_tasks.items():
          log('-->Class 1:'+str(i)+' with '+str(len(j))+' tasks')
          #for t in j: log('   -->Class 1 Tark:'+str(t))
        if self.allocateFSM(partition,slice,class0_tasks,class1_tasks) is None:
          self.free(rundp_name,partition)
          return None
        if debug: log('allocateFSM(slice,info_obj) took:'+str(time.time()-start)+' seconds')
        return (self.name+'_'+slice,recv_slots,strm_slots)
    error('Failed to allocate slots for partition:'+self.name)
    return None
  
  # ===========================================================================
  def allocateFSM(self,partition,slice,class0_tasks,class1_tasks):
    slice_name = self.name+'_'+slice
    fsm_manip = FSMmanip(partition,slice_name,self.manager,'_FwFsmDevice')
    fsm_manip.collectTaskSlots()
    fsm_manip.reset()
    if fsm_manip.allocateInfrastructure(class0_tasks) is None:
      return None
    if fsm_manip.allocateProcesses(class1_tasks) is None:
      return None
    return fsm_manip.commit()

  # ===========================================================================
  def free(self, rundp_name, partition):
    "Free partition and all related content."
    res = self.getPartition(partition)
    if res is not None:
      dp, nam, id = res
      part_info = PartitionInfo(self.manager,nam)
      if part_info.load(self.reader):
        slice = part_info.fsmSlice()
        #print 'Partition name:',nam, slice
        for i in part_info.datapoints[4].data:
          self.recvSlices.data.push_back(i)
        for i in part_info.datapoints[6].data:
          self.strmSlices.data.push_back(i)
        part_info.datapoints[0].data = ''
        part_info.datapoints[1].data = 0
        part_info.datapoints[2].data = ''
        part_info.datapoints[3].data.clear()
        part_info.datapoints[4].data.clear()
        part_info.datapoints[5].data.clear()
        part_info.datapoints[6].data.clear()
        self.writer.add(self.recvSlices)
        self.writer.add(self.strmSlices)
        self.writer.add(part_info.datapoints[0])
        self.writer.add(part_info.datapoints[1])
        self.writer.add(part_info.datapoints[2])
        self.writer.add(part_info.datapoints[3])
        self.writer.add(part_info.datapoints[4])
        self.writer.add(part_info.datapoints[5])
        self.writer.add(part_info.datapoints[6])
        inst = self.infoInterface.create(rundp_name,partition)
        inst.load()
        if self.freePartition(res):
          wr = inst.manager.devWriter()
          inst.clearTasks(wr)
          if wr.execute():
            return (1,slice)
        error('[Cannot commit] Cannot free partition for '+partition,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
        return (0,None)
    error('[Partition not allocated] Cannot free partition for '+partition,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
    return (2,None)

  # ===========================================================================
  def showSummary(self, extended=None):
    actor = PVSS.DpVectorActor(self.manager)
    typ   = self.manager.typeMgr().type('StreamPartition')
    actor.lookup(DataPoint.original(self.name+'_Partition*.Name'),typ)
    partitions = []
    log('Stream Control contains a total of '+str(len(actor.container))+' partitions',timestamp=1)
    for i in xrange(len(actor.container)):
      nam = actor.container[i].name()
      nam = nam[nam.find(':')+1:nam.find('.')]
      partitions.append(PartitionInfo(self.manager,nam))
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
        info_obj = self.infoInterface.create(i.detectorName())
      else: info_obj = None
      self.infoInterface.showPartition(info_obj,i)
    
  # ===========================================================================
  def makeAnswer(self,status,msg):
    "Create answer object from status."
    m = status + msg
    # log('Answer:'+str(m))
    self.writer.clear()
    self.state.data = m
    self.writer.add(self.state)
    if self.writer.execute():
      return 1
    return 0
  
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1
  
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    cmd = ''
    try:
      nam = self.dp().name()
      cmd = self.dp().value().data()
      itms = cmd.split('/')
      if len(itms) == 4:
        command   = itms[0]
        storage   = itms[1]
        partition = itms[2]
        partID    = itms[3]
        runDpName = "ECS"+partition+":"+partition+"_RunInfo"
        answer = '/'+storage+'/'+partition+"/"+str(partID)
        result = None
        if storage == self.name:
          try:
            if command == "ALLOCATE":
              result = self.allocate(runDpName,partition,5,2)
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result[0])
            elif command == "DEALLOCATE":
              result = self.free(runDpName,partition)
              if result[0] == 1: result = result[1]
              else: result = None                   # partition was not allocated!
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result)
            elif command == "RECOVER":
              result = self.free(runDpName,partition)
              if result[0] == 1: result = result[1]
              elif result[0] == 2: result = "WAS_NOT_ALLOCATED"
              else: result = None                   # partition was not allocated!
            if result is not None:
              #print answer+'/'+result
              return self.makeAnswer('READY',answer+'/'+result)
            msg = 'The command:"'+cmd+'" failed. [Internal Error] '
            error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
            return self.makeAnswer('ERROR',answer)
          except Exception,X:
            error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.ILLEGAL_ARG)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
          except:
            error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1,type=PVSS.ILLEGAL_ARG)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
        msg = 'The command:"'+cmd+'" failed. [Bad Streaming System] '+storage
        error(msg,timestamp=1,type=PVSS.ILLEGAL_ARG)
        return self.makeAnswer('ERROR',answer)
      error('The command:"'+cmd+'" failed. [Insufficient parameters] ',timestamp=1,type=PVSS.ARG_MISSING)
      return 0
    except Exception,X:
      error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
      return 0
    except:
      error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
    return 0
 
  # ===========================================================================
  def run(self):
    self.sensor=PVSS.DeviceSensor(self.manager,self.control)
    self.sensor.addListener(self)
    self.sensor.run(1)

# =============================================================================
def runTest():
  c = PVSS.controlsMgr()
  s = Control(c,'TestStorage')
  return (c,s)

# =============================================================================
def run(manager=None,name='TestStorage'):
  mgr = manager
  if mgr is None: mgr = PVSS.controlsMgr()
  ctrl = Control(mgr,name)
  sensor=PVSS.DeviceSensor(mgr,ctrl.control)
  sensor.addListener(ctrl)
  sensor.run(1)
  return (mgr,ctrl,sensor)

# =============================================================================
if __name__ == "__main__":
  mgr,ctrl = runTest()
  ctrl.show()
  
runInfoCmds = """
import Online.PVSS as PVSS
from   Online.RunInfo import RunInfoCreator as RunInfo
import Online.Streaming.StreamingControl as StreamingControl
from   Online.RunInfoClasses.Monitoring import MonitoringInfoCreator as MonInfo
mgr = PVSS.controlsMgr()

ctrl = StreamingControl.Control(mgr,'MonTest',MonInfo(mgr,'Storage'))
ctrl.show()
ctrl.show(extended=1)
ctrl.free('LHCb')
res=ctrl.allocate('LHCb',5,2)
ctrl.free('LHCb')
ctrl.show()


ctrl = StreamingControl.Control(mgr,'Storage',RunInfo(mgr))
ctrl.show()
ctrl.show(extended=1)
sensor=PVSS.DeviceSensor(mgr,ctrl.control)
sensor.addListener(ctrl)
sensor.run(1)

info_obj=RunInfo(mgr,'LHCb')
info_obj.show()


ctrl.allocate('MUON',5,2)
ctrl.free('MUON')

reload(StreamingControl)
mgr,ctrl = StreamingControl.runTest()
ctrl.free('MUON')
ctrl.allocate('MUON',5,2)

"""
