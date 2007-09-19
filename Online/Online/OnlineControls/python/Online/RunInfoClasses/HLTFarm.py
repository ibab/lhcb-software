import time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.PVSSSystems as Systems
from Online.RunInfoClasses.General import General as General

log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
DataPoint = PVSS.DataPoint

# =============================================================================
class HLTFarmInfo(General):
  """ @class HLTFarm
  
      Connect to all storage related datapoints in the RunInfo object

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
    self.nodes     = self.dp('HLTFarm.nodeList')
    self.subFarms  = self.dp('HLTFarm.subFarms')
    self.reader.add(self.nodes)
    self.reader.add(self.subFarms)
    
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


# =============================================================================
class HLTFarmInfoCreator:
  def __init__(self):
    pass
  def create(self,rundp_name,partition):
    items = rundp_name.split(':')
    mgr = Systems.controlsMgr(items[0])
    return HLTFarmInfo(mgr,partition)
