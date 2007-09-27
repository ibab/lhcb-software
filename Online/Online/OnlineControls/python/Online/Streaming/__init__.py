""" Online module: Storage submodule

    @author M.Frank
    @date   1/3/2007

Example usage:

import Online.Streaming; Online.Streaming.install('Storage')

import Online.PVSS as PVSS; PVSS.setDebug(55);
import Online.Streaming; res=Online.Streaming.runStorage();
import Online.Streaming; res=Online.Streaming.runStorage(sim=['Slice00']);
import Online.Streaming; res=Online.Streaming.runMonitoring();
PVSS.setDebug(66)
"""

import sys, time
import Online.SetupParams as Params

def _mgr(name):
  "Access PVSS controls manager by name"
  import Online.PVSSSystems as Systems
  return Systems.controlsMgr(name)

def _dbg(val):
  "Adjust PVSS debug level"
  import Online.PVSS as PVSS;
  PVSS.setDebug(val);
  
def install(name='Storage'):
  "Install streaming system identified by name"
  import Online.Streaming.StorageInstaller as SOI
  SOI.install(name)

def uninstall(name='Storage'):
  "Uninstall streaming system identified by name"
  #import Online.Streaming.StorageInstaller as SOI
  #SOI.uninstall(name)
  return

def runHLTopts(name='HLT',sim=None):
  """
  Execute job options writer for the HLT farm.

   Arguments:
   @param name        System name
   @param sim         (Dummy)

   @author M.Frank
  """
  import Online.PVSS as PVSS
  import Online.AllocatorControl as Control
  import Online.RunInfoClasses.HLTFarm as RI
  import Online.JobOptions.OptionsWriter as JobOptions

  nam    = name+'JobOptions'
  info   = RI.HLTFarmInfoCreator()
  mgr    = _mgr(PVSS.defaultSystemName())
  writer = JobOptions.HLTOptionsWriter(mgr,nam,info)
  ctrl   = Control.Control(mgr,nam,'Writer',[writer]).run()
  return (ctrl,run(name,mgr,sim))
  
def runStorage(name='Storage',sim=None):
  """
  Execute Storage system streaming component to
   - allocate execution slots and
   - write job options.

   Arguments:
   @param name        System name
   @param sim         List of stream slices for task simulation simulated

   @author M.Frank
  """
  import Online.RunInfoClasses.Storage as RI
  import Online.AllocatorControl    as Control
  import Online.Streaming.Allocator as StreamAllocator
  import Online.JobOptions.OptionsWriter as JobOptions

  info     = RI.StorageInfoCreator()
  mgr      = _mgr(Params.storage_system_name)
  streamer = StreamAllocator.Allocator(mgr,name,info)
  writer   = JobOptions.StorageOptionsWriter(mgr,name+'JobOptions',info)
  ctrl = Control.Control(mgr,name,'Alloc',[streamer,writer]).run()
  return (ctrl,run(name,mgr,sim))

def runMonitoring(name='Monitoring',sim=None):
  """
  Execute Monitoring system streaming component to
   - allocate execution slots and
   - write job options.

   Arguments:
   @param name        System name
   @param sim         List of stream slices for task simulation simulated

   @author M.Frank
  """
  import Online.RunInfoClasses.Monitoring as RI
  import Online.AllocatorControl    as Control
  import Online.Streaming.Allocator as StreamAllocator
  import Online.JobOptions.OptionsWriter as JobOptions

  info     = RI.MonitoringInfoCreator('Storage')
  mgr      = _mgr(Params.monitor_system_name)
  streamer = StreamAllocator.Allocator(mgr,name,info)
  writer   = JobOptions.MonitoringOptionsWriter(mgr,name+'JobOptions',info)
  ctrl = Control.Control(mgr,name,'Alloc',[streamer,writer]).run()
  return (ctrl,run(name,mgr,sim))

def run(name,mgr,sim=None):
  """
  Start task simulators on the streaming layer(s)

   Arguments:
   @param name        System name
   @param mgr         Controls manager for this system
   @param sim         List of stream slices for task simulation simulated (if any)

   @author M.Frank
  """
  import Online.Streaming.Simulator as SIM
  sims = []
  if sim:
    if not isinstance(sim,list):
      print "Simulator instances must be a list of slices:['Slice00','Slice01']"
      return None
    for slice in sim:
      simulator = SIM.Simulator(mgr,name+'_'+slice)
      simulator.run()
      sims.append(simulator)
  return (mgr,sims)

def execute(args):
  "Argument dispatcher to start various streaming/option writers from the command line."
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

  function = None

  if inst == 1 and nam is None:
    print 'Need to specify Stream name for installation!'
    sys.exit(0)
  elif inst == 1:
    if typ == 'Storage':
      install(nam)
    else:
      print 'Unknown installation!'
  elif typ == 'Storage':
    function = runStorage
    res = runStorage(sim=sim)
  elif typ == 'Monitoring':
    function = runMonitoring
    res = runMonitoring(sim=sim)
  elif typ == 'HLTOptions':
    function = runHLTopts
  else:
    print 'Wrong action option set:',typ
    return

  if function and nam:
    res = function(name=nam,sim=sim)
  elif function:
    res = function(sim=sim)
    
  if res and wait != 0:
    import Online.PVSS as PVSS
    if not PVSS.batchMode(): print 'Hit CTRL-C to quit!'
    res[0].sleep()
    print '\n\nDone...\n\n'
    if res[1] and len(res[1])>0:
      for i in res[1][1]:
        del(i)

