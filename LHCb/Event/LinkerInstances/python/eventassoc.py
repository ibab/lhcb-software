import gaudimodule
gaudimodule.loaddict('LinkerInstancesDict')

def linkedTo( t1, t2, location ):
  appMgr = gaudimodule.AppMgr()
  cl = gaudimodule.gbl.LinkedTo( t1, t2 )
  lt = cl( appMgr.evtsvc()._idp, None, location )
  return lt

def linkedFrom( t1, t2, location ):
  appMgr = gaudimodule.AppMgr()
  cl = gaudimodule.gbl.LinkedFrom( t1, t2 )
  lt = cl( appMgr.evtsvc()._idp, None, location )
  return lt
