import PyCintex
_global   = PyCintex.makeNamespace('')
cpp      = _global
LHCb     = cpp.LHCb

def MatchCallback(key) :
  settings = { 'VeloCalo' :   { "Tool" : "HltVeloTCaloMatch"
                              , "RecoID" :  100 # TODO
                              , "TransferIDs":True
                              , "TransferAncestor":True
                              , "TransferInfo":False
                              , "TrackType":  LHCb.Track.Velo
                              , "TESOutput": "Hlt1/Track/VeloCalo"
                              , "Quality":"Chi2Y"
                              , "Quality2":"Chi2X"
                              }
             , 'VeloTDist' :  { "Tool":"HltMatchTVeloTracks"
                              , "RecoID":  101 # //TODO
                              , "TransferIDs":True
                              , "TransferAncestor":True
                              , "TransferInfo":False
                              , "TrackType": LHCb.Track.Long
                              , "TESOutput": "Hlt1/Track/VeloTDist"
                              , "Quality":"deltaX"
                              , "Quality2":"deltaY"
                              }
             , 'VeloT' :      { "Tool":"PatMatchTool"
                              , "RecoID":  102 #//TODO
                              , "TransferIDs":True
                              , "TransferAncestor":True
                              , "TransferInfo":True
                              , "TrackType":  LHCb.Track.Long
                              , "TESOutput": "Hlt1/Track/VeloT" # //TODO
                              , "Quality":"chi2_PatMatch"
                              , "Quality2":""
                              }

             }

  def callback(configurable) :
        for k,v in settings[key].iteritems() : setattr(configurable,k,v)

  return callback
