from OnlineEnvBase import CondDBTag as condDBTag
from OnlineEnvBase import DDDBTag   as DDDBTag
from OnlineEnvBase import ConditionsMapping as ConditionsMapping

def _get_conddb_data(configuration):
  import os, sys, imp
  dir_name = os.path.dirname(configuration)
  nam,ext = os.path.splitext(os.path.basename(configuration))
  if not sys.modules.has_key(nam):
    if not dir_name in sys.path:
      print '[DEBUG] add python path:'+dir_name
      sys.path.insert(0,dir_name)
    mod = imp.find_module(nam,None)
    module=imp.load_module(nam,mod[0],mod[1],mod[2])
    return module
  return sys.modules[nam]

RunChangeHandlerConditions = {}
OnlineConditionsModule = None
if len(ConditionsMapping)>0:
  OnlineConditionsModule = _get_conddb_data(ConditionsMapping)
  RunChangeHandlerConditions = OnlineConditionsModule.ConditionMap
