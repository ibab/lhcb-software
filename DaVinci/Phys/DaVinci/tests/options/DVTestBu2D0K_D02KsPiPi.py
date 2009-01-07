## $Id: DVTestBu2D0K_D02KsPiPi.py,v 1.1 2009-01-07 16:35:51 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
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
importOptions("$DAVINCIROOT/options/PreloadUnits.opts")
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
from Configurables import OldFilterDesktop, PhysDesktop, ByPIDFilterCriterion, TrueMCFilterCriterion, MCDecayFinder
SelectTrueNoPIDsDecay = OldFilterDesktop("SelectTrueNoPIDsDecay")
trueSeq.Members += [ SelectTrueNoPIDsDecay ]
SelectTrueNoPIDsDecay.FilterCriterion = "ByPIDFilterCriterion"
SelectTrueNoPIDsDecay.addTool(PhysDesktop())
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
SelectTrueNoPIDsDecay.Filter.Decay.MCDecayFinder.Decay = "[B+ -> (D~0 -> (KS0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc" 
##
## Dummy MakeResonances to check that all tracks are there
##
from Configurables import CombineParticles
AllTrueNoPIDsDecay = CombineParticles("AllTrueNoPIDsDecay")
trueSeq.Members += [ AllTrueNoPIDsDecay ]
AllTrueNoPIDsDecay.MotherCut = "ALL"
AllTrueNoPIDsDecay.addTool(PhysDesktop())
AllTrueNoPIDsDecay.PhysDesktop.InputLocations = [ "SelectTrueNoPIDsDecay" ]
AllTrueNoPIDsDecay.DecayDescriptor = "[B+ -> K+ pi- pi+ pi- pi+]cc"  ## all particles need to be there
##----------------------------------------------------------------------------------------------------
##
## Truth filter for loose PID 
##
SelectTrueLooseDecay = SelectTrueNoPIDsDecay.clone("SelectTrueLooseDecay")
trueSeq.Members += [ SelectTrueLooseDecay ]
SelectTrueLooseDecay.addTool(PhysDesktop())
SelectTrueLooseDecay.PhysDesktop.InputLocations = [ "StdDC06LoosePions",
                                                    "StdDC06LooseKaons"]
##
## Dummy MakeResonances to check that all tracks are there
##
AllTrueLooseDecay = AllTrueNoPIDsDecay.clone("AllTrueLooseDecay")
AllTrueLooseDecay.addTool(PhysDesktop())
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

##-- GAUDI data cards generated on 11/19/07 4:03 PM
##-- For Event Type = 12165102 / Data type = DST 1
##--     Configuration = DC06 - phys-v2-lumi2
##--     DST 1 datasets produced by Brunel - v30r14
##--     From DIGI 1 datasets produced by Boole - v12r10
##--     From SIM 1 datasets produced by Gauss - v25r8
##--     Database version = v30r14
##--     Cards content = physical-physical
##--

DaVinci().Input   = [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000008_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001660/DST/0000/00001660_00000010_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]


DaVinci().EvtMax = 200
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 50


