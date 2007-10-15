"""
Install the Online Prompt reconstruction Control components

Author: M.Frank

"""

import time, FarmSetup, Online.PVSS, Online.PVSSSystems, Online.InstallerBase
import Online.Streaming.StreamInstaller

std     = Online.PVSS.gbl.std
log     = Online.Utils.log
warning = Online.Utils.warning
DP      = Online.PVSS.DataPoint

# =============================================================================
class Installer(Online.InstallerBase.InstallerBase):
  """ @class Installer

      This class initializes the data structure for the storage control
      
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, mgr, name):
    "Default constructor"
    Online.InstallerBase.InstallerBase.__init__(self,mgr,name)
    self.myName           = self.name
    self.numFarm          = FarmSetup.numFarm
    self.numSubFarm       = FarmSetup.numSubFarm
    self.subFarms         = FarmSetup.subFarms
    self.sendNodes        = FarmSetup.storageSenderNodes
    self.recvNodes        = FarmSetup.storageReceiverNodes
    self.typeMgr          = self.manager.typeMgr()
    self.devMgr           = self.manager.deviceMgr()

  # ===========================================================================
  def create(self):
    sf  = self.subFarms
    mgr = self.manager
    typ = self.typeMgr.type('StreamControl')
    dev = self.devMgr.createDevice(self.name+'Alloc',typ,1)
    if dev.get() is None:
      warning('Failed to create farm allocator for "'+self.name+'"',timestamp=1)

    typ = self.typeMgr.type('RecoFarmInfo')
    dev = self.devMgr.createDevice(self.name,typ,1)
    if dev.get() is None:
      warning('Failed to create farm device for "'+self.name+'"',timestamp=1)

    log('Accessing farm device '+self.name,timestamp=1)
    self.set('Name',self.name)
    self.set('SubFarms',sf.keys())
    self.set('InUse',['' for i in xrange(len(sf))])
    self.set('State','NOT_READY')
    typ = self.typeMgr.type('RecoSubFarmInfo')
    for k,n in sf.items():
      nam = self.myName+'_'+k
      dev = self.devMgr.createDevice(nam,typ,1)
      if dev.get() is None:
        warning('Failed to create subfarm device "'+nam+'"',timestamp=1)
      log('Accessing subfarm device '+nam,timestamp=1)
      self.name = nam
      self.set('Name',k)
      self.set('Nodes',n[0])
      self.set('NodesCPU',n[1])
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')

  # ===========================================================================
  def createIO(self, manager=None):
    if manager is None: manager = self.manager
    inst = Online.Streaming.StreamInstaller.Installer(manager,self.name+'IO')
    inst.numPartition     = self.numFarm
    inst.recvNodes        = self.recvNodes
    inst.strmNodes        = self.sendNodes
    inst.numRecvSlices    = self.numSubFarm
    inst.numStreamSlices  = self.numSubFarm
    inst.create()
    return self
    
  # ===========================================================================
  def createActivity(self, name):
    nam = 'RecoFarmActivity_'+name
    typ = self.typeMgr.type('RecoFarmActivity')
    dev = self.devMgr.createDevice(nam,typ,1)
    if dev.get() is None:
      warning('Failed to create farm activity "'+nam+'"',timestamp=1)
    log('Accessing farm activity '+nam,timestamp=1)
    self.name = nam
    self.set('Name',name)
    self.set('Farm.Infrastructure',  ['MBMReco'])
    self.set('Farm.TaskTypes',       ['Brunel'])
    self.set('Farm.ReceiverType',    'RecvReco')
    self.set('Farm.SenderType',      'SendReco')
    self.set('Storage.streamInfrastructure', ['MBMReco'])
    self.set('Storage.streamTypes',          ['SendToFarm','FileRDR'])
    self.set('Storage.strmStrategy',         2)
    self.set('Storage.recvInfrastructure',   ['MBMReco'])
    self.set('Storage.recvTypes',            ['RecvFromFarm','DiskWR'])
    self.set('Storage.recvStrategy',         2)
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')
  
  # ===========================================================================
  def createSlices(self):
    typ = self.typeMgr.type('RecoFarmSlice')
    for i in xrange(self.numFarm):
      nam = '%s_Slice%02X'%(self.myName,i)
      dev = self.devMgr.createDevice(nam,typ,1)
      if dev.get() is None:
        warning('Failed to create farm Slice "'+nam+'"',timestamp=1)
      log('Accessing farm Slice '+nam,timestamp=1)
      self.name = nam
      self.set('Name',nam)
      self.set('InUse',0)
      self.set('Activity','')
      self.set('Command','')
      self.set('State','NOT_READY')
      self.set('SubFarms',std.vector('std::string')())
      self.set('Tasks',std.vector('std::string')())
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')
    
# =============================================================================
def install(name=FarmSetup.Name,system=FarmSetup.pvss_system):
  inst = Installer(Online.PVSSSystems.controlsMgr(system),name)
  inst.create()
  inst.createSlices()
  inst.createActivity('REPRO_1')
  inst.createActivity('REPRO_2')
  inst.createActivity('REPRO_3')
  inst.createIO()
  return inst.manager

# =============================================================================
if __name__ == "__main__":
  install()
