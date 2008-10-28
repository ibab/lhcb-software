#------------------------------
# Definition of the Hlt Reconstruction
#-----------------------------
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from Configurables import DecodeVeloRawBuffer
from Configurables import PatPV2D, PatPV3D, PatForward, PatForwardTool
from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking
from Configurables import RawBankToSTLiteClusterAlg
from Configurables import HltTrackFilter, HltVertexFilter, HltTrackUpgrade

#----------------------- HltTrack sequence
# ---------------------
## make sure we continue, even if no PV is found
patVeloR = Tf__PatVeloRTracking('HltRecoRZVelo' , OutputTracksName = "Hlt/Track/RZVelo" ) 

recoRZVeloTracksSequence = GaudiSequencer( 'HltRecoRZVeloTracksSequence', MeasureTime = True
                                         , Members = [ DecodeVeloRawBuffer() , patVeloR ] 
                                         )

recoRZPVSequence = GaudiSequencer( 'HltRecoRZPVSequence' , MeasureTime = True, IgnoreFilterPassed = True
                                 , Members = 
                                 [ PatPV2D( 'HltRecoPV2D'
                                          , InputTracksName = patVeloR.OutputTracksName
                                          , OutputVerticesName = "Hlt/Vertex/PV2D" ) ] )

recoRZVeloSequence = GaudiSequencer ( 'HltRecoRZVeloSequence', MeasureTime = True
                                    , Members = 
                                    [  recoRZVeloTracksSequence
                                    ,  recoRZPVSequence ] )

decodeTT = GaudiSequencer( 'HltDecodeTT', MeasureTime = True 
                         , Members = 
                         [ RawBankToSTLiteClusterAlg('createTTLiteClusters') ] )

decodeT = GaudiSequencer( 'HltDecodeT', MeasureTime = True
                        , Members = 
                        [ RawBankToSTLiteClusterAlg('createITLiteClusters', detType = 'IT') ] )

recoVelo = Tf__PatVeloSpaceTracking('HltRecoVelo'
                                   , InputTracksName = patVeloR.OutputTracksName
                                   , OutputTracksName = "Hlt/Track/Velo" )

prepareVelo = HltTrackFilter('HltPrepareVelo'
                            , InputSelection = "TES:" + recoVelo.OutputTracksName
                            , OutputSelection = "Velo1"
                            , RequirePositiveInputs = False )

recoForward = PatForward( 'HltRecoForward'
                        , InputTracksName = recoVelo.OutputTracksName
                        , OutputTracksName = "Hlt/Track/Forward" )

prepareForward = HltTrackFilter( 'HltPrepareForward' 
                               , InputSelection = "TES:" + recoForward.OutputTracksName
                               , OutputSelection = "Forward1"
                               , RequirePositiveInputs = False )

recoPV3D =  PatPV3D('Hlt1RecoPV3D' ) #TODO: check IPVOfflineTool config!!! InputTracksName = recoVelo.OutputTracksName )
#Hlt1RecoPV3D.PVOfflineTool.InputTracks = [ recoVelo.OutputTracksName ]


#TODO: could this be a 'boundMembers' instance? 
trackRecoSequence = GaudiSequencer( 'HltTrackRecoSequence'
                                  ,  Members =
                                  [  recoRZVeloSequence
                                  ,  recoVelo
                                  ,  prepareVelo
                                  #,  recoPV3D
                                  ,  decodeTT
                                  ,  decodeT
                                  ,  recoForward
                                  ,  prepareForward
                                  ] )

#------------------------------
# Definition of the Hlt1 Reconstruction
#-----------------------------

prepareRZVelo = HltTrackFilter( 'Hlt1PrepareRZVelo'
                              , InputSelection   = "TES:" + patVeloR.OutputTracksName
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )

preparePV2D = HltVertexFilter( 'Hlt1PreparePV2D'
                             , InputSelection = "TES:" + PatPV2D('HltRecoPV2D').OutputVerticesName
                             , RequirePositiveInputs = False
                             , OutputSelection   = "PV2D" )


hlt1RecoRZVeloTracksSequence = GaudiSequencer( 'Hlt1RecoRZVeloTracksSequence' , MeasureTime = True
                                             , Members = [ recoRZVeloTracksSequence , prepareRZVelo ] )

hlt1RecoRZPVSequence = GaudiSequencer( 'Hlt1RecoRZPVSequence', MeasureTime = True
                                     , Members = [ recoRZPVSequence , preparePV2D ] )

#TODO: could this be a 'boundMembers' instance? That would buy us automated OutputSelection
#      recognition... 
# (can we make 'boundMembers' instances singletons, like configurables?)
# (and make them check that once 'OutputSelection' is picked up, they become 'immutable')
recoRZVelo = GaudiSequencer( 'Hlt1RecoRZVeloSequence' , MeasureTime = True
                           , Members = 
                           [ recoRZVeloSequence , prepareRZVelo, preparePV2D ] ) 

reco1Velo = HltTrackUpgrade( 'Hlt1RecoVelo'
                          , InputSelection = prepareRZVelo.OutputSelection
                          , OutputSelection = "Velo"
                          , RecoName = "Velo"
                          , HistogramUpdatePeriod = 0 )

recoFwd = HltTrackUpgrade( 'Hlt1RecoForward'
                         , InputSelection = reco1Velo.OutputSelection
                         , OutputSelection = "Forward"
                         , RecoName = "Forward"
                         , HistogramUpdatePeriod = 0 )

hlt1RecoSequence = GaudiSequencer( 'Hlt1RecoSequence', MeasureTime = True
                                 , Members = 
                                 [ hlt1RecoRZVeloTracksSequence
                                 , hlt1RecoRZPVSequence
                                 , reco1Velo
                                 , recoPV3D # does this not abort the remainder of the sequence if no primary??
                                 , decodeTT
                                 , decodeT
                                 , recoFwd ] )

# Forward
#----------------
PatForwardTool( MinMomentum = 1000.
              , MinPt = 1000.
              , AddTTClusterName = "" )

recoSeq = GaudiSequencer('HltRecoSequence', MeasureTime = True
                        , Members =
                        [ trackRecoSequence
                        # //,GaudiSequencer('HltCaloRecoSequence')  // In HltChargedProtoPAlg for the time being
                        ] )

#// --------------------- HltCalosequence // In HltChargedProtoPAlg for the time being
#// Options for Calo reconstruction
importOptions( "$CALORECOROOT/options/HltCaloSeq.opts" )
GaudiSequencer('HltCaloRecoSequence', Members = [ GaudiSequencer('RecoCALOSeq') ] )

#/// @todo This cannot work, as tracking must have been done to process the rest.
#//recoSeq.IgnoreFilterPassed = True; // process both track and calo independently
