"""
     DataQuality monitoring parameter configuration

     \author  M.Frank
     \version 1.0
"""
#-------------------------------------------------------------------------------
def _addData(module):
  import sys
  attrs = dir(module)
  ##print sys.modules
  nodeInfo = sys.modules['DataQualityScan.Config']
  for i in attrs:
    if len(i)>2 and i[:2] != '__':
      setattr(nodeInfo,i,getattr(module,i))

#-------------------------------------------------------------------------------
def __importModule2Config(configuration):
  import os, sys, imp
  cfg = os.path.abspath(configuration)
  dir_name = os.path.dirname(cfg)
  nam,ext = os.path.splitext(os.path.basename(cfg))
  if not sys.modules.has_key(nam):
    if not dir_name in sys.path:
      if os.environ.has_key('DQ_DEBUG'):
        print 'echo "[DEBUG] add python path:'+dir_name+'";'
      sys.path.insert(0,dir_name)
    mod = imp.find_module(nam,None)
    module=imp.load_module(nam,mod[0],mod[1],mod[2])
    _addData(module)
  return sys.modules[nam]

def __load_config():
  import os
  if os.environ.has_key('DQ_CONFIG'):
    __importModule2Config(os.environ['DQ_CONFIG'])
  else:
    name = os.environ['DATAQUALITYROOT']+os.sep+'python'+\
        os.sep+'DataQualityScan'+os.sep+'DQ_Config.py'
    #print 'echo "[INFO] Import module '+name+'";'
    __importModule2Config(name)
    
__load_config()
