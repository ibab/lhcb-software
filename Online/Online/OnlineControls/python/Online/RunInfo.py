import time
import Online.PVSS as PVSS
import Online.Utils as Utils
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

def sum(cont):
  count = 0
  for i in cont:
    count = count + i
  return count
# =============================================================================
class RunInfo:
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
    self.manager       = manager
    self.name          = name
    self.runTyp        = self.dp('general.runType')
    self.partID        = self.dp('general.partId')
    self.tell1Boards   = self.dp('SubDeterctors.tell1List')
    self.nodes         = self.dp('HLTFarm.nodeList')
    self.subFarms      = self.dp('HLTFarm.subFarms')
    self.nSubFarm      = self.dp('HLTFarm.nSubFarms')
    self.receivers     = self.dp('HLTFarm.receiverTasks')
    self.flag          = self.dp('Storage.storeFlag')
    self.rcvInfra      = self.dp('Storage.recvInfrastructure')
    self.rcvInfraTasks = self.dp('Storage.recvInfrastructureTasks')
    self.rcvSenders    = self.dp('Storage.recvSenders')

    self.streams       = self.dp('Storage.streamTypes')
    self.streamers     = self.dp('Storage.streamWriters')
    self.strMult       = self.dp('Storage.streamMultiplicity')
    self.strInfra      = self.dp('Storage.streamInfrastructure')
    self.strInfraTasks = self.dp('Storage.streamInfrastructureTasks')
    self.strReceivers  = self.dp('Storage.streamReceivers')

    self.reader        = self.manager.devReader()
    self.reader.add(self.runTyp)
    self.reader.add(self.partID)
    self.reader.add(self.tell1Boards)    
    self.reader.add(self.nodes)
    self.reader.add(self.subFarms)
    self.reader.add(self.nSubFarm)
    self.reader.add(self.nodes)
    self.reader.add(self.flag)
    self.reader.add(self.streams)
    self.reader.add(self.rcvInfra)
    self.reader.add(self.strInfra)
    self.reader.add(self.strMult)
    self.addTaskDevices(self.reader)
    
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
  def addTaskDevices(self, deviceIO):
    "Add task devices to device IO structure for read/write access"
    wr = deviceIO
    if wr is None: wr = self.manager.devWriter()
    wr.add(self.streamers)
    wr.add(self.strReceivers)
    wr.add(self.strInfraTasks)
    wr.add(self.receivers)
    wr.add(self.rcvSenders)
    wr.add(self.rcvInfraTasks)
    if deviceIO is None:
      if wr.execute(0,1):
        return wr
      return None
    return deviceIO

  # ===========================================================================
  def showStreams(self):
    "Show output stream related information of the RunInfo datapoint."
    streams = self.streams.data
    multiplicity = self.strMult.data
    for i in xrange(len(streams)):
      log(' -> Data stream:%-24s with  mutiplicity:%d'%(streams[i],multiplicity[i]))

  # ===========================================================================
  def showSubfarms(self):
    "Show subfarm related information of the RunInfo datapoint."
    farms = self.subFarms.data
    recv = self.receivers.data
    nSF = self.nSubFarm.data
    if len(farms) != nSF:
      error('Subfarm allocation inconsistency: nSubFarm(%d) != len(subFarms)(%d)'%(len(farms),nSF))
    full = len(recv) == len(farms)
    for i in xrange(len(farms)):
      if full:
        log(' -> Subfarm:%-12s sending to: %s'%(farms[i],recv[i]))
      else:
        log(' -> Subfarm:%-24s '%(farms[i],))

  # ===========================================================================
  def show(self):
    "Show all information from the RunInfo structure."
    if self.load():
      log('Input from %d sub farms'%self.nSubFarm.data)
      self.showSubfarms()
      log('Output to %d logical streams'%len(self.streams.data))
      self.showStreams()
      log('Run type:                    '+str(self.runTyp.data))
      log('Partition ID                 '+str(self.partID.data))
      return
    error('Failed to load RunInfo for partition:'+self.name)
    
  # ===========================================================================
  def defineTasks(self,recv_slots,strm_slots):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.
    
    """
    streams = []
    for j in xrange(len(self.streams.data)):
      for i in xrange(self.strMult.data[j]):
        streams.append([self.streams.data[j],i])
    strmNodes = getNodesFromSlots(strm_slots)
    recvNodes = getNodesFromSlots(recv_slots)

    streamers = []
    self.streamers.data.clear()
    self.strReceivers.data.clear()
    self.strInfraTasks.data.clear()
    for i in xrange(len(strm_slots)):
      slot = strm_slots[i]
      node = slot[:slot.find(':')]
      item = streams[i][0]+('_%02d'%streams[i][1])
      short_name = 'WRT_'+item
      task = self.name+'_WRT_'+node+'_'+item
      self.streamers.data.push_back(node+'/'+task+'/'+short_name+'/WRT_'+streams[i][0])
      short_name = 'RCV_'+item
      task = self.name+'_RCV_'+node+'_'+item
      self.strReceivers.data.push_back(node+'/'+task+'/'+short_name+'/RCV_'+streams[i][0])
      streamers.append([self.name,node,slot,'%02d'%streams[i][1],streams[i][0]])
      
    for j in strmNodes:
      #for i in xrange(len(self.streamers.data)):
      #  part,node,slot,ident,type = streamers[i]
      #  short_name = 'RCV_'+type+'_'+ident
      #  task = part+'_RCV_'+j+'_'+type+'_'+ident
      #  self.strReceivers.data.push_back(j+'/'+task+'/'+short_name+'/'+type)
      for i in self.strInfra.data:
        self.strInfraTasks.data.push_back(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i)
    self.receivers.data.clear()
    self.rcvSenders.data.clear()
    self.rcvInfraTasks.data.clear()
    for i in xrange(len(recv_slots)):
      slot = recv_slots[i]
      node = slot[:slot.find(':')]
      short_name = self.subFarms.data[i]+'_HLT'
      task = self.name+'_'+node+'_'+short_name
      self.receivers.data.push_back(node+'/'+task+'/'+short_name+'/HLT_Receiver/'+self.subFarms.data[i])
    for j in recvNodes:
      for i in xrange(len(streamers)):
        part,node,slot,ident,type = streamers[i]
        short_name = 'SND_'+type+'_'+ident
        sender = part+'_SND_'+j+'_'+type+'_'+ident
        target = part+'_RCV_'+node+'_'+type+'_'+ident
        self.rcvSenders.data.push_back(j+'/'+sender+'/'+short_name+'/SND_'+type+'/'+node+'/'+target)
      for i in self.rcvInfra.data:
        self.rcvInfraTasks.data.push_back(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i)
    wr = self.manager.devWriter()
    return self.addTaskDevices(wr).execute(0,1)

  # ===========================================================================
  def clearTasks(self, writer=None):
    """
    Clear all tasks definition in the storage layer for a given partition.
    """
    wr = writer
    if wr is None: wr = self.manager.devWriter()
    empty = std.vector('std::string')()
    self.streamers.data = empty
    self.strReceivers.data = empty
    self.strInfraTasks.data = empty
    self.receivers.data = empty
    self.rcvSenders.data = empty
    self.rcvInfraTasks.data = empty
    self.addTaskDevices(wr)
    if writer is None:
      return wr.execute(0,1)
    return writer

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
