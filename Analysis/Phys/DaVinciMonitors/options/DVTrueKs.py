 ## @file DVTrueKs.py
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2009-01-22
 #
########################################################################
#
# The MC truth Tuple
#
from Configurables import MCDecayTreeTuple
mcTuple = MCDecayTreeTuple("MCTuple")
mcTuple.Decay = "[KS0 -> ^pi+ ^pi- {, gamma}{, gamma}]cc"
mcTuple.ToolList = [ "MCTupleToolMCTruth", "TupleToolEventInfo", "MCTupleToolReconstructed", "MCTupleToolAngles"  ]
#mcTuple.OutputLevel = 1
########################################################################
#
# The reconstructed tuple
#
from Configurables import DecayTreeTuple, PhysDesktop 
tuple = DecayTreeTuple("Tuple")
tuple.ToolList +=  [
      "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolAngles"
    , "TupleToolPrimaries"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
     ]
tuple.addTool( PhysDesktop)
tuple.PhysDesktop.InputLocations = ["StdLooseKsLL", "StdLooseKsDD"]
tuple.Decay = "[KS0 -> ^pi+ ^pi-]cc"

########################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 100000
DaVinci().PrintFreq = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().TupleFile = "test.root"  # Ntuple
DaVinci().UserAlgorithms = [  ]
DaVinci().MoniSequence = [ mcTuple, tuple ]
