import time, FarmSetup, Online.Utils, Online.PVSSSystems
import Online.Streaming.Allocator as StreamAlloc
import Online.Streaming.PartitionInfo as PartitionInfo

import Online.PVSS as PVSS

DataPoint     = PVSS.DataPoint
std           = PVSS.gbl.std
log           = Online.Utils.log
error         = Online.Utils.error
printSlots    = Online.Utils.printSlots
StreamAlloc   = Online.Streaming.Allocator.Allocator
PartitionInfo = Online.Streaming.PartitionInfo.PartitionInfo

# ===========================================================================
def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()

# ===========================================================================
def showRecoPartition(partition,extended=None):
  q = partition.tasks
  p = partition.datapoints
  name = partition.name
  if partition.inUse():
    partName  = partition.detectorName()
    strmNodes = partition.streamNodes()
    recvNodes = partition.recvNodes()
    log('------------------------------- '+name+' -------------------------------',1)
    log('-> Partition:'+name+' is used by '+partName)
    printSlots(recvNodes,'   1st. layer nodes in use['+str(len(recvNodes))+']:',99,10)
    log('   Storage layer slices allocated:%d'%len(partition.recvSlices()))
    for i in xrange(len(partition.dataSources())):
      node,task,nick,type,clazz,dns,sys,farm = partition.dataSources()[i].split('/')
      slot = partition.recvSlice(i)
      recv = partition.recvReceiver(i).split('/')[1]
      log('     Farm:%-8s [%s,%s] sends to %-14s using slot:%s'%(eval(farm)[0],type,clazz,recv,slot))
    for i in xrange(len(partition.recvReceivers())):
      node,task,nick,type,clazz,dns,sys,farm = partition.recvReceiver(i).split('/')
      slot = partition.recvSlice(i)
      log('     Recv:%-24s [%s,%s] receives from %-6s using slot:%s'%(task,type,clazz,eval(farm)[0],slot))
    log('   Storage Layer file writer(s):')
    for i in partition.recvSenders():
      node,task,nick,type,clazz,dns,sys,t = i.split('/')
      log('     Writer:%-38s [%s] on %-10s '%(task+'/'+clazz,type,node))
    log('   Storage Layer infrastructure tasks:')
    for i in partition.recvInfrastructure():
      node,task,nick,type,clazz,dns,sys,opt = i.split('/')
      log('     %-38s on %-12s as %s'%(task+'/'+clazz,node,type))

    log('')
    printSlots(strmNodes,'   Reading Layer nodes in use['+str(len(strmNodes))+']:',99,10)
    log('   Reading Layer slices allocated:%d'%len(partition.streamSlices()))
    log('   File reader tasks:')
    for i in partition.streamReceivers():
      node,task,nick,type,clazz,dns,sys,t = i.split('/')
      log('     Reader:%-38s [%s] on %-10s '%(task+'/'+clazz,type,node))
    log('   Farm sender tasks:')
    for i in xrange(len(partition.streamSenders())):
      node,task,nick,type,clazz,dns,sys,t = partition.streamSender(i).split('/')
      slot = partition.streamSlice(i)
      log('     Sender:%-24s [%s,%s] on %-12s sends to:%s using slot:%s'%(task,type,clazz,node,eval(t)[0],slot))
    log('   Reading layer infrastructure Tasks:')
    for i in partition.streamInfrastructure():
      node,task,nick,type,clazz,dns,sys,opt = i.split('/')
      log('     %-38s on %-12s as %s'%(task+'/'+clazz,node,type))
    return
  log('-> Partition:'+name+' is FREE.')


# =============================================================================
class DatapointLoader:
  """ @class DatapointLoader
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    self.name = name
    self.dpName = name
    self.manager = manager
    self.reader = self.manager.devReader()
  # ===========================================================================
  def dp(self,name):
    "Access internal datapoint of the datapoint structure."
    return DataPoint(self.manager,DataPoint.original(self.dpName+'.'+name))

  # ===========================================================================
  def load(self):
    "Load the datapoints assigned to the reader."
    if self.reader.execute(1,1):
      return self
    error('Failed to load run information for:'+self.name)
    return None
        
# =============================================================================
class FarmSliceInfo(DatapointLoader):
  """ @class FarmInfo
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    DatapointLoader.__init__(self,manager,name)
    self.dpName      = self.name
    self.inUse       = self.dp('InUse')
    self.activity    = self.dp('Activity')
    self.sliceName   = self.dp('Name')
    self.subFarms    = self.dp('SubFarms')
    self.ioSlice     = self.dp('IOSlice')
    self.tasks       = self.dp('Tasks')
    self.addDp(self.reader)
    self.load()

  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.inUse)
    deviceIO.add(self.activity)
    deviceIO.add(self.sliceName)
    deviceIO.add(self.subFarms)
    deviceIO.add(self.ioSlice)
    deviceIO.add(self.tasks)
    return self

  # ===========================================================================
  def doStreaming(self):
    "Access to the use-flag"
    return True
  # ===========================================================================
  def numLayer1Slots(self):
    "Access the number of slices required for stream layer 1"
    return len(self.subFarms.data)
  # ===========================================================================
  def numLayer2Slots(self):
    "Access the number of slices required for stream layer 2"
    return len(self.subFarms.data)
  # ===========================================================================
  def show(self, prefix=''):
    "Show sub-farm information"
    if self.load():
      log(prefix+self.name+':'+self.sliceName.data+'  I/O slice:'+self.ioSlice.data,timestamp=1)
      fmt = '%sNodes:%s'
      s = ''
      farms = self.subFarms.data
      num = len(farms)
      for n in xrange(num):
        q = n+1
        s = s + '%s '%(farms[n],cpus[n])
        if (q%10)==0 or (q==num):
          log(fmt%(prefix,s),timestamp=1,with_trailer=q==num)
          s = ''
  # ===========================================================================
  def defineTasks(self,partition):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.
    
    """
    recv_slots = partition.recvSlices()
    strm_slots = partition.streamSlices()
    activity = FarmActivity(self.manager,self.activity.data).load()
    recvNodes = partition.recvNodesFromSlots()
    strmNodes = partition.streamNodesFromSlots()
    dimDns    = self.manager.hostName()
    streamers = []
    strmSenders = []
    strmReceivers = []
    strmInfrastructure = []
    recvReceivers = []
    recvSenders = []
    recvInfrastructure = []
    dataSources = []
    opt = '/'+dimDns+'/'+partition.manager.name()+'/'
    cl0 = '/Class0'+opt
    cl1 = '/Class1'+opt
    cl2 = '/Class2'+opt

    for i in xrange(len(recv_slots)):
      slot = recv_slots[i]
      node = slot[:slot.find(':')]
      sub_farm = self.subFarms.data[i] # 'SF%02d'%(i,)
      short_name = sub_farm+'_Recv'
      task = node+'_'+short_name
      brunel = 'Brunel_'+sub_farm
      brunel_task = node+'_'+brunel
      recvReceivers.append(node+'/'+task+'/'+short_name+'/'+activity.outputTypes.data[0]+cl1+'("'+sub_farm+'",)')
      dataSources.append(node+'/'+brunel_task+'/'+brunel+'/Brunel'+cl1+'("'+sub_farm+'",)')
    
    for i in xrange(len(recvNodes)):
      node = recvNodes[i]
      short_name = node+'_Writer'
      task = self.name+'_'+short_name
      recvSenders.append(node+'/'+task+'/'+short_name+'/'+activity.outputTypes.data[1]+cl1+'("'+activity.outputTypes.data[1]+'",)')
      for j in activity.outputInfrastructure.data:
        task = node+'_'+j
        recvInfrastructure.append(node+'/'+task+'/'+task+'/'+j+cl0+'("'+j+'",)')
      
    for i in xrange(len(strm_slots)):
      slot = strm_slots[i]
      node = slot[:slot.find(':')]
      item = self.subFarms.data[i]
      short_name = item+'_Send'
      task = node+'_'+short_name
      strmSenders.append(node+'/'+task+'/'+short_name+'/'+activity.inputTypes.data[0]+cl2+'("'+item+'",)')

    for i in xrange(len(strmNodes)):
      node = strmNodes[i]
      item = activity.inputTypes.data[1]
      short_name = node+'_Reader'
      task = self.name+'_'+short_name
      strmReceivers.append(node+'/'+task+'/'+short_name+'/'+item+cl1+'("'+item+'",)')
      for j in activity.inputInfrastructure.data:
        task = node+'_'+j
        strmInfrastructure.append(node+'/'+task+'/'+task+'/'+j+cl0+'("'+j+'",)')

    partition.setDataSources(dataSources)
    partition.setRecvInfrastructure(recvInfrastructure)
    partition.setRecvReceivers(recvReceivers)
    partition.setRecvSenders(recvSenders)
    partition.setStreamInfrastructure(strmInfrastructure)
    partition.setStreamReceivers(strmReceivers)
    partition.setStreamSenders(strmSenders)
    if partition.saveTasks():
      tasks = partition.collectTasks(tasks={},with_data_sources=0)
      return tasks
    return None
          
# =============================================================================
class SubFarmInfo(DatapointLoader):
  """ @class FarmInfo
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    DatapointLoader.__init__(self,manager,name)
    self.dpName      = self.name
    self.subFarmName = self.dp('Name')
    self.nodes       = self.dp('Nodes')
    self.numCPU      = self.dp('NodesCPU')
    self.used        = self.dp('UsedBy')
    self.addDp(self.reader)

  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.subFarmName)
    deviceIO.add(self.nodes)
    deviceIO.add(self.numCPU)
    deviceIO.add(self.used)
    return self

  # ===========================================================================
  def subfarmName(self):
    "Return the name of the processing farm"
    return self.subFarmName.data
  # ===========================================================================
  def processors(self):
    "Return the subfarm names"
    return self.nodes.data
  # ===========================================================================
  def nodesCPU(self):
    "Return the subfarm names"
    return self.numCPU.data
  # ===========================================================================
  def usedBy(self):
    "Return used by string"
    return self.used.data
  # ===========================================================================
  def show(self, prefix=''):
    "Show sub-farm information"
    if self.load():
      n = prefix+'Subfarm:'+self.name+':'+self.subfarmName()
      if self.usedBy() != '': n = n + '  Used by '+self.usedBy()
      log(n,timestamp=1)
      fmt = '%sNodes:%s'
      s = ''
      nodes = self.processors()
      cpus = self.nodesCPU()
      num = len(nodes)
      for n in xrange(num):
        q = n+1
        s = s + '%s [%d]  '%(nodes[n],cpus[n])
        if (q%5)==0 or (q==num):
          log(fmt%(prefix,s),timestamp=1,with_trailer=q==num)
          s = ''
          
# =============================================================================
class FarmInfo(DatapointLoader):
  """ @class FarmInfo
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    DatapointLoader.__init__(self,manager,name)
    self.dpName      = self.name
    self.farmName    = self.dp('Name')
    self.subFarms    = self.dp('SubFarms')
    self.inUse       = self.dp('InUse')
    self.addDp(self.reader)
  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.farmName)
    deviceIO.add(self.subFarms)
    deviceIO.add(self.inUse)
    return self
  # ===========================================================================
  def getFreeSubFarms(self):
    "Return list with tuples (name,index) containing free subfarms"
    if self.load():
      free = []
      for i in xrange(len(self.inUse.data)):
        if self.inUse.data[i] == '':  free.append((self.subFarms.data[i],i))
      return free
    return None
  # ===========================================================================
  def getAllocatedSubFarms(self,slice):
    "Return list with tuples (name,index) containing subfarms allocated for 'slice'"
    if self.load():
      farms = []
      for i in xrange(len(self.inUse.data)):
        if self.inUse.data[i]==slice: farms.append((self.subFarms.data[i],i))
      return farms
    return None
  # ===========================================================================
  def farmName(self):
    "Return the name of the processing farm"
    return self.farmName.data
  # ===========================================================================
  def subFarms(self):
    "Return the subfarm names"
    return self.subFarms.data
  # ===========================================================================
  def inUse(self):
    "Return the use flags for the sub farms."
    return self.inUse.data
  # ===========================================================================
  def allocate(self,slice,farms,writer):
    "Allocate subfarms"
    for i in farms:
      self.inUse.data[i[1]] = slice
    writer.add(self.inUse)
    return self
  # ===========================================================================
  def free(self,slice,writer):
    "Free subfarms allocated for 'slice'"
    if self.load():
      for i in xrange(len(self.inUse.data)):
        if self.inUse.data[i]==slice:
          self.inUse.data[i] = ''
      writer.add(self.inUse)
      return self
    return None
  # ===========================================================================
  def show(self):
    "Show farm information"
    if self.load():
      log('Summary information of reconstruction farm:'+self.name,timestamp=1,with_trailer=1)
      log('SubFarm information:',timestamp=1)
      log('Number %-16s: %s'%('Subfarm-name','Used by...'),timestamp=1)
      for i in xrange(len(self.subFarms.data)):
        sf_name = self.subFarms.data[i]
        if self.inUse.data[i] == '':
          log('%5d: %-16s  %s'%(i, sf_name, '-------------'),timestamp=1)
        else:
          log('%5d: %-16s  %s'%(i, sf_name, self.inUse.data[i]),timestamp=1)
        SubFarmInfo(self.manager,sf_name).show('%5d: '%i)


# =============================================================================
class FarmActivity(DatapointLoader):
  """ @class FarmActivity
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    DatapointLoader.__init__(self,manager,name)
    self.dpName               = self.name
    self.activityName         = self.dp('Name')
    self.farmInfrastructure   = self.dp('Farm.Infrastructure')
    self.farmTasks            = self.dp('Farm.TaskTypes')
    self.farmReceiver         = self.dp('Farm.ReceiverType')
    self.farmSender           = self.dp('Farm.SenderType')
    self.inputInfrastructure  = self.dp('Storage.streamInfrastructure')
    self.inputTypes           = self.dp('Storage.streamTypes')
    self.outputInfrastructure = self.dp('Storage.recvInfrastructure')
    self.outputTypes          = self.dp('Storage.recvTypes')
    self.addDp(self.reader)
  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.activityName)
    deviceIO.add(self.farmInfrastructure)
    deviceIO.add(self.farmTasks)
    deviceIO.add(self.farmReceiver)
    deviceIO.add(self.farmSender)
    deviceIO.add(self.inputInfrastructure)
    deviceIO.add(self.inputTypes)
    deviceIO.add(self.outputInfrastructure)
    deviceIO.add(self.outputTypes)
    return self
  # ===========================================================================
  def show(self):
    "Show activity information"
    if self.load():
      log('Actifity information:'+self.activityName.data,timestamp=1,with_trailer=1,with_header=1)
      log('Farm task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.farmInfrastructure.data]),timestamp=1)
      log('    Task types:    '+str([i for i in self.farmTasks.data]),timestamp=1)
      log('    Receiver type: '+self.farmReceiver.data,timestamp=1)
      log('    Sender type:   '+self.farmSender.data,timestamp=1)
      log('Input task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.inputInfrastructure.data]),timestamp=1)
      log('    Reader type:   '+self.inputTypes.data[1],timestamp=1)
      log('    Sender type:   '+self.inputTypes.data[0],timestamp=1)
      log('Output task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.outputInfrastructure.data]),timestamp=1)
      log('    Receiver type: '+self.outputTypes.data[0],timestamp=1)
      log('    Writer type:   '+self.outputTypes.data[1],timestamp=1,with_trailer=1)


# =============================================================================
class FarmSliceManager:
  # ===========================================================================
  def __init__(self,manager,name):
    self.manager = manager
    self.name = name
    self.dpName = self.name
    self.farm = FarmInfo(self.manager,self.name)
    self.streamAlloc = StreamAlloc(self.manager,self.name+'IO',self)

  # ===========================================================================
  def slices(self):
    rdr   = self.manager.devReader()
    actor = PVSS.DpVectorActor(self.manager)
    names = PVSS.DpVectorActor(self.manager)
    typ   = self.manager.typeMgr().type('RecoFarmSlice')
    actor.lookupOriginal(self.dpName+'_Slice*.InUse',typ)
    names.lookupOriginal(self.dpName+'_Slice*.Name',typ)
    rdr.add(actor.container)
    rdr.add(names.container)
    if rdr.execute():
      slices = []
      num = len(actor.container)
      for j in xrange(num):
        slices.append((names.container[j].data,actor.container[j].data))
      return slices
    return None
  
  # ===========================================================================
  def findSlice(self,slice):
    slices = self.slices()
    for i in slices:
      if i[0] == slice:
        return i
    return None
  
  # ===========================================================================
  def findFreeSlice(self):
    slices = self.slices()
    for i in slices:
      if i[1] == 0:
        return i[0]
    return None
  
  # ===========================================================================
  def makeError(self,msg):
    error(msg,timestamp=1)
    return None
  
  # ===========================================================================
  def _free(self,slice_obj, writer=None):
    "Free an allocated slice from pool and return allocated subfarms back to the farm description"
    slice,index = slice_obj
    if writer is None: writer = self.manager.devWriter()
    self.farm.free(slice,writer)
    inUse = DataPoint(self.manager,DataPoint.original(slice+'.InUse'))
    inUse.data = 0
    writer.add(inUse)
    sf = DataPoint(self.manager,DataPoint.original(slice+'.SubFarms'))
    sf.data = std.vector('std::string')()
    writer.add(sf)
    if writer.execute():
      return self
    return None
  
  # ===========================================================================
  def _freeIO(self,slice,writer=None):
    self.streamAlloc.free(slice,slice)
    return self
  
  # ===========================================================================
  def _allocate(self, slice, numSubFarms, writer=None):
    "Allocate a given number of subfarms from pool"
    if writer is None: writer = self.manager.devWriter()
    farms = self.farm.getFreeSubFarms()
    if len(farms)<numSubFarms:
      log('Cannot allocate '+str(numSubFarms)+' sub-farms. Got only:'+str(len(farms)),timestamp=1)
      return None
    farms = [farms[i] for i in xrange(numSubFarms)]
    inUse = DataPoint(self.manager,DataPoint.original(slice+'.InUse'))
    inUse.data = 1
    writer = self.manager.devWriter()
    writer.add(inUse)
    sf = DataPoint(self.manager,DataPoint.original(slice+'.SubFarms'))
    sf.data = [i[0] for i in farms]
    writer.add(sf)
    self.farm.allocate(slice,farms,writer)
    if writer.execute():
      log('Allocated slice:%s with %d subfarms:%s'%(slice,numSubFarms,[i for i in sf.data]),timestamp=1)
      return self
    return None

  # ===========================================================================
  def _allocateIO(self, slice, writer=None):
    ioSlice_name = self.streamAlloc.allocate(slice,slice)
    if writer is None: writer = self.manager.devWriter()
    if ioSlice_name:
      ioSlice = DataPoint(self.manager,DataPoint.original(slice+'.IOSlice'))
      ioSlice.data = ioSlice_name
      writer.add(ioSlice)
      if writer.execute():
        log('Got I/O partition:'+ioSlice_name+' for slice:'+slice,timestamp=1)
        return self
      log('Failed to update I/O partition:'+ioSlice_name+' for slice:'+slice,timestamp=1)
    log('No I/O partition availible for slice:'+slice,timestamp=1)
    return None
  
  # ===========================================================================
  def allocate(self, numSubFarms):
    "Allocate a given number of subfarms from pool"
    slice = self.findFreeSlice()
    writer = self.manager.devWriter()
    if slice:
      log('Allocating slice:'+slice,timestamp=1)
      if self._allocate(slice, numSubFarms, writer):
        if self._allocateIO(slice, writer):
          return slice
      return None
    return self.makeError('Failed to find free slice...')
  
  # ===========================================================================
  def free(self,slice):
    "Free an allocated slice from pool and return allocated subfarms back to the farm description"
    slice_obj = self.findSlice(slice)
    if slice_obj:
      self.streamAlloc.load()
      if self._free(slice_obj):
        if self._freeIO(slice):
          log('Free slice:%s'%(slice),timestamp=1)
          return slice
        return self.makeError('Failed to free I/O partition for slice:'+slice)
      return self.makeError('Failed to write data when freeing slice '+slice)
    return self.makeError('Failed to find slice:'+slice)
  
  # ===========================================================================
  def _configureIO(self,slice_obj,activity,farm):
    slice,index = slice_obj
    writer = self.manager.devWriter()
    activityDp      = DataPoint(self.manager,DataPoint.original(slice+'.Activity'))
    activityDp.data = 'RecoFarmActivity_'+activity
    runInfo = DataPoint(self.manager,DataPoint.original(slice+'.RunInfo'))
    runInfo.data = activityDp.data
    partition = DataPoint(self.manager,DataPoint.original(slice+'.general.partName'))
    partition.data = slice
    writer.add(activityDp)
    writer.add(partition)
    writer.add(runInfo)
    if writer.execute():
      dp, io_slice, id = self.streamAlloc.getPartition(slice)
      res = self.streamAlloc.configure(slice,slice)
      if res:
        runInfo = DataPoint(self.manager,DataPoint.original(io_slice+'.RunInfo'))
        runInfo.data = activityDp.data
        writer.add(runInfo)
        if writer.execute():
          return res
    return None
  
  # ===========================================================================
  def defineTasks(self,slice_obj,activity, farms):
    slice,index = slice_obj
    task_set = {}
    opt = '/'+self.manager.hostName()+'/'+self.manager.name()+'/'
    cl0 = '/Class0'+opt
    cl1 = '/Class1'+opt
    cl2 = '/Class2'+opt
    for subfarm_name,index in farms:
      task_set[subfarm_name] = {}
      sf = SubFarmInfo(self.manager,self.name+'_'+subfarm_name).load()
      cpus = sf.nodesCPU()
      nodes = sf.processors()
      max_tasks = 23
      tasks = []
      nset = 0
      for i in xrange(nodes.size()):
        p = nodes[i]
        ncpu = cpus[i]
        node_tasks = []
        for j in activity.farmInfrastructure.data:
          node_tasks.append(p+'/'+p+'_'+j+'/'+p+'_'+j+'/'+j+cl0)
        typ = activity.farmReceiver.data
        node_tasks.append(p+'/'+p+'_'+typ+'/'+p+'_'+typ+'/'+typ+cl1)
        typ = activity.farmSender.data
        node_tasks.append(p+'/'+p+'_'+typ+'/'+p+'_'+typ+'/'+typ+cl2)
        for j in xrange(ncpu):
          itm = '_%02d'%j
          for k in activity.farmTasks.data:
            task = p+'_%s'%(k,)
            node_tasks.append(p+'/'+task+itm+'/'+task+'/'+k+cl1)

        if len(tasks)+len(node_tasks)<= max_tasks:
          for j in node_tasks: tasks.append(j)
        else:
          set_name = self.name+'_Slice%02d'%nset
          opts = '("'+subfarm_name+'","'+set_name+'")'
          task_set[subfarm_name][set_name] = [j+opts for j in tasks]
          tasks = [j for j in node_tasks]
          nset = nset + 1
      set_name = self.name+'_Slice%02d'%nset
      opts = '("'+subfarm_name+'","'+set_name+'")'
      task_set[subfarm_name][set_name] = [j+opts for j in tasks]
    return task_set

  # ===========================================================================
  def configure(self,slice,activity):
    slice_obj = self.findSlice(slice)
    if slice_obj is None:
      return self.makeError('Failed to find slice:'+slice)    
    act = FarmActivity(self.manager,'RecoFarmActivity_'+activity).load()
    if not act:
      return self.makeError('Failed to find reconstruction activity:'+activity)    
    farms = self.farm.getAllocatedSubFarms(slice)
    self.streamAlloc.load()
    
    act.show()
    for name,index in farms:
      sf = SubFarmInfo(self.manager,self.name+'_'+name).load()
      sf.show()
    task_set = self.defineTasks(slice_obj,act,farms)
    all_tasks = {}
    for n,tasks in task_set.items():
      print n,':'
      for s,t in tasks.items():
        print '\t',s
        for tt in t:
          print '\t\t\t',tt
          items = tt.split('/')
          if len(items)>0:
            if not tasks.has_key(items[0]):
              all_tasks[items[0]] = []
            all_tasks[items[0]].append(items)
    # return all_tasks
    if not self._configureIO(slice_obj,activity,farms):
      return self.makeError('Failed configure I/O for slice '+slice+' and activity:'+activity)    
    return self

  def create(self,rundp_name,partition):
    return FarmSliceInfo(self.manager,partition)
  def showPartition(self,partition,extended=None):
    import Online.Streaming.PartitionInfo as Info
    partition.show(method=showRecoPartition,extended=extended)

# =============================================================================
def do_it(name='Reco'):
  mgr = FarmSliceManager(Online.PVSSSystems.controlsMgr('RECOTEST'),name)
  mgr.free(name+'_Slice00')
  mgr.allocate(2)
  mgr.configure(name+'_Slice00','REPRO_1')
  return mgr
  
  
def bla(name):
  res = mgr.allocate(2)
  if res is None:
    mgr.free(name+'_Slice00')
    print "mgr.free(name+'_Slice00')"
  res = mgr.allocate(2)
  farm = FarmInfo(mgr.manager,name)
  farm.show()
  mgr = FarmSliceManager(mgr,name)
  mgr.defineTasks(name+'_Slice00')
  return mgr

if __name__ == "__main__":
  do_it()
  
