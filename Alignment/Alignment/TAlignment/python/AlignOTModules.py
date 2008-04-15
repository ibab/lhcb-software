## File to setup Alignment
from Gaudi.Configuration import *

ApplicationMgr( AppName         = "TAlignment", AppVersion      = "v3r4",
                AuditTools      = True       , AuditServices   = True  , AuditAlgorithms = True )

from Configurables import LbAppInit
ApplicationMgr().TopAlg.append( LbAppInit( ApplicationMgr().AppName ) )

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.printing_level += 1

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
# AlConfigurable().AlternativeDB        = "$TALIGNMENTROOT/python/LHCBCOND.db/LHCBCOND"
# AlConfigurable().AlternativeCondDBTag = "MisA-OTL-1"
# AlConfigurable().AlternativeOverlay   = "/Conditions/OT"

## Patttern Recognition?
AlConfigurable().Pat            = True

## Set output level
AlConfigurable().OutputLevel    = INFO

from OTAlElements import *
elements = OTAlElements().OTModules("TxRz")

nIter   = 2

## Configure alignment
AlConfigurable().ElementsToAlign              = { "OTModules" : elements }
AlConfigurable().GroupElements                = True
AlConfigurable().NumIterations                = nIter
AlConfigurable().AlignInputTackCont           = "Alignment/AlignmentTracks"
AlConfigurable().UseCorrelations              = True
AlConfigurable().CanonicalConstraintStrategy  = 1
AlConfigurable().Constraints                  = ["Tx", "Szx"]
AlConfigurable().UseWeightedAverageConstraint = False
AlConfigurable().MinNumberOfHits              = 1
AlConfigurable().UsePreconditioning           = False
AlConfigurable().SolvTool                     = "gslSolver"
AlConfigurable().WriteCondToXML               = True
AlConfigurable().CondFileName                 = "OTConditions.xml"
AlConfigurable().Precision                    = 8

## Call after all options are set
AlConfigurable().applyConf()

## Here we configure the track fitting/selection and what else?
importOptions("$TALIGNMENTROOT/python/OTTrackFitSel.py")

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

nEvents = 2000

def update(algorithm, appMgr) :
   # get pointer to incident service
   incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
   # incident to trigger update of constants
   updateConstants = gbl.Incident( algorithm, 'UpdateConstants' )
   incSvc.fireIncident( updateConstants )

HistogramPersistencySvc().OutputFile = "$TALIGNMENTROOT/python/test.root"

## Instantiate application manager
appMgr = AppMgr()

## Print flow of application
AlConfigurable().printFlow(appMgr)

evtSel           = appMgr.evtSel()
evtSel.printfreq = 500

## Open Files; Also initialises Application Manager
evtSel.open( )

for i in range( nIter ) :

    evtSel.rewind()
    appMgr.run( nEvents )
    update( "Alignment", appMgr )

appMgr.finalize()
