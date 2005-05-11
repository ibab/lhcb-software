import DIM, FSM, gaudimodule, time, threading

log_enabled = 1
def log_transition(action_type, transition):
  if ( log_enabled==1 ):
    DIM.log('Executing transition[%d]: %s -> %s'%(action_type,transition.fr.name,transition.to.name))

class Handler(DIM.CommandHandler):
  def __init__(self, handler, name):
    self.invokeHandler = handler
    DIM.CommandHandler.__init__(self)
    self.command = DIM.Command(name,'C',self)

  def commandHandler(self):
    self.invokeHandler(self.command.getString())

class Client:
  """
        Gaudi task object object
        @author M.Frank
  """
  def __init__(self, node, name):
    """
         Constructor
    """
    self.node = node
    self.name = name
    self.task_id = name

  def invokeTransition(self, target_state):
    DIM.Client.sendCommand('self.task_id/FSM',target_state)

  def setOptions(self, options):
    DIM.Client.sendCommand('self.task_id/OPTIONS',options)
    print 'setOptions:', options


class Call:
  """
        Gaudi call encapsulation object
        @author M.Frank
  """
  def __init__(self, call):
    """
         Constructor
    """
    self.call = call
  def invoke(self, action_type, transition):
    log_transition(action_type, transition)
    result = self.call()
    if ( result.getCode() == gaudimodule.SUCCESS.getCode() ):
      return 1
    return 0

class Task:
  """
        Gaudi task object object
        @author M.Frank
  """
  def __init__(self, name, options=''):
    """
         Constructor
    """
    self.name = name
    self.enable = 0
    self.appMgr = gaudimodule.AppMgr(outputlevel=3,joboptions=options)
    self.transitions = []
    self.fsm = FSM.Machine('Inactive')
    self.fsm.addTransition('Inactive', 'Active',   action=Call(self.appMgr.initialize).invoke)  
    self.fsm.addTransition('Active',   'Running',  action=self.enable_trigger)  
    self.fsm.addTransition('Running',  'Active',   action=self.disable_trigger)  
    self.fsm.addTransition('Active',   'Inactive', action=self.reset_task)  
    self.fsm.addTransition('Active',   'Done',     action=Call(self.appMgr.finalize).invoke)  
    self.fsm.addTransition('Done',     'Inactive', action=self.stop_task)    
    self.command = Handler(self.changeState, self.name+'/FSM')
    self.options = Handler(self.setOptions, self.name+'/OPTIONS')
    #self.service = DIM.Service(self.name+'/STATUS','')
    DIM.log('Task '+self.name+' started successfully.')
    self.event = threading.Event()
    
  def start(self):
    DIM.Server.start(self.name)
    while(1):
      self.event.wait()
      self.event.clear()
      while ( len(self.transitions) > 0 ):
        target_state = self.transitions[0]
        del(self.transitions[0])
        if ( target_state != self.fsm.currentState.name ):
          status = self.fsm.queueTransition(target_state)
          if ( status != 1 ):
            DIM.log('Unknown transition to state:'+target_state+'. status='+str(status))
          else:
            status = self.fsm.start()
            if ( status != 1 ):
              DIM.log('Failed to start FSM transitions. status='+str(status))
    return 1

  def loop(self):
    while(self.enable == 1):      
      result = self.appMgr._evtpro.executeRun(1)
      if ( result.getCode() != gaudimodule.SUCCESS.getCode() ):
        return

  def changeState(self, target_state):
    if (target_state.upper() == 'CANCEL'):
      self.transitions = []
      self.fsm.cancel()
    elif (target_state.upper() == 'NOLOG'):
      log_enabled = 0
      DIM.log('Transition logging is now disabled');
    elif (target_state.upper() == 'LOG'):
      DIM.log('Transition logging is now enabled');
      log_enabled = 1
    else:
      self.transitions.append(target_state)
      self.event.set()

  def setOptions(self, options):
    exec(options)
    print 'setOptions:', options,'done.'

  def enable_trigger(self, action_type, transition):
    log_transition(action_type, transition)
    self.enable = 1
    self.looper = threading.Thread()
    self.looper.run=self.loop
    self.looper.start()
    return 1

  def disable_trigger(self, action_type, transition):
    log_transition(action_type, transition)
    self.enable = 0
    if ( self.looper is not None ):
      self.looper.join()
    self.looper = None
    return 1

  def reset_task(self, action_type, transition):
    log_transition(action_type, transition)
    self.appMgr.finalize()
    return 1

  def stop_task(self, action_type, transition):
    log_transition(action_type, transition)
    self.appMgr.finalize()
    return 1

def run(name='Hello_0', options=''):
  t=Task(name,options)
  t.start()

if __name__=='__main__':
  run()
