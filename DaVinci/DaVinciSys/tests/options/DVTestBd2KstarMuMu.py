## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.15 $
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
                               "StdVeryLooseMuons",
                               "StdLooseMuons",
                               "StdLooseKaons",
                               "StdLoosePions",
                               "StdVeryLooseDiMuon",
                               "StdVeryLooseDetachedKst2Kpi", 
                               "Early_Signal_Bd2KstarMuMu" ]
###
# DaVinci
###
## Preselection

from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream
from StrippingSelections.StrippingBd2KstarMuMu import StrippingBd2KstarMuMuConf

stream = StrippingStream("Bmuon")
stream.appendLines( [ StrippingBd2KstarMuMuConf().Early_SignalLine() ] )

sc = StrippingConf()
sc.appendStream( stream )

from Configurables import DaVinci
DaVinci().UserAlgorithms = [ trueSeq ]
DaVinci().MoniSequence = [ TestCorrelations ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )

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

