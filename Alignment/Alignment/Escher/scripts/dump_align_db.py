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


class DetectorObject:
    def __init__(self,name,x,y,z,dx,dy,dz):

        self.name = name

        self.X  =  x
        self.Y  =  y
        self.Z  =  z
        
        self.TX = dx
        self.TY = dy
        self.TZ = dz

    def phi(self):
        """
        polar angle
        """
        if (self.X!=0):
            return math.atan(self.Y/self.X)
        else:
            return 0.0

    def rho(self):
        """
        distance from z axis
        """
        return math.sqrt(self.X**2+self.Y**2)

    def theta(self):
        if self.Z!=0:
            return math.atan(self.rho()/self.Z)

    def eta(self):
        """
        pseudorapidity
        """
        return -math.log(math.tan(self.theta()/2))

    def r(self):
        """
        distance to origin        
        """
        return math.sqrt(self.X**2+self.Y**2+self.Z**2)

    
class Plot:
    def __init__(self,name,RegExpression,Fun,selector = lambda x: True,graph = None ):
        import re
        self.name = name
        self.re =re.compile(RegExpression)
        if verbose:
            self.reFull = RegExpression
            print RegExpression
        self.functor = Fun
        if graph:
            self.Graph = graph
        else:
            self.graph = TGraph()
            self.graph.SetMarkerSize(5)
            self.graph.SetMarkerStyle(6)
            self.graph.SetTitle(RegExpression)
        self.current = 0
        self.selector = selector
        #self.initialiseAxes()
        
    def Fill(self,DetObj):
        if self.re.match(DetObj.name):
            if verbose:
                print DetObj.name , "match", self.reFull
            if self.selector(DetObj):
                [x,y] = self.functor(DetObj)
                if self.graph.GetN() < self.current:
                    self.graph.Set(self.graph.GetN()+1000)
                self.graph.SetPoint(self.current,x,y)
                self.current += 1
                self.initialiseAxes()
        else:
            if verbose:
                print DetObj.name , "no match" , self.reFull

    def initialiseAxes(self):
        #find out whicht variables are plottet


        #are all values different in do?
        #this is nessecary to assign the axisses unabiguesly...

        do = DetectorObject("dummy",1.,2.,3.,4.,5.,6.) 
        
        values = {do.X:"x",
                  do.Y:"y",
                  do.Z:"z",
                  do.TX:"Tx",
                  do.TY:"Ty",
                  do.TZ:"Tz",
                  do.phi():"#phi",
                  do.eta():"#eta",
                  do.theta():"#theta",
                  do.rho():"#rho"}

                
        self.graph.Draw("A")
        xaxis = self.graph.GetXaxis()
        yaxis = self.graph.GetYaxis()
        [x,y] = self.functor(do)

        for (val,axis) in [(x,xaxis),(y,yaxis)]:
            axis.SetTitle(values[val])
            if verbose:
                print values[val]
        self.initialised = True
    def __del__(self):
        if self.current==0:
            return
        #self.initialiseAxes()
        #print self.name
        self.graph.Set(self.current)
        #print self.name
        self.graph.Draw("AP")
        #print self.name
        gPad.Print(self.name) 



def dumpToFile( element, output ) :
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
            
            ex =  gbl.Gaudi.XYZPoint(1,0,0)
            #mex = gbl.Gaudi.XYZPoint(-1,0,0)
            #print ex.Phi() #prints 0 
            #print mex.Phi() #prints pi
            discr = (geometryinfo.toGlobalMatrixNominal().Rotation()*ex).Phi()/math.pi
            #print [fabs (discr-x)<0.001 for x in [0,1,-0.0277777777778, 0.972222222222,-0.972222401409,-1,0.0277775985909]],discr, element.name()
            #print fabs (discr) <0.3 ,0.7 < discr < 1.3,-0.7 > discr > -1.3,discr, element.name() 

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
            
            line = element.name() 
            line += " : " + aligncondition.name() + " : "
            line += str( globalcenter.x() ) + " " +  str( globalcenter.y() ) + " " +  str( globalcenter.z() ) + " : "
            line += str( totaltranslations[0] ) + " " + str( totaltranslations[1] ) + " "+ str( totaltranslations[2] ) + " " 
            line += str( totalrotations[0] ) + " " + str( totalrotations[1] ) + " "+ str( totalrotations[2] ) + " : " 
            line += str( localtranslations[0] ) + " " + str( localtranslations[1] ) + " "+ str( localtranslations[2] ) + " " 
            line += str( localrotations[0] ) + " " + str( localrotations[1] ) + " "+ str( localrotations[2] )
            line += '\n'
            output.write( line )
            if plots != None:
                DetObj = DetectorObject(aligncondition.name(),
                                        globalcenter.x(),globalcenter.y(),globalcenter.z(),
                                        totaltranslations[0],totaltranslations[1],totaltranslations[2])
                map(lambda x:x.Fill(DetObj),plots)
                
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
    parser.add_option("-p","--plot",type="string",default=None,dest="plot",help="generate a lot of plots.")
    parser.add_option("-v","--verbose",action="store_true",dest="verbose",default=False,
                      help="make a lot o noise!")

    (opts, args) = parser.parse_args()
    global plots
    plots =  None
    global verbose
    verbose = opts.verbose
    if opts.plot:
        plots = [Plot("TT-ZX"+opts.plot,"/TT.*",lambda do:[do.Z,do.X],lambda do:do.Z < 3000),
                 Plot("all-ZX"+opts.plot,".*",lambda do:[do.Z,do.X]),

                 Plot("TT-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 3000),
                 Plot("TT-YTX"+opts.plot,"/TT.*",lambda do:[do.Y,do.TX],lambda do:do.Z < 3000),
                 Plot("TT-ZTX"+opts.plot,"/TT.*",lambda do:[do.Z,do.TX],lambda do:do.Z < 3000),
                 
                 Plot("TT-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 3000),
                 Plot("TT-YTY"+opts.plot,"/TT.*",lambda do:[do.Y,do.TY],lambda do:do.Z < 3000),
                 Plot("TT-ZTY"+opts.plot,"/TT.*",lambda do:[do.Z,do.TY],lambda do:do.Z < 3000),
                 
                 Plot("TT-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 3000),
                 Plot("TT-YTZ"+opts.plot,"/TT.*",lambda do:[do.Y,do.TZ],lambda do:do.Z < 3000),
                 Plot("TT-ZTZ"+opts.plot,"/TT.*",lambda do:[do.Z,do.TZ],lambda do:do.Z < 3000),

                 Plot("TTaX-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2340 and do.Z>2300),
                 Plot("TTaU-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2400 and do.Z>2360),
                 Plot("TTbV-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2625 and do.Z>2550),
                 Plot("TTbX-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2670 and do.Z>2625),
                 Plot("TTaX-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2340 and do.Z>2300),
                 Plot("TTaU-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2400 and do.Z>2360),
                 Plot("TTbV-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2625 and do.Z>2550),
                 Plot("TTbX-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2670 and do.Z>2625),
                 Plot("TTaX-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2340 and do.Z>2300),
                 Plot("TTaU-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2400 and do.Z>2360),
                 Plot("TTbV-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2625 and do.Z>2550),
                 Plot("TTbX-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2670 and do.Z>2625),
                 Plot("IT-ZY"+opts.plot,".*",lambda do:[do.Z,do.Y],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),
                 Plot("IT-ZX"+opts.plot,".*",lambda do:[do.Z,do.X],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),

                 Plot("IT-XTX"+opts.plot,".*",lambda do:[do.X,do.TX],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),




                 Plot("IT-RhoPhi"+opts.plot,".*",lambda do:[do.rho(),do.phi()],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),
                 Plot("all-ZY"+opts.plot,".*",lambda do:[do.Z,do.Y]),
                 Plot("all-RhoPhi"+opts.plot,".*",lambda do:[do.rho(),do.phi()]) ]

        for s in ["TTbV","TTbX","TTaX","TTaU",
                  "ITT1CSide","ITT1ASide","ITT1BottomLayer","ITT1TopLayer",
                  "ITT2CSide","ITT2ASide","ITT2BottomLayer","ITT2TopLayer",
                  "ITT3CSide","ITT3ASide","ITT3BottomLayer","ITT3TopLayer",
                  "ITT"]:
            for (i,function) in enumerate([lambda do:[do.X,do.TX],
                                           lambda do:[do.X,do.TY],
                                           lambda do:[do.X,do.TZ],

                                           lambda do:[do.Y,do.TX],
                                           lambda do:[do.Y,do.TY],
                                           lambda do:[do.Y,do.TZ],
                                           
                                           lambda do:[do.Z,do.TX],
                                           lambda do:[do.Z,do.TY],
                                           lambda do:[do.Z,do.TZ]]):

                plots.append(Plot(s+"XYZ"[i//3]+"T"+"XYZ"[i%3]+opts.plot,"/"+s+"(?!.*Sensor.*)",function))
        
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
