from math import pi
from ROOT import *
from GaudiConf.Configuration import *
from AnalysisPython import Functors
from Gaudi.Configuration import EventSelector
from GaudiPython.Bindings import gbl, AppMgr, Helper
from AnalysisPython.HistoUtils import book, fill , HistoFile
from GaudiKernel import SystemOfUnits
from Configurables import MCMatchObjP2MCRelator
import GaudiPython


#import PartProp.Service

from LoKiPhys.decorators import *


####
locationRoot = '/Event/MicroDST'


#BsJpsiPhi (13144002)
#microDSTFile = ['/castor/cern.ch/user/j/jpalac/MicroDST/DaVinci_v25r2/MC_2010_3.5TeV_CPV_20Kevt.SeqBs2JpsiPhiPrescaled.mdst']

microDSTFile = ['/castor/cern.ch/grid/lhcb/user/d/dvaneijk/7108/7108459/13144002.SeqBs2Jpsi2MuMuPhi2KK.mdst']

# get rid of some spam
EventSelector().PrintFreq=500

# set up some useful paths of locations on the MicroDST
selectionPath = locationRoot +  '/Phys/Bs2Jpsi2MuMuPhi2KK_Bs'
particlePath = selectionPath + '/Particles'
stdVertexAssocPath = selectionPath + '/Particle2VertexRelations'
pvLocation = locationRoot+"/Rec/Vertex/Primary"

lhcbApp = LHCbApp(DDDBtag = 'default',
                  CondDBtag = '',
                  DataType = '2010',
                  Simulation = True)

appMgr = AppMgr(outputlevel=4)
appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
appMgr.initialize()
appMgr.HistogramPersistency = "ROOT"

import atexit
atexit.register(appMgr.exit)

evtSvc = appMgr.evtSvc()
toolSvc = appMgr.toolsvc()
evtSel = appMgr.evtSel()

nextEvent = Functors.NextEvent(appMgr, EvtMax = 10000)
pp = Functors.PartPropSvc(appMgr)
ppSvc = pp

# get an instance of PropertimeFitter
properTimeFitter = toolSvc.create('PropertimeFitter',
                                  interface='ILifetimeFitter')



myMCP2VVAngleCalculator = toolSvc.create('MCBs2JpsiPhiAngleCalculator',
                                         interface='IP2VVMCPartAngleCalculator')
# get an instance of IP2VVPartAngleCalculator
myP2VVAngleCalculator = toolSvc.create('Bs2JpsiPhiAngleCalculator',
                                       interface='IP2VVPartAngleCalculator')

_theta_tr = Double(-100.)
_psi_tr   = Double(-100.)
_phi_tr   = Double(-100.)

# open the MicroDST
evtSel.open(microDSTFile)

nEvents=0
nRecEvents=0
nParticles = 0
nBs = 0
nPrimaryVertices = 0

###################
### RooRealVars ###
###################

propertime  = RooRealVar("propertime", "propertime (ps)", -0.15, 10)

ctheta = RooRealVar("ctheta","cos(#theta)", -1.0,  1.0)
cpsi   = RooRealVar("cpsi",  "cos(#psi)",   -1.0,  1.0)
phi    = RooRealVar("phi",   "#phi",        -pi ,  pi )

mass = RooRealVar("mass","mass (GeV)",5200,5500)

##################
### RooArgSets ###
##################
    
allSet = RooArgSet()

allSet.add(propertime)
allSet.add(ctheta)
allSet.add(cpsi)
allSet.add(phi)

allSet.add(mass)

RecDataSet = RooDataSet("RecDataSet","RecDataSet",allSet)

####################
### makeDatasets ###
####################

def makeRecSet(particle,
               propTimeFun,
               recdataset
               ):

    propTime  = propTimeFun(particle)
    if propTime != None :
        b_pid = particle.particleID().pid()
        for i in range(1,particle.nChildren()+1) :
            vector = particle.child(i)
            pid_vector = vector.particleID().pid()


            propertime.setVal(propTime)

            mySC  = myP2VVAngleCalculator.calculateTransversityAngles(particle, _theta_tr, _phi_tr, _psi_tr)
            ctheta.setVal(cos(_theta_tr))
            cpsi.setVal(cos(_psi_tr))
            phi.setVal(_phi_tr)

            mass.setVal(particle.measuredMass())
            
            result = recdataset.add(allSet, 1.0)
    else :
        print 'No proper time'
##################
### Event loop ###
##################
while ( nextEvent() ) :
    nEvents+=1
    PVs = evtSvc[pvLocation]
    if PVs :
        nPrimaryVertices += PVs.size()
    
    particles = evtSvc[particlePath]
    if particles :
        bestVertexFun = Functors.BestTo(evtSvc[stdVertexAssocPath])
        propTimeFun = Functors.PropTime(bestVertexFun, properTimeFitter)
        for p_s in particles:
            if p_s :
                nParticles += 1
                makeRecSet(p_s,
                           propTimeFun,
                           RecDataSet)

print "Found Reco info in ", nRecEvents, "/", nEvents, "events"
print "Found ", nPrimaryVertices, " PVs in ", nEvents, "events"
print "Found ", nParticles, " B candidates in ", nEvents, "events"

#####################
### Building PDFs ###
#####################

#Simple Mass Pdf
MassMean = RooRealVar("MassMean","MassMean", 5366, 5300, 5400)
MassSigma = RooRealVar("MassSigma","MassSigma", 10, 0, 50)
MassPdf = RooGaussian("MassPdf","Mass PDF", mass, MassMean, MassSigma)

#Simple Time Resolution Model
resMean = RooRealVar("resMean","resMean",0.,-0.01,0.01)
resSigma = RooRealVar("resSigma","resSigma",0.035,0.025,0.065)
GaussModel = RooGaussModel("GaussModel","GaussModel",propertime,resMean,resSigma)

#Simplified Time Pdf (JpsiPhi in real life is far from a simple exponential! To make a full JpsiPhi analysis, one could use the P2VV fit package)
tau = RooRealVar("tau","tau", 1.47, 1.4, 1.5)
TimePdf = RooDecay("TimePdf","TimePdf", propertime, tau, GaussModel, RooDecay.SingleSided)

###############
### Fitting ###
###############

MassPdf.fitTo(RecDataSet)
TimePdf.fitTo(RecDataSet)

################
### Plotting ###
################
MyCanvas = TCanvas("MyCanvas","MyCanvas",900,1000)
MyCanvas.Divide(3,2)

MyCanvas.cd(1)
propertimeframe1 = propertime.frame(-1.,8)
RecDataSet.plotOn(propertimeframe1,RooLinkedList())
TimePdf.plotOn(propertimeframe1)
TimePdf.paramOn(propertimeframe1)
propertimeframe1.Draw()

MyCanvas.cd(2)
cthetaframe = ctheta.frame()
RecDataSet.plotOn(cthetaframe,RooLinkedList())
cthetaframe.Draw()

MyCanvas.cd(3)
cpsiframe = cpsi.frame()
RecDataSet.plotOn(cpsiframe,RooLinkedList())
cpsiframe.Draw()

MyCanvas.cd(4)
phiframe = phi.frame()
RecDataSet.plotOn(phiframe,RooLinkedList())
phiframe.Draw()

MyCanvas.cd(5)
massframe = mass.frame()
RecDataSet.plotOn(massframe,RooLinkedList())
MassPdf.plotOn(massframe)
MassPdf.paramOn(massframe)
massframe.Draw()
