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

def runSubfarm(project,name):
  """
   Arguments:
   @param name        System name
   @author M.Frank
  """
  import Online.AllocatorControl as Control
  import Online.ProcessorFarm.FarmDescriptor as Farm
  mgr = _mgr(project)
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
  import Online.AllocatorControl as Control
  import Online.ProcessorFarm.FarmDescriptor as Farm
  mgr  = _mgr(project)
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

  if typ == 'Subfarm':
    function = runSubfarm
  elif typ == 'Farm':
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
