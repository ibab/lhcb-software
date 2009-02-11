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
import os, sys, time, threading
import copy


import Online.PVSSSystems as Systems
import Online.PVSS as PVSS
DataPoint   = PVSS.DataPoint
std         = PVSS.gbl.std

N_TURNS = 19.0

class Future:

  def __init__(self,func,*param):
    # Constructor
    self.__done=0
    self.__result=None
    self.__status='working'
    self.__C=threading.Condition()   # Notify on this Condition when result is ready
    # Run the actual function in a separate thread
    self.__T=threading.Thread(target=self.Wrapper,args=(func,param))
    self.__T.setName("FutureThread")
    self.__T.start()

  def __repr__(self):
    return '<Future at '+hex(id(self))+':'+self.__status+'>'

  def __call__(self):
    self.__C.acquire()
    while self.__done==0:
      self.__C.wait()
    self.__C.release()
    # We deepcopy __result to prevent accidental tampering with it.
    a=copy.deepcopy(self.__result)
    return a

  def Wrapper(self, func, param):
    # Run the actual function, and let us housekeep around it
    self.__C.acquire()
    try:
      self.__result=func(*param)
    except:
      self.__result="Exception raised within Future"
    self.__done=1
    self.__status=`self.__result`
    self.__C.notify()
    self.__C.release()

class Listener(PVSS.PyDeviceListener):
  def __init__(self, manager, dp):
    self.manager = manager
    self.do_sleep= 1
    self.control = dp
    PVSS.PyDeviceListener.__init__(self,self,manager)
    self.sensor = PVSS.DeviceSensor(manager,self.control)
    print '--> Successfully created device listener for ',self.control.name()

  # ===========================================================================
  def handleInvalidDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    try:
      nam = self.dp().name()
      PVSS.error('The device '+nam+' is dead.....\n'+\
            'This should never occur and is a serious error condition\n'+\
            'We will exit the system.',timestamp=1,type=PVSS.DPNOTEXISTENT)
      self.do_sleep = 0
    except Exception,X:
      PVSS.error(str(X),timestamp=1,type=PVSS.DPNOTEXISTENT)
      traceback.print_exc()
      return 0
    except:
      PVSS.error('(Unknown exception)',timestamp=1,type=PVSS.DPNOTEXISTENT)
      traceback.print_exc()
    return 0      

  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1

  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    try:
      print '--> CHANGE:   ',self.control.name(),' Value=',self.control.data
      if self.control.data>=N_TURNS: self.do_sleep = False
    except Exception,X:
      PVSS.error('Failure:'+str(X),timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
      return 0
    except:
      PVSS.error('Failure (Unknown exception)',timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
    return 0
 
  # ===========================================================================
  def run(self):
    "Start the controls task by activating the listening devices."
    self.sensor.addListener(self)
    self.sensor.run(1)
    return self

  # ===========================================================================
  def sleep(self):
    "Serve controls requests in daemon mode"
    import sys, time
    if PVSS.batchMode(): Online.Utils.log('Sleeping ....',timestamp=1)
    else:                print 'Sleeping ....'
    sys.stdout.flush()
    try:
      while(self.do_sleep):
        time.sleep(1)        

    except Exception,X:
      print 'Exception:',str(X)
    except:
      print 'Unknown Exception'
    self.sensor.removeListener(self)
    print '--> Leaving Device listener.'

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

  def createModifyDelete(self):
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
    if not self.reader.execute():
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
      f = 0
      for i in actor.container:
        f = f + 1
        bad = False
        for j in xrange(i.data.size()):
          if i.data[j] != f*j:
            print '--> Bad data: found',i.data[j],'in',i.name(),'[',j,']  Expected:',f*j
            bad = True
        if bad:
          print '--> %-12s Datapoint:%-48s %d  ##Elem:%d Data BAD'%(nam,i.name(),i.value().type(),i.data.size())
        else:
          print '--> %-12s Datapoint:%-48s %d  ##Elem:%d Data OK'%(nam,i.name(),i.value().type(),i.data.size())
    else:
      print '--> Could not access datapoint:',DataPoint.original(nam)
      return None
    if not self.devMgr.deleteDevice(nam,1):
      print '--> Failed to delete datapoint:',nam,' of type ',typ.name()
      return None
    print '--> Successfully deleted datapoint:',nam,' of type ',typ.name()    
    return self

  def connectDp(self,name):
    dp=self.dp(name)
    ctrl = Listener(self.mgr,dp)
    ctrl.run()
    ctrl.sleep()
    return self
    
def _check(val):
  if val is not None: return
  print ' ================================= TEST FAILED ================================='
  raise 'Test Failed'

def server():
  t=Test('dist_17')
  _check(t.test_access('ExampleDP_AlertHdl1','BIT'))
  _check(t.test_access('ExampleDP_Arg1','FLOAT'))
  _check(t.test_access('ExampleDP_SumAlert','TEXT'))
  _check(t.createModifyDelete())
  A = Future(t.connectDp,'ExampleDP_Arg1')
  print '-->  Client starting.....'
  dp = t.dp('ExampleDP_Arg1')
  cnt = 1.0
  time.sleep(2)
  while(cnt<=N_TURNS):
    time.sleep(1)
    cnt = cnt + 1.0
    dp.data = cnt
    wr = t.mgr.devWriter()
    wr.add(dp)
    if not wr.execute():
      print '--> Failure modifying datapoint',dp.name()
    else:
      print '--> Modified  ',dp.name(),' to    ',dp.data
  time.sleep(2)
  print '================================= Basic test completed successfully. ================================='





if __name__ == "__main__":
  "Main entry point"
  server()
