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
  def __init__(self, manager, hosttype, name, info):
    "Object constructor."
    Control.AllocatorClient.__init__(self,manager,name)
    self.hostType = hosttype
    self.optionsDir = None
    self.infoCreator = info
    devMgr = self.manager.deviceMgr()
    self.optionsMgr = Systems.controlsMgr(Params.jobopts_system_name)
    dp_name = self.manager.name()+':'+name
    if not devMgr.exists(dp_name):
      typ = self.manager.typeMgr().type('JobOptionsControl')
      if not devMgr.createDevice(dp_name,typ,1).release():
        error('Failed to create the datapoint:"'+dp_name+'"',timestamp=1,type=PVSS.DP_NOT_EXISTENT)
      else:
        log('Created the datapoint:"'+dp_name+'"',timestamp=1)

  # ===========================================================================
  def get(self,name):
    nam = DataPoint.original(self.name+'.'+name)
    log('Access datapoint:'+nam)
    return DataPoint(self.manager,nam)

  # ===========================================================================
  def activityName(self, run):
    log('Activity name:'+run.name+' '+self.hostType+'_'+run.runType(),timestamp=1)
    return self.hostType+'_'+run.runType()

  # ===========================================================================
  def getParams(self, rundp_name, partition):
    run = self.infoCreator.create(rundp_name,partition).load()
    #print 'getParams',rundp_name,partition,str(run)
    if run:
      tasks = []
      activity_name = self.activityName(run)
      #log('getParams:'+rundp_name+' '+str(partition)+' '+str(activity_name))
      activity = JobOptions.Activity(self.optionsMgr,activity_name)
      if activity.exists():
        task_names = activity.taskNames()
        #log('Tasks:'+str(task_names))
        for name in task_names:
          if len(name)>0:
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
    #log('Opts:'+opts.value)
    #log('Run: '+run.name)
    #log('Task:'+task.name)
    #log('Activity:'+activity.name)
    return self.writeOptionsFile(run.name, task.name, opts.value)

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
      return run
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
    print 'HLTOptionsWriter:',name
    OptionsWriter.__init__(self, manager, 'HLT', name, info)
    devMgr = self.manager.deviceMgr()
    typ = self.manager.typeMgr().type('JobOptionsControl')
    print 'Manager:',self.manager.name()
    print 'Options Manager:',self.optionsMgr.name()
    for i in xrange(16):
      nam = self.manager.name()+':'+name+'Writer_%02X'%(i,)
      if not DataPoint.exists(nam):
        if not devMgr.createDevice(nam,typ,1).release():
          error('Failed to create the datapoint:"'+nam+'"',timestamp=1,type=PVSS.DP_NOT_EXISTENT)
        else:
          log('Created the datapoint:"'+nam+'"',timestamp=1)

  # ===========================================================================
  def configure(self, rundp_name, partition):
    import Online.Streaming.StreamingDescriptor as StreamingDescriptor
    import Online.Streaming.PartitionInfo as PartitionInfo
    import Online.SetupParams as Params
    run = OptionsWriter.configure(self,rundp_name,partition)
    if run:
      storage_mgr = Systems.controlsMgr(Params.storage_system_name)
      res = StreamingDescriptor.findPartition(storage_mgr,'Storage',run.name)
      if res is not None:
        dp, nam, id = res
        info = PartitionInfo.PartitionInfo(storage_mgr,nam).load()
        farms = run.subFarms.data
        slots = info.recvSlices()
        if len(farms) != len(slots):
          error('Severe mismatch: Number of subfarms:'+str(len(farms))+' Number of recv tasks:'+str(len(recv)))
          return None
        num = len(farms)
        run_type = run.runType()
        for i in xrange(num):
          node = slots[i].split(':')[0]
          name = '%s_%s_SF%02d_HLT'%(partition,node,i)
          farm = farms[i]
          opts = '// Auto generated options for partition:'+partition+' activity:'+run_type+'\n'+\
                 '// ---------------- Data sending information for sub-farm:'+farm+'\n'+\
                 'OnlineEnv.Target  = "'+node+'-d1::'+name+'";\n'+\
                 'OnlineEnv.Target0 = { "'+node+'-d1::'+name+'" };\n'+\
                 'OnlineEnv.Target1 = { "'+node+'-d1" };\n'+\
                 'OnlineEnv.Target2 = { "'+name+'" };'
          fname = partition+'_'+farm
          if self.writeOptionsFile(partition, fname, opts) is None:
            error('Failed to write farm options file:'+fname,timestamp=1,type=PVSS.ILLEGAL_VALUE)
            return None
          log('--> Farm:'+farm+' sends to '+slots[i]+', '+name,timestamp=1)
          print opts
        return run
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
    OptionsWriter.__init__(self, manager, 'RECV', name, info)
  # ===========================================================================
  def additionalOptions(self, context, run, activity, task):
    return {'HostName':'@HostName@', 'ProcessName':'@ProcessName@', 'ProcessNickName':'@ProcessNickName@', 'ProcessType':'@ProcessType@'}
  # ===========================================================================
  def checkTasks(self, opts, run, activity, task, data):
    done = 0
    checked = 0
    for i in data:
      it = i.split('/')
      items = it[:-1]
      eval_item = eval(it[-1])
      ## print '++++++++++++++++++++++>',task.name,items
      checked = checked+1
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
    if not done and checked>0: return self
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
    self.printTasks(self.streamInfo.dataSources())
    self.printTasks(self.streamInfo.recvReceivers())
    self.printTasks(self.streamInfo.recvSenders())
    self.printTasks(self.streamInfo.recvInfrastructure())
    self.printTasks(self.streamInfo.streamReceivers())
    self.printTasks(self.streamInfo.streamSenders())
    self.printTasks(self.streamInfo.streamInfrastructure())
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
    OptionsWriter.__init__(self, manager, 'MONRELAY', name, info)
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
    checked = 0
    for i in data:
      it = i.split('/')
      items = it[:-1]
      eval_item = eval(it[-1])
      checked = checked + 1
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
    if done==0 and checked>0: return self
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
