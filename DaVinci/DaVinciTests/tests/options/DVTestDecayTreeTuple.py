## @file DVTestHltDecayTreeTuple.py
 #
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#######################################################################
#
# Selection
#

from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream
from StrippingSelections import StrippingBd2JpsiKS

stream = StrippingStream("Test")
stream.appendLines( [ StrippingBd2JpsiKS.line1 ] )
sc = StrippingConf()
sc.appendStream( stream )
########################################################################
#
# The Decay Tuple
#
from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("Tuple")
tuple.ToolList +=  [
      "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
#    , "TupleToolTISTOS"
     ]
tuple.InputLocations = [StrippingBd2JpsiKS.line1.outputLocation()]
tuple.Decay = "[B0 -> (^J/psi(1S) -> ^mu+ ^mu-) (^KS0 -> ^pi+ ^pi-)]cc"
#tuple.OutputLevel = 1 ;
########################################################################
#
# The Event Tuple
#
from Configurables import EventTuple, TupleToolTrigger
evtTuple = EventTuple()
evtTuple.ToolList = [ "TupleToolEventInfo",
                      "TupleToolGeneration" ]
evtTuple.addTool(TupleToolTrigger())
evtTuple.TupleToolTrigger.VerboseHlt1 = True 
evtTuple.TupleToolTrigger.VerboseHlt2 = True
# evtTuple.OutputLevel = 1 
########################################################################
#
# The MC truth Tuple
#
from Configurables import MCDecayTreeTuple
mcTuple = MCDecayTreeTuple("MCTuple")
mcTuple.Decay = "{[ [B0]nos -> (^J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (^KS0 -> ^pi+ ^pi- {,gamma}{,gamma}) {,gamma}]cc, [ [B0]os -> (^J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (^KS0 -> ^pi+ ^pi- {,gamma}{,gamma}) {,gamma}]cc}"
mcTuple.ToolList = [ "MCTupleToolKinematic", "TupleToolEventInfo", "MCTupleToolReconstructed"  ]
from Configurables import MCTupleToolReconstructed, MCReconstructed
#ToolSvc().addTool(MCReconstructed)
#ToolSvc().MCReconstructed.OutputLevel = 1

#mcTuple.OutputLevel = 1

from Configurables import PrintMCTree
pmc = PrintMCTree()
pmc.ParticleNames = [ "B0", "B~0" ]
########################################################################
#
# DaVinci
#
from Configurables import DaVinci

DaVinci().EvtMax = 1000
DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09" 
DaVinci().Simulation   = True
#DaVinci().CondDBtag = 'MC09-20100827-vc-md100'
DaVinci().TupleFile = "DecayTreeTuple.root"  # Ntuple
DaVinci().MoniSequence = [ tuple, evtTuple, mcTuple ] # , pmc ] 
DaVinci().appendToMainSequence( [ sc.sequence() ] )
#-- GAUDI jobOptions generated on Tue Jun 23 11:54:57 2009
#-- Contains event types : 
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004889/0000/00004889_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
