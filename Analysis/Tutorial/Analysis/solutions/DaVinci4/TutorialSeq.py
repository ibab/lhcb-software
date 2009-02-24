#######################################################################
#
# 1) Let's define a sequence
#
from Configurables import GaudiSequencer
tutorialseq = GaudiSequencer("TutorialSeq")
#######################################################################
#
# 2) Make a J/psi
#
from Configurables import CombineParticles, PhysDesktop
jpsi2mumu = CombineParticles("Jpsi2MuMu")
jpsi2mumu.addTool( PhysDesktop )
jpsi2mumu.PhysDesktop.InputLocations = [ "StdLooseMuons" ] 
jpsi2mumu.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 
jpsi2mumu.DaughtersCuts = { "mu+" : "ALL" } 
jpsi2mumu.CombinationCut = "ADAMASS('J/psi(1S)')<30*MeV" 
jpsi2mumu.MotherCut = "(VFASPF(VCHI2/VDOF)<100)" 
tutorialseq.Members += [ jpsi2mumu ]
#######################################################################
#
# 2) Filter J/psi further
#
from Configurables import FilterDesktop
psifilter = FilterDesktop("PsiFilter")
psifilter.addTool( PhysDesktop )
psifilter.PhysDesktop.InputLocations = [ "Jpsi2MuMu" ]
psifilter.Code = "(PT>1*GeV) & (P>3*GeV)"
tutorialseq.Members += [ psifilter ]
#######################################################################
#
# 3) make the Phi
#
phi2kk = CombineParticles("Phi2KK")
phi2kk.addTool( PhysDesktop )
phi2kk.PhysDesktop.InputLocations = [ "StdLooseKaons" ]
phi2kk.DecayDescriptor  =  "phi(1020) -> K+ K-" 
phi2kk.CombinationCut = "ADAMASS('phi(1020)')<50" 
phi2kk.MotherCut = "(VFASPF(VCHI2/VDOF)<100)"
tutorialseq.Members += [ phi2kk ]
#######################################################################
#
# 3) make the Bs
#
bs2jpsiphi = CombineParticles("Bs2JpsiPhi")
bs2jpsiphi.DecayDescriptor = "B_s0 -> phi(1020) J/psi(1S)"
bs2jpsiphi.CombinationCut = "ADAMASS('B_s0')<2*GeV"
bs2jpsiphi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<100)"
bs2jpsiphi.addTool( PhysDesktop )
bs2jpsiphi.PhysDesktop.InputLocations = [ "PsiFilter", "Phi2KK" ]
from Configurables import LoKi__Hybrid__PlotTool as PlotTool
import GaudiKernel.SystemOfUnits as Units 
bs2jpsiphi.HistoProduce = True
bs2jpsiphi.addTool( PlotTool("DaughtersPlots") )
bs2jpsiphi.DaughtersPlots.Histos = { "P/1000"  : ('momentum',0,100) , 
                                     "PT/1000" : ('pt_%1%',0,10,200) , 
                                     "M"       : ('mass in MeV_%1%_%2%_%3%',0.8*Units.GeV,4*Units.GeV) }
bs2jpsiphi.addTool( PlotTool("MotherPlots") )
bs2jpsiphi.MotherPlots.Histos = { "P/1000"  : ('momentum',0,100) , 
                                 "PT/1000" : ('pt_%1%',0,10,200) , 
                                 "M"       : ('mass_%1%_%2%_%3%',4*Units.GeV,6*Units.GeV) }
tutorialseq.Members += [ bs2jpsiphi ]
