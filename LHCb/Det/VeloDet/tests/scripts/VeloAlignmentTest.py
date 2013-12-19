#!/usr/bin/env python
"""
Test the
"""
__author__ = "Marco Clemencic"

ConditionPaths =  ["/dd/Conditions/Online/Velo/MotionSystem",
                   "/dd/Conditions/Alignment/Velo/VeloLeft",
                   "/dd/Conditions/Alignment/Velo/VeloRight"]

use_cases = { "simple": "basic/plain geometry",
              "override_motion_system": "change the Velo position overriding motion system data",
              "override_alignment": "change the Velo position overriding alignment condition",
              "override_alignment2": "override [XY]OffsetCoeffs of alignment condition",
              }


def configure(use_case = "simple"):
    # Common configuration
    import Gaudi.Configuration
    from Configurables import (ApplicationMgr, MessageSvc,
                               DDDBConf, CondDB, CondDBAccessSvc,
                               UpdateManagerSvc)
    DDDBConf() # detector description
    localDb = CondDBAccessSvc("VeloAlignCondTestDB",
                              ConnectionString = "sqlite_file:../data/VeloAlignCondTest.db/DDDB",
                              DefaultTAG = "simple")
    CondDB().addLayer(localDb) # use local DB

    ApplicationMgr(TopAlg = [], EvtSel = "NONE")
    #MessageSvc(OutputLevel = 1)

    if use_case == "simple":
        # nothing to do more
        return
    elif use_case == "override_motion_system":
        # Example of how to move the Velo
        UpdateManagerSvc().ConditionsOverride.append("Conditions/Online/Velo/MotionSystem := double ResolPosY = 100")
    elif use_case == "override_alignment":
        UpdateManagerSvc().ConditionsOverride.append("Conditions/Alignment/Velo/VeloLeft := double_v dPosXYZ = 0 100 0")
    elif use_case == "override_alignment2":
        UpdateManagerSvc().ConditionsOverride.append("Conditions/Alignment/Velo/VeloLeft := double_v YOffsetCoeffs = 20 -5")
        UpdateManagerSvc().ConditionsOverride.append("Conditions/Alignment/Velo/VeloRight := double_v YOffsetCoeffs = 0 1")

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

def main(use_case = "simple"):
    if use_case not in use_cases:
        print "ERROR: uknown use case to test '%s'" % use_case
        return 2
    screenwidth = 120
    print "#" * screenwidth
    txt_use_case = "testing use case '%s': %s" % (use_case, use_cases[use_case])
    print " " * max(0,(screenwidth - len(txt_use_case))/2) + txt_use_case
    print "#" * screenwidth
    configure(use_case)

    from Gaudi.Configuration import configurationDict
    from pprint import pprint

    import ROOT
    Math = ROOT.ROOT.Math

    import GaudiPython
    app = GaudiPython.AppMgr()
    pprint(configurationDict())
    app.initialize()
    app.start()

    dds = app.detsvc()
    # load everything in the store
    nodes = node_names(dds)
    app.run(1)

    print "=== Begin Conditions ==="
    for n in ConditionPaths:
        print n
        print dds[n]
    print "=== End Conditions ==="

    if use_case == "simple":
        expected = { "Left": Math.Transform3D(1, 0, 0, 30,
                                              0, 1, 0, 1,
                                              0, 0, 1, 0),
                     "Right": Math.Transform3D(1, 0, 0, -30,
                                               0, 1, 0, 15,
                                               0, 0, 1, 0) }
    elif use_case == "override_motion_system":
        expected = { "Left": Math.Transform3D(1, 0, 0, 30,
                                              0, 1, 0, 100,
                                              0, 0, 1, 0),
                     "Right": Math.Transform3D(1, 0, 0, -30,
                                               0, 1, 0, -480,
                                               0, 0, 1, 0) }
    elif use_case == "override_alignment":
        expected = { "Left": Math.Transform3D(1, 0, 0, 30,
                                              0, 1, 0, 101,
                                              0, 0, 1, 0),
                     "Right": Math.Transform3D(1, 0, 0, -30,
                                               0, 1, 0, 15,
                                               0, 0, 1, 0) }
    elif use_case == "override_alignment2":
        expected = { "Left": Math.Transform3D(1, 0, 0, 30,
                                              0, 1, 0, 15,
                                              0, 0, 1, 0),
                     "Right": Math.Transform3D(1, 0, 0, -30,
                                               0, 1, 0, 1,
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
    from sys import exit, argv
    if len(argv) > 1:
        use_case = argv[1]
    else:
        use_case = "simple"
    exit(main(use_case))
