
def install(name='Storage'):
  import Online.Storage.Installer as SOI
  import Online.JobOptions.Installer as JOI
  SOI.install('Storage')
  JOI.install()

def uninstall(name='Storage'):
  #import Online.Storage.Installer as SOI
  import Online.JobOptions.Installer as JOI
  #SOI.uninstall('Storage')
  JOI.uninstall()

def run(name='Storage',sim=None):
  import Online.PVSS as PVSS
  import Online.JobOptions.JobOptionsControl as WR
  import Online.Storage.StorageControl as StorageControl
  import Online.Storage.Simulator as StorageSimulator
  mgr = PVSS.controlsMgr()
  ctrl = StorageControl.Control(mgr,name)
  sensor=PVSS.DeviceSensor(mgr,ctrl.point)
  sensor.addListener(ctrl)
  sensor.run(1)
  wr = WR.StorageOptionsWriter(mgr,'JobOptions')
  wr.run()
  if sim:
    if not isinstance(sim,list):
      print "Simulator instances must be a list of slices:['Slice00','Slice01']"
      return
    sims = []
    for slice in sim:
      sim = StorageSimulator.Simulator(mgr,name+'_'+slice)
      sim.run()
      sims.append(sim)
    return (mgr,wr,ctrl,sensor,sims)
  return (mgr,wr,ctrl,sensor)
