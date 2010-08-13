## $Id: DVTestBu2LLK-DC06.py,v 1.2 2009/09/17 12:09:11 jpalac Exp $
## ============================================================================
## CVS tag $Name:  $
## ============================================================================
"""
@file $Name:  $

Test file for Bu->eeK selection with 2008 data

@author P. Koppenburg
@date 2009-01-07
"""
from Gaudi.Configuration import *
##
## Truth checking 
##
from Configurables import AlgorithmCorrelationsAlg
##
## Correlations
##
from Configurables import AlgorithmCorrelationsAlg, AlgorithmCorrelations
TestCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
TestCorrelations.addTool(AlgorithmCorrelations())
TestCorrelations.AlgorithmCorrelations.OnlyNonZero = False
TestCorrelations.Algorithms = [ "StdLooseElectrons",
                                "StdLooseMuons",
                                "Bu2LLK09_DiLepton",
                                "StdLooseKaons",
                                "Presel09Bu2LLK",
                                "Sel09Bu2LLK" ]
###
# Selection
###
importOptions( "$DAVINCITESTSROOT/tests/options/Do09selBu2LLK.py")
from Configurables import DaVinci
Sel09Bu2LLKFilterSequence = GaudiSequencer("Sel09Bu2LLKFilterSequence")
DaVinci().UserAlgorithms += [Sel09Bu2LLKFilterSequence]
DaVinci().MoniSequence = [ TestCorrelations ]

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


DaVinci().EvtMax = 100 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 
DaVinci().DataType = "DC06"
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

