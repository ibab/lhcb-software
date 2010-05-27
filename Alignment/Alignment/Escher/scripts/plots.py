#!/usr/bin/env python
__author__  = "Albert Bursche"
__version__ = ""

from dump_align_db import *
from ROOT import *

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
    parser.add_option("-p","--plot",type="string",default=".png",dest="plot",help="generate a lot of plots.")
    parser.add_option("-v","--verbose",action="store_true",dest="verbose",default=False,
                      help="make a lot o noise!")
    
    (opts, args) = parser.parse_args()
    global verbose
    verbose = opts.verbose
    outputfilename = args[0]
    global plots
    
    OutFile=TFile.Open("out.root","recreate")
    
    plots = [ArrowPlot("TT-ZX"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.Z,do.Z+10*do.TZ,do.X,do.X+10*do.TX]),
             ArrowPlot("TT-XY"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.X,do.X+10*do.TX,do.Y,do.Y+10*do.TY]),
             ArrowPlot("TT-ZY"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.Z,do.Z+10*do.TZ,do.Y,do.Y+10*do.TY]),
             ArrowPlot("ST-ZX"+opts.plot,".*TT(?!.*Sensor.*)",lambda do:[do.Z,do.Z+100*do.TZ,do.X,do.X+100*do.TX]),
             ArrowPlot("all-ZXarr"+opts.plot,".*(?!.*Sensor.*)",lambda do:[do.Z,do.Z+100*do.TZ,do.X,do.X+100*do.TX]),
             ArrowPlot("VeLo-ZXarr"+opts.plot,".*",lambda do:[do.Z,do.Z+10*do.TZ,do.X,do.X+10*do.TX],lambda do:do.Z<500),
             
             Plot("TT-ZtX"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.Z,do.TX]),
             Plot("TT-XtX"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.X,do.TX]),

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

    CallDumpToFile(outputfilename,opts,callback)
    map(lambda p:p.save(),plots)
    OutFile.Write()
    OutFile.Close()

class Graph:
    def __init__(self,name):
        self.n = 1000
        self.graph = TGraph()
        self.current = 0
        self.graph.SetMarkerSize(5)
        self.graph.SetMarkerStyle(6)
        self.graph.SetTitle(name)
 
    def Fill(self,x,y):
        if self.graph.GetN() < self.current:
            self.graph.Set(self.graph.GetN()+1000)
        self.graph.SetPoint(self.current,x,y)
        self.current += 1


 


class DetectorObject:
    def __init__(self,name,x,y,z,dx,dy,dz):

        self.name = name

        self.X  =  x
        self.Y  =  y
        self.Z  =  z
        
        self.TX = dx 
        self.TY = dy
        self.TZ = dz
    def __init__(self,name,globalcenter,totaltranslations,localdelta):
        self.name = name
        #print globalcenter,totaltranslations,localdelta
        self.X = globalcenter.x()
        self.Y = globalcenter.y()
        self.Z = globalcenter.z()
        
        self.TX = totaltranslations[0]
        self.TY = totaltranslations[1]
        self.TZ = totaltranslations[2]

        self.DX = localdelta.Translation().Vect().X()
        self.DY = localdelta.Translation().Vect().Y()
        self.DZ = localdelta.Translation().Vect().Z()
        

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
        self.initialised =False
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
                if not self.initialised:
                    self.initialiseAxes()
        else:
            if verbose:
                print DetObj.name , "no match" , self.reFull

    def initialiseAxes(self):
        #find out whicht variables are plotted by the functor
        
        globalcenter =  gbl.Gaudi.XYZPoint(1,2,3)
        totaltranslations = [4.0,5.0,6.0]

        localdelta = ROOT.Math.Transform3D(1,2,3,4,5,6,7,8,9,10,11,12)
        do = DetectorObject("dummy",globalcenter,totaltranslations,localdelta)
        values = {do.X:"x",
                  do.Y:"y",
                  do.Z:"z",
                  do.TX:"Tx",
                  do.TY:"Ty",
                  do.TZ:"Tz",
                  do.DX:"#Delta_{x}",
                  do.DY:"#Delta_{y}",
                  do.DZ:"#Delta_{z}",
                  do.phi():"#phi",
                  do.eta():"#eta",
                  do.theta():"#theta",
                  do.rho():"#rho"}
        #print values
        #are all values different in do?
        #this is nessecary to assign the axisses unabiguesly...
         
        self.graph.Draw("A")
        xaxis = self.graph.GetXaxis()
        yaxis = self.graph.GetYaxis()
        [x,y] = self.functor(do)

        for (val,axis) in [(x,xaxis),(y,yaxis)]:
            axis.SetTitle(values[val])
            if verbose:
                print values[val]
        self.initialised = True

    def save(self):
        if self.current==0:
            return
        self.initialiseAxes()
        #print self.name
        self.graph.Set(self.current)
        #print self.name
        self.graph.Draw("AP")
        #print self.name
        gPad.Print(self.name) 

class ArrowPlot:
    def __init__(self,name,regex,fun,selector=lambda x:True):
        import re
        self.name = name
        self.re = re.compile(regex)
        self.functor = fun
        self.selector = selector
        self.arrows = []
        self.gNom = Graph(regex)
        self.gShift = Graph("")
        
    def Fill(self,do):
        if self.re.match(do.name):
            if self.selector(do):
                vals = self.functor(do)
                self.arrows.append(TArrow(vals[0],vals[2],vals[1],vals[3],0.005,">"))
                self.gNom.Fill(vals[0],vals[2])
                self.gShift.Fill(vals[1],vals[3])
                #print vals
                
    def save(self):
        self.initialiseAxes()
        self.gShift.graph.Draw("AP")
        self.gNom.graph.Draw("P")
        self.gShift.graph.SetMarkerColor(2)
        
        #self.gNom.graph.Dump()
        #pad = TCanvas("","",-5000,-5000,5000,5000)
        for a in self.arrows:
            a.Draw()
            #a.Dump()
            pass
        self.gShift.graph.Draw("P")
        gPad.Print(self.name)

    def initialiseAxes(self): #copied from "Plot"; ToDo move to class graph to avoid code dublication
        #find out whicht variables are plotted by the functor
        
        globalcenter =     gbl.Gaudi.XYZPoint(1,2,3)
        totaltranslations = [4.0,5.0,6.0]

        localdelta = ROOT.Math.Transform3D(1,2,3,4,5,6,7,8,9,10,11,12)
        do = DetectorObject("dummy",globalcenter,totaltranslations,localdelta)
        values = {do.X:"x",
                  do.Y:"y",
                  do.Z:"z",
                  do.TX:"Tx",
                  do.TY:"Ty",
                  do.TZ:"Tz",
                  do.DX:"#Delta_{x}",
                  do.DY:"#Delta_{y}",
                  do.DZ:"#Delta_{z}",
                  do.phi():"#phi",
                  do.eta():"#eta",
                  do.theta():"#theta",
                  do.rho():"#rho"}
        #print values
        #are all values different in do?
        #this is nessecary to assign the axisses unabiguesly...
         
        self.gShift.graph.Draw("A")
        xaxis = self.gShift.graph.GetXaxis()
        yaxis = self.gShift.graph.GetYaxis()
        [x,dx,y,dy] = self.functor(do)

        for (val,axis) in [(x,xaxis),(y,yaxis)]:
            axis.SetTitle(values[val])
            if verbose:
                print values[val]
        self.initialised = True  
def callback(name,globalcenter,totaltranslations,localdelta):
    #DetObj = DetectorObject(name,
    #                        globalcenter.x(),globalcenter.y(),globalcenter.z(),
    #                        totaltranslations[0],totaltranslations[1],totaltranslations[2])
    
    DetObj = DetectorObject(name,globalcenter,totaltranslations,localdelta)
    map(lambda x:x.Fill(DetObj),plots)





main()
