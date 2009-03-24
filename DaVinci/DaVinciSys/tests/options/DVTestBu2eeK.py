## $Id: DVTestBu2eeK.py,v 1.3 2009-03-24 10:43:21 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
## ============================================================================
"""
@file DVTestBu2eeK.py

Test file for Bu->eeK selection

@author J. Palacios
@date 2007-11-12
"""
from Gaudi.Configuration import *
##--------------------------------------------------------------
##
## Truth checking 
##
from Configurables import GaudiSequencer
trueSeq = GaudiSequencer("FindTrueDecay")
trueSeq.IgnoreFilterPassed = True
##----------------------------------------------------------------------------------------------------
##
## Truth filter
##
from Configurables import OldFilterDesktop, PhysDesktop, ByPIDFilterCriterion, TrueMCFilterCriterion, MCDecayFinder
SelectTrueNoPIDsDecay = OldFilterDesktop("SelectTrueNoPIDsDecay")
trueSeq.Members += [ SelectTrueNoPIDsDecay ]
SelectTrueNoPIDsDecay.FilterCriterion = "ByPIDFilterCriterion"
SelectTrueNoPIDsDecay.addTool(PhysDesktop)
SelectTrueNoPIDsDecay.PhysDesktop.InputLocations = [ "StdDC06NoPIDsElectrons",
                                                     "StdDC06NoPIDsKaons" ]
SelectTrueNoPIDsDecay.addTool(ByPIDFilterCriterion("Filter"))
SelectTrueNoPIDsDecay.Filter.Selections =  [ "K+ : TrueMCFilterCriterion/Decay",
                                             "e+ : TrueMCFilterCriterion/Decay" ]
##
## The decay descriptor is the only part that requires some thinking...
##
SelectTrueNoPIDsDecay.Filter.addTool(TrueMCFilterCriterion("Decay"))
SelectTrueNoPIDsDecay.Filter.Decay.addTool(MCDecayFinder()) 
SelectTrueNoPIDsDecay.Filter.Decay.MCDecayFinder.Decay = "[B+ -> ^K+ ^e+ ^e- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc" 
##
## Dummy MakeResonances to check that all tracks are there
##
from Configurables import CombineParticles
AllTrueNoPIDsDecay = CombineParticles("AllTrueNoPIDsDecay")
trueSeq.Members += [ AllTrueNoPIDsDecay ]
AllTrueNoPIDsDecay.MotherCut = "ALL"
AllTrueNoPIDsDecay.addTool(PhysDesktop)
AllTrueNoPIDsDecay.PhysDesktop.InputLocations = [ "SelectTrueNoPIDsDecay" ]
AllTrueNoPIDsDecay.DecayDescriptor = "[B+ -> e- e+ K+]cc"  ## all particles need to be there
##----------------------------------------------------------------------------------------------------
##
## Truth filter for loose PID 
##
SelectTrueLooseDecay = SelectTrueNoPIDsDecay.clone("SelectTrueLooseDecay")
trueSeq.Members += [ SelectTrueLooseDecay ]
SelectTrueLooseDecay.addTool(PhysDesktop)
SelectTrueLooseDecay.PhysDesktop.InputLocations = [ "StdDC06LooseElectrons",
                                                    "StdDC06LooseKaons"]
##
## Dummy MakeResonances to check that all tracks are there
##
AllTrueLooseDecay = AllTrueNoPIDsDecay.clone("AllTrueLooseDecay")
AllTrueLooseDecay.addTool(PhysDesktop)
AllTrueLooseDecay.PhysDesktop.InputLocations = [ "SelectTrueLooseDecay" ]
trueSeq.Members += [ AllTrueLooseDecay  ]
##
## Correlations
##
from Configurables import AlgorithmCorrelationsAlg, AlgorithmCorrelations
TestCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
trueSeq.Members += [ TestCorrelations  ]
TestCorrelations.addTool(AlgorithmCorrelations())
TestCorrelations.AlgorithmCorrelations.OnlyNonZero = False
TestCorrelations.Algorithms = [ "SelectTrueNoPIDsDecay",
                                "AllTrueNoPIDsDecay",
                                "SelectTrueLooseDecay",
                                "AllTrueLooseDecay",
                                "DiLeptonForPreselBu2LLK",
                                "PreselBu2LLK" ]
###
# DaVinci
###
## Preselection
from Configurables import DaVinci
DaVinci().MainOptions = "$B2DILEPTONROOT/options/DoPreselBu2LLK.opts"
DaVinci().UserAlgorithms = [ trueSeq ]

##-- GAUDI data cards generated on 11/19/07 2:55 PM
##-- For Event Type = 12123001 / Data type = DST 1
##--     Configuration = DC06 - phys-v2-lumi2
##--     DST 1 datasets produced by Brunel - v30r14
##--     From DIGI 1 datasets produced by Boole - v12r10
##--     From SIM 1 datasets produced by Gauss - v25r8
##--     Database version = v30r14
##--     Cards content = physical-physical
##--
##-- Datasets replicated at CERN
##-- 166 dataset(s) - NbEvents = 82494
##--

DaVinci().Input   = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001659/DST/0000/00001659_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001659/DST/0000/00001659_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001659/DST/0000/00001659_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001659/DST/0000/00001659_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001659/DST/0000/00001659_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]


DaVinci().EvtMax = 500 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

