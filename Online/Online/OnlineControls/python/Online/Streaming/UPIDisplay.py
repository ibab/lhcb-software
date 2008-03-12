import os, sys, time, socket
import Online.PVSS as PVSS
import Online.Utils
import UPI as upi

log = Online.Utils.log
timeStamp = Online.Utils.timeStamp
menus = {}
CMD_UPDATE = 2
CMD_INCREMENTAL = 3
CMD_QUIT   = 4
_timeSensor = upi.gbl.TimeSensor.instance()
_iocSensor  = upi.gbl.IocSensor.instance()

mk_timestamps = not PVSS.batchMode()

def targetName(self):
  return self.name.replace('_Slice','Display_Slice')+'.Tasks'
def targetHLT(self):
  return PVSS.defaultSystemName()+'_FSMDisplay.Tasks'

class ReallyClose(upi.Interactor):
  # ===========================================================================
  def __init__(self,parent):
    upi.Interactor.__init__(self,self)
    self.id = 12345+upi.sensor().newID()
    self.parent = parent
    self.CMD_QUIT = 5
    self.CMD_NOQUIT = 6
    upi.open_window()
    upi.open_detached_menu(self.id,0,0,'Confirm','Really quit the application?','Really quit the application?')
    upi.add_comment(8,'-------------------------------------------------')
    upi.add_comment(1,'  Please be very careful:                        ')
    upi.add_comment(2,'  -- this is a PVSS application.                 ')
    upi.add_comment(3,'  -- it should only be stopped from the console. ')
    upi.add_comment(4,'-------------------------------------------------')
    upi.add_command(5,'.                    YES                        .')
    upi.add_command(6,'.                    NO                         .')
    upi.add_comment(7,'-------------------------------------------------')
    upi.close_menu()
    upi.set_cursor(self.id,6,0)
    upi.sensor().add(self,self.id)

  # ===========================================================================
  def __del__(self):
    #log('Deleting menu '+str(self.id),timestamp=mk_timestamps)    
    upi.sensor().remove(self,self.id)
    upi.delete_menu(self.id)
    
  # ===========================================================================
  def handleEvent(self):
    ev = self.event()
    if ev.eventtype == upi.EventType:
      if ev.command_id==6:
        upi.set_cursor(self.parent.mid,CMD_QUIT,0)
        self.parent.reallyClose=None
      elif ev.command_id==5:
        upi.set_cursor(self.parent.mid,CMD_QUIT,0)
        _iocSensor.send(self.parent,CMD_QUIT,CMD_QUIT)
        self.parent.reallyClose=None

class Display(upi.Interactor):
  # ===========================================================================
  def __init__(self,manager,name,target=targetName,standalone=None):
    self.manager = manager
    self.name = name
    self.lines = CMD_QUIT
    self.incremental = True
    self.reader = self.manager.devReader()
    self.dp = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(target(self)))
    self.reader.add(self.dp)
    log('Starting display for slice:'+self.name+' on system:'+self.manager.name(),timestamp=mk_timestamps)
    upi.Interactor.__init__(self,self)
    self.mid = len(menus)+1
    menus[self.mid] = self
    upi.open_window()
    upi.open_menu(self.mid,0,0,timeStamp(),name,self.manager.name())
    upi.add_command(CMD_UPDATE,'Update')
    upi.add_command(CMD_INCREMENTAL,'Show full status')
    upi.add_command(CMD_QUIT,'Close')
    upi.close_menu()
    self.updateDisplay()
    upi.set_cursor(self.mid,CMD_UPDATE,0)
    upi.sensor().add(self,self.mid)
    self.isAlive = 1
    self.standalone = standalone
    #if self.standalone:
    #  _timeSensor.add(self,10,0)

  # ===========================================================================
  def handleEvent(self):
    ev = self.event()
    if ev.eventtype == upi.EventType:  # UpiEvent
      if ev.command_id==CMD_QUIT:
        if self.standalone:
          _iocSensor.send(self,CMD_QUIT,CMD_QUIT)
        else:
          self.stop()
      elif ev.command_id==CMD_INCREMENTAL:
        self.incremental = not self.incremental
        if self.incremental:
          upi.replace_command(self.mid,CMD_INCREMENTAL,'Show full status')
        else:
          upi.replace_command(self.mid,CMD_INCREMENTAL,'Show incremental status')
        log(self.name+'> Incremental status is now:'+str(self.incremental),timestamp=mk_timestamps)
        self.updateDisplay()
      elif ev.command_id==CMD_UPDATE:
        self.updateDisplay()
      elif ev.menu_id == 0 and ev.command_id == 0 and ev.param_id == 0:
        upi.refresh_screen()
      else:
        log(self.name+'> UPI Event arrived.')
        self.updateDisplay()
    elif ev.eventtype == 0:            # TimeEvent
      log(self.name+'> Time event arrived')
      self.updateDisplay()
      if self.standalone:
        _timeSensor.add(self,10,0)
    elif ev.eventtype == 3:            # IocEvent
      if ev.iocType() == CMD_UPDATE:
        self.updateDisplay()
      elif ev.iocType() == CMD_QUIT:
        upi.quit()
        upi.resetANSI()
        sys.exit(0)
      else:
        log(self.name+'> Unknown IOC event arrived'+str(ev.type)+' '+str(ev.iocType())+' '+str(ev.iocData()),timestamp=mk_timestamps)
    else:
      log(self.name+'> Unknown event of type '+str(ev.eventtype)+' arrived')

  # ===========================================================================
  def update(self):
    _iocSensor.send(self,CMD_UPDATE,CMD_UPDATE)
    return self
  
  # ===========================================================================
  def updateDisplay(self):
    if self.reader.execute(1,1):
      last = ''
      fmt0 = '%-34s'
      fmt1 = '%-12s  %-22s'
      fmt2 = '%-36s %16s %16s'
      opt = {}
      all_lines = []
      for i in self.dp.data:
        items  = i.split('#')
        #print items
        node   = items[2]
        task   = items[3]
        state  = items[4]
        action = items[5]
        cmd    = items[6]
        dp     = items[7]
        fsm    = items[8]
        label  = items[9]
        if len(task)>0 and len(label)>0: task=label
        if node == self.name:
          node = task
          task = ''
        if last != node and len(task)>0:
          line = fmt1%(node,task)
          last = node
          n = node
        elif last != node and len(task)==0:
          line = fmt0%(node)
          last = node
          n = node
        else:
          line = fmt1%('',task)
          n = ''
        opt[node] = 0
        line = fmt2%(line,state,action)
        all_lines.append((node,n,line))
        if self.incremental:
          if state=='ERROR': opt[node]=2
          elif len(action)>0: opt[node]=1

      lineNo = CMD_QUIT
      upi.begin_update()
      upi.change_titles(self.mid,timeStamp(),self.name,self.manager.name())
      for node,n,line in all_lines:
        if self.incremental:
          if n == '' and opt[node]==0:
            continue
        lineNo = lineNo + 1
        if lineNo>self.lines:
          if lineNo%10 != 0:
            upi.insert_comment(self.mid,CMD_QUIT,lineNo,line)
          else:
            upi.insert_command(self.mid,CMD_QUIT,lineNo,line)
          self.lines = lineNo
        else:
          if lineNo%10 != 0:
            upi.replace_comment(self.mid,lineNo,line)
          else:
            upi.replace_command(self.mid,lineNo,line)
      l = self.lines
      while(l>lineNo):
        #upi.replace_comment(self.mid,l,'')
        upi.delete_comment(self.mid,l)
        self.lines = self.lines - 1
        l = l - 1
      upi.end_update()
      return
    log(self.name+'> Failed to read datapoint:'+self.dp.name(),timestamp=mk_timestamps)
    
  # ===========================================================================
  def stop(self):
    _timeSensor.remove(self,0)
    upi.sensor().remove(self,self.mid)
    upi.delete_menu(self.mid)
    self.isAlive = 0
    return self

# =============================================================================
class ManagerMenu(upi.Interactor):
  # ===========================================================================
  def __init__(self,manager,dispMgr):
    self.manager = manager
    self.name = self.manager.name()
    self.dispMgr = dispMgr
    self.mid = len(menus)+1
    menus[self.mid] = self
    upi.Interactor.__init__(self,self)
    upi.open_window()
    upi.open_menu(self.mid,0,0,'Display Manager',self.name,socket.gethostname())
    upi.add_command(CMD_QUIT,'Quit')
    upi.add_comment(CMD_QUIT+1,'  ')
    #upi.resetANSI()
    for i in xrange(len(self.dispMgr.slices.container)):
      n = self.dispMgr.slices.container[i].name()
      n = n[:n.find('.')]
      upi.add_command(CMD_QUIT+i+2,n)
    upi.close_menu()
    upi.set_cursor(self.mid,CMD_QUIT,0)
    upi.sensor().add(self,self.mid)

  # ===========================================================================
  def handleEvent(self):
    ev = self.event()
    if ev.eventtype == upi.EventType:  # UpiEvent
      if ev.command_id==CMD_QUIT:
        self.ReallyClose = None
        self.reallyClose = ReallyClose(self)
        #upi.resetANSI()
        #sys.exit(0)
      elif ev.command()<CMD_QUIT+2+len(self.dispMgr.slices.container):
        cmd = ev.command()-CMD_QUIT-2
        nam = self.dispMgr.slices.container[cmd].name()
        n = nam[:nam.find('.')]
        if self.dispMgr.slices.container[cmd].data>0:
          n = n[n.find(':')+1:]
          self.dispMgr.servers[nam] = Display(self.manager,n)
          log(self.name+'> Started display server:'+n,timestamp=mk_timestamps)
        else:
          log(self.name+'> The slice '+n+' is currently not used. No Display availible.',timestamp=mk_timestamps)
      else:
        log(self.name+'> UPI Event arrived.')
    elif ev.eventtype == 3:            # IocEvent
      if ev.iocType() == CMD_QUIT:
        upi.quit()
        upi.resetANSI()
        sys.exit(0)
    else:
      log(self.name+'> Unknown event of type '+str(ev.eventtype)+' arrived')

# =============================================================================
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
    log('Starting display server on system:'+self.manager.name(),timestamp=mk_timestamps)
    self.servers = {}
    for i in self.slices.container:
      self.servers[i.name()] = None
    rdr = self.manager.devReader()
    rdr.add(self.slices.container)
    rdr.execute()
    upi.attach_terminal()
    Online.Utils.makePrint = upi.write_message
    self.menu = ManagerMenu(self.manager,self)
    #self.handleDevices()

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
        srv = Display(self.manager,n)
        self.servers[nam] = srv
        log('Started display server:'+self.servers[nam].name,timestamp=mk_timestamps)
      elif i.data<=0 and self.servers[nam] is not None:
        log('Stop display server:'+self.servers[nam].name,timestamp=mk_timestamps)
        self.servers[nam].stop()
        self.servers[nam] = None
    return 1
  # ===========================================================================
  def update(self):
    for n,s in self.servers.items():
      if s is not None:
        if s.isAlive>0:
          _iocSensor.send(s,CMD_UPDATE,CMD_UPDATE)
        else:
          self.servers[n] = None
# =============================================================================
def sleep(mgrs=[]):
  "Serve controls requests in daemon mode"
  upi.refresh_screen()
  log('Sleeping ....',timestamp=1)
  try:
    cnt = 0
    while(1):
      time.sleep(1)
      cnt = cnt + 1
      if cnt%2 == 0:
        for i in mgrs:
          if hasattr(i,'update'):
            i.update()
  except Exception,X:
    print 'Exception:',str(X)
  except:
    print 'Unknown Exception'
      

# =============================================================================
def runDataflow():
  import Online.PVSSSystems as Systems
  mgr=DisplayManager(Systems.controlsMgr(PVSS.defaultSystemName())).run()
  managers=[mgr]
  sleep(managers)

# =============================================================================
def runHLT(with_server=1):
  import Online.Streaming.DisplayServer as Srv
  upi.attach_terminal()
  Online.Utils.makePrint = upi.write_message
  mgr = PVSS.controlsMgr()
  servers = [Display(mgr,mgr.name()+'_FSMDisplay',target=targetHLT)]
  if with_server:
    servers.append(Srv.BaseDisplayServerManager(mgr))
  else:
    cont = PVSS.DataPointVector()
    sensor = PVSS.DeviceSensor(mgr,cont)
    sensor.run(1)
  sleep(servers)

if __name__=="__main__":
  if PVSS.defaultSystemName()[:3]=='HLT':
    runHLT()
  else:
    runDataflow()
  
