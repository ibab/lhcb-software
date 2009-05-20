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
    , "TupleToolTISTOS"
     ]
tuple.InputLocations = ["StdLooseJpsi2MuMu"]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"
#tuple.OutputLevel = 1 ;
########################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "2009" # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().TupleFile = "DecayTreeTuple_TisTos.root"  # Ntuple
DaVinci().MoniSequence = [ tuple ]
DaVinci().ReplaceL0BanksWithEmulated = True
DaVinci().HltType = "Hlt1+Hlt2"
DaVinci().Input = [
    "DATAFILE='PFN:/castor/cern.ch/user/c/cattanem/Brunel/v34r5/30000000-100ev-20090407-MC09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
