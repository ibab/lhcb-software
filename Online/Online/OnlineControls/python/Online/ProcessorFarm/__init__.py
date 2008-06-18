"""

Online PVSS module for prompt reconstruction

@author M.Frank

"""
import sys, time
import Online.SetupParams as Params

def _mgr(name):
  "Access PVSS controls manager by name"
  import Online.PVSS as PVSS;
  import Online.PVSSSystems as Systems
  if name is None:
    return PVSS.controlsMgr()
  return Systems.controlsMgr(name)

def _dbg(val):
  "Adjust PVSS debug level"
  import Online.PVSS as PVSS;
  PVSS.setDebug(val);

  
def runTheStorage(name,mgr,sim):
  """
  Execute Storage system streaming component to
   - allocate execution slots and
   - write job options.

   Arguments:
   @param name        System name
   @param mgr         PVSS controls manager
   @param sim         List of stream slices for task simulation simulated

   @author M.Frank
  """
  import Online.RunInfoClasses.Storage as RI
  import Online.AllocatorControl    as Control
  import Online.Streaming.Allocator as StreamAllocator
  # import Online.JobOptions.OptionsWriter as JobOptions

  info     = RI.StorageInfoCreator()
  streamer = StreamAllocator.Allocator(mgr,name,info)
  #writer   = JobOptions.StorageOptionsWriter(mgr,name,info)
  #ctrl = Control.Control(mgr,name,'Alloc',[streamer,writer]).run()
  ctrl = Control.Control(mgr,name,'Alloc',[streamer]).run()
  return (ctrl,run(name,mgr,sim))

def runStorage(project,name='Storage',sim=None):
  return runTheStorage(name,_mgr(Params.recstorage_system_name),sim)

def runSubfarm(project,name):
  """
   Arguments:
   @param name        System name
   @author M.Frank
  """
  import Online.PVSS as PVSS;
  import Online.AllocatorControl as Control
  import Online.ProcessorFarm.FarmDescriptor as Farm
  mgr = _mgr(project)
  if name=='Unknown':
    rdr = mgr.devReader()
    actor = PVSS.DpVectorActor(mgr)
    typ   = mgr.typeMgr().type('FarmSubInfo')
    actor.lookupOriginal('*.Name',typ)
    if len(actor.container)>0:
      rdr.add(actor.container)
      if rdr.execute():
        name = actor.container[0].data
  print '---> Starting controller for subfarm:',name
  items = name.split('_')
  subfarm = Farm.SubFarmConfigurator(mgr,name)
  ctrl = Control.Control(mgr,name,'Alloc',[subfarm]).run()
  return (ctrl,mgr)

def runFarm(project,name):
  """
   Arguments:
   @param name        System name
   @author M.Frank
  """
  import Online.PVSS as PVSS;
  import Online.AllocatorControl as Control
  import Online.ProcessorFarm.FarmDescriptor as Farm
  mgr  = _mgr(project)
  if name=='Unknown':
    rdr = mgr.devReader()
    actor = PVSS.DpVectorActor(mgr)
    typ   = mgr.typeMgr().type('FarmInfo')
    actor.lookupOriginal('*.Name',typ)
    if len(actor.container)>0:
      rdr.add(actor.container)
      if rdr.execute():
        name = actor.container[0].data
  print '---> Starting controller for farm:',name

  farm = Farm.FarmConfigurator(mgr,name)
  ctrl = Control.Control(mgr,name,'Alloc',[farm]).run()
  return (ctrl,mgr)

def execute(args):
  "Argument dispatcher to start various streaming/option writers from the command line."
  wait = 0
  typ = 'Storage'
  nam = None
  res = None
  pro = None
  function = None
  i = -1
  while((i+1)<len(args)):
    i = i + 1
    if args[i] == "-type":
      typ = args[i+1]
      i = i + 1
    elif args[i] == "-debug":
      _dbg(int(args[i+1]))
      i = i + 1
    elif args[i] == "-name":
      nam = args[i+1]
      i = i + 1
    elif args[i] == "-project":
      pro = args[i+1]
      i = i + 1
    elif args[i] == "-function":
      function = args[i+1]
      i = i + 1
    elif args[i] == "-wait":
      wait = 1
    else:
      print 'Ignored option ',args[i]

  if typ == 'Storage':
    function = runStorage
  elif typ == 'Subfarm':
    if nam is None: nam = 'Unknown'
    function = runSubfarm
  elif typ == 'Farm':
    if nam is None: nam = 'Unknown'
    function = runFarm
  elif function is None:
    print 'Unknown action requested.'
    return None

  if function and nam:
    res = function(project=pro,name=nam)
    
  if res and wait != 0:
    import Online.PVSS as PVSS
    if not PVSS.batchMode(): print 'Hit CTRL-C to quit!'
    res[0].sleep()
    print '\n\nDone...\n\n'
  return 1
