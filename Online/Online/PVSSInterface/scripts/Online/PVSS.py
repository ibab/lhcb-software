import os, sys, time, string, platform
import PyCintex as PyLCGDict

#PyLCGDict.gbl.Cintex.SetDebug(1)
lib_prefix = ''
if platform.system()=='Linux':
  lib_prefix = 'lib'
  PyLCGDict.loadDict(lib_prefix+'PVSSInterfaceDict')
PyLCGDict.loadDict(lib_prefix+'STLRflx')

gbl  = PyLCGDict.makeNamespace('')
PVSS = PyLCGDict.makeNamespace('PVSS')
  
# == External class definitions ===============================================
Sensor            = gbl.Sensor
Event             = gbl.Event
gbl  = PyLCGDict.makeNamespace('')
PVSS = PyLCGDict.makeNamespace('PVSS')
Interactor        = gbl.Interactor
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
DataPoint         = PVSS.DataPoint
def DataPoint_get(self):
  return self.value().data()
def DataPoint_set(self,value):
  import traceback
  #print 'DataPoint_set:',value.__class__
  if isinstance(value,list) or isinstance(value,tuple):
    if len(value) > 0:
      traceback.print_stack()
      print 'DataPoint_set_item:',value.__class__,value[0].__class__
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
DataPoint.data = property(DataPoint_get,DataPoint_set)

def debug():
  return PVSS.pvss_debug()
def setDebug(val):
  PVSS.pvss_set_debug(val)
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

# =============================================================================
def defaultSystem():
  return (PVSS.defaultSystemID(),PVSS.defaultSystemName())
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
    atty = os.isatty(sys.stdout.fileno())
    if atty and platform.system()=='Linux':
      name_stdout = os.ttyname(sys.stdout.fileno())
      name_stderr = os.ttyname(sys.stdout.fileno())
    
    apiManager = PVSS.pvss_create_manager('Python'+str(os.getpid()),dll, function)
    result = apiManager.start()
    if atty and platform.system()=='Linux':
      print 'Online.PVSS> Running in interactive mode....'
      fd_stdout = os.open(name_stdout,os.O_WRONLY)
      fd_stderr = os.open(name_stderr,os.O_WRONLY)
      os.dup2(fd_stdout,sys.stdout.fileno())
      os.dup2(sys.stderr.fileno(),fd_stderr)
    elif platform.system()=='Linux':
      print 'Online.PVSS> Running in batch mode....'
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

def printErrorCodes():
  for i in xrange(110):
    logger.info(i, 'Test message')

# Printout to logger window
def log(severity, type, message):
  logger.log(severity, type, message)

# Informational printout to logger window
def info(message,timestamp=None,type=default_error_type):
  import Online.Utils
  Online.Utils.log(message,timestamp=timestamp)
  logger.info(type, message)

# Warning printout to logger window
def warning(message,timestamp=None,type=default_error_type):
  import Online.Utils
  Online.Utils.warning(message,timestamp=timestamp)
  logger.warning(type, message)

# Sever error printout to logger window
def error(message,timestamp=None,type=default_error_type):
  import Online.Utils
  Online.Utils.error(message,timestamp=timestamp)
  logger.error(type, message)

# Fatal printout to logger window. kill the program instance!
def fatal(message,timestamp=None,type=default_error_type):
  import Online.Utils
  Online.Utils.error(message,timestamp=timestamp)
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
    from Online.Utils import log, error
    cmd = ''
    try:
      nam = self.dp().name()
      cmd = self.dp().value().data()
      log('Command received:'+nam[:nam.find(':')]+' -> '+cmd,timestamp=1)
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
