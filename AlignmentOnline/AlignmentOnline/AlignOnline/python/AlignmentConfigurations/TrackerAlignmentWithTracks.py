def configureAlignment():
    from Configurables import TAlignment
    from TAlignment.TrackSelections import GoodLongTracks, VeloOverlapTracks, VeloBackwardTracks
    from TAlignment.VertexSelections import configuredPVSelection
    # specify what we actually align for
    TAlignment().TrackSelections = [GoodLongTracks(),
                                    VeloOverlapTracks(),
                                    VeloBackwardTracks()]

    # add the default PV selection
    TAlignment().PVSelection = configuredPVSelection()

    from TAlignment.AlignmentScenarios import configureTrackerAlignment
    configureTrackerAlignment(True)
