#!/usr/bin/env python
__author__  = "Wouter Hulsbergen"
__version__ = ""

#apparently this needs to happen before Gaudi/Pool is loaded
from ROOT import TGraph, TCanvas
from ROOT import *

import os,time,math
import CondDBUI
import CondDBUI.Admin
from PyCool import cool
import GaudiPython
from GaudiPython.Bindings import gbl 

from Gaudi.Configuration import *

def dumpToFile( element, output ,callback = lambda a,c,b,d:0) :
    geometryinfo = element.geometry()
    if geometryinfo:
        aligncondition = geometryinfo.alignmentCondition()
        if aligncondition:
            # get the nominal transform
            globalnominaltransform = geometryinfo.toGlobalMatrixNominal()
            # get the nominal position in the global frame
            globalorigin = gbl.Gaudi.XYZPoint()
            #globalcenter = geometryinfo.toGlobal( origin )
            globalcenter = globalnominaltransform * globalorigin
            globalnominaltranslation = gbl.Gaudi.Transform3D(globalnominaltransform.Translation())
            
            # compute the total delta in the global frame 
            globaltransform        = geometryinfo.toGlobalMatrix()
            globalnominaltransform = geometryinfo.toGlobalMatrixNominal()
            globaltotaldelta = globaltransform * globalnominaltransform.Inverse()

            # now transform it to the local frame
            localtotaldelta  = globalnominaltranslation.Inverse() * globaltotaldelta * globalnominaltranslation
            totaltranslations = GaudiPython.gbl.vector('double')(3)
            totalrotations = GaudiPython.gbl.vector('double')(3)
            gbl.DetDesc.getZYXTransformParameters(localtotaldelta, totaltranslations, totalrotations);

            # now the same for the local delta. first transform it to the global frame.
            # this is the local delta:
            localdelta   = geometryinfo.ownToOffNominalMatrix()
            # note that toGlobalMatrix includes the local delta:  however, the transform to the global frame is still given by:
            globallocaldelta = globaltransform * localdelta * globaltransform.Inverse()
            # now transform it to our new local frame
            locallocaldelta =  globalnominaltranslation.Inverse() * globallocaldelta * globalnominaltranslation
            localtranslations = GaudiPython.gbl.vector('double')(3)
            localrotations = GaudiPython.gbl.vector('double')(3)
            gbl.DetDesc.getZYXTransformParameters(locallocaldelta, localtranslations, localrotations)
             
            ex =  gbl.Gaudi.XYZPoint(1,0,0)
            #mex = gbl.Gaudi.XYZPoint(-1,0,0)
            #print ex.Phi() #prints 0 
            #print mex.Phi() #prints pi            
            discr = (geometryinfo.toGlobalMatrixNominal().Rotation()*ex).Phi()/math.pi
            rotations = [0,-0.0277777777778,0.0277775985909, 0.972222222222,-0.972222401409,1,-1]
            pattern = [fabs (discr-x)<0.001 for x in rotations]
            if True not in pattern:
                print "Error: unknown rotation" , discr 
            if True in pattern[3:7]:
                RotZ180deg =  ROOT.Math.Transform3D(ROOT.Math.RotationZ(math.pi))
                localdelta = localdelta * RotZ180deg

            line = element.name() 
            line += " : " + aligncondition.name() + " : "
            line += str( globalcenter.x() ) + " " +  str( globalcenter.y() ) + " " +  str( globalcenter.z() ) + " : "
            line += str( totaltranslations[0] ) + " " + str( totaltranslations[1] ) + " "+ str( totaltranslations[2] ) + " " 
            line += str( totalrotations[0] ) + " " + str( totalrotations[1] ) + " "+ str( totalrotations[2] ) + " : " 
            line += str( localtranslations[0] ) + " " + str( localtranslations[1] ) + " "+ str( localtranslations[2] ) + " " 
            line += str( localrotations[0] ) + " " + str( localrotations[1] ) + " "+ str( localrotations[2] )
            line += '\n'
            output.write( line )

#            if plots != None:
#                DetObj = DetectorObject(aligncondition.name(),
#                                        globalcenter.x(),globalcenter.y(),globalcenter.z(),
#                                        totaltranslations[0],totaltranslations[1],totaltranslations[2])
#                map(lambda x:x.Fill(DetObj),plots)
            callback(aligncondition.name(),globalcenter,totaltranslations,localdelta)
    for daughter in element.childIDetectorElements() :
        dumpToFile( daughter, output ,callback)
 

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
    parser.add_option("-p","--plot",type="string",default=None,dest="plot",help="generate a lot of plots.")
    #parser.add_option("-v","--verbose",action="store_true",dest="verbose",default=False,
    #                  help="make a lot o noise!")

    (opts, args) = parser.parse_args()
    global plots
    plots =  None
    #global verbose
    #verbose = opts.verbose
    outputfilename = args[0]
    CallDumpToFile(outputfilename,opts)

def CallDumpToFile(outputfilename,opts,callback = lambda a,c,b,d:0):
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
    print "started AppMgr"
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
        dumpToFile( parentelement, outputfile ,callback)
    outputfile.close()


#    if plots:
#        map(lambda p:p.save(),plots)
#    outputfile.close()
    
if __name__ == '__main__':
    main()
