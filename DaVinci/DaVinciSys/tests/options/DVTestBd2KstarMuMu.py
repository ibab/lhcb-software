## $Id: DVTestBd2KstarMuMu.py,v 1.5 2009-05-20 17:04:01 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
## ============================================================================
"""
@file DVTestBd2KstarMuMu.py

Test file for B0->mumuK* selection

@author J. Palacios
@date 2007-11-12
"""
from Gaudi.Configuration import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
##--------------------------------------------------------------
##
## Truth checking 
##
from Configurables import GaudiSequencer
trueSeq = GaudiSequencer("FindTrueDecay")
trueSeq.IgnoreFilterPassed = True
##----------------------------------------------------------------------------------------------------
##
## Truth filter No PID
##
from Configurables import OldFilterDesktop, ByPIDFilterCriterion, TrueMCFilterCriterion, MCDecayFinder
SelectTrueNoPIDsDecay = OldFilterDesktop("SelectTrueNoPIDsDecay")
trueSeq.Members += [ SelectTrueNoPIDsDecay ]
SelectTrueNoPIDsDecay.FilterCriterion = "ByPIDFilterCriterion"
SelectTrueNoPIDsDecay.InputLocations = [ "StdNoPIDsMuons",
                                         "StdNoPIDsKaons",
                                         "StdNoPIDsPions"]
SelectTrueNoPIDsDecay.addTool(ByPIDFilterCriterion("Filter"))
SelectTrueNoPIDsDecay.Filter.Selections =  [ "K+ : TrueMCFilterCriterion/Decay",
                                             "mu+ : TrueMCFilterCriterion/Decay",
                                             "pi+ : TrueMCFilterCriterion/Decay" ]
##
## The decay descriptor is the only part that requires some thinking...
##
SelectTrueNoPIDsDecay.Filter.addTool(TrueMCFilterCriterion("Decay"))
SelectTrueNoPIDsDecay.Filter.Decay.addTool(MCDecayFinder()) 
SelectTrueNoPIDsDecay.Filter.Decay.MCDecayFinder.Decay = "[[B0]nos => ^K+ ^pi- ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc, [[B0]os => ^K- ^pi+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc"
##
## Dummy MakeResonances to check that all tracks are there
##
from Configurables import CombineParticles
AllTrueNoPIDsDecay = CombineParticles("AllTrueNoPIDsDecay")
trueSeq.Members += [ AllTrueNoPIDsDecay ]
AllTrueNoPIDsDecay.MotherCut = "ALL"
AllTrueNoPIDsDecay.InputLocations = [ "SelectTrueNoPIDsDecay" ]
AllTrueNoPIDsDecay.DecayDescriptor = "[B0 -> mu- mu+ K+ pi-]cc"  ## all particles need to be there
##----------------------------------------------------------------------------------------------------
##
## Truth filter for Loose PID : copy of above with NoPIDs -> Loose 
##
SelectTrueLooseDecay = SelectTrueNoPIDsDecay.clone("SelectTrueLooseDecay")
trueSeq.Members += [ SelectTrueLooseDecay ]
SelectTrueLooseDecay.InputLocations = [ "StdLooseMuons",
                                        "StdLooseKaons",
                                        "StdLoosePions"]
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
TestCorrelations.Algorithms = ["AllTrueNoPIDsDecay", "AllTrueLooseDecay", 
                               "Strip_10Hz_Bd2KstarMuMu", "StdLoosePenguinDimuon", "StdLooseKstar2KPi" ]
###
# DaVinci
###
## Preselection
from StrippingConf.Configuration import StrippingConf
StrippingConf().ActiveLines = []
StrippingConf().OutputType = "DST"
importOptions ( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2KstarMuMu.py" )

from Configurables import DaVinci
DaVinci().UserAlgorithms = [ trueSeq ]

##
## Options
##
##-- GAUDI data cards generated on 11/8/07 5:37 PM
##-- For Event Type = 11114001 / Data type = DST 1

DaVinci().Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-lumi2/00001586/DST/0000/00001586_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]


DaVinci().EvtMax = 100 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

