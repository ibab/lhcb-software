import os, sys, string
import PyCintex as PyLCGDict
#PyLCGDict.gbl.Cintex.SetDebug(1)
PyLCGDict.loadDict('PVSSInterfaceDict')
PVSS = PyLCGDict.makeNamespace('PVSS')
gbl  = PyLCGDict.makeNamespace('')
PyLCGDict.loadDict('STLRflx')

TestVector   = gbl.std.vector('PVSS::TestObject')
print TestVector().__class__
TestVectorP  = gbl.std.vector('PVSS::TestObject*')
print TestVectorP().__class__
TestMap      = gbl.std.map('int','PVSS::TestObject')
print TestMap().__class__
TestMapP     = gbl.std.map('int','PVSS::TestObject*')
print TestMapP().__class__


"""
import time, PVSS
std=PVSS.gbl.std
o=PVSS.PVSS.TestVector()
v=std.vector('char')()
o.set(v)
v=std.vector('int')()
o.set(v)
v=std.vector('float')()
o.set(v)
v=std.vector('double')()
o.set(v)

o.i_set(v)


import time, PVSS
m=PVSS.createAPIMgr()
m.start()
c=PVSS.controlsMgr()
DP=PVSS.DataPoint
rdr=c.devReader()
dp=DP(c,DP.online('dummy_RunInfo.nTriggers'))
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

"""
