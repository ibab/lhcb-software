import time
import Online.PVSS as PVSS
import Online.Utils as Utils
from Online.RunInfoClasses.General import General as General

log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
DataPoint = PVSS.DataPoint

# =============================================================================
class HLTFarm:
  """ @class HLTFarm
  
      Connect to all storage related datapoints in the RunInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    self.nodes     = self.dp('HLTFarm.nodeList')
    self.subFarms  = self.dp('HLTFarm.subFarms')
    self.reader.add(self.nodes)
    self.reader.add(self.subFarms)
    
  # ===========================================================================
  def addDevices(self, deviceIO):
    """
    Add task devices to device IO structure for read/write access
    """
    return deviceIO
  # ===========================================================================
  def showSubfarms(self):
    """
    Show subfarm related information of the RunInfo datapoint.
    """
    farms = self.subFarms.data
    nSF = self.nSubFarm.data
    if len(farms) != nSF:
      error('Subfarm allocation inconsistency: nSubFarm(%d) != len(subFarms)(%d)'%(len(farms),nSF))
    log('Input from %d sub farms'%nSF)
    for i in nSF:
      log(' -> Subfarm:%-24s '%(farms[i],))
      #log(' -> Subfarm:SF%02d '%(i,))
  # ===========================================================================
  def show(self):
    """
    Show all information from the RunInfo structure concerning the HLT farm.
    """
    if self.load():
      self.showSubfarms()
      self.showGeneral()
      return
    error('Failed to load RunInfo for partition:'+self.name)    
  # ===========================================================================
  def defineTasks(self,recv_slots,strm_slots):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.    
    """
    pass

  # ===========================================================================
  def clearTasks(self, writer):
    """
    Clear all tasks definition in the storage layer for a given partition.
    """
    self.addDevices(writer)
    return writer

# =============================================================================
class HLTFarmInfo(HLTFarm,General):
  """ @class HLTFarmInfo
  
      Connect to all HLT farm related datapoints in the RunInfo object
      Standalone object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Detector/Partition name of the RunInfo datapoint

        @return reference to initialized object
    """
    General.__init__(self,manager,name)
    HLTFarm.__init__(self)
