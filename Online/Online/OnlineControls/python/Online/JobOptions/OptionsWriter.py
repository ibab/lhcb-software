import socket, time
import Online.Utils
import Online.PVSS as PVSS
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.AllocatorControl as Control
import Online.JobOptions.JobOptions as JobOptions
import Online.Streaming.PartitionInfo as PartitionInfo
import Online.Streaming.StreamingDescriptor as StreamingDescriptor

log          = Online.Utils.log
error        = PVSS.error
warning      = Online.Utils.warning
DataPoint    = PVSS.DataPoint
fatal_banner = PVSS.fatal_banner

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
  def comment(self,name=''):
    return self.add('// '+name)
  # ===========================================================================
  def add(self,name,value=None,operator='='):
    "Add a new options item"
    if value is not None:
      v = str(value)
      if isinstance(value,str): v = '"'+v+'"'
      v = v.replace("'",'"').replace('[','{').replace(']','}')      
      s = '%s.%-24s %s %s;\n'%(self.object,name,operator,v)
      self.value = self.value + s
    elif len(name)>0:
      self.value = self.value + name + '\n'
    return self
  # ===========================================================================
  def append(self,name,value):
    "Append items to existing options"
    return self.add(name,value,'+=')
  
# =============================================================================
class PyOptions:
  # ===========================================================================
  def __init__(self,msg=''):
    "Object constructor"
    self.value = msg
    if len(self.value)>0: self.value = self.value+'\n'
  # ===========================================================================
  def comment(self,name=''):
    return self.add('# '+name)
  # ===========================================================================
  def add(self,name,value=None,operator='='):
    "Add a new options item"
    if value is not None:
      v = str(value)
      if isinstance(value,str): v = '"'+v+'"'
      v = v.replace("'",'"').replace('{','[').replace('}',']')      
      s = '%-24s %s %s\n'%(name,operator,v)
      self.value = self.value + s
    elif len(name)>0:
      self.value = self.value + name + '\n'
    return self
  # ===========================================================================
  def append(self,name,value):
    "Append items to existing options"
    return self.add(name,value,'+=')

# =============================================================================
class OptionsWriter(Control.AllocatorClient):
  """
  OptionsWriter class.
  Base class to writing job options to file

  @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, hosttype, name, info):
    "Object constructor."
    Control.AllocatorClient.__init__(self,manager,name)
    self.hostType = hosttype
    self._hostnames = {}
    self.run = None
    self.optionsMgr = None
    self.infoCreator = info
    self.optionsDir = Params.jobopts_optsdir
    self.dp_name = self.manager.name()+':'+name+'JobOptions'
    fatal_banner(['Test','Starting job options writer '+name,'Test'],timestamp=1)

  # ===========================================================================
  def connect(self,name):
    "Connect job options writer as a standalone device listening to commands."
    devMgr = self.optionsManager().deviceMgr()
    jo_dev = devMgr.manager().name()+':'+name
    #print 'Dev exists:',str(devMgr.exists(jo_dev)),jo_dev
    if not devMgr.exists(jo_dev):
      typ = devMgr.manager().typeMgr().type('JobOptionsControl')
      if not devMgr.createDevice(jo_dev,typ,1).release():
        error('Failed to create the datapoint:"'+jo_dev+'"',timestamp=1,type=PVSS.DP_NOT_EXISTENT)
      else:
        log('Created the datapoint:"'+jo_dev+'"',timestamp=1)
  # ===========================================================================
  def optionsManager(self):
    if self.optionsMgr is None:
      self.optionsMgr = Systems.controlsMgr(Params.jobopts_system_name)
    return self.optionsMgr

  # ===========================================================================
  def get(self,name):
    nam = DataPoint.original(self.dp_name+'.'+name)
    # log('Access datapoint:'+nam,timestamp=1)
    return DataPoint(self.manager,nam)

  # ===========================================================================
  def activityName(self):
    ##log('Activity name:'+self.run.name+' '+self.hostType+'_'+self.run.runType(),timestamp=1)
    return self.hostType+'_'+self.run.runType()

  # ===========================================================================
  def getStreamInfo(self,mgr,name):
    info = None
    res = StreamingDescriptor.findPartition(mgr,name,self.run.name)
    if res is not None:
      dp, nam, id = res
      info = PartitionInfo.PartitionInfo(mgr,nam).load()
    if info is None:
      fatal_banner('###\n###   Cannot access Stream Slice for partition:'+self.run.name+' ['+mgr.name()+']\n###',
                   timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return info
  
  # ===========================================================================
  def getTaskTypes(self, activity_name):
    activity = JobOptions.Activity(self.optionsManager(),activity_name)
    if activity.exists():
      return activity.taskNames()
    fatal_banner('The activity '+activity_name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None
    
  # ===========================================================================
  def getTasks(self, activity_name):
    tasks = []
    mgr = self.optionsManager()
    task_names = self.getTaskTypes(activity_name)
    for name in task_names:
      if len(name)>0:
        task = JobOptions.TaskType(mgr,name)
        if task.exists():
          tasks.append(task)
        else:
          fatal_banner('The task '+name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
          return None
    return tasks
    
  # ===========================================================================
  def writeOptions(self, opts, activity, task):
    return self.writeOptionsFile(self.run.name, task.name, opts.value)

  # ===========================================================================
  def writeOptionsFile(self, partition, name, opts, suffix='opts'):
    import os
    if self.optionsDir is not None:
      try:
        fd = self.optionsDir+'/'+partition
        fn = fd+'/'+name+'.'+suffix
        ####log('###   Writing options: '+fn,timestamp=1)
        try:
          os.stat(fd)
        except:
          os.mkdir(fd)
        desc = open(fn, 'w')
        print >>desc, opts
        desc.close()
      except Exception,X:
        error('Failed to write options for task:'+name+' ['+str(X)+']',timestamp=1,type=PVSS.FILEOPEN)
        return None
      return self
    error('Failed to write options for task:'+name+' [No Directory]',timestamp=1,type=PVSS.FILEOPEN)
    return None
  
  # ===========================================================================
  def writePythonFile(self, partition, name, subdir, opts):
    import os
    if self.optionsDir is not None:
      try:
        fd = self.optionsDir+'/'+partition
        if subdir is not None:
          fd = fd + '/' + str(subdir)
        fn = fd+'/'+name+'.py'
        ###log('###   Writing options: '+fn,timestamp=1)
        try:
          os.stat(fd)
        except:
          os.makedirs(fd)
        desc = open(fn, 'w')
        print >>desc, opts
        desc.close()
      except Exception,X:
        error('Failed to write options for task:'+name+' ['+str(X)+']',timestamp=1,type=PVSS.FILEOPEN)
        return None
      return self
    error('Failed to write options for task:'+name+' [No Directory]',timestamp=1,type=PVSS.FILEOPEN)
    return None
  
  # ===========================================================================
  def additionalOptions(self, activity, task):
    return None

  # ===========================================================================
  def configure(self, rundp_name, partition):
    try:
      log('Starting job options: Read RunInfo from '+rundp_name+' for '+partition,timestamp=1)
      self.boards = None
      self.run = self.infoCreator.create(rundp_name,partition).load()
      if self.run:
        return self._configure(partition)
    except Exception,X:
      print 'Exception:',X
    fatal_banner('Run Information for partition '+partition+' does not exist!',
                 timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # ===========================================================================
  def optHeader(self,partition):
    activity = self.activityName()
    run_type = self.run.runType()
    msg = 'Auto generated RunInfo options for partition:'+partition+\
        ' activity:'+run_type+'  '+time.ctime()
    return msg

  # ===========================================================================
  def _makeRunInfo(self,partition):
    opts = Options('//'+self.optHeader(partition))
    opts.object = 'OnlineRunInfo'
    rt = self.run.runType().lower()
    opts.add('//\n// ---------------- RunInfo:  ')
    opts.add('general.PartitionID',           self.run.partitionID())
    opts.add('general.PartitionName',         partition)
    opts.add('general.PartitionIDName',       '%04X'%self.run.partitionID())
    opts.add('general.RunType',               self.run.runType())
    opts.add('general.TAE',                   self.run.TAE())
    opts.add('HLTFarm.nSubFarms',             self.run.nSubFarm.data)
    opts.add('Storage.dataDirectory',         self.run.storageDir.data)
    opts.add('Storage.storeFlag',             self.run.storeFlag.data)
    opts.add('Storage.recvInfrastructure',    [i for i in self.run.rcvInfra.data])
    opts.add('Storage.streamInfrastructure',  [i for i in self.run.strInfra.data])
    opts.add('Storage.streamTypes',           [i for i in self.run.streams.data])
    opts.add('Storage.streamMultiplicity',    [i for i in self.run.strMult.data])
    opts.add('MonFarm.monFlag',               self.run.monFlag.data)
    opts.add('MonFarm.monTypes',              [i for i in self.run.monTypes.data])
    opts.add('MonFarm.monStreams',            [i for i in self.run.monStreams.data])
    opts.add('MonFarm.monMultiplicity',       [i for i in self.run.monMult.data])
    opts.add('MonFarm.monInfrastructure',     [i for i in self.run.monInfra.data])
    opts.add('MonFarm.relayInfrastructure',   [i for i in self.run.relayInfra.data])
    if partition != 'LHCb1' and partition != 'LHCb2' and partition != 'LHCbA':
      opts.add('SubDetectors.tell1List',        [i for i in self.run.tell1Boards.data])
    #print opts.value
    return self.writeOptionsFile(self.run.name, self.run.name+'_RunInfo', opts.value)
  
  # ===========================================================================
  def addGeneralInfo(self,partition,opts):
    opts.comment().comment('---------------- General partition parameters:    ')
    opts.add('PartitionID',    self.run.partitionID())
    opts.add('PartitionIDName','%04X'%self.run.partitionID())
    opts.add('PartitionName',  partition)
    opts.add('Activity',       self.run.runType())
    opts.add('TAE',            self.run.TAE())
    opts.add('OutputLevel',    self.run.outputLevel())
    opts.add('HltArchitecture',str(self.run.hltArchitecture()))
    opts.add('CalibArchitecture',str(self.run.calibArchitecture()))
    return opts

  # ===========================================================================
  def addTriggerInfo(self,opts):
    import math
    opts.comment('---------------- Trigger parameters:    ')
    if self.run.DeferHLT is not None:
      opts.add('DeferHLT',     self.run.DeferHLT.data)
    else:
      opts.add('DeferHLT',     0)

    if self.run.passThroughDelay is not None:
      opts.add('passThroughDelay',int(self.run.passThroughDelay.data))
    else:
      opts.add('passThroughDelay',0)

    if self.run.acceptRate() is None:
      print 'Rate is None!!!!!'
    opts.add('AcceptRate',     float('%.7f'%(self.run.acceptRate(),)))
    tck = self.run.TCK()
    if tck is not None:
      opts.add('InitialTCK',   str(hex(0xFFFFFFFF&int(tck))).strip())
    else:
      opts.comment('Note: No TCK information present for partition:'+str(partition))

    if self.run.mooreStartup is not None:
      opts.add('MooreStartupMode',self.run.mooreStartup.data)
    else: 
      opts.add('MooreStartupMode',   0)

    if self.run.condDBtag is not None and len(self.run.condDBtag.data)>0:
      opts.add('CondDBTag',   str(self.run.condDBtag.data))
      opts.add('condDBTag',   str(self.run.condDBtag.data))
    else:
      opts.add('CondDBTag',   'cond-20120831')
      opts.add('condDBTag',   'cond-20120831')

    if self.run.DDDBtag is not None and len(self.run.DDDBtag.data)>0:
      opts.add('DDDBTag',   str(self.run.DDDBtag.data))
    else:
      opts.add('DDDBTag',   'dddb-20120831')

    if self.run.lumiTrigger is not None:
      opts.add('LumiTrigger',   self.run.lumiTrigger.data)
    else: 
      opts.add('LumiTrigger',   0)

    if self.run.beamgasTrigger is not None:
      opts.add('BeamGasTrigger',self.run.beamgasTrigger.data)
    else: 
      opts.add('BeamGasTrigger',   0)

    pars = []
    if self.run.lumiPars is not None:
      num = len(self.run.lumiPars.data)
      for i in xrange(num):
        txt = '%7.5e'%(float(self.run.lumiPars.data[i]),)
        pars.append(float(txt)) # Do proper rounding ....
    opts.add('LumiPars',   pars)
      
    if self.run.L0Type is not None:
      opts.add('L0Type',   str(self.run.L0Type.data))
    if self.run.hltType is not None:
      opts.add('HLTType',   str(self.run.hltType.data))
    if self.run.mooreOnlineVersion is not None:
      opts.add('MooreOnlineVersion',str(self.run.mooreOnlineVersion.data))
    if self.run.mooreVersion is not None:
      opts.add('MooreVersion',   str(self.run.mooreVersion.data))
    if self.run.onlineVersion is not None:
      opts.add('OnlineVersion',   str(self.run.onlineVersion.data))
    if self.run.dataflowVersion is not None:
      opts.add('DataflowVersion',   str(self.run.dataflowVersion.data))
    if self.run.gaudiVersion is not None:
      opts.add('GaudiVersion',   str(self.run.gaudiVersion.data))

    if hasattr(self.run,'recStartup') and self.run.recStartup is not None:
      opts.add('RecoStartupMode',self.run.recStartup.data)
      opts.add('OnlineBrunelVersion',self.run.recVersion.data)
    else: 
      opts.add('RecoStartupMode',   1)
      opts.add('OnlineBrunelVersion', 'OnlineBrunel')

    if self.run.triggerConditionsMap is not None:
      opts.add('ConditionsMapping',self.run.triggerConditionsMap.data)
    else: 
      opts.add('ConditionsMapping',"None")

    return opts

  # ===========================================================================
  def _getOnlineEnv(self,partition):
    opts = Options('//'+self.optHeader(partition))
    opts = self.addGeneralInfo(partition,opts)
    opts.add('MessageSvc.OutputLevel     = '+str(self.run.outputLevel())+';')
    return self.addTriggerInfo(opts)

  # ===========================================================================
  def _writeOnlineEnv(self,partition):
    opts = self._getOnlineEnv(partition)
    fname = partition+'_OnlineEnv'
    if self.writeOptionsFile(partition, fname, opts.value) is None:
      return None
    return self.run

  # ===========================================================================
  def _writePyOnlineEnv(self,partition,fname='OnlineEnv',subdir=None):
    opts = PyOptions('#'+self.optHeader(partition))
    opts = self.addGeneralInfo(partition,opts)
    opts = self.addTriggerInfo(opts)
    if self.run.deferredRuns is not None and len(self.run.deferredRuns.data)>0:
      opts.add('DeferredRuns', ["%s"%i for i in self.run.deferredRuns.data])
    else:
      opts.add('DeferredRuns', ["*"])

    if self.writePythonFile(partition, fname+'Base', subdir, opts.value) is None:
      return None
    opts = PyOptions('#'+self.optHeader(partition))
    if partition == 'LHCb2':
      opts.comment().add('import OnlineNodeEnv')
      opts.add('OnlineNodeEnv.load_node_info()')
    opts.comment().add('from '+fname+'Base import *')
    opts.comment().add('from OnlineConfig import *')
    if self.writePythonFile(partition, fname, subdir, opts.value) is None:
      return None
    return self.run

  
  # ===========================================================================
  def _tell1Boards(self,partition='Unknown'):
    if ( self.boards is None ):
      boards = []
      log('Resolving Tell1 names for partition:'+partition,timestamp=1)
      for b in self.run.tell1Boards.data:
        itms = b.split(':')
        try:
          board = itms[0]
          if len(itms)>1: board = itms[1]
          boards.append((self._gethost(board+'-d1'),board,));
        except Exception,X:
          fatal_banner(['Failed to retrieve TELL1 specs for '+str(b),'Error '+str(X)],timestamp=1)
          self.boards = None
          return self.boards
      self.boards = boards
    return self.boards

  # ===========================================================================
  def _getTell1Boards(self,partition,hdr=''):
    h = hdr
    if len(h)>0: h = h+'\n'
    opts = Options(h+'// ---------------- Tell1 board information:')
    opts.add('OnlineEnv.Tell1Boards         = {')
    err = None
    num = 0;
    boards = self._tell1Boards(partition)
    if boards:
      for b in boards:
        opts.add('  "%s", "%s", "",'%b)
        num = num + 1
      if num > 0:
        opts.value = opts.value[:-2] + '\n};\n'
      else:
        opts.value = opts.value + '\n};\n'
    else:
      return None

    odin = 'Unknown'
    try:
      # If OdinData or OdinIP is explicitly set to None
      # No odin is in the readout
      odin  = self.run.odinData.data
      host  = odin
      if odin != "None":
        host = self._gethost(odin)
      opts.append('Tell1Boards',[host,odin,'ODIN'])

      odin  = self.run.odinRequest.data
      host  = odin
      if odin != "None":
        host = self._gethost(odin)

      opts.add('ODIN_Name',odin)
      opts.add('ODIN_IP',  host)
    except Exception,X:
      fatal_banner(['Failed to retrieve ODIN ip address for '+str(odin),
                    'Error [IGNORED for NOW] '+str(X)],timestamp=1)
      err = self
    if err:
      return None
    return opts
  
  # ===========================================================================
  def _writeTell1Boards(self,partition):
    opts = self._getTell1Boards(partition)
    opts.value = '//'+self.optHeader(partition)+'\n'+opts.value
    fname = partition+'_Tell1Boards'
    if self.writeOptionsFile(partition, fname, opts.value) is None:
      return None
    self._makeRunInfo(partition)
    return self.run

  # ===========================================================================
  def _gethost(self,ip):
    try:
      if self._hostnames.has_key(ip):
        return self._hostnames[ip]
      h = socket.gethostbyname(ip)
      self._hostnames[ip] = h
      return h
    except:
      return str(None) ## str(ip)
    
  # ===========================================================================
  def _getInjectorInfo(self,partition):
    b = ''
    try:
      opts = Options().comment('---------------- Injector information:')
      #opts.comment('  RUN_Info.Injector.FullPartId:')
      #opts.add('Injector_FullPartId',      self.run.inj_fullPartId.data)
      opts.comment('  RUN_Info.Injector.MEPRequestAddr:')
      b = self.run.inj_mepAddress.data
      opts.add('Injector_odinIF_Name',     b)
      opts.add('Injector_odinIF_IP',       self._gethost(b))
      opts.comment('  RUN_Info.Injector.DataAddr:')
      b = self.run.inj_dataAddress.data
      opts.add('Injector_farmIF_Name',     b)
      opts.add('Injector_farmIF_IP',       self._gethost(b))
      opts.comment('  RUN_Info.Injector.OdinMEPRequest:')
      b = self.run.inj_odinRequest.data
      opts.add('Injector_ODIN_REQ_Name',   b)
      opts.add('Injector_ODIN_REQ_IP',     self._gethost(b))
      opts.comment('  RUN_Info.Injector.OdinData:')
      b = self.run.inj_odinData.data
      opts.add('Injector_ODIN_DATA_Name',  b)
      opts.add('Injector_ODIN_DATA_IP',    self._gethost(b))

      # opts.add('\n// ---------------- Tell1 borard information: (RUN_Info.Injector.FullTell1List)\nOnlineEnv.FullTell1Boards = {   ')  # Trailing spaces mandatory!
      # for b in self.run.inj_tell1Boards.data:
      #   opts.add('  "%s", "%s", "",'%(self._gethost(b+'-d1'),b+'-d1'))
      # opts.value = opts.value[:-2] + '\n};\n'
      opts.add('\n// ---------------- Injector node list: (RUN_Info.Injector.nodeList)\nOnlineEnv.Injector_Nodes = {   ')  # Trailing spaces mandatory!
      for n in self.run.inj_nodeList.data:
        opts.add('  "%s",'%n)
      opts.value = opts.value[:-2] + '\n};\n'
      opts.add('\n// ---------------- Input file list: (RUN_Info.Injector.FileList)\nOnlineEnv.Injector_Files = {   ')  # Trailing spaces mandatory!
      for f in self.run.inj_fileList.data:
        opts.add('  "DATA=\'PFN:root:file://%s\' IgnoreChecksum=\'YES\' SVC=\'LHCb::MDFSelector\'",'%f)
      opts.value = opts.value[:-2] + '\n};\n'

      opts.add('Injector_LoopFiles',self.run.inj_loopOnFiles.data)
      opts.add('OnlineEnv.Injector_Readers = {   ')
      for f in self.run.inj_readerNames.data:
        opts.add('  "%s",'%f)
      opts.value = opts.value[:-2] + '\n};\n'
      for n in self.run.inj_readers.keys():
        r = self.run.inj_readers[n]
        opts.add('// ---------------- Data for '+n)
        opts.add(n+'_Name',r['Name'].data)
        opts.add(n+'_Directory',r['DataDirectory'].data)
        opts.add('OnlineEnv.'+n+'_Files = {   ')  # Trailing spaces mandatory!
        for f in r['FileList'].data:
          opts.add('  "%s",'%f)
        opts.value = opts.value[:-2] + '\n};\n'

      return opts

    except Exception,X:
      fatal_banner(['Failed to retrieve specs '+str(b),
                    'Error '+str(X)],timestamp=1)
    return None
  
  # ===========================================================================
  def _configure(self, partition):
    activity = self.activityName()
    tasks = self.getTasks(activity)
    env = self._getOnlineEnv(partition).value
    if tasks is not None:
      for task in tasks:
        opts = Options('//'+self.optHeader(partition)+'\n#include "$PREAMBLE_OPTS"')
        if task.defaults.data:
          rt = self.run.runType().lower()
          opts.value = opts.value + env
          opts.add('HostType',       self.hostType)
          opts.add('HostTypes',     [self.hostType])
          opts.add('TaskType',       task.name)
          oloc = self.run.storageDir.data+'/lhcb/data/'+time.strftime('%Y')+'/RAW/'+partition+'/'+rt
          opts.add('OutputLocation', oloc)
        else: opts.comment('---------------- NO partition information')
        addOpts = self.additionalOptions(activity,task)
        if addOpts:
          opts.comment().comment('---------------- Additional specific options:')
          for nam,opt in addOpts.items(): opts.add(nam,opt)
        else: opts.comment('---------------- NO additional options')
        if task.tell1s.data:
          opts.value = opts.value+self._getTell1Boards(partition).value
        else: opts.comment('---------------- NO tell1 board information') 
        if task.options.data:
          opts.comment().comment('---------------- Task specific information:')
          opts.add(task.options.data)
        else: opts.comment('---------------- NO task specific information')
        if task.defaults.data:
          if task.name[:3] == 'WRT':
            opts.add('OnlineEnv.OutputLevel   = 3;')
            opts.add('MessageSvc.OutputLevel   = 3;')
          else:
            opts.add('MessageSvc.OutputLevel   = @OnlineEnv.OutputLevel;')
          opts.add('MessageSvc.fifoPath        = @OnlineEnv.LogFifoName;')
        if not self.writeOptions(opts,activity,task):
          return None
      if len(tasks)==0:
        log('No tasks found for activity:'+activity,timestamp=1)
      return self.run
    error('Cannot retrieve tasks for activity:'+activity)
    return None

# =============================================================================
class HLTOptionsWriter(OptionsWriter):
  """ Specialized options writer for HLT control nodes.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    "Object constructor."
    OptionsWriter.__init__(self, manager, 'HLT', name, info)
    devMgr = self.manager.deviceMgr()
    typ = self.manager.typeMgr().type('JobOptionsControl')
    self.storageMgr = Systems.controlsMgr(Params.storage_system_name)
    log(name+': Uses the managers '+self.manager.name()+' and '+self.storageMgr.name(),timestamp=1)
    for i in xrange(16):
      nam = self.manager.name()+':'+name+'Writer_%02X'%(i,)
      if not DataPoint.exists(nam):
        if not devMgr.createDevice(nam,typ,1).release():
          error('Failed to create the datapoint:"'+nam+'"',timestamp=1,type=PVSS.DP_NOT_EXISTENT)
        else:
          log('Created the datapoint:"'+nam+'"',timestamp=1)

  # ===========================================================================
  def _configure(self, partition):
    if self.run:
      #log('(*****) Starting to write HLT TEXT job options',timestamp=1)
      res = self._writeHLTOpts(partition)
      #log('(*****) Finished to write HLT TEXT job options',timestamp=1)
      if res:
        log('Starting to write HLT PYTHON job options',timestamp=1)
        self._writePyOnlineEnv(partition=partition,fname='OnlineEnv',subdir='HLT')
        log('Finished to write HLT PYTHON job environment options',timestamp=1)
        return self._writePyHLTOpts(partition)
        #log('(*****) Finished to write HLT PYTHON job options',timestamp=1)
        #return res
    return None

  # ===========================================================================
  def _writeHLTOpts(self,partition):
    farms = self.run.subFarms.data
    num = len(farms)
    farm_names = []
    for i in xrange(num):
      farm = farms[i]
      farm_names.append(farm)
    hdr='//'+self.optHeader(partition)+'\n//  Number of subfarms:'+str(len(farms))
    #
    # 
    #  If the storage is in use (storeFlag>0) we have to check if the slices match!
    #
    if self.run.storeFlag.data > 0:
      info = self.getStreamInfo(self.storageMgr,'Storage')
      #
      # Check availability of the storage streaming information
      #
      if info is None:
        fatal_banner('No storage streaming information present!')
        return None
      #
      # Check if slice numbers match:
      #
      slots = info.recvSlices()
      if len(farms) != len(slots):
        fatal_banner(['Severe mismatch: Number of subfarms:'+str(len(farms))+' Number of recv tasks:'+str(len(slots)),
                      'Information seen by different projects is inconsistent',
                      'Check if ALLOCATE worked properly',
                      'If error persists try to DEALLOCATE and then ALLOCATE again'])
        return None
      for i in xrange(num):
        farm = farms[i]
        node = slots[i].split(':')[0]
        name = '%s_%s_SF%02d_HLT'%(partition,node,i)
        opts = hdr+'\n//\n'+\
            '// ---------------- Data sending information for sub-farm:'+farm+'\n'+\
            'OnlineEnv.Target  = "'+node+'-d1::'+name+'";\n'+\
            'OnlineEnv.Target0 = { "'+node+'-d1::'+name+'" };\n'+\
            'OnlineEnv.Target1 = { "'+node+'-d1" };\n'+\
            'OnlineEnv.Target2 = { "'+name+'" };'
        fname = partition+'_'+farm+'_HLT'
        if self.writeOptionsFile(partition, fname, opts) is None:
          return None
        log('      --> Farm:'+farm+' sends to slot:'+slots[i]+', Task:'+name+', opts:'+fname,timestamp=1)

    opts = self._getOnlineEnv(partition)
    opts.comment('---------------- HLT patrameters:   ')
    opts.add('SubFarms',       farm_names)

    #log(' HLT OPTS: Adding deferred runs.......',timestamp=1,type=PVSS.ILLEGAL_VALUE)
    # For the HLT info add the list of deferred runs
    if self.run.deferredRuns is not None and len(self.run.deferredRuns.data)>0:
      opts.add('DeferredRuns', ["%s"%i for i in self.run.deferredRuns.data])
    else:
      opts.add('DeferredRuns', ["*"])
      
    if self.writeOptionsFile(partition, partition+'_Info', opts.value) is None:
      return None

    if partition != 'LHCb1' and partition != 'LHCb2' and partition != 'LHCbA':
      opts = self._getTell1Boards(partition,hdr)
      if self.writeOptionsFile(partition, partition+'_Tell1Boards', opts.value) is None:
        return None
    self._makeRunInfo(partition)
    return self.run

  # ===========================================================================
  def _writePyHLTOpts(self,partition):
    opt_header = self.optHeader(partition)
    farms = self.run.subFarms.data
    num = len(farms)
    farm_names = []
    for i in xrange(num):
      farm = farms[i]
      farm_names.append(farm)
    hdr='//'+self.optHeader(partition)+'\n//  Number of subfarms:'+str(len(farms))
    run_type = self.run.runType()
    #
    # 
    #  If the storage is in use (storeFlag>0) we have to check if the slices match!
    #
    if self.run.storeFlag.data > 0:
      info = self.getStreamInfo(self.storageMgr,'Storage')
      #
      # Check availability of the storage streaming information
      #
      if info is None:
        fatal_banner('No storage streaming information present!')
        return None
      #
      # Check if slice numbers match:
      #
      slots = info.recvSlices()
      if len(farms) != len(slots):
        fatal_banner(['Severe mismatch: Number of subfarms:'+str(len(farms))+' Number of recv tasks:'+str(len(slots)),
                      'Information seen by different projects is inconsistent',
                      'Check if ALLOCATE worked properly',
                      'If error persists try to DEALLOCATE and then ALLOCATE again'])
        return None

      for i in xrange(num):
        farm = farms[i]
        node = slots[i].split(':')[0]
        name = '%s_%s_SF%02d_HLT'%(partition,node,i)
        opts = '#'+opt_header+'\n#\n'+\
            '# ---------------- Data sending information for sub-farm:'+farm+'\n'+\
            'Target  = "'+node+'-d1::'+name+'"\n'+\
            'Target0 = [ "'+node+'-d1::'+name+'" ]\n'+\
            'Target1 = [ "'+node+'-d1" ]\n'+\
            'Target2 = [ "'+name+'" ]'
        fname = partition+'_'+farm+'_HLT'
        if self.writePythonFile(partition, fname, subdir='HLT', opts=opts) is None:
          return None
        #log('      --> Farm:'+farm+' sends to slot:'+slots[i]+', Task:'+name+', pyOpts:HLT/'+fname,timestamp=1)
        
    opts = PyOptions('#'+opt_header)
    opts = self.addGeneralInfo(partition,opts)
    opts.comment('---------------- HLT patrameters:   ')
    opts.add(    'SubFarms', farm_names)

    # For the HLT info add the list of deferred runs -- NOT NEEDED FOR THE TIME BEING!
    if self.run.deferredRuns is not None and len(self.run.deferredRuns.data)>0:
      opts.add('DeferredRuns', ["%s"%i for i in self.run.deferredRuns.data])
    else:
      opts.add('DeferredRuns', ["*"])

    opts = self.addTriggerInfo(opts)
    if self.writePythonFile(partition, partition+'_Info', subdir='HLT', opts=opts.value) is None:
      return None

    # Do not write Tell1 board content for LHCb1 (HLT reprocessing)
    if partition == 'LHCb1' or partition == 'LHCb2' or partition == 'LHCbA':
      self._makeRunInfo(partition)
      return self.run
        
    opts = Options('#'+opt_header)
    opts.comment().comment('---------------- Tell1 board information:  ')
    opts.add('Tell1Boards         = [')
    err = None
    num = 0;
    print 'New version of HLT options generator....'
    boards = self._tell1Boards(partition)
    if boards:
      for b in boards:
        opts.add('  "%s", "%s", "",'%b)
        num = num + 1
      if num > 0:
        opts.value = opts.value[:-2] + '\n]\n'
      else:
        opts.value = opts.value + '\n]\n'
    else:
      return None

    odin = 'Unknown'
    try:
      # If OdinData or OdinIP is explicitly set to None
      # No odin is in the readout
      odin  = self.run.odinData.data
      host  = odin
      if odin != "None":
        host = self._gethost(odin)
      opts.append('Tell1Boards',[host,odin,'ODIN'])

      odin  = self.run.odinRequest.data
      host  = odin
      if odin != "None":
        host = self._gethost(odin)

      opts.add('ODIN_Name',odin)
      opts.add('ODIN_IP',  host)
    except Exception,X:
      fatal_banner(['Failed to retrieve ODIN ip address for '+str(odin),
                    'Error [IGNORED for NOW] '+str(X)],timestamp=1)
      err = self
    if err:
      return None
    fname = partition+'_Tell1Boards'
    if self.writePythonFile(partition, fname, subdir='HLT', opts=opts.value) is None:
      return None
    self._makeRunInfo(partition)
    return self.run

#==============================================================================
class InjectorOptionsWriter(OptionsWriter):
  """ Specialized options writer for Injector control node.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    "Object constructor."
    OptionsWriter.__init__(self, manager, '', name, info)
    nam = 'INJJobOptions'
    self.optionsMgr = manager
    self.dp_name = self.manager.name()+':'+nam
    self.connect(nam)

  # ===========================================================================
  def _configure(self, partition):
    opts = self._getOnlineEnv(partition)
    b = None
    try:
      opts.value = opts.value + '\n' + self._getTell1Boards(partition).value
      opts.value = opts.value + '\n' + self._getInjectorInfo(partition).value
    except Exception,X:
      error('Failed to write injector options:'+str(X),timestamp=1)
      return None
    if self.writeOptionsFile(partition, 'Injector_Info', opts.value) is None:
      return None
    return self.run

#==============================================================================
class StreamingOptionsWriter(OptionsWriter):
  """ Specialized options writer for the storage control node.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, type, name, info, streamMgr, layers):
    "Object constructor."
    OptionsWriter.__init__(self, manager, type, name, info)
    self.streamMgr = streamMgr
    self.streamingLayers = layers

  # ===========================================================================
  def additionalOptions(self, activity, task):
    return {'HostName':       '@HostName@',
            'ProcessName':    '@ProcessName@',
            'ProcessNickName':'@ProcessNickName@',
            'ProcessType':    '@ProcessType@',
            'ProcessClass':   '@ProcessClass@',
            'DimDnsNode':     '@DimDnsNode@',
            'SystemName':     '@SystemName@',
            'LogFifoName':    '@LogFifoName@'
            }

  # ===========================================================================
  def getTaskTypes(self, activity_name):
    tasks = {}
    for i in self.taskList:
      typ = i.split('/')[3]
      tasks[typ] = 1
    return tasks.keys()

  # ===========================================================================
  def writeOptions(self, opts, activity, task):
    done = 0
    for i in self.taskList:
      it = i.split('/')
      ####print 'writeOptions',it
      items = it[:-1]
      
      eval_item = eval(it[-1])
      if task.name == items[3]:
        done = done+1
        sys_nam = items[6].upper()
        slice_nam = items[7]
        options = opts.value
        options = options.replace('@HostName@',items[0])
        options = options.replace('@ProcessName@',items[1])
        options = options.replace('@ProcessNickName@',items[2])
        options = options.replace('@ProcessType@',items[3])
        options = options.replace('@ProcessClass@',items[4])
        options = options.replace('@DimDnsNode@',items[5].upper())
        options = options.replace('@SystemName@',sys_nam)
        options = options.replace('@LogFifoName@','/tmp/'+slice_nam+'.fifo')
        if isinstance(eval_item,list) or isinstance(eval_item,tuple):
          s = ''
          s0 = ''
          s1 = ''
          prefix = ''
          tot_len = len(eval_item)
          if tot_len>1: prefix = '\n     '
          ##print eval_item,tot_len
          for q in xrange(tot_len):
            val = eval_item[q]
            if isinstance(val,tuple) or isinstance(val,list):
              if q > 0: s = s+','+prefix
              elif len(prefix): s = prefix
              if len(val)==2:
                if q > 0:
                  s0 = s0 + ','+prefix
                  s1 = s1 + ','+prefix
                elif len(prefix):
                  s = s0 = s1 = prefix
                s = s + '"'+str(val[0])+'::'+str(val[1])+'"'
                s0 = s0 + '"'+str(val[0])+'"'
                s1 = s1 + '"'+str(val[1])+'"'
              if len(val)==1:
                s = s + '"'+str(val[0])+'"'
            else:
              s = s + '"'+str(val)+'"'
          if len(eval_item)>1:
            s = '%-26s = {%s%s};\n'%('OnlineEnv.Opt0',s,prefix)
          else:
            tmp = s
            s = '%-26s = {%s};\n%-26s = %s;\n'%('OnlineEnv.Opt0s',tmp,'OnlineEnv.Opt0',tmp)
            try:
              # print 'Stream identifier:',tmp,tmp.rfind('_')+1
              if tmp.rfind('_')>0:
                s = s + '%-26s = %d;\n'%('OnlineEnv.StreamID',int(tmp[tmp.rfind('_')+1:-1]))
            except Exception,X:
              pass
              #print str(X)
          if len(s0)>0:
            if len(eval_item)==1: s = s + '%-26s = %s;\n%-26s = {%s};\n'%('OnlineEnv.Opt1',s0,'OnlineEnv.Opt1s',s0)
            else:          s = s + '%-26s = {%s%s};\n'%('OnlineEnv.Opt1',s0,prefix)
          if len(s1)>0:
            if len(eval_item)==1: s = s + '%-26s = %s;\n%-26s = {%s};\n'%('OnlineEnv.Opt2',s1,'OnlineEnv.Opt2s',s1)
            else:          s = s + '%-26s = {%s%s};\n'%('OnlineEnv.Opt2',s1,prefix)
          options = options + s
        else:
          options = options+'OnlineEnv.Opt0 = "'+str(eval_item)+'";\n'
        qq = str(eval_item).replace("'",'"').replace('[','{').replace(']','}').replace(')','}').replace('(','{').replace(',}','}')
        if qq.find('},')>0: qq = ('{\n    '+qq[1:-1]+'\n    }').replace('},','},\n   ')
        options = options+'%-26s = %s;\n'%('OnlineEnv.Options',qq)
        qq = str(items).replace("'",'"').replace('[','{').replace(']','}').replace(')','}').replace('(','{').replace(',}','}')
        if qq.find(',')>0: qq = ('{\n    '+qq[1:-1]+'\n    }').replace(',',',\n   ')
        options = options+'%-26s = %s;\n'%('OnlineEnv.Items',qq)
        if self.writeOptionsFile(self.run.name, items[1], options) is None:
          done = 0

    if done > 0: return self
    fatal_banner('###\n###   UNKNOWN task requires options: '+task.name+' type:'+self.hostType+'\n###',
                 timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # ===========================================================================
  def _configure(self, partition):
    self.taskList = None
    self.streamInfo = self.getStreamInfo(self.streamMgr,self.name)
    if self.streamInfo:
      result = self
      info = self.streamInfo
      #print info.tasks[PartitionInfo.RECVINFRA].name(),len(info.tasks[PartitionInfo.RECVINFRA].data)
      for layer in self.streamingLayers:
        self.hostType  = layer
        self.taskList  = self.setupLayer(layer)
        res = OptionsWriter._configure(self,partition)
        if res is None:
          result = None
      return result
    fatal_banner('Failed to access the Storage info:'+self.streamMgr.name()+' '+self.name)
    return None
  
# =============================================================================
class StorageOptionsWriter(StreamingOptionsWriter):
  """ Specialized options writer for the storage control node.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    streamMgr = Systems.controlsMgr(Params.storage_system_name)
    StreamingOptionsWriter.__init__(self, manager, 'RECV', name, info, streamMgr, ['RECV','STREAM'])
  # ===========================================================================
  def setupLayer(self, layer):
    i = self.streamInfo
    if layer == self.streamingLayers[0]:
      return _addTasks([i.recvSenders(),i.recvReceivers(),i.recvInfrastructure()])
    elif layer == self.streamingLayers[1]:
      return _addTasks([i.streamReceivers(),i.streamSenders(),i.streamInfrastructure()])
    return []
  # ===========================================================================
  def _configure(self, partition):
    res=StreamingOptionsWriter._configure(self,partition)
    #self._writePyOnlineEnv(partition,fname='StorageEnv')
    self._writePyOnlineEnv(partition,fname='OnlineEnv',subdir='STORAGE')
    return res
      
# =============================================================================
class MonitoringOptionsWriter(StreamingOptionsWriter):
  """ Specialized options writer for the monitoring controls node.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    streamMgr = Systems.controlsMgr(Params.monitor_system_name)
    StreamingOptionsWriter.__init__(self, manager, 'MONRELAY', name, info, streamMgr, ['MONRELAY','MONI'])
  # ===========================================================================
  def setupLayer(self, layer):
    i = self.streamInfo
    if layer == self.streamingLayers[0]:
      return _addTasks([i.dataSources(),i.recvSenders(),i.recvReceivers(),i.recvInfrastructure()])
    elif layer == self.streamingLayers[1]:
      return _addTasks([i.streamReceivers(),i.streamSenders(),i.streamInfrastructure()])
    return []
  # ===========================================================================
  def _writeTaskList(self, partition,fname):
    opts = ''
    # This gives back ALL tasks: 
    # for i in self.taskList:
    #
    # We only use the 'true' monitoring tasks
    for i in self.streamInfo.streamSenders():
      itm = i.split('/')
      opts = opts + itm[1] + '\n'
    self.writeOptionsFile(self.run.name, fname, opts[:-1], suffix='txt')
    return self
  # ===========================================================================
  def _configure(self, partition):
    res=StreamingOptionsWriter._configure(self,partition)
    self._writePyOnlineEnv(partition,fname='MonitoringEnv')
    self._writePyOnlineEnv(partition,fname='OnlineEnv',subdir='MONITORING')
    self._writeTaskList(partition,fname='MonitoringTasks')
    return res

# =============================================================================
class RecoOptionsWriter(StreamingOptionsWriter):
  """ Specialized options writer for the prompt reconstruction control node.
  
      @author  M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    streamMgr = Systems.controlsMgr(Params.monitor_system_name)
    StreamingOptionsWriter.__init__(self, manager, 'MONRELAY', name, info, streamMgr, ['MONRELAY','MONI'])
  # ===========================================================================
  def setupLayer(self, layer):
    i = self.streamInfo
    if layer == self.streamingLayers[0]:
      return _addTasks([i.dataSources(),i.recvInfrastructure(),i.recvSenders(),i.recvReceivers()])
    elif layer == self.streamingLayers[1]:
      return _addTasks([i.streamInfrastructure(),i.streamSenders()])
    return []
  # ===========================================================================
  def _configure(self, partition):
    res=StreamingOptionsWriter._configure(self,partition)
    self._writeOnlineEnv(partition)
    #self._writePyOnlineEnv(partition,fname='ReconstructionEnv')
    self._writePyOnlineEnv(partition,fname='OnlineEnv',subdir='RECONSTRUCTION')
    return res
