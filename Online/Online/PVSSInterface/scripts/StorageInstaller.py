import time
import Online.PVSS    as PVSS
import Online.Utils   as Utils
import Online.RunInfo as RunInfo
std = PVSS.gbl.std
PVSS.logPrefix = 'PVSS Control '
log            = Utils.log
error          = Utils.error
warning        = Utils.warning
DP = PVSS.DataPoint

# =============================================================================
class Installer:
  """ @class Installer

      This class initializes the data structure for the storage control
      
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, mgr, dev='Control'):
    "Default constructor"
    self.numSubFarm  = 50
    self.numPartition = 16
    self.numStreamPerNode = 10
    self.recvNodes = ['storerecv01', 'storerecv02', 'storerecv03', 'storerecv04', 'storerecv05']
    self.strmNodes = ['storestrm01', 'storestrm02', 'storestrm03', 'storestrm04']
    self.name = dev
    self.partID = 1
    self.manager = mgr
    self.dps = PVSS.DataPointVector()

  # ===========================================================================
  def set(self, dp, value):
    "Set datapoint value in storage definition"
    self.dps.push_back(DP(self.manager,DP.original(self.name+'.'+dp)))
    self.dps.back().data = value
    return self

  # ===========================================================================
  def write(self,prefix=''):
    "Write information to datapoints"
    wr = self.manager.devWriter()
    for i in self.dps: wr.add(i)
    if wr.execute(1,1):
      log(prefix+'All DPs successfully created.')
    else:
      error(prefix+'Failed to set DP values.')
    self.dps.clear()
    return self
      
  # ===========================================================================
  def setRunInfo(self, info, dp, value):
    "Set datapoints in the corresponding RunInfo structure"
    self.dps.push_back(DP(self.manager,DP.original(info+'_RunInfo.'+dp)))
    self.dps.back().data = value
    return self
    
  # ===========================================================================
  def createPartition(self, name):
    "Create a new partition"
    empty = std.vector('std::string')()
    dm = self.manager.deviceMgr()
    typ = self.manager.typeMgr().type('StoragePartition')
    device = dm.createDevice(name,typ,1)
    if device.get() is None:
      warning('Failed to create device "'+name+'"')
    device = dm.device(name)
    if device.get() is None:
      error('Failed to access device "'+name+'"')
      return (None,None)
    self.dps.push_back(DP(self.manager,DP.original(name+'.InUse')))
    self.dps.back().data = 0
    self.dps.push_back(DP(self.manager,DP.original(name+'.Name')))
    self.dps.back().data = ''
    self.dps.push_back(DP(self.manager,DP.original(name+'.RecvNodes')))
    self.dps.back().data = empty
    self.dps.push_back(DP(self.manager,DP.original(name+'.RecvSlices')))
    self.dps.back().data = empty
    self.dps.push_back(DP(self.manager,DP.original(name+'.StreamNodes')))
    self.dps.back().data = empty
    self.dps.push_back(DP(self.manager,DP.original(name+'.StreamSlices')))
    self.dps.back().data = empty
    return device

  # ===========================================================================
  def create(self):
    recvNodes = std.vector('std::string')()
    strmNodes = std.vector('std::string')()
    for i in self.recvNodes: recvNodes.push_back(i)
    for i in self.strmNodes: strmNodes.push_back(i)
    dm     = self.manager.deviceMgr()
    typ    = self.manager.typeMgr().type('StorageControl')
    device = dm.createDevice(self.name,typ,1)
    if device.get() is None:
      warning('Failed to create device "'+self.name+'"')
    partitions = PVSS.DpIDVector()
    errs = 0
    self.dps.clear()
    for i in xrange(self.numPartition):
      device = self.createPartition(self.name+'_Partition_%02X'%(i,))
      if ( device ):
        partitions.push_back(device.id())
      else:
        error("Errors occurred during device creation.")
        return
    self.set('Command','unload')
    self.set('State','UNKNOWN')
    self.set('PartitionDesc',partitions)
    self.set('InUse',std.vector('int')(16,0))
    self.set('ActivePartitions',std.vector('std::string')())
    slices = std.vector('std::string')(self.numSubFarm,'')
    nodes = {}
    for i in xrange(self.numSubFarm):
      node = self.recvNodes[i%len(self.recvNodes)]
      if not nodes.has_key(node): nodes[node] = 0
      nodes[node] = nodes[node] + 1
      slices[i] = node+":%02d"%nodes[node]
    self.set('RecvNodes',recvNodes)
    self.set('RecvSlices',slices)
    length = len(self.strmNodes)*self.numStreamPerNode
    slices.resize(length,'')
    nodes = {}
    for i in xrange(length):
      node = self.strmNodes[i%len(self.strmNodes)]
      if not nodes.has_key(node): nodes[node] = 0
      nodes[node] = nodes[node] + 1
      slices[i] = node+":%02d"%nodes[node]
    self.set('StreamNodes',strmNodes)
    self.set('StreamSlices',slices)
    return self.write(prefix='StorageControl('+self.name+'): ')

  # ===========================================================================
  def createRunInfo(self, name, stream_creator,nf):
    import random
    self.partID = self.partID*10
    dm     = self.manager.deviceMgr()
    typ    = self.manager.typeMgr().type('RunInfo')
    device = dm.createDevice(name+'_RunInfo',typ,1)
    if device.get() is None:
      warning('Failed to create device "'+self.name+'"')
    sv = std.vector('std::string')()
    sf = std.vector('std::string')()
    dw = std.vector('std::string')()
    self.setRunInfo(name,'general.partId',self.partID)
    self.setRunInfo(name,'general.runType','PHYSICS')
    self.setRunInfo(name,'general.runNumber',random.randint(0,100000))
    self.setRunInfo(name,'general.runNTriggers',0)
    
    self.setRunInfo(name,'Storage.storeFlag',1)
    for i in xrange(nf):
      sf.push_back('lbhlt%02d'%i)
      for j in xrange(30):
        sv.push_back('lbhlt%03d%02d'%(i,j))
    self.setRunInfo(name,'HLTFarm.nodeList',sv)
    self.setRunInfo(name,'HLTFarm.nSubFarms',nf)
    self.setRunInfo(name,'HLTFarm.subFarms',sf)
    self.setRunInfo(name,'HLTFarm.receiverTasks',dw)
    dw, dwmult = stream_creator()
    empty = std.vector('std::string')()
    emptymult = std.vector('int')()
    infrastructure = std.vector('std::string')()
    infrastructure.push_back('ErrSrv')
    infrastructure.push_back('StoreStream_MBM')
    self.setRunInfo(name,'Storage.streamTypes',dw)
    self.setRunInfo(name,'Storage.streamWriters',empty)
    self.setRunInfo(name,'Storage.streamMultiplicity',dwmult)
    self.setRunInfo(name,'Storage.streamInfrastructure',infrastructure)
    self.setRunInfo(name,'Storage.streamInfrastructureTasks',empty)
    self.setRunInfo(name,'Storage.recvSenders',empty)
    infrastructure[infrastructure.size()-1] = 'StoreRecv_MBM'
    self.setRunInfo(name,'Storage.recvInfrastructure',infrastructure)
    self.setRunInfo(name,'Storage.recvInfrastructureTasks',empty)
    return self.write(prefix='Runinfo('+name+'): ')

# =============================================================================
def LHCb_streams():
  dw = std.vector('std::string')()
  dw.push_back('Higgs')
  dw.push_back('B2JPsi')
  dw.push_back('B2Dstar')
  dw.push_back('B2Charm')
  dw.push_back('BBinclusive')
  dwmult = std.vector('int')()
  for i in xrange(len(dw)):
    if i==3:   dwmult.push_back(2)
    elif i==4: dwmult.push_back(5)
    else:      dwmult.push_back(1)
  return (dw,dwmult)

# =============================================================================
def subdetector_single_streams():
  dw = std.vector('std::string')()
  dw.push_back('Events')
  dwmult = std.vector('int')()
  dwmult.push_back(1)
  return (dw,dwmult)

# =============================================================================
def subdetector_double_streams():
  dw = std.vector('std::string')()
  dw.push_back('Events')
  dwmult = std.vector('int')()
  dwmult.push_back(2)
  return (dw,dwmult)

# =============================================================================
def install(name='TestStorage'):
  c = PVSS.controlsMgr()
  inst = Installer(c,name).create()
  inst.createRunInfo('LHCb',LHCb_streams,35)
  inst.createRunInfo('MUON',subdetector_double_streams,5)
  inst.createRunInfo('VELO',subdetector_double_streams,8)
  inst.createRunInfo('RICH',subdetector_double_streams,2)
  inst.createRunInfo('PRS', subdetector_single_streams,1)
  inst.createRunInfo('ECAL',subdetector_single_streams,1)
  return c

createTestCmds = """
import StorageInstaller
mgr=StorageInstaller.install()
"""

if __name__ == "__main__":
  install()
