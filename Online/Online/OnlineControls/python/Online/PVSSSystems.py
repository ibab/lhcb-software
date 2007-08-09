import Online.PVSS as PVSS

_systems = {}
_sysIds  = {}


# =============================================================================
def controlsMgr(name):
  "Access controls manager of known PVSS systems by name"
  if name is not None:
    #if name != 'LBECS':
      if _systems.has_key(name):
        return _systems[name][1]
      # Hack for now ....
      if name[:3]=='ECS':
        print 'PVSS System switch:',name[:3],name,'--> LBECS'
        name = 'LBECS'        
      id,nam = PVSS.system(name)
      if _sysIds.has_key(id):
        ctrl = _sysIds[id][1]
      else:
        ctrl = PVSS.controlsMgr(id,nam)

      if not _systems.has_key(name):
        _systems[name] = (id,ctrl)
      if not _systems.has_key(nam):
        _systems[nam] = (id,ctrl)
      if not _sysIds.has_key(id):
        _sysIds[id]    = (name,ctrl)
      return _systems[nam][1]
    #raise Exception,'Invalid system name specified:'+name
  raise Exception,'Invalid system name specified!'

# =============================================================================
def reload():
  pass

# =============================================================================
def clear():
  _systems = {}
  _sysIds = {}
  
