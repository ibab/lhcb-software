## $Id: DVTestBd2KstarMuMu.py,v 1.10 2009-08-06 09:04:24 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.10 $
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
##------------------------------------------------------------------------------##
## Truth filter No PID
##
from Configurables import  FilterTrueTracks, MCDecayFinder
trueSeq.Members += [ FilterTrueTracks() ]
FilterTrueTracks().addTool(MCDecayFinder)
FilterTrueTracks().MCDecayFinder.Decay = "[[B0]nos => ^K+ ^pi- ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc, [[B0]os => ^K- ^pi+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc"
##
## Correlations
##
from Configurables import AlgorithmCorrelationsAlg, AlgorithmCorrelations
TestCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
TestCorrelations.addTool(AlgorithmCorrelations())
TestCorrelations.AlgorithmCorrelations.OnlyNonZero = False
TestCorrelations.Algorithms = ["FilterTrueTracks", 
                               "StdLooseDiMuon", "StdLooseDetachedKst2Kpi", 
                               "Strip_loose_Bd2KstarMuMu",
                               "filter_Bd2KstarMuMu_10Hz" ]
###
# DaVinci
###
## Preselection
from StrippingConf.Configuration import StrippingConf
StrippingConf().ActiveLines = [ "Bd2KstarMuMu_10Hz" ]  
StrippingConf().OutputType = "NONE"
StrippingConf().MainOptions = "$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py"

from Configurables import DaVinci
DaVinci().UserAlgorithms = [ trueSeq ]
DaVinci().MoniSequence = [ TestCorrelations ]

##
## Options
##
##-- GAUDI data cards generated on 11/8/07 5:37 PM
##-- For Event Type = 11114001 / Data type = DST 1

DaVinci().Input = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004871/0000/00004871_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

DaVinci().EvtMax = 500
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 10
DaVinci().DataType = "MC09"
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

