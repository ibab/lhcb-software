import sys, OnlineEnvBase
nodeInfo = sys.modules['OnlineEnvBase']

def load_node_info():
  import os, imp, socket
  dir_name = os.path.dirname(os.path.dirname(imp.find_module('OnlineEnvBase')[1])) + os.sep + 'Nodes'
  nod_name = socket.gethostname().upper()
  nod_idx  = nod_name.find(".")
  if nod_idx>0: nod_name = nod_name[:nod_idx]
  mod_name = nod_name+'_TriggerInfo.py'
  nam,ext = os.path.splitext(mod_name)
  if not sys.modules.has_key(nam):
    if not dir_name in sys.path:
      print '[DEBUG] add python path:'+dir_name
      sys.path.insert(0,dir_name)
    mod = imp.find_module(nam,None)
    module=imp.load_module(nam,mod[0],mod[1],mod[2])
    attrs = dir(module)
    for i in attrs:
      if len(i)>2 and i[:2] != '__':
        setattr(nodeInfo,i,getattr(module,i))
  return dir(sys.modules[nam])

def printattrs(name):
  print 50*'=',name,50*'='
  mod = sys.modules[name]
  attrs = dir(mod)
  for i in attrs:
    if len(i)>2 and i[:2] != '__':
      print str(i)+'='+str(getattr(mod,i))
