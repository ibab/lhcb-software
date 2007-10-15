import socket
import Online.Utils
import Online.PVSS as PVSS
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.AllocatorControl as Control
import Online.JobOptions.JobOptions as JobOptions
import Online.Streaming.PartitionInfo as PartitionInfo
import Online.Streaming.StreamingDescriptor as StreamingDescriptor

log       = Online.Utils.log
error     = PVSS.error
warning   = Online.Utils.warning
DataPoint = PVSS.DataPoint

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
    if len(self.value)>0: self.value = self.value+'\n'
  # ===========================================================================
  def add(self,name,value=None):
    "Add a new options item"
    if value:
      v = str(value)
      if isinstance(value,str): v = '"'+v+'"'
      s = 'OnlineEnv.%-16s = %s;\n'%(name,v.replace("'",'"').replace('[','{').replace(']','}'))
      self.value = self.value + s
    elif len(name)>0:
      self.value = self.value + name + '\n'
    return self

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
    self.run = None
    self.optionsMgr = None
    self.infoCreator = info
    self.optionsDir = Params.jobopts_optsdir
    self.dp_name = self.manager.name()+':'+name+'JobOptions'
    devMgr = self.manager.deviceMgr()
    if not devMgr.exists(self.dp_name):
      typ = self.manager.typeMgr().type('JobOptionsControl')
      if not devMgr.createDevice(self.dp_name,typ,1).release():
        error('Failed to create the datapoint:"'+self.dp_name+'"',timestamp=1,type=PVSS.DP_NOT_EXISTENT)
      else:
        log('Created the datapoint:"'+self.dp_name+'"',timestamp=1)

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
    log('Activity name:'+self.run.name+' '+self.hostType+'_'+self.run.runType(),timestamp=1)
    return self.hostType+'_'+self.run.runType()

  # ===========================================================================
  def getStreamInfo(self,mgr,name):
    info = None
    res = StreamingDescriptor.findPartition(mgr,name,self.run.name)
    if res is not None:
      dp, nam, id = res
      info = PartitionInfo.PartitionInfo(mgr,nam).load()
    if info is None:
      error('###\n###   Cannot access Stream Slice for partition:'+self.run.name+' ['+mgr.name()+']\n###',
            timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return info
  
  # ===========================================================================
  def getTaskTypes(self, activity_name):
    activity = JobOptions.Activity(self.optionsManager(),activity_name)
    if activity.exists():
      return activity.taskNames()
    error('The activity '+activity_name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
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
          error('The task '+name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
          return None
    return tasks
    
  # ===========================================================================
  def writeOptions(self, opts, activity, task):
    return self.writeOptionsFile(self.run.name, task.name, opts.value)

  # ===========================================================================
  def writeOptionsFile(self, partition, name, opts):
    import os
    if self.optionsDir is not None:
      try:
        fd = self.optionsDir+'/'+partition
        fn = fd+'/'+name+'.opts'
        log('###   Writing options: '+fn,timestamp=1)
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
  def additionalOptions(self, activity, task):
    return None

  # ===========================================================================
  def configure(self, rundp_name, partition):
    self.run = self.infoCreator.create(rundp_name,partition).load()
    if self.run:
      return self._configure(partition)
    error('Run Information for partition '+partition+' does not exist!',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # ===========================================================================
  def _configure(self, partition):
    activity = self.activityName()
    tasks = self.getTasks(activity)
    if tasks:
      run_type = self.run.runType()
      for task in tasks:
        opts = Options('//  Auto generated options for partition:'+partition+\
               ' activity:'+run_type+' task:'+task.name+'\n' +\
               '#include "$PREAMBLE_OPTS"')
        if task.defaults.data:
          opts.add('//\n// ---------------- General partition information:  ')
          opts.add('PartitionID',    self.run.partitionID())
          opts.add('PartitionName',  partition)
          opts.add('PartitionIDName','%04X'%self.run.partitionID())
          opts.add('Activity',       run_type);
          opts.add('HostType',       self.hostType)
          opts.add('HostTypes',     [self.hostType])
          opts.add('TaskType',       task.name)
        else: opts.add('// ---------------- NO partition information')
        addOpts = self.additionalOptions(activity,task)
        if addOpts:
          opts.add('//\n// ---------------- Additional specific options:')
          for nam,opt in addOpts.items(): opts.add(nam,opt)
        else: opts.add('// ---------------- NO additional options')
        if task.tell1s.data:
          opts.add('//\n// ---------------- Tell1 board information:')
          opts.add('OnlineEnv.Tell1Boards         = {')
          err = None
          for b in self.run.tell1Boards.data:
            itms = b.split(':')
            try:
              board = itms[0]
              if len(itms)>1: board = itms[1]
              ip = socket.gethostbyname(board+"-d1.data");
              opts.add('  "%s", "%s", "",'%(ip,board))
            except Exception,X:
              error('Failed to retrieve TELL1 specs for '+str(b))
              err = self
          opts.value = opts.value[:-2] + '\n};\n'
          if err:
            return None
        else: opts.add('// ---------------- NO tell1 board information') 
        if task.options.data:
          opts.add('//\n// ---------------- Task specific information:')
          opts.add(task.options.data)
        else: opts.add('// ---------------- NO task specific information')
        if not self.writeOptions(opts,activity,task):
          return None
      return self.run
    error('No tasks found for activity:'+activity)
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
      run_type = self.run.runType()
      info = self.getStreamInfo(self.storageMgr,'Storage')
      if info is not None:
        farms = self.run.subFarms.data
        slots = info.recvSlices()
        if len(farms) != len(slots):
          error('Severe mismatch: Number of subfarms:'+str(len(farms))+' Number of recv tasks:'+str(len(slots)))
          return None
        num = len(farms)
        run_type = self.run.runType()
        farm_names = []
        for i in xrange(num):
          node = slots[i].split(':')[0]
          name = '%s_%s_SF%02d_HLT'%(partition,node,i)
          farm = farms[i]
          farm_names.append(farm)
          opts = '// Auto generated options for partition:'+partition+' activity:'+run_type+'\n//\n'+\
                 '// ---------------- Data sending information for sub-farm:'+farm+'\n'+\
                 'OnlineEnv.Target  = "'+node+'-d1::'+name+'";\n'+\
                 'OnlineEnv.Target0 = { "'+node+'-d1::'+name+'" };\n'+\
                 'OnlineEnv.Target1 = { "'+node+'-d1" };\n'+\
                 'OnlineEnv.Target2 = { "'+name+'" };'
          fname = partition+'_'+farm+'_HLT'
          if self.writeOptionsFile(partition, fname, opts) is None:
            return None
          log('      --> Farm:'+farm+' sends to slot:'+slots[i]+', Task:'+name,timestamp=1)
          
        opts = Options('//  Auto generated options for partition:'+partition+' activity:'+run_type)
        opts.add('//\n// ---------------- General partition information:  ')
        opts.add('PartitionID',    self.run.partitionID())
        opts.add('PartitionIDName','%04X'%self.run.partitionID())
        opts.add('PartitionName',  partition)
        opts.add('Activity',       run_type)
        opts.add('SubFarms',       farm_names)
        
        fname = partition+'_Info'
        if self.writeOptionsFile(partition, fname, opts.value) is None:
          return None

        opts = Options('//  Auto generated options for partition:'+partition+' activity:'+run_type)
        opts.add('//\n// ---------------- Tell1 board information:')
        opts.add('OnlineEnv.Tell1Boards         = {')
        err = None
        for b in self.run.tell1Boards.data:
          itms = b.split(':')
          try:
            board = itms[0]
            if len(itms)>1: board = itms[1]
            ip = socket.gethostbyname(board+"-d1.data");
            opts.add('  "%s", "%s", "",'%(ip,board))
          except Exception,X:
            error('Failed to retrieve TELL1 specs for '+str(b),timestamp=1)
            error('Error '+str(X),timestamp=1)
            err = self
        opts.value = opts.value[:-2] + '\n};\n'
        if err:
          return None
        odin = 'Unknown'
        itms = ''
        try:
          odin  = self.run.odin.data
          itms  = odin.split(':')
          board = itms[0]
          if len(itms)>1: board = itms[1]
          ip    = socket.gethostbyname(board+"-d1.data");
          opts.add('ODIN_Name',   board)
          opts.add('ODIN_IP',     ip)
        except Exception,X:
          error('Failed to retrieve ODIN ip address for '+str(odin),timestamp=1)
          error('Error [IGNORED for NOW] '+str(X),timestamp=1)
          err = None
        if err:
          return None

        fname = partition+'_Tell1Boards'
        if self.writeOptionsFile(partition, fname, opts.value) is None:
          return None
        return self.run
    return None

# =============================================================================
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
    return {'HostName':'@HostName@', 'ProcessName':'@ProcessName@', 'ProcessNickName':'@ProcessNickName@', 'ProcessType':'@ProcessType@'}

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
      items = it[:-1]
      eval_item = eval(it[-1])
      if task.name == items[3]:
        done = done+1
        options = opts.value
        options = options.replace('@HostName@',items[0])
        options = options.replace('@ProcessName@',items[1])
        options = options.replace('@ProcessNickName@',items[2])
        options = options.replace('@ProcessType@',items[3])
        if isinstance(eval_item,list) or isinstance(eval_item,tuple):
          s = ''
          s0 = ''
          s1 = ''
          prefix = ''
          tot_len = len(eval_item)
          if tot_len>1: prefix = '\n     '
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
          if len(eval_item)>1: s = '%-26s = {%s%s};\n'%('OnlineEnv.Opt0',s,prefix)
          else:         s = '%-26s = {%s};\n%-26s = %s;\n'%('OnlineEnv.Opt0s',s,'OnlineEnv.Opt0',s)
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
    error('###\n###   UNKNOWN task requires options: '+task.name+' type:'+self.hostType+'\n###',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # ===========================================================================
  def _configure(self, partition):
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
      self.taskList = None
      return result
    error('Failed to access the Storage info:'+self.streamMgr.name()+' '+self.name)
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
      
# =============================================================================
class MonitoringOptionsWriter(StreamingOptionsWriter):
  """ Specialized options writer for the storage control node.
  
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
