#!/usr/bin/env python
"""
Test the 
"""
__author__ = "Marco Clemencic"

ConditionPaths =  ["/dd/Conditions/Online/Velo/MotionSystem",
                   "/dd/Conditions/Alignment/Velo/VeloLeft",
                   "/dd/Conditions/Alignment/Velo/VeloRight"]

def configure():
    from Gaudi.Configuration import (importOptions,
                                     ApplicationMgr,
                                     MessageSvc,
                                     DetectorDataSvc)
    importOptions("$DDDBROOT/options/DDDB.py")
    DetectorDataSvc().DetDbLocation = "../data/lhcb.xml"
    
    # This is needed to trigger the instantiation of the update manager service 
    from Configurables import DetCondTest__TestConditionAlg
    alg = DetCondTest__TestConditionAlg()
    alg.Conditions = ConditionPaths
    
    ApplicationMgr(TopAlg = [alg], EvtSel = "NONE")
    #MessageSvc(OutputLevel = 1)
    
    # Example of how to move the Velo
    #from Configurables import UpdateManagerSvc
    #UpdateManagerSvc().ConditionsOverride.append("Conditions/Online/Velo/MotionSystem := double YOffset = 100")

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

def main():
    configure()
    
    from Gaudi.Configuration import configurationDict
    from pprint import pprint
    pprint(configurationDict())
    
    import ROOT
    ROOT.gSystem.Load("libMathCore")
    
    import GaudiPython
    app = GaudiPython.AppMgr()
    app.initialize()
    
    dds = app.detsvc()
    # load everything in the store
    nodes = node_names(dds)

    print "=== Begin Conditions ==="
    for n in ConditionPaths:
        print n
        print dds[n]
    print "=== End Conditions ==="
    
    expected = { "Left": ROOT.Math.Transform3D(1, 0, 0, 30,
                                               0, 1, 0, 1,
                                               0, 0, 1, 0),
                 "Right": ROOT.Math.Transform3D(1, 0, 0, -30,
                                                0, 1, 0, 15,
                                                0, 0, 1, 0) }

    result = 0 # success
    for i in expected:
        path = "/dd/Conditions/Alignment/Velo/Velo%s"%i
        m = dds[path].offNominalMatrix()
        if m != expected[i]:
            print "ERROR: transforamtion for %s is not what expected" % path
            result = 1
    
    return result

if __name__ == '__main__':
    from sys import exit
    exit(main())
