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
    General.__init__(self,manager,name,complete=None,postfix='')
    self.addBasic()
    self.addStorage()
    self.addHLT()
  # ===========================================================================
  def doStreaming(self):
    "Access to the use-flag"
    return self.storeFlag.data != 0
  # ===========================================================================
  def numLayer2Slots(self):
    return self.nSubFarm.data  
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
    recvNodes = partition.recvNodesFromSlots()
    strmNodes = partition.streamNodesFromSlots()
    opt = '/'+self.manager.hostName()+'/'+partition.manager.name()+'/'
    cl0 = '/Class0'+opt
    cl1 = '/Class1'+opt
    cl2 = '/Class2'+opt


    partition.setDataSources([])
    tasks = []
    pn = self.partitionName()
    print '---------------------- Partition name is:',pn
    for i in xrange(len(recv_slots)):
      slot = recv_slots[i]
      node = slot[:slot.find(':')]
      sub_farm = 'SF%02d'%(i,)
      short_name = 'Send_'+sub_farm
      task = pn+'_'+node+'_'+short_name
      tasks.append(node+'/'+task+'/'+short_name+'/RecStorageSend'+cl1+'("'+sub_farm+'",'+str(i)+',)')
    partition.setRecvSenders(tasks)
    tasks = []
    for i in xrange(len(strm_slots)):
      slot = strm_slots[i]
      node = slot[:slot.find(':')]
      sub_farm = 'SF%02d'%(i,)
      short_name = 'Recv_'+sub_farm
      task = pn+'_'+node+'_'+short_name
      tasks.append(node+'/'+task+'/'+short_name+'/RecStorageRecv'+cl1+'("'+sub_farm+'",'+str(i)+',)')
    partition.setStreamReceivers(tasks)
    cnt = 0
    tasks = []
    infra = []
    for j in recvNodes:
      tasks.append(j+'/'+pn+'_'+j+'_Reader/Reader/RecStorageRead'+cl1+'[]')
      for i in self.rcvInfra.data:
        infra.append(j+'/'+pn+'_'+j+'_'+i+'/'+i+'/'+i+cl0+'("'+str(cnt)+'",)')
      cnt = cnt + 1
    partition.setRecvInfrastructure(infra)
    partition.setRecvReceivers(tasks)
    cnt = 0
    tasks = []
    infra = []
    for j in strmNodes:
      tasks.append(node+'/'+pn+'_'+node+'_Writer/Writer/RecStorageWrite'+cl1+'[]')
      for i in self.strInfra.data:
        infra.append(j+'/'+pn+'_'+j+'_'+i+'/'+i+'/'+i+cl0+'("'+str(cnt)+'",)')
      cnt = cnt + 1
    partition.setStreamInfrastructure(infra)
    partition.setStreamSenders(tasks)
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
    print 'Creating StorageInfoCreator',items[0],rundp_name,partition
    mgr = Systems.controlsMgr(items[0])
    return StorageInfo(mgr,items[1])
  def showPartition(self,partition,extended=None):
    import Online.Streaming.PartitionInfo as Info
    partition.show(method=Info.showRecStorage,extended=extended)
