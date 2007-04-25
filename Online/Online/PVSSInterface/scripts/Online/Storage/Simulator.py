import Online.PVSS    as PVSS
import Online.Utils   as Utils
from   random  import Random as Random

sum            = Utils.sum
log            = Utils.log
error          = Utils.error
warning        = Utils.warning
std            = PVSS.gbl.std
DeviceSensor   = PVSS.DeviceSensor
DeviceListener = PVSS.PyDeviceListener

DataPoint = PVSS.DataPoint

# =============================================================================
class Simulator(DeviceListener):
  # ===========================================================================
  def __init__(self,manager,name):
    DeviceListener.__init__(self,self,manager)
    self.manager    = manager
    self.reader     = self.manager.devReader()
    self.writer     = self.manager.devWriter()
    self.name       = name
    self.tasks      = {}
    self.random     = Random()
    self.failurePropability = 0.
    self.deathPropability = 0.
    self.transitionTime = 0.
    
  # ===========================================================================
  def handleDevices(self):
    "Callback once per device sensor list on datapoint change."
    return 1
  # ===========================================================================
  def handleDevice(self):
    "Callback once per item in the device sensor list on datapoint change."
    import traceback
    cmd = ''
    try:
      name = self.dp().name()
      disp = name[:name.find(':')]
      if disp.find(':',2)>0: disp=disp[1:disp.find(':')]
      cmd = self.dp().value().data()
      log('Command received '+name[name.find(':'):]+' -> '+cmd,timestamp=1)
      if ( self.tasks.has_key(name) ):
        item = self.tasks[name]
        if ( cmd == 'load' ):
          item[1].data = 'NOT_READY'
        elif ( cmd == 'configure' ):
          item[1].data = 'READY'
        elif ( cmd == 'start' ):
          item[1].data = 'RUNNING'
        elif ( cmd == 'stop' ):
          item[1].data = 'READY'
        elif ( cmd == 'reset' ):
          item[1].data = 'NOT_READY'
        elif ( cmd == 'unload' ):
          item[1].data = 'UNKNOWN'
        else:
          log('The command:"'+cmd+'" failed [Command is not known]',timestamp=1)      
          return 0
        
        if ( self.deathPropability > 0. ):
          p = self.random.uniform(0.,1.)
          if ( p < self.deathPropability ):
            log('Task:"'+name+'" DIED...',timestamp=1)      
            item[1].data = "UNKNOWN"
            self.writer.add(item[1])
            self.writer.execute(0,1)
            return 1
        if ( self.failurePropability > 0. ):
          p = self.random.uniform(0.,1.)
          if ( p < self.failurePropability ):
            log('Task:"'+name+'" is in ERROR...',timestamp=1)      
            item[1].data = "ERROR"
            self.writer.add(item[1])
            self.writer.execute(0,1)
            return 1
        self.writer.add(item[1])
        self.writer.execute(0,1)
        return 1
      log('The command:"'+cmd+'" failed: The device "'+name+'" is not known',timestamp=1)      
      return 0
    except Exception,X:
      log('The command:"'+cmd+'" failed:'+str(X),timestamp=1)
      traceback.print_exc()
      return 0
    except:
      log('The command:"'+cmd+'" failed (Unknown exception)',timestamp=1)
      traceback.print_exc()
    return 0
  # ===========================================================================
  def run(self):
    mgr = self.manager
    actor = PVSS.DpVectorActor(mgr)
    typ   = self.manager.typeMgr().type('FSM_DimTask')
    actor.lookup(self.name+'*',typ)
    #actor.lookup(DataPoint.original(self.name+'_Slice*.Name'),typ)
    print typ.name(), actor.container.size()
    for i in actor.container:
      print i.name()
      item = []
      cmd = DataPoint.original(i.name()+'.Command')
      item.append(i.name())
      item.append(DataPoint(mgr,DataPoint.original(i.name()+'.State')))
      item.append(DataPoint(mgr,cmd))
      item.append(PVSS.DeviceSensor(mgr,item[2]))
      item[3].addListener(self)
      item[3].run(1)
      self.tasks[cmd] = item
  
runInfoCmds = """
import Online.Storage.Simulator as StorageSimulator
reload(StorageSimulator)
import Online.PVSS as PVSS

mgr = PVSS.controlsMgr()
sim = StorageSimulator.Simulator(mgr,'TestStorage_Slice00')
sim.run()

"""
