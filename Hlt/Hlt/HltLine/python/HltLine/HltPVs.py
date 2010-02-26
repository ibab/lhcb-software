#!/usr/bin/env python
# =============================================================================
# $Id: HltPVs.py,v 1.3 2010-02-26 04:58:39 gligorov Exp $
# =============================================================================
## @file HltLine/HltPVs.py
#  Define the 2D and 3D primary vertex making algorithms for the Hlt
#  @author V. Gligorov vladimir.gligorov@cern.ch
#  @date 2010-02-21
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'PV2D' can be obtained by doing 
#
#   from HltLine.HltPVs import PV2D
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
from Configurables import PatPV2DFit3D, PatPV3D, PVOfflineTool
#############################################################################################
# Configure PV algorithms
#############################################################################################
from HltReco import MinimalRZVelo
from HltTrackNames import HltSharedRZVeloTracksName, HltSharedTracksPrefix, _trackLocation
from HltLine import bindMembers
from Configurables import Hlt2Tracking
from Configurables import HltVertexFilter
from HltVertexNames import HltSharedVerticesPrefix,Hlt1VerticesPrefix,Hlt2VerticesPrefix
from HltVertexNames import Hlt2DPrimaryVerticesName, Hlt3DPrimaryVerticesName,_vertexLocation
from HltTrackNames import HltGlobalTrackLocation
from HltVertexNames import HltGlobalVertexLocation

#### Primary vertex algorithms...

def PV2D() :
	# Todo: fix hardcoding here
	patPV2D = PatPV2DFit3D( 'HltPVsPV2D' , InputTracksName = _trackLocation(HltSharedTracksPrefix, HltGlobalTrackLocation,"", HltSharedRZVeloTracksName) 
                                      , OutputVerticesName = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt2DPrimaryVerticesName) )  
	patPV2D.addTool(PVOfflineTool, name = 'PVOfflineTool')
	patPV2D.PVOfflineTool.PVFitterName='LSAdaptPV3DFitter'

	preparePV2D = HltVertexFilter( 'Hlt1PreparePV2D'
                             , InputSelection = "TES:" + PatPV2DFit3D('HltPVsPV2D').OutputVerticesName
                             , RequirePositiveInputs = False
                             , FilterDescriptor = ["VertexZPosition,>,-5000","VertexTransversePosition,>,-1"]
                             , HistoDescriptor = {'VertexZPosition': ( 'PV2D: VertexZPosition',-200.,200.,200),
                                                  'VertexZPositionBest': ( 'PV2D: Highest VertexZPosition',-200,200.,100),
                                                  'VertexTransversePosition': ( 'PV2D: VertexTransversePosition',0,1,50),
                                                  'VertexTransversePositionBest': ( 'PV2D: Highest VertexTransversePosition',0,1,50)
                                                    }
                             , OutputSelection   = "PV2D" )

	return bindMembers( None, [ MinimalRZVelo, patPV2D, preparePV2D ] )

def PV3D() :

	recoPV3D =  PatPV3D('HltPVsPV3D' )
	recoPV3D.addTool( PVOfflineTool, name = 'PVOfflineTool' )
	recoPV3D.PVOfflineTool.InputTracks = [ (Hlt2Tracking().hlt2VeloTracking()).outputSelection() ]
	recoPV3D.OutputVerticesName = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt3DPrimaryVerticesName) 

	return bindMembers( None, [ Hlt2Tracking().hlt2VeloTracking(), recoPV3D ] )
