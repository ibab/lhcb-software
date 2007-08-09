"""
import Online.StreamingComponent as Strm; Strm.install('Storage')

import Online.PVSS as PVSS; PVSS.setDebug(55);
import Online.StreamingComponent as Strm; res=Strm.runStorage();
import Online.StreamingComponent as Strm; res=Strm.runMonitoring();
PVSS.setDebug(66)
"""
import Online.SetupParams as Params
import Online.PVSSSystems as Systems

def install(name='Storage'):
  import Online.Streaming.StorageInstaller as SOI
  SOI.install(name)

def uninstall(name='Storage'):
  #import Online.Streaming.StorageInstaller as SOI
  #SOI.uninstall(name)
  return

def runStorage(name='Storage',sim=None):
  import Online.RunInfo as RI
  import Online.JobOptions.JobOptionsControl as WR  
  info = RI.RunInfoCreator()
  system = Params.storage_system_name
  system_mgr = Systems.controlsMgr(system)
  wr = WR.StorageOptionsWriter(system_mgr,name+'JobOptions',info)
  if wr:
    wr.optionsDir = Params.jobopts_optsdir
    wr.run()
  return (wr,run(name,system_mgr,info,sim))

def runMonitoring(name='Monitoring',sim=None):
  import Online.RunInfoClasses.Monitoring as MI
  import Online.JobOptions.JobOptionsControl as WR
  info = MI.MonitoringInfoCreator('Storage')
  system = Params.monitor_system_name
  system_mgr = Systems.controlsMgr(system)
  wr = WR.MonitoringOptionsWriter(system_mgr,name+'JobOptions',info)
  if wr:
    wr.optionsDir = Params.jobopts_optsdir
    wr.run()
  return (wr,run('Monitoring',system_mgr,info,sim))

def run(name,system_mgr,info_creator,sim=None):
  import Online.PVSS as PVSS
  import Online.Streaming.Simulator as SIM
  import Online.Streaming.StreamingControl as CTRL

  ctrl = CTRL.Control(system_mgr,name,info_creator)
  ctrl.run()
  if sim:
    if not isinstance(sim,list):
      print "Simulator instances must be a list of slices:['Slice00','Slice01']"
      return
    sims = []
    for slice in sim:
      simulator = SIM.Simulator(system_mgr,name+'_'+slice)
      simulator.run()
      sims.append(simulator)
    return (system_mgr,ctrl,sims)
  return (system_mgr,ctrl)
