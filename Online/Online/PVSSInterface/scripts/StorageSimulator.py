import time
import Online.PVSS    as PVSS
import Online.Utils   as Utils

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
    self.manager = manager
    self.name = name
    self.tasks = {}
    
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
      cmd = self.dp().value().data()
      log('Command received:'+name[:name.find(':')]+' -> '+cmd,timestamp=1)
      if ( self.tasks.has_key(name) ):
        item = self.tasks[name]
        return 1
      log('The command:"'+cmd+'" failed: The device "'+nam'" is not known',timestamp=1)      
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
    actor.lookup(self.name+'_Slice*',typ)
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
reload(StorageSimulator)
import Online.PVSS as PVSS
from StorageSimulator import Simulator as Simulator

mgr = PVSS.controlsMgr()
sim = Simulator(mgr,'TestStorage')
sim.run()

"""
