#-------------------------------------------------------------------------------
#
#  Patch the OnlineEnv values according to the HLT2 parameters used.
#
#  \author  M.Frank
#  \version 1.0
#
#-------------------------------------------------------------------------------

#----------------- OnlineEnvBase default values: -------------------------------
# ---------------- General partition parameters:    
PartitionID              = 259
PartitionName            = "DQ"
Activity                 = "PHYSICS"
TAE                      = 0
OutputLevel              = 4
# ---------------- Trigger parameters:    
InitialTCK               = "0x80001710"
CondDBTag                = "cond-20120831"
DDDBTag                  = "dddb-20120831"
ConditionsMapping        = "/group/online/hlt/conditions/RunChangeHandler/Online.py"
# ---------------- DQ Specific stuff (dummies, but necessary): 
MooreOnlineVersion       = "dummy"
MooreVersion             = "dummy"
MooreStartupMode         = 1
# ---------------- DQ Specific stuff: 
RecoStartupMode          = 1
OnlineVersion            = "OnlineRelease"
OnlineBrunelVersion      = "DataQuality"
DataDirectory            = ""
EventsPerFile            = 0
AllowedRuns              = []
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def _setOnlineBaseAttr(name,value):
  import sys
  nodeInfo = sys.modules['OnlineEnvBase']
  setattr(nodeInfo,name,getattr(nodeInfo,value))

#-------------------------------------------------------------------------------
def _addData(module):
  import sys
  attrs = dir(module)
  nodeInfo = sys.modules['OnlineEnvBase']
  for i in attrs:
    if len(i)>2 and i[:2] != '__':
      setattr(nodeInfo,i,getattr(module,i))

#-------------------------------------------------------------------------------
def __importModule2OnlineEnv(configuration):
  import os, sys, imp
  cfg = os.path.abspath(configuration)
  dir_name = os.path.dirname(cfg)
  nam,ext = os.path.splitext(os.path.basename(cfg))
  if not sys.modules.has_key(nam):
    if not dir_name in sys.path:
      if os.environ.has_key('DQ_DEBUG'):
        print '[DEBUG] add python path:'+dir_name
      sys.path.insert(0,dir_name)
    mod = imp.find_module(nam,None)
    module=imp.load_module(nam,mod[0],mod[1],mod[2])
    _addData(module)
  return sys.modules[nam]

#-------------------------------------------------------------------------------
def __printAttrs(name):  
  import os, sys, types
  if os.environ.has_key('DQ_DEBUG'):
    print 50*'=',name,50*'='
    mod = sys.modules[name]
    attrs = dir(mod)
    for i in attrs:
      if len(i)>2 and i[:2] != '__':
        attr = getattr(mod,i)
        s = ''
        if isinstance(attr,str): s = '"'
        print name,':',str(i)+'='+s+str(attr)+s
  
#-------------------------------------------------------------------------------
def __patchValues():
  """
  Main entry point.
  The function requires access to the configuration module 'Config'.

  \author  M.Frank
  \version 1.0
  """
  import os
  try:
    import sys, Config
    c = Config.Config()
    mod = __importModule2OnlineEnv(c.optsDirectory+os.sep+'ReaderInput.py')
    runs = getattr(sys.modules['OnlineEnvBase'],'AllowedRuns')
    if len(runs)>0:
      run = runs[0]
      cfg = c.condDirectory+os.sep+str(run) + os.sep + 'HLT2Params.py'
      return __importModule2OnlineEnv(cfg)
  except Exception,X:
    pass
  if os.environ.has_key('DQ_DEBUG'):
    print '[INFO] No run to be processed take OnlineEnvBase defaults: ',str(X)+';'

#-------------------------------------------------------------------------------
#
#  Patch the values according to the HLT2Params
#
__patchValues()
__printAttrs('OnlineEnvBase')
#
#-------------------------------------------------------------------------------
