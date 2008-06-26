#!/usr/bin/env python
"""
Script for the test of possible use-cases of the direct mapping between
the COOL hierarchy and the transient store one.
Use-cases:
 0) basic mapping (CondDBAccessSvc)
 1) alternative for a folderset in the main DB
 2) alternative for a folderset _not_ in the main DB
 3) alternative for a folder _not_ in the main DB
 4) layers
"""
__author__ = "Marco Clemencic"

def configure(version):
    from Gaudi.Configuration import (importOptions,
                                     ApplicationMgr,
                                     MessageSvc)
    importOptions("$DDDBROOT/options/DDDB.py")
    
    from DetCond.Configuration import (CondDBAccessSvc,CondDBCnvSvc,
                                       addCondDBAlternative,
                                       addCondDBLayer)
    
    DBs = []
    for i in range(3):
        data = { "name": "TESTDB%d"%i }
        DBs.append(CondDBAccessSvc(data["name"],
                                   ConnectionString="sqlite_file:../data/%(name)s.db/%(name)s"%data,
                                   EnableXMLDirectMapping = True))
    
    CondDBCnvSvc(CondDBReader = DBs[0])
    if version == 1:
        addCondDBAlternative(DBs[1],'/AutoMap/FolderSet2')
    elif version == 2:
        addCondDBAlternative(DBs[2],'/AutoMap/FolderSet3')
    elif version == 3:
        addCondDBAlternative(DBs[1],'/AutoMap/FolderSet2/ObjectA.xml')
    elif version == 4:
        addCondDBLayer(DBs[1])
        addCondDBLayer(DBs[2])
    elif version != 0:
        raise RuntimeError("Invalid version number")

    ApplicationMgr(TopAlg = ["LoadDDDB"], EvtSel = "NONE")
    #MessageSvc(OutputLevel = 1)

def datastore_walk(ds, top = "/"):
    if top == "/":
        top = ds._idm.rootName()
    obj = ds[top]
    if obj is not None:
        yield ds[top]
        nodes = [ i.identifier() for i in ds.leaves(ds[top]) ]
        for n in nodes:
            for obj in datastore_walk(ds,n):
                yield obj

def node_names(ds, top = "/"):
    return [ obj.registry().identifier() for obj in datastore_walk(ds,top) ]

def main(conf):
    configure(conf)
    
    from Gaudi.Configuration import configurationDict
    from pprint import pprint
    pprint(configurationDict())
    
    import GaudiPython
    app = GaudiPython.AppMgr()
    app.initialize()
    
    dds = app.detsvc()
    # load everything in the store
    nodes = node_names(dds)
    nodes.sort()
    
    print "=== Begin Nodes ==="
    for n in nodes:
        print n
    print "=== End Nodes ==="
    
if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2:
        version = 0
    else:
        version = int(sys.argv[1])
    
    main(version)