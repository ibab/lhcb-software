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
    self.manager     = manager
    self.name        = name
    self.reader      = self.manager.devReader()
    self.runTyp      = self.dp('general.runType')
    self.partID      = self.dp('general.partId')
    self.partition   = self.dp('general.partName')
    self.nSubFarm    = self.dp('HLTFarm.nSubFarms')
    self.storeFlag   = self.dp('Storage.storeFlag')
    self.rcvInfra    = self.dp('Storage.recvInfrastructure')
    self.strInfra    = self.dp('Storage.streamInfrastructure')
    self.streams     = self.dp('Storage.streamTypes')
    self.strMult     = self.dp('Storage.streamMultiplicity')

    self.monFlag     = self.dp('MonFarm.monFlag')
    self.monTypes    = self.dp('MonFarm.monTypes')
    self.monStreams  = self.dp('MonFarm.monStreams')
    self.monMult     = self.dp('MonFarm.monMultiplicity')
    self.monInfra    = self.dp('MonFarm.monInfrastructure')
    self.relayInfra  = self.dp('MonFarm.relayInfrastructure')

    self.tell1Boards = self.dp('SubDetectors.tell1List')

    self.reader.add(self.runTyp)
    self.reader.add(self.partID)
    self.reader.add(self.partition)
    self.reader.add(self.nSubFarm)
    # Storage information
    self.reader.add(self.storeFlag)
    self.reader.add(self.streams)
    self.reader.add(self.strMult)
    self.reader.add(self.rcvInfra)
    self.reader.add(self.strInfra)
    # Monitoring information
    self.reader.add(self.monFlag)
    self.reader.add(self.monTypes)
    self.reader.add(self.monMult)
    self.reader.add(self.monStreams)
    self.reader.add(self.monInfra)
    self.reader.add(self.relayInfra)
    # Subdetector information
    self.reader.add(self.tell1Boards)    

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
  def showGeneral(self):
    "Show all information from the RunInfo structure."
    log('Run type:                    '+str(self.runTyp.data))
    log('Partition ID                 '+str(self.partID.data))
    if self.storeFlag>0:
      log('Data storage is enabled')
      for i in xrange(len(self.streams.data)):
        fmt = 'Data stream:                 %-20s with multiplicity:%d'
        log(fmt%(self.streams.data[i],self.strMult.data[i]))
    s = 'Receive infrastructure:      '
    for i in xrange(len(self.recvInfra.data)):
      s = s + self.recvInfra.data[i]
    log(s)
    if self.storeFlag>0:
      s = 'Streaming infrastructure:    '
      for i in xrange(len(self.recvInfra.data)):
        s = s + self.recvInfra.data[i]
      log(s)

  # ===========================================================================
  def show(self):
    "Show all information from the Storage structure."
    if self.load():
      self.showStreams()
      self.showMonitors()
      self.showTell1Boards()
      self.showGeneral()
      return
    error('Failed to load RunInfo for partition:'+self.name)

  # ===========================================================================
  def showTell1Boards(self):
    "Show TELL1 boards related information of the RunInfo datapoint."
    t1 = self.tell1Boards
    log('Number of TELL1 boards:%d'%len(t1))
    s = ''
    for i in xrange(len(t1)):
      s = s + ' %-12s'%(t1[i],)
      if i>0 and ((i%5) == 0 or i==(len(t1)-1)):
        log('->  '+s)
        s = ''

  # ===========================================================================
  def showStreams(self):
    "Show output stream related information of the RunInfo datapoint."
    streams = self.streams.data
    multiplicity = self.strMult.data
    log('Output to %d logical streams'%len(self.streams.data))
    for i in xrange(len(streams)):
      log(' -> Data stream:%-24s with  mutiplicity:%d'%(streams[i],multiplicity[i]))

  # ===========================================================================
  def showMonitors(self):
    "Show monitor stream related information of the RunInfo datapoint."
    streams = self.monTypes.data
    multiplicity = self.monMult.data
    log('Output to %d logical streams'%len(streams))
    for i in xrange(len(streams)):
      log(' -> Data stream:%-24s with  mutiplicity:%d'%(streams[i],multiplicity[i]))
      
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

# =============================================================================
def create(rundp_name):
    items = rundp_name.split(':')
    mgr = Systems.controlsMgr(items[0])
    partition = items[1][:items[1].find('_')]
    return General(mgr,partition)
