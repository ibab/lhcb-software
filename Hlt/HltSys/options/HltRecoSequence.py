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
recoRZPVSequence = GaudiSequencer( 'HltRecoRZPVSequence' , MeasureTime = True
                                 , Members = 
                                 [ PatPV2D( 'HltRecoPV2D'
                                          , InputTracksName = "Hlt/Track/RZVelo"
                                          , OutputVerticesName = "Hlt/Vertex/PV2D" ) ] )

recoRZVeloTracksSequence = GaudiSequencer( 'HltRecoRZVeloTracksSequence', MeasureTime = True
                                         , Members = 
                                         [ DecodeVeloRawBuffer()
                                         , Tf__PatVeloRTracking('HltRecoRZVelo' , OutputTracksName = "Hlt/Track/RZVelo" ) ] )

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
                                   , InputTracksName = "Hlt/Track/RZVelo"
                                   , OutputTracksName = "Hlt/Track/Velo" )

prepareVelo = HltTrackFilter('HltPrepareVelo'
                            , InputSelection = "TES:Hlt/Track/Velo"
                            , OutputSelection = "Velo1"
                            , RequirePositiveInputs = False )

recoForward = PatForward( 'HltRecoForward'
                        , InputTracksName = "Hlt/Track/Velo"
                        , OutputTracksName = "Hlt/Track/Forward" )

prepareForward = HltTrackFilter( 'HltPrepareForward' 
                               , InputSelection = "TES:Hlt/Track/Forward"
                               , OutputSelection = "Forward1"
                               , RequirePositiveInputs = False )

recoPV3D =  PatPV3D('Hlt1RecoPV3D' ) # FIXME: check InputTracksName = "Hlt/Track/Velo" )

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
                              , InputSelection   = "TES:Hlt/Track/RZVelo"
                              , RequirePositiveInputs = False
                              , AddInfo = False
                              , FilterDescriptor = ["IsBackward,<,0.5"]
                              , OutputSelection     = "RZVelo" )

preparePV2D = HltVertexFilter( 'Hlt1PreparePV2D'
                             , InputSelection = "TES:Hlt/Vertex/PV2D"
                             , RequirePositiveInputs = False
                             , OutputSelection   = "PV2D" )


hlt1RecoRZVeloTracksSequence = GaudiSequencer( 'Hlt1RecoRZVeloTracksSequence' , MeasureTime = True
                                             , Members = [ recoRZVeloTracksSequence , prepareRZVelo ] )

hlt1RecoRZPVSequence = GaudiSequencer( 'Hlt1RecoRZPVSequence', MeasureTime = True
                                     , Members = [ recoRZPVSequence , preparePV2D ] )

recoRZVelo = GaudiSequencer( 'Hlt1RecoRZVeloSequence' , MeasureTime = True
                           , Members = 
                           [ recoRZVeloSequence , prepareRZVelo, preparePV2D ] ) 

recoVelo = HltTrackUpgrade( 'Hlt1RecoVelo'
                          , InputSelection = "RZVelo"
                          , OutputSelection = "Velo"
                          , RecoName = "Velo"
                          , HistogramUpdatePeriod = 0 )

recoFwd = HltTrackUpgrade( 'Hlt1RecoForward'
                         , InputSelection = "Velo"
                         , OutputSelection = "Forward"
                         , RecoName = "Forward"
                         , HistogramUpdatePeriod = 0 )

hlt1RecoSequence = GaudiSequencer( 'Hlt1RecoSequence', MeasureTime = True
                                 , Members = 
                                 [ hlt1RecoRZVeloTracksSequence
                                 , hlt1RecoRZPVSequence
                                 , recoVelo
                                 , recoPV3D
                                 , decodeTT
                                 , decodeT
                                 , recoFwd ] )

# Forward
#----------------
PatForwardTool().MinMomentum = 1000.
PatForwardTool().MinPt = 1000.
PatForwardTool().AddTTClusterName = ""

#ToolSvc.PatForwardTool.MinMomentum = 1000. ;  
#ToolSvc.PatForwardTool.MinPt       = 80. ;   
#ToolSvc.PatForwardTool.AddTTClusterName = "";


recoSeq = GaudiSequencer('HltRecoSequence', MeasureTime = True
                        , Members =
                        [ trackRecoSequence
                        # //,GaudiSequencer('HltCaloRecoSequence')  // In HltChargedProtoPAlg for the time being
                        ] )

#// --------------------- HltCalosequence // In HltChargedProtoPAlg for the time being
#// Options for Calo reconstruction
importOptions( "$CALORECOROOT/options/HltCaloSeq.opts" )
GaudiSequencer('HltCaloRecoSequence', Members = ["GaudiSequencer/RecoCALOSeq" ] )

#/// PATCH /// @todo remove
#print 'Before patch: ' + str(HltRecoCALOSeq.Members)
#HltRecoCALOSeq.Members = [ i in HltRecoCALOSeq.Members if i is not "GaudiSequencer/HltCaloDigits" ] 
#print 'After  patch: ' + str(HltRecoCALOSeq.Members)
#/// @todo This cannot work, as tracking must have been done to process the rest.
#//recoSeq.IgnoreFilterPassed = True; // process both track and calo independently
