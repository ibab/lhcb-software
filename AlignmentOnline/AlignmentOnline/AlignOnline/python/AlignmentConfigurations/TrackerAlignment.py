def configureAlignment():
    from Configurables import Escher, TAlignment
    Escher().HltFilterCode = "HLT_PASS_RE( 'Hlt1CalibTrackingDecision' )"
    from TAlignment.ParticleSelections import defaultHLTD0Selection
    TAlignment().ParticleSelections = [ defaultHLTD0Selection() ]
    from TAlignment.TrackSelections import NoPIDTracksFromHlt
    TAlignment().TrackSelections = [ NoPIDTracksFromHlt() ]

    from TAlignment.AlignmentScenarios import configure2012DataAlignment
    configure2012DataAlignment()
