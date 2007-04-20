import Online.PVSS as PVSS
import Online.Utils as Utils
from   Online.RunInfo import RunInfo as RunInfo

log        = Utils.log
error      = Utils.error
std        = PVSS.gbl.std
DataPoint  = PVSS.DataPoint
printSlots = Utils.printSlots

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
        @param  name          Name of the Storage Partition datapoint

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
  def load(self, rdr):
    "Load all datapoints contained in the storage partition description"
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
  def show(self, extended=None):
    "Show all information from the Storage Partition structure."
    p = self.datapoints
    if p[1].data:
      run = None
      partName  = p[0].data
      strmNodes = p[5].data
      recvNodes = p[3].data
      if extended and DataPoint.exists(p[0].data+'_RunInfo'):
        run = RunInfo(self.manager,p[0].data).load()
      log('------------------------------- '+self.name+' -------------------------------',1)
      log('-> Partition:'+self.name+' is used by '+partName)
      printSlots(p[3].data,'   Receiving nodes in use['+str(len(recvNodes))+']:',99,10)
      log('   Receiving layer slices allocated:%d'%len(p[4].data))
      if run:
        for i in xrange(len(run.receivers.data)):
          farm,node,task,nick,type = run.receivers.data[i].split('/')
          slot = p[4].data[i]
          log('     Farm:%-8s sends to %-14s using slot:%s'%(farm,task,slot))
        log('   Receiving layer senders:')
        for i in run.rcvSenders.data:
          node,task,nick,type,tnode,target = i.split('/')
          log('     %-36s on %-10s sends %-12s to %-36s on %s'%(task,node,type,target,tnode))
        log('   Infrastructure Tasks:')
        for i in run.rcvInfraTasks.data:
          node,task,nick,type = i.split('/')
          log('     %-32s on %-12s as %s'%(task,node,type))
      else:
        printSlots(p[4].data,'           ',5)
      log('')
      printSlots(p[5].data,'   Streaming nodes in use['+str(len(strmNodes))+']:',99,10)
      log('   Streaming layer slices allocated:%d'%len(p[6].data))
      if run:
        log('   Receiver Tasks:')
        for i in run.strReceivers.data:
          node,task,nick,type = i.split('/')
          log('     %-36s on %8s of type:%s'%(task,node,type))
        log('   Streaming Tasks:')
        for i in run.streamers.data:
          node,task,nick,type = i.split('/')
          log('     %-36s on %8s streams:%s'%(task,node,type))
        printSlots(run.streamers.data,'  ',1)
        log('   Infrastructure Tasks:')
        for i in run.strInfraTasks.data:
          node,task,nick,type = i.split('/')
          log('     %-32s on %-12s as %s'%(task,node,type))
      else:
        printSlots(p[6].data,'           ',5)
    else:
      log('-> Partition:'+self.name+' is FREE.')

