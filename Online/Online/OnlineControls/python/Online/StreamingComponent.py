"""
import Online.StreamingComponent as Strm
Strm.install('Storage')

"""
recvNodes=['storerecv01', 'storerecv02']
strmNodes=['storestrm01', 'storestrm02']

def install(name='Storage'):
  import Online.Streaming.StorageInstaller as SOI
  import Online.JobOptions.Installer as JOI
  SOI.install(name,recvNodes,strmNodes)
  JOI.install()

def uninstall(name='Storage'):
  #import Online.Streaming.StorageInstaller as SOI
  import Online.JobOptions.Installer as JOI
  #SOI.uninstall(name)
  JOI.uninstall()

def run(name='Storage',sim=None):
  import Online.PVSS as PVSS
  import Online.RunInfo.RunInfoCreator as RunInfoCreator
  import Online.JobOptions.JobOptionsControl as WR
  import Online.Streaming.StreamingControl as StreamingControl
  import Online.Streaming.Simulator as StreamingSimulator
  mgr = PVSS.controlsMgr()
  ctrl = StreamingControl.Control(mgr,name,RunInfoCreator(mgr))
  sensor=PVSS.DeviceSensor(mgr,ctrl.point)
  sensor.addListener(ctrl)
  sensor.run(1)
  wr = WR.StreamingOptionsWriter(mgr,'JobOptions')
  wr.run()
  if sim:
    if not isinstance(sim,list):
      print "Simulator instances must be a list of slices:['Slice00','Slice01']"
      return
    sims = []
    for slice in sim:
      sim = StreamingSimulator.Simulator(mgr,name+'_'+slice)
      sim.run()
      sims.append(sim)
    return (mgr,wr,ctrl,sensor,sims)
  return (mgr,wr,ctrl,sensor)
