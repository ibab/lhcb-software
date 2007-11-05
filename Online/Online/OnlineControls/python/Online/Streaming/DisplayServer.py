import os, sys, time
import Online.PVSS as PVSS
import Online.Utils

log = Online.Utils.log

class SliceDisplaySrv(PVSS.PyDeviceListener):
  # ===========================================================================
  def __init__(self,manager,name,match='*'):
    self.manager = manager
    self.name = name
    self._init()
    PVSS.PyDeviceListener.__init__(self,self,manager)
    log('Starting display server for slice:'+self.name+' on system:'+self.manager.name(),timestamp=1)

  # ===========================================================================
  def _init(self):
    typ1          = self.manager.typeMgr().type('_FwFsmDevice')
    typ2          = self.manager.typeMgr().type('_FwFsmObject')
    self.actions  = self._fsmLookup('fsm.executingAction',typ1,typ2)
    self.states   = self._fsmLookup('fsm.currentState',typ1,typ2)
    self.commands = self._fsmLookup('fsm.sendCommand',typ1,typ2)
    self.enabled  = self._fsmLookup('mode.enabled',typ1,typ2)
    self.tnodes   = self._fsmLookup('tnode',typ1,typ2)
    self.fsmtypes = self._fsmLookup('type',typ1,typ2)

    self.writer = self.manager.devWriter()
    nam = self.name.replace('_Slice','Display_Slice')
    self.taskData = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(nam+'.Tasks'))
    self.writer.add(self.taskData)
    self.sensor = PVSS.DeviceSensor(self.manager,self.actions)
    self.taskData.data = PVSS.StringVector()
    
  # ===========================================================================
  def _fsmLookup(self,dp,typ1,typ2):
    match = '_*.'
    nam = self.name+'|'+self.name
    obj = PVSS.DpVectorActor(self.manager)
    obj.lookupOriginal(nam+'_*.'+dp,typ2)
    obj.lookupOriginal(nam+'_*.'+dp,typ1)
    v = PVSS.DataPointVector()
    cfg = 'blabla'
    for i in obj.container:
      if i.name().find(nam+'_Config')>0:
        cfg = i.name()
        v.push_back(PVSS.DataPoint(self.manager,i.name()))
        break
    for i in obj.container:
      if i.name().find('_FWM') > 0: continue
      if i.name().find('_FWDM') > 0: continue
      if i.name().find('_FWCNM') > 0: continue
      if i.name() != cfg:
        v.push_back(PVSS.DataPoint(self.manager,i.name()))
    return v
  # ===========================================================================
  def collectInfo(self):
    rdr = self.manager.devReader()
    rdr.add(self.tnodes)
    rdr.add(self.states)
    rdr.add(self.enabled)
    rdr.add(self.fsmtypes)
    rdr.add(self.commands)
    if rdr.execute():
      tasks = {}
      for j in xrange(len(self.enabled)):
        try:
          i = self.enabled[j]
          enabled = i.data
          if enabled>0:
            nam = i.name()
            idx = nam.find(':')
            sys = nam[:idx]
            slice = nam[idx+1:nam.find('|')]
            state = self.states[j].data
            action = self.actions[j].data
            cmd = self.commands[j].data
            fsm = i.name()[:i.name().find('.')]
            fsm_dp = self.tnodes[j].data
            typ = self.fsmtypes[j].data 
            if typ == 'FSM_DimTask':
              task = nam[idx+2*len(slice)+3:nam.find('.')]
              node = task[:task.find('_')]
              #print sys,slice,node,task,state,'"'+action+'"',cmd,sys+':'+slice+'_'+task
              data = sys+'#'+slice+'#'+node+'#'+task+'#'+state+'#'+action+'#'+cmd+'#'+fsm_dp+'#'+fsm
            elif typ == 'FSM_Tasks':
              task = nam[idx+2*len(slice)+3:]
              node = task[:task.find('.')]
              task = 'FSM_Tasks'
              data = sys+'#'+slice+'#'+node+'#'+task+'#'+state+'#'+action+'#'+cmd+'#'+fsm_dp+'#'+fsm
            elif typ == 'StreamConfigurator':
              node = slice
              data = sys+'#'+slice+'#'+slice+'#StreamConfigurator#'+state+'#'+action+'#'+cmd+'#'+fsm_dp+'#'+fsm
            else:
              #print typ,fsm
              continue
            if not tasks.has_key(node): tasks[node] = []
            tasks[node].append(data)
        except Exception,X:
          print X
      self.taskData.data.clear()
      for k,task_list in tasks.items():
        for t in task_list:
          self.taskData.data.push_back(t)
      self.writer.execute(1)
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    #print 'Some datapoints changed!'
    self.collectInfo()
    return 1
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    return 1
  # ===========================================================================
  def run(self):
    "Start the controls task by activating the listening devices."
    self.sensor.addListener(self)
    self.sensor.run(1)
    return self
  # ===========================================================================
  def stop(self):
    "Start the controls task by activating the listening devices."
    self.sensor.removeListener(self)
    self.sensor = None
    self.writer = None
    self.taskData = None
    return self

class DisplayManager(PVSS.PyDeviceListener):
  # ===========================================================================
  def __init__(self,manager):
    self.manager = manager
    self.do_sleep= 1
    typ = self.manager.typeMgr().type('StreamPartition')
    self.slices = PVSS.DpVectorActor(self.manager)
    self.slices.lookupOriginal('*_Slice??.InUse',typ)
    self.sensor = PVSS.DeviceSensor(self.manager,self.slices.container)
    PVSS.PyDeviceListener.__init__(self,self,manager)
    log('Starting display server on system:'+self.manager.name(),timestamp=1)
    self.servers = {}
    for i in self.slices.container:
      self.servers[i.name()] = None
    rdr = self.manager.devReader()
    rdr.add(self.slices.container)
    rdr.execute()
    self.handleDevices()

  # ===========================================================================
  def run(self):
    "Start the controls task by activating the listening devices."
    self.sensor.addListener(self)
    self.sensor.run(1)
    return self
  
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    return 1
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    for i in self.slices.container:
      nam = i.name()
      if i.data > 0 and self.servers[nam] is None:
        n = nam[nam.find(':')+1:]
        n = n[:n.find('.')]
        srv = SliceDisplaySrv(self.manager,n).run()
        self.servers[nam] = srv
        log('Started display server:'+self.servers[nam].name,timestamp=1)
      elif i.data<=0 and self.servers[nam] is not None:
        log('Stop display server:'+self.servers[nam].name,timestamp=1)
        self.servers[nam].stop()
        self.servers[nam] = None
    return 1
  
  # ===========================================================================
  def sleep(self):
    "Serve controls requests in daemon mode"
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
    
if __name__=="__main__":
  mgr = PVSS.controlsMgr()
  mgr = DisplayManager(mgr)
  mgr.run().sleep()
