"""
Install the Online Storage Control components

Commands:

import Online.Streaming.StorageInstaller as SI
mgr=SI.install('Storage')

Author: M.Frank

"""

import time
import Online.PVSS        as PVSS
import Online.Utils       as Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
from Online.InstallerBase import InstallerBase as InstallerBase
PVSS.logPrefix = 'PVSS Control '
std            = PVSS.gbl.std
log            = Utils.log
error          = Utils.error
warning        = Utils.warning

# =============================================================================
class Installer(InstallerBase):
  """ @class Installer

      This class initializes the data structure for the storage control
      
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, mgr, dev):
    "Default constructor"
    InstallerBase.__init__(self,mgr,dev)
    self.numSubFarm       = Params.hlt_numSubFarms
    self.numPartition     = Params.daq_numPartition
    self.numStreamPerNode = Params.storage_streams_per_node
    self.recvNodes        = Params.storage_recv_nodes
    self.strmNodes        = Params.storage_strm_nodes

  # ===========================================================================
  def create(self):
    recvNodes = std.vector('std::string')()
    strmNodes = std.vector('std::string')()
    for i in self.recvNodes: recvNodes.push_back(i)
    for i in self.strmNodes: strmNodes.push_back(i)
    dm     = self.manager.deviceMgr()
    typ    = self.manager.typeMgr().type('StreamControl')
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
    return self.write(prefix='StreamControl('+self.name+'): ')

  # ===========================================================================
  def createPartition(self, name):
    "Create a new partition"
    DP = PVSS.DataPoint
    empty = std.vector('std::string')()
    dm = self.manager.deviceMgr()
    typ = self.manager.typeMgr().type('StreamPartition')
    device = dm.createDevice(name,typ,1)
    if device.get() is None:
      warning('Failed to create device "'+name+'"')
    device = dm.device(name)
    if device.get() is None:
      error('Failed to access device "'+name+'"')
      return None
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

# =============================================================================
def install(name):
  c = Systems.controlsMgr(Params.storage_system_name)
  inst = Installer(c,name).create()
  return c

# =============================================================================
if __name__ == "__main__":
  install('Storage')
