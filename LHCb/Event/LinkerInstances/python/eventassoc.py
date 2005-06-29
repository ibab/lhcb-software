import gaudimodule

import PyLCGDict
PyLCGDict.loadDict('LinkerInstancesDict')

def linkedTo(t1, t2, location):
  gaudi = gaudimodule.AppMgr()
  lt = gaudimodule.gbl.LinkedTo(t1,t2,'KeyedContainer<%s,Containers::KeyedObjectManager<Containers::hashmap> >'%t1.__name__)(gaudi.evtsvc()._idp,None,location)
  return lt
