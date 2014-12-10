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
from HltVertexNames import Hlt3DPrimaryVerticesName as PV3DSelection
from HltVertexNames import _vertexLocation

ProtoPV3DSelection = 'Proto' + PV3DSelection

def PV3D(where):
    """ PV3D(where) -- where must either by Hlt1 or Hlt2 """
    #from HltTrackNames import HltSharedRZVeloTracksName, HltSharedTracksPrefix, _baseTrackLocation
    from HltVertexNames import HltSharedVerticesPrefix
    from HltVertexNames import HltGlobalVertexLocation

    from Configurables import PatPV3D
    from Configurables import PVOfflineTool, LSAdaptPV3DFitter

    if where.upper() not in ['HLT1','HLT2'] : raise KeyError('PV3D: where must be either HLT1 or HLT2')

    output3DVertices = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,PV3DSelection)
    proto3DVertices = _vertexLocation(HltSharedVerticesPrefix,HltGlobalVertexLocation,ProtoPV3DSelection)
    recoPV3D = PatPV3D('HltPVsPV3D' )
    recoPV3D.addTool(PVOfflineTool,"PVOfflineTool")
    recoPV3D.PVOfflineTool.UseBeamSpotRCut = True
    recoPV3D.PVOfflineTool.PVSeedingName = "PVSeedTool"
    recoPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
    recoPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter, "LSAdaptPV3DFitter")
    recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2.
    recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 0.3
    #recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.zVtxShift = 0.0
    recoPV3D.OutputVerticesName = proto3DVertices
    
    
    from HltSharedTracking import MinimalVelo
    velo = MinimalVelo
    name = "HltPV3D"

    if where.upper() == "HLT2" :
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        velo  = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()
        if velo.members() != MinimalVelo.members() : name = "Hlt2PV3D"

    pv3dAlgos = ','.join( [ "'%s'"%m.getFullName() for m in velo.members() + [ recoPV3D ] ] )
    recoPV3D.PVOfflineTool.InputTracks = [ velo.outputSelection() ]
        
    from Configurables import LoKi__HltUnit as HltUnit
    ## Hlt vertex beamspot filter
    ##-- todo: can we integrate this in the main streamers directly, using 'tee' ?
    filterPV3D = HltUnit(
        name,
        Preambulo = [ 'from LoKiPhys.decorators import *',
                      'from LoKiTrigger.decorators import *',
                      'from LoKiHlt.algorithms import *' ],
        Code = """
        execute( %(algo)s ) * VSOURCE( '%(tesInput)s' )
        >> VX_SINK( '%(hltProto)s' )
        >> ( VX_BEAMSPOTRHO( 1 * mm ) < 0.3 * mm )
        >> RV_SINKTES( '%(tesFinal)s' )
        >> VX_SINK( '%(hltFinal)s' )
        >> ~VEMPTY
        """ % { 'algo'     : pv3dAlgos,
                'tesInput' : recoPV3D.OutputVerticesName,
                'hltProto' : ProtoPV3DSelection,
                'tesFinal' : output3DVertices,
                'hltFinal' : PV3DSelection   }

        )
    
    pv3d = bindMembers( name+'Sequence', [ filterPV3D ] ).setOutputSelection( PV3DSelection ) 
    pv3d.output = output3DVertices
    return pv3d

## Symbols for streamer framework
# don't work, as this now contains a HltUnit, and we get into trouble if we go recursive in HltUnit
#FullPV3D = "FullPV3D =  execute( %s )" % [ m.getFullName() for m in PV3D().members() ]
#RecoPV3D = "RecoPV3D =  execute( %s )" % [ m.getFullName() for m in PV3D().members() ]
