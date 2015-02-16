def configureAlignment():
    from Configurables import TAlignment
    from TAlignment.TrackSelections import TEDVeloTracks
    from TAlignment.VertexSelections import configuredPVSelection

    TAlignment().TrackSelections = [TEDVeloTracks()]

    # add the default PV selection
    TAlignment().PVSelection = configuredPVSelection()

    # Configure the alignment
    from TAlignment.AlignmentScenarios import configureTEDVeloSensorAlignment
    configureTEDVeloSensorAlignment()
