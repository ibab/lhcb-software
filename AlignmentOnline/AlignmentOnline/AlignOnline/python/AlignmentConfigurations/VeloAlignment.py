def configureAlignment():
    from Configurables import TAlignment
    from TAlignment.TrackSelections import GoodLongTracks, VeloOverlapTracks, VeloBackwardTracks
    from TAlignment.VertexSelections import configuredLoosePVSelection
    # specify what we actually align for
    TAlignment().TrackSelections = [GoodLongTracks(),
                                    VeloOverlapTracks(),
                                    VeloBackwardTracks()]

    # add the default PV selection
    TAlignment().PVSelection = configuredLoosePVSelection()

    from TAlignment.AlignmentScenarios import configureVeloAlignment
    configureVeloAlignment()
