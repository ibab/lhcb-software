import socket
import Online.Utils
import Online.PVSS as PVSS
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.AllocatorControl as Control
import Online.JobOptions.JobOptions as JobOptions

log             = Online.Utils.log
error           = PVSS.error
warning         = Online.Utils.warning
DataPoint       = PVSS.DataPoint
DeviceSensor    = PVSS.DeviceSensor
CommandListener = PVSS.CommandListener

class Options:
  def __init__(self,msg=''):
    self.value = msg
    if len(self.value)>0: self.value = self.value+'\n'
  def add(self,name,value=None):
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
  # ===========================================================================
  def __init__(self, manager, hosttype, cluster, name, info):
    "Object constructor."
    Control.AllocatorClient.__init__(self,manager,name)
    self.hostType = hosttype
    self.cluster  = cluster
    self.optionsDir = None
    self.infoCreator = info
    devMgr = self.manager.deviceMgr()
    self.optionsMgr = Systems.controlsMgr(Params.jobopts_system_name)
    if not devMgr.exists(name):
      typ = self.manager.typeMgr().type('JobOptionsControl')
      if not devMgr.createDevice(name,typ,1).release():
        error('Failed to create the datapoint:"'+name+'"',timestamp=1,type=PVSS.DP_NOT_EXISTENT)
      else:
        log('Created the datapoint:"'+name+'"',timestamp=1)

  # ===========================================================================
  def activityName(self, run):
    log('Activity name:'+run.name+' '+self.hostType+'_'+run.runType(),timestamp=1)
    return self.hostType+'_'+run.runType()

  # ===========================================================================
  def getParams(self, rundp_name, partition):
    run = self.infoCreator.create(rundp_name,partition).load()
    # print 'getParams',rundp_name,partition,str(run)
    if run:
      tasks = []
      activity_name = self.activityName(run)
      # print 'getParams',rundp_name,partition,activity_name
      activity = JobOptions.Activity(self.optionsMgr,activity_name)
      if activity.exists():
        task_names = activity.taskNames()
        for name in task_names:
          task = JobOptions.TaskType(self.optionsMgr,name)
          if task.exists():
            tasks.append(task)
          else:
            error('The task '+name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
            return None
        return (run,activity,tasks)
      else:
        error('The activity '+activity_name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
        return None
    error('Information for partition '+partition+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # ===========================================================================
  def writeOptions(self, opts, run, activity, task):
    return self.writeOptionsFile(run.name, task.name, opts)

  # ===========================================================================
  def writeOptionsFile(self, partition, name, opts):
    import os
    if self.optionsDir is not None:
      try:
        fd = self.optionsDir+'/'+partition
        fn = fd+'/'+name+'.opts'
        log('###   Writing options for task: '+fn,timestamp=1)
        try:
          os.stat(fd)
        except:
          os.mkdir(fd)
        desc = open(fn, 'w')
        print >>desc, opts
        desc.close()
      except Exception,X:
        error('Failed to write options for task:'+name+' '+str(X),timestamp=1,type=PVSS.FILEOPEN)
        return None
      return self
    error('Failed to write options for task:'+name+' [No Directory]',timestamp=1,type=PVSS.FILEOPEN)
    return None
  
  # ===========================================================================
  def additionalOptions(self, context, run, activity, task):
    return context

  # ===========================================================================
  def configureAdditionalOptions(self, run, activity, tasks):
    return None

  # ===========================================================================
  def configure(self, rundp_name, partition):
    res = self.getParams(rundp_name,partition)
    if res:
      run, activity, tasks = res
      run_type = run.runType()
      context = self.configureAdditionalOptions(run, activity, tasks)
      for task in tasks:
        opts = Options('//  Auto generated options for partition:'+partition+\
               ' activity:'+run_type+' task:'+task.name+'\n' +\
               '#include "$PREAMBLE_OPTS"')
        if task.defaults.data:
          #print 'Task', task
          opts.add('//\n// ---------------- General partition information:  ')
          opts.add('PartitionID',   run.partitionID())
          opts.add('PartitionName', partition)
          opts.add('Activity',      run_type);
          opts.add('HostType',      self.hostType)
          opts.add('HostTypes',    [self.hostType])
          opts.add('TaskType',      task.name)
        else: opts.add('// ---------------- NO partition information')
        addOpts = self.additionalOptions(context,run,activity,task)
        if addOpts:
          opts.add('//\n// ---------------- Additional specific options:')
          for nam,opt in addOpts.items(): opts.add(nam,opt)
        else: opts.add('// ---------------- NO additional options')
        if task.tell1s.data:
          opts.add('//\n// ---------------- Tell1 board information:')
          opts.add('OnlineEnv.Tell1Boards         = {\n')
          for b in run.tell1Boards.data: opts.add('  "%s", \"%s", "",\n'%(b,b))
          opts.value = opts.value[:-2] + '\n};\n'
        else: opts.add('// ---------------- NO tell1 board information') 
        if task.options.data:
          opts.add('//\n// ---------------- Task specific information:')
          opts.add(task.options.data)
        else: opts.add('// ---------------- NO task specific information')
        if not self.writeOptions(opts,run,activity,task):
          return None
      return self
    else:
      error('The partition '+partition+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

# =============================================================================
class HLTOptionsWriter(OptionsWriter):
  """ Specialized options writer for HLT control nodes.
  
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    "Object constructor."
    node = socket.gethostbyaddr(socket.gethostname())[0]
    cluster = node[:node.find('.')]
    cluster = 'lbhlt04'
    OptionsWriter.__init__(self,manager,'HLT',cluster, name, info)
  # ===========================================================================
  def configureAdditionalOptions(self, run, activity, tasks):
    opts = {}
    for i in run.receivers.data:
      storage, name, short, type, node = i.split('/')
      if node == self.cluster:
        opts['ClusterName'] = '"'+self.cluster+'"'
        opts['TargetTask']  = '"'+name+'"'
        opts['TargetNode']  = '"'+storage+'"'
        return opts
    return None

# =============================================================================
class StorageOptionsWriter(OptionsWriter):
  """ Specialized options writer for the storage control node.
  
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    "Object constructor."
    cluster = 'storage'
    OptionsWriter.__init__(self, manager, 'RECV', cluster, name, info)
  # ===========================================================================
  def additionalOptions(self, context, run, activity, task):
    return {'HostName':'@HostName@', 'ProcessName':'@ProcessName@', 'ProcessNickName':'@ProcessNickName@', 'ProcessType':'@ProcessType@'}
  # ===========================================================================
  def checkTasks(self, opts, run, activity, task, data):
    done = 0
    for i in data:
      it = i.split('/')
      items = it[:-1]
      eval_item = eval(it[-1])
      ## print '++++++++++++++++++++++>',task.name,items
      if task.name == items[3]:
        done = 1
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
        if self.writeOptionsFile(run.name, items[1], options) is None:
          return None
    if done: return self
    return None
  def printTasks(self,data):
    for i in data:
      print 'Task:',i
  # ===========================================================================
  def writeOptions(self, opts, run, activity, task):
    import Online.Streaming.StreamingDescriptor as StreamingDescriptor
    import Online.Streaming.PartitionInfo as PartitionInfo
    import Online.SetupParams as Params
    if not self.streamInfo:
      system_mgr = Systems.controlsMgr(Params.storage_system_name)
      res = StreamingDescriptor.findPartition(system_mgr,'Storage',run.name)
      if res is not None:
        dp, nam, id = res
        self.streamInfo = PartitionInfo.PartitionInfo(system_mgr,nam).load()
    if not self.streamInfo:
      error('###\n###   Cannot access Stream Slice for partition:'+run.name+'\n###',
            timestamp=1,type=PVSS.ILLEGAL_VALUE)
      return None
    if self.hostType == 'RECV':
      if self.checkTasks(opts,run,activity,task,self.streamInfo.recvSenders()):
        return self
      if self.checkTasks(opts,run,activity,task,self.streamInfo.recvReceivers()):
        return self
      if self.checkTasks(opts,run,activity,task,self.streamInfo.recvInfrastructure()):
        return self
    elif self.hostType == 'STREAM':
      if self.checkTasks(opts,run,activity,task,self.streamInfo.streamReceivers()):
        return self
      if self.checkTasks(opts,run,activity,task,self.streamInfo.streamSenders()):
        return self
      if self.checkTasks(opts,run,activity,task,self.streamInfo.streamInfrastructure()):
        return self
    error('###\n###   UNKNOWN task requires options: '+task.name+' type:'+self.hostType+'\n###',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None
  # ===========================================================================
  def configure(self, rundp_name, partition):
    self.streamInfo = None
    self.hostType = 'RECV'
    res1 = OptionsWriter.configure(self, rundp_name, partition)
    self.hostType = 'STREAM'
    res2 = OptionsWriter.configure(self, rundp_name, partition)
    self.streamInfo = None
    if res1 is None or res2 is None: return None
    return self

# =============================================================================
class MonitoringOptionsWriter(OptionsWriter):
  """ Specialized options writer for the storage control node.
  
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, info):
    "Object constructor."
    cluster = 'monitoring'
    OptionsWriter.__init__(self, manager, 'MONRELAY', cluster, name, info)
  # ===========================================================================
  def additionalOptions(self, context, run, activity, task):
    return {'HostName':'@HostName@',
            'ProcessName':'@ProcessName@',
            'ProcessNickName':'@ProcessNickName@',
            'ProcessType':'@ProcessType@'
            }
  # ===========================================================================
  def checkTasks(self, opts, run, activity, task, data):
    done = 0
    for i in data:
      it = i.split('/')
      items = it[:-1]
      eval_item = eval(it[-1])
      # print '++++++++++++++++++++++>',task.name,items[3],items
      if task.name == items[3]:
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
        if self.writeOptionsFile(run.name, items[1], options) is None:
          return None
        done = done + 1
    return done

  def printTasks(self,data):
    for i in data:
      print 'Task:',i
  # ===========================================================================
  def writeOptions(self, opts, info, activity, task):
    import Online.Streaming.StreamingDescriptor as StreamingDescriptor
    import Online.Streaming.PartitionInfo as PartitionInfo
    import Online.SetupParams as Params
    if not self.streamInfo:
      system_mgr = Systems.controlsMgr(Params.monitor_system_name)
      res = StreamingDescriptor.findPartition(system_mgr,'Monitoring',info.name)
      if res is not None:
        dp, nam, id = res
        self.streamInfo = PartitionInfo.PartitionInfo(system_mgr,nam).load()
    if not self.streamInfo:
      error('###\n###   Cannot access Stream Slice for partition:'+info.name+'\n###',
            timestamp=1,type=PVSS.ILLEGAL_VALUE)
      return None
    done = 0
    if self.hostType == 'MONRELAY':
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.dataSources())
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.recvReceivers())
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.recvSenders())
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.recvInfrastructure())
    elif self.hostType == 'MONI':
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.streamReceivers())
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.streamSenders())
      done = done + self.checkTasks(opts,info,activity,task,self.streamInfo.streamInfrastructure())
    if done > 0: return self

    self.printTasks(self.streamInfo.dataSources())
    self.printTasks(self.streamInfo.recvReceivers())
    self.printTasks(self.streamInfo.recvSenders())
    self.printTasks(self.streamInfo.recvInfrastructure())
    self.printTasks(self.streamInfo.streamReceivers())
    self.printTasks(self.streamInfo.streamSenders())
    self.printTasks(self.streamInfo.streamInfrastructure())
    error('### UNKNOWN task requires options: '+task.name+' type:'+self.hostType+'\n###',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None
  # ===========================================================================
  def configure(self, rundp_name, partition):
    self.streamInfo = None
    self.hostType = 'MONRELAY'
    res1 = OptionsWriter.configure(self, rundp_name, partition)
    self.hostType = 'MONI'
    res2 = OptionsWriter.configure(self, rundp_name, partition)
    self.streamInfo = None
    if res1 is None or res2 is None: return None
    return self
