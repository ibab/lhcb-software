import Online.PVSS as PVSS
import Online.Utils as Utils

log         = Utils.log
error       = Utils.error
warning     = Utils.warning
printSlots  = Utils.printSlots
std         = PVSS.gbl.std
DataPoint   = PVSS.DataPoint

NAME        = 0
INUSE       = 1
FSM_SLICE   = 2
RECVNODES   = 3
RECVSLICES  = 4
STRMNODES   = 5
STRMSLICES  = 6
RUNINFO     = 7
DATASENDERS = 0
RECVINFRA   = 1
RECVRECV    = 2
RECVSENDERS = 3
STRMINFRA   = 4
STRMRECV    = 5
STRMSENDERS = 6

EMPTY = std.vector('std::string')()

# =============================================================================
def printTasks(msg,tasks):
  for i in tasks:
    node,name,short,type,clazz,dns,sys,sl,vals = i.split('/')
    v = eval(vals)
    fm = ' %6s:%-35s %-16s %%s'
    fmt = fm%(clazz,name,msg)
    m0  = fm%('','','')
    # print '----',i
    if len(v)>0:
      for j in v:
        log(fmt%str(j))
        fmt = m0
    else:
      log(fmt%str(v))

# ===========================================================================
def showPartition(partition,extended=None):
  name = partition.name
  if partition.inUse():
    partName  = partition.detectorName()
    strmNodes = partition.streamNodes()
    recvNodes = partition.recvNodes()
    log('------------------------------- '+name+' -------------------------------',1)
    log('-> Partition:'+name+' is used by '+partName)
    printSlots(recvNodes,'   1st. layer nodes in use['+str(len(recvNodes))+']:',99,10)
    log('   1st layer slices allocated:%d'%len(partition.recvSlices()))
    printSlots(partition.recvSlices(),'           ',5)

    log('')
    printSlots(strmNodes,'   2nd. Layer nodes in use['+str(len(strmNodes))+']:',99,10)
    log('   2nd Layer slices allocated:%d'%len(partition.streamSlices()))
    printSlots(partition.streamSlices(),'           ',5)
    return
  log('-> Partition:'+name+' is FREE.')
  
showReconstruction=showPartition

# ===========================================================================
def showStorage(partition,extended=None):
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
    log('   1st layer slices allocated:%d'%len(partition.recvSlices()))
    for i in xrange(len(partition.recvReceivers())):
      node,task,nick,type,clazz,dns,sys,sl,farm = partition.recvReceiver(i).split('/')
      slot = partition.recvSlice(i)
      log('     Farm:%-8s [%s,%s] sends to %-14s using slot:%s'%(eval(farm)[0],type,clazz,task,slot))
    log('   Writer Tasks:')
    for i in partition.recvSenders():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      opt = eval(opt)
      for i in xrange(len(opt)):
        if i == 0:
          log('     %-38s on %-12s of type:%-10s'%(task+'/'+clazz,node,type))
        else:
          log('     %-38s    %-12s         %-10s'%('','',''))
  log('-> Partition:'+name+' is FREE.')

# ===========================================================================
def showStorage_2_layers(partition,extended=None):
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
    log('   1st layer slices allocated:%d'%len(partition.recvSlices()))
    for i in xrange(len(partition.dataSources())):
      node,task,nick,type,clazz,dns,sys,sl,farm = partition.dataSources()[i].split('/')
      slot = partition.recvSlice(i)
      log('     Farm:%-8s [%s,%s] sends to %-14s using slot:%s'%(eval(farm)[0],type,clazz,task,slot))
    for i in xrange(len(partition.recvReceivers())):
      node,task,nick,type,clazz,dns,sys,sl,farm = partition.recvReceiver(i).split('/')
      slot = partition.recvSlice(i)
      log('     Farm:%-8s [%s,%s] sends to %-14s using slot:%s'%(eval(farm)[0],type,clazz,task,slot))
    log('   1st. Layer senders:')
    for i in partition.recvSenders():
      node,task,nick,type,clazz,dns,sys,sl,t = i.split('/')
      tnode,target = eval(t)[0]
      log('     %-38s on %-10s sends %-12s to %-36s on %s'%(task+'/'+clazz,node,type,target,tnode))
    log('   Infrastructure Tasks:')
    for i in partition.recvInfrastructure():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s as %s'%(task+'/'+clazz,node,type))

    log('')
    printSlots(strmNodes,'   2nd. Layer nodes in use['+str(len(strmNodes))+']:',99,10)
    log('   2nd Layer slices allocated:%d'%len(partition.streamSlices()))
    log('   Receiver Tasks:')
    for i in partition.streamReceivers():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      opt = eval(opt)
      for i in xrange(len(opt)):
        if i == 0:
          log('     %-38s on %-12s of type:%-10s input:%s'%(task+'/'+clazz,node,type,str(opt[0])))
        else:
          log('     %-38s    %-12s         %-10s       %s'%('','','',str(opt[i])))
    log('   Streaming Tasks:')
    for i in partition.streamSenders():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s of type:%-10s streams:%s'%(task+'/'+clazz,node,type,eval(opt)[0]))
    log('   Infrastructure Tasks:')
    for i in partition.streamInfrastructure():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s as %s'%(task+'/'+clazz,node,type))
    return
  log('-> Partition:'+name+' is FREE.')

# ===========================================================================
def showRecStorage(partition,extended=None):
  q = partition.tasks
  p = partition.datapoints
  name = partition.name
  if partition.inUse():
    partName  = partition.detectorName()
    strmNodes = partition.streamNodes()
    recvNodes = partition.recvNodes()
    log('-------------------------------'+partition.manager.name()+' '+name+' -------------------------------',1)
    log('-> Partition:'+name+' is used by '+partName)
    printSlots(recvNodes,'   SENDING layer nodes in use['+str(len(recvNodes))+', '+str(len(partition.recvSlices()))+' Slices]:',99,10)
    for i in partition.recvSenders():
      node,task,nick,type,clazz,dns,sys,sl,farm = i.split('/')
      log('     %-38s on %-10s of type %s sends to %s'%(task+'/'+clazz,node,type,farm))
    log('   File readers:')
    for i in partition.recvReceivers():
      node,task,nick,type,clazz,dns,sys,sl,t = i.split('/')
      log('     %-38s on %-10s of type %-12s reads from file'%(task+'/'+clazz,node,type))
    log('   Infrastructure Tasks:')
    for i in partition.recvInfrastructure():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s as %s'%(task+'/'+clazz,node,type))
    log('')
    printSlots(strmNodes,'   WRITING. Layer nodes in use['+str(len(strmNodes))+', '+str(len(partition.streamSlices()))+' Slices]:',99,10)
    log('   Receiver Tasks:')
    for i in partition.streamReceivers():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s of type:%-10s receives from:%s'%(task+'/'+clazz,node,type,str(opt)))
    log('   Streaming Tasks:')
    for i in partition.streamSenders():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s of type:%-10s streams:%s'%(task+'/'+clazz,node,type,str(opt)))
    log('   Infrastructure Tasks:')
    for i in partition.streamInfrastructure():
      node,task,nick,type,clazz,dns,sys,sl,opt = i.split('/')
      log('     %-38s on %-12s as %s'%(task+'/'+clazz,node,type))
    return
  log('-> Partition:'+name+' is FREE.')

# ===========================================================================
def showMonitoring(partition,extended=None):
  name = partition.name
  if partition.inUse():
    partName  = partition.detectorName()
    strmNodes = partition.streamNodes()
    recvNodes = partition.recvNodes()
    log('------------------------------- '+name+' -------------------------------',1)
    log('-> Partition:'+name+' is used by '+partName)
    printSlots(recvNodes,'   1st. layer nodes in use['+str(len(recvNodes))+']:',99,10)
    log('   1st layer slices allocated:%d'%len(partition.recvSlices()))    
    log('-----> Data senders on storage layer:')
    printTasks('sends to',partition.dataSources())
    log('-----> Data receivers on relay:')
    printTasks('uses',partition.recvReceivers())
    log('-----> Data senders on relay:')
    printTasks('uses',partition.recvSenders())
    log('-----> Infrastructure on relay nodes:')
    printTasks('of type',partition.recvInfrastructure())

    printSlots(strmNodes,'   2nd. Layer nodes in use['+str(len(strmNodes))+']:',99,10)
    log('   2nd Layer slices allocated:%d'%len(partition.streamSlices()))
    log('-----> Data receivers on monitoring nodes:')
    printTasks('receives from',partition.streamReceivers())
    log('-----> Monitoring tasks:')
    printTasks('requires input',partition.streamSenders())
    log('-----> Infrastructure on monitoring nodes:')
    printTasks('of type',partition.streamInfrastructure())
    return
  log('-> Partition:'+name+' is FREE.')

# ===========================================================================
def create(manager, name, cfg_name):
  "Create a new partition"
  DP = PVSS.DataPoint
  dm = manager.deviceMgr()
  tm = manager.typeMgr()
  typ = tm.type('StreamPartition')
  device = dm.createDevice(name,typ,1)
  if device.get() is None:
    warning('Failed to create device "'+name+'"',timestamp=1)
  device = dm.device(name)
  if device.get() is None:
    error('Failed to access device "'+name+'"',timestamp=1)
    return None
  log('Successfully accessed device:'+name,timestamp=1)
  info = PartitionInfo(manager,name)
  info.clear()
  typ = tm.type('StreamConfigurator')
  cfg = dm.createDevice(cfg_name,typ,1)
  if cfg.get() is None:
    warning('Failed to create device "'+cfg_name+'"',timestamp=1)
  cfg = dm.device(cfg_name)
  if cfg.get() is None:
    error('Failed to access device "'+cfg_name+'"',timestamp=1)
    return None
  log('Successfully accessed device:'+cfg_name,timestamp=1)
  dp = DataPoint(manager,DataPoint.original(cfg_name+'.Command'))
  dp.data = "UNKNOWN"
  info.datapoints.push_back(dp)
  dp = DataPoint(manager,DataPoint.original(cfg_name+'.State'))
  dp.data = ''
  info.datapoints.push_back(dp)
  wr = manager.devWriter()
  info.clear().saveAll(wr)
  if wr.execute():
    return (device,cfg,info)
  return None

# =============================================================================
class PartitionInfo:
  """ @class PartitionInfo
  
      Connect to all storage related datapoints in the PartitionInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Streaming Partition datapoint
                              Note: This is the name of the slice - not the name
                              of the detector partition

        @return reference to initialized object
    """
    self.name = name
    self.manager = manager
    self.tasks   = PVSS.DataPointVector()
    self.datapoints = PVSS.DataPointVector()
    self.datapoints.push_back(self.dp('Name'))
    self.datapoints.push_back(self.dp('InUse'))
    self.datapoints.push_back(self.dp('FSMSlice'))
    self.datapoints.push_back(self.dp('RecvNodes'))
    self.datapoints.push_back(self.dp('RecvSlices'))
    self.datapoints.push_back(self.dp('StreamNodes'))
    self.datapoints.push_back(self.dp('StreamSlices'))
    self.datapoints.push_back(self.dp('RunInfo'))
    self.tasks.push_back(self.dp('DataSenders'))
    self.tasks.push_back(self.dp('RecvInfrastructure'))
    self.tasks.push_back(self.dp('RecvReceivers'))
    self.tasks.push_back(self.dp('RecvSenders'))
    self.tasks.push_back(self.dp('StreamInfrastructure'))
    self.tasks.push_back(self.dp('StreamReceivers'))
    self.tasks.push_back(self.dp('StreamSenders'))
    
  # ===========================================================================
  def load(self, rdr=None):
    "Load all datapoints contained in the storage partition description"
    if rdr is None: rdr = self.manager.devReader();
    rdr.add(self.datapoints)
    rdr.add(self.tasks)
    if rdr.execute():
      return self
    return None

  # ===========================================================================
  def saveTasks(self,writer=None):
    wr = writer
    if not writer:
      wr = self.manager.devWriter()
    wr.add(self.tasks)
    if not writer:
      if wr.execute():
        return self
      return None
    return self

  # ===========================================================================
  def saveSlice(self,writer=None):
    wr = writer
    if not writer:
      wr = self.manager.devWriter()
    wr.add(self.datapoints)
    if not writer:
      if wr.execute():
        return self
      return None
    return self

  # ===========================================================================
  def saveAll(self,writer=None):
    wr = writer
    if not writer:
      wr = self.manager.devWriter()
    wr.add(self.datapoints)
    wr.add(self.tasks)
    if not writer:
      if wr.execute():
        return self
      return None
    return self

  # ===========================================================================
  def initialize(self,rundp_name,partition,writer):
    log('Using partition: '+partition,timestamp=1)
    self.datapoints[INUSE].data = 1
    self.datapoints[NAME].data = partition
    self.datapoints[FSM_SLICE].data = self.name
    self.datapoints[RUNINFO].data = rundp_name
    if writer:
      writer.add(self.datapoints[INUSE])
      writer.add(self.datapoints[NAME])
      writer.add(self.datapoints[FSM_SLICE])
      writer.add(self.datapoints[RUNINFO])
    return self

  # ===========================================================================
  def dp(self,name):
    "Access internal datapoint of the datapoint structure."
    return DataPoint(self.manager,DataPoint.original(self.name+'.'+name))
  # ===========================================================================
  def wrdp(self,name):
    "Access internal datapoint of the datapoint structure."
    return DataPoint(self.manager,DataPoint.online(self.name+'.'+name))
  # ===========================================================================
  def runInfo(self):
    return self.datapoints[RUNINFO].data
  # ===========================================================================
  def detectorName(self):
    return self.datapoints[NAME].data
  # ===========================================================================
  def inUse(self):
    return self.datapoints[INUSE].data
  # ===========================================================================
  def fsmSlice(self):
    return self.datapoints[FSM_SLICE].data
  # ===========================================================================
  def recvNodes(self):
    return self.datapoints[RECVNODES].data
  # ===========================================================================
  def recvSlices(self):
    return self.datapoints[RECVSLICES].data
  # ===========================================================================
  def recvSlice(self,which):
    return self.datapoints[RECVSLICES].data[which]
  # ===========================================================================
  def streamNodes(self):
    return self.datapoints[STRMNODES].data
  # ===========================================================================
  def streamSlices(self):
    return self.datapoints[STRMSLICES].data
  # ===========================================================================
  def streamSlice(self,which):
    return self.datapoints[STRMSLICES].data[which]
  # ===========================================================================  
  def dataSources(self):
    return self.tasks[DATASENDERS].data    
  # ===========================================================================
  def recvInfrastructure(self):
    return self.tasks[RECVINFRA].data
  # ===========================================================================
  def recvReceivers(self):
    return self.tasks[RECVRECV].data
  # ===========================================================================
  def recvReceiver(self,which):
    return self.tasks[RECVRECV].data[which]
  # ===========================================================================
  def recvSenders(self):
    return self.tasks[RECVSENDERS].data
  # ===========================================================================
  def streamInfrastructure(self):
    return self.tasks[STRMINFRA].data
  # ===========================================================================
  def streamReceivers(self):
    return self.tasks[STRMRECV].data
  # ===========================================================================
  def streamReceiver(self,which):
    return self.tasks[STRMRECV].data[which]
  # ===========================================================================
  def streamSenders(self):
    return self.tasks[STRMSENDERS].data
  # ===========================================================================
  def streamSender(self,which):
    return self.tasks[STRMSENDERS].data[which]
  # ===========================================================================  
  def setDataSources(self, dataSources):
    self.tasks[DATASENDERS].data.clear()    
    for i in dataSources:        self.tasks[DATASENDERS].data.push_back(i)
  # ===========================================================================
  def setRecvInfrastructure(self, recvInfrastructure):
    self.tasks[RECVINFRA].data.clear()
    for i in recvInfrastructure: self.tasks[RECVINFRA].data.push_back(i)
  # ===========================================================================
  def setRecvReceivers(self, recvReceivers):
    self.tasks[RECVRECV].data.clear()
    for i in recvReceivers:      self.tasks[RECVRECV].data.push_back(i)
  # ===========================================================================
  def setRecvSenders(self, recvSenders):
    self.tasks[RECVSENDERS].data.clear()
    for i in recvSenders:        self.tasks[RECVSENDERS].data.push_back(i)
  # ===========================================================================
  def setStreamInfrastructure(self, strmInfrastructure):
    self.tasks[STRMINFRA].data.clear()
    for i in strmInfrastructure: self.tasks[STRMINFRA].data.push_back(i)
  # ===========================================================================
  def setStreamReceivers(self, strmReceivers):
    self.tasks[STRMRECV].data.clear()
    for i in strmReceivers:      self.tasks[STRMRECV].data.push_back(i)
  # ===========================================================================
  def setStreamSenders(self, strmSenders):
    self.tasks[STRMSENDERS].data.clear()
    for i in strmSenders:        self.tasks[STRMSENDERS].data.push_back(i)
  # ===========================================================================
  def show(self,method=showPartition,extended=None):
    "Show all information from the Streaming Partition structure."
    method(self,extended)
  # ===========================================================================
  def _collectTasks(self,tasks,data):
    for i in data.data:
      items = i.split('/')
      if len(items) > 0:
        if not tasks.has_key(items[0]):
          tasks[items[0]] = []
        tasks[items[0]].append(items)
    return tasks  
  # ===========================================================================
  def collectTasks(self,tasks,with_data_sources):
    if with_data_sources:
      tasks = self._collectTasks(tasks,self.tasks[DATASENDERS])
    tasks = self._collectTasks(tasks,self.tasks[RECVINFRA])
    tasks = self._collectTasks(tasks,self.tasks[RECVRECV])
    tasks = self._collectTasks(tasks,self.tasks[RECVSENDERS])
    tasks = self._collectTasks(tasks,self.tasks[STRMINFRA])
    tasks = self._collectTasks(tasks,self.tasks[STRMRECV])
    tasks = self._collectTasks(tasks,self.tasks[STRMSENDERS])
    return tasks
  # ===========================================================================
  def clear(self):
    self.clearTasks()
    return self.clearSlice()
  # ===========================================================================
  def clearTasks(self):
    self.tasks[DATASENDERS].data = EMPTY
    self.tasks[RECVINFRA].data = EMPTY
    self.tasks[RECVRECV].data = EMPTY
    self.tasks[RECVSENDERS].data = EMPTY
    self.tasks[STRMINFRA].data = EMPTY
    self.tasks[STRMRECV].data = EMPTY
    self.tasks[STRMSENDERS].data = EMPTY
    return self
  # ===========================================================================
  def clearSlice(self):
    self.datapoints[NAME].data = ''
    self.datapoints[INUSE].data = 0
    self.datapoints[RUNINFO].data = ''
    self.datapoints[FSM_SLICE].data = ''
    self.datapoints[RECVNODES].data = EMPTY
    self.datapoints[RECVSLICES].data = EMPTY
    self.datapoints[STRMNODES].data = EMPTY
    self.datapoints[STRMSLICES].data = EMPTY
    return self

  # ===========================================================================
  def streamNodesFromSlots(self):
    slots = self.streamSlices()
    nodes = {}
    for i in slots:
      nodes[i[:i.find(':')]] = 1
    return nodes.keys()

  # ===========================================================================
  def recvNodesFromSlots(self):
    slots = self.recvSlices()
    nodes = {}
    for i in slots:
      nodes[i[:i.find(':')]] = 1
    return nodes.keys()

