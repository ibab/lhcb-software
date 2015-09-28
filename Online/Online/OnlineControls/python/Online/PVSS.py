import os, sys, time, string, platform
import Online.Utils as Utils
PyLCGDict = Utils.Dictionary

if platform.system()=='Linux':
  PyLCGDict.loadDict(Utils.lib_prefix+'PVSSDict')

gbl  = Utils.gbl
PVSS = PyLCGDict.makeNamespace('PVSS')

def batchMode():
  """
  Helper function to determine if the process executes in batch mode or interactive mode.
  The distinction is done by checking the availability of a terminal as output device.
  """
  return not os.isatty(sys.stdout.fileno())

# == External class definitions ===============================================
Sensor            = gbl.CPP.Sensor
Event             = gbl.CPP.Event
Interactor        = gbl.CPP.Interactor
IAPIManager       = PVSS.IAPIManager
Printer           = PVSS.Printer
DeviceSensor      = PVSS.DeviceSensor
DevElement        = PVSS.DevElement
DevTypeElement    = PVSS.DevTypeElement
DeviceIO          = PVSS.DeviceIO
ReadTransaction   = PVSS.ReadTransaction
CfgDetail         = PVSS.CfgDetail
CfgAttribute      = PVSS.CfgAttribute
NamedIdentified   = PVSS.NamedIdentified
BitAlarm          = PVSS.BitAlarm
BasicAlarm        = PVSS.BasicAlarm
ArchiveSetting    = PVSS.ArchiveSetting
CfgSetting        = PVSS.CfgSetting
WriteTransaction  = PVSS.WriteTransaction
DpID              = PVSS.DpID
DPTime            = PVSS.DPTime
DPRef             = PVSS.DPRef
Value             = PVSS.Value
ControlsManager   = PVSS.ControlsManager
Environment       = PVSS.Environment
DevAnswer         = PVSS.DevAnswer
CfgType           = PVSS.CfgType
CfgManager        = PVSS.CfgManager
DevType           = PVSS.DevType
DevTypeManager    = PVSS.DevTypeManager
DeviceManager     = PVSS.DeviceManager
HotLinkCallback   = PVSS.HotLinkCallback
PyDeviceListener  = PVSS.PyDeviceListener
DpVectorActor     = PVSS.DataPointContainerActor('std::vector<PVSS::DataPoint>')
DpListActor       = PVSS.DataPointContainerActor('std::list<PVSS::DataPoint>')
# == External function definitions =============================================
defaultSystemID   = PVSS.defaultSystemID
defaultSystemName = PVSS.defaultSystemName
# == STL container definitions =================================================
DpIDVector        = gbl.std.vector('PVSS::DpID')
DataPointVector   = gbl.std.vector('PVSS::DataPoint')
DataPointVectorP  = gbl.std.vector('PVSS::DataPoint*')
DataPointList     = gbl.std.list('PVSS::DataPoint')
DataPointIdMap    = gbl.std.map('PVSS::DpID','PVSS::DataPoint*')
DataPointStringMap= gbl.std.map('std::string','PVSS::DataPoint*')
StringVector      = gbl.std.vector('std::string')
IntVector         = gbl.std.vector('int')
#
#
DataPoint         = PVSS.DataPoint
DP                = PVSS.DataPoint
#
def DataPoint_get(self):
  "Datapoint property function: get"
  try:
    return self.value().data()
  except Exception,X:
    print 'Data point: ',self.name()
    print '     value: ',self.value()
    if self.value() is not None:
      print '     type:  ',self.value().type()
      
    raise X

def DataPoint_set(self,value):
  "Datapoint property function: set"
  import traceback
  #print 'DataPoint_set:',value.__class__
  try:
    if isinstance(value,list) or isinstance(value,tuple):
      if len(value) > 0:
        #traceback.print_stack()
        #print 'DataPoint_set_item:',value.__class__,value[0].__class__
        if isinstance(value[0],int):
          v = gbl.std.vector('int')()
          for i in value: v.push_back(i)
          self.set(v)
          return self
        elif isinstance(value[0],float):
          v = gbl.std.vector('float')()
          for i in value: v.push_back(i)
          self.set(v)
          return self
        elif isinstance(value[0],str):
          v = gbl.std.vector('std::string')()
          for i in value: v.push_back(i)
          self.set(v)
          return self
        elif isinstance(value[0],DataPoint):
          v = gbl.std.vector('PVSS::DataPoint')()
          for i in value: v.push_back(i)
          self.set(v)
          return self
        elif isinstance(value[0],DpID):
          v = gbl.std.vector('PVSS::DpID')()
          for i in value: v.push_back(i)
          self.set(v)
          return self
      else:
        try:
          self.value().data().clear()
        except:
          pass
    self.set(value)
    return self
  except Exception, X:
    print 'Failed to set datapoint:',self.name(),' value:',str(value)
    raise X
    return None
  except:
    print 'Failed to set datapoint:',self.name(),' value:',str(value)
    return None
DataPoint.data = property(DataPoint_get,DataPoint_set)

# =============================================================================
def debug():
  "Access PVSS debug flag"
  return PVSS.pvss_debug()

# =============================================================================
def setDebug(val):
  "Set PVSS debug flag"
  PVSS.pvss_set_debug(val)

# =============================================================================
def system(name=None):
  "Access PVSS System name and identifier"
  if name is not None:
    id = PVSS.systemID(name)
    if id != 0:
      return (id,name)
    raise Exception,'Exception: Unknown system:'+str(name)
  return (PVSS.defaultSystemID(),PVSS.defaultSystemName())

# =============================================================================
def defaultSystem():
  "Access PVSS default system"
  return (PVSS.defaultSystemID(),PVSS.defaultSystemName())

# =============================================================================
def defaultSystemName():
  "Access PVSS default system name"
  return PVSS.defaultSystemName()

# =============================================================================
def defaultSystemID():
  "Access PVSS default system ID"
  return PVSS.defaultSystemID()

# =============================================================================
class APIManager:
  """
      Wrapper around PVSS APIManager instance.
      Should never be directly instantiated by the user
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self,dll='',function=''):
    "Create PVSS API manager for python."
    interactive = not batchMode()
    if interactive and platform.system()=='Linux':
      name_stdout = os.ttyname(sys.stdout.fileno())
      name_stderr = os.ttyname(sys.stdout.fileno())
    
    apiManager = PVSS.pvss_create_manager('Python'+str(os.getpid()),dll, function)
    result = apiManager.start()
    if interactive and platform.system()=='Linux':
      fd_stdout = os.open(name_stdout,os.O_WRONLY)
      fd_stderr = os.open(name_stderr,os.O_WRONLY)
      os.dup2(fd_stdout,sys.stdout.fileno())
      os.dup2(fd_stderr,sys.stderr.fileno())
      print 'PVSS> Running in interactive mode....'
    elif platform.system()=='Linux':
      print 'PVSS> Running in batch mode....'
    return result

# Instantiate API manager. Should never be called by user directly
apiManager = APIManager()
logger     = PVSS.createLogger()

CONNECTED           =  4        # Connected to
CANT_CONNECT        =  5        # Connection not possible
DPNOTEXISTENT       =  7        # Datapoint does not exist
ELNOTEXISTENT       =  8        # Element does not exist
OUTOFUSERRANGE      = 30        # value out of user range
DM                  = 33        # common error in data manager
SYNTAX              = 36        # DM: Incorrect name syntax
NOTEXISTINGADDRESS  = 49        # Error when writing! Wrong address?
PARAMETERERROR      = 51        # Parameter incorrect
ILLEGALFUNCALL      = 53        # Illegal function call
UNEXPECTEDSTATE     = 54        # Unexpected state
TYPENOTEXISTENT     = 57        # unknown datapoint type
ELEMENTNOTEXISTENT  = 59        # unknoen datapoint element
FILEOPEN            = 61        # cannot open file
DP_NOT_EXISTENT     = 71        # DP does not exist
UNDEFD_FUNC         = 72        # Function not defined
ARG_MISSING         = 75        # Argument missing in function
ILLEGAL_ARG         = 76        # Invalid argument in function
SYNTAX_ERROR        = 81        # Syntax error
FILE_NOT_OPEN       = 82        # File not open
ILLEGAL_VALUE       = 86        # Invalid value
ILLEGAL_OP          = 88        # Operation not permitted with these arguments
ILLEGAL_KEYWORD     = 95        # Unknown keyword in resource
default_error_type  = ILLEGAL_VALUE

# =============================================================================
def printErrorCodes():
  "Print all PVSS error codes."
  for i in xrange(110):
    logger.info(i, 'Test message')

# =============================================================================
def log(severity, type, message):
  "Print message to PVSS logger device."
  logger.log(severity, type, message)

# =============================================================================
def info(message,timestamp=None,type=default_error_type):
  "Informational printout to PVSS logger window"
  Utils.log(message,timestamp=timestamp)
  logger.info(type, message)

# =============================================================================
def warning(message,timestamp=None,type=default_error_type):
  "Warning printout to PVSS logger window"
  Utils.warning(message,timestamp=timestamp)
  logger.warning(type, message)

# =============================================================================
def error(message,timestamp=None,type=default_error_type):
  "Sever error printout to logger window"
  Utils.error(message,timestamp=timestamp)
  logger.error(type, message)

# =============================================================================
def fatal_banner(message,timestamp=None,type=default_error_type):
  "Sever error printout to logger window"
  Utils.fatal(132*'*',timestamp=timestamp)
  Utils.fatal('** ',timestamp=timestamp)
  if isinstance(message,str):
    Utils.fatal('** '+message,timestamp=timestamp)
  elif isinstance(message,list) or isinstance(message,tuple):
    for m in message:
      Utils.fatal('** '+m,timestamp=timestamp)
  else:
    Utils.fatal('** '+str(message),timestamp=timestamp)
  Utils.fatal('** ',timestamp=timestamp)
  Utils.fatal(132*'*',timestamp=timestamp)
  if isinstance(message,str):
    logger.error(type, message)
  elif isinstance(message,list) or isinstance(message,tuple):
    for m in message:
      logger.error(type, m)
  else:
    logger.error(type, str(message))

# =============================================================================
def fatal(message,timestamp=None,type=default_error_type):
  "Fatal printout to logger window. kill the program instance!"
  Utils.fatal(message,timestamp=timestamp)
  logger.fatal(type, message)

# =============================================================================
def controlsMgr(systemID=None,systemName=None):
  "Create controls manager."
  if systemID is None: systemID = PVSS.defaultSystemID()
  if systemName is None: systemName = PVSS.defaultSystemName()
  return PVSS.ControlsManager(systemID, systemName)

# =============================================================================
class DeviceListener(PyDeviceListener):
  """ 
      Standard Python PVSS DeviceListener
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager):
    "Standard constructor: needs ControlsManager as argument "
    self.manager = manager
    PyDeviceListener.__init__(self, self, self.manager)

  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1

  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    return 1
  
# =============================================================================
class DevicePrintListener(PyDeviceListener):
  """ 
      Python PVSS DeviceListener, which prints the content of the 
      sensor's datapoint list
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager):
    "Standard constructor: needs ControlsManager as argument "
    PyDeviceListener.__init__(self,self, manager)

  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    d = self.devArray()
    print 'Devices changed:',d.size()
    for i in d:
      print '  ->',i.name(), '=', i.value().data()
    return 1
  
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    print 'Device changed:', self.dp().name(), '=', self.dp().value().data()
    return 1
  
# =============================================================================
class CommandListener(PyDeviceListener):
  """ 
      Python PVSS DeviceListener, which prints the content of the 
      sensor's datapoint list
      
      @author M.Frank
  """
  # ===========================================================================
  def __init__(self, manager, sysname, datapoint, npar, cmd='Command', state='State'):
    "Standard constructor: needs ControlsManager as argument "
    PyDeviceListener.__init__(self,self, manager)
    self.manager  = manager
    self.sysname  = sysname
    self.numParam = npar
    self.writer   = self.manager.devWriter()
    self.control  = DataPoint(self.manager,DataPoint.original(datapoint+'.'+cmd))
    self.state    = DataPoint(self.manager,DataPoint.original(datapoint+'.'+state))
    self.sensor   = DeviceSensor(self.manager,self.control)
    info(sysname+': Listen to '+self.control.name(),1)
    info(sysname+': Answer to '+self.state.name(),1)

  # ===========================================================================
  def run(self):
    "Run the instance and listen to commands."
    self.sensor.addListener(self)
    self.sensor.run(1)
    
  # ===========================================================================
  def makeAnswer(self,status,msg):
    "Create answer object from status."
    self.state.data = status + msg
    self.writer.add(self.state)
    if self.writer.execute(0,1):
      return 1
    return 0
  
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1

  # ===========================================================================
  def handle(self,items):
    "Dummy routine to be overloaded by clients."
    pass
  
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    cmd = ''
    try:
      nam = self.dp().name()
      cmd = self.dp().value().data()
      Utils.log('Command received:'+nam[:nam.find(':')]+' -> '+cmd,timestamp=1)
      itms = cmd.split('/')
      if len(itms) == self.numParam:
        command   = itms[0]
        sysName   = itms[1]
        answer = ''
        for i in xrange(len(itms)):
          if i > 0: answer = answer + '/' + itms[i]
        result = None
        if sysName == self.sysname:
          try:
            result = self.handle(itms, answer)
            if result is None:
              error('The command:"'+cmd+'" failed. [Internal Error] ',timestamp=1)
              return self.makeAnswer('ERROR',answer)
            return result
          except Exception,X:
            error('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
          except:
            error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
        error('The command:"'+cmd+'" failed. [Bad System] '+sysName+' <> '+self.sysname,timestamp=1)
        return self.makeAnswer('ERROR',answer)
      error('The command:"'+cmd+'" failed. [Insufficient parameters] ',timestamp=1)
      return 0
    except Exception,X:
      error('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
      traceback.print_exc()
      return 0
    except:
      error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
      traceback.print_exc()
    return 0

# =============================================================================
class Container:
  # ===========================================================================
  def __init__(self,manager,name):
    self.name       = name
    self.manager    = manager
    self.datapoints = Online.PVSS.DataPointVector()
    
  # ===========================================================================
  def dp(self,name):
    return DataPoint(self.manager,DataPoint.original(self.name+'.'+name))

  # ===========================================================================
  def clear(self):
    "Clear internal datapoint cache"
    self.datapoints.clear()
    
  # ===========================================================================
  def set(self, dp, value):
    "Set datapoint value in storage definition"
    self.datapoints.push_back(self.dp(dp))
    self.datapoints.back().data = value
    return self
