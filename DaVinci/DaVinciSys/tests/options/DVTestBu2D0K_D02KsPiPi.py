## $Id: DVTestBu2D0K_D02KsPiPi.py,v 1.7 2009-04-29 13:27:53 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
## ============================================================================
"""
@file DVTestBu2D0K_D02KsPiPi.py

Test file for B+ -> D0 K, D0 -> Ks Pi Pi selection

@author J. Palacios
@date 2007-11-12
"""
from Gaudi.Configuration import *
##
## catch selection
##
from Configurables import GaudiSequencer, MakeResonances, PrintHeader
seqPresel = GaudiSequencer("SeqPreselBu2D0K_D02KsPiPi")
seqPresel.Members += [ MakeResonances("PreselBu2D0K_D02KsPiPi"), PrintHeader() ]
importOptions("$STDOPTS/PreloadUnits.opts")
importOptions("$B2D0XROOT/options/PreselBu2D0K_D02KsPiPi.opts")
##--------------------------------------------------------------
##
## Truth checking 
##
trueSeq = GaudiSequencer("FindTrueDecay")
trueSeq.IgnoreFilterPassed = True
##----------------------------------------------------------------------------------------------------
##
## Truth filter
##
from Configurables import OldFilterDesktop, ByPIDFilterCriterion, TrueMCFilterCriterion, MCDecayFinder
SelectTrueNoPIDsDecay = OldFilterDesktop("SelectTrueNoPIDsDecay")
trueSeq.Members += [ SelectTrueNoPIDsDecay ]
SelectTrueNoPIDsDecay.FilterCriterion = "ByPIDFilterCriterion"
SelectTrueNoPIDsDecay.InputLocations = [ "StdDC06NoPIDsPions",
                                                     "StdDC06NoPIDsKaons" ]
SelectTrueNoPIDsDecay.addTool(ByPIDFilterCriterion("Filter"))
SelectTrueNoPIDsDecay.Filter.Selections =  [ "K+ : TrueMCFilterCriterion/Decay",
                                             "pi+ : TrueMCFilterCriterion/Decay" ]
##
## The decay descriptor is the only part that requires some thinking...
##
SelectTrueNoPIDsDecay.Filter.addTool(TrueMCFilterCriterion("Decay"))
SelectTrueNoPIDsDecay.Filter.Decay.addTool(MCDecayFinder()) 
SelectTrueNoPIDsDecay.Filter.Decay.MCDecayFinder.Decay = "[B+ -> (D~0 -> (KS0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc" 
##
## Dummy MakeResonances to check that all tracks are there
##
from Configurables import CombineParticles
AllTrueNoPIDsDecay = CombineParticles("AllTrueNoPIDsDecay")
trueSeq.Members += [ AllTrueNoPIDsDecay ]
AllTrueNoPIDsDecay.MotherCut = "ALL"
AllTrueNoPIDsDecay.InputLocations = [ "SelectTrueNoPIDsDecay" ]
AllTrueNoPIDsDecay.DecayDescriptor = "[B+ -> K+ pi- pi+ pi- pi+]cc"  ## all particles need to be there
##----------------------------------------------------------------------------------------------------
##
## Truth filter for loose PID 
##
SelectTrueLooseDecay = SelectTrueNoPIDsDecay.clone("SelectTrueLooseDecay")
trueSeq.Members += [ SelectTrueLooseDecay ]
SelectTrueLooseDecay.InputLocations = [ "StdDC06LoosePions",
                                                    "StdDC06LooseKaons"]
##
## Dummy MakeResonances to check that all tracks are there
##
AllTrueLooseDecay = AllTrueNoPIDsDecay.clone("AllTrueLooseDecay")
AllTrueLooseDecay.InputLocations = [ "SelectTrueLooseDecay" ]
trueSeq.Members += [ AllTrueLooseDecay  ]
##
## Correlations
##
from Configurables import AlgorithmCorrelationsAlg, AlgorithmCorrelations
TestCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
trueSeq.Members += [ TestCorrelations  ]
TestCorrelations.addTool(AlgorithmCorrelations())
TestCorrelations.AlgorithmCorrelations.OnlyNonZero = False
TestCorrelations.Algorithms = [ "AllTrueNoPIDsDecay", 
                                "AllTrueLooseDecay",
                                "StdDC06LooseKsLL",
                                "StdDC06LooseKsDD",
                                "StdDC06LooseD02KsPiPi",
                                "PreselBu2D0K_D02KsPiPi"
]
###
# DaVinci
###
## Preselection
from Configurables import DaVinci
DaVinci().UserAlgorithms = [ seqPresel, trueSeq ]

DaVinci().Input = [
      "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bu2KD-KSPiPi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
DaVinci().EvtMax = 100
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 50

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

