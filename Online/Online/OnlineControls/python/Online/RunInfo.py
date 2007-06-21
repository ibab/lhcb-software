import time
import Online.PVSS as PVSS
import Online.Utils as Utils
from Online.RunInfoClasses.General import General as General
from Online.RunInfoClasses.Storage import Storage as Storage
from Online.RunInfoClasses.HLTFarm import HLTFarm as HLTFarm
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
class RunInfo(General,Storage,HLTFarm):
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
    self.reader      = manager.devReader()
    General.__init__(self,manager,name)
    Storage.__init__(self)
    HLTFarm.__init__(self)
    self.tell1Boards   = self.dp('SubDeterctors.tell1List')
    self.reader.add(self.tell1Boards)    
    
  # ===========================================================================
  def numLayer2Slots(self):
    return sum(self.strMult.data)
  # ===========================================================================
  def numLayer1Slots(self):
    return self.nSubFarm.data
  # ===========================================================================
  def addDevices(self, deviceIO):
    "Add task devices to device IO structure for read/write access"
    # import traceback
    wr = deviceIO
    if wr is None or wr.get() is None: wr = self.manager.devWriter()
    # log('Adddevices:RunInfo wr='+str(wr.get())+'  '+str(deviceIO.get()))
    Storage.addDevices(self,wr)
    HLTFarm.addDevices(self,wr)
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
      self.showSubfarms()
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
    HLTFarm.defineTasks(self,recv_slots,strm_slots)
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
    HLTFarm.clearTasks(self,writer)
    return writer
  
  # ===========================================================================
  def collectTasks(self,class0_tasks,class1_tasks):
    class0_tasks = self._collectTasks(class0_tasks,self.rcvInfraTasks)
    print 'rcvInfraTasks:',len(self.rcvInfraTasks.data)
    for i,j in class0_tasks.items(): log('+++-->Class 0:'+str(i)+' with '+str(len(j))+' tasks')
    
    class0_tasks = self._collectTasks(class0_tasks,self.strInfraTasks)
    class1_tasks = self._collectTasks(class1_tasks,self.receivers)
    class1_tasks = self._collectTasks(class1_tasks,self.rcvSenders)
    class1_tasks = self._collectTasks(class1_tasks,self.strReceivers)
    class1_tasks = self._collectTasks(class1_tasks,self.streamers)
    return (class0_tasks,class1_tasks)

class RunInfoCreator:
  def __init__(self,manager):             self.manager = manager
  def create(self,partition):             return RunInfo(self.manager,partition)
  def showPartition(self,info,partition):
    import Online.RunInfoClasses.Storage as S
    S.showPartition(info,partition)
"""
import Online.PVSS as PVSS
import Online.RunInfo as RI
import Online.RunInfoClasses.General as GEN
import Online.RunInfoClasses.Storage as STO
import Online.RunInfoClasses.HLTFarm as HLT

m=PVSS.controlsMgr()
g=GEN.General(m,"LHCb")
g.show()
s=STO.StorageInfo(m,"LHCb")
s.show()
f=HLT.HLTFarmInfo(m,"LHCb")
f.show()
r=RI.RunInfo(PVSS.controlsMgr(),"LHCb")
r.show()


"""
