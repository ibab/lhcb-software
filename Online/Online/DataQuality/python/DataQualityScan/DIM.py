import os, sys, time, copy, pydim, threading
from DataQualityScan import *


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

#----------------------------------------------------------------------------------------
class IntegerService:
  #--------------------------------------------------------------------------------------
  def __init__(self, name, init_state):
    self.__name  = name
    self.__value = init_state
    self.__svcID = pydim.dis_add_service(self.__name, 'I', self.service, 1)
    pydim.dis_update_service(self.__svcID, (int(self.__value),))

  #--------------------------------------------------------------------------------------
  def update(self, new_state):
    self.__value = new_state
    pydim.dis_update_service(self.__svcID, (int(self.__value),))

  #--------------------------------------------------------------------------------------
  def service(self,tag):
    return (int(self.__value),)


#----------------------------------------------------------------------------------------
class FSM:
  """
    No need to take locks. Everything is protected by the global DIM lock.
  """
  ST_OFFLINE    = 'OFFLINE'
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
    self.previousState = FSM.ST_UNKNOWN
    log(INFO,'DIM Command starting.... ')
    pydim.dis_update_service(self.__svcID, (self.__state,))
    pydim.dis_update_service(self.__fsmID, self.fsm_service(self))
    log(INFO,'DIM Command starting....Done ')

  #--------------------------------------------------------------------------------------
  def state(self):
    return self.__state

  #--------------------------------------------------------------------------------------
  def keepAlive(self):
    return self.__keep

  #--------------------------------------------------------------------------------------
  def start(self):
    log(INFO,"Start serving DIM....")
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

  def _setState(self, new_state):
    self.previousState = self.__state
    self.__state = new_state

  #--------------------------------------------------------------------------------------
  def handleCommand(self,cmd):
    self.monitor.last = int(time.time())
    log(INFO,'DIM Command: '+str(cmd))
    sys.stdout.flush()
    cb = 'handle'+cmd[0].upper()+cmd[1:]
    if hasattr(self,cb):
      if not getattr(self,cb)():
        self.state = FSM.ST_ERROR
        if hasattr(self,'on'+FSM.ST_ERROR):
          getattr(self,'on'+FSM.ST_ERROR)()
        cmd = ''

    if cmd == FSM.CMD_CONFIGURE:
      self._setState(FSM.ST_READY)
    elif cmd == FSM.CMD_START:
      self._setState(FSM.ST_RUNNING)
    elif cmd == FSM.CMD_STOP:
      self._setState(FSM.ST_STOPPED)
    elif cmd == FSM.CMD_RESET:
      self._setState(FSM.ST_NOT_READY)
    elif cmd == FSM.CMD_CONTINUE:
      self._setState(FSM.ST_RUNNING)
    elif cmd == FSM.CMD_LOAD:
      self._setState(FSM.ST_NOT_READY)
    elif cmd == FSM.CMD_UNLOAD:
      self._setState(FSM.ST_UNKNOWN)
      self.__keep = False
    elif cmd == FSM.CMD_FORCE_RESET:
      self._setState(FSM.ST_UNKNOWN)
      self.__keep = False
    elif cmd == FSM.CMD_GET_STATE:
      pass
    elif cmd == FSM.CMD_STOP_TRIGGER:
      return
    elif cmd == FSM.CMD_START_TRIGGER:
      return
    else:
      self._setState(FSM.ST_ERROR)

    if hasattr(self,'on'+self.__state):
      getattr(self,'on'+self.__state)()

    pydim.dis_update_service(self.__svcID,(self.__state,))
    if self.__fsmID != 0: pydim.dis_update_service(self.__fsmID)


  #--------------------------------------------------------------------------------------
  def callback(self, *args):
    log(INFO,'++ FSM status callback. Args are %s'%str(args))
    r = args[0][0]
    r = r[:r.find('\0')]
    self.handleCommand(r)
    pass

#----------------------------------------------------------------------------------------
"""
from DimInterface import *
c = Controller('MONA1001_R_Controller/status','C')

"""
class Controller:
  #--------------------------------------------------------------------------------------
  def __init__(self, client, format='C'):
    log(INFO,"+++ Starting control object connected to %s"%(client,))
    self.__client = client
    self.__state = ''
    self.__lock = threading.Lock()
    self.__infoID = pydim.dic_info_service(self.__client,format,self.callback)
    self.__callbacks = {}

  #--------------------------------------------------------------------------------------
  def state(self):
    return str(self.__state)

  def state2(self):
    self.__lock.acquire()
    data = None
    if self.__state:
      data = copy.deepcopy(self.__state)
    self.__lock.release()
    return data

  def register(self, state, call):
    self.__callbacks[state] = call

  #--------------------------------------------------------------------------------------
  def callback(self, *args):
    log(VERBOSE,'++ Controller callback. Args are %s'%str(args))
    if len(args) > 1:
      self.__lock.acquire()
      r = args[1]
      r = r[:r.find('\0')]
      self.__state = r
      if self.__callbacks.has_key(self.__state):
        self.__callbacks[self.__state]()
      self.handleState(r)
      self.__lock.release()

  #--------------------------------------------------------------------------------------
  def handleState(self, state):
    log(INFO,'++ Controller callback. State: %s'%str(state))    
    return self



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

