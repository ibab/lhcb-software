from Gaudi.Configuration import *
from Configurables import ( Escher, TrackSys )
from GaudiKernel.ProcessJobOptions import importOptions
from GaudiKernel.SystemOfUnits import mm
from Configurables import ( ProcessPhase, MagneticFieldSvc,
                            DecodeVeloRawBuffer,
                            Tf__PatVeloRTracking, Tf__PatVeloSpaceTool,
                            Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking,
                            Tf__PatVeloTrackTool, Tf__PatVeloGeneric
                           )
#
# tuning for Pattern recognition algorithm
#

# Uncomment to skip the PatVeloAlignTool

#PatVeloLoadClusters().IgnoreAlignment       = True

# 
# 1. For classic VELO tracks
# 

Tf__PatVeloRTracking().rMatchTol   = 2.
Tf__PatVeloRTracking().rExtraTol   = 2.
Tf__PatVeloRTracking().MaxRSlope   = 0.6
#Tf__PatVeloRTracking().rOverlapTol = 0.
#Tf__PatVeloRTracking().PrivateBest = True # !!! This option is not yet available

Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
toolPatVeloSpaceTracking = Tf__PatVeloSpaceTracking("PatVeloSpaceTracking").PatVeloSpaceTool
toolPatVeloSpaceTracking.PhiAngularTol = 0.02
toolPatVeloSpaceTracking.PhiMatchTol   = 1.5
toolPatVeloSpaceTracking.PhiFirstTol   = 1.5
toolPatVeloSpaceTracking.MaxChiSqDof   = 15.0
#toolPatVeloSpaceTracking.CleanOverlaps    = True

#
# 2. For HALO tracks
#


Tf__PatVeloGeneralTracking().PointErrorMin = 2*mm
Tf__PatVeloGeneralTracking().MaxExtrapStations = 4
Tf__PatVeloGeneralTracking().PointErrorScale = 2
Tf__PatVeloGeneralTracking().ErrorExtrapScale = 10
#Tf__PatVeloGeneralTracking().MaxChiAdd = 
#Tf__PatVeloGeneralTracking().MaxMissedSensor = 
Tf__PatVeloGeneralTracking().MaxChiSqDof = 10
Tf__PatVeloGeneralTracking("PatVeloGeneralTracking").addTool(Tf__PatVeloTrackTool(), name="PatVeloTrackTool")
Tf__PatVeloTrackTool("PatVeloTrackTool").highChargeFract = 0.5;

Tf__PatVeloGeneric().FullAlignment        = True
Tf__PatVeloGeneric().MinModules           = 7  # 5 for closed VELO, 3 for Open
Tf__PatVeloGeneric().ClusterCut           = 30
Tf__PatVeloGeneric().KalmanState          = 4
Tf__PatVeloGeneric().ForwardProp          = True
Tf__PatVeloGeneric().MaxSkip              = 3
Tf__PatVeloGeneric().CleanSeed            = True
Tf__PatVeloGeneric().PrivateBest          = True

Tf__PatVeloGeneric().SigmaTol             = 5
Tf__PatVeloGeneric().RAliTol              = 0.05
Tf__PatVeloGeneric().PAliTol              = 0.005

Tf__PatVeloGeneric().ACDC                 = False  

Tf__PatVeloGeneric().ConsiderOverlaps     = True
