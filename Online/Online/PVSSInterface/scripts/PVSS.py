import os, sys, string
import PyCintex as PyLCGDict
#PyLCGDict.gbl.Cintex.SetDebug(1)
PyLCGDict.loadDict('PVSSInterfaceDict')
PVSS = PyLCGDict.makeNamespace('PVSS')
gbl = PyLCGDict.makeNamespace('')
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

def void_call(self): 
  return 1

def timeStamp():
  import time
  return time.ctime(time.time())

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

Sensor            = gbl.Sensor
Event             = gbl.Event
Interactor        = gbl.Interactor
IAPIManager       = PVSS.IAPIManager
Printer           = PVSS.Printer
DeviceSensor      = PVSS.DeviceSensor
DevElement        = PVSS.DevElement
DevTypeElement    = PVSS.DevTypeElement
DeviceIO      = PVSS.DeviceIO
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
DataPointVector   = gbl.std.vector('PVSS::DataPoint')
DataPointList     = gbl.std.list('PVSS::DataPoint')
#DataPointStringMap= gbl.std.map('')

def createAPIMgr(dll='',function=''):
  return PVSS.pvss_create_manager(dll, function)

def startAPIMgr(mgr):
  return mgr.start()

def controlsMgr(systemID=1,systemName='dist_1'):
  return PVSS.ControlsManager(systemID, systemName)
  
  
# DIM info handler
#class InfoHandler(DIM.InfoHandler):
#  def __init__(self):       DIM.InfoHandler.__init__(self,self)
#  def infoHandler(self):    pass

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
for i in xrange(10000):
  s='test_dev_'+str(i)+'.value'
  if dm.exists(s): 
    dps.push_back(DP(c,DP.original(s)))



print 'Found ',len(dps), ' valid devices.'
wr.start()
for i in dps:
  s='Hello darling:'+i.name()
  wr.setValue(i,s)



print 'Execute transaction:',wr.execute(1,1)


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

