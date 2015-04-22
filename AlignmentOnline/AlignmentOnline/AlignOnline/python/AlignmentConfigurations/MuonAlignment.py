def configureAlignment():

    from Configurables import Escher, TAlignment

    Escher().HltFilterCode = "HLT_PASS( 'Hlt1DiMuonHighMassDecision' )"
    from TAlignment.TrackSelections import BestMuonTracks
    TAlignment().TrackSelections = [ BestMuonTracks('OnlineMuonAlignment') ]    

    from TAlignment.AlignmentScenarios import configureMuonAlignment
    configureMuonAlignment()
