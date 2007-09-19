"""
import Online.PVSSSystems as Systems
import Online.RunInfo as RI
import Online.RunInfoClasses.General as GEN
import Online.RunInfoClasses.Storage as STO
import Online.RunInfoClasses.HLTFarm as HLT

m=Systems.controlsMgr('LBECS')
g=GEN.General(m,"LHCb")
g.show()
s=STO.StorageInfo(m,"LHCb")
s.show()
f=HLT.HLTFarmInfo(m,"LHCb")
f.show()
r=RI.RunInfo(m,"LHCb")
r.show()

"""

import time
import Online.PVSS as PVSS
import Online.Utils as Utils
import Online.PVSSSystems as Systems
from Online.RunInfoClasses.General import General as General
from Online.RunInfoClasses.Storage import StorageInfo as Storage
sum       = Utils.sum
log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
DataPoint = PVSS.DataPoint

def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()
def vector2String(c):
  s = ''
  for i in c:
    s = s + ' ' + str(i)
  return s

# =============================================================================
class RunInfo(General,Storage):
  """ @class RunInfo
  
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
    Storage.__init__(self)
    #HLTFarm.__init__(self)
    
  # ===========================================================================
  def addDevices(self, deviceIO):
    "Add task devices to device IO structure for read/write access"
    # import traceback
    wr = deviceIO
    if wr is None or wr.get() is None: wr = self.manager.devWriter()
    #log('Adddevices:RunInfo wr='+str(wr.get())+'  '+str(deviceIO.get()))
    Storage.addDevices(self,wr)
    #HLTFarm.addDevices(self,wr)
    if deviceIO is None or deviceIO.get() is None:
      #print '+++++++++++++++++++++++++++++++++++++> Exec IO'
      if wr.execute(0,1):
        return wr
      return None
    #else:
    #   traceback.print_stack()
    #   print '-------------------------------------> NO Exec IO'
    return deviceIO
  # ===========================================================================
  def show(self):
    "Show all information from the RunInfo structure."
    if self.load():
      #self.showSubfarms()
      self.showStreams()
      General.show(self)
      return
    error('Failed to load RunInfo for partition:'+self.name)
  # ===========================================================================
  def defineTasks(self,recv_slots,strm_slots):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.
    """
    log('RunInfo: Defining tasks for partition '+self.name)
    Storage.defineTasks(self,recv_slots,strm_slots)
    #HLTFarm.defineTasks(self,recv_slots,strm_slots)
    log('RunInfo: Defining tasks for partition '+self.name+'   ... done')
    wr = self.manager.devWriter()
    self.addDevices(wr)
    return wr.execute(0,1)
  # ===========================================================================
  def clearTasks(self, writer=None):
    """
    Clear all tasks definition in the storage layer for a given partition.
    """
    Storage.clearTasks(self,writer)
    #HLTFarm.clearTasks(self,writer)
    return writer
  
class RunInfoCreator:
  def __init__(self):
    pass
  def create(self,rundp_name,partition):
    items = rundp_name.split(':')
    mgr = Systems.controlsMgr(items[0])
    return RunInfo(mgr,partition)
  def showPartition(self,info,partition):
    import Online.RunInfoClasses.Storage as S
    S.showPartition(info,partition)
