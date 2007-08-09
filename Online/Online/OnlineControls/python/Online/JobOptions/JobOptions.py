"""

import Online.PVSS as PVSS
import Online.SetupParams as Params
import Online.PVSSSystems as Systems
import Online.JobOptions.JobOptions as JobOptions
mgr = Systems.controlsMgr(Params.jobopts_system_name)
a = JobOptions.Activity(mgr,'LHCb_PHYSICS')
a.show()

reload(JobOptions)
JobOptions.UI(mgr).run()
"""

import Online.PVSS
import Online.Utils
import Online.SetupParams as Params
import Online.PVSSSystems as Systems

DpIDVector     = Online.PVSS.DpIDVector
DataPoint      = Online.PVSS.DataPoint
DPVector       = Online.PVSS.DataPointVector
DpVectorActor  = Online.PVSS.DpVectorActor
log            = Online.Utils.log
error          = Online.Utils.error
warning        = Online.Utils.warning

Partition_t    = 'JobOptionsPartition'
Activity_t     = 'JobOptionsActivity'
TaskType_t     = 'JobOptionsTaskType'
debug = 0

# =============================================================================
class JobOptionsObject:
  # ===========================================================================
  def __init__(self, manager, name, type):
    self.manager    = manager
    self.name       = name
    self.writer     = self.manager.devWriter()
    self.reader     = self.manager.devReader()
    self.type       = self.manager.typeMgr().type(type)
    self.typeName   = type
  # ===========================================================================
  def fullname(self):
    return self.manager.name()+':'+self.typeName+'_'+self.name
  # ===========================================================================
  def datapoint(self):
    return DataPoint(self.manager,self.fullname())
  # ===========================================================================
  def create(self):
    name = self.fullname()
    mgr  = self.manager.deviceMgr()
    if not mgr.exists(name):
      dp = mgr.createDevice(name,self.type).release()
      if dp is None:
        log('Failed to create datapoint:'+name+' [Internal error]',timestamp=1)
      return dp
    log('Failed to create datapoint:'+name+' [Datapoint exists]',timestamp=1)
    return None
  # ===========================================================================
  def drop(self):
    name = self.fullname()
    mgr = self.manager.deviceMgr()
    if mgr.exists(name):
      result = mgr.deleteDevice(name)
      if result is None:
        log('Failed to delete datapoint:'+name+' [Internal error]',timestamp=1)
      return result
    log('Failed to delete datapoint:'+name+' [Datapoint does not exist]',timestamp=1)
    return None
  # ===========================================================================
  def exists(self):
    return DataPoint.exists(self.fullname())
  # ===========================================================================
  def loadIfExists(self):
    name = self.fullname()
    if DataPoint.exists(name):
      self.load()
    else:
      log('The datapoint:'+name+' does not exist.',timestamp=1)
  # ===========================================================================
  def _dp(self, nam):
    return DataPoint(self.manager,DataPoint.original(self.fullname()+'.'+nam))
  # ===========================================================================
  def _getItem(self, dp):
    if dp.data is None: self.load()
    return dp.data
  # ===========================================================================
  def _setItem(self, dp, value):
    dp.data = value
    return self.save()

# =============================================================================
class TaskType(JobOptionsObject):
  # ===========================================================================
  def __init__(self, manager, name):
    JobOptionsObject.__init__(self,manager,name,TaskType_t)
    self.defaults = self._dp('NeedDefaults')
    self.tell1s   = self._dp('NeedTell1s')
    self.options  = self._dp('Options')
    self.loadIfExists()
  # ===========================================================================
  def load(self):
    self.reader.add(self.defaults)
    self.reader.add(self.tell1s)
    self.reader.add(self.options)
    if debug: log('TaskType> Loading '+self.name,timestamp=1)
    return self.reader.execute(0,1)
  
  # ===========================================================================
  def save(self):
    self.writer.add(self.defaults)
    self.writer.add(self.tell1s)
    self.writer.add(self.options)
    if debug: log('TaskType> Saving '+self.name+' Defaults:'+str(self.defaults.data)+' Tell1s:'+str(self.tell1s.data),timestamp=1)
    return self.writer.execute(0,1)

  # ===========================================================================
  def show(self, prefix=''):
    p = prefix
    if self.options.data is None: self.load()
    log(p+self.name+'.Options:     "'+str(self.options.data)+'"',timestamp=1)
    log(p+self.name+'.NeedDefaults:'+str(self.defaults.data),timestamp=1)
    log(p+self.name+'.NeedTell1s:  '+str(self.tell1s.data),timestamp=1)

  # ===========================================================================
  def getOptions(self):         return self._getItem(self.options)
  def setOptions(self, value):  return self._setItem(self.options,value)
  def getDefaults(self):        return self._getItem(self.defaults)
  def setDefaults(self, value): return self._setItem(self.defaults,value)
  def getTell1s(self):          return self._getItem(self.tell1s)
  def setTell1s(self, value):   return self._setItem(self.tell1s,value)

# =============================================================================
class Activity(JobOptionsObject):
  # ===========================================================================
  def __init__(self, manager, name):
    JobOptionsObject.__init__(self,manager,name,Activity_t)
    self.tasks = self._dp('')
    self.loadIfExists()

  # ===========================================================================
  def load(self):
    self.reader.add(self.tasks)
    if debug: log('Activity> Loading '+self.name,timestamp=1)
    return self.reader.execute(0,1)
  
  # ===========================================================================
  def save(self):
    self.writer.add(self.tasks)
    if debug: log('Activity> Saving '+self.name,timestamp=1)
    return self.writer.execute(0,1)

  # ===========================================================================
  def taskNames(self):
    names = []
    for i in self.tasks.data:
      name = DataPoint(self.manager, i).elementName()[len(TaskType_t)+1:]
      names.append(name)
    return names

  # ===========================================================================
  def addTask(self,task):
    dp = DataPoint(self.manager,TaskType_t+'_'+task)
    self.tasks.data.push_back(dp.id())

  # ===========================================================================
  def removeTask(self,task):
    dp = DataPoint(self.manager,TaskType_t+'_'+task)
    id = dp.id()
    tasks = DataPoint(self.manager,DataPoint.original(self.typeName+'_'+self.name()+'.'+nam))
    tasks.data = DpIDVector()
    found = 0
    for i in xrange(self.tasks.size()):
      if id != self.tasks[i].id():
        tasks.push_back(self.tasks[i].id())
      else:
        found = 1
    if found:
      self.tasks = tasks
      return self
    log('The task '+task+' is not member of the activity '+self.name,timestamp=1)
    return None
  
  # ===========================================================================
  def show(self, level=0):
    log('Activity:'+self.name+' has the following tasks:',timestamp=1)
    tasks = {}
    if self.tasks.data is None: self.load()
    for i in xrange(self.tasks.data.size()):
      dp = DataPoint(self.manager,self.tasks.data[i])
      nam = DataPoint.dpname(dp.id())
      nam = nam[nam.find(':')+1:]
      nam = nam[nam.find('_')+1:]
      if level > 0:
        tasks[nam] = TaskType(self.manager,nam)
      else:
        tasks[nam] = None
    keys = tasks.keys()
    if keys:
      for i in xrange(len(keys)):
        log('   %3d:  %s'%(i,keys[i],),timestamp=1)
        if level > 0:
          tasks[keys[i]].show(prefix='         ')
    return keys
  # ===========================================================================
  def getTasks(self):         return self._getItem(self.tasks)
  def setTasks(self, value):  return self._setItem(self.tasks,value)

# =============================================================================
class Partition(JobOptionsObject):
  # ===========================================================================
  def __init__(self, manager, name):
    JobOptionsObject.__init__(self,manager,name,Partition_t)
    self.activity = self._dp('Activity')
    self.partID   = self._dp('PartitionID')
    self.Tell1    = self._dp('Tell1Boards')
    self.command  = self._dp('Command')
    self.state    = self._dp('State')
    self.loadIfExists()

  # ===========================================================================
  def _loadRef(self,name,ref):
    nam = DataPoint.original(DataPoint.dpname(ref.data))
    dp = DataPoint(self.manager,nam)
    self.reader.add(dp)
    if self.reader.execute(0,1):
      self.activityName = dp.data
      if debug: log(name+':'+str(dp.data),timestamp=1)
      return dp
    log(name+' DP:'+nam+' does not exist.',timestamp=1)
    return None

  # ===========================================================================
  def _setValue(self,name,ref, data):
    nam = DataPoint.original(DataPoint.dpname(ref.data))
    dp  = DataPoint(self.manager,nam)
    dp.data = data
    self.writer.add(dp)
    if self.writer.execute(0,1):
      self.activityName = dp.data
      if debug: log(name+' is set to:'+str(dp.data),timestamp=1)
      return dp
    log(name+' DP:'+nam+' does not exist.',timestamp=1)
    return None

  # ===========================================================================
  def _setRef(self,name,ref, data):
    ref.data = val
    self.writer.add(ref)
    return self.writer.execute(0,1)
  
  # ===========================================================================
  def load(self):
    self.reader.add(self.activity)
    self.reader.add(self.partID)
    self.reader.add(self.Tell1)
    if self.reader.execute(0,1) is not None:
      if debug: log('Partition> Loading:'+self.name,timestamp=1)
      return self
    return None

  # ===========================================================================
  def save(self):
    self.writer.add(self.activity)
    self.writer.add(self.partID)
    self.writer.add(self.Tell1)
    if debug: log('Partition> Saving:'+self.name,timestamp=1)
    return self.writer.execute(0,1)

  # ===========================================================================
  def activity(self):
    return self._loadRef('Activity',self.activity)
  
  # ===========================================================================
  def partitionID(self):
    return self._loadRef('Partition ID',self.partID)
  
  # ===========================================================================
  def tell1Boards(self):
    return self._loadRef('Tell1 Boards',self.Tell1)

  # ===========================================================================
  def setActivity(self, val, isRef=0):
    if isRef: return self._setRef('Activity',self.activity,val)
    return self._setValue('Activity',self.activity,val)

  # ===========================================================================
  def show(self):
    fmt = '%-32s:  %s'
    if self.Tell1.data is None: self.load()
    log(fmt%('Activity',self.activity().data,),timestamp=1)
    log(fmt%('PartitionID',self.partitionID().data,),timestamp=1)
    s = ''
    dp = self.tell1Boards()
    c = dp.data
    for i in xrange(c.size()):
      s = s + str(c[i]) + ' '
    log(fmt%('Tell 1 Boards',s,),timestamp=1)

# =============================================================================
class UI:
  #============================================================================
  def __init__(self, manager=None):
    self.manager = manager
    if manager is None:
      raise Exception,'Invalid PVSS controls manager passed to job options component.'
    self.activity = None
    self.partition = None
  #============================================================================
  def selectInput(self,prompt):
    val = ''
    try:
      res = str(input(prompt))
      print res
      return res
    except:
      print 'Invalid input....try again.'
      return None
  #============================================================================
  def selectItem(self,actor,device_type):
    val = self.selectInput("Select item [<Enter> to Cancel]:")
    if val:
      try:
        nam = actor.container[int(val)].name()
        nam = nam[nam.find(device_type)+len(device_type)+1:]
        return nam
      except:
        pass
    return None
    
  #============================================================================
  def selectFromList(self,items):
    val = self.selectInput("Select item [<Enter> to Cancel]:")
    if val:
      try:
        return items[val]
      except:
        pass
    return None
  #============================================================================
  def drop(self,object):
    if object.exists():
      object.drop()
  #============================================================================
  def run(self):
    import sys
    while(1):
      prompt = """
        What do you want to do?
        1 - Select partition
        2 - Select activity
        3 - Add task to activity
        4 - Remove task type from activity

        10 - Create a partition
        11 - Create a activity
        12 - Create a task type

        20 - Delete a partition
        21 - Delete a activity
        22 - Delete a task type
        
      Partition:"""+str(self.partition)+"""
      Activity: """+str(self.activity)+"""
      Enter command [<Enter> to Exit]:"""
      val = '1'
      try:
        val = str(input(prompt))
      except:
        print 'Bye bye...'
        return
        
      try:
        if ( val == '1' ):
          self.partition=self.selectItem(showPartitions(manager=self.manager),Partition_t)
        elif ( val == '2' ):
          self.activity=self.selectItem(showActivities(manager=self.manager),Activity_t)
        elif ( val == '3' ):
          task = self.selectItem(showTasks(manager=self.manager),TaskType_t)
          if task:
            activity = Activity(self.manager, self.activity)
            if activity.exists():
              t = TaskType(self.manager,task)
              if t.exists():
                activity.addTask(task)
                activity.save()
                activity.show()
        elif ( val == '4' ):
          activity = Activity(self.manager, self.activity)
          if activity.exists():
            task = self.selectFromList(activity.show())
            if task:
              activity.removeTask(task)
              activity.save()
              activity.show()
        elif ( val == '10' ):
          showPartitions(manager=self.manager)
          self.partition = self.selectInput('Enter partition name [<Enter> to Cancel]:')
          if self.partition:
            partition = Partition(self.manager, self.partition)
            if not partition.exists():
              if partition.create():
                showPartitions(manager=self.manager)
        elif ( val == '11' ):
          showActivities(manager=self.manager)
          self.activity = self.selectInput('Enter activity name [<Enter> to Cancel]:')
          if self.activity:
            activity = Activity(self.manager, self.activity)
            if not activity.exists():
              if activity.create():
                showActivities(manager=self.manager)
        elif ( val == '12' ):
          showTasks(manager=self.manager)
          task = self.selectInput('Enter task name [<Enter> to Cancel]:')
          if task:
            t = Activity(self.manager, task)
            if not t.exists():
              if t.create():
                showTasks(manager=self.manager)
        elif ( val == '20' ):
          partition=self.selectItem(showPartitions(manager=self.manager),Partition_t)
          if partition:
            self.drop(Partition(self.manager, partition))
        elif ( val == '21' ):
          activity=self.selectItem(showActivities(manager=self.manager),Activity_t)
          if activity:
            self.drop(Activity(self.manager, activity))
        elif ( val == '22' ):
          task = self.selectItem(showTasks(manager=self.manager),TaskType_t)
          if task:
            self.drop(TaskType(self.manager,task))

      except Exception, X:
        import sys, traceback
        print str(X), val
        info = sys.exc_info()
        lns=traceback.format_exception(info[0],info[1],info[2])
        for line in lns:
          print line
      except:
        print 'Unknown Exception on input '
        pass

# =============================================================================
def _getDevices(device_type, match, manager, load):
  mgr = manager
  _match = match
  if _match.find('*')<0:
    _match = _match+'*'
  if not mgr: mgr = Systems.controlsMgr(Params.jobopts_system_name)
  actor = DpVectorActor(mgr)
  typ   = mgr.typeMgr().type(device_type)
  actor.lookup(device_type+'_'+_match,typ)
  if load:
    rd = mgr.devReader()
    for i in actor.container: rd.add(i)
    if rd.execute(0,1):
      return actor
    return None
  return actor
# =============================================================================
def _showDevices(device_type, match, manager):
  actor = _getDevices(device_type, match, manager, 0)
  log('Found %d objects of type %s and match "%s*"'%(actor.container.size(),device_type,match),timestamp=1)
  count = 0
  for i in actor.container:
    nam = i.name()
    nam = nam[nam.find(device_type)+len(device_type)+1:]
    log('  %3d   %s'%(count,nam),timestamp=1)
    count = count + 1
  return actor
    
# =============================================================================
def showTasks(match='',manager=None):
  return _showDevices(TaskType_t,match,manager)
# =============================================================================
def showActivities(match='',manager=None):
  return _showDevices(Activity_t,match,manager)  
# =============================================================================
def showPartitions(match='',manager=None):
  return _showDevices(Partition_t,match,manager)
# =============================================================================
def tasks(match='',manager=None,load=0):
  return _getDevices(TaskType_t,match,manager,load)
# =============================================================================
def taskItems(match='*.*',manager=None,load=0):
  return _getDevices(TaskType_t,DataPoint.original(match),manager,load)
# =============================================================================
def activities(match='',manager=None,load=0):
  return _getDevices(Activity_t,match,manager,load)  
# =============================================================================
def activityItems(match='*.',manager=None,load=0):
  return _getDevices(Activity_t,DataPoint.original(match),manager,load)
# =============================================================================
def partitions(match='',manager=None,load=0):
  return _getDevices(Partition_t,match,manager,load)
# =============================================================================
def partitionItems(match='*.*',manager=None,load=0):
  return _getDevices(Partition_t,DataPoint.original(match),manager,load)
