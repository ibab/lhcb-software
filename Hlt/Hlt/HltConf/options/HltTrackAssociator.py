#/** @file HltTrackAssociator.opts
# *  
# *  Creation of table for TrackAssociator
# *
# *  @author P. Koppenburg
# *  @date 2008-02-18
# */
#//--------------------------------------------------------------

from Configurables import DataOnDemandSvc, TrackAssociator

DataOnDemandSvc().AlgMap.update(  { 'Link/Hlt/Track/Forward': 'TrackAssociator/HltTrackAssociator'
                                  , 'Link/Hlt/Track/Muons':   'TrackAssociator/HltMuonAssociator'
                                  , 'Link/Hlt/Track/SeedTT':  'TrackAssociator/HltSeedAssociator'
                                  } )

TrackAssociator("HltTrackAssociator").TracksInContainer = "Hlt/Track/Forward" 
TrackAssociator("HltMuonAssociator").TracksInContainer = "Hlt/Track/Muons" 
TrackAssociator("HltSeedAssociator").TracksInContainer = "Hlt/Track/SeedTT"
