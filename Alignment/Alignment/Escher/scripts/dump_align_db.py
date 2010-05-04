#!/usr/bin/env python
__author__  = "Wouter Hulsbergen"
__version__ = ""

#apparently this needs to happen before Gaudi/Pool is loaded
from ROOT import TGraph, TCanvas

import os,time
import CondDBUI
import CondDBUI.Admin
from PyCool import cool
import GaudiPython
from GaudiPython.Bindings import gbl

from Gaudi.Configuration import *

def dumpToFile( element, output ) :
    geometryinfo = element.geometry()
    if geometryinfo:
        aligncondition = geometryinfo.alignmentCondition()
        if aligncondition:
            origin = gbl.Gaudi.XYZPoint()
            globalcenter = geometryinfo.toGlobal( origin )
            # compute the total delta 
            globaltransform        = geometryinfo.toGlobalMatrix()
            globalnominaltransform = geometryinfo.toGlobalMatrixNominal()
            globaltotaldelta = globaltransform * globalnominaltransform.Inverse()
            totaltranslations = GaudiPython.gbl.vector('double')(3)
            totalrotations = GaudiPython.gbl.vector('double')(3)
            gbl.DetDesc.getZYXTransformParameters(globaltotaldelta, totaltranslations, totalrotations);
            # compute the local delta (in the local fraem for now)
            localdelta   = geometryinfo.ownToOffNominalMatrix()
            localtranslations = GaudiPython.gbl.vector('double')(3)
            localrotations = GaudiPython.gbl.vector('double')(3)
            gbl.DetDesc.getZYXTransformParameters(localdelta, localtranslations, localrotations);
            line = element.name() 
            line += " : " + aligncondition.name() + " : "
            line += str( globalcenter.x() ) + " " +  str( globalcenter.y() ) + " " +  str( globalcenter.z() ) + " : "
            line += str( totaltranslations[0] ) + " " + str( totaltranslations[1] ) + " "+ str( totaltranslations[2] ) + " " 
            line += str( totalrotations[0] ) + " " + str( totalrotations[1] ) + " "+ str( totalrotations[2] ) + " : " 
            line += str( localtranslations[0] ) + " " + str( localtranslations[1] ) + " "+ str( localtranslations[2] ) + " " 
            line += str( localrotations[0] ) + " " + str( localrotations[1] ) + " "+ str( localrotations[2] )
            line += '\n'
            output.write( line )
    for daughter in element.childIDetectorElements() :
        dumpToFile( daughter, output )
 

def main():
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] database element-name",
                          version = __version__,
                          description =
                          """This script produces a plot of the value of an alignment condition versus time. For example\n
                          drawGlobalPosition.py /afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/IOVTest/LocalDDDB.db /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight""")
    parser.add_option("-s", "--sqlitefilename",
                      dest="sqlitefilename", type="string",
                      help="Name of the an LHCBCOND sqlitefile to be loaded on top of the default connection [default: %default]",
                      default="None")
    parser.add_option("-T", "--tag",
                      action="store_true", dest="tag",
                      help="tag for the snapshot [default: %default]",
                      default="HEAD")
    parser.add_option("-t", "--time",
                      dest="time", type="int",
                      help="validity time in seconds [default: now]",
                      default=-1)
    parser.add_option("-d", "--aligndb",action="append", dest="aligndb",help="path to file with alignment database layer. you can use this option multiple times")
    (opts, args) = parser.parse_args()

    outputfilename = args[0]

    # we'll count everything in seconds
    nsPerSecond  = 1000000000
    from PyCool import cool
    from GaudiPython.Bindings import gbl
    # 1 september:  1231459200
    # 15 september: 1268092800
    
    from Configurables import LHCbApp, ApplicationMgr
    LHCbApp().DDDBtag   = "default"
    LHCbApp().CondDBtag = "HEAD"
    if opts.time>0:
        from Configurables import EventClockSvc
        print "Time:  ", opts.time, " --> ", gbl.Gaudi.Time(opts.time).format(True,"%c")
        EventClockSvc().InitialTime = opt.time
        
    if opts.aligndb:
        from Configurables import ( CondDB, CondDBAccessSvc )
        counter = 1
        for db in opts.aligndb:
            alignCond = CondDBAccessSvc( 'AlignCond' + str(counter) )
            alignCond.ConnectionString = 'sqlite_file:' + db + '/LHCBCOND'
            CondDB().addLayer( alignCond )
            counter += 1

    appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'myBrunel' )
    appMgr = GaudiPython.AppMgr()

    parentelements = [
        '/dd/Structure/LHCb/BeforeMagnetRegion/Velo',
        '/dd/Structure/LHCb/BeforeMagnetRegion/TT',
        '/dd/Structure/LHCb/AfterMagnetRegion/T/IT',
        '/dd/Structure/LHCb/AfterMagnetRegion/T/OT'
        #'/dd/Structure/LHCb/DownstreamRegion/Muon'
        ]
    
    outputfile = open(outputfilename,'w')

    for parentelementname in parentelements:
        parentelement = appMgr.detSvc()[parentelementname]
        dumpToFile( parentelement, outputfile )

    outputfile.close()
    
if __name__ == '__main__':
    main()
