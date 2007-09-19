import Online.PVSS    as PVSS
error          = PVSS.error
PVSS.logPrefix = 'PVSS Control '


# =============================================================================
class AllocatorClient:
  # ===========================================================================
  def __init__(self,manager,name):
    self.manager = manager
    self.name = name
  # ===========================================================================
  def configure(self,rundp_name, partition):
    return self

  # ===========================================================================
  def allocate(self,rundp_name, partition):
    return 'SUCCESS'

  # ===========================================================================
  def free(self,rundp_name, partition):
    return 'SUCCESS'
  # ===========================================================================
  def recover(self,rundp_name, partition):
    return 'SUCCESS'

  # ===========================================================================
  def recover_slice(self,rundp_name, partition):
    return 'SUCCESS'

    
# =============================================================================
#
# CLASS: Control
#
# =============================================================================
class Control(PVSS.PyDeviceListener):
  """ @class Control

      @author  M.Frank
      @version 1.0
  """
  # ===========================================================================
  def __init__(self, manager, name, postfix, objects=[]):
    """ Default constructor
        @param  manager       Reference to PVSS ControlManager
        @param  name          Name of the Streaming control datapoint

        @return reference to initialized object
    """
    self.objects = objects
    self.manager = manager
    self.name    = name
    self.postfix = postfix
    PVSS.PyDeviceListener.__init__(self,self,manager)
    self.writer  = manager.devWriter()
    self.control = self.objects[0].get('Command')
    self.state   = self.objects[0].get('State')
    PVSS.info(name+': Listen to '+self.control.name(),1)
    PVSS.info(name+': Answer to '+self.state.name(),1)
    self.sensor = PVSS.DeviceSensor(manager,self.control)
    
  # ===========================================================================
  def makeAnswer(self,status,msg):
    "Create answer object from status."
    m = status + msg
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
    result = None
    r0 = None
    for i in self.objects:
      result = getattr(i,function)(runDpName, partition)
      if r0 is None: r0 = result
      if result is None:
        return result
    return r0

  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    cmd = ''
    try:
      nam = self.dp().name()
      cmd = self.dp().value().data()
      itms = cmd.split('/')
      if len(itms) >= 5:
        command   = itms[0]
        storage   = itms[1][:-len(self.postfix)]
        partition = itms[2]
        partID    = itms[3]
        runDpName = itms[4]

        answer = '/'+itms[1]+'/'+partition+"/"+str(partID)
        result = None
        if storage == self.name:
          try:
            if command == "CONFIGURE":
              dp = itms[5]
              ok = itms[6]
              err = itms[7]
              data = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(dp))
              data.data = err
              result = self.doExecute('configure',runDpName,partition)
              if result is not None:
                data.data = ok
              self.writer.add(data)
              self.writer.execute()
              return self
            elif command == "RECOVER_SLICE":
              dp = itms[5]
              ok = itms[6]
              err = itms[7]
              data = PVSS.DataPoint(self.manager,PVSS.DataPoint.original(dp))
              data.data = err
              result = self.doExecute('recover_slice',runDpName,partition)
              if result is not None:
                data.data = ok
              self.writer.add(data)
              self.writer.execute()
              return self
            elif command == "ALLOCATE":
              result = self.doExecute('allocate',runDpName,partition)
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result)
            elif command == "DEALLOCATE":
              result = self.doExecute('free',runDpName,partition)
              if result == "WAS_NOT_ALLOCATED": result=None
              if result is not None:
                return self.makeAnswer(command,answer+'/'+result)
            elif command == "RECOVER":
              result = self.doExecute('recover',runDpName,partition)
            if result is not None:
              #print answer+'/'+result
              return self.makeAnswer('READY',answer+'/'+result)
            msg = 'The command:"'+cmd+'" failed. [Internal Error] '
            error(msg,timestamp=1,type=PVSS.UNEXPECTEDSTATE)
            return self.makeAnswer('ERROR',answer)
          except Exception,X:
            error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.ILLEGAL_ARG)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
          except:
            error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1,type=PVSS.ILLEGAL_ARG)
            traceback.print_exc()
            return self.makeAnswer('ERROR',answer)
        msg = 'The command:"'+cmd+'" failed. [Bad Streaming System] '+storage+' should be:['+self.name+']'
        error(msg,timestamp=1,type=PVSS.ILLEGAL_ARG)
        return self.makeAnswer('ERROR',answer)
      print 'Parameters are:',str(itms)
      error('The command:"'+cmd+'" failed. [Insufficient parameters] ',timestamp=1,type=PVSS.ARG_MISSING)
      return self.makeAnswer('ERROR','/'+self.name+'/UNKNOWN/0/Insufficient parameters')
    except Exception,X:
      error('The command:"'+cmd+'" failed:'+str(X),timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
      return 0
    except:
      error('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1,type=PVSS.UNDEFD_FUNC)
      traceback.print_exc()
    return 0
 
  # ===========================================================================
  def run(self):
    self.sensor.addListener(self)
    self.sensor.run(1)

  # ===========================================================================
  def sleep(self):
    import sys, time
    print 'Sleeping ....'
    sys.stdout.flush()
    try:
      while(1):
        time.sleep(1)
    except Exception,X:
      print 'Exception:',str(X)
    except:
      print 'Unknown Exception'
    
