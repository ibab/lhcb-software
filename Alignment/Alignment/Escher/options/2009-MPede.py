# Configuration example using private databases for a millepede style alignment of the OT
# using 2009 collision data.
#
# example of particular conditions, and magnetic field polarity inversion,
# uncomment to use.
#from Configurables import ( CondDB, CondDBAccessSvc )
#otCalib = CondDBAccessSvc( 'OTCalib' )
#otCalib.ConnectionString ='sqlite_file:/data/work/DataBases/Collision09_OT_QuarterT0s_071209.db/LHCBCOND'
#CondDB().addLayer( otCalib )
#
#from Configurables import UpdateManagerSvc
#UpdateManagerSvc().ConditionsOverride += ["Conditions/Online/LHCb/Magnet/Measured := double Current = 5849.9936523438 ; int Polarity = -1;"]
#
#aligndb = '/data/work/DataBases/TTLayerITLayerOTModules_CollisionsDecember2009.db'
#from Configurables import ( CondDB, CondDBAccessSvc )
#alignCond = CondDBAccessSvc( 'AlignCond' )
#alignCond.ConnectionString = 'sqlite_file:' + aligndb + '/LHCBCOND'
#CondDB().addLayer( alignCond )

from Configurables import ( TrackSys, GaudiSequencer, Escher, TAlignment, TStation, ATrackSelector, TrackMonitor, OTTrackMonitor )


TrackSys.TrackPatRecAlgorithms = ["PatSeed"]
GaudiSequencer("RecoRICHSeq").Enable = False
GaudiSequencer("RecoVELOSeq").Enable = False
GaudiSequencer("RecoTTSeq").Enable = False
GaudiSequencer("RecoITSeq").Enable = True

TAlignment().WriteCondSubDetList = ["OT"]
Escher().EvtMax = 1000
Escher().PrintFreq = 1000
Escher().AlignmentLevel = "layers"
Escher().Millepede = True
Escher().Kalman = False
Escher().Incident = "GlobalMPedeFit"

ATrackSelector().UniformCutOff = 4
#ATrackSelector().UniformCutOff = 9
ATrackSelector().MinEnergyCut = 0.0
#ATrackSelector().MinEnergyCut = 0.0
ATrackSelector().MinPCut = 0.0
ATrackSelector().MinPtCut = 0.0
ATrackSelector().MinChi2Cut = 0.0
ATrackSelector().MaxChi2Cut = 15.0
ATrackSelector().Charge = 0
ATrackSelector().MaxPCut = -1
ATrackSelector().MaxPtCut = -1
ATrackSelector().MinITHitCut    = 0
ATrackSelector().MinOTHitCut = 15
ATrackSelector().MinTTHitCut = 0
#ATrackSelector().YCutMin_at_T1 = -2500.0
#ATrackSelector().YCutMax_at_T1 = -830.0
ATrackSelector().OutputLevel = 1

from Configurables import TStation
TStation().OutputLevel = 3
TStation().InputContainer = "Rec/Track/Seed"
#TStation().InputContainer = "Rec/Track/Best"

TrackSys().ExpertTracking += ["noDrifttimes"]

from Configurables import (CountingPrescaler, ProcessPhase )
Escher().MainSequence = [ CountingPrescaler("EscherPrescaler")
                        , "ProcessPhase/Init"
                        , "ProcessPhase/Reco"
                        , GaudiSequencer("AlignSequence")
                        , "ProcessPhase/Moni" ]
OTTrackMonitor().TrackLocation = "Event/Alignment/OTTracks"
TrackMonitor().TracksInContainer = "Event/Alignment/OTTracks"


