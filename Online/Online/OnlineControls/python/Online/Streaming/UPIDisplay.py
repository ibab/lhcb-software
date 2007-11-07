import os, sys, time
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

class SliceDisplaySrv(upi.Interactor):
  # ===========================================================================
  def __init__(self,manager,name):
    self.manager = manager
    self.name = name
    self.lines = CMD_QUIT
    self.incremental = True
    self.reader = self.manager.devReader()
    nam = self.name.replace('_Slice','Display_Slice')
    self.dp = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(nam+'.Tasks'))
    self.reader.add(self.dp)
    log('Starting display for slice:'+self.name+' on system:'+self.manager.name(),timestamp=1)
    upi.Interactor.__init__(self,self)
    self.mid = len(menus)+1
    menus[self.mid] = self
    upi.open_window()
    upi.open_menu(self.mid,0,0,self.manager.name(),name,timeStamp())
    upi.add_command(CMD_UPDATE,'Update')
    upi.add_command(CMD_INCREMENTAL,'Show full status')
    upi.add_command(CMD_QUIT,'Close')
    upi.close_menu()
    self.update()
    upi.set_cursor(self.mid,CMD_UPDATE,0)
    upi.sensor().add(self,self.mid)
    self.isAlive = 1

  # ===========================================================================
  def handleEvent(self):
    ev = self.event()
    if ev.eventtype == upi.EventType:  # UpiEvent
      if ev.command_id==CMD_QUIT:
        self.stop()
        #upi.resetANSI()
        #sys.exit(0)
      elif ev.command_id==CMD_INCREMENTAL:
        self.incremental = not self.incremental
        if self.incremental:
          upi.replace_command(self.mid,CMD_INCREMENTAL,'Show full status')
        else:
          upi.replace_command(self.mid,CMD_INCREMENTAL,'Show incremental status')
        log(self.name+'> Incremental status is now:'+str(self.incremental),timestamp=1)
        self.update()
      elif ev.command_id==CMD_UPDATE:
        self.update()
      elif ev.menu_id == 0 and ev.command_id == 0 and ev.param_id == 0:
        upi.refresh_screen()
      else:
        log(self.name+'> UPI Event arrived.')
        self.update()
    elif ev.eventtype == 0:            # TimeEvent
      log(self.name+'> Time event arrived')
      self.update()
    elif ev.eventtype == 3:            # IocEvent
      if ev.iocType() == CMD_UPDATE:
        self.update()
      else:
        log(self.name+'> Unknown IOC event arrived'+str(ev.type)+' '+str(ev.iocType())+' '+str(ev.iocData()),timestamp=1)
    else:
      log(self.name+'> Unknown event of type '+str(ev.eventtype)+' arrived')
      
  # ===========================================================================
  def update(self):
    if self.reader.execute(1,1):
      last = ''
      fmt0 = '%-34s'
      fmt1 = '%-12s  %-22s'
      fmt2 = '%-36s %16s %16s'
      opt = {}
      all_lines = []
      for i in self.dp.data:
        items = i.split('#')
        action = items[5]
        node = items[2]
        task = items[3]
        state = items[4]
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
      upi.change_titles(self.mid,self.manager.name(),self.name,timeStamp())
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
      #upi.resetANSI()
      _timeSensor.add(self,10,0)
      return
    log(self.name+'> Failed to read datapoint:'+self.dp.name(),timestamp=1)
    
  # ===========================================================================
  def stop(self):
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
    self.lines = CMD_QUIT
    self.mid = len(menus)+1
    menus[self.mid] = self
    upi.Interactor.__init__(self,self)
    upi.open_window()
    upi.open_menu(self.mid,0,0,'Display Manager',self.name,self.name)
    upi.add_command(CMD_QUIT,'Quit')
    #upi.resetANSI()
    for i in xrange(len(self.dispMgr.slices.container)):
      n = self.dispMgr.slices.container[i].name()
      n = n[:n.find('.')]
      upi.add_command(CMD_QUIT+i+1,n)
    upi.close_menu()
    upi.set_cursor(self.mid,CMD_QUIT,0)
    upi.sensor().add(self,self.mid)

  # ===========================================================================
  def handleEvent(self):
    ev = self.event()
    if ev.eventtype == upi.EventType:  # UpiEvent
      if ev.command_id==CMD_QUIT:
        upi.resetANSI()
        sys.exit(0)
      elif ev.command()<CMD_QUIT+1+len(self.dispMgr.slices.container):
        cmd = ev.command()-CMD_QUIT-1
        nam = self.dispMgr.slices.container[cmd].name()
        n = nam[:nam.find('.')]
        if self.dispMgr.slices.container[cmd].data>0:
          n = n[n.find(':')+1:]
          self.dispMgr.servers[nam] = SliceDisplaySrv(self.manager,n)
          log(self.name+'> Started display server:'+n,timestamp=1)
        else:
          log(self.name+'> The slice '+n+' is currently not used. No Display availible.',timestamp=1)
      else:
        log(self.name+'> UPI Event arrived.')
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
    log('Starting display server on system:'+self.manager.name(),timestamp=1)
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
        srv = SliceDisplaySrv(self.manager,n)
        self.servers[nam] = srv
        log('Started display server:'+self.servers[nam].name,timestamp=1)
      elif i.data<=0 and self.servers[nam] is not None:
        log('Stop display server:'+self.servers[nam].name,timestamp=1)
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
  cnt = 0
  while(1):
    time.sleep(1)
    cnt = cnt + 1
    if cnt%2 == 0:
      for i in mgrs:
        i.update()
      
if __name__=="__main__":
  import Online.PVSSSystems as Systems
  sto_mgr = DisplayManager(Systems.controlsMgr('STORAGE')).run()
  mon_mgr = DisplayManager(Systems.controlsMgr('MONITORING')).run()
  upi.refresh_screen()
  sleep([sto_mgr,mon_mgr])
