 ## @file DVTestHltDecayTreeTuple-DC06.py
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
########################################################################
#
# The Decay Tuple
#
from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("Tuple")
tuple.ToolList +=  [
      "TupleToolTrigger"
    , "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
#    , "TupleToolTISTOS"
     ]
tuple.InputLocations = ["Strip_10Hz_Bd2KstarMuMu"]
tuple.Decay = "[B0 -> (^J/psi(1S) -> ^mu+ ^mu-) (^K*(892)0 -> ^K+ ^pi-)]cc"
#tuple.OutputLevel = 1 ;
########################################################################
#
# The Event Tuple
#
from Configurables import EventTuple, TupleToolTrigger
evtTuple = EventTuple()
evtTuple.ToolList = [ "TupleToolTrigger",
                      "TupleToolEventInfo",
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
mcTuple.Decay = "{[ [B0]nos -> ^mu+ ^mu- (^K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) {,gamma}{,gamma}{,gamma}]cc, [ [B~0]os -> ^mu+ ^mu- (^K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) {,gamma}{,gamma}{,gamma}]cc}"
mcTuple.ToolList = [ "MCTupleToolMCTruth", "TupleToolEventInfo", "MCTupleToolReconstructed"  ]
#mcTuple.OutputLevel = 1
#######################################################################
#
# Selection
#
from StrippingConf.Configuration import StrippingConf
StrippingConf().ActiveLines = []
StrippingConf().OutputType = "DST"
importOptions ( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2KstarMuMu.py"  )

from Configurables import DaVinci
DaVinci().MainOptions = "$STRIPPINGSELECTIONSROOT/options/StrippingBd2KstarMuMu.py" 
########################################################################
#
# DaVinci
#
DaVinci().EvtMax = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "DC06" # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().TupleFile = "DecayTreeTuple_DC06.root"  # Ntuple
DaVinci().MoniSequence = [ tuple, evtTuple, mcTuple ]
DaVinci().ReplaceL0BanksWithEmulated = True
DaVinci().HltType = "Hlt1+Hlt2"
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bd2KstarMuMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
