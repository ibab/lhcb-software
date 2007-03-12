import os, sys, string
import PyCintex as PyLCGDict
#PyLCGDict.gbl.Cintex.SetDebug(1)
#PyLCGDict.loadDict('PVSSInterfaceDict')
PVSS = PyLCGDict.makeNamespace('PVSS')
gbl  = PyLCGDict.makeNamespace('')
PyLCGDict.loadDict('STLRflx')

logPrefix     = ''
logHeader     = '+----------------------------------------------------------------------------------------------------'
logTrailer    = logHeader

#----enable tab completion---------------------------------------------------------------
try:
  import rlcompleter,readline    
  readline.parse_and_bind("tab: complete")
except:
  pass
# =============================================================================
def timeStamp():
  import time
  return time.ctime(time.time())
# =============================================================================
def log(msg, with_header=0, with_trailer=0):
  if ( with_header != 0 ):
    print logPrefix + ' ' + timeStamp() + ' ' + logHeader
  if msg.__class__ == str().__class__:
    print logPrefix + ' ' + timeStamp() + ' |  ' + msg
  else:
    for line in msg:
      print logPrefix + ' ' + timeStamp() + ' |  ' + line
  if ( with_trailer != 0 ):
    print logPrefix + ' ' + timeStamp() + ' ' + logTrailer
# =============================================================================
def error(msg):
  log('Error:   '+msg)
# =============================================================================
def warning(msg):
  log('Warning: '+msg)
  
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
DataPoint         = PVSS.DataPoint
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
DpVectorActor     = PVSS.DataPointContainerActor('std::vector<PVSS::DataPoint>')
DpListActor       = PVSS.DataPointContainerActor('std::list<PVSS::DataPoint>')

DpIDVector        = gbl.std.vector('PVSS::DpID')
DataPointVector   = gbl.std.vector('PVSS::DataPoint')
DataPointVectorP  = gbl.std.vector('PVSS::DataPoint*')
DataPointList     = gbl.std.list('PVSS::DataPoint')
DataPointIdMap    = gbl.std.map('PVSS::DpID','PVSS::DataPoint*')
DataPointStringMap= gbl.std.map('std::string','PVSS::DataPoint*')
StringVector      = gbl.std.vector('std::string')
IntVector         = gbl.std.vector('int')

# =============================================================================
def createAPIMgr(dll='',function=''):
  "Create PVSS API manager for python."
  return PVSS.pvss_create_manager(dll, function)

# =============================================================================
def startAPIMgr(mgr):
  "Start PVSS API manager for python."
  return mgr.start()

# =============================================================================
def controlsMgr(systemID=None,systemName=None):
  "Create controls manager."
  if systemID is None: systemID = PVSS.defaultSystemID()
  if systemName is None: systemName = PVSS.defaultSystemName()
  return PVSS.ControlsManager(systemID, systemName)

# =============================================================================
class DeviceListener(PVSS.PyDeviceListener):
  """ 
      Standard Python PVSS DeviceListener
      
      @author M.Frank
  """
  def __init__(self, manager):
    "Standard constructor: needs ControlsManager as argument "
    self.manager = manager
    PVSS.PyDeviceListener.__init__(self, self, self.manager)

  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    pass

  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    pass
  
# =============================================================================
class DevicePrintListener(DeviceListener):
  """ 
      Python PVSS DeviceListener, which prints the content of the 
      sensor's datapoint list
      
      @author M.Frank
  """
  def __init__(self, manager):
    "Standard constructor: needs ControlsManager as argument "
    PVSS.PyDeviceListener.__init__(self,self, manager)
   
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    d = self.devArray()
    print 'Devices changed:',d.size()
    for i in d:
      print '  ->',i.name(), '=', i.value().data()

  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    print 'Device changed:', self.dp().name(), '=', self.dp().value().data()
  
"""

import time, PVSS
DP=PVSS.DataPoint
m=PVSS.createAPIMgr()
m.start()
c=PVSS.ControlsManager(1,'dist_1')
rdr=c.devReader()
dp=DP(c,DP.online('JobOptionsPartition_LHCb.Command'))
dp.name()
rdr.add(dp)
rdr.execute()
print 'Got value:',dp.value().data()

dm=c.deviceMgr()
dtm=c.typeMgr()
t=dtm.type('TestType')
print t.id()

dps = PVSS.DataPointVector()
for i in xrange(100):
  s='test_dev_'+str(i)+'.value'
  if dm.exists(s):
    d = DP(c,DP.original(s))
    d.setFlag(1,'a')
    dps.push_back(d)



print 'Found ',len(dps), ' valid devices.'

# Device sensor tests:
sensor = PVSS.DeviceSensor(c,dps)
listener = PVSS.DeviceListener(c)
sensor.addListener(listener)
sensor.run(1)


wr=c.devWriter()
for i in dps:
  i.set('ddddd')
  wr.add(i)


rdr.clear()
for i in dps:
  i.set('---------------------')
  rdr.add(i)

rdr.execute(1,1)

wr.execute(1,1)

wr=c.writeTransaction()
wr.start()
for i in dps:
  s='Hello darling:'+i.name()
  wr.setValue(i,s)



print 'Execute transaction:',wr.execute(1,1)

wrt=c.devWriter()
for i in dps:
  i.set('471896454674396574365784Hello darling:'+i.name())
  wrt.add(i)



wrt.execute()



"""

def createDevs(dm,t):
  d = 0
  for i in xrange(10000):
    s='test_dev_'+str(i)
    if ( not dm.exists(s) ):
      d=dm.createDevice(s,t,1)
    if i%100==0: print 'created ',i

def delDevs(dm):
  d = 0
  for i in xrange(10000):
    s='test_dev_'+str(i)
    if dm.exists(s):
      d=dm.deleteDevice(s,1)
    if i%100==0: print 'Deleted ',i

def setVals(many,c,dm,wr):
  DP=PVSS.DataPoint
  dps = []
  for i in xrange(many):
    s='test_dev_'+str(i)+'.value'
    if dm.exists(s): 
      dps.append(DP(c,DP.original(s)))
  print 'Found ',len(dps), ' valid devices.'
  wr.start()
  for i in dps:
    s='Hello darling:'+i.name()
    wr.setValue(i,s)
  print 'Execute transaction:',wr.execute(1)

