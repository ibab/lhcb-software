##################################################################
# Helper function to create a sequence to fit the tracks and run the hitadder
##################################################################
def configuredFitAndHitAdderSequence( Name, InputLocation, OutputLocation):
    from TrackFitter.ConfiguredFitters import ConfiguredEventFitter
    from Configurables import (TrackHitAdder, TrackContainerCopy, 
                               TrackSelector, GaudiSequencer,
                               TrackStateInitAlg)
    # create the sequence
    if isinstance(Name,GaudiSequencer) :
        seq = Sequence
        Name = seq.name()
        Name.replace( 'Sequence','')
        Name.replace( 'Seq','')
    else:
        seq = GaudiSequencer(Name + 'Seq')

    # I am lazy: use the DOD to get the decoded clusters
    #importOption( "$STDOPTS/DecodeRawEvent.py" )
    # now setup the fitters
    seq.Members += [ 
        TrackStateInitAlg(Name + 'FitInit', TrackLocation = InputLocation),
        ConfiguredEventFitter(Name + 'FitBeforeHitAdder',TracksInContainer = InputLocation),
        TrackHitAdder( Name + 'HitAdder', TrackLocation = InputLocation ),
        ConfiguredEventFitter(Name + 'FitAfterHitAdder',TracksInContainer = InputLocation)]
    tracksel =  TrackContainerCopy(Name + 'CopyAndSelect',
                                   inputLocation = InputLocation,
                                   outputLocation = OutputLocation,
                                   Selector = TrackSelector())
    # also apply a selection
    tracksel.Selector.MaxChi2Cut = 5
    tracksel.Selector.MaxChi2PerDoFMatch = 5
    tracksel.Selector.MaxChi2PerDoFVelo = 5
    tracksel.Selector.MaxChi2PerDoFDownstream = 5
    seq.Members.append( tracksel )
    return seq
