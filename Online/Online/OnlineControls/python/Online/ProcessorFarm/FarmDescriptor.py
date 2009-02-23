import Online.RunInfoClasses.General
import Online.AllocatorControl, Online.DatapointLoader, Online.Utils, Online.PVSS, Online.PVSSSystems
import Online.Streaming.PartitionInfo
from   Online.Streaming.Allocator import FSMmanip as FSMmanip
import Online.ProcessorFarm.FarmSetup as FarmSetup
import Online.RunInfoClasses.RecStorage as RunInfo
import Online.SetupParams as Params
import Online.JobOptions.JobOptions as JobOptions
import socket, time

PartitionInfo = Online.Streaming.PartitionInfo.PartitionInfo
DP     = Online.PVSS.DataPoint
std    = Online.PVSS.gbl.std
log    = Online.Utils.log
error  = Online.Utils.error
debug  = None

# Enumerations to access task item fields in the storage
STORAGE_NODENAME=0
STORAGE_TASKUTGID=1
STORAGE_TASKNAME=2
STORAGE_OPTITEMS=8

# =============================================================================
class FarmSlice(Online.DatapointLoader.DatapointLoader):
  """ @class FarmActivity
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)
    self.dpName               = self.name
    self.inuse                = self.dp('InUse')
    self.slice                = self.dp('FSMSlice')
    self.info                 = self.dp('RunInfo')
    self.addDp(self.reader)

  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.inuse)
    deviceIO.add(self.slice)
    deviceIO.add(self.info)
    return self

  # ===========================================================================
  def reset(self):
    "Add all datapoints to deviceIO object"
    self.inuse.data = 0
    self.slice.data = ''
    self.info.data = ''
    return self

# =============================================================================
class FarmRunInfo(Online.RunInfoClasses.General.General):
  """ @class FarmActivity
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    Online.RunInfoClasses.General.General.__init__(self,manager,name,complete=None,postfix='')
    self.addStorage()

  # ===========================================================================
  def copyTo(self,to):
    "INCOMPLETE!"
    if hasattr(self,'runTyp') and hasattr(to,'runTyp'):
      to.runTyp.data        = self.runTyp.data
      to.partition.data     = self.partition.data
      to.partID.data   = self.partID.data
    if hasattr(self,'nSubFarm'):
      to.nSubFarm.data    = self.nSubFarm.data
      to.subFarms.data    = self.subFarms.data
    if hasattr(self,'storeFlag'):
      to.storeFlag.data   = self.storeFlag.data
      to.storageDir.data  = self.storageDir.data
      to.storeSlice.data  = self.storeSlice.data
      to.rcvStrategy.data = self.rcvStrategy.data
      to.rcvInfra.data    = self.rcvInfra.data
      to.strStrategy.data = self.strStrategy.data
      to.strInfra.data    = self.strInfra.data
      to.streams.data     = self.streams.data
      to.strMult.data     = self.strMult.data
    return self
    
  # ===========================================================================
  def copyFrom(self,fr):
    "INCOMPLETE!"
    if hasattr(self,'runTyp') and hasattr(fr,'runTyp'):
      self.runTyp.data        = fr.runTyp.data
      self.partition.data     = fr.partition.data
      self.partID.data        = fr.partID.data
    if hasattr(self,'nSubFarm') and hasattr(fr,'nSubFarm'):
      self.nSubFarm.data    = fr.nSubFarm.data
      self.subFarms.data    = fr.subFarms.data
    if hasattr(self,'storeFlag'):
      print self.name,'COPY STORAGE INFO....'
      self.storeFlag.data   = fr.storeFlag.data
      self.storeSlice.data  = fr.storeSlice.data
      self.storageDir.data  = fr.storageDir.data
      self.rcvStrategy.data = fr.rcvStrategy.data
      self.rcvInfra.data    = fr.rcvInfra.data
      self.strStrategy.data = fr.strStrategy.data
      self.strInfra.data    = fr.strInfra.data
      self.streams.data     = fr.streams.data
      self.strMult.data     = fr.strMult.data
    return self
  
  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    if hasattr(self,'runTyp'):
      deviceIO.add(self.runTyp)
      deviceIO.add(self.partition)
      deviceIO.add(self.partID)
    if hasattr(self,'nSubFarm'):
      deviceIO.add(self.nSubFarm)
      deviceIO.add(self.subFarms)
    if hasattr(self,'storeFlag'):
      deviceIO.add(self.storeFlag)
      deviceIO.add(self.storageDir)
      deviceIO.add(self.storeSlice);
      deviceIO.add(self.rcvStrategy)
      deviceIO.add(self.rcvInfra)
      deviceIO.add(self.strStrategy)
      deviceIO.add(self.strInfra)
      deviceIO.add(self.streams)
      deviceIO.add(self.strMult)
    return self
    
  # ===========================================================================
  def reset(self):
    if hasattr(self,'runTyp'):
      self.runTyp.data          = ''
      self.partition.data       = ''
      self.partID.data     = 0
    if hasattr(self,'nSubFarm'):
      self.nSubFarm.data      = 0
      self.subFarms.data.clear()
    if hasattr(self,'storeFlag'):
      self.storeFlag.data     = 1
      self.storageDir.data    = ''
      self.storageSlice.data  = ''
      self.rcvStrategy.data = 0
      self.rcvInfra.data.clear()
      self.strStrategy.data = 0
      self.strInfra.data.clear()
      self.streams.data.clear()
      self.strMult.data.clear()
    return self
 
# =============================================================================
class FarmActivity(Online.DatapointLoader.DatapointLoader):
  """ @class FarmActivity
  

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the datapoint

        @return reference to initialized object
    """
    Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)
    self.dpName               = self.name
    self.activityName         = self.dp('Name')
    self.farmInfrastructure   = self.dp('Farm.Infrastructure')
    self.farmWorker           = self.dp('Farm.Worker')
    self.ctrlInfrastructure   = self.dp('Control.Infrastructure')
    self.inputInfrastructure  = self.dp('Storage.streamInfrastructure')
    self.inputTypes           = self.dp('Storage.streamTypes')
    self.outputInfrastructure = self.dp('Storage.recvInfrastructure')
    self.outputTypes          = self.dp('Storage.recvTypes')
    self.addDp(self.reader)

  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.activityName)
    deviceIO.add(self.farmInfrastructure)
    deviceIO.add(self.farmWorker)
    deviceIO.add(self.ctrlInfrastructure)
    deviceIO.add(self.inputInfrastructure)
    deviceIO.add(self.inputTypes)
    deviceIO.add(self.outputInfrastructure)
    deviceIO.add(self.outputTypes)
    return self

  # ===========================================================================
  def show(self):
    "Show activity information"
    if self.load():
      log('Activity information:'+self.activityName.data,timestamp=1,with_trailer=1,with_header=1)
      log('Controls PC task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.ctrlInfrastructure.data]),timestamp=1)
      log('Farm task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.farmInfrastructure.data]),timestamp=1)
      log('    Task types:    '+str([i for i in self.farmWorker.data]),timestamp=1)
      log('Input task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.inputInfrastructure.data]),timestamp=1)
      if len(self.inputTypes.data)>1:
        log('    Reader type:   '+self.inputTypes.data[1],timestamp=1)
        log('    Sender type:   '+self.inputTypes.data[0],timestamp=1)
      log('Output task setup:',timestamp=1)
      log('    Infrastructure:'+str([i for i in self.outputInfrastructure.data]),timestamp=1)
      if len(self.outputTypes.data)>1:
        log('    Receiver type: '+self.outputTypes.data[0],timestamp=1)
        log('    Writer type:   '+self.outputTypes.data[1],timestamp=1,with_trailer=1)

# =============================================================================
class FarmDescriptor(Online.DatapointLoader.DatapointLoader):
  """ @class FarmDescriptor
      This class describes the shared farm control structure.
   
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Streaming control datapoint

        @return reference to initialized object
    """
    Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)
    self.strerror   = ''
    self.inUse      = self.dp('InUse')
    self.subfarms   = self.dp('SubFarms')
    self.writer     = self.manager.devWriter()
    self.reader.add(self.inUse)
    self.reader.add(self.subfarms)

  # ===========================================================================
  def error(self,msg,timestamp=1):
    error(msg,timestamp=timestamp)
    self.strerror = msg
    return None
  
  # ===========================================================================
  def show(self, extended=None):
    "Show the farm sustem content."
    if self.subfarms.value() is None: self.load()
    subfarms = self.subfarms.data
    inuse  = self.inUse.data
    log('System: '+self.name+' has '+str(len(subfarms))+' subfarms.')
    if len(subfarms) > 0:
      for i in xrange(len(subfarms)):
        log('       %-32s  in use by "%s"'%(subfarms[i],inuse[i]),timestamp=None)

  # ===========================================================================
  def allocate(self,partition, num_farms, activity, runinfo_dp):
    """
    Allocate num_farms subfarms for a partition.
    @param  partition     Partition name as string
    @param  num_farms     Number of subfarms to be allocated
    @param  runinfo_dp    Datapoint name with the farm configuration parameters
    
    @return Reference to self in case of success. None in case of failure
    """
    if self.load():
      nf = num_farms
      got = 0
      used  = Online.PVSS.StringVector()
      farms = Online.PVSS.StringVector()
      dpv   = Online.PVSS.DataPointVector()
      for i in xrange(len(self.inUse.data)):
        f = self.inUse.data[i]
        n = self.subfarms.data[i]
        if len(f)==0 and got<nf:
          farm_name = n.split('/')[0]
          farm_sys  = n.split('/')[1]
          farm_mgr  = Online.PVSSSystems.controlsMgr(farm_sys)
          dpv.clear()
          dpv.push_back(DP(farm_mgr,DP.original(self.name+'_'+farm_name+'.UsedBy')))
          dpv.back().data = partition
          dpv.push_back(DP(farm_mgr,DP.original(self.name+'_'+farm_name+'.RunInfo')))
          dpv.back().data = runinfo_dp
          dpv.push_back(DP(farm_mgr,DP.original(self.name+'_'+farm_name+'.Activity')))
          dpv.back().data = activity
          wr = farm_mgr.devWriter()
          wr.add(dpv)
          if wr.execute() is None:
            self.error('Failed to write datapoints for farm:'+farm_name)
          used.push_back(partition)
          farms.push_back(farm_name)
          got = got + 1
        else:
          used.push_back(f)
      if got==nf:
        dpv.clear()
        if len(runinfo_dp)>0:
          #dpv.push_back(self.dp2(runinfo_dp,'general.partName'))
          #dpv.back().data = partition
          #dpv.push_back(self.dp2(runinfo_dp,'HLTFarm.nSubFarms'))
          #dpv.back().data = nf
          dpv.push_back(self.dp2(runinfo_dp,'HLTFarm.subFarms'))
          dpv.back().data = farms
        self.inUse.data = used
        self.writer.add(dpv)
        self.writer.add(self.inUse)
        if self.writer.execute():
          return 'SUCCESS'
        self.error('Failed to update allocation information for partition '+\
                   partition+' in farm system:'+self.name)
        return None
      return self.error('Not enough free subfarms availible for partition '+partition+\
                        ' in farm system:'+self.name)
    return self.error('Failed to load information for partition '+partition+\
                      ' in farm system:'+self.name)

  # ===========================================================================
  def free(self,partition,runinfo_dp):
    """
    Free all subfarms allocated by one partition.
    @param  partition     Partition name as string
    
    @return Reference to self in case of success. None in case of failure
    """
    if self.load():
      res = Online.PVSS.StringVector()
      dpv = Online.PVSS.DataPointVector()
      for i in xrange(len(self.inUse.data)):
        n = self.subfarms.data[i]
        f = self.inUse.data[i]
        if f==partition:
          res.push_back('')
          farm_name = n.split('/')[0]
          farm_sys  = n.split('/')[1]
          farm_mgr  = Online.PVSSSystems.controlsMgr(farm_sys)
          dpv.clear()
          dpv.push_back(DP(farm_mgr,DP.original(self.name+'_'+farm_name+'.UsedBy')))
          dpv.back().data = ''
          dpv.push_back(DP(farm_mgr,DP.original(self.name+'_'+farm_name+'.RunInfo')))
          dpv.back().data = ''
          dpv.push_back(DP(farm_mgr,DP.original(self.name+'_'+farm_name+'.Activity')))
          dpv.back().data = ''
          wr = farm_mgr.devWriter()
          wr.add(dpv)
          if wr.execute() is None:
            self.error('Failed to write datapoints for farm:'+farm_name)
        else:
          res.push_back(f)
      dpv.clear()
      if len(runinfo_dp)>0:
        #dpv.push_back(self.dp2(runinfo_dp,'general.partName'))
        #dpv.back().data = ''
        #dpv.push_back(self.dp2(runinfo_dp,'HLTFarm.nSubFarms'))
        #dpv.back().data = 0
        dpv.push_back(self.dp2(runinfo_dp,'HLTFarm.subFarms'))
        dpv.back().data = Online.PVSS.StringVector()
      self.inUse.data = res
      self.writer.add(dpv)
      self.writer.add(self.inUse)
      if self.writer.execute():
        return 'SUCCESS'
      return self.error('Failed to update deallocation information for partition '+\
                        partition+' in farm system:'+self.name)
    return self.error('Failed to load information for partition '+partition+\
                      ' in farm system:'+self.name)

  # ===========================================================================
  def subFarms(self,partition,full_name=None):
    """
    Access the subfarms of one partition as list.
    @param  partition     Partition name as string
    
    @return Reference to self in case of success. None in case of failure
    """
    if self.inUse.value() is None: self.load()
    got = []
    for i in xrange(len(self.inUse.data)):
      if self.inUse.data[i]==partition:
        if full_name:
          got.append(self.name+'_'+self.subfarms.data[i])
        else:
          got.append(self.subfarms.data[i])
    return got


# =============================================================================
class SubFarmDescriptor(Online.DatapointLoader.DatapointLoader):
  """ @class StreamingDescriptor
      This class describes the shared streaming control structure.
   
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the control datapoint

        @return reference to initialized object
    """
    Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)
    self.strerror    = ''
    self.sfName      = self.dp('Name')
    self.nodes       = self.dp('Nodes')
    self.usedby      = self.dp('UsedBy')
    self.setup       = self.dp('Activity')
    self.runInfo     = self.dp('RunInfo')
    self.writer      = self.manager.devWriter()
    self.addDp(self.reader)

  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.sfName)
    deviceIO.add(self.nodes)
    deviceIO.add(self.usedby)
    deviceIO.add(self.setup)
    deviceIO.add(self.runInfo)
    
  # ===========================================================================
  def subfarmName(self):
    "Return the name of the processing farm"
    return self.sfName.data
  # ===========================================================================
  def processors(self):
    "Return the subfarm names"
    return self.nodes.data
  # ===========================================================================
  def usedBy(self):
    "Return used by string"
    return self.usedby.data
  # ===========================================================================
  def detectorName(self):
    "Return used by string, which is equivalent to the detector name"
    return self.usedby.data
  # ===========================================================================
  def show(self, prefix=''):
    "Show sub-farm information"
    if self.load():
      n = prefix+'Subfarm:'+self.name+':'+self.subfarmName()
      if self.usedBy() != '': n = n + '  Used by '+self.usedBy()
      log(n,timestamp=1)
      fmt = '%sNodes:%s'
      s = ''
      nodes = self.processors()
      num = len(nodes)
      for n in xrange(num):
        q = n+1
        s = s + '%s  '%(nodes[n])
        if (q%5)==0 or (q==num):
          log(fmt%(prefix,s),timestamp=1,with_trailer=q==num)
          s = ''
  # ===========================================================================
  def defineTasks(self):
    items = self.setup.data.split(':')
    print 'Definetasks:',self.setup.name(),self.setup.data,items,len(items)
    if len(items)>1:
      sys = items[0]
      act = items[1]
    elif len(items)==1:
      sys = self.manager.name()
      act = items[0]
    else:
      error('Invalid run information content in setup datapoint:'+self.setup.name()+': '+self.setup.data,timestamp=1)
      return None
    mgr = Online.PVSSSystems.controlsMgr(sys)
    activity = FarmActivity(mgr,act).load()
    if activity:
      tasks = []
      nodes = self.processors()
      sys   = self.manager.name()
      sfn   = self.manager.hostName().upper()
      part  = self.usedBy()  #activity.activityName.data
      #
      # node/task-name/short-name/task-type/class/dim-dns/partition/sub-farm
      # MONA0801/LHCb_MONA0801_MBMRelay/MBMRelay/MBMRelay/Class0/mona08/MONITORING/("MBMRelay",)
      #
      # Setup tasks running on the controls PC
      dns = sfn
      for i in activity.ctrlInfrastructure.data:
        typ,clazz = i.split('/')
        task = '%s/%s_%s_%s/%s/%s/%s/%s/%s/%s'%(sfn,part,sfn,typ,typ,typ,clazz,dns,sys,part)
        tasks.append(task)
      # Setup tasks on worker nodes
      for l in xrange(len(nodes)):
        n,ncpu = nodes[l].split('/')
        # Infrastructure tasks
        for i in activity.farmInfrastructure.data:
          typ,clazz = i.split('/')
          task = '%s/%s_%s_%s/%s/%s/%s/%s/%s/%s'%(n,part,n,typ,typ,typ,clazz,dns,sys,part)
          tasks.append(task)
        # Worker tasks
        typ,clazz = activity.farmWorker.data.split('/')
        for num in xrange(int(ncpu)):
          task = '%s/%s_%s_%s_%02d/%s_%02d/%s/%s/%s/%s/%s'%(n,part,n,typ,num,typ,num,typ,clazz,dns,sys,part)
          tasks.append(task)
      return tasks
    error('Failed to load data for activity:'+str(activity),timestamp=1)
    return None

# =============================================================================
class SubFarmFSM:
  """ @class SubFarmFSM
   
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self,fsm_manip,subfarmDescriptor):
    self.manip = fsm_manip
    self.subfarm = subfarmDescriptor
    self.manip.configureTask = self._configureTask

  # ===========================================================================
  def _configureTask(self, fsm_node, item, task):
    "Configure single task object"
    node    = task[0]
    utgid   = task[1]
    sysname = task[6]
    dimdns  = task[5]
    type    = task[3]
    clazz   = task[4]
    script  = FarmSetup.gaudiScript
    account = 'online'
    detector= 'ANY'
    script,account,detector=self.manip._startupInfo(type)
    if len(account) == 0: account = 'online'
    opts    = type+'.opts'
    cmd = sysname+'#-E /tmp/logReco.fifo -O /tmp/logReco.fifo -c -u '+utgid+' -n '+account+\
          ' -D TASKTYPE='+type+\
          ' -D TASKCLASS='+clazz+\
          ' -D PARTITION='+self.subfarm.usedby.data+\
          ' -D LOGFIFO=/tmp/logReco.fifo'
    if len(detector)>0: cmd = cmd + ' -D DETECTOR='+detector
    cmd = cmd + ' '+script+' '+opts+' '+clazz+' '+type+' '+dimdns
    self.manip.setupTask(item,node=fsm_node,name=utgid,type=type,inUse=1,prio=0,cmd=cmd,sysname=sysname,dimdns=dimdns,nodename=node)
    return self

# =============================================================================
class SubFarmConfigurator(Online.AllocatorControl.AllocatorClient,Online.DatapointLoader.DatapointLoader):
  """ @class StreamingDescriptor
      This class describes the shared streaming control structure.
   
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the control datapoint

        @return reference to initialized object
    """
    Online.AllocatorControl.AllocatorClient.__init__(self,manager,name)
    Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)

  # ===========================================================================
  def get(self,name):
    """ Compatibility with AllocatorControl interface.
        @param internal datapoint name
        @return Initialized PVSS datapoint
    """
    return self.dp2(self.name+'Alloc',name)

  # ===========================================================================
  def configure(self,rundp_name,partition):
    "Configure partition content after all information is set."
    dsc = SubFarmDescriptor(self.manager,self.name).load()
    if dsc:
      tasks = dsc.defineTasks()
      if tasks:
        fsm_manip = FSMmanip(dsc,'_FwFsmDevice',match='*')
        sfManip = SubFarmFSM(fsm_manip,dsc)
        slots = fsm_manip.collectTaskSlots()
        if slots:
          used_tasks = {}
          for t in tasks:
            items = t.split('/')
            if not used_tasks.has_key(items[0]): used_tasks[items[0]] = []
            used_tasks[items[0]].append(items)
          dsc.show()
          fsm_manip.disableTasks(slots)
          slots = fsm_manip.allocateProcesses(used_tasks,slots)
          if slots is None:
            return None
          if fsm_manip.commit(slots) is None:
            self.manip=None
            return None
          self.manip=None
          return self
        error('No task slots found of:'+self.name+' for partition:'+partition,timestamp=1)
        return None
      error('Failed to configure slots of:'+self.name+' for partition:'+partition,timestamp=1)
      return None
    error('Failed to access config info of subfarm:'+self.name+' for partition:'+partition,timestamp=1)
    return None


# =============================================================================
class FarmConfigurator(FarmDescriptor):
  """ @class FarmConfigurator
   
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the control datapoint
        @return reference to initialized object
    """
    print 'FarmConfigurator',name
    FarmDescriptor.__init__(self,manager,name)
    #Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)
    self.typeMgr = self.manager.typeMgr()
    setup = 'ONLINE'
    setup = 'RECO'
    if setup=='RECO':
      self.allocatePartition = self.allocRecoPartition
      self.getPartition      = self.getRecoPartition
      self.freePartition     = self.freeRecoPartition
      self.loadRunInfo       = self.loadRecoRunInfo
      self.runInfo_type      = self.typeMgr.type('RunFarmInfo')
      self.runinfos = Online.PVSS.DpVectorActor(self.manager)
      self.runinfos.lookupOriginal(self.name+'_Reco??.general.partName',self.runInfo_type)
      for i in self.runinfos.container:
        print 'RunInfo:',i.name()
    else:
      self.allocatePartition = self.allocateSlice
      self.getPartition      = self.getSlice
      self.freePartition     = self.freeSlice
      self.loadRunInfo       = self.loadRecoRunInfo
      self.sliceType         = self.typeMgr.type('FarmSlice')
      self.runInfo_type      = self.typeMgr.type('FarmRunInfo')
    
    self.writer   = self.manager.devWriter()
    self.fsm_typ  = self.typeMgr.type('_FwFsmDevice')

  # ===========================================================================
  def freeSlice(self,context,rundp_name,partition):
    slice,runinfo,farm = context
    if runinfo.partitionName.data != partition:
      print 'Inconsistent partition names:',slice.partitionName.data,partition
    log('Reset:'+slice.name+' farm:'+farm.name+' for partition:'+partition,timestamp=1)
    wr = self.manager.devWriter()
    slice.reset().addDp(wr)
    farm.reset().addDp(wr)
    if wr.execute():
      return 'SUCCESS'
    return self.error('Failed to save slice information '+slice.name+' for partition:'+partition,timestamp=1)

  # ===========================================================================
  def getSlice(self,rundp_name,partition):
    rdr = self.manager.devReader()
    infos = Online.PVSS.DpVectorActor(self.manager)
    infos.lookupOriginal(self.name+'_Slice??.RunInfo',self.sliceType)
    rdr.add(infos.container)
    if rdr.execute():
      for info in infos.container:
        if info.data==rundp_name:
          nam = self.manager.dpSysElementName(info.name())
          slice = FarmSlice(self.manager,nam).load()
          rinfo = FarmRunInfo(self.manager,slice.info.data).load()
          farm  = FarmRunInfo(self.manager,slice.slice.data).load()
          if rinfo.partitionName.data != partition:
            print 'Inconsistent partition names:',rinfo.partitionName.data,partition
          log('Got partition:'+info.name()+' '+info.data,timestamp=1)
          return (slice.slice.data,(slice,rinfo,farm))
      return None
    return self.error('Failed to load slice information for partition:'+partition,timestamp=1)

  # ===========================================================================
  def allocateSlice(self,rundp_name,partition):
    #rundp_name = 'RECOTEST:LHCb_RunInfo'
    res = self.getPartition(rundp_name,partition)
    if res:
      return self.error('[ALREADY_ALLOCATED] Failed to allocate subfarms for partition:'+partition,timestamp=1)

    rdr = self.manager.devReader()

    infos = Online.PVSS.DpVectorActor(self.manager)
    infos.lookupOriginal(self.name+'_Reco??.general.partName',self.runInfo_type)
    inuse = Online.PVSS.DpVectorActor(self.manager)
    inuse.lookupOriginal(self.name+'_Slice??.InUse',self.sliceType)

    rdr.add(inuse.container)
    rdr.add(infos.container)
    rdr.execute()

    found = None
    for j in infos.container:
      if j.data == partition:
        found = j
        break
      elif found is None and len(j.data)==0:
        found = j
    if found:
      for i in xrange(len(inuse.container)):
        dp = inuse.container[i]
        if dp.data == 0:
          wr = self.manager.devWriter()
          nam = self.manager.dpSysElementName(dp.name())
          slname = self.manager.dpSysElementName(found.name())
          dp.data = 1
          wr.add(dp)
          slice = FarmSlice(self.manager,nam).load()
          slice.info.data  = rundp_name
          slice.slice.data = slname
          slice.addDp(wr)
          slinfo = FarmRunInfo(self.manager,slname).load()
          slinfo.copyFrom(FarmRunInfo(self.manager,rundp_name).load())
          slinfo.addDp(wr)
          wr.execute()
          return (slname,found)
    return None

  # ===========================================================================
  def loadRecoRunInfo(self,manager,dp):
    do_load = 0
    print 'Loading farm info:',dp,
    info = FarmRunInfo(manager,self.manager.dpElementName(dp))
    info.addBasic()
    info.addHLT()
    if info.load():
      if do_load:
        activity_name = self.manager.dpSysElementName(dp)
        print 'Loading run info from ',info.runTyp.data,' to ',dp
        print 'Storeflag:',info.storeFlag.data,info.storeFlag.name()
        print 'Activity name:',activity_name
        activity_info = FarmRunInfo(self.manager,info.runTyp.data).load()
        info.copyFrom(activity_info)
        wr = self.manager.devWriter()
        info.addDp(wr)
        if wr.execute() is None:
          return None
      info.show()
    return info
  
  # ===========================================================================
  def getRecoPartition(self,rundp_name,partition):
    rdr = self.manager.devReader()
    rdr.add(self.runinfos.container)
    if rdr.execute():
      for i in self.runinfos.container:
        if i.data==partition:
          return (i.name()[:i.name().find('.')],i)
    return None

  # ===========================================================================
  def freeRecoPartition(self,name,rundp_name,partition):
    print 'freePartition:',name,rundp_name,partition
    res = self.getPartition(rundp_name,partition)
    if res is not None:
      wr = None
      #for i in self.runinfos.container:
      #  if i.data==partition:
      #    i.data = ''
      #    if wr is None: wr=self.manager.devWriter()
      #    wr.add(i)
      if wr: wr.execute()
    return None

  # ===========================================================================
  def allocRecoPartition(self,rundp_name,partition):
    res = self.getPartition(rundp_name,partition)
    if res is None:
      for i in self.runinfos.container:
        if len(i.data)==0:
          wr = self.manager.devWriter()
          i.data = partition
          wr.add(i)
          wr.execute()
          return (i.name()[:i.name().find('.')],i)
    return res

  # ===========================================================================
  def _fsmLookup(self,dp,type):
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookupOriginal(self.name+'|'+self.name+'_'+self.match+'.'+dp,type)
    return obj
  
  # ===========================================================================
  def get(self,name):
    """ Compatibility with AllocatorControl interface.
        @param internal datapoint name
        @return Initialized PVSS datapoint
    """
    return self.dp2(self.name+'Alloc',name)

  # ===========================================================================
  def configureFSM(self,farm_name,partition,action):
    #Trigger_Farm00|Trigger_HLT000|Trigger_HLT000.mode.enabled 1
    #Trigger_Farm00|FSM_SLice_FWDM.fsm.sendCommand             DISABLE/DEVICE=TRIGGER_HLT000::TRIGGER_HLT000

    if self.load():
      dpv   = Online.PVSS.DataPointVector()
      for i in self.subfarms.data:
        n = i.split('/')[0]
        sys = i.split('/')[1]
        mgr = Online.PVSSSystems.controlsMgr(sys)
        rdr = mgr.devReader()
        dp = DP(mgr,DP.original(self.name+'_'+n+'.UsedBy'))
        rdr.add(dp)
        if rdr.execute() is None:
          log('ConfigureFSM Failed to read datapoint:'+dp.name(),timestamp=1)
          
        elt = mgr.dpElementName(dp.name())
        dpv.push_back(self.dp2(farm_name+'|'+elt+'|'+elt,'mode.enabled'))
        print dp.name(),elt
        if dp.data==partition and action=='ENABLE':
          dpv.back().data = 1
          cmd = 'ENABLE/DEVICE(S)='+elt+'::'+elt
          if debug: log('ConfigureFSM [INCLUDED]:'+farm_name+' '+elt+' '+dpv.back().name(),timestamp=1)
        else:
          dpv.back().data = 0
          cmd = 'DISABLE/DEVICE(S)='+elt+'::'+elt
          if debug: log('ConfigureFSM [EXCLUDED]:'+farm_name+' '+elt+' '+dpv.back().name(),timestamp=1)
        dpv.push_back(self.dp2(farm_name+'|FSM_Slice_FWDM','fsm.sendCommand'))
        dpv.back().data = cmd.upper()
        wr = self.manager.devWriter()
        wr.add(dpv)
        if wr.execute() is None:
          return self.error('Failed to update FSM machines for partition:'+partition,timestamp=1)
        dpv.clear()
      return self
    return self.error('Failed to read subfarm information for partition:'+partition,timestamp=1)
  
  # ===========================================================================
  def configure(self,rundp_name,partition):
    # import Online.ProcessorFarm.Sopts as Sopts
    time.sleep(2)
    res = self.getPartition(rundp_name,partition)
    if res:
      name,dp = res
      run_info = self.loadRunInfo(self.manager,name)
      if run_info:
        numSF = run_info.nSubFarm.data
        nam = run_info.storageSlice()
        storage = PartitionInfo( Online.PVSSSystems.controlsMgr(FarmSetup.storage_system),nam).load()
        if storage:
          Online.Streaming.PartitionInfo.showRecStorage(storage,extended=1)
          if not writeOptions(run_info, storage, self):
            return None
          return self
    return None
  
  # ===========================================================================
  def allocate(self,rundp_name,partition):
    #partition = rundp_name
    res = self.allocatePartition(rundp_name,partition)
    if res:
      name,dp = res
      run_info = self.loadRunInfo(self.manager,name)
      if run_info:
        numSF = run_info.nSubFarm.data
        activity = run_info.runTyp.data
        if FarmDescriptor.allocate(self,partition,numSF,activity,name):
          if self.configureFSM(name,partition,'ENABLE'):
            return 'SUCCESS'
          return self.error('Failed to configure subfarm FSMs for partition:'+partition,timestamp=1)
        return self.error('Failed to load run-type for partition:'+partition,timestamp=1)
      return self.error('Failed to allocate subfarms for partition:'+partition,timestamp=1)
    return self.error('Failed to access run info for partition:'+partition,timestamp=1)
  
  # ===========================================================================
  def free(self,rundp_name,partition):
    #partition = rundp_name
    res = self.getPartition(rundp_name,partition)
    print 'free:',rundp_name,partition
    if res:
      name,context = res
      if FarmDescriptor.free(self,partition,name):
        if self.configureFSM(name,partition,'DISABLE'):
          self.freePartition(context,rundp_name,partition)
          print 'free [6]:',name,context
          return 'SUCCESS'
        return self.error('Failed to reset subfarm FSMs for partition:'+partition,timestamp=1)
      return self.error('Failed to free subfarms for partition:'+partition,timestamp=1)
    return self.error('Failed to access run info for partition:'+partition,timestamp=1)
    
  # ===========================================================================
  def recover(self,rundp_name,partition):
    res = self.free(rundp_name,partition)
    if res:
      return res
    return 'SUCCESS'

  # ===========================================================================
  def recover_slice(self,rundp_name,partition):
    return self.recover(rundp_name,partition)

# =============================================================================
def testAllocation(name):
  mgr=Online.PVSS.controlsMgr()
  fd = FarmDescriptor(mgr,name)
  log('Initial farm allocation',timestamp=1)
  fd.show(extended=1)
  log('Allocate 2 subfarms for LHCb',timestamp=1)
  fd.allocate('LHCb',2,mgr.name()+':FarmActivity_REPRO_1','')
  fd.show(extended=1)
  log(str(fd.subFarms('LHCb')),timestamp=1)
  log('Allocate 1 subfarms for RICH2',timestamp=1)
  fd.allocate('RICH2',1,mgr.name()+':FarmActivity_REPRO_2','')
  fd.show(extended=1)
  log(str(fd.subFarms('RICH2')),timestamp=1)
  log('Allocate 1 subfarms for MUON',timestamp=1)
  fd.allocate('MUON',1,mgr.name()+':FarmActivity_REPRO_3','')
  fd.show(extended=1)
  log(str(fd.subFarms('MUON')),timestamp=1)
  log('Free LHCb',timestamp=1)
  fd.free('LHCb','')
  fd.show(extended=1)
  log('Free RICH2',timestamp=1)
  fd.free('RICH2','')
  fd.show(extended=1)

# =============================================================================
def testActivity():
  mgr=Online.PVSS.controlsMgr()
  for i in [mgr.name()+':FarmActivity_REPRO_1',mgr.name()+':FarmActivity_REPRO_2',mgr.name()+':FarmActivity_REPRO_3']:
    a = FarmActivity(mgr,i)
    a.show()
  
# =============================================================================
def testSubFarm(name):
  det = 'RICH1'
  mgr=Online.PVSS.controlsMgr()
  fd = FarmDescriptor(mgr,name)
  log('Allocate 1 subfarms for '+det,timestamp=1)
  fd.free(det,'')
  fd.allocate(det,1,mgr.name()+':FarmActivity_REPRO_2','')
  sfn=fd.subFarms(det,full_name=1)[0]
  sf = SubFarmDescriptor(mgr,sfn)
  sf.show()
  print sf.name
  tasks = sf.defineTasks()
  used_tasks = {}
  for t in tasks:
    items = t.split('/')
    if not used_tasks.has_key(items[0]):
      used_tasks[items[0]] = []
    used_tasks[items[0]].append(items)
    print 'Task:',t.split('/')
  manip = FSMmanip(sf,'_FwFsmDevice',match='*')
  manip.setStartupInfoData({})
  slots = manip.collectTaskSlots()
  #manip.printSlots()
  manip.disableTasks(slots)
  slots = manip.allocateProcesses(used_tasks,slots)
  manip.commit(slots)

def testConfigure(name):
  import Online.ProcessorFarm
  det = 'Markus'
  mgr=Online.PVSS.controlsMgr()
  fd = FarmDescriptor(mgr,name)
  run_info = 'RECOTEST:Trigger_Farm00'
  log('Allocate 1 subfarms for '+det,timestamp=1)
  fd.free(det,run_info)
  fd.allocate(det,2,mgr.name()+':FarmActivity_REPRO_2',run_info)
  sf_names=fd.subFarms(det,full_name=1)
  for sfn in sf_names:
    log('Configuring subfarm:'+sfn,timestamp=1)
    cfg=SubFarmConfigurator(mgr,sfn)
    cfg.configure(run_info,run_info)
  return (mgr,cfg)


# =============================================================================
def _addTasks(data):
  tasks = []
  for i in data:
    for j in i:
      tasks.append(j)
  return tasks

# =============================================================================
class Options:
  # ===========================================================================
  def __init__(self,msg=''):
    "Object constructor"
    self.value = msg
    self.object = 'OnlineEnv'
    if len(self.value)>0: self.value = self.value+'\n'
  # ===========================================================================
  def add(self,name,value=None,operator='='):
    "Add a new options item"
    if value is not None:
      v = str(value)
      if isinstance(value,str): v = '"'+v+'"'
      v = v.replace("'",'"').replace('[','{').replace(']','}')      
      s = '%s.%-16s %s %s;\n'%(self.object,name,operator,v)
      self.value = self.value + s
    elif len(name)>0:
      self.value = self.value + name + '\n'
    return self
  # ===========================================================================
  def append(self,name,value):
    "Append items to existing options"
    return self.add(name,value,'+=')

# =============================================================================
class FarmOptionsWriter:
  """
  OptionsWriter class.
  Base class to writing job options to file

  @author M.Frank
  """
  # ===========================================================================
  def __init__(self, run_info, storage_info, farm_info):
    "Object constructor."
    self.run = run_info
    self.farm = farm_info
    self.storage = storage_info
    self.optionsMgr = None
    self.optionsDir = Params.jobopts_optsdir

  # ===========================================================================
  def optionsManager(self):
    if self.optionsMgr is None:
      self.optionsMgr = Online.PVSSSystems.controlsMgr(Params.jobopts_system_name)
    return self.optionsMgr

  # ===========================================================================
  def getTasks(self):
    good = 1
    names = []
    task_list = []
    i = self.storage
    a = FarmActivity(self.run.manager,self.run.runType()).load()
    tasks = _addTasks([i.recvSenders(),i.recvReceivers(),i.recvInfrastructure(),\
                       i.streamReceivers(),i.streamSenders(),i.streamInfrastructure()])
    mgr = self.optionsManager()
    names = [i.split('/')[3] for i in tasks]
    tasks = _addTasks([a.farmInfrastructure.data,\
                       [a.farmWorker.data],\
                       a.ctrlInfrastructure.data])
    for i in tasks: names.append(i.split('/')[0])
    for name in names:
      task = JobOptions.TaskType(mgr,name)
      if task.exists():
        task_list.append(task)
        continue
      PVSS.error('The task '+name+' does not exist!',timestamp=1,type=Online.PVSS.ILLEGAL_VALUE)
      good = None
    if good is None: task_list = []
    return task_list

  # ===========================================================================
  def writeOptions(self, opts, partition, task):
    return self.writeOptionsFile(partition, task.name, opts.value)

  # ===========================================================================
  def writeOptionsFile(self, partition, name, opts):
    import os
    if self.optionsDir is not None:
      try:
        fd = self.optionsDir+os.sep+partition
        fn = fd+os.sep+name+'.opts'
        log('###   Writing options: '+fn,timestamp=1)
        try:
          os.stat(fd)
        except:
          os.mkdir(fd)
        desc = open(fn, 'w')
        print >>desc, opts
        desc.close()
      except Exception,X:
        PVSS.error('Failed to write options for task:'+name+' ['+str(X)+']',timestamp=1,type=Online.PVSS.FILEOPEN)
        return None
      return self
    PVSS.error('Failed to write options for task:'+name+' [No Directory]',timestamp=1,type=Online.PVSS.FILEOPEN)
    return None

  # ===========================================================================
  def configure(self):
    tasks = self.getTasks()
    if tasks is not None:
      partition = self.run.partitionName()
      run_type = self.run.runType()
      opts = self.partitionOptions()
      if not self.writeOptionsFile(partition, "PartitionInfo", opts.value):
        return None
      for task in tasks:
        opts = Options('//  Auto generated options for partition:'+partition+\
               ' activity:'+run_type+' task:'+task.name+'  '+time.ctime()+'\n' +\
               '#include "$PREAMBLE_OPTS"')
        if task.defaults.data:
          opts.add('//\n// ---------------- General partition information:  ')
          opts.add('#include "$OPTIONSDIR/PartitionInfo.opts"')
          #opts.add('PartitionID',    self.run.partitionID())
          #opts.add('PartitionName',  self.run.partitionName())
          #opts.add('PartitionIDName','%04X'%self.run.partitionID())
          #opts.add('Activity',       run_type);
          opts.add('TaskType',       task.name)
          #opts.add('OutputLevel',    self.run.outputLevel())
          opts.add('MessageSvc.fifoPath = @OnlineEnv.LogFifoName;')
        else: opts.add('// ---------------- NO partition information')
        if task.options.data:
          opts.add('//\n// ---------------- Task specific information:')
          opts.add(task.options.data)
        else: opts.add('// ---------------- NO task specific information')
        if not self.writeOptions(opts,partition,task):
          return None
      if len(tasks)==0:
        log('No tasks found for activity:'+activity,timestamp=1)
      if not self.writeIoOptions():
        return None
      farmOpts=self.farmOptions(self.run,self.storage)
      if not farmOpts:
        return None
      for nodeOptions in farmOpts.values():
        opts = Options( '//  Farm options for partition:'+partition+\
               ' activity:'+run_type+'  '+time.ctime()+'\n' )
        for o in nodeOptions.keys():
          opts.add(o, nodeOptions[o])
        if not self.writeOptionsFile( partition, nodeOptions['opts'], opts.value ):
          return None
      return self.run
    PVSS.error('Cannot retrieve tasks for activity:'+self.run.runType())
    return None

  def farmOptions(self, run_info, storage_info):
    ret = {}
    recvSenders=storage_info.recvSenders()
    if not recvSenders:
      return None
    for recvSender in recvSenders:
      items     = recvSender.split('/')
      nodeInfo  = eval(items[STORAGE_OPTITEMS])
      node      = nodeInfo[0]
      nodeIndex = nodeInfo[1]
      nodeName  = run_info.subFarms.data[nodeIndex]
      nl = nodeName.lower()
      ret[nl] = {}
      print "%s.opts %s::%s" %(nl,items[STORAGE_NODENAME],items[STORAGE_TASKUTGID])
      ret[nl][ 'opts' ] =  nl
      ret[nl][ 'reqNode' ] = items[STORAGE_NODENAME]+'-d1'
      ret[nl][ 'reqTask' ] = items[STORAGE_NODENAME]+'-d1::'+items[STORAGE_TASKUTGID]

    streamReceivers=storage_info.streamReceivers()
    if not streamReceivers:
      return None
    for streamReceiver in streamReceivers:
      items      = streamReceiver.split('/')
      nodeInfo   = eval(items[STORAGE_OPTITEMS])
      node       = nodeInfo[0]
      nodeIndex  = nodeInfo[1]
      nodeName   = run_info.subFarms.data[nodeIndex]
      nl = nodeName.lower()
      print "%s.opts: %s::%s" %(nl,items[STORAGE_NODENAME],items[STORAGE_TASKUTGID])
      if not nl in ret:
        print "Node name does not match!!"
        return None
      ret[nl][ 'targetNode' ] = items[STORAGE_NODENAME]+'-d1'
      ret[nl][ 'targetTask' ] = items[STORAGE_NODENAME]+'-d1::'+items[STORAGE_TASKUTGID]
    return ret 

  def partitionOptions(self):
    partition = self.run.partitionName()
    run_type = self.run.runType()
    opts = Options('//  Auto generated options for partition:'+partition+\
                           ' activity:'+run_type+' OnlineEnv settings:'+'  '+time.ctime()+'\n')
    opts.add('PartitionID',    self.run.partitionID())
    opts.add('PartitionName',  self.run.partitionName())
    opts.add('PartitionIDName','%04X'%self.run.partitionID())
    opts.add('Activity',       run_type);
    #opts.add('OutputLevel',    self.run.outputLevel())
    opts.add('OutputLevel',    4)
    opts.add('LogFifoName' ,  "$LOGFIFO")
    return opts
    
  def writeIoOptions(self):
    partition = self.run.partitionName()
    # DimRPCFileReader
    opts = Options('//  Auto generated options for partition:'+partition+\
                   ' DimRPCFileReader  '+time.ctime()+'\n' +\
                   '#include "$OPTIONSDIR/PartitionInfo.opts"\n' +\
                   '#include "$ONLINETASKS/options/DimRPCFileReader.opts"\n'+\
                   '#include "$ONLINETASKS/options/RecMessageSvc.opts"')
    if not self.writeOptionsFile( partition, 'DimRPCFileReader', opts.value ):
      return None
    # RECIOBuffers
    opts = Options('//  Auto generated options for partition:'+partition+\
                   ' RECBuffers  '+time.ctime()+'\n' +\
                   '#include "$OPTIONSDIR/PartitionInfo.opts"\n'+\
                   '#include "$ONLINETASKS/options/MBM.opts"\n'+\
                   '#include "$ONLINETASKS/options/RecMessageSvc.opts"')
    opts.add('MEPManager.InitFlags       = "-s=7000 -e=100 -u=5 -b=12 -f -i=READ -c";')
    if not self.writeOptionsFile( partition, 'RECIOBuffers', opts.value ):
      return None
    # INPUTSenders
    for i in range(1,3):
      opts = Options('//  Auto generated options for partition:'+partition+\
                     ' INPUTSender  '+time.ctime()+'\n' +\
                     '#include "$OPTIONSDIR/PartitionInfo.opts"\n'+\
                     '#include "$ONLINETASKS/options/SND.opts"\n'+\
                     '#include "$ONLINETASKS/options/RecMessageSvc.opts"')
      opts.add('MEPManager.Buffers                   = {"READ"};')
      opts.add('MEPManager.PartitionBuffers          = true;')
      opts.add('EventSelector.Input                  = "READ";')
      opts.add('EventSelector.Decode                 = false;')
      opts.add('OnlineEnv.Opt0                       = "STORERECV%02X-d1::%s_STORERECV%02X_RecStorageRead";' %(i,partition,i) )
      opts.add('Sender.DataSink                      = @OnlineEnv.Opt0;')
      opts.add('Sender.DataType                      = 3;')
      opts.add('Sender.InputDataType                 = 3;')
      # Hardcoded options for testing
      # opts.add('EventSelector.REQ1                   = "EvType=2;TriggerMask=0x1,0x1,0x1,0x1;VetoMask=0xffffffff,0xffffffff,0xffffffff,0xffffffff;MaskType=ANY;UserType=ONE;Frequency=PERC;Perc=100.0";')
      if not self.writeOptionsFile( partition, 'INPUTSender%02X' %i, opts.value ):
        return None
    # Everything went ok 
    return True
    
def writeOptions(run_info,storage_info,farm_info):
  wr = FarmOptionsWriter(run_info,storage_info,farm_info)
  res = wr.configure()
  return res

import Online.JobOptions.OptionsWriter as OptionsWriter
# =============================================================================
class RecStorageOptionsWriter(OptionsWriter.StreamingOptionsWriter):
  """ Specialized options writer for the storage control node.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    OptionsWriter.StreamingOptionsWriter.__init__(self, manager, 'RECV', name, info, manager, ['RECV','STREAM'])
  # ===========================================================================
  def setupLayer(self, layer):
    i = self.streamInfo
    if layer == self.streamingLayers[0]:
      return OptionsWriter._addTasks([i.recvSenders(),i.recvReceivers(),i.recvInfrastructure()])
    elif layer == self.streamingLayers[1]:
      return OptionsWriter._addTasks([i.streamReceivers(),i.streamSenders(),i.streamInfrastructure()])
    return []
      

if __name__ == "__main__":
  name = 'Trigger'
  #testActivity()
  #testAllocation(name)
  #testSubFarm(name)
  testConfigure(name)
  
