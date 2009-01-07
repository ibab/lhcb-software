## $Id: DVTestBs2PhiGamma.py,v 1.1 2009-01-07 13:45:48 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
## ============================================================================
"""
@file DVTestBs2PhiGamma.py

Test file for Bs->PhiGamma selection

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
SelectTrueLooseDecay = OldFilterDesktop("SelectTrueLooseDecay")
trueSeq.Members += [ SelectTrueLooseDecay ]
SelectTrueLooseDecay.FilterCriterion = "ByPIDFilterCriterion"
SelectTrueLooseDecay.addTool(PhysDesktop())
SelectTrueLooseDecay.PhysDesktop.InputLocations = [ "StdDC06LooseAllPhotons",
                                                    "StdDC06LooseKaons" ]
SelectTrueLooseDecay.addTool(ByPIDFilterCriterion("Filter"))
SelectTrueLooseDecay.Filter.Selections =  [ "K+ : TrueMCFilterCriterion/DecayK",
                                             "gamma : TrueMCFilterCriterion/DecayGamma" ]
##
## The decay descriptor is the only part that requires some thinking...
##
SelectTrueLooseDecay.Filter.addTool(TrueMCFilterCriterion("DecayGamma"))
SelectTrueLooseDecay.Filter.DecayGamma.addTool(MCDecayFinder())
SelectTrueLooseDecay.Filter.DecayGamma.AssociationMethod = 3
SelectTrueLooseDecay.Filter.DecayGamma.MCDecayFinder.Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}{,gamma}) gamma]cc" 
SelectTrueLooseDecay.Filter.addTool(TrueMCFilterCriterion("DecayK"))
SelectTrueLooseDecay.Filter.DecayK.addTool(MCDecayFinder())
SelectTrueLooseDecay.Filter.DecayK.MCDecayFinder.Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) gamma]cc" 
##
## Dummy MakeResonances to check that all tracks are there
##
from Configurables import CombineParticles
AllTrueLooseDecay = CombineParticles("AllTrueLooseDecay")
trueSeq.Members += [ AllTrueLooseDecay ]
AllTrueLooseDecay.MotherCut = "ALL"
AllTrueLooseDecay.addTool(PhysDesktop())
AllTrueLooseDecay.PhysDesktop.InputLocations = [ "SelectTrueLooseDecay" ]
AllTrueLooseDecay.DecayDescriptor = "[B_s0 -> K+ K- gamma]cc"  ## all particles need to be there
##
## Correlations
##
from Configurables import AlgorithmCorrelationsAlg, AlgorithmCorrelations
TestCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
trueSeq.Members += [ TestCorrelations  ]
TestCorrelations.addTool(AlgorithmCorrelations())
TestCorrelations.AlgorithmCorrelations.OnlyNonZero = False
TestCorrelations.Algorithms = [ "AllTrueLooseDecay",                           
                                "PreselXb2GammaX", 
                                "StdDC06LoosePhi2KK" ]
###
# DaVinci
###
## Preselection
from Configurables import DaVinci
DaVinci().MainOptions = "$RADIATIVEROOT/options/DoPreselXb2GammaX.opts"
DaVinci().UserAlgorithms = [ trueSeq ]

##-- GAUDI data cards generated on 11/21/07 11:28 AM
##-- For Event Type = 13102201 / Data type = DST 1
##--     Configuration = DC06 - phys-lumi2
##--     DST 1 datasets produced by Brunel - v30r14
##--     From DIGI 1 datasets produced by Boole - v12r10
##--     From SIM 1 datasets produced by Gauss - v25r7
##--     Database version = v30r14
##--     Cards content = physical-physical
##--     
##-- Datasets replicated at CERN
##-- 160 dataset(s) - NbEvents = 79521
##-- 

DaVinci().Input   = [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001538/DST/0000/00001538_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]

DaVinci().EvtMax = 200 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 


