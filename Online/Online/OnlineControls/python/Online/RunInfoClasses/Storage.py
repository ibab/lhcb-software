import time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems

from Online.RunInfoClasses.General import General as General
log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std

# ===========================================================================
def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()


# =============================================================================
class StorageInfo(General):
  """ @class Storage
  
      Connect to all storage related datapoints in the RunInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    General.__init__(self,manager,name,complete=1)
  # ===========================================================================
  def doStreaming(self):
    "Access to the use-flag"
    return self.storeFlag.data != 0
  # ===========================================================================
  def numLayer2Slots(self):
    return sum(self.strMult.data)
  # ===========================================================================
  def numLayer1Slots(self):
    return self.nSubFarm.data 
  # ===========================================================================
  def defineTasks(self,partition):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.
    
    """
    recv_slots = partition.recvSlices()
    strm_slots = partition.streamSlices()
    streams = []
    #
    # Need to change order in this loop to better distribute the tasks
    # in the storage layers:
    #
    node_slots = {}
    len_strm_slots = len(strm_slots)
    for i in xrange(len_strm_slots):
      slot = strm_slots[i]
      node = slot[:slot.find(':')]
      if node_slots.has_key(node):
        node_slots[node].append(slot)
      else:
        node_slots[node] = [slot]
    strm_slots = []
    num_slots = 0
    while num_slots < len_strm_slots:
      for i in node_slots.keys():
        if len(node_slots[i])>0:
          strm_slots.append(node_slots[i][0])
          num_slots = num_slots + 1
          del node_slots[i][0]

    num_streams = 0
    for j in xrange(len(self.streams.data)):
      for i in xrange(self.strMult.data[j]):
        streams.append([self.streams.data[j],i])
        num_streams = num_streams + 1

    # Check of streams and slots fit....
    if len(strm_slots) != num_streams:
      PVSS.error('FATAL: Partition: %s The number of stream slots does not %s'%
                 (self.name,'match the number of data streams.',),
                 timestamp=1,type=PVSS.ILLEGAL_VALUE)
      PVSS.error('FATAL: Partition: %s Num data stream: %d != Num slots: %d'%
                 (self.name, len(strm_slots), num_streams,),
                 timestamp=1,type=PVSS.ILLEGAL_VALUE)
      return None

    """          
    multiplicity = [0 for i in self.strMult.data]
    sum_mult_total = 0
    for i in xrange(len(self.strMult.data)):
      sum_mult_total = sum_mult_total + self.strMult.data[i]
    sum_mult = 0
    while sum_mult<sum_mult_total:
      for j in xrange(len(self.streams.data)):
        if multiplicity[j] < self.strMult.data[j]:
          streams.append([self.streams.data[j],multiplicity[j]])
          multiplicity[j] = multiplicity[j] + 1
          sum_mult = sum_mult + 1
    """
    #
    # This formally does the same thing as the above, but the
    # order is different and hence the tasks are unequally distributed:
    #
    #for j in xrange(len(self.streams.data)):
    #  for i in xrange(self.strMult.data[j]):
    #    streams.append([self.streams.data[j],i])
    #
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
    opt = '/'+dimDns+'/'+partition.manager.name()+'/'+partition.name+'/'
    cl0 = '/Class0'+opt
    cl1 = '/Class1'+opt
    cl2 = '/Class2'+opt
    for i in xrange(len(recv_slots)):
      slot = recv_slots[i]
      node = slot[:slot.find(':')]
      sub_farm = 'SF%02d'%(i,)
      short_name = sub_farm+'_HLT'
      task = self.name+'_'+node+'_'+short_name
      recvReceivers.append(node+'/'+task+'/'+short_name+'/HLTRec'+cl1+'("'+sub_farm+'",)')
      dataSources.append(sub_farm+'/'+self.name+'_'+sub_farm+'_Sender/'+sub_farm+'_Sender/HLTSend'+cl2+'("'+node+','+task+'",)')

    # Here we bind the various slots to the streams:
    for i in xrange(len(strm_slots)):
      slot = strm_slots[i]
      node = slot[:slot.find(':')]
      item = streams[i][0]+('_%02d'%streams[i][1])
      short_name = 'WRT'+item
      task = self.name+'_'+node+'_WRT'+item
      strmSenders.append(node+'/'+task+'/'+short_name+'/WRT'+streams[i][0]+cl2+'("'+item+'",)')
      short_name = 'RCV'+item
      task = self.name+'_'+node+'_RCV'+item
      strmReceivers.append(node+'/'+task+'/'+short_name+'/RCV'+streams[i][0])
      streamers.append([self.name,node,slot,'%02d'%streams[i][1],streams[i][0]])

    # Add for each of the stream layer nodes the required infrastructure tasks
    for j in strmNodes:
      for i in self.strInfra.data:
        strmInfrastructure.append(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+cl0+'("'+i+'",)')
    rcv_targets = {}
    for i in xrange(len(streamers)):
      rcv_targets[i] = []
    for j in recvNodes:
      for i in xrange(len(streamers)):
        part,node,slot,ident,type = streamers[i]
        short_name = type+'_'+ident
        sender = part+'_'+j+'_SND'+short_name
        target = part+'_'+node+'_RCV'+short_name
        rcv_targets[i].append((j+'-d1',sender))
        recvSenders.append(j+'/'+sender+'/SND'+short_name+'/SND'+type+cl2+'[("'+node+'-d1","'+target+'")]')
      for i in self.rcvInfra.data:
        recvInfrastructure.append(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+cl0+'("'+i+'",)')

    for i,data in rcv_targets.items():
      strmReceivers[i] = strmReceivers[i]+cl1+str(data)

    if self.storeFlag.data == 0:
      # If we do not store the data, the streaming layer
      # does not have to be instrumented.
      recvSenders = PVSS.StringVector()
      strmInfrastructure = PVSS.StringVector()
      strmReceivers = PVSS.StringVector()
      strmSenders = PVSS.StringVector()
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
class StorageInfoCreator:
  def __init__(self):
    pass
  def create(self,rundp_name,partition):
    items = rundp_name.split(':')
    # partition = items[1][:items[1].find('_')]
    mgr = Systems.controlsMgr(items[0])
    return StorageInfo(mgr,partition)
  def showPartition(self,partition,extended=None):
    import Online.Streaming.PartitionInfo as Info
    partition.show(method=Info.showStorage,extended=extended)
