
START             = 0
DONE_PREACTION    = 1
DONE_ACTION       = 2
DONE_POSTACTION   = 3
DONE              = 4
FAILED_POSTACTION = 10
FAILED_ACTION     = 11
FAILED_PREACTION  = 12
FAILED            = 13

PREACTION   = 1
ACTION      = 2 
POSTACTION  = 3

_debug = 0

def log(msg):
  if ( _debug ):
    print msg

def dummy_call(type, transition):
  return 1

class Transition:
  """
        FSM transition object
        @author M.Frank
  """
  def __init__(self, fr, to, pre_action=dummy_call, action=dummy_call, post_action=dummy_call):
    """ Default constructor
        @param fr          Initial state
        @param to          Target state
        @param pre_action  User callback executed as transition pre-action
        @param action      User callback executed during transition.
        @param post_action User callback executed as transition post-action
    """
    self.fr = fr
    self.to = to
    self.preAction  = pre_action
    self.action     = action
    self.postAction = post_action

  def _check(self,status,m,s1,s2):
    """ Transition execution method """
    if ( status ):
      m.informListeners(s1,self)
    else:
      m.informListeners(s2,self)
    return status

  def _action(self, machine):
    """ Action executor """
    return self._check(self.action(ACTION,self),machine,DONE_ACTION,FAILED_ACTION)

  def _preAction(self,machine):
    """ Pre-Action executor """
    return self._check(self.preAction(PREACTION,self),machine,DONE_PREACTION,FAILED_PREACTION)

  def _postAction(self,machine):
    """ Post-Action executor """
    return self._check(self.postAction(POSTACTION,self),machine,DONE_POSTACTION,FAILED_POSTACTION)
  
  def __str__(self):
    s = ("%-16s -> %-16s")%(self.fr.name,self.to.name)
    if ( self.preAction ):
      s = s + 'Y'
    else:
      s = s + 'N'
    if ( self.action ):
      s = s + 'Y'
    else:
      s = s + 'N'
    if ( self.postAction ):
      s = s + 'Y'
    else:
      s = s + 'N'
    return s

class State:
  """
        FSM state object

        @author M.Frank
  """
  def __init__(self, name):
    """ Default constructor
        @param  name       State name
    """
    self.name = name
    self.transitions = {}
    log('Created state:%s'%name)

  def findTransition(self, to):
    """ Find output transition from this state.
        @param to    Output state (name/State object)
        @return Transition leading to the requested state (None on failure)
    """
    for t in self.transitions.values():
      if ( t is not None ):
        if isinstance(to,str):
          if ( t.to.name == to ):
            return t
        elif isinstance(to,State):
          if ( t.to.name == to.name ):
            return t
    return None

  def addTransition(self, to, pre_action, action, post_action):
    """ Add new output transition
        @param to          Target state
        @param pre_action  User callback executed as transition pre-action
        @param action      User callback executed during transition.
        @param post_action User callback executed as transition post-action
    """
    if ( self.findTransition(to) is None ):
      t = Transition(self, to, pre_action, action, post_action)
      self.transitions[to.name] = t
      return t
    return None
  def __str__(self):
    s = self.name + ':' + str(self.transitions)
    return s

class Machine:
  """
        Main FSM machine object
        @author M.Frank
  """
  def __init__(self, init_state=None):
    self.queue        = []
    self.states       = {}
    self.listeners    = []
    self.transitions  = []
    self.initialState = None
    self.currentState = None
    self.targetState  = None
    self.executing    = None
    if ( init_state ):
      self.setInitialState(self.state(init_state,1))

  def state(self, name, create=None):
    if isinstance(name, str):
      if ( not(self.states.has_key(name)) and (create is not None) ):
        self.states[name] = State(name)
      if ( self.states.has_key(name) ):
        return self.states[name]
    if isinstance(name, State):
      return self.state(name.name,create)
    return None

  def informListeners(self, type, transition):
    for l in self.listeners:
      l.notify(type, transition)

  def invokeTransition(self, tr):
    if ( tr ):
      try:
        self.targetState = tr.to
        self.informListeners(START, tr)
        tr._preAction(self)
        tr._action(self)
        tr._postAction(self)
        self.currentState = self.targetState
        self.informListeners(DONE, tr)
        return 1
      except Exception, X:
        print X
    self.targetState = self.currentState;
    try:
      self.informListeners(FAILED, tr);
    except Exception, X:
      print X
    return 0

  def cancel(self):
    self.queue = []
    return 1
            
  def start(self):
    """ Start working down the transition queue.
        @return StatusCode indicating success (1) or failure (0).
    """
    if ( not self.executing ):
      r = 1
      while (len(self.queue) > 0):
        t = self.queue[0]
        del(self.queue[0])
        self.executing = 1        
        if ( t is not None ):
          if ( self.currentState.name != t.to.name ):
            tr = self.currentState.findTransition(t.to)
            if ( tr is not None ):
              r=self.invokeTransition(tr)
          else:
            r = 1
        if ( not(r) ):
          self.queue = []
          self.executing = 0
          return r
      self.executing = 0
      return r
    return 0
    
  def isRunning(self):
    """ Check if task is running """
    if ( self.currentState ):
      if ( self.initialState ):
        return self.currentState.name != self.initialState.name
    return false

  def setInitialState(self, baseState ):
    """ Set Initial state of the FSM machine.
    """
    self.initialState = self.currentState = self.targetState = self.state(baseState, 1)

  def setCurrentState(self, state ):
    """ Set the current state of the machine.
        @param    state  Name of the current state
        @return StatusCode indicating success (1) or failure (0).
    """
    st = self.state(state, false)
    if ( st is not None ):
      self.currentState = st
      self.informListeners(STATE_CHANGED, None)
      return 1
    return 0

  def findTransition(self, fr, to):
    """ Access any Transition by name 
        @param fr      Name of the starting state
        @paran to      Name of the end state
        @return Transition to be executed.
    """
    init = self.state(fr)
    st   = self.state(to)
    t = None
    if ( st is not None and init is not None ):
      if ( init.transitions.has_key(to) ):
        t = init.transitions[to]
        log('findTransition: %s'%str(t))
        return t
      for t in init.transitions.values():
        if ( st.name == t.to.name ):
          log('findTransition: %s'%str(t))
          return t  
    log('findTransition: %s'%str(None))
    return None

  def addTransition(self, fr, to, pre_action=dummy_call, action=dummy_call, post_action=dummy_call):
    """ Add Transition to finite state diagram.
        @param fr          Initial state
        @param to          Target state
        @param pre_action  User callback executed as transition pre-action
        @param action      User callback executed during transition.
        @param post_action User callback executed as transition post-action
        @return StatusCode indicating success (1) or failure (0).
    """
    st_to = self.state(to,1)
    st_from = self.state(fr,1)
    t = st_from.addTransition(st_to, pre_action, action, post_action)
    if ( t is None ):
      return 0
    self.transitions.append(t)
    return t

  def queueTransition( self, tname ):
    """ Queue new Transition by the name of the next state.
        @return StatusCode indicating success (1) or failure (0).
    """
    tr = None
    if ( len(self.queue)==0 ):
      tr = self.findTransition(self.currentState, tname)
    else:
      tr = self.findTransition(self.queue[-1].to, tname)
    if ( tr is not None ):
      self.queue.append(tr)
      return 1
    return 0
  
class ControlPage:    
  """
        Graphics control object of a FSM machine object

        @author M.Frank
  """
  def __init__(self, parent, name, machine):
    """ Default constructor
        @param  parent     Parent wx widget
        @param  name       State name
        @param  machine    FSM state machine
    """
    import wx, graphics.tools
    self.tools = []    
    self.name = name
    self.parent = parent
    self.machine = machine
    self.machine.listeners.append(self)
    for i in xrange(100):
      siz = wx.Size(250,35)
      if ( i<2 ): 
        siz=wx.Size(300,50)
      c = wx.Button(parent, wx.NewId(), '', size=siz)
      wx.EVT_BUTTON(parent, c.GetId(), self.OnClick)
      self.parent.sizer.Add(c, i, wx.ALIGN_CENTER, border=1)
      c.Show(0)
      self.tools.append([c,c.GetId(),None])
    self.tools[0][0].SetBackgroundColour(wx.Color(255,255,0))
    self.tools[0][0].Show(1)
    self.tools[1][0].SetLabel('')
    self.tools[1][0].Show(1)
    self.placeButtons()

  def placeButtons(self):
    import wx
    i = 2
    self.tools[0][0].SetLabel('Current State:'+self.machine.currentState.name)
    for k in self.machine.currentState.transitions.keys():
      s = self.machine.currentState.transitions[k]
      desc = s.fr.name + ' -> ' +s.to.name
      self.tools[i][2] = s.to
      self.tools[i][0].SetLabel(desc)
      self.tools[i][0].Show(1)
      i = i + 1
    self.parent.Layout()      
  #
  def fsm_handler(self):
    try:
      self.machine.start()
    except Exception, X:
      wx.LogMessage('Exception during request processing: '+str(X))
      return
    except:
      wx.LogMessage('Unknown exception during request processing')
      return
      
  def OnClick(self, event):
    import wx, threading
    data   = None
    button = event.GetEventObject()
    for t in self.tools[2:]:
      if ( t[1] == button.GetId() ):
        data = t[2]
        break
    if ( data is not None ):
      for t in self.tools[2:]:
        t[0].Show(0)
      if ( self.machine.queueTransition(data) is not None ):
        wx.LogMessage('Queue transition: %s -> %s'%(self.machine.currentState.name,str(data)))
      else:
        wx.LogMessage('FAILED to queue transition: %s -> %s'%(self.machine.currentState.name,str(data)))
      self.fsm_handler()
      self.placeButtons()        

  """
      try:
        t = threading.Thread(target=self.fsm_handler)
        t.start()
        t.join(1000.0)
        self.placeButtons()        
        if ( t.isAlive() ):
          raise IOError, "Couldn't read request: Timout signal from HTTP handler"
      except Exception, X:
        wx.LogMessage('Exception during request processing: '+str(X))
        return
      except:
        wx.LogMessage('Unknown exception during request processing')
        return
  """
  def notify(self, type, transition):
    import wx
    if type == START:
      self.tools[0][0].SetLabel('Current state:'+self.machine.currentState.name)
      self.tools[1][0].SetLabel('Target  state:'+self.machine.targetState.name)
      self.tools[1][0].SetBackgroundColour(wx.Color(255,255,0))
    elif type == DONE:
      self.tools[0][0].SetLabel('Current state:'+self.machine.currentState.name)
      self.tools[1][0].SetLabel('Transition executed successfully')
      self.tools[1][0].SetBackgroundColour(wx.Color(0,255,0))
    elif type == FAILED:
      self.tools[0][0].SetLabel('Current state:'+self.machine.currentState.name)
      self.tools[1][0].SetLabel('Transition FAILED')
      self.tools[1][0].SetBackgroundColour(wx.Color(255,0,0))
      wx.LogMessage('Transition Failed')

if __name__ == '__main__':   # Unit test
  def call(type, transition):
    print 'Callback:',type, transition.fr.name,'--->',transition.to.name
    return 1
  m = Machine('Offline')
  m.addTransition('Offline', 'Config',  call, call, call)
  m.addTransition('Config',  'Active',  call, call, call)  
  m.addTransition('Active',  'Done',    call, call, call)  
  m.addTransition('Done',    'Offline', call, call, call)  
  print 'Queue transition to Config: ', m.queueTransition('Config')
  print 'Queue transition to Active: ', m.queueTransition('Active')
  print 'Queue transition to Done:   ', m.queueTransition('Done')
  print 'Queue transition to Offline:', m.queueTransition(m.state('Offline'))
  print 'Queue transition to BlaBla: ', m.queueTransition('BlaBla')
  print 'Queue transition to BlaBla: ',  m.queueTransition(m.state('BlaBla'))
  print 'Initial State:', m.initialState.name
  print 'Current State:', m.currentState.name
  m.start()
  print 'Current State:', m.currentState.name
