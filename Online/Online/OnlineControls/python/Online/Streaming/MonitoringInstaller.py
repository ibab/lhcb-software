"""

import Online.Streaming.MonitoringInstaller as MI

mgr=MI.install('Monitoring')

"""

import Online.PVSS        as PVSS
import Online.Utils       as Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.Streaming.PartitionInfo as PartitionInfo
from Online.InstallerBase import InstallerBase as InstallerBase
std            = PVSS.gbl.std

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
    InstallerBase.__init__(self,mgr,dev+'Alloc')
    self.system           = dev
    self.numPartition     = Params.daq_numPartition
    self.numWorkersPerNode = Params.monitor_workers_per_node
    self.numStreams       = Params.monitor_streams_per_node
    self.relayNodes       = Params.monitor_relay_nodes
    self.monNodes         = Params.monitor_nodes

  # ===========================================================================
  def create(self):
    relayNodes = std.vector('std::string')()
    monNodes = std.vector('std::string')()
    for i in self.relayNodes: relayNodes.push_back(i)
    for i in self.monNodes: monNodes.push_back(i)
    dm     = self.manager.deviceMgr()
    typ    = self.manager.typeMgr().type('StreamControl')
    device = dm.createDevice(self.name,typ,1)
    if device.get() is None:
      Utils.warning('Failed to create device "'+self.name+'"')
    partitions = PVSS.DpIDVector()
    errs = 0
    self.dps.clear()
    infos = []
    for i in xrange(self.numPartition):
      slice = '_Slice%02X'%(i,)
      p = PartitionInfo.create(self.manager,self.system+slice,self.system+slice+'_Config')
      if p:
        partitions.push_back(p[0].id())
        infos.append(p)
      else:
        Utils.error("Errors occurred during device creation.")
        return
    self.set('Command','unload')
    self.set('State','NOT_READY')
    self.set('PartitionDesc',partitions)
    self.set('InUse',std.vector('int')(16,0))
    self.set('ActivePartitions',std.vector('std::string')())
    length = len(self.monNodes)*self.numStreams
    relay_slices = std.vector('std::string')(length,'')
    nodes = {}
    for i in xrange(length):
      node = self.relayNodes[i%len(self.relayNodes)]
      if not nodes.has_key(node): nodes[node] = 0
      nodes[node] = nodes[node] + 1
      relay_slices[i] = node+":%02d"%nodes[node]
    self.set('RecvNodes',relayNodes)
    self.set('RecvSlices',relay_slices)

    length = len(self.monNodes)*self.numWorkersPerNode
    worker_slices = std.vector('std::string')(length,'')
    nodes = {}
    for i in xrange(length):
      node = self.monNodes[i%len(self.monNodes)]
      if not nodes.has_key(node): nodes[node] = 0
      nodes[node] = nodes[node] + 1
      worker_slices[i] = node+":%02d"%nodes[node]
    self.set('StreamNodes',monNodes)
    self.set('StreamSlices',worker_slices)
    return self.write(prefix='StreamControl('+self.name+'): ')

# =============================================================================
def install(name,system=Params.monitor_system_name):
  c = Systems.controlsMgr(system)
  inst = Installer(c,name).create()
  return c

# =============================================================================
if __name__ == "__main__":
  install('Monitoring')
