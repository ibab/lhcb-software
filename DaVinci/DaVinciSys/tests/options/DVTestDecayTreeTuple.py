 ## @file DVTestHltDecayTreeTuple.py
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
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
tuple.InputLocations = ["StdLooseJpsi2MuMu"]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"
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
mcTuple.Decay = "J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}"
mcTuple.ToolList = [ "MCTupleToolMCTruth", "TupleToolEventInfo", "MCTupleToolReconstructed"  ]
#mcTuple.OutputLevel = 1
########################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "2009" # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().TupleFile = "DecayTreeTuple.root"  # Ntuple
DaVinci().MoniSequence = [ tuple, evtTuple, mcTuple ]
DaVinci().ReplaceL0BanksWithEmulated = True
DaVinci().HltType = "Hlt1+Hlt2"
DaVinci().Input = [
    "DATAFILE='PFN:/castor/cern.ch/user/c/cattanem/Brunel/v34r5/00001820_00000001.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
