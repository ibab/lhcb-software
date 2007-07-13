import time
import Online.PVSS as PVSS
import Online.Utils as Utils
from   Online.RunInfoClasses.General import General as General
from   Online.Streaming.PartitionInfo import PartitionInfo as PartitionInfo
from   Online.Streaming.StreamingDescriptor import StreamingDescriptor as StreamDescriptor

log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
printSlots  = Utils.printSlots

# =============================================================================
def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()

# =============================================================================
def printTasks(msg,tasks):
  for i in tasks:
    node,name,short,type,vals = i.split('/')
    v = eval(vals)
    fmt = ' %-32s %-16s %%s'%(name,msg)
    m0  = ' %-32s %-16s %%s'%('','')
    # print '----',i
    if len(v)>0:
      for j in v:
        log(fmt%str(j))
        fmt = m0
    else:
      log(fmt%str(v))
    
# ===========================================================================
def showPartition(info,partition):
  p = partition.datapoints
  name = partition.name
  if p[1].data:
    partName  = p[0].data
    strmNodes = p[5].data
    recvNodes = p[3].data
    log('------------------------------- '+name+' -------------------------------',1)
    log('-> Partition:'+name+' is used by '+partName)
    printSlots(p[3].data,'   1st. layer nodes in use['+str(len(recvNodes))+']:',99,10)
    log('   1st layer slices allocated:%d'%len(p[4].data))    
    if info:
      info.load()
      log('-----> Data senders on storage layer:')
      printTasks('sends to',info.senders.data)
      log('-----> Data receivers/senders on relay:')
      printTasks('uses',info.relayTasks.data)
      log('-----> Infrastructure on relay nodes:')
      printTasks('of type',info.relayInfraTasks.data)
    else:
      printSlots(p[4].data,'           ',5)
      log('')

    printSlots(p[5].data,'   2nd. Layer nodes in use['+str(len(strmNodes))+']:',99,10)
    log('   2nd Layer slices allocated:%d'%len(p[6].data))
    if info:
      log('-----> Data receivers on monitoring nodes:')
      printTasks('receives from',info.monReceivers.data)
      log('-----> Monitoring tasks:')
      printTasks('requires input',info.monTasks.data)
      log('-----> Infrastructure on monitoring nodes:')
      printTasks('of type',info.monInfraTasks.data)
    else:
      printSlots(p[6].data,'           ',5)
  else:
    log('-> Partition:'+name+' is FREE.')


# =============================================================================
class Monitoring:
  """ @class Monitoring
  
      Connect to all storage related datapoints in the RunInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, storage):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    self.storage = storage
    type = 'MonFarm'
    self.flag            = self.dp(type+'.monFlag')
    self.monTypes        = self.dp(type+'.monTypes')
    self.monMult         = self.dp(type+'.monMultiplicity')

    self.senders         = self.dp(type+'.senderTasks')
    self.relayInfra      = self.dp(type+'.relayInfrastructure')
    self.monStreams      = self.dp(type+'.monStreams')
    self.relayInfraTasks = self.dp(type+'.relayInfrastructureTasks')
    self.relayTasks      = self.dp(type+'.relayTasks')

    self.monInfra        = self.dp(type+'.monInfrastructure')
    self.monInfraTasks   = self.dp(type+'.monInfrastructureTasks')
    self.monReceivers    = self.dp(type+'.monReceivers')
    self.monTasks        = self.dp(type+'.monTasks')

    self.reader.add(self.flag)
    self.reader.add(self.senders)
    self.reader.add(self.monTypes)
    self.reader.add(self.monInfra)
    self.reader.add(self.relayInfra)
    self.reader.add(self.monMult)
    self.reader.add(self.monStreams)

    self.reader.add(self.monTasks)
    self.reader.add(self.monInfraTasks)
    self.reader.add(self.monReceivers)
    self.reader.add(self.relayInfraTasks)
    self.reader.add(self.relayTasks)

  # ===========================================================================
  def numLayer2Slots(self):
    return sum(self.monMult.data)
  # ===========================================================================
  def numLayer1Slots(self):
    return 2*len(self.monStreams.data)
  # ===========================================================================
  def addDevices(self, deviceIO):
    "Add task devices to device IO structure for read/write access"
    wr = deviceIO
    if wr is None: wr = self.manager.devWriter()
    wr.add(self.senders)
    wr.add(self.monTasks)
    wr.add(self.monReceivers)
    wr.add(self.monInfraTasks)
    wr.add(self.relayTasks)
    wr.add(self.relayInfraTasks)
    if deviceIO is None:
      if wr.execute(0,1):
        return wr
      return None
    return deviceIO
  # ===========================================================================
  def showStreams(self):
    "Show output stream related information of the RunInfo datapoint."
    streams = self.monTypes.data
    multiplicity = self.strMult.data
    log('Output to %d logical streams'%len(self.monTypes.data))
    for i in xrange(len(streams)):
      log(' -> Data stream:%-24s with  mutiplicity:%d'%(streams[i],multiplicity[i]))
  # ===========================================================================
  def show(self):
    "Show all information from the Monitoring structure."
    if self.load():
      self.showStreams()
      self.showGeneral()
      return
    error('Failed to load RunInfo for partition:'+self.name)
  # ===========================================================================
  def defineTasks(self,relay_slots,mon_slots):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.
    """
    monNodes = getNodesFromSlots(mon_slots)
    relayNodes = getNodesFromSlots(relay_slots)

    empty = std.vector('std::string')()
    self.monTasks.data = empty
    self.monInfraTasks.data = empty
    self.relayTasks.data = empty
    self.relayInfraTasks.data = empty
    self.monReceivers.data = empty
    self.senders.data = empty
    monStreams = set([])
    for i in self.monStreams.data: monStreams.add(i)

    for j in monNodes:
      for i in self.monInfra.data:
        self.monInfraTasks.data.push_back(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+'/("'+i+'",)')

    for j in relayNodes:
      for i in self.relayInfra.data:
        self.relayInfraTasks.data.push_back(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+'/("'+i+'",)')

    slot = 0
    monStreamsByNode = {}
    monStreamsByType = {}
    for j in xrange(len(self.monTypes.data)):
      for i in xrange(self.monMult.data[j]):
        slot_name = mon_slots[slot]
        typ = self.monTypes.data[j]
        node = slot_name[:slot_name.find(':')]
        item = typ+('_%02d'%i)
        print '--->',i,slot_name,typ,node,item,j
        strm = self.monStreams.data[j]
        if not monStreamsByNode.has_key(node): monStreamsByNode[node] = set([])
        monStreamsByNode[node].add(strm)
        if not monStreamsByType.has_key(strm): monStreamsByType[strm] = set([])
        monStreamsByType[strm].add(node)
        task  = node+'/'+self.name+'_'+node+'_'+item+'/'+item+'/'+typ+'/'+str((strm,))
        self.monTasks.data.push_back(task)
        slot = slot + 1

    res = StreamDescriptor(self.manager,self.storage).getPartition(self.name)
    if not res:
      error('Failed to access storage information for partition '+self.name+' ['+str(self.storage)+']')
      return None

    dp, part_name, slot = res
    part = PartitionInfo(self.manager,part_name)
    part.load(self.manager.devReader())
    recv_nodes = part.recvNodes()
    strm_nodes = part.streamNodes()
    feeders = []
    receivers = []
      
    storageSenders = []
    relayReceivers = []
    storageSenderTypes = {}
    relayReceiverTypes = {}
    for i in monStreams:
      storageSenderTypes[i] = []
      relayReceiverTypes[i] = []
      # Define the sender tasks on the storage layer
      for j in recv_nodes:
        task = [j,self.name+'_'+j+'_SND'+i,i]
        storageSenders.append(task)
        storageSenderTypes[i].append(task)
      # Define the receiver tasks on the relay
      for j in relayNodes:
        task = [j,self.name+'_'+j+'_RCV'+i,i]
        relayReceivers.append(task)
        relayReceiverTypes[i].append(task)

    # Define the receiver tasks on the relay for each stream type
    relayTargets = {}
    for i,nodes in relayReceiverTypes.items():
      relayTargets[i] = []
      for j in nodes: relayTargets[i].append((j[0],j[1]))

    # Define the sources of the receiver tasks for the senders on the storage layer
    storageSources = {}
    for i in storageSenderTypes.keys():
      storageSources[i] = []
      for j in storageSenderTypes[i]: storageSources[i].append((j[0],j[1]))

    # Update task information for the senders on the storage layer
    for i in storageSenders: i.append(relayTargets[i[2]])

    # Update task information for the receivers on the relays
    for i in relayReceivers: i.append(storageSources[i[2]])

    # Define the receiver tasks on monitoring nodes
    # Only send events to node with known stream consumers!
    monReceivers = []
    monTargets = {}
    for i in monNodes:
      if monStreamsByNode.has_key(i):
        task = [i,self.name+'_'+i+'_RCVMon','RCVMon']
        monReceivers.append(task)
        monTargets[i] = task

    # Define the sender tasks on the relay to feed monitoring nodes
    relaySenders = []
    monSources = {}
    print 'Relay Nodes',relayNodes 
    for i,nodes in monStreamsByType.items():
      rcv = []
      print 'MonNodes:',nodes 
      for k in nodes:
        if monStreamsByNode.has_key(k) and monStreamsByNode[k].issuperset([i]):
          rcv.append((monTargets[k][0],monTargets[k][1]))
          if not monSources.has_key(k): monSources[k]=[]
        if len(rcv)>0:
          for j in relayNodes:
            task = [j,self.name+'_'+j+'_SND'+i,'SND'+i,'SND'+i,rcv]
            monSources[k].append((task[0],task[1]))
            relaySenders.append(task)

    # Update task information for the receivers on the monitoring farm
    for i in monReceivers:  i.append(monSources[i[0]])

    # Fill all datapoints:
    for i in storageSenders:
      node,name,type,targets = i
      task = node+'/'+name+'/SND'+type+'/SND'+type+'/'+str(targets)
      self.senders.data.push_back(task)
    for i in relayReceivers:
      node,name,type,sources = i
      task = node+'/'+name+'/RCV'+type+'/RCV'+type+'/'+str(sources)
      self.relayTasks.data.push_back(task)
    for i in relaySenders:
      node,name,short,type,targets = i
      task = node+'/'+name+'/'+short+'/'+type+'/'+str(targets)
      self.relayTasks.data.push_back(task)
    for i in monReceivers:
      node,name,type,sources = i
      task = node+'/'+name+'/'+type+'/'+type+'/'+str(sources)
      self.monReceivers.data.push_back(task)

    # save information to datapoints
    wr = self.manager.devWriter()
    self.addDevices(wr)
    res = wr.execute(0,1)
    showPartition(self,part)
    return res
  
  # ===========================================================================
  def clearTasks(self, writer=None):
    """
    Clear all tasks definition in the storage layer for a given partition.
    """
    empty = std.vector('std::string')()
    self.monTasks.data = empty
    self.monInfraTasks.data = empty
    self.monReceivers.data = empty
    self.relayInfraTasks.data = empty
    self.relayTasks.data = empty
    self.senders.data = empty
    return self.addDevices(writer)

  # ===========================================================================
  def collectTasks(self,class0_tasks,class1_tasks):
    class0_tasks = self._collectTasks(class0_tasks,self.monInfraTasks)
    class0_tasks = self._collectTasks(class0_tasks,self.relayInfraTasks)
    class1_tasks = self._collectTasks(class1_tasks,self.senders)
    class1_tasks = self._collectTasks(class1_tasks,self.monTasks)
    class1_tasks = self._collectTasks(class1_tasks,self.monReceivers)
    class1_tasks = self._collectTasks(class1_tasks,self.relayTasks)
    return (class0_tasks,class1_tasks)

# =============================================================================
class MonitoringInfo(Monitoring,General):
  """ @class MonitoringInfo
  
      Connect to all storage related datapoints in the RunInfo object
      Standalone object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, storage):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    General.__init__(self,manager,name)
    Monitoring.__init__(self,storage)

class MonitoringInfoCreator:
  def __init__(self,manager,storage):
    self.manager = manager
    self.storage = storage
  def create(self,partition):
    return MonitoringInfo(self.manager,partition,self.storage)
  def showPartition(self,info,partition):
    showPartition(info,partition)
