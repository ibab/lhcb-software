########################################################################
#
# Options for exercise 4
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units 
from Gaudi.Configuration import *
from Configurables import CombineParticles, TutorialAlgorithm, PhysDesktop
from Configurables import LoKi__Hybrid__PlotTool as PlotTool 

importOptions( "$ANALYSISROOT/solutions/DaVinci3/DVTutorial_3.py" )

#
# Append Bs maker
#
Bs2JpsiPhi = CombineParticles("Bs2JpsiPhi")

Bs2JpsiPhi.addTool( PhysDesktop() )
Bs2JpsiPhi.PhysDesktop.InputLocations = [ "Jpsi2MuMu", "Phi2KK" ]
Bs2JpsiPhi.DecayDescriptor = "B_s0 -> phi(1020) J/psi(1S)" 
Bs2JpsiPhi.CombinationCut = "ADAMASS('B_s0')<2*GeV"
Bs2JpsiPhi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<100)"
Bs2JpsiPhi.HistoProduce = True
Bs2JpsiPhi.addTool( PlotTool("DaughtersPlots") )
Bs2JpsiPhi.DaughtersPlots.Histos = { "P/1000"  : ('momentum',0,100) , 
                                     "PT/1000" : ('pt_%1%',0,5,500) , 
                                     "M"       : ('mass in MeV_%1%_%2%_%3%',0.8*Units.GeV,4*Units.GeV) }
Bs2JpsiPhi.addTool( PlotTool("MotherPlots") )
Bs2JpsiPhi.MotherPlots.Histos = { "P/1000"  : ('momentum',0,100) , 
                                 "PT/1000" : ('pt_%1%',0,5,500) , 
                                 "M"       : ('mass in MeV_%1%_%2%_%3%',4*Units.GeV,6*Units.GeV) }

GaudiSequencer("TutorialSeq").Members.append(Bs2JpsiPhi)
GaudiSequencer("TutorialSeq").IgnoreFilterPassed = False 


HistogramPersistencySvc().OutputFile = "DVHistos_4.root";
ApplicationMgr().EvtMax = 1000 ;
