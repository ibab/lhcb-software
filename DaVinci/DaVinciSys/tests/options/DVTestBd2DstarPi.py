## $Id: DVTestBd2DstarPi.py,v 1.3 2009-03-24 10:43:21 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
## ============================================================================
"""
@file DVTestBd2DstarPi.py

Test file for HLT Bd->D*pi selection

@author J. Palacios
@date 2007-11-12
"""
from Gaudi.Configuration import *
##--------------------------------------------------------------
##
## Truth checking for B0 -> D*Pi
##
from Configurables import GaudiSequencer
trueSeq = GaudiSequencer("FindTrueDecay")
trueSeq.IgnoreFilterPassed = True
##----------------------------------------------------------------------------------------------------
##
## Truth filter for B0 -> D*pi  No PID
##
from Configurables import OldFilterDesktop, PhysDesktop, ByPIDFilterCriterion, TrueMCFilterCriterion, MCDecayFinder
SelectTrueNoPIDsDecay = OldFilterDesktop("SelectTrueNoPIDsDecay")
trueSeq.Members += [ SelectTrueNoPIDsDecay ]
SelectTrueNoPIDsDecay.FilterCriterion = "ByPIDFilterCriterion"
SelectTrueNoPIDsDecay.addTool(PhysDesktop)
SelectTrueNoPIDsDecay.PhysDesktop.InputLocations = [ "StdDC06NoPIDsPions",
                                                     "StdDC06NoPIDsKaons" ]
SelectTrueNoPIDsDecay.addTool(ByPIDFilterCriterion("Filter"))
SelectTrueNoPIDsDecay.Filter.Selections =  [ "K+ : TrueMCFilterCriterion/Decay",
                                             "pi+ : TrueMCFilterCriterion/Decay" ]
##
## The decay descriptor is the only part that requires some thinking...
##
SelectTrueNoPIDsDecay.Filter.addTool(TrueMCFilterCriterion("Decay"))
SelectTrueNoPIDsDecay.Filter.Decay.addTool(MCDecayFinder()) 
SelectTrueNoPIDsDecay.Filter.Decay.MCDecayFinder.Decay = "{[[B0]nos -> ^pi+ {,gamma}{,gamma} (D*(2010)- -> ^pi- {,gamma}{,gamma} (D~0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma})) ...]cc, [[B~0]os -> ^pi+ {,gamma}{,gamma} (D*(2010)- -> ^pi- {,gamma}{,gamma} (D~0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma})) ...]cc}" 
##
## Dummy MakeResonances to check that all tracks are there
##
from Configurables import CombineParticles
AllTrueNoPIDsDecay = CombineParticles("AllTrueNoPIDsDecay")
trueSeq.Members += [ AllTrueNoPIDsDecay ]
AllTrueNoPIDsDecay.MotherCut = "ALL"
AllTrueNoPIDsDecay.addTool(PhysDesktop)
AllTrueNoPIDsDecay.PhysDesktop.InputLocations = [ "SelectTrueNoPIDsDecay" ]
AllTrueNoPIDsDecay.DecayDescriptor = "[B0 -> pi- K+ pi- pi+]cc"  ## all particles need to be there
##----------------------------------------------------------------------------------------------------
##
## Truth filter for Bd -> D*Pi Loose PID : copy of above with NoPIDs -> Loose 
##
SelectTrueLooseDecay = SelectTrueNoPIDsDecay.clone("SelectTrueLooseDecay")
trueSeq.Members += [ SelectTrueLooseDecay ]
SelectTrueLooseDecay.addTool(PhysDesktop)
SelectTrueLooseDecay.PhysDesktop.InputLocations = [ "StdDC06LoosePions",
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
TestCorrelations.Algorithms = [ "AllTrueNoPIDsDecay", 
                                "AllTrueLooseDecay",
                                "StdDC06LooseD02KPi",
                                "StdDC06LooseDstarWithD02KPi",
                                "FilterDstarForBd2DstarPi", 
                                "PreselBd2DstarPi"          ]
###
# DaVinci
###
## Preselection
from Configurables import DaVinci
DaVinci().MainOptions = "$B2DSTARXROOT/options/DoPreselBd2DstarPi.opts"
DaVinci().UserAlgorithms = [ trueSeq ]

##-- GAUDI data cards generated on 11/12/07 6:36 PM
##-- For Event Type = 11463000 / Data type = DST 1
##--     Configuration = DC06 - phys-v2-lumi2
##--     DST 1 datasets produced by Brunel - v30r14
##--     From DIGI 1 datasets produced by Boole - v12r10
##--     From SIM 1 datasets produced by Gauss - v25r8
##--     Database version = v30r14
##--     Cards content = physical-physical
##--     
##-- Datasets replicated at CERN
##-- 200 dataset(s) - NbEvents = 99411
##-- 

DaVinci().Input   = [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001695/DST/0000/00001695_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001695/DST/0000/00001695_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001695/DST/0000/00001695_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001695/DST/0000/00001695_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001695/DST/0000/00001695_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001695/DST/0000/00001695_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
DaVinci().EvtMax = 500 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

