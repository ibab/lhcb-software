#/** @file HltTrackAssociator.opts
# *  
# *  Creation of table for TrackAssociator
# *
# *  @author P. Koppenburg
# *  @date 2008-02-18
# */
#//--------------------------------------------------------------

from Configurables import DataOnDemandSvc, TrackAssociator

DataOnDemandSvc().AlgMap.update(  { 'Link/Hlt/Track/Long': 'TrackAssociator/HltTrackAssociator'
                                  , 'Link/Hlt/Track/Muons':   'TrackAssociator/HltMuonAssociator'
                                  , 'Link/Hlt/Track/SeedTT':  'TrackAssociator/HltSeedAssociator'
                                  , 'Link/Hlt/Track/TFForwardForTopo':  'TrackAssociator/HltTFAssociator'
                                  } )

TrackAssociator("HltTrackAssociator").TracksInContainer = "Hlt/Track/Long" 
TrackAssociator("HltMuonAssociator").TracksInContainer = "Hlt/Track/Muons" 
TrackAssociator("HltSeedAssociator").TracksInContainer = "Hlt/Track/SeedTT"
TrackAssociator("HltTFAssociator").TracksInContainer = "Hlt/Track/TFForwardForTopo"
# TrackAssociator("HltTFAssociator").OutputLevel = 1
#
# temporary stuff to be fixed
#
from Gaudi.Configuration import *
from Configurables import P2MCPFromProtoP
ToolSvc().addTool(P2MCPFromProtoP)
ToolSvc().P2MCPFromProtoP.Locations += [ "Relations/HltTF/ProtoP/Charged" ]
#ToolSvc().P2MCPFromProtoP.OutputLevel = 1

from Configurables import ChargedPP2MC
ChargedPP2MC().InputData += [ "HltTF/ProtoP/Charged" ]
#ChargedPP2MC().OutputLevel = 1
