import time
import Online.PVSS    as PVSS
import Online.Utils   as Utils
from   Online.RunInfo import RunInfo as RunInfo
from Online.Storage.PartitionInfo import PartitionInfo as PartitionInfo
from Online.Storage.StorageDescriptor import StorageDescriptor as StorageDescriptor

sum            = Utils.sum
log            = Utils.log
error          = Utils.error
warning        = Utils.warning
printSlots     = Utils.printSlots
std            = PVSS.gbl.std
DeviceSensor   = PVSS.DeviceSensor
DeviceListener = PVSS.PyDeviceListener
PVSS.logPrefix = 'PVSS Control '

DataPoint = PVSS.DataPoint
debug = 0

# =============================================================================
class Creator:
  """ @class Control

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Storage control datapoint

        @return reference to initialized object
    """
    self.manager = manager
    self.name = name

      

  # ===========================================================================
  def _usePartition(self,datapoint,partition):
    log('Using partition: '+partition,timestamp=1)
    use = DataPoint(self.manager,DataPoint.original(datapoint+'.InUse'))
    use.data = 1
    self.writer.add(use)
    nam = DataPoint(self.manager,DataPoint.original(datapoint+'.Name'))
    nam.data = partition
    self.writer.add(nam)
    self.active.data.push_back(datapoint)
    self.writer.add(self.active)
    if self.writer.execute(0,1):
      return datapoint
    error('Failed to allocate partition:'+datapoint+' for '+partition)
    return None

  # ===========================================================================
  def getPartition(self, partition):
    "Select partition by allocation name."
    actor = PVSS.DpVectorActor(self.manager)
    typ   = self.manager.typeMgr().type('StoragePartition')
    actor.lookup(DataPoint.original(self.name+'_Partition*.Name'),typ)
    for i in actor.container:
      self.reader.add(i)
    if self.load():
      for i in actor.container:
        nam = i.name()
        nam = nam[nam.find(':')+1:nam.find('.')]
        id  = int(nam[len(nam)-2:],16)
        if i.data == partition:
          return (DataPoint(self.manager,i.name()), nam, id)
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
      error('Failed to allocate sufficient RECEIVE slots. Got only %s slots, but requested %d.'%(num,num_recv_slots))
      return None
    all_recv_slots,all_recv_nodes,rem_recv_slices = self._selectSlices(slices,recvSlices)

    policy = self.strmAllocationPolicy(False)
    strmSlices = self.strmSlices.data
    num,slices = policy.allocateSlices(num_strm_slots,strm_slots_per_node,strmSlices)
    if num < num_strm_slots:
      error('Failed to allocate sufficient STREAMER slots. Got only %s slots, but requested %d.'%(num,num_strm_slots))
      return None
    all_strm_slots,all_strm_nodes,rem_strm_slices = self._selectSlices(slices,strmSlices)

    dp = self.allocPartition(partition)
    if dp is not None:
      info = PartitionInfo(self.manager,dp)
      self.recvSlices.data = rem_recv_slices
      self.strmSlices.data = rem_strm_slices
      info.datapoints[3].data = all_recv_nodes   # RecvNodes
      info.datapoints[4].data = all_recv_slots   # RecvSlices
      info.datapoints[5].data = all_strm_nodes   # StreamNodes
      info.datapoints[6].data = all_strm_slots   # StreamSlices
      self.writer.add(self.recvSlices)
      self.writer.add(self.strmSlices)
      self.writer.add(info.datapoints[3])
      self.writer.add(info.datapoints[4])
      self.writer.add(info.datapoints[5])
      self.writer.add(info.datapoints[6])
      if self.writer.execute(0,1):
        return (all_recv_slots,all_strm_slots)
    return None
  
  # ===========================================================================
  def allocate(self,partition,recv_slots_per_node,strm_slots_per_node):
    """
    Allocate slots in the receiving and streaming layer of the storage system.
    Note: All input data must already be read.

    @param  partition           Partition name
    @param  recv_slots_per_node Number of receiving layer slots to be allocated at once per node (if possible)
    @param  strm_slots_per_node Number of streaming layer slots to be allocated at once per node (if possible)

    @return None on failure, on Succes tuple (all_recv_slots,all_strm_slots) with allocated slots
    """
    existing = self.getPartition(partition)
    if existing is not None:
      error('[Partition already allocated] Cannot allocate partition for:'+partition)
      return None
    runInfo = RunInfo(self.manager,partition).load()
    if runInfo.flag.data == 0:
      warning('Storage flag is not set. No need to allocate resources on the streaming layer for:'+partition)
      return None
    nStreams = sum(runInfo.strMult.data)
    nSubFarm = runInfo.nSubFarm.data
    res = self.allocateSlots(partition,nSubFarm,recv_slots_per_node,nStreams,strm_slots_per_node)
    if res is not None:
      # Allocation was successful: Now update RunInfo table
      recv_slots,strm_slots = res
      if runInfo.defineTasks(recv_slots,strm_slots):
        return (recv_slots,strm_slots)
    return None  
    
  # ===========================================================================
  def free(self, partition):
    "Free partition and all related content."
    res = self.getPartition(partition)
    if res is not None:
      dp, nam, id = res
      info = PartitionInfo(self.manager,nam)
      if info.load(self.reader):
        for i in info.datapoints[4].data:
          self.recvSlices.data.push_back(i)
        for i in info.datapoints[6].data:
          self.strmSlices.data.push_back(i)
        info.datapoints[3].data.clear()
        info.datapoints[4].data.clear()
        info.datapoints[5].data.clear()
        info.datapoints[6].data.clear()
        self.writer.add(self.recvSlices)
        self.writer.add(self.strmSlices)
        self.writer.add(info.datapoints[3])
        self.writer.add(info.datapoints[4])
        self.writer.add(info.datapoints[5])
        self.writer.add(info.datapoints[6])
        runInfo = RunInfo(self.manager,partition)
        runInfo.clearTasks(self.writer)
        if self.freePartition(res):
          return 1
        return 0
    error('[Partition not allocated] Cannot free partition for '+partition)
    return 2

  # ===========================================================================
  def showSummary(self, extended=None):
    actor = PVSS.DpVectorActor(self.manager)
    typ   = self.manager.typeMgr().type('StoragePartition')
    actor.lookup(DataPoint.original(self.name+'_Partition*.Name'),typ)
    partitions = []
    log('Storage Control contains a total of '+str(len(actor.container))+' partitions',timestamp=1)
    for i in xrange(len(actor.container)):
      nam = actor.container[i].name()
      nam = nam[nam.find(':')+1:nam.find('.')]
      partitions.append(PartitionInfo(self.manager,nam))
    for i in partitions:
      self.reader.add(i.datapoints)
    self.load()
    StorageDescriptor.show(self,extended)
    return partitions
  # ===========================================================================
  def show(self, extended=None):
    partitions = self.showSummary(self)
    for i in partitions: i.show(extended=extended)
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1
  def makeAnswer(self,status,msg):
    self.state.data = status + msg
    self.writer.add(self.state)
    if self.writer.execute(0,1):
      return 1
    return 0
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    #print 'device changed.'
    cmd = ''
    try:
      nam = self.dp().name()
      cmd = self.dp().value().data()
      log('Command received:'+nam[:nam.find(':')]+' -> '+cmd,timestamp=1)
      itms = cmd.split('/')
      if len(itms) == 4:
        command   = itms[0]
        storage   = itms[1]
        partition = itms[2]
        partID    = itms[3]
        answer = '/'+storage+'/'+partition+"/"+str(partID)
        result = None
        if storage == self.name:
          try:
            if command == "ALLOCATE":
              result = self.allocate(partition,5,2)
            elif command == "DEALLOCATE":
              result = self.free(partition)
            elif command == "RECOVER":
              result = self.free(partition)
              if result == 2:
                return self.makeAnswer('READY',answer)
              result = self.allocate(partition,5,2)
            if result is not None:
              return self.makeAnswer('READY',answer)
            log('The command:"'+cmd+'" failed. [Internal Error] ',timestamp=1)
            return self.makeAnswer('ERROR',answer)
          except Exception,X:
            log('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
          except:
            log('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
        log('The command:"'+cmd+'" failed. [Bad StorageSystem] '+storage,timestamp=1)
        return self.makeAnswer('ERROR',answer)
      log('The command:"'+cmd+'" failed. [Insufficient parameters] ',timestamp=1)
      return 0
    except Exception,X:
      log('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
      traceback.print_exc()
      return 0
    except:
      log('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
      traceback.print_exc()
    return 0
 
# =============================================================================
def runTest():
  c = PVSS.controlsMgr()
  s = Control(c,'TestStorage')
  return (c,s)

# =============================================================================
if __name__ == "__main__":
  mgr,ctrl = runTest()
  ctrl.show()
  
runInfoCmds = """
import StorageInstaller
StorageInstaller.install()

import Online.PVSS as PVSS
from   Online.RunInfo import RunInfo as RunInfo
import StorageControl

mgr = PVSS.controlsMgr()
ctrl = StorageControl.Control(mgr,'TestStorage')
PVSS.setDebug(2)
sensor=PVSS.DeviceSensor(mgr,ctrl.point)
sensor.addListener(ctrl)
sensor.run(1)

ctrl.show()
info=RunInfo(mgr,'LHCb')
info.show()


ctrl.allocate('MUON',5,2)
ctrl.free('MUON')

reload(StorageControl)
mgr,ctrl = StorageControl.runTest()
ctrl.free('MUON')
ctrl.allocate('MUON',5,2)

"""
runTestCmds = """
import StorageControl
mgr,ctrl=StorageControl.runTest()
"""
