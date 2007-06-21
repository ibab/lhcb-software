import time
import Online.PVSS as PVSS
import Online.Utils as Utils
from Online.RunInfoClasses.General import General as General
log       = Utils.log
error     = Utils.error
std       = PVSS.gbl.std
printSlots  = Utils.printSlots

# ===========================================================================
def getNodesFromSlots(slots):
  nodes = {}
  for i in slots:
    nodes[i[:i.find(':')]] = 1
  return nodes.keys()

# ===========================================================================
def showPartition(info,partition):
  p = partition.datapoints
  name = partition.name
  if p[1].data:
    run = info
    partName  = p[0].data
    strmNodes = p[5].data
    recvNodes = p[3].data
    log('------------------------------- '+name+' -------------------------------',1)
    log('-> Partition:'+name+' is used by '+partName)
    printSlots(p[3].data,'   1st. layer nodes in use['+str(len(recvNodes))+']:',99,10)
    log('   1st layer slices allocated:%d'%len(p[4].data))
    if run:
      run.load()
      for i in xrange(len(run.receivers.data)):
        node,task,nick,type,farm = run.receivers.data[i].split('/')
        slot = p[4].data[i]
        log('     Farm:%-8s [%s] sends to %-14s using slot:%s'%(eval(farm)[0],type,task,slot))
      log('   1st. Layer senders:')
      for i in run.rcvSenders.data:
        node,task,nick,type,t = i.split('/')
        tnode,target = eval(t)
        log('     %-32s on %-10s sends %-12s to %-36s on %s'%(task,node,type,target,tnode))
      log('   Infrastructure Tasks:')
      for i in run.rcvInfraTasks.data:
        node,task,nick,type,opt = i.split('/')
        log('     %-32s on %-12s as %s'%(task,node,type))
    else:
      printSlots(p[4].data,'           ',5)
    log('')
    printSlots(p[5].data,'   2nd. Layer nodes in use['+str(len(strmNodes))+']:',99,10)
    log('   2nd Layer slices allocated:%d'%len(p[6].data))
    if run:
      log('   Receiver Tasks:')
      for i in run.strReceivers.data:
        node,task,nick,type,opt = i.split('/')
        opt = eval(opt)
        for i in xrange(len(opt)):
          if i == 0:
            log('     %-32s on %-12s of type:%-10s input:%s'%(task,node,type,str(opt[0])))
          else:
            log('     %-32s    %-12s         %-10s       %s'%('','','',str(opt[i])))
      log('   Streaming Tasks:')
      for i in run.streamers.data:
        node,task,nick,type,opt = i.split('/')
        log('     %-32s on %-12s of type:%-10s streams:%s'%(task,node,type,eval(opt)[0]))
      #printSlots(run.streamers.data,'  ',1)
      log('   Infrastructure Tasks:')
      for i in run.strInfraTasks.data:
        node,task,nick,type,opt = i.split('/')
        log('     %-32s on %-12s as %s'%(task,node,type))
    else:
      printSlots(p[6].data,'           ',5)
    return
  log('-> Partition:'+name+' is FREE.')

# =============================================================================
class Storage:
  """ @class Storage
  
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

    self.reader.add(self.flag)
    self.reader.add(self.streams)
    self.reader.add(self.rcvInfra)
    self.reader.add(self.strInfra)
    self.reader.add(self.strMult)
    self.reader.add(self.streamers)
    self.reader.add(self.strReceivers)
    self.reader.add(self.strInfraTasks)
    self.reader.add(self.rcvSenders)
    self.reader.add(self.rcvInfraTasks)
    
  # ===========================================================================
  def addDevices(self, deviceIO):
    "Add task devices to device IO structure for read/write access"
    wr = deviceIO
    if wr is None: wr = self.manager.devWriter()
    # log('Adddevices:Storage wr='+str(wr))

    wr.add(self.streamers)
    wr.add(self.strReceivers)
    wr.add(self.strInfraTasks)
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
    log('Output to %d logical streams'%len(self.streams.data))
    for i in xrange(len(streams)):
      log(' -> Data stream:%-24s with  mutiplicity:%d'%(streams[i],multiplicity[i]))
  # ===========================================================================
  def show(self):
    "Show all information from the Storage structure."
    if self.load():
      self.showStreams()
      self.showGeneral()
      return
    error('Failed to load RunInfo for partition:'+self.name)

  # ===========================================================================
  def defineTasks(self,recv_slots,strm_slots):
    """
    Define all tasks in the storage layer for a given partition.
    The result is storen in runInfo datapoints for further processing.
    
    """
    # log('Storage: Defining tasks for partition '+self.name)
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
    self.rcvSenders.data.clear()
    self.rcvInfraTasks.data.clear()
    
    for i in xrange(len(strm_slots)):
      slot = strm_slots[i]
      node = slot[:slot.find(':')]
      item = streams[i][0]+('_%02d'%streams[i][1])
      short_name = 'WRT'+item
      task = self.name+'_'+node+'_WRT'+item
      self.streamers.data.push_back(node+'/'+task+'/'+short_name+'/WRT'+streams[i][0]+'/("'+item+'",)')
      short_name = 'RCV'+item
      task = self.name+'_'+node+'_RCV'+item
      self.strReceivers.data.push_back(node+'/'+task+'/'+short_name+'/RCV'+streams[i][0])
      streamers.append([self.name,node,slot,'%02d'%streams[i][1],streams[i][0]])
      
    for j in strmNodes:
      for i in self.strInfra.data:
        self.strInfraTasks.data.push_back(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+'/("'+i+'",)')
    rcv_targets = {}
    for i in xrange(len(streamers)): rcv_targets[i] = []
    for j in recvNodes:
      for i in xrange(len(streamers)):
        part,node,slot,ident,type = streamers[i]
        short_name = type+'_'+ident
        sender = part+'_'+j+'_SND'+short_name
        target = part+'_'+node+'_RCV'+short_name
        rcv_targets[i].append((j+'-d1',sender))
        self.rcvSenders.data.push_back(j+'/'+sender+'/SND'+short_name+'/SND'+type+'/[("'+node+'-d1","'+target+'")]')
      for i in self.rcvInfra.data:
        self.rcvInfraTasks.data.push_back(j+'/'+self.name+'_'+j+'_'+i+'/'+i+'/'+i+'/("'+i+'",)')

    for i,data in rcv_targets.items():
      self.strReceivers.data[i] = self.strReceivers.data[i]+'/'+str(data)

  # ===========================================================================
  def clearTasks(self, writer):
    """
    Clear all tasks definition in the storage layer for a given partition.
    """
    empty = std.vector('std::string')()
    self.streamers.data = empty
    self.strReceivers.data = empty
    self.strInfraTasks.data = empty
    self.rcvSenders.data = empty
    self.rcvInfraTasks.data = empty
    self.addDevices(writer)
    return writer

# =============================================================================
class StorageInfo(Storage,General):
  """ @class StorageInfo
  
      Connect to all storage related datapoints in the RunInfo object
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
    Storage.__init__(self)
