
from Configurables import ( Rich__Rec__ForwardTrackSelector,
                            Rich__Rec__MatchTrackSelector,
                            Rich__Rec__VeloTTTrackSelector,
                            Rich__Rec__SeedTrackSelector,
                            Rich__Rec__KsTrackSelector,
                            Rich__Rec__BaseTrackSelector )

# Method to just set the cuts in a track selector object
def configureTrackSelectorCuts(selector,cuts):
    selector.TrackAlgs = sorted(cuts.keys())
    for type in sorted(cuts.keys()) :
        if type == "Forward" : selector.addTool( Rich__Rec__ForwardTrackSelector, type )
        if type == "Match"   : selector.addTool( Rich__Rec__MatchTrackSelector,   type )
        if type == "VeloTT"  : selector.addTool( Rich__Rec__VeloTTTrackSelector,  type )
        if type == "Seed"    : selector.addTool( Rich__Rec__SeedTrackSelector,    type )
        if type == "KsTrack" : selector.addTool( Rich__Rec__KsTrackSelector,      type )
        if type == "Velo" or type == "Trigger" or type == "MCRichTrack" :
            selector.addTool( BaseTrackSelector, type )
        ts = getattr(selector,type)
        for name,cut in cuts[type].iteritems() :
            ts.setProp("Min"+name,cut[0])
            ts.setProp("Max"+name,cut[1])
