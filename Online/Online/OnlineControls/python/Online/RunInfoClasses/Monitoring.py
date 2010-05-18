import time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.JobOptions.JobOptions as JobOptions
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
class Monitoring(General):
  """ @class Monitoring
  
      Connect to all storage related datapoints in the RunInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, storage, stream):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    General.__init__(self,manager,name,complete=1)
    self.storage = storage
    self.streamManager = stream
    #print '+++++++++++++',self.streamManager.name()

  # ===========================================================================
  def doStreaming(self):
    "Access to the use-flag"
    return self.monFlag.data != 0
  # ===========================================================================
  def numLayer2Slots(self):
    return sum(self.monMult.data)
  # ===========================================================================
  def numLayer1Slots(self):
    return len(self.monStreams.data)
  # ===========================================================================
  def monitoringTypes(self):
    return self.monTypes.data
  # ===========================================================================
  def monitoringMultiplicity(self):
    return self.monMult.data
  # ===========================================================================
  def monitoringStreams(self):
    return self.monStreams.data
  # ===========================================================================
  def defineTasks(self,partition):
    """
         Define all tasks in the storage layer for a given partition.
         The result is storen in runInfo datapoints for further processing.
    """

    relay_slots = partition.recvSlices()
    mon_slots   = partition.streamSlices()
    monNodes    = partition.streamNodesFromSlots()
    relayNodes  = partition.recvNodesFromSlots()
    dimDns      = self.manager.hostName()
    opt = '/'+dimDns+'/'+partition.manager.name()+'/'+partition.name+'/'
    cl0 = '/Class0'+opt
    cl1 = '/Class1'+opt
    cl2 = '/Class2'+opt

    streams = []
    jo_mgr = Systems.controlsMgr(Params.jobopts_system_name)
    #log('Detectors in readout:'+str(self.detectorsInReadout()))
    monStreams = set()
    mon_types  = self.monitoringTypes()
    mon_mult   = self.monitoringMultiplicity()
    mon_stream = self.monitoringStreams()
    for j in xrange(len(mon_types)):
      for i in xrange(mon_mult[j]):
        typ = mon_types[j]
        task = JobOptions.TaskType(jo_mgr,typ)
        if task.exists():
          # Check if the detector is in the readout
          partID = self.partID.data
          detector = task.getDetector()
          det_used = self.isDetectorInReadout(detector)
          if detector.upper()=='ANY' or det_used is not None:
            monStreams.add(mon_stream[j])
            streams.append([typ,mon_stream[j],i])
            if detector.upper()=='ANY':
              log('USE  monitoring task:%-16s [Task started for all detectors]'%(typ,))
            else:
              log('USE  monitoring task:%-16s [Detector %-8s is in the readout]:%s'%(typ,detector,str(det_used)))
          else:
            log('SKIP monitoring task:%-16s [Detector %-8s is NOT in the readout]'%(typ,detector,))
        else:
          error('The task '+typ+' is not defined in the job options database. Failed to define monitoring streams.')
          return None

    monTasks = []
    monStreamsByNode = {}
    monStreamsByType = {}
    #print 'Slots:',mon_slots, mon_slots.size()
    for typ, strm, i in streams:
      slot_name = mon_slots[len(monTasks)]
      node = slot_name[:slot_name.find(':')]
      item = typ+('_%02d'%i)
      if not monStreamsByNode.has_key(node): monStreamsByNode[node] = set([])
      monStreamsByNode[node].add(strm)
      if not monStreamsByType.has_key(strm): monStreamsByType[strm] = set([])
      monStreamsByType[strm].add(node)
      task  = node+'/'+self.name+'_'+node+'_'+item+'/'+item+'/'+typ+cl1+str((strm,))
      monTasks.append(task)

    res = StreamDescriptor(self.streamManager,self.storage).getPartition(self.name)
    if not res:
      error('Failed to access stream information for partition '+self.name+' ['+str(self.storage)+']')
      error('Are you sure the storage slice is allocated for partition '+self.name+' ?')
      return None

    dp, part_name, slot = res
    part = PartitionInfo(self.streamManager,part_name)
    part.load()
    recv_nodes = part.recvNodes()

    relayReceivers = []
    relayReceiverTypes = {}
    for i in monStreams:
      relayReceiverTypes[i] = []
      # Define the receiver tasks on the relay
      for j in relayNodes:
        task = [j,self.name+'_'+j+'_RCV'+i,i]
        relayReceivers.append(task)
        relayReceiverTypes[i].append(task)

    # Define the receiver tasks on the relay for each stream type
    relayTargets = {}
    for i,nodes in relayReceiverTypes.items():
      relayTargets[i] = []
      for j in nodes: relayTargets[i].append((j[0]+'-d1',j[1]))

    storageSenders = []
    storageSenderTypes = {}
    stor_opt = '/Class2/'+dimDns+'/'+self.streamManager.name()+'/'+part.name+'/'
    for i in monStreams:
      storageSenderTypes[i] = []
      # Define the sender tasks on the storage layer
      for j in recv_nodes:
        nam = self.name+'_'+j+'_SND'+i
        task = j+'/'+nam+'/SND'+i+'/SND'+i+stor_opt+str(relayTargets[i])
        storageSenders.append(task)
        storageSenderTypes[i].append([j,nam,i])
        
    # Define the sources of the receiver tasks for the senders on the storage layer
    storageSources = {}
    for i in storageSenderTypes.keys():
      storageSources[i] = []
      for j in storageSenderTypes[i]: storageSources[i].append((j[0]+'-d1',j[1]))

    # Define the receiver tasks on monitoring nodes
    # Only send events to node with known stream consumers!
    monReceivers = []
    monTargets = {}
    for i in monNodes:
      if monStreamsByNode.has_key(i):
        nam = self.name+'_'+i+'_RCVMon'
        monReceivers.append([i,nam])
        monTargets[i] = [(i+'-d1',nam)]

    # Define the sender tasks on the relay to feed monitoring nodes
    relaySenders = []
    monSources = {}
    # print 'Relay Nodes',relayNodes 
    for i,nodes in monStreamsByType.items():
      rcv = 0
      # print 'MonNodes:',nodes 
      for k in nodes:
        if monStreamsByNode.has_key(k) and monStreamsByNode[k].issuperset([i]):
          if not monSources.has_key(k): monSources[k]=[]
          for j in relayNodes:
            nick = 'SND'+i+'_'+k
            nam  = self.name+'_'+j+'_'+nick
            task = j+'/'+nam+'/'+nick+'/SND'+i+cl2+str(monTargets[k])
            monSources[k].append((j+'-d1',nam))
            relaySenders.append(task)

    # Fill all datapoints:

    # First the senders on the storage system
    partition.setDataSources(storageSenders)

    # Update Relay infrastructure
    infra = []
    for j in relayNodes:
      for i in self.relayInfra.data:
        infra.append(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+cl0+'("'+i+'",)')
    partition.setRecvInfrastructure(infra)

    # Update task information for the receivers on the relays
    rcv = []
    for i in relayReceivers:
      node, name, type = i
      rcv.append(node+'/'+name+'/RCV'+type+'/RCV'+type+cl1+str(storageSources[i[2]]))
    partition.setRecvReceivers(rcv)

    # Update relay receivers
    partition.setRecvSenders(relaySenders)

    # Update Monfarm infrastructure
    infra = []
    for j in monNodes:
      for i in self.monInfra.data:
        infra.append(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+cl0+'("'+i+'",)')
    partition.setStreamInfrastructure(infra)

    # Update task information for the receivers on the monitoring farm
    rcv = []
    for i in monReceivers:
      node, nam = i
      rcv.append(node+'/'+nam+'/RCVMon/RCVMon'+cl1+str(monSources[node]))
    partition.setStreamReceivers(rcv)

    partition.setStreamSenders(monTasks)
    if partition.saveTasks():
      tasks = partition.collectTasks(tasks={},with_data_sources=1)
      return tasks
    return None

# =============================================================================
class MonitoringInfoCreator:
  """ @class MonitoringInfoCreator
  
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,storage):
    self.storage = storage
  def create(self,rundp_name,partition):
    items = rundp_name.split(':')
    mgr = Systems.controlsMgr(items[0])
    sto = Systems.controlsMgr(Params.storage_system_name)
    return Monitoring(mgr,partition,self.storage,sto)
  def showPartition(self,partition,extended=None):
    import Online.Streaming.PartitionInfo as Info
    partition.show(method=Info.showMonitoring,extended=extended)
