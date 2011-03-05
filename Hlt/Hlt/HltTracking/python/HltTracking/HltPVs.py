#!/usr/bin/env python
# =============================================================================
# $Id: HltPVs.py,v 1.11 2010-08-23 20:32:30 witekma Exp $
# =============================================================================
## @file HltTracking/HltPVs.py
#  Define the 2D and 3D primary vertex making algorithms for the Hlt
#  @author V. Gligorov vladimir.gligorov@cern.ch
#  @date 2010-02-21
# =============================================================================
#
# the (ordered) list of (configurables corresponding to the) algorithms needed to 
# obtain the Hlt1 selection 'PV3D' can be obtained by doing 
#
#   from HltTracking.HltPVs import PV3D
#   PV3D.members() 
#
# in addition, the 'outputselection' of the last member can be 
# obtained by doing:
#
#   PV3D.outputSelection()
#
# which should return 'PV3D' of course...
# The returned object can be used in the 'algos' list used by Hlt1Line,
# and, given that 'outputSelection' is properly set, partakes in the automated
# daisychaining present in Hlt1Line. In case you do _not_ want this
# daisychaining, you can do:
#
#  PV3D.ignoreOutputSelection()
#
# which 'skips' PV3D for the daisychaining (but will insert the right configurables)
#

__all__ = ( 'PV3D'            # bindMembers instance with algorithms needed to get 'PV3D'
          )
#############################################################################################
# Import Configurables
#############################################################################################
from Gaudi.Configuration import *
#############################################################################################
# Configure PV algorithms
#############################################################################################
from HltLine.HltLine import bindMembers

PV3DSelection = 'PV3D'

def _RecoPV3D():
    from HltTrackNames import HltSharedRZVeloTracksName, HltSharedTracksPrefix, _baseTrackLocation
    from HltVertexNames import HltSharedVerticesPrefix
    from HltVertexNames import Hlt3DPrimaryVerticesName,_vertexLocation
    from HltVertexNames import HltGlobalVertexLocation
    from HltReco import MinimalVelo

    from Configurables import PatPV3D
    from Configurables import PVOfflineTool, LSAdaptPV3DFitter

    output3DVertices = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,Hlt3DPrimaryVerticesName)

    recoPV3D = PatPV3D('HltPVsPV3D' )
    recoPV3D.addTool(PVOfflineTool,"PVOfflineTool")
    recoPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter, "LSAdaptPV3DFitter")
    recoPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
    recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2.
    #recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.zVtxShift = 0.0
    recoPV3D.OutputVerticesName = output3DVertices
    recoPV3D.PVOfflineTool.InputTracks = [ MinimalVelo.outputSelection() ]

    from Configurables import HltCopySelection_LHCb__RecVertex_ as HltCopyVertexSelection
    preparePV3D = HltCopyVertexSelection( 'Hlt1PreparePV3D'
                                   , InputSelection = "TES:" + recoPV3D.OutputVerticesName
                                   , RequirePositiveInputs = False
                                   , OutputSelection   = PV3DSelection )

    return bindMembers( "HltPVsRecoPV3D", [ recoPV3D, preparePV3D ] )

def PV3D():
    from HltReco import MinimalVelo
    return bindMembers( "HltPVsPV3D", [ MinimalVelo, _RecoPV3D() ])

## Symbols for streamer framework
RecoPV3D = "RecoPV3D =  execute( %s )" % [ m.getFullName() for m in _RecoPV3D().members() ]
FullPV3D = "FullPV3D =  execute( %s )" % [ m.getFullName() for m in PV3D().members() ]
