"""
Install the Online Streaming Control components

Author: M.Frank

"""

import Online.PVSS        as PVSS
import Online.Utils       as Utils
import Online.Streaming.PartitionInfo as PartitionInfo
from Online.InstallerBase import InstallerBase as InstallerBase

std            = PVSS.gbl.std

# =============================================================================
class Installer(InstallerBase):
  """ @class Installer

      This class initializes the data structure for the streaming control
      
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, mgr, dev):
    "Default constructor"
    InstallerBase.__init__(self,mgr,dev+'Alloc')
    self.system           = dev
    self.numRecvSlices    = 0
    self.numPartition     = 0
    self.recvNodes        = []
    self.strmNodes        = []
    self.numStreamSlices  = 0

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
      Utils.warning('Failed to create device "'+self.name+'"',timestamp=1)
    partitions = PVSS.DpIDVector()
    errs = 0
    self.dps.clear()
    infos = []
    for i in xrange(self.numPartition):
      slice = '_Slice%02X'%(i,)
      p = PartitionInfo.create(self.manager,self.system+slice,self.system+'Config'+slice)
      if p:
        partitions.push_back(p[0].id())
        infos.append(p)
      else:
        Utils.error("Errors occurred during device creation.",timestamp=1)
        return

    self.set('Command','')
    self.set('State','UNKNOWN')
    self.set('PartitionDesc',partitions)
    self.set('InUse',std.vector('int')(16,0))
    self.set('ActivePartitions',std.vector('std::string')())
    slices = std.vector('std::string')(self.numRecvSlices,'')
    nodes = {}
    for i in xrange(self.numRecvSlices):
      node = self.recvNodes[i%len(self.recvNodes)]
      if not nodes.has_key(node): nodes[node] = 0
      nodes[node] = nodes[node] + 1
      slices[i] = node+":%02d"%nodes[node]
    self.set('RecvNodes',recvNodes)
    self.set('RecvSlices',slices)
    slices.resize(self.numStreamSlices,'')
    nodes = {}
    for i in xrange(self.numStreamSlices):
      node = self.strmNodes[i%len(self.strmNodes)]
      if not nodes.has_key(node): nodes[node] = 0
      nodes[node] = nodes[node] + 1
      slices[i] = node+":%02d"%nodes[node]
    self.set('StreamNodes',strmNodes)
    self.set('StreamSlices',slices)
    return self.write(prefix='StreamControl('+self.name+'): ')
