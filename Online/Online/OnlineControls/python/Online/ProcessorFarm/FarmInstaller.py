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
    self.typeMgr          = self.manager.typeMgr()
    self.devMgr           = self.manager.deviceMgr()


  # ===========================================================================
  def createSubFarmControls(self):
    sf  = self.subFarms
    typ = self.typeMgr.type('StreamControl')
    for k,n in sf.items():
      nam = self.name+'_'+k+'Alloc'
      dev = self.devMgr.createDevice(nam,typ,1)
      if dev.get() is None:
        warning('Failed to create farm control "'+nam+'"',timestamp=1)
    return self

  # ===========================================================================
  def create(self):
    sf  = self.subFarms
    mgr = self.manager
    #typ = self.typeMgr.type('StreamControl')
    #dev = self.devMgr.createDevice(self.name+'Alloc',typ,1)
    #if dev.get() is None:
    #  warning('Failed to create farm allocator for "'+self.name+'"',timestamp=1)

    typ = self.typeMgr.type('FarmInfo')
    dev = self.devMgr.createDevice(self.name,typ,1)
    if dev.get() is None:
      warning('Failed to create farm device for "'+self.name+'"',timestamp=1)

    log('Accessing farm device '+self.name,timestamp=1)
    self.set('Name',self.name)
    self.set('SubFarms',sf.keys())
    self.set('InUse',['' for i in xrange(len(sf))])
    self.set('State','NOT_READY')
    return self.write(prefix='FarmInstaller('+self.name+'): ')
    
  # ===========================================================================
  def createSubFarms(self):
    sf  = self.subFarms
    mgr = self.manager
    typ = self.typeMgr.type('FarmSubInfo')
    v = std.vector('std::string')()
    for k,n in sf.items():
      nam = self.myName+'_'+k
      dev = self.devMgr.createDevice(nam,typ,1)
      if dev.get() is None:
        warning('Failed to create subfarm device "'+nam+'"',timestamp=1)
      log('Accessing subfarm device '+nam,timestamp=1)
      self.name = nam
      self.set('Name',nam)
      for i in xrange(len(n[0])):
        v.push_back(n[0][i]+'/'+str(n[1][i]))
      self.set('Nodes',v)
      v.clear()
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')

  # ===========================================================================
  def createIO(self, manager=None):
    if manager is None: manager = self.manager
    inst = Online.Streaming.StreamInstaller.Installer(manager,self.name+'IO')
    inst.numPartition     = self.numFarm
    inst.numRecvSlices    = self.numSubFarm
    inst.numStreamSlices  = self.numSubFarm
    inst.create()
    return self
    
  # ===========================================================================
  def createActivity(self, name):
    nam = 'FarmActivity_'+name
    typ = self.typeMgr.type('FarmActivity')
    dev = self.devMgr.createDevice(nam,typ,1)
    if dev.get() is None:
      warning('Failed to create farm activity "'+nam+'"',timestamp=1)
    log('Accessing farm activity '+nam,timestamp=1)
    self.name = nam
    self.set('Name',name)
    self.set('Farm.Infrastructure',   ['MBM/Class0','Receiver/Class1','Sender/Class2'])
    self.set('Farm.Worker',            'Brunel/Class1')
    self.set('Control.Infrastructure',['Adder/Class1'])
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')
  
  # ===========================================================================
  def createSlices(self):
    typ = self.typeMgr.type('FarmSlice')
    for i in xrange(self.numFarm):
      nam = '%s_Slice%02X'%(self.myName,i)
      dev = self.devMgr.createDevice(nam,typ,1)
      if dev.get() is None:
        warning('Failed to create farm Slice "'+nam+'"',timestamp=1)
      log('Accessing farm Slice '+nam,timestamp=1)
      self.name = nam
      self.set('InUse',0)
      self.set('FSMSlice','')
      self.set('RunInfo','')
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')

  # ===========================================================================
  def createPartitions(self):
    typ = self.typeMgr.type('FarmRunInfo')
    for i in xrange(16):
      nam = '%s_Reco%02X'%(self.myName,i)
      dev = self.devMgr.createDevice(nam,typ,1)
      if dev.get() is None:
        warning('Failed to create farm Slice "'+nam+'"',timestamp=1)
      log('Accessing farm Slice '+nam,timestamp=1)
      self.name = nam
      self.set('general.partName','')
      self.set('general.partId',1<<i)
      self.set('general.activePartId',1<<i)
      self.set('HLTFarm.nSubFarms',0)
      self.set('HLTFarm.subFarms',std.vector('std::string')())
    self.name = self.myName
    return self.write(prefix='FarmInstaller('+self.name+'): ')
    
  # ===========================================================================
  def createFMC(self):
    typ = self.typeMgr.type('StreamTaskCreator')
    for k,n in self.subFarms.items():
      for i in xrange(len(n[0])):
        nam = n[0][i]+'_StreamTaskCreator'
        dev = self.devMgr.createDevice(nam,typ,1)
        if dev.get() is None:
          warning('Failed to FMC device "'+nam+'"',timestamp=1)
    return self

# =============================================================================
def installFarm(name=FarmSetup.Name,system=FarmSetup.pvss_system):
  inst = Installer(Online.PVSSSystems.controlsMgr(system),name)
  inst.create()
  # inst.createSubFarms()
  inst.createSlices()
  inst.createPartitions()
  inst.createActivity('REPRO_1')
  inst.createActivity('REPRO_2')
  inst.createActivity('REPRO_3')
  # inst.createFMC()
  # inst.createIO()
  return inst.manager

# =============================================================================
def installSubFarm(name=FarmSetup.Name,system=FarmSetup.pvss_system):
  inst = Installer(Online.PVSSSystems.controlsMgr(system),name)
  inst.createSubFarmControls()
  inst.createSubFarms()
  inst.createFMC()
  return inst.manager

# =============================================================================
def install(name=FarmSetup.Name,system=FarmSetup.pvss_system):
  return installFarm(name,system)

# =============================================================================
if __name__ == "__main__":
  install()
