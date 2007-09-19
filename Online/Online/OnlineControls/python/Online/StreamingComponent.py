"""
import Online.StreamingComponent as Strm; Strm.install('Storage')

import Online.PVSS as PVSS; PVSS.setDebug(55);
import Online.StreamingComponent as Strm; res=Strm.runStorage();  import sys;sys.stdin.readline();
import Online.StreamingComponent as Strm; res=Strm.runStorage(sim=['Slice00']);
import Online.StreamingComponent as Strm; res=Strm.runMonitoring();
PVSS.setDebug(66)
"""

import sys, time
import Online.SetupParams as Params

def _mgr(name):
  import Online.PVSSSystems as Systems
  return Systems.controlsMgr(name)

def _dbg(val):
  import Online.PVSS as PVSS;
  PVSS.setDebug(val);
  
def install(name='Storage'):
  import Online.Streaming.StorageInstaller as SOI
  SOI.install(name)

def uninstall(name='Storage'):
  #import Online.Streaming.StorageInstaller as SOI
  #SOI.uninstall(name)
  return

def runHLTopts(name='HLT'):
  import Online.RunInfoClasses.HLTFarm as RI
  import Online.AllocatorControl    as Control
  import Online.JobOptions.OptionsWriter as JobOptions

  info = RI.HLTFarmInfoCreator()
  system = Params.storage_system_name
  system = Params.jobOptionsSystemName()
  mgr = _mgr(system)
  nam = name+'JobOptions'
  writer = JobOptions.HLTOptionsWriter(mgr,nam,RI.HLTFarmInfoCreator())
  writer.optionsDir = Params.jobopts_optsdir
  ctrl = Control.Control(mgr,nam,'Writer',[writer])
  ctrl.run()
  return (ctrl,writer)
  
def runStorage(name='Storage',sim=None):
  import Online.RunInfoClasses.Storage as RI
  import Online.AllocatorControl    as Control
  import Online.Streaming.Allocator as StreamAllocator
  import Online.JobOptions.OptionsWriter as JobOptions

  info = RI.StorageInfoCreator()
  system = Params.storage_system_name
  mgr = _mgr(system)
  streamer = StreamAllocator.Allocator(mgr,name,info)
  writer   = JobOptions.StorageOptionsWriter(mgr,name+'JobOptions',info)
  writer.optionsDir = Params.jobopts_optsdir

  ctrl = Control.Control(mgr,name,'Alloc',[streamer,writer])
  ctrl.run()
  return (ctrl,run(name,mgr,info,sim))

def runMonitoring(name='Monitoring',sim=None):
  import Online.RunInfoClasses.Monitoring as RI
  import Online.AllocatorControl    as Control
  import Online.Streaming.Allocator as StreamAllocator
  import Online.JobOptions.OptionsWriter as JobOptions

  info = RI.MonitoringInfoCreator('Storage')
  mgr  = _mgr(Params.monitor_system_name)
  streamer = StreamAllocator.Allocator(mgr,name,info)
  writer   = JobOptions.MonitoringOptionsWriter(mgr,name+'JobOptions',info)
  writer.optionsDir = Params.jobopts_optsdir

  ctrl = Control.Control(mgr,name,'Alloc',[streamer,writer])
  ctrl.run()
  return (ctrl,run(name,mgr,info,sim))

def run(name,mgr,info_creator,sim=None):
  import Online.Streaming.Simulator as SIM
  sims = []
  if sim:
    if not isinstance(sim,list):
      print "Simulator instances must be a list of slices:['Slice00','Slice01']"
      return
    for slice in sim:
      simulator = SIM.Simulator(mgr,name+'_'+slice)
      simulator.run()
      sims.append(simulator)
  return (mgr,sims)

def execute(args):
  sim = []
  wait = 0
  inst = 0
  typ = 'Storage'
  nam = None
  res = None
  for i in xrange(len(args)):
    if args[i] == "-sim":
      sim.append(args[i+1])
      i = i + 1
    elif args[i] == "-type":
      typ = args[i+1]
      i = i + 1
    elif args[i] == "-debug":
      _dbg(int(args[i+1]))
      i = i + 1
    elif args[i] == "-wait":
      wait = 1
    elif args[i] == "-name":
      nam = args[i+1]
      i = i + 1
    elif args[i] == "-install":
      inst = 1

  if len(sim)==0: sim=None

  if inst == 1 and nam is None:
    print 'Need to specify Stream name for installation!'
    sys.exit(0)
  elif inst == 1:
    if typ == 'Storage':
      install(nam)
    else:
      print 'Unknown installation!'
  elif typ == 'Storage':
    res = runStorage(sim=sim)
  elif typ == 'Monitoring':
    res = runMonitoring(sim=sim)
  else:
    print 'Wrong action option set:',typ
    
  if res and wait != 0:
    print 'Hit CTRL-C to quit!'
    res[0].sleep()
    print '\n\nDone...\n\n'
    for i in res[1][1]:
      del(i)

if __name__ == "__main__":
  execute(args=sys.argv[1:])

