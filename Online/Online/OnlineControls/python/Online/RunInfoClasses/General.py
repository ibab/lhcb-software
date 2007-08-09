import time
import Online.PVSS as PVSS
import Online.Utils as Utils
log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
DataPoint = PVSS.DataPoint
debug     = None

# =============================================================================
class General:
  """ @class General
  
      Connect to all general information related datapoints in the RunInfo object

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
    self.manager = manager
    self.name    = name
    self.reader  = self.manager.devReader()
    self.runTyp  = self.dp('general.runType')
    self.partID  = self.dp('general.partId')
    self.reader.add(self.runTyp)
    self.reader.add(self.partID)

  # ===========================================================================
  def dp(self,name):
    "Access internal datapoint of the datapoint structure."
    return DataPoint(self.manager,DataPoint.original(self.name+'_RunInfo.'+name))

  # ===========================================================================
  def _setDataItem(self, datapoint):
    datapoint.data = partID
    if save: return self._save(datapoint)
    return self

  # ===========================================================================
  def _dataItem(self, datapoint):
    if datapoint.data is None:
      self.load()
    return datapoint.data

  # ===========================================================================
  def load(self):
    "Load the RunInfo structure datapoint."
    if self.reader.execute(1,1):
      return self
    error('Failed to load run information for:'+self.name)
    return None
        
  # ===========================================================================
  def _save(self,datapoint,deviceIO=None):
    """Save single datapoint. If the deviceIO structure is present only
       add the datapoint to the pending transaction
    """
    if deviceIO is not None:
      deviceIO.add(datapoint)
      return self
    wr = self.manager.devWriter()
    wr.add(self.partID)
    if wr.execute(0,1) is not None:
      return self
    return None
  # ===========================================================================
  def _collectTasks(self,tasks,data):
    if debug: print '_collectTasks: ',data.name(),data.data.size()
    for i in data.data:
      items = i.split('/')
      if len(items) > 0:
        if not tasks.has_key(items[0]):
          tasks[items[0]] = []
        tasks[items[0]].append(items)
    return tasks
  
  # ===========================================================================
  def showGeneral(self):
    "Show all information from the RunInfo structure."
    log('Run type:                    '+str(self.runTyp.data))
    log('Partition ID                 '+str(self.partID.data))
  # ===========================================================================
  def show(self):
    "Show all information from the RunInfo structure."
    if self.load():
      self.showGeneral()
      return;
    error('Failed to load RunInfo for partition:'+self.name)
  # ===========================================================================
  def setPartitionID(self,partID,save=0):
    return self._setDataItem(self.partID,partID,save)
  # ===========================================================================
  def partitionID(self):
    return self._dataItem(self.partID)
  # ===========================================================================
  def setRunType(self,runType,save=0):
    return self._setDataItem(self.runTyp,runType,save)
  # ===========================================================================
  def runType(self):
    return self._dataItem(self.runTyp)
