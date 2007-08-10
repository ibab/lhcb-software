import Online.PVSS as PVSS
import Online.Utils as Utils

log         = Utils.log
error       = Utils.error
std         = PVSS.gbl.std
DataPoint   = PVSS.DataPoint
printSlots  = Utils.printSlots

NAME        = 0
INUSE       = 1
FSM_SLICE   = 2
RECVNODES   = 3
RECVSLICES  = 4
STRMNODES   = 5
STRMSLICES  = 6

# =============================================================================
class PartitionInfo:
  """ @class PartitionInfo
  
      Connect to all storage related datapoints in the PartitionInfo object

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Streaming Partition datapoint

        @return reference to initialized object
    """
    self.name = name
    self.manager = manager
    self.datapoints = PVSS.DataPointVector()
    self.datapoints.push_back(self.dp('Name'))
    self.datapoints.push_back(self.dp('InUse'))
    self.datapoints.push_back(self.dp('FSMSlice'))
    self.datapoints.push_back(self.dp('RecvNodes'))
    self.datapoints.push_back(self.dp('RecvSlices'))
    self.datapoints.push_back(self.dp('StreamNodes'))
    self.datapoints.push_back(self.dp('StreamSlices'))
  # ===========================================================================
  def load(self, rdr=None):
    "Load all datapoints contained in the storage partition description"
    if rdr is None: rdr = self.manager.devReader();
    rdr.add(self.datapoints)
    return rdr.execute(0,1)
  # ===========================================================================
  def dp(self,name):
    "Access internal datapoint of the datapoint structure."
    return DataPoint(self.manager,DataPoint.original(self.name+'.'+name))
  # ===========================================================================
  def wrdp(self,name):
    "Access internal datapoint of the datapoint structure."
    return DataPoint(self.manager,DataPoint.online(self.name+'.'+name))
  # ===========================================================================
  def detectorName(self):
    return self.datapoints[NAME].data
  # ===========================================================================
  def inUse(self):
    return self.datapoints[INUSE].data
  # ===========================================================================
  def fsmSlice(self):
    return self.datapoints[FSM_SLICE].data
  # ===========================================================================
  def recvNodes(self):
    return self.datapoints[RECVNODES].data
  # ===========================================================================
  def recvSlices(self):
    return self.datapoints[RECVSLICES].data
  # ===========================================================================
  def streamNodes(self):
    return self.datapoints[STRMNODES].data
  # ===========================================================================
  def streamSlices(self):
    return self.datapoints[STRMSLICES].data
  # ===========================================================================
  def show(self,info,extended=None):
    "Show all information from the Streaming Partition structure."
    info.showPartition(self)
