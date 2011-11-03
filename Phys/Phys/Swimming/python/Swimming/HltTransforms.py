def getTransform(name,triggerstorun) :
  if name == "2010" :
    return { 'GaudiSequencer/.*' : 
                                    { 'Members' : 
                                                    { "'PatPV3D/HltPVsPV3D' , 'HltVertexFilter/Hlt1PreparePV3D'" : 
                                                      "'PatPV3D/HltPVsPV3D', 'HltMoveVerticesForSwimming/HltMovePVs4Swimming'" 
                                                    } 
                                    } ,
             'DeterministicPrescaler/Hlt2ForwardPreScaler' : { 'AcceptFraction' : {'.*' : '1'}} ,
             'GaudiSequencer/Hlt1TrackPhotonFilterSequence' :
                                    { 'Members' :
                                                    { "'HltTrackFilter/Hlt1TrackPhotonTFOTIP' , 'HltTrackFilter/Hlt1TrackPhotonTFOTNH' , 'HltTrackFilter/Hlt1TrackPhotonTFOTEXH' , 'HltTrackUpgrade/Hlt1TrackPhotonTUForward' , 'HltTrackFilter/Hlt1TrackPhotonTFOTPT' , 'HltTrackFilter/Hlt1TrackPhotonTFOTMom' , 'HltTrackUpgrade/Hlt1TrackPhotonTUFitTrack' , 'HltTrackFilter/Hlt1TrackPhotonTFTrkChi2' , 'HltFilterFittedTracks/Hlt1TrackPhotonHltFilterFittedTracksFFT'" :
                                                      "'HltTrackUpgrade/Hlt1TrackPhotonTUForward' , 'HltTrackUpgrade/Hlt1TrackPhotonTUFitTrack'"
                                                    }
                                    } ,
             'HltTrackUpgrade/Hlt1TrackPhotonTUForward' : { 'InputSelection'  : { 'Hlt1TrackPhotonTFOTEXH' : 'Velo'}} ,
             'HltTrackUpgrade/Hlt1TrackPhotonTUFitTrack': { 'OutputSelection' : { 'Hlt1TrackPhotonTUFitTrack' : 'Hlt1TrackPhotonDecision' } ,
                                                            'InputSelection'  : { 'Hlt1TrackPhotonTFOTMom' : 'Hlt1TrackPhotonTUForward'} 
                                                          } ,
             'LoKi::L0Filter/Hlt1TrackPhotonL0DUFilter' : { 'Code' : {"L0_CHANNEL.*Photon.*" : "L0_DECISION_PHYSICS"} }
           }
  elif name == "2011_NoBeamSpotFilter" :
    PrescalerFiddlePrefix = 'DeterministicPrescaler/'
    PrescalerFiddleSuffix = '(?!Hlt1Global)(?!Hlt2Global)(?!Hlt1TrackPhoton).*PreScaler'
    PrescalerFiddle = "".join(['(?!%s)'%trigger for trigger in triggerstorun]) 
    return {PrescalerFiddlePrefix+PrescalerFiddle+PrescalerFiddleSuffix : { 'AcceptFraction' : {'.*' : '0'}},
            'DeterministicPrescaler/Hlt2ForwardPreScaler' : { 'AcceptFraction' : {'.*' : '1'}} ,
            'LoKi::L0Filter/Hlt1TrackPhotonL0DUFilter' : { 'Code' : {"L0_CHANNEL('PhotonHi')|L0_CHANNEL('ElectronHi')" : "L0_DECISION_PHYSICS"} },
            '.*HltUnit/Hlt1TrackPhotonUnit' : {'Code' : {"Velo.*EMPTY":"VeloCandidates>>LooseForward>>FitTrack>>SINK( 'Hlt1TrackPhotonDecision' )>>~TC_EMPTY"}},
            'GaudiSequencer/.*' : {'Members':{"'HltCopySelection<LHCb::RecVertex>/Hlt1PreparePV3D'":
                                              "'HltMoveVerticesForSwimming/HltMovePVs4Swimming'"}},
            '.*HltUnit/.*': {'Preambulo' : {'HltCopySelection<LHCb::RecVertex>/Hlt1PreparePV3D':
                                            'HltMoveVerticesForSwimming/HltMovePVs4Swimming'}}}
  elif name == "2011_NoBeamSpotFilter_NoRecoLines" :
    PrescalerFiddlePrefix = 'DeterministicPrescaler/'
    PrescalerFiddleSuffix = '(?!Hlt1Global)(?!Hlt2Global).*PreScaler'
    PrescalerFiddle = "".join(['(?!%s)'%trigger for trigger in triggerstorun]) 
    return {PrescalerFiddlePrefix+PrescalerFiddle+PrescalerFiddleSuffix : { 'AcceptFraction' : {'.*' : '0'}},
            'DeterministicPrescaler/Hlt2ForwardPreScaler' : { 'AcceptFraction' : {'.*' : '1'}} ,
            'GaudiSequencer/.*' : {'Members':{"'HltCopySelection<LHCb::RecVertex>/Hlt1PreparePV3D'":
                                              "'HltMoveVerticesForSwimming/HltMovePVs4Swimming'"}},
            '.*HltUnit/.*': {'Preambulo' : {'HltCopySelection<LHCb::RecVertex>/Hlt1PreparePV3D':
                                            'HltMoveVerticesForSwimming/HltMovePVs4Swimming'}}}
  elif name == "2011_WithBeamSpotFilter" :
    PrescalerFiddlePrefix = 'DeterministicPrescaler/'
    PrescalerFiddleSuffix = '(?!Hlt1Global)(?!Hlt2Global)(?!Hlt1TrackPhoton).*PreScaler'
    PrescalerFiddle = "".join(['(?!%s)'%trigger.split('Decision')[0] for trigger in triggerstorun])
    print PrescalerFiddlePrefix+PrescalerFiddle+PrescalerFiddleSuffix 
    return {PrescalerFiddlePrefix+PrescalerFiddle+PrescalerFiddleSuffix : { 'AcceptFraction' : {'.*' : '0'}},
            'DeterministicPrescaler/Hlt1TrackForwardPassThroughLoosePreScaler' : { 'AcceptFraction' : {'.*' : '1'}} ,
            'DeterministicPrescaler/Hlt2ForwardPreScaler' : { 'AcceptFraction' : {'.*' : '1'}} ,
            'LoKi::L0Filter/Hlt1TrackPhotonL0DUFilter' : { 'Code' : {"L0_CHANNEL('PhotonHi')|L0_CHANNEL('ElectronHi')" : "L0_DECISION_PHYSICS"} },
            '.*HltUnit/Hlt1TrackPhotonUnit' : {'Code' : {"Velo.*EMPTY":"VeloCandidates>>LooseForward>>FitTrack>>SINK( 'Hlt1TrackPhotonDecision' )>>~TC_EMPTY"}},
            '.*HltPV3D': {'Code' : {"'PV3D'":"'PV3D_PreSwim'"}},
            'GaudiSequencer/.*' : {'Members':{"HltPV3D'":"HltPV3D', 'HltMoveVerticesForSwimming/HltMovePVs4Swimming'"}}} 
  elif name == "2011_WithBeamSpotFilter_NoRecoLines" :
    PrescalerFiddlePrefix = 'DeterministicPrescaler/'
    PrescalerFiddleSuffix = '(?!Hlt1Global)(?!Hlt2Global).*PreScaler'
    PrescalerFiddle = "".join(['(?!%s)'%trigger.split('Decision')[0] for trigger in triggerstorun])
    print PrescalerFiddlePrefix+PrescalerFiddle+PrescalerFiddleSuffix
    return {PrescalerFiddlePrefix+PrescalerFiddle+PrescalerFiddleSuffix : { 'AcceptFraction' : {'.*' : '0'}},
            'DeterministicPrescaler/Hlt2ForwardPreScaler' : { 'AcceptFraction' : {'.*' : '1'}} ,
            '.*HltPV3D': {'Code' : {"'PV3D'":"'PV3D_PreSwim'"}},
            'GaudiSequencer/.*' : {'Members':{"HltPV3D'":"HltPV3D', 'HltMoveVerticesForSwimming/HltMovePVs4Swimming'"}}}
  else :
    return {}
