import socket
import Online.PVSS
import Online.Utils
import Online.RunInfo
import Online.JobOptions.JobOptions as JobOptions

log            = Online.Utils.log
error          = Online.Utils.error
warning        = Online.Utils.warning
RunInfo        = Online.RunInfo.RunInfo
DataPoint      = Online.PVSS.DataPoint
DeviceSensor   = Online.PVSS.DeviceSensor
CommandListener = Online.PVSS.CommandListener

# =============================================================================
class OptionsWriter(CommandListener):
  # ===========================================================================
  def __init__(self, manager, hosttype, cluster, name):
    "Object constructor."
    self.hostType = hosttype
    self.cluster  = cluster
    CommandListener.__init__(self,manager,name,name,4)
    devMgr = self.manager.deviceMgr()
    if not devMgr.exists(name):
      typ = self.manager.typeMgr().type('JobOptionsControl')
      if not devMgr.createDevice(name,typ,1).release():
        error('Failed to create the datapoint:"'+name+'"',timestamp=1)
      else:
        log('Created the datapoint:"'+name+'"',timestamp=1)

  # ===========================================================================
  def activityName(self, run):
    print 'Activity name:',run.name, self.hostType+'_'+run.runType()
    return self.hostType+'_'+run.runType()

  # ===========================================================================
  def getParams(self, partition):
    run = RunInfo(self.manager,partition).load()
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
            error('The task '+name+' does not exist!',timestamp=1)
            return None
        return (run,activity,tasks)
      else:
        error('The activity '+self.activityName(run)+' does not exist!',timestamp=1)
        return None
    error('The RunInfo for partition '+partition+' does not exist!',timestamp=1)
    return None

  # ===========================================================================
  def writeOptions(self, opts, run, activity, task):
    print '###\n###   Writing options for task: '+task.name+'\n###\n'+opts
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
        opts = '//  Auto generated options for partition:'+partition+\
          ' activity:'+run_type+' task:'+task.name+'\n'
        if task.defaults.data:
          opts = opts + '//\n' +\
            '// ---------------- General partition information:  \n'+ \
            'OnlineEnv.PartitionID   = '+str(run.partitionID())+';\n'+ \
            'OnlineEnv.PartitionName = "'+partition+'";\n'+ \
            'OnlineEnv.Activity      = "'+run_type+'";\n'+ \
            'OnlineEnv.HostType      = "'+self.hostType+'";\n'+ \
            'OnlineEnv.TaskType      = "'+task.name+'";\n'
        else:
          opts = opts + '// ---------------- NO partition information\n'
        additionalOpts = self.additionalOptions(context,run,activity,task)
        if additionalOpts:
          opts = opts + '//\n' + \
            '// ---------------- Additional specific options:  \n'
          for o in additionalOpts.keys():
            opts = opts + 'OnlineEnv.'+o+' = '+str(additionalOpts[o])+';\n'
        else:
          opts = opts + '// ---------------- NO additional options\n'
        if task.tell1s.data:
          opts = opts + '//\n' + \
            '// ---------------- Tell1 board information:  \n'+  \
            'OnlineEnv.Tell1Boards    = {' 
          for b in run.tell1Boards.data:
            opts = opts + '\n  "'+b+'", \"'+b+'", "",'
          opts = opts[:-1] + '\n};\n'
        else:
          opts = opts + '// ---------------- NO tell1 board information\n'
        if task.options.data:
          opts = opts + '//\n' +\
            '// ---------------- Task specific information:  \n'+  \
            task.options.data+'\n'
        else:
          opts = opts + '// ---------------- NO task specific information\n'
        if not self.writeOptions(opts,run,activity,task):
          log('Failed to write options for task:'+task.name,timestamp=1)
          return None
      return self
    else:
      log('The partition '+partition+' does not exist!',timestamp=1)
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
      result = self.write(partition)
      if result is not None:
        return self.makeAnswer('READY',answer)
      
    return None

# =============================================================================
class HLTOptionsWriter(OptionsWriter):
  """ Specialized options writer for HLT control nodes.
  
      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name):
    "Object constructor."
    node = socket.gethostbyaddr(socket.gethostname())[0]
    cluster = node[:node.find('.')]
    cluster = 'lbhlt04'
    OptionsWriter.__init__(self,manager,'HLT',cluster, name)
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
  def __init__(self, manager, name):
    "Object constructor."
    cluster = 'storage'
    OptionsWriter.__init__(self,manager,'RECV',cluster, name)
  # ===========================================================================
  def additionalOptions(self, context, run, activity, task):
    return {'DataSink':'@DataSink@'}
  def checkTasks(self,opts,data,item,task):
    done = 0
    for i in data:
      items = i.split('/')
      #print task.name,items
      if task.name == items[3]:
        done = 1
  def printTasks(self,data):
    for i in data:
      print 'Task:',i
        if item<0:
          options = opts.replace('@DataSink@','"NONE"')
        else:
          options = opts.replace('@DataSink@','"'+items[item]+'"')
        print '###\n###   Writing options for task: '+items[1]+'\n###'
        print options
    if done: return self
    return None
  def printTasks(self,data):
    for i in data:
      self.printTasks(run.strReceivers.data)
      print i
  # ===========================================================================
  def writeOptions(self, opts, run, activity, task):
    if self.hostType == 'RECV':
      if self.checkTasks(opts,run.rcvSenders.data,5,task):      return self
      if self.checkTasks(opts,run.receivers.data,-1,task):      return self
      if self.checkTasks(opts,run.rcvInfraTasks.data,-1,task):  return self
      self.printTasks(run.receivers.data)
    elif self.hostType == 'STREAM':
      if self.checkTasks(opts,run.strReceivers.data,3,task):    return self
      if self.checkTasks(opts,run.streamers.data,3,task):       return self
      if self.checkTasks(opts,run.strInfraTasks.data,3,task):   return self
      self.printTasks(run.strInfraTasks.data)
    print '###\n###   Writing options for UNKNOWN tasks: '+task.name+' type:'+self.hostType+'\n###'
    return None
  # ===========================================================================
  def write(self, partition):
    self.hostType = 'RECV'
    res1 = OptionsWriter.write(self,partition)
    self.hostType = 'STREAM'
    res2 = OptionsWriter.write(self,partition)
    if res1 is None or res2 is None: return None
    return self
"""
import Online.PVSS as PVSS
import Online.JobOptions.JobOptionsControl as WR
mgr = PVSS.controlsMgr()
# w = WR.HLTOptionsWriter(mgr,'JobOptions')

w = WR.StorageOptionsWriter(mgr,'JobOptions')
# w.write('LHCb')

w.run()
"""
