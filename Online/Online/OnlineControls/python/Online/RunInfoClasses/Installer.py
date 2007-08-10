"""

Install Runinfo objects on general LHCb ECS
See module Online.SetupParams for configuration parameters

Commands:

import Online.RunInfoClasses.Installer as RI
mgr=RI.install()

"""

import time
import Online.SetupParams as Pars
import Online.PVSS        as PVSS
import Online.PVSSSystems as Systems
import Online.Utils       as Utils
import Online.RunInfo     as RunInfo
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
  def __init__(self, mgr):
    "Default constructor"
    InstallerBase.__init__(self,mgr,mgr.name())

  # ===========================================================================
  def setRunInfo(self, info, dp, value):
    "Set datapoints in the corresponding RunInfo structure"
    self.dps.push_back(PVSS.DataPoint(self.manager,PVSS.DataPoint.original(info+'_RunInfo.'+dp)))
    self.dps.back().data = value
    return self

  # ===========================================================================
  def create(self, name, partID):
    import random
    dm     = self.manager.deviceMgr()
    typ    = self.manager.typeMgr().type('RunInfo')
    device = dm.createDevice(name+'_RunInfo',typ,1)
    if device.get() is None:
      warning('Failed to create device "'+self.name+':'+name+'_RunInfo"')
    self.setRunInfo(name,'general.partId',partID)
    self.setRunInfo(name,'general.runType','PHYSICS')
    self.setRunInfo(name,'general.runNumber',random.randint(0,100000))
    self.setRunInfo(name,'general.runNTriggers',0)
    if self.write(prefix='Basic-Runinfo('+name+'): ') is not None:
      return self
    return None
  
  # ===========================================================================
  def createStorage(self, name, streams, nf): 
    sv = std.vector('std::string')()
    sf = std.vector('std::string')()
    dw = std.vector('std::string')()
    self.setRunInfo(name,'Storage.storeFlag',1)
    for i in xrange(nf):
      sf.push_back('lbhlt%02d'%i)
      for j in xrange(30):
        sv.push_back('lbhlt%03d%02d'%(i,j))
    self.setRunInfo(name,'HLTFarm.nodeList',sv)
    self.setRunInfo(name,'HLTFarm.nSubFarms',nf)
    self.setRunInfo(name,'HLTFarm.subFarms',sf)
    self.setRunInfo(name,'HLTFarm.receiverTasks',dw)
    dw = std.vector('std::string')()
    dwmult = std.vector('int')()
    for i in streams:
      dw.push_back(i[0])
      dwmult.push_back(i[1])
    empty = std.vector('std::string')()
    emptymult = std.vector('int')()
    infrastructure = std.vector('std::string')()
    infrastructure.push_back('ErrSrv')
    infrastructure.push_back('StrmMBM')
    self.setRunInfo(name,'Storage.streamTypes',dw)
    self.setRunInfo(name,'Storage.streamWriters',empty)
    self.setRunInfo(name,'Storage.streamMultiplicity',dwmult)
    self.setRunInfo(name,'Storage.streamInfrastructure',infrastructure)
    self.setRunInfo(name,'Storage.streamInfrastructureTasks',empty)
    self.setRunInfo(name,'Storage.recvSenders',empty)
    infrastructure[infrastructure.size()-1] = 'RecvMBM'
    self.setRunInfo(name,'Storage.recvInfrastructure',infrastructure)
    self.setRunInfo(name,'Storage.recvInfrastructureTasks',empty)
    return self.write(prefix='Storage-Runinfo('+name+'): ')
  
  # ===========================================================================
  def createMonitoring(self, name, streams):
    self.setRunInfo(name,'MonFarm.monFlag',1)
    dw = std.vector('std::string')()
    relay = std.vector('std::string')()
    dwmult = std.vector('int')()
    for i in streams:
      type,strm,mult = i
      dw.push_back(type)
      dwmult.push_back(mult)
      relay.push_back(strm)
    empty = std.vector('std::string')()
    emptymult = std.vector('int')()
    infrastructure = std.vector('std::string')()
    infrastructure.push_back('ErrSrv')
    infrastructure.push_back('MBMMon')
    self.setRunInfo(name,'MonFarm.monFlag',1)
    self.setRunInfo(name,'MonFarm.monTypes',dw)
    self.setRunInfo(name,'MonFarm.monMultiplicity',dwmult)
    self.setRunInfo(name,'MonFarm.monInfrastructure',infrastructure)
    self.setRunInfo(name,'MonFarm.monInfrastructureTasks',empty)
    self.setRunInfo(name,'MonFarm.monTasks',empty)
    infrastructure = std.vector('std::string')()
    infrastructure.push_back('ErrSrv')
    infrastructure.push_back('MBMRelay')
    self.setRunInfo(name,'MonFarm.monStreams',relay)
    self.setRunInfo(name,'MonFarm.relayTasks',empty)
    self.setRunInfo(name,'MonFarm.relayInfrastructure',infrastructure)
    self.setRunInfo(name,'MonFarm.relayInfrastructureTasks',empty)
    return self.write(prefix='Monitoring-Runinfo('+name+'): ')

# =============================================================================
def installInfo():
  for name,data in Pars.detectors.items():
    s = data['System']
    c = Systems.controlsMgr(s)
    inst = Installer(c)
    inst.create(name,data['PartitionID'])
  return c

# =============================================================================
def installStorage():
  for name,data in Pars.detectors.items():
    s = data['System']
    c = Systems.controlsMgr(s)
    inst = Installer(c)
    nf = data['SubFarms']
    streams = data['StorageStreams']
    inst.createStorage(name,streams,nf)
  return c


# =============================================================================
def installMonitoring():
  for name,data in Pars.detectors.items():
    s = data['System']
    c = Systems.controlsMgr(s)
    inst = Installer(c)
    streams = data['MonStreams']
    inst.createMonitoring(name,streams)
  return c

# =============================================================================
def install():
  installInfo()
  installStorage()
  installMonitoring()

# =============================================================================
if __name__ == "__main__":
  install()
