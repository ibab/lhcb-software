#!/usr/bin/env python
# =============================================================================
# $Id: HltPVs.py,v 1.3 2010-04-21 08:24:33 graven Exp $
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

def PV2D() :

    from Configurables import PatPV2DFit3D, PVOfflineTool
    from HltReco import MinimalRZVelo
    from Configurables import HltVertexFilter
    from Configurables import PatPV3D
    from Configurables import PVOfflineTool 
    
    # Temporary cheat while the 2D PVs are not fixed yet!
    recoCheatPV2D =  PatPV3D('HltPVsCheatPV2D' )
    recoCheatPV2D.addTool( PVOfflineTool, name = 'PVOfflineTool' )
    recoCheatPV2D.PVOfflineTool.InputTracks = [ "Hlt1/Track/Velo" ]
    recoCheatPV2D.OutputVerticesName = "Hlt/Vertex/PV2D"

    prepareCheatedPV2D = HltVertexFilter( 'Hlt1PrepareCheatedPV2D'
                             , InputSelection = "TES:" + recoCheatPV2D.OutputVerticesName
                             , RequirePositiveInputs = False
                             , FilterDescriptor = ["VertexZPosition,>,-5000","VertexTransversePosition,>,-1"]
                             , HistoDescriptor = {'VertexZPosition': ( 'PV2D: VertexZPosition',-200.,200.,200),
                                                  'VertexZPositionBest': ( 'PV2D: Highest VertexZPosition',-200,200.,100),
                                                  'VertexTransversePosition': ( 'PV2D: VertexTransversePosition',0,1,50),
                                                  'VertexTransversePositionBest': ( 'PV2D: Highest VertexTransversePosition',0,1,50)
                                                    }
                             , OutputSelection   = "PV2D" )

    from Configurables import HltTrackUpgradeTool
    from HltLine.HltLine import Hlt1Member as Member

    upgrade2Dto3D =  Member ( 'TU', 'Velo',  RecoName = 'Velo')

    from HltReco import RZVelo

    return bindMembers( "HltPVsCheatedPV2D", [ RZVelo, upgrade2Dto3D, recoCheatPV2D, prepareCheatedPV2D ] )
    
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
    from Configurables import PVOfflineTool 

    output3DVertices = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt3DPrimaryVerticesName)

    recoPV3D =  PatPV3D('HltPVsPV3D' )
    recoPV3D.addTool( PVOfflineTool, name = 'PVOfflineTool' )
    recoPV3D.PVOfflineTool.InputTracks = [ (Hlt2UnfittedForwardTracking().hlt2VeloTracking()).outputSelection() ]
    recoPV3D.OutputVerticesName = output3DVertices

    return bindMembers( "HltPVsPV3D", [ Hlt2UnfittedForwardTracking().hlt2VeloTracking(), recoPV3D ] )
