from Gaudi.Configuration import *

appName            = "LaddersAndModuleAlignment"
appVersion         = "v1r0"
nIter              = 1
nEvents            = 1 #20000
minNumHits         = 100
alignDoFs          = "Tx"
constraintStrategy = 0
constraints        = []
solver             = "gslSolver"

from Alignables import *
elements           = Alignables()
elements.ITLadderOTModules( alignDoFs )

trackingOpts       = "$TALIGNMENTROOT/python/ITOTTracking.py"

from OTdata import *
data               = Bs2JpsiPhiAtCern
output             = "MisAlignedStationsInTxRzInDDDBAlignForTxTyTzRxRyRz"


# Go past this line only when you know what you are doing
############################################################################################################################
## File to setup Alignment

ApplicationMgr( AppName         = appName, AppVersion      = appVersion,
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


# AlConfigurable().CondDBTag = "DC06-repro0710"

## AlternativeDB
AlConfigurable().AlternativeCondDB           = "/afs/cern.ch/lhcb/software/releases/DBASE/Det/SQLDDDB/v4r3/db/LHCBCOND.db/LHCBCOND"
AlConfigurable().AlternativeCondDBTag        = "DC06-20080407"
AlConfigurable().AlternativeCondDBOverlays   = [ "/Conditions/IT", "/Conditions/ET"]

## Patttern Recognition?
AlConfigurable().Pat                          = True

## Set output level
AlConfigurable().OutputLevel                  = INFO

## Configure alignment
AlConfigurable().ElementsToAlign              = elements
AlConfigurable().NumIterations                = nIter
AlConfigurable().AlignInputTackCont           = "Alignment/FilteredTracks"
AlConfigurable().UseCorrelations              = True
AlConfigurable().CanonicalConstraintStrategy  = constraintStrategy
AlConfigurable().Constraints                  = constraints
AlConfigurable().UseWeightedAverageConstraint = False
AlConfigurable().MinNumberOfHits              = minNumHits
AlConfigurable().UsePreconditioning           = False
AlConfigurable().SolvTool                     = solver
AlConfigurable().WriteCondToXML               = True
AlConfigurable().CondFileName                 = "Conditions.xml"
AlConfigurable().Precision                    = 8

## Call after all options are set
AlConfigurable().applyConf()

## Here we configure the track fitting/selection and what else?
importOptions( trackingOpts )

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

def update(algorithm, appMgr) :
   # get pointer to incident service
   incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
   # incident to trigger update of constants
   updateConstants = gbl.Incident( algorithm, 'UpdateConstants' )
   incSvc.fireIncident( updateConstants )

HistogramPersistencySvc().OutputFile = "$TALIGNMENTROOT/python/" + output + ".root"

## Instantiate application manager
appMgr = AppMgr()

## Print flow of application
AlConfigurable().printFlow(appMgr)

## Get pointer to event selector
evtSel           = appMgr.evtSel()
## Set print frequency
evtSel.printfreq = 500

## Open Files; Also initialises Application Manager
evtSel.open( data )

## Get pointer to ot traj projector
otProjector = appMgr.tool( "ToolSvc.TrajOTProjector" )
chi2Cleaner = appMgr.tool( "CleanChi2.Selector" )

chi2CutStrategy = [100, 40, 20, 10]

for i in range( nIter ) :
    evtSel.rewind()

    if i == 0 :
       chi2Cleaner.Chi2PerDoFMaxCut = chi2CutStrategy[0]
    elif i == 1 :
       chi2Cleaner.Chi2PerDoFMaxCut = chi2CutStrategy[1]
    elif i == 2 :
       chi2Cleaner.Chi2PerDoFMaxCut = chi2CutStrategy[2]
    elif i > 3 :
       chi2Cleaner.Chi2PerDoFMaxCut = chi2CutStrategy[3]
    
    if i < (nIter/2) :
       otProjector.UseDrift = False
    else :
       otProjector.UseDrift = True

    appMgr.run( nEvents )
    update( "Alignment", appMgr )

appMgr.finalize()


