import Online.AllocatorControl, Online.DatapointLoader, Online.Utils, Online.PVSS, Online.PVSSSystems
from   Online.Streaming.Allocator import FSMmanip as FSMmanip
std    = Online.PVSS.gbl.std
log    = Online.Utils.log
error  = Online.Utils.error
debug  = None

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
class FarmRunInfo(Online.DatapointLoader.DatapointLoader):
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
    self.dpName        = self.name
    self.runType       = self.dp('general.runType')
    self.partitionName = self.dp('general.partName')
    self.partitionID   = self.dp('general.activePartId')
    self.nSubFarm      = self.dp('HLTFarm.nSubFarms')
    self.subfarms      = self.dp('HLTFarm.subFarms')
    self.addDp(self.reader)

  # ===========================================================================
  def addDp(self,deviceIO):
    "Add all datapoints to deviceIO object"
    deviceIO.add(self.runType)
    deviceIO.add(self.partitionName)
    deviceIO.add(self.partitionID)
    deviceIO.add(self.nSubFarm)
    deviceIO.add(self.subfarms)
    return self

  # ===========================================================================
  def copyTo(self,to):
    to.runType.data       = self.runType.data
    to.partitionName.data = self.partitionName.data
    to.partitionID.data   = self.partitionID.data
    to.nSubFarm.data      = self.nSubFarm.data
    to.subfarms.data      = self.subfarms.data
    return self
    
  # ===========================================================================
  def copyFrom(self,fr):
    self.runType.data       = fr.runType.data
    self.partitionName.data = fr.partitionName.data
    self.partitionID.data   = fr.partitionID.data
    self.nSubFarm.data      = fr.nSubFarm.data
    self.subfarms.data      = fr.subfarms.data
    return self
    
  # ===========================================================================
  def reset(self):
    self.runType.data       = ''
    self.partitionName.data = ''
    self.partitionID.data   = 0
    self.nSubFarm.data      = 0
    self.subfarms.data      = std.vector('std::string')()
    return self
    
  # ===========================================================================
  def show(self):
    "Show activity information"
    if self.load():
      log('Partition information: '+self.name,timestamp=1,with_trailer=1,with_header=1)
      log('    Partition name:    '+self.partitionName.data,timestamp=1)
      log('    Partition ID:      '+str(self.partitionID.data),timestamp=1)
      log('    Run/activity type: '+self.runType.data,timestamp=1)
      log('    Number of subfarms:'+str(self.nSubFarm.data),timestamp=1)
      log('    Subfarms:          '+str([i for i in self.subfarms.data]),timestamp=1)

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
  """ @class StreamingDescriptor
      This class describes the shared streaming control structure.
   
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
          dpv.push_back(self.dp2(self.name+'_'+n,'UsedBy'))
          dpv.back().data = partition
          dpv.push_back(self.dp2(self.name+'_'+n,'RunInfo'))
          dpv.back().data = runinfo_dp
          dpv.push_back(self.dp2(self.name+'_'+n,'Activity'))
          dpv.back().data = activity
          used.push_back(partition)
          farms.push_back(n)
          got = got + 1
        else:
          used.push_back(f)
      if got==nf:
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
          dpv.push_back(self.dp2(self.name+'_'+n,'UsedBy'))
          dpv.back().data = ''
          dpv.push_back(self.dp2(self.name+'_'+n,'RunInfo'))
          dpv.back().data = ''
          dpv.push_back(self.dp2(self.name+'_'+n,'Activity'))
          dpv.back().data = ''
        else:
          res.push_back(f)
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
    sys,act = self.setup.data.split(':')
    mgr = Online.PVSSSystems.controlsMgr(sys)
    activity = FarmActivity(mgr,act).load()
    if activity:
      tasks = []
      sfn   = self.subfarmName()
      nodes = self.processors()
      sys   = self.manager.name()
      part  = activity.activityName.data
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
    sfn = self.name
    sfDesc = SubFarmDescriptor(self.manager,sfn).load()
    if sfDesc:
      tasks = sfDesc.defineTasks()
      if tasks:
        fsm_manip = FSMmanip(sfDesc,'_FwFsmDevice',match='*')
        fsm_manip.setStartupInfoData({})
        slots = fsm_manip.collectTaskSlots()
        if slots:
          used_tasks = {}
          for t in tasks:
            items = t.split('/')
            if not used_tasks.has_key(items[0]): used_tasks[items[0]] = []
            used_tasks[items[0]].append(items)
          fsm_manip.disableTasks(slots)
          slots = fsm_manip.allocateProcesses(used_tasks,slots)
          if slots is None:
            return None
          if fsm_manip.commit(slots) is None:
            return None
          return self
        error('No task slots found of:'+sfn+' for partition:'+partition,timestamp=1)
        return None
      error('Failed to configure slots of:'+sfn+' for partition:'+partition,timestamp=1)
      return None
    error('Failed to access config info of subfarm:'+sfn+' for partition:'+partition,timestamp=1)
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
    FarmDescriptor.__init__(self,manager,'Trigger')
    #Online.DatapointLoader.DatapointLoader.__init__(self,manager,name)
    self.typeMgr = self.manager.typeMgr()
    setup = 'RECO'
    setup = 'ONLINE'
    if setup=='RECO':
      self.allocatePartition = self.allocRecoPartition
      self.getPartition      = self.getRecoPartition
      self.freePartition     = self.freeRecoPartition
      self.loadRunInfo       = self.loadRecoRunInfo
      self.runInfo_type      = self.typeMgr.type('RunFarmInfo')
      self.runinfos = Online.PVSS.DpVectorActor(self.manager)
      self.runinfos.lookupOriginal(self.name+'_Farm??.general.partName',self.runInfo_type)
    else:
      self.allocatePartition = self.allocateSlice
      self.getPartition      = self.getSlice
      self.freePartition     = self.freeSlice
      self.loadRunInfo       = self.loadRecoRunInfo
      self.sliceType         = self.typeMgr.type('FarmSlice')
      self.runInfo_type      = self.typeMgr.type('FarmRunInfo')
    
    self.writer   = self.manager.devWriter()
    self.fsm_typ  = self.typeMgr.type('_FwFsmDevice')
    subinfo_typ = self.typeMgr.type('FarmSubInfo')
    self.allsubfarms = Online.PVSS.DpVectorActor(self.manager)
    self.allsubfarms.lookupOriginal(self.name+'_*.UsedBy',subinfo_typ)

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
    infos.lookupOriginal(self.name+'_Farm??.general.partName',self.runInfo_type)
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
    return FarmRunInfo(manager,dp).load()
  
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
    pass

  # ===========================================================================
  def allocRecoPartition(self,rundp_name,partition):
    res = self.getPartition(rundp_name,partition)
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
    rdr = self.manager.devReader()
    rdr.add(self.allsubfarms.container)
    if rdr.execute():
      dpv   = Online.PVSS.DataPointVector()
      for i in self.allsubfarms.container:
        sys = self.manager.dpSystemName(i.name())
        elt = self.manager.dpElementName(i.name())
        dpv.push_back(self.dp2(farm_name+'|'+elt+'|'+elt,'mode.enabled'))
        if i.data==partition and action=='ENABLE':
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
  def allocate(self,rundp_name,partition):
    res = self.allocatePartition(rundp_name,partition)
    if res:
      name,dp = res
      run_info = self.loadRunInfo(self.manager,name)
      if run_info:
        numSF = run_info.nSubFarm.data
        activity = run_info.runType.data
        if FarmDescriptor.allocate(self,partition,numSF,activity,name):
          run_info.show()
          if self.configureFSM(name,partition,'ENABLE'):
            return 'SUCCESS'
          return self.error('Failed to configure subfarm FSMs for partitionn:'+partition,timestamp=1)
        return self.error('Failed to load run-type for partition:'+partition,timestamp=1)
      return self.error('Failed to allocate subfarms for partition:'+partition,timestamp=1)
    return self.error('Failed to access run info for partition:'+partition,timestamp=1)
  
  # ===========================================================================
  def free(self,rundp_name,partition):
    res = self.getPartition(rundp_name,partition)
    if res:
      name,context = res
      if FarmDescriptor.free(self,partition,name):
        if self.configureFSM(name,partition,'DISABLE'):
          self.freePartition(context,rundp_name,partition)
          return 'SUCCESS'
        return self.error('Failed to reset subfarm FSMs for partitionn:'+partition,timestamp=1)
      return self.error('Failed to free subfarms for partitionn:'+partition,timestamp=1)
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

  # ===========================================================================
  def configure(self,rundp_name, partition):
    "Default client callback on the command 'configure'"
    return self
  
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

if __name__ == "__main__":
  name = 'Trigger'
  #testActivity()
  #testAllocation(name)
  #testSubFarm(name)
  testConfigure(name)
  
