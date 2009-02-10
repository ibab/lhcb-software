"""

     Test file for python-PVSS bridge
     This executes the basic procedures to access
     PVSS datapoints from python as well as datapoint creation
     and deletion.

     If this works, most of the stuff should be functional.....

     to execute:
     . python -c "import Online.testPVSS"

     You should see at the end:
    ================================= Basic test completed successfully. =================================
 

     @author   M.Frank
     @version  1.0
     
"""

import Online.PVSSSystems as Systems
import Online.PVSS as PVSS
DataPoint   = PVSS.DataPoint
std         = PVSS.gbl.std


class Test:
  def __init__(self, name):
    self.name = name
    print 'Starting PVSS test for system:',self.name
    self.mgr = self._mgr()
    print 'Got controls manager:',self.mgr.name()
    self.typeMgr = self.mgr.typeMgr()
    print 'Got TYPE manager:',self.mgr.name()
    self.devMgr = self._devMgr()
    print 'Got DEV manager:',self.devMgr.name()
    self.reader = self.mgr.devReader()
    print 'Got DEV READER:',self.mgr.name()

  def _mgr(self):
    mgr = Systems.controlsMgr(self.name)
    return mgr

  def _devMgr(self):
    return self.mgr.deviceMgr()

  def dp(self,name):
    """ Return datapoint in Streaming control structure
        @param internal datapoint name
      
        @return Initialized PVSS datapoint
    """
    return DataPoint(self.mgr,DataPoint.original(name))

  def test_access(self,name,comment):
    dp=self.dp(name)
    self.reader.add(dp)
    if self.reader.execute():
      print '%-12s Datapoint:%-48s %s'%(comment,dp.name(),"'"+str(dp.data)+"'")
    else:
      print '--> Could not access datapoint:',DataPoint.original(name)
      return None
    return self

  def modify_BarTrend(self):
    nam = 'test_device_01'
    actor = PVSS.DpVectorActor(self.mgr)
    typ   = self.typeMgr.type('ExampleDP_BarTrend')
    device = self.devMgr.createDevice(nam,typ,1)
    if device.get() is None:
      print '--> Failed to create device "'+nam+'" of type ',typ.name()
      return None
    print '--> Successfully created datapoint:',nam,' of type ',typ.name()
    
    actor.lookupOriginal(nam+'.*',typ)
    print 'Found %d datapoints of type %s'%(actor.container.size(),nam)
    self.reader.add(actor.container)
    if self.reader.execute():
      for i in actor.container:
        print '--> %-12s Datapoint:%-48s %d'%(nam,i.name(),i.value().type())
    else:
      print '--> Could not access datapoint:',DataPoint.original(nam)
      return None
    actor.container[0].data = std.vector('float')()
    actor.container[1].data = std.vector('float')()
    actor.container[2].data = std.vector('float')()
    actor.container[3].data = std.vector('float')()
    actor.container[4].data = std.vector('float')()
    actor.container[5].data = std.vector('float')()
    actor.container[6].data = std.vector('float')()
    actor.container[7].data = std.vector('float')()
    actor.container[8].data = std.vector('int')()
    actor.container[9].data = std.vector('int')()
    for i in xrange(10):
      f = float(i)
      actor.container[0].data.push_back(f)
      actor.container[1].data.push_back(2*f)
      actor.container[2].data.push_back(3*f)
      actor.container[3].data.push_back(4*f)
      actor.container[4].data.push_back(5*f)
      actor.container[5].data.push_back(6*f)
      actor.container[6].data.push_back(7*f)
      actor.container[7].data.push_back(8*f)
      actor.container[8].data.push_back(9*i)
      actor.container[9].data.push_back(10*i)

    wr = self.mgr.devWriter()
    wr.add(actor.container)
    if not wr.execute():
      print '--> Could not access datapoint:',DataPoint.original(nam)
      return None

    self.reader.add(actor.container)
    if self.reader.execute():
      for i in actor.container:
        print '--> %-12s Datapoint:%-48s ##Elements: %d'%(nam,i.name(),i.data.size())
    else:
      print 'Could not access datapoint:',DataPoint.original(nam)
      return None
    if not self.devMgr.deleteDevice(nam,1):
      print '--> Failed to delete datapoint:',nam,' of type ',typ.name()
      return None
    print '--> Successfully deleted datapoint:',nam,' of type ',typ.name()
    
    return self

def _check(val):
  if val is not None: return
  print ' ================================= TEST FAILED ================================='
  raise 'Test Failed'

t=Test('dist_17')
_check(t.test_access('ExampleDP_AlertHdl1','BIT'))
_check(t.test_access('ExampleDP_Arg1','FLOAT'))
_check(t.test_access('ExampleDP_SumAlert','TEXT'))
_check(t.modify_BarTrend())
print '================================= Basic test completed successfully. ================================='
