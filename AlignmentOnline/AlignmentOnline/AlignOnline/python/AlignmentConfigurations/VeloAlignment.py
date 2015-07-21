def configureAlignment():
    from Configurables import TAlignment
    from TAlignment.TrackSelections import GoodLongTracks, VeloOverlapTracks, VeloBackwardTracks, GoodLongRefittedVeloTracks
    from TAlignment.VertexSelections import configuredLoosePVSelection, configuredPVSelection
    # specify what we actually align for
    TAlignment().TrackSelections = [GoodLongRefittedVeloTracks('TrackRefitSeq'),
                                    VeloOverlapTracks(),
                                    VeloBackwardTracks()]

    # add the default PV selection
    TAlignment().PVSelection = configuredPVSelection()

    from TAlignment.AlignmentScenarios import configureVeloAlignment
    configureVeloAlignment()

    # Prescale Beam Beam interactions
    def prescaleBB(BBfraction = 1):
        from Configurables import LoKi__ODINFilter  as ODINFilter
        from Configurables import DeterministicPrescaler, GaudiSequencer
        odinFiltNonBB  = ODINFilter ('ODINBXTypeFilterNonBB', Code = 'ODIN_BXTYP < 3') 
        odinFiltBB  = ODINFilter ('ODINBXTypeFilterBB', Code = 'ODIN_BXTYP == 3')
        prescBB = DeterministicPrescaler("DetPrescBB", AcceptFraction = BBfraction)
        prescBB_seq  = GaudiSequencer("PrescBB")
        prescBB_seq.Members = [odinFiltBB, prescBB]
        collTypeSeq = GaudiSequencer('CollTypeSelector', ModeOR = True)
        collTypeSeq.Members = [odinFiltNonBB, prescBB_seq]
        GaudiSequencer('HltFilterSeq').Members.append( collTypeSeq )
        
    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction( prescaleBB )

