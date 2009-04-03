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
from Configurables import LHCbConfigurableUser


class LumiCounterDefinitionConf(LHCbConfigurableUser) :
  __slots__ = {
    'FullReco'   : True,
    'Definition' : {},
    }

  ####### counter definition...
  def defineCounters( self ):
    fullReco = self.getProp('FullReco')
    self.Definition  = {
      'Velo'     : [LumiCountTracks   , fullReco, 'Hlt/Track/Velo'  ,   5,  200],
      'RZVelo'   : [LumiCountTracks   , fullReco, 'Hlt/Track/RZVelo',   5,  200],
      'Muon'     : [LumiCountTracks   , False   , 'Rec/Tracks/Muons',   5,  200],
      'PV2D'     : [LumiCountVertices , fullReco, 'Hlt/Vertex/PV2D' ,   1,   20],
      'PV3D'     : [LumiCountVertices , False   , 'Hlt/Vertex/PV3D' ,   1,   20],
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

            
