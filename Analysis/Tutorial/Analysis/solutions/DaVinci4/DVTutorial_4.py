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
bs2jpsiphi = CombineParticles("Bs2JpsiPhi")

bs2jpsiphi.addTool( PhysDesktop() )
bs2jpsiphi.PhysDesktop.InputLocations = [ "Phys/Jpsi2MuMu", "Phys/Phi2KK" ]
bs2jpsiphi.DecayDescriptor = "B_s0 -> phi(1020) J/psi(1S)" # mandatory
bs2jpsiphi.CombinationCut = "ADAMASS('B_s0')<2*GeV"
bs2jpsiphi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<100)"
bs2jpsiphi.HistoProduce = True
bs2jpsiphi.addTool( PlotTool("DaughtersPlots") )
bs2jpsiphi.DaughtersPlots.Histos = { "P/1000"  : ('momentum',0,100) , 
                                     "PT/1000" : ('pt_%1%',0,5,500) , 
                                     "M"       : ('mass in MeV_%1%_%2%_%3%',0.8*Units.GeV,4*Units.GeV) }
bs2jpsiphi.addTool( PlotTool("MotherPlots") )
bs2jpsiphi.MotherPlots.Histos = { "P/1000"  : ('momentum',0,100) , 
                                 "PT/1000" : ('pt_%1%',0,5,500) , 
                                 "M"       : ('mass in MeV_%1%_%2%_%3%',4*Units.GeV,6*Units.GeV) }

GaudiSequencer("TutorialSeq").Members.append(bs2jpsiphi)
GaudiSequencer("TutorialSeq").IgnoreFilterPassed = False 


HistogramPersistencySvc().OutputFile = "DVHistos_4.root";
ApplicationMgr().EvtMax = 1000 ;
