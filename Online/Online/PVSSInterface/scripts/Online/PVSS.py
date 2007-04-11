import os, sys, time, string
import PyCintex as PyLCGDict

#PyLCGDict.gbl.Cintex.SetDebug(1)
#PyLCGDict.loadDict('PVSSInterfaceDict')
PVSS = PyLCGDict.makeNamespace('PVSS')
gbl  = PyLCGDict.makeNamespace('')
PyLCGDict.loadDict('STLRflx')

# == External class definitions ===============================================
Sensor            = gbl.Sensor
Event             = gbl.Event
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
    apiManager = PVSS.pvss_create_manager(dll, function)
    return apiManager.start()
# Instantiate API manager. Should never be called by user directly
apiManager = APIManager()

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
    pass
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    pass
  
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
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    print 'Device changed:', self.dp().name(), '=', self.dp().value().data()
  
