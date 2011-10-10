 ## @file DVTestHltDecayTreeTuple-TisTos.py
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
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolMCTruth"
    , "TupleToolTISTOS"
     ]
tuple.Inputs = ["Phys/StdLooseJpsi2MuMu"]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"
#tuple.OutputLevel = 1 
########################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1 
DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09" 
DaVinci().Simulation   = True
DaVinci().CondDBtag = 'MC09-20100430-vc-md100'
DaVinci().TupleFile = "DecayTreeTuple_TisTos.root"  # Ntuple
DaVinci().MoniSequence = [ tuple ]
DaVinci().ReplaceL0BanksWithEmulated = True
DaVinci().Hlt = True 
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09'

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
