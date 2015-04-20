import os, sys, time, copy, pydim, threading
from HLT2 import *


#----------------------------------------------------------------------------------------
class RunFileClient:

  #--------------------------------------------------------------------------------------
  def __init__(self, name, format='C'):
    self.lastData = None
    self.__lock = threading.Lock()
    self.__runs = {}
    self.__infoID = pydim.dic_info_service(name,format,self.callback)

  #--------------------------------------------------------------------------------------
  def getData(self):
    self.__lock.acquire()
    data = copy.deepcopy(self.__runs)
    self.__lock.release()
    return data

  #--------------------------------------------------------------------------------------
  def callback(self, *args):
    log(VERBOSE,'++ RunFileClient callback. Args are %s'%str(args))
    if len(args) > 1:
      self.__lock.acquire()
      r = args[1]
      r = r[:r.find('\0')]
      items = r.split('|')
      runs = {}
      for r in items:
        run,files = r.split('/') 
        runs[run] = files
      self.__runs = runs
      self.__lock.release()
      self.lastData = time.time()

#----------------------------------------------------------------------------------------
class Monitor:
  def __init__(self, partitionid):
    self.done   = int(time.time())
    self.last   = int(time.time())
    self.pid    = os.getpid()
    self.partID = partitionid

class FSM:
  """
    No need to take locks. Everything is protected by the global DIM lock.
  """
  ST_UNKNOWN    = 'UNKNOWN'
  ST_NOT_READY  = 'NOT_READY'
  ST_READY      = 'READY'
  ST_RUNNING    = 'RUNNING'
  ST_STOPPED    = 'READY'
  ST_PAUSED     = 'PAUSED'
  ST_ERROR      = 'ERROR'
  Meta = {ST_UNKNOWN: 'U', ST_NOT_READY: 'N', ST_READY: 'r', ST_RUNNING: 'R', ST_STOPPED: 'r', ST_PAUSED: 'r', ST_ERROR: 'E'}  
  CMD_CONFIGURE = 'configure'
  CMD_START     = 'start'
  CMD_STOP      = 'stop'
  CMD_RESET     = 'reset'
  CMD_CONTINUE  = 'continue'
  CMD_LOAD      = 'load'
  CMD_UNLOAD    = 'unload'
  
  CMD_GET_STATE = '!state'
  CMD_FORCE_RESET = 'RESET'

  #--------------------------------------------------------------------------------------
  def __init__(self, name, partitionid=0, auto=False):
    self.monitor  = Monitor(partitionid)
    self.__name   = name
    self.__auto   = auto
    self.__keep   = True
    self.__state  = FSM.ST_UNKNOWN
    self.__cmdID  = pydim.dis_add_cmnd(name,'C',self.callback,1)
    self.__svcID  = pydim.dis_add_service(name+'/status', 'C', self.service, 1)
    self.__fsmID  = pydim.dis_add_service(name+'/fsm_status','L:2;I:1;C:4;I:1',self.fsm_service, 4)
    pydim.dis_update_service(self.__svcID, (self.__state,))
    pydim.dis_update_service(self.__fsmID, self.fsm_service(self))

  #--------------------------------------------------------------------------------------
  def state(self):
    return self.__state

  #--------------------------------------------------------------------------------------
  def keepAlive(self):
    return self.__keep

  #--------------------------------------------------------------------------------------
  def start(self):
    pydim.dis_start_serving(self.__name)
    self.handleCommand(FSM.CMD_LOAD)
    if self.__auto:
      self.handleCommand(FSM.CMD_CONFIGURE)
      self.handleCommand(FSM.CMD_START)

  #--------------------------------------------------------------------------------------
  def service(self,tag):
    return self.__state

  #--------------------------------------------------------------------------------------
  def inState(self, states):
    if type(states) is list:
      for s in states: 
        if self.__state == s: return True
    elif type(states) is tuple:
      for s in states: 
        if self.__state == s: return True
    elif type(states) is set:
      for s in states: 
        if self.__state == s: return True
    elif self.__state == states:
      return True
    return False

  #--------------------------------------------------------------------------------------
  def fsm_service(self,tag):
    meta = FSM.Meta[self.__state]
    self.monitor.metastate = meta+meta+'S'+' '
    self.monitor.last = self.monitor.done
    self.monitor.done = int(time.time())
    result = (long(self.monitor.last), 
              long(self.monitor.done), 
              int(self.monitor.pid), 
              self.monitor.metastate,
              int(self.monitor.partID),)
    log(INFO,'Publishing fsm metastate: '+str(result))
    return result

  #--------------------------------------------------------------------------------------
  def handleCommand(self,cmd):
    self.monitor.last = int(time.time())
    if cmd == FSM.CMD_CONFIGURE:
      self.__state = FSM.ST_READY
    elif cmd == FSM.CMD_START:
      self.__state = FSM.ST_RUNNING
    elif cmd == FSM.CMD_STOP:
      self.__state = FSM.ST_STOPPED
    elif cmd == FSM.CMD_RESET:
      self.__state = FSM.ST_NOT_READY
    elif cmd == FSM.CMD_CONTINUE:
      self.__state = FSM.ST_RUNNING
    elif cmd == FSM.CMD_LOAD:
      self.__state = FSM.ST_NOT_READY
    elif cmd == FSM.CMD_UNLOAD:
      self.__state = FSM.ST_UNKNOWN
      self.__keep = False
    elif cmd == FSM.CMD_FORCE_RESET:
      self.__state = FSM.ST_UNKNOWN
      self.__keep = False
    elif cmd == FSM.CMD_GET_STATE:
      pass
    elif cmd == FSM.CMD_STOP_TRIGGER:
      return
    elif cmd == FSM.CMD_START_TRIGGER:
      return
    else:
      self.__state = FSM.ST_ERROR
    pydim.dis_update_service(self.__svcID,(self.__state,))
    if self.__fsmID != 0: pydim.dis_update_service(self.__fsmID)


  #--------------------------------------------------------------------------------------
  def callback(self, *args):
    log(DEBUG,'++ FSM status callback. Args are %s'%str(args))
    r = args[0][0]
    r = r[:r.find('\0')]
    self.handleCommand(r)
    pass

def runVoidTask(name=None,auto=False):
  utgid = ''
  partition = 0x103
  if name is not None: utgid = name
  for i in xrange(len(sys.argv)):
    a = sys.argv[i]
    if a[:1] == '-a': auto=True
    if a[:2] == '-u': utgid = sys.argv[i+1]
    if a[:2] == '-p': partition = int(sys.argv[i+1])
  if not len(utgid) and os.environ.has_key('UTGID'):
    utgid = os.environ['UTGID']
  log(INFO,"FSM task: utgid:"+utgid+" auto:"+str(auto))
  fsm = FSM(name=utgid, partitionid=partition, auto=auto)
  pydim.dis_start_serving(utgid)
  fsm.handleCommand(FSM.CMD_LOAD)
  fsm.start()
  while fsm.keepAlive():
    time.sleep(200)


if __name__ == "__main__":
  utgid = 'UTGID'
  if os.environ.has_key('UTGID'): utgid = os.environ['UTGID']
  c = RunFileClient('/HLT/Deferred/Runs','C')
  f = FSM(utgid)
  pydim.dis_start_serving(utgid)
  f.handleCommand(FSM.CMD_LOAD)
  while 1:
    time.sleep(200)

