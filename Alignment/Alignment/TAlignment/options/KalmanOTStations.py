from AlConfigurable import *

## Override OT Stations conditions
#AlConfigurable().CondDBOverride =  [
#     "Conditions/Alignment/OT/T1Station := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#     "Conditions/Alignment/OT/T2Station := double_v dPosXYZ = -0.1 1.0 0.0  ; double_v dRotXYZ = 0.0  0.0005  0.0001 ;",
#     "Conditions/Alignment/OT/T3Station := double_v dPosXYZ =  0.2 -1.0 0.0 ; double_v dRotXYZ = 0.0 -0.0001 -0.0005 ;"
#     ]

## Override OT Layer conditions
#AlConfigurable().CondDBOverride =  [
#    "Conditions/Alignment/OT/T1StationX1layer := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T1StationUlayer  := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T1StationVlayer  := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T1StationX2layer := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T2StationX1layer := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T2StationUlayer  := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T2StationVlayer  := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T2StationX2layer := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T3StationX1layer := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T3StationUlayer  := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T3StationVlayer  := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;",
#    "Conditions/Alignment/OT/T3StationX2layer := double_v dPosXYZ =  0.3 1.5 0.0  ; double_v dRotXYZ = 0.0  0.0001  0.0005 ;"
#    ]


## AlternativeDB
#AlConfigurable().AlternativeDB        = "$TALIGNMENTROOT/python/LHCBCOND.db/LHCBCOND"
#AlConfigurable().AlternativeCondDBTag = "MisA-OTL-1"
#AlConfigurable().AlternativeOverlay   = "/Conditions/OT"

## AlternativeDB
#AlConfigurable().AlternativeDB        = "/afs/cern.ch/user/j/jblouw/LHCBCOND.db"
#AlConfigurable().AlternativeCondDBTag = "AW2_IT_OT_01"
#AlConfigurable().AlternativeOverlay   = "/Conditions/OT"

## Simplified geometry
AlConfigurable().SimplifiedGeom = False

## Patttern Recognition?
AlConfigurable().Pat            = True

## Set output level
AlConfigurable().OutputLevel    = INFO

from OTAlElements import *
elements = OTAlElements().OTStations("TxTyTzRxRyRz")

## Configure alignment
AlConfigurable().ElementsToAlign              = { "OTStations" : elements }
AlConfigurable().GroupElements                = False
AlConfigurable().NumIterations                = nIter
AlConfigurable().AlignInputTackCont           = "Alignment/AlignmentTracks"
AlConfigurable().UseCorrelations              = True
AlConfigurable().CanonicalConstraintStrategy  = 0 #1
AlConfigurable().Constraints                  = [] #["Tx", "Szx"]
AlConfigurable().UseWeightedAverageConstraint = False
AlConfigurable().MinNumberOfHits              = 1
AlConfigurable().UsePreconditioning           = False
AlConfigurable().SolvTool                     = "CLHEPSolver"
AlConfigurable().WriteCondToXML               = True
AlConfigurable().CondFileName                 = "OTConditions.xml"
AlConfigurable().Precision                    = 8

## Call after all options are set
AlConfigurable().applyConf()

## Here we configure the track fitting/selection and what else?
importOptions("$TALIGNMENTROOT/python/OTTrackFitSel.py")

