import time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.JobOptions.JobOptions as JobOptions
from   Online.RunInfoClasses.General import General as General
from   Online.Streaming.PartitionInfo import PartitionInfo as PartitionInfo
from   Online.Streaming.StreamingDescriptor import StreamingDescriptor as StreamDescriptor

log        = Utils.log
error      = Utils.error
std        = PVSS.gbl.std
printSlots = Utils.printSlots

# =============================================================================
def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()

# =============================================================================
class Reconstruction(General):
  """ @class Reconstruction
  
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
    General.__init__(self,manager,name)
    self.storage = storage
    self.streamManager = stream
    self.addReconstruction()

  # ===========================================================================
  def detectorName(self):
    return 'LHCb'
  # ===========================================================================
  def doStreaming(self):
    "Access to the use-flag"
    return self.recFlag.data != 0
  # ===========================================================================
  def numLayer2Slots(self):
    num = int(self.recWorker.data.split('/')[1])
    print 'NumLayer2 slots:',num
    return num
  # ===========================================================================
  def numLayer1Slots(self):
    num = len(self.recStreams.data)
    print 'NumLayer1 slots:',num
    return 20
  # ===========================================================================
  def monitoringTypes(self):
    return [self.recWorker.data.split('/')[0]]
  # ===========================================================================
  def monitoringStreams(self):
    return self.recStreams.data
  # ===========================================================================
  def defineTasks(self,partition):
    """
         Define all tasks in the storage layer for a given partition.
         The result is storen in runInfo datapoints for further processing.
    """
    import traceback
    relay_slots = partition.recvSlices()
    mon_slots   = partition.streamSlices()
    monNodes    = partition.streamNodesFromSlots()
    relayNodes  = partition.recvNodesFromSlots()
    dimDns      = self.manager.hostName()
    opt = '/'+dimDns+'/'+partition.manager.name()+'/'+partition.name
    cl0 = '/Class0'+opt+'/("",)'
    cl1 = '/Class1'+opt+'/("",)'
    cl2 = '/Class2'+opt

    res = StreamDescriptor(self.streamManager,self.storage).getPartition(self.name)
    if not res:
      error('Failed to access stream information for partition '+self.name+' ['+str(self.storage)+']')
      error('Are you sure the storage slice is allocated for partition '+self.name+' ?')
      return None

    dp, part_name, slot = res
    part = PartitionInfo(self.streamManager,part_name)
    part.load()
    recv_nodes = part.recvNodes()

    storageTasks = []              # Define the sender tasks on the storage layer
    stor_opt1 = '/Class1/'+dimDns+'/'+self.streamManager.name()+'/'+part.name
    stor_opt2 = '/Class2/'+dimDns+'/'+self.streamManager.name()+'/'+part.name
    for j in recv_nodes:
      cnt = 0
      for i in self.recStreams.data:
        cnt = cnt + 1
        for k in relayNodes:
          rel_task = self.name+'_'+k+'_RCV_'+j
          storageTasks.append(j+'/'+self.name+'_'+j+'_SND_'+k+'_'+str(cnt)+'/Send_'+i+'/SND_'+i+stor_opt2+'/(("'+k+'-d1","'+rel_task+'"),)')
      storageTasks.append(j+'/'+self.name+'_'+j+'_RCV_Rec/Recv_'+j+'/RCV'+stor_opt1+'/("",)')
    
    # Define the infrastructure, receiver and sender tasks on the relay
    relayInfra=[]
    relaySenders=[]
    relayReceivers=[]
    for j in relayNodes:
      for r in recv_nodes:
        relayReceivers.append(j+'/'+self.name+'_'+j+'_RCV_'+r+'/Receiver_'+r+'/RCV'+cl1)
      for k in monNodes:
        relaySenders.append(j+'/'+self.name+'_'+j+'_SND_'+k+'/Send_'+k+'/SND'+cl2+'/(("'+k+'-d1","'+self.name+'_'+k+'_RCV'+'"),)')
      for d in self.recRelayInfra.data:
        i,mul = d.split('/')
        for k in xrange(int(mul)):
          relayInfra.append(j+'/'+self.name+'_'+j+'_'+i+'_'+str(k)+'/'+i+'_'+str(k)+'/'+i+cl0)

    # Define monitoring infrastructure
    monInfra = []
    monSenders = []
    cnt=0
    for j in monNodes:
      for d in self.recNodeInfra.data:
        i,mul = d.split('/')
        for k in xrange(int(mul)):
          monInfra.append(j+'/'+self.name+'_'+j+'_'+i+'_'+str(k)+'/'+i+'_'+str(k)+'/'+i+cl0)
      for d in self.recStrmInfra.data:
        i,mul = d.split('/')
        for k in xrange(int(mul)):
          monInfra.append(j+'/'+self.name+'_'+j+'_'+i+'_'+str(k)+'/'+i+'_'+str(k)+'/'+i+cl0)
      cnt = cnt+1
      rcv_node = recv_nodes[cnt%len(recv_nodes)]
      rcv_task = self.name+'_'+rcv_node+'_RCV_Rec'
      monSenders.append(j+'/'+self.name+'_'+j+'_SND_'+rcv_node+'/Sender/SND_Rec2Storage'+cl2+'/(("'+rcv_node+'","'+rcv_task+'"),)')
      # Not really senders, but it works....
      monSenders.append(j+'/'+self.name+'_'+j+'_RCV/Receiver/RCV'+cl1)

    # Define reconstruction tasks: treat them like receivers (they are all class 1 tasks)
    monReceivers = []
    worker = self.monitoringTypes()[0]
    for s in mon_slots:
      j,k=s.split(':')
      monReceivers.append(j+'/'+self.name+'_'+j+'_'+worker+'_'+str(k)+'/'+worker+'_'+str(k)+'/'+worker+cl1)

    # Fill all datapoints:
    # First the senders on the storage system
    partition.setDataSources(storageTasks)
    # Update Relay infrastructure
    partition.setRecvInfrastructure(relayInfra)
    # Update task information for the receivers on the relays
    partition.setRecvReceivers(relayReceivers)
    # Update relay receivers
    partition.setRecvSenders(relaySenders)
    # Update Monfarm infrastructure
    partition.setStreamInfrastructure(monInfra)
    # Update task information for the receivers on the monitoring farm
    partition.setStreamReceivers(monReceivers)
    partition.setStreamSenders(monSenders)

    if partition.saveTasks():
      tasks = partition.collectTasks(tasks={},with_data_sources=1)
      return tasks
    return None

# =============================================================================
class InfoCreator:
  """ @class ReconstructionInfoCreator
  
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
    return Reconstruction(mgr,partition,self.storage,sto)
  def showPartition(self,partition,extended=None):
    import Online.Streaming.PartitionInfo as Info
    partition.show(method=Info.showReconstruction,extended=extended)
