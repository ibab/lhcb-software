import socket
import Online.PVSS as PVSS
import Online.Utils
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
class OptionsWriter(CommandListener):
  # ===========================================================================
  def __init__(self, manager, hosttype, cluster, name, info):
    "Object constructor."
    self.hostType = hosttype
    self.cluster  = cluster
    self.optionsDir = None
    self.infoCreator = info
    CommandListener.__init__(self,manager,name,name,4)
    devMgr = self.manager.deviceMgr()
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
  def getParams(self, partition):
    run = self.infoCreator.create(partition).load()
    if run:
      tasks = []
      activity = JobOptions.Activity(self.manager,self.activityName(run))
      if activity.exists():
        task_names = activity.taskNames()
        for name in task_names:
          task = JobOptions.TaskType(self.manager,name)
          if task.exists():
            tasks.append(task)
          else:
            error('The task '+name+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
            return None
        return (run,activity,tasks)
      else:
        error('The activity '+self.activityName(run)+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
        return None
    error('Information for partition '+partition+' does not exist!',timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None

  # ===========================================================================
  def writeOptions(self, opts, run, activity, task):
    return self.writeOptionsFile(opts, task.name)

  # ===========================================================================
  def writeOptionsFile(self, name, opts):
    log('###   Writing options for task: '+self.optionsDir+'/'+name,timestamp=1)
    if self.optionsDir is not None:
      try:
        desc = open(self.optionsDir+'/'+name+'.opts','w')
        print >>desc, opts
        desc.close()
      except Exception,X:
        error('Failed to write options for task:'+name+' '+str(X),timestamp=1,type=PVSS.FILEOPEN)
        return None
    return self
  
  # ===========================================================================
  def additionalOptions(self, context, run, activity, task):
    return context

  # ===========================================================================
  def configureAdditionalOptions(self, run, activity, tasks):
    return None

  # ===========================================================================
  def write(self, partition):
    res = self.getParams(partition)
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

  # ===========================================================================
  def handle(self, items, answer):
    "Callback once per item in the device sensor list on datapoint change."
    command   = items[0]
    sysName   = items[1]
    partition = items[2]
    partID    = items[3]
    result = None
    print command, sysName, partition, partID
    if command == "CONFIGURE" or command == "ALLOCATE":
      result = self.write(partition)
      if result is not None:
        return self.makeAnswer('READY',answer)
    elif command == "RESET" or command == "DEALLOCATE":
      return self.makeAnswer('READY',answer)
    elif command == "RECOVER":
      return self.makeAnswer('READY',answer)
      #result = self.write(partition)
      #if result is not None:
      #  return self.makeAnswer('READY',answer)
      
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
  def checkTasks(self, opts, run, activity, task, data, item, opt=-1):
    done = 0
    for i in data:
      it = i.split('/')
      items = it[:-1]
      eval_item = eval(it[-1])
      # print '++++++++++++++++++++++>',task.name,items
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
                # if val[0] == "storestrm01-d1": val = ("192.167.10.11",val[1])
                # if val[0] == "storestrm02-d1": val = ("192.167.10.12",val[1])
                # if val[0] == "storerecv01-d1": val = ("192.167.10.1",val[1])
                # if val[0] == "storerecv02-d1": val = ("192.167.10.2",val[1])
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
        if self.writeOptionsFile(items[1], options) is None:
          return None
    if done: return self
    return None
  def printTasks(self,data):
    for i in data:
      print 'Task:',i
  # ===========================================================================
  def writeOptions(self, opts, run, activity, task):
    if self.hostType == 'RECV':
      if self.checkTasks(opts,run,activity,task,run.rcvSenders.data,5,4):
        return self
      if self.checkTasks(opts,run,activity,task,run.receivers.data,-1):
        return self
      if self.checkTasks(opts,run,activity,task,run.rcvInfraTasks.data,-1):
        return self
      self.printTasks(run.receivers.data)
    elif self.hostType == 'STREAM':
      if self.checkTasks(opts,run,activity,task,run.strReceivers.data,3):
        return self
      if self.checkTasks(opts,run,activity,task,run.streamers.data,2,3):
        return self
      if self.checkTasks(opts,run,activity,task,run.strInfraTasks.data,3):
        return self
      self.printTasks(run.strInfraTasks.data)
    error('###\n###   UNKNOWN task requires options: '+task.name+' type:'+self.hostType+'\n###',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None
  # ===========================================================================
  def write(self, partition):
    self.hostType = 'RECV'
    res1 = OptionsWriter.write(self,partition)
    self.hostType = 'STREAM'
    res2 = OptionsWriter.write(self,partition)
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
  def checkTasks(self, opts, run, activity, task, data, item, opt=-1):
    done = 0
    for i in data:
      it = i.split('/')
      items = it[:-1]
      eval_item = eval(it[-1])
      # print '++++++++++++++++++++++>',task.name,items
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
                # if val[0] == "storestrm01-d1": val = ("192.167.10.11",val[1])
                # if val[0] == "storestrm02-d1": val = ("192.167.10.12",val[1])
                # if val[0] == "storerecv01-d1": val = ("192.167.10.1",val[1])
                # if val[0] == "storerecv02-d1": val = ("192.167.10.2",val[1])
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
        if self.writeOptionsFile(items[1], options) is None:
          return None
    if done: return self
    return None
  def printTasks(self,data):
    for i in data:
      print 'Task:',i
  # ===========================================================================
  def writeOptions(self, opts, info, activity, task):
    if self.hostType == 'MONRELAY':
      if self.checkTasks(opts,info,activity,task,info.senders.data,5,4):
        return self
      if self.checkTasks(opts,info,activity,task,info.relayTasks.data,5,4):
        return self
      if self.checkTasks(opts,info,activity,task,info.relayInfraTasks.data,-1):
        return self
      self.printTasks(info.senders.data)
      self.printTasks(info.relayTasks.data)
      self.printTasks(info.relayInfraTasks.data)
    elif self.hostType == 'MONI':
      if self.checkTasks(opts,info,activity,task,info.monReceivers.data,3):
        return self
      if self.checkTasks(opts,info,activity,task,info.monTasks.data,2,3):
        return self
      if self.checkTasks(opts,info,activity,task,info.monInfraTasks.data,3):
        return self
      self.printTasks(info.monReceivers.data)
      self.printTasks(info.monTasks.data)
      self.printTasks(info.monInfraTasks.data)
    error('### UNKNOWN task requires options: '+task.name+' type:'+self.hostType+'\n###',
          timestamp=1,type=PVSS.ILLEGAL_VALUE)
    return None
  # ===========================================================================
  def write(self, partition):
    self.hostType = 'MONRELAY'
    res1 = OptionsWriter.write(self,partition)
    self.hostType = 'MONI'
    res2 = OptionsWriter.write(self,partition)
    if res1 is None or res2 is None: return None
    return self

"""
import Online.PVSS as PVSS
import Online.JobOptions.JobOptionsControl as WR
mgr = PVSS.controlsMgr()
# w = WR.HLTOptionsWriter(mgr,'JobOptions')

w = WR.StorageOptionsWriter(mgr,'StorageJobOptions')
# w.write('LHCb')

w.run()
"""
