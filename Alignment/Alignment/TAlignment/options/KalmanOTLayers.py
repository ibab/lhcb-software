import Gaudi
import GaudiKernel.ProcessJobOptions
from Gaudi.Configuration  import *
from Configurables import TAlignment
from TAlignment import *

appName            = "LayerAlignment"
appVersion         = "v3r4"
nIter              = 1
nEvents            = 1
minNumHits         = 100
alignDoFs          = "TxRz"
constraintStrategy = 0  #1
constraints        = [] #[ "Tx" ]
preCondition       = False
solver             = "gslSVDSolver"
from Configurables import TAlignment
from TAlignment import Alignables

ali = Alignables.Alignables()
elements           = ali.OTLayers()


trackingOpts       = "$TALIGNMENTROOT/options/OTTrackFitSel.py"

# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

#GaudiKernel.ProcessJobOptions.printing_level += 1


## AlternativeDB
TAlignment().AlternativeCondDB          = "$TALIGNMENTROOT/python/LHCBCOND.db/LHCBCOND"
TAlignment().AlternativeCondDBTag       = "MisA-OTL-1"
TAlignment().AlternativeCondDBOverlays  = ["/Conditions/OT"]

## Patttern Recognition?
TAlignment().Pat                          = True

## Set output level
TAlignment().OutputLevel                  = INFO

## Configure alignment
TAlignment().ElementsToAlign              = elements
TAlignment().NumIterations                = nIter
TAlignment().TrackContainer               = "Alignment/AlignmentTracks"
TAlignment().UseCorrelations              = True
TAlignment().CanonicalConstraintStrategy  = constraintStrategy
TAlignment().Constraints                  = constraints
TAlignment().UseWeightedAverageConstraint = False
TAlignment().MinNumberOfHits              = minNumHits
TAlignment().UsePreconditioning           = preCondition
TAlignment().SolvTool                     = solver
TAlignment().WriteCondToXML               = True
TAlignment().CondFileName                 = "Conditions.xml"
TAlignment().Precision                    = 8

## Call after all options are set
#TAlignment().applyConf()

## Here we configure the track fitting/selection and what else?
importOptions( trackingOpts )
