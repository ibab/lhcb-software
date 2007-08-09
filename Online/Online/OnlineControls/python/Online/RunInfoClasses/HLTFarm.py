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
    self.nSubFarm  = self.dp('HLTFarm.nSubFarms')
    self.receivers = self.dp('HLTFarm.receiverTasks')
    self.reader.add(self.nodes)
    self.reader.add(self.subFarms)
    self.reader.add(self.nSubFarm)
    self.reader.add(self.nodes)
    self.reader.add(self.receivers)
    
  # ===========================================================================
  def addDevices(self, deviceIO):
    """
    Add task devices to device IO structure for read/write access
    """
    wr = deviceIO
    if wr is None: wr = self.manager.devWriter()
    wr.add(self.receivers)
    #log('HLTFarm: Adddevices:HLT wr='+str(wr)+'  '+str(deviceIO.get()))
    if deviceIO is None:
      if wr.execute(0,1):
        return wr
      return None
    return deviceIO
  # ===========================================================================
  def showSubfarms(self):
    """
    Show subfarm related information of the RunInfo datapoint.
    """
    farms = self.subFarms.data
    recv = self.receivers.data
    nSF = self.nSubFarm.data
    if len(farms) != nSF:
      error('Subfarm allocation inconsistency: nSubFarm(%d) != len(subFarms)(%d)'%(len(farms),nSF))
    full = len(recv) == len(farms)
    log('Input from %d sub farms'%self.nSubFarm.data)
    for i in xrange(len(farms)):
      if full:
        log(' -> Subfarm:%-12s sending to: %s'%(farms[i],recv[i]))
      else:
        log(' -> Subfarm:%-24s '%(farms[i],))
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
    # log('HLTFarm: Defining tasks for partition '+self.name)
    self.receivers.data.clear()
    for i in xrange(len(recv_slots)):
      slot = recv_slots[i]
      node = slot[:slot.find(':')]
      short_name = self.subFarms.data[i]+'_HLT'
      task = self.name+'_'+node+'_'+short_name
      self.receivers.data.push_back(node+'/'+task+'/'+short_name+'/HLTRec/("'+self.subFarms.data[i]+'",)')

  # ===========================================================================
  def clearTasks(self, writer):
    """
    Clear all tasks definition in the storage layer for a given partition.
    """
    empty = std.vector('std::string')()
    self.receivers.data = empty
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
