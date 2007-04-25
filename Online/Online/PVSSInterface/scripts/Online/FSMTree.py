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
class Dump:
  # ===========================================================================
  def __init__(self,manager,name):
    self.manager    = manager
    self.reader     = self.manager.devReader()
    self.name       = name
    
  # ===========================================================================
  def show(self, devices=0):
    mgr = self.manager
    actor1 = PVSS.DpVectorActor(mgr)
    actor2 = PVSS.DpVectorActor(mgr)
    if devices:
      typ   = self.manager.typeMgr().type('_FwFsmDevice')
    else:
      typ   = self.manager.typeMgr().type('_FwFsmObject')
    actor1.lookup(DataPoint.original(self.name+'|'+self.name+'*.fsm.currentState'),typ)
    actor2.lookup(DataPoint.original(self.name+'|'+self.name+'*.mode.enabled'),typ)
    print typ.name(), actor1.container.size()
    for i in actor1.container:
      self.reader.add(i)
    for i in actor2.container:
      self.reader.add(i)
    if self.reader.execute(0,1):
      for num in xrange(actor1.container.size()):
        i = actor1.container[num]
        j = actor2.container[num]
        n = i.name()
        n = n[n.find('|')+1:]
        n = n[:n.find('.fsm.currentState:')]
        if j.data < 0: continue
        if n[len(n)-4:] == "_FWM": continue
        if n[len(n)-5:] == "_FWDM": continue
        if n[len(n)-6:] == "_FWCNM": continue
        print "%-100s %-16s %d"%(n, i.data, j.data)
  
  # ===========================================================================
  def showAll(self):
    self.show(0)
    self.show(1)
  
def show(mgr=None,name='TestStorage_Slice00',devices=0):
  m = mgr
  if m is None: m = PVSS.controlsMgr()
  dmp = Dump(mgr,name)
  dmp.show(devices)

if __name__ == "__main__":
  show()
