## CVS tag $Name:  $, version $Revision: 1.15 $
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
                               "StdNoPIDsKaons",
                               "StdNoPIDsPions",
                               "Sel_Bd2KstarMuMu_DiMuon",
                               "Sel_Bd2KstarMuMu_Kstar",
                               "StrippingBd2KstarMuMu_BdToKstarMuMuLine" ]
###
# DaVinci
###
## Preselection

from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream

from StrippingArchive.Utils import lineBuilder
lineBuilder = lineBuilder('Stripping17', 'Bd2KstarMuMu')
stream = StrippingStream("BKstarMuMuTest", Lines = lineBuilder.lines())
sc = StrippingConf()
sc.appendStream( stream )

from Configurables import DaVinci
DaVinci().UserAlgorithms = [ trueSeq ]
DaVinci().MoniSequence = [ TestCorrelations ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
########################################################################
#-- GAUDI jobOptions generated on Tue Jan 18 17:40:43 2011
#-- Contains event types : 
#--   11114004 - 1 files - 10000 events - 2.78 GBytes
# MC_2010_428935_2010-Sim08Reco06-withTruth_11114004
#
from Gaudi.Configuration import * 

EventSelector().Input   = [
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2010/DST/00008055/0000/00008055_00000053_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
########################################################################
DaVinci().EvtMax = 500
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 10
DaVinci().DataType = "MC09"
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

