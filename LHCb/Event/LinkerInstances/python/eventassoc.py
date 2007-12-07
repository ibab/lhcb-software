import GaudiPython

def linkedTo( t1, t2, location ):
  appMgr = GaudiPython.AppMgr()
  cl = GaudiPython.gbl.LinkedTo( t1, t2)
  lt = cl( appMgr.evtsvc()._idp, None, location )
  return lt

def linkedFrom( t1, t2, location ):
  appMgr = GaudiPython.AppMgr()
  cl = GaudiPython.gbl.LinkedFrom( t1, t2, int)
  lt = cl( appMgr.evtsvc()._idp, None, location )
  return lt
