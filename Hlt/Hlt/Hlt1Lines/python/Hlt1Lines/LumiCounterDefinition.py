##############################
#     helper configuration for HLT Lumi Line(s)
##############################


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from Configurables import ( LumiCountVertices,
                            LumiCountTracks,
                            LumiFromL0DU,
                            LumiCountHltTracks
                            )
from Configurables  import LHCbConfigurableUser

# We import the track and vertex names from
# HltTrackNames and HltVertexNames
#
from HltLine.HltTrackNames import _trackLocation, HltSharedTracksPrefix, HltGlobalTrackLocation
from HltLine.HltTrackNames import HltSharedRZVeloTracksName, HltMuonTracksName, HltUnfittedTracksSuffix  
from HltLine.HltVertexNames import _vertexLocation, HltSharedVerticesPrefix, HltGlobalVertexLocation
from HltLine.HltVertexNames import Hlt2DPrimaryVerticesName, Hlt3DPrimaryVerticesName
#from HltLine.HltReco import MinimalRZVelo
#
# TODO : make this inherit the relevant tracking configurables
#        instead, once configuration issues are resolved

class LumiCounterDefinitionConf(LHCbConfigurableUser) :
  __slots__ = {
    'Definition' : {},
    }

  ####### counter definition...
  def defineCounters( self ):
    self.Definition  = {
      'RZVelo'   : [LumiCountTracks   , True    , "Hlt/Track/RZVelo",   5,  200],
      'Muon'     : [LumiCountTracks   , False   , "Hlt2/Track/Unfitted/Forward/Muon" ,   5,  200], #TODO: remove!!!
      'TTIP'     : [LumiCountTracks   , True    , 'Hlt/Track/TTIP'  ,   5,  100], #TODO : define in HltTrackNames 
      'TTMIB'    : [LumiCountTracks   , False   , 'Hlt/Track/TTMIB' ,   5,  100], #TODO : define in HltTrackNames
      'PV2D'     : [LumiCountVertices , True    , _vertexLocation(HltSharedVerticesPrefix, HltGlobalVertexLocation, Hlt2DPrimaryVerticesName) ,   1,   20],
      'PV3D'     : [LumiCountVertices , False   , _vertexLocation(HltSharedVerticesPrefix, HltGlobalVertexLocation, Hlt3DPrimaryVerticesName) ,   1,   20],
      'RZVeloBW' : [LumiCountHltTracks, True    , 'RZVeloBW'        ,   5,  200],
      'SPDMult'  : [LumiFromL0DU      , True    , 'Spd(Mult)'       ,   6,  500],
      'PUMult'   : [LumiFromL0DU      , True    , 'PUHits(Mult)'    ,   3,  200],
      'CaloEt'   : [LumiFromL0DU      , True    , 'Sum(Et)'         , 500, 6000],
      }
    return self.Definition


  def defineHistos(self):
    '''
    returns list of counters and properties for histogramming
    '''
    counters = self.defineCounters()
    defs={}
    for key, definition in counters.iteritems():
      (op, flag, inputDef, threshold, bins) = definition
      if flag:
        defs[key]=[threshold, bins]
    return defs

  def __repr__(self):
    return self.Definition
  
  def __apply_configuration__(self) :
    '''
    defines counter definition for common usage
    '''
    self.defineCounters()

            
