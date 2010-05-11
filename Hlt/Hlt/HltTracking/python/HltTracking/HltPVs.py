#!/usr/bin/env python
# =============================================================================
# $Id: HltPVs.py,v 1.8 2010-05-11 17:30:31 gligorov Exp $
# =============================================================================
## @file HltTracking/HltPVs.py
#  Define the 2D and 3D primary vertex making algorithms for the Hlt
#  @author V. Gligorov vladimir.gligorov@cern.ch
#  @date 2010-02-21
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'PV2D' can be obtained by doing 
#
#   from HltTracking.HltPVs import PV2D
#   PV2D.members() 
#
# in addition, the 'outputselection' of the last member can be 
# obtained by doing:
#
#   PV2D.outputSelection()
#
# which should return 'PV2D' of course...
# The returned object can be used in the 'algos' list used by Hlt1Line,
# and, given that 'outputSelection' is properly set, partakes in the automated
# daisychaining present in Hlt1Line. In case you do _not_ want this
# daisychaining, you can do:
#
#  PV2D.ignoreOutputSelection()
#
# which 'skips' PV2D for the daisychaining (but will insert the right configurables)
#

__all__ = ( 'PV2D'            # bindMembers instance with algorithms needed to get 'PV2D'
          , 'PV3D'            # bindMembers instance with algorithms needed to get 'PV3D'
          )
#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
#############################################################################################
# Configure PV algorithms
#############################################################################################
from HltTrackNames import HltSharedRZVeloTracksName, HltSharedTracksPrefix, _baseTrackLocation
from HltLine.HltLine import bindMembers
from HltVertexNames import HltSharedVerticesPrefix,Hlt1VerticesPrefix,Hlt2VerticesPrefix
from HltVertexNames import Hlt2DPrimaryVerticesName, Hlt3DPrimaryVerticesName,_vertexLocation
from HltVertexNames import HltGlobalVertexLocation

#### Primary vertex algorithms...

def ForcedRecoVeloForPVs() :
    # The temporary fix for the PV problem by Kalman fitting all the Velo tracks assuming
    # a 400 MeV PT.
    # TODO: replace this with a proper solution!!!

    from HltReco import MinimalRZVelo
 
    outputTracks = "Hlt/Track/ForcedVelo"
    outputFitted = "Hlt/Track/SpecialFitted/ForcedVelo"

    from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool 
    recoVelo		 = Tf__PatVeloSpaceTracking('HltForcedRecoVeloForPV'
                                       	, InputTracksName  = MinimalRZVelo.outputSelection() 
                                       	, OutputTracksName = outputTracks )
    
    recoVelo.addTool( Tf__PatVeloSpaceTool, name="PatVeloSpaceTool" )
    recoVelo.PatVeloSpaceTool.MarkClustersUsed=False

    from Configurables import TrackEventFitter, TrackMasterFitter
    HltForcedFastFitForPV_name      = "HltForcedFastFitForPV"
    HltForcedFastFitForPV           = TrackEventFitter(HltForcedFastFitForPV_name)
    HltForcedFastFitForPV.TracksInContainer	    = outputTracks 
    HltForcedFastFitForPV.TracksOutContainer    = outputFitted  
    HltForcedFastFitForPV.addTool(TrackMasterFitter, name = 'Fitter')
    from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
    fitter = ConfiguredFastFitter(getattr(HltForcedFastFitForPV,'Fitter'))
    fitter.NodeFitter.BiDirectionalFit  = True
    fitter.NodeFitter.Smooth        = True
    fitter.AddDefaultReferenceNodes = True 
    
    return bindMembers("HltForcedRecoVeloForPV",[recoVelo,HltForcedFastFitForPV]).setOutputSelection(outputFitted) 

def PV2D() :

    from Configurables import PatPV2DFit3D, PVOfflineTool
    from HltReco import MinimalRZVelo
    from Configurables import HltVertexFilter
    from Configurables import PatPV3D
    from Configurables import PVOfflineTool,PVSeedTool,LSAdaptPVFitter 
    
    # Temporary cheat while the 2D PVs are not fixed yet!
    recoCheatPV2D =  PatPV3D('HltPVsCheatPV2D' )
    recoCheatPV2D.addTool( PVOfflineTool, "PVOfflineTool")
    recoCheatPV2D.OutputVerticesName = "Hlt/Vertex/PV2D"
    recoCheatPV2D.PVOfflineTool.InputTracks = [ "Hlt1/Track/Velo" ]
    
    EarlyData = False
    if EarlyData :
        recoCheatPV2D.PVOfflineTool.addTool(PVSeedTool, "PVSeedTool")
        recoCheatPV2D.PVOfflineTool.addTool(LSAdaptPVFitter, "LSAdaptPVFitter")
        recoCheatPV2D.PVOfflineTool.PVSeedTool.minClusterMult = 2
        recoCheatPV2D.PVOfflineTool.PVSeedTool.minCloseTracksInCluster = 2
        recoCheatPV2D.PVOfflineTool.PVSeedTool.ratioSig2HighMult = 1
        recoCheatPV2D.PVOfflineTool.PVSeedTool.ratioSig2LowMult =1
        recoCheatPV2D.PVOfflineTool.LSAdaptPVFitter.MinTracks = 2 
        

    prepareForcedPV2D = HltVertexFilter( 'Hlt1PrepareForcedPV2D'
                             , InputSelection = "TES:" + recoCheatPV2D.OutputVerticesName
                             , RequirePositiveInputs = False
                             , FilterDescriptor = ["VertexZPosition,>,-5000","VertexTransversePosition,>,-1"]
                             , HistoDescriptor = {'VertexZPosition': ( 'PV2D: VertexZPosition',-200.,200.,200),
                                                  'VertexZPositionBest': ( 'PV2D: Highest VertexZPosition',-200,200.,100),
                                                  'VertexTransversePosition': ( 'PV2D: VertexTransversePosition',0,1,50),
                                                  'VertexTransversePositionBest': ( 'PV2D: Highest VertexTransversePosition',0,1,50)
                                                    }
                             , OutputSelection   = "PV2D" )

    recoCheatPV2D.PVOfflineTool.InputTracks = [ForcedRecoVeloForPVs().outputSelection()]

    return bindMembers( "HltPVsForcedPV2D", [ MinimalRZVelo, ForcedRecoVeloForPVs(), recoCheatPV2D, prepareForcedPV2D ] )  
 
    #from Configurables import HltTrackUpgradeTool
    #from HltLine.HltLine import Hlt1Member as Member 
    #upgrade2Dto3D =  Member ( 'TU', 'Velo',  InputSelection = "TES:Hlt/Track/RZVelo" , RecoName = 'Velo')
    #from HltReco import RZVelo
    #return bindMembers( "HltPVsForcedPV2D", [ RZVelo, upgrade2Dto3D, recoCheatPV2D, prepareForcedPV2D ] )

    #return bindMembers( "HltPVsForcedPV2D", [ MinimalRZVelo, upgrade2Dto3D, recoCheatPV2D, prepareForcedPV2D ] )
    
    # End of temporary cheat, rest of code is ignored for now!!!

    # Todo: fix hardcoding here
    #
    output2DVertices = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt2DPrimaryVerticesName)
    #
    patPV2D = PatPV2DFit3D( 'HltPVsPV2D' , InputTracksName = MinimalRZVelo.outputSelection()
                                      , OutputVerticesName = output2DVertices )  
    patPV2D.addTool(PVOfflineTool, name = 'PVOfflineTool')
    patPV2D.PVOfflineTool.PVFitterName='LSAdaptPV3DFitter'

    preparePV2D = HltVertexFilter( 'Hlt1PreparePV2D'
                             , InputSelection = "TES:" + patPV2D.OutputVerticesName
                             , RequirePositiveInputs = False
                             , FilterDescriptor = ["VertexZPosition,>,-5000","VertexTransversePosition,>,-1"]
                             , HistoDescriptor = {'VertexZPosition': ( 'PV2D: VertexZPosition',-200.,200.,200),
                                                  'VertexZPositionBest': ( 'PV2D: Highest VertexZPosition',-200,200.,100),
                                                  'VertexTransversePosition': ( 'PV2D: VertexTransversePosition',0,1,50),
                                                  'VertexTransversePositionBest': ( 'PV2D: Highest VertexTransversePosition',0,1,50)
                                                    }
                             , OutputSelection   = "PV2D" )

    return bindMembers( "HltPVsPV2D", [ MinimalRZVelo, patPV2D, preparePV2D ] )

def PV3D() :

    from Configurables import PatPV3D
    from Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
    from Hlt2TrackingConfigurations import Hlt2BiKalmanFittedLongTracking
    from Configurables import PVOfflineTool,PVSeedTool,LSAdaptPVFitter 
    from HltReco import MinimalVelo

    output3DVertices = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt3DPrimaryVerticesName)

    recoPV3D =  PatPV3D('HltPVsPV3D' )
    recoPV3D.addTool(PVOfflineTool,"PVOfflineTool")
    recoPV3D.OutputVerticesName = output3DVertices
    recoPV3D.PVOfflineTool.InputTracks = [MinimalVelo.outputSelection()]
    #recoPV3D.PVOfflineTool.InputTracks = [ForcedRecoVeloForPVs().outputSelection()] 
    #recoPV3D.PVOfflineTool.InputTracks = [ (Hlt2UnfittedForwardTracking().hlt2VeloTracking()).outputSelection() ]
    #recoPV3D.PVOfflineTool.InputTracks = [ (Hlt2BiKalmanFittedLongTracking().hlt2PrepareTracks()).outputSelection() ]   

    EarlyData = False
    if EarlyData :

        recoPV3D.PVOfflineTool.addTool(PVSeedTool, "PVSeedTool")
        recoPV3D.PVOfflineTool.addTool(LSAdaptPVFitter, "LSAdaptPVFitter")
        recoPV3D.PVOfflineTool.PVSeedTool.minClusterMult = 2
        recoPV3D.PVOfflineTool.PVSeedTool.minCloseTracksInCluster = 2
        recoPV3D.PVOfflineTool.PVSeedTool.ratioSig2HighMult = 1
        recoPV3D.PVOfflineTool.PVSeedTool.ratioSig2LowMult =1
        recoPV3D.PVOfflineTool.LSAdaptPVFitter.MinTracks = 2 

    from Configurables import HltVertexFilter   
 
    preparePV3D = HltVertexFilter( 'Hlt1PreparePV3D'
                             , InputSelection = "TES:" + recoPV3D.OutputVerticesName
                             , RequirePositiveInputs = False
                             , FilterDescriptor = ["VertexZPosition,>,-5000","VertexTransversePosition,>,-1"]
                             , HistoDescriptor = {'VertexZPosition': ( 'PV3D: VertexZPosition',-200.,200.,200),
                                                  'VertexZPositionBest': ( 'PV3D: Highest VertexZPosition',-200,200.,100),
                                                  'VertexTransversePosition': ( 'PV3D: VertexTransversePosition',0,1,50),
                                                  'VertexTransversePositionBest': ( 'PV3D: Highest VertexTransversePosition',0,1,50)
                                                    }
                             , OutputSelection   = "PV3D" )

    return bindMembers( "HltPVsPV3D", [ MinimalVelo, recoPV3D, preparePV3D]) 
    #return bindMembers( "HltPVsPV3D", [ ForcedRecoVeloForPVs(), recoPV3D ] )   
    #return bindMembers( "HltPVsPV3D", [ Hlt2BiKalmanFittedLongTracking().hlt2PrepareTracks(), recoPV3D ] )
    #return bindMembers( "HltPVsPV3D", [ Hlt2UnfittedForwardTracking().hlt2VeloTracking(), recoPV3D ] )
