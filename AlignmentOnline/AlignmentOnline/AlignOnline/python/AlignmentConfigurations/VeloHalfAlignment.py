def configureAlignment():
    from Configurables import TAlignment
    from TAlignment.TrackSelections import GoodLongTracks, VeloOverlapTracks, VeloBackwardTracks, GoodLongRefittedVeloTracks
    from TAlignment.VertexSelections import configuredPVSelection
    # specify what we actually align for
    TAlignment().TrackSelections = [
        GoodLongRefittedVeloTracks('TrackRefitSeq'),
        VeloOverlapTracks(),
        VeloBackwardTracks()]
    
    # add the default PV selection
    TAlignment().PVSelection = configuredPVSelection()

    from TAlignment.AlignmentScenarios import configureVeloHalfAlignment
    configureVeloHalfAlignment()
