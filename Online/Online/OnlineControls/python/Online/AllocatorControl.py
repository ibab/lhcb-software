import sys, Online.Utils
import Online.PVSS as PVSS

# =============================================================================
class AllocatorClient:
  """
  Base class with default implementations for standard callbacks.

  @author M.Frank
  """
  # ===========================================================================
  def __init__(self,manager,name):
    """
    Object constructor

    @param  manager     PVSS Controls manager for this client
    @param  name        Client name
    """
    self.manager = manager
    self.name = name

  # ===========================================================================
  def configure(self,rundp_name, partition):
    "Default client callback on the command 'configure'"
    return self

  # ===========================================================================
  def allocate(self,rundp_name, partition):
    "Default client callback on the command 'allocate'"
    return 'SUCCESS'

  # ===========================================================================
  def reallocate(self,rundp_name, partition):
    "Default client callback on the command 'reallocate'"
    return 'SUCCESS'

  # ===========================================================================
  def free(self,rundp_name, partition):
    "Default client callback on the command 'free'"
    return 'SUCCESS'

  # ===========================================================================
  def recover(self,rundp_name, partition):
    "Default client callback on the command 'recover'"
    return 'SUCCESS'

  # ===========================================================================
  def recover_slice(self,rundp_name, partition):
    "Default client callback on the command 'recover_slice'"
    return 'SUCCESS'

    
# =============================================================================
#
# CLASS: Control
#
# =============================================================================
class Control(PVSS.PyDeviceListener):
  """ @class Control

      Generic device listener object to serve PVSS requests.
      Requests are forwarded to registered clients.

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, postfix, objects=[]):
    """ Object constructor
    
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Streaming control datapoint
        @param  postfix       Modifier for reply datapoint name
        @param  objects       List of clients to be managed

        @return reference to initialized object
    """
    self.objects = objects
    self.manager = manager
    self.name    = name
    self.postfix = postfix
    self.do_sleep= 1
    PVSS.PyDeviceListener.__init__(self,self,manager)
    self.writer  = manager.devWriter()
    self.control = self.objects[0].get('Command')
    self.state   = self.objects[0].get('State')
    Online.Utils.openFMC()
    PVSS.info(name+': Listen to '+self.control.name(),timestamp=1,type=PVSS.CONNECTED)
    PVSS.info(name+': Answer to '+self.state.name(),timestamp=1,type=PVSS.CONNECTED)
    self.sensor = PVSS.DeviceSensor(manager,self.control)
    ##PVSS.info(name+': Sensor created...'+self.postfix,timestamp=1,type=PVSS.CONNECTED)
    
  # ===========================================================================
  def makeAnswer(self,status,msg):
    "Create answer object from status."
    print 'Answer message:',status,msg
    PVSS.info('Result ('+msg+')',timestamp=1,type=PVSS.UNEXPECTEDSTATE)
    m = status + msg
    print 'Answer message:',m
    print 'DP:',self.state.name()
    self.writer.clear()
    self.state.data = m
    self.writer.add(self.state)
    if self.writer.execute():
      return 1
    return 0
  
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1

  # ===========================================================================
  def doExecute(self,function,runDpName,partition):
    """
    Execute service request.
    The request is forwarded to each controlled client object.
    The callback name is given as a string.

    Parameters:
    @param  function   Callback name of the client
    @param  runDpName  Name of the RunInfo datapoint
    @param  partition  Partition name
    """
    result = None
    r0 = None
    ##PVSS.info('Executing action:'+function,timestamp=1)
    for i in self.objects:
      if hasattr(i,function):
        result = getattr(i,function)(runDpName, partition)
        if r0 is None: r0 = result
        if result is None:
          PVSS.error('The controlled object '+i.name+' failed: '+function,timestamp=1,type=PVSS.ILLEGAL_ARG)
          return result
        PVSS.info('The controlled object '+i.name+' succeeded: '+function,timestamp=1)
    return r0

  # ===========================================================================
  def handleInvalidDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    cmd = ''
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
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    cmd = ''
    try:
      print "Callback once per item in the device sensor list on datapoint change."
      nam = self.dp().name()
      cmd = self.dp().value().data()
      itms = cmd.split('/')
      print nam,cmd
      if len(itms) >= 5:
        command   = itms[0]
        storage   = itms[1][:-len(self.postfix)]
        partition = itms[2]
        partID    = itms[3]
        runDpName = itms[4]
        Online.Utils.setPartition(partition)
        answer = '/'+itms[1]+'/'+partition+"/"+str(partID)
        result = None
        if storage == self.name:
          if command == "CONFIGURE":
            dp = itms[5]
            ok = itms[6]
            err = itms[7]
            data = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(dp))
            data.data = ok
            try:
              result = self.doExecute('configure',runDpName,partition)
              if result is None: data.data = err
            except Exception, X:
              PVSS.error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.ILLEGAL_ARG)
              traceback.print_exc()
              data.data = err
            self.writer.add(data)
            Online.Utils.log('The command:"'+cmd+'" finished.',timestamp=1)
            self.writer.execute()
            Online.Utils.log('---> Wrote answer '+data.data+' to datapoint:'+dp,timestamp=1)
            return self
          elif command == "RECOVER_SLICE":
            dp = itms[5]
            ok = itms[6]
            err = itms[7]
            data = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(dp))
            data.data = ok
            try:
              result = self.doExecute('recover_slice',runDpName,partition)
              if result is None: data.data = err
            except Exception, X:
              PVSS.error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.ILLEGAL_ARG)
              traceback.print_exc()
              data.data = err
            self.writer.add(data)
            self.writer.execute()
            return self
          try:
            if command == "ALLOCATE":
              result = self.doExecute('allocate',runDpName,partition)
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result)
            elif command == "REALLOCATE":
              result = self.doExecute('free',runDpName,partition)
              if result is None:
                PVSS.error('The command:"free" failed.',timestamp=1,type=PVSS.ILLEGAL_ARG)
              result = self.doExecute('allocate',runDpName,partition)
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result)
            elif command == "DEALLOCATE":
              result = self.doExecute('free',runDpName,partition)
              PVSS.info('Result ('+command+')',timestamp=1,type=PVSS.UNEXPECTEDSTATE)
              if result == "WAS_NOT_ALLOCATED":
                msg = '[WAS_NOT_ALLOCATED] Ignore Error on deallocate on request from Clara'
                PVSS.error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
                #result=None
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result)
            elif command == "RECOVER":
              result = self.doExecute('recover',runDpName,partition)
            if result is not None:
              #print answer+'/'+result
              return self.makeAnswer('READY',answer+'/'+result)
            msg = 'The command:"'+cmd+'" failed. [Internal Error] '
            PVSS.error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
            return self.makeAnswer('ERROR',answer)
          except Exception,X:
            PVSS.error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.ILLEGAL_ARG)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
          except:
            PVSS.error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1,type=PVSS.ILLEGAL_ARG)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
        msg = 'The command:"'+cmd+'" failed. [Bad Streaming System] '+storage+' should be:['+self.name+']'
        PVSS.error(msg,timestamp=1,type=PVSS.ILLEGAL_ARG)
        return self.makeAnswer('ERROR',answer)
      print 'Parameters are:',str(itms)
      PVSS.error('The command:"'+cmd+'" failed. [Insufficient parameters] ',timestamp=1,type=PVSS.ARG_MISSING)
      return self.makeAnswer('ERROR','/'+self.name+'/UNKNOWN/0/Insufficient parameters')
    except Exception,X:
      PVSS.error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
      return 0
    except:
      PVSS.error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
    return 0
 
  # ===========================================================================
  def run(self):
    "Start the controls task by activating the listening devices."
    self.sensor.addListener(self)
    self.sensor.run(1)
    PVSS.info(self.name+': Sensor started...',timestamp=1,type=PVSS.CONNECTED)
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
    
