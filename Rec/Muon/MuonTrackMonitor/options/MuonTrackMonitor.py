from TrackSys.Configuration import TrackSys
# from Configurables import ( TrackEventFitter, TrackMasterFitter, TrackKalmanFilter,
#                             TrackProjectorSelector, TrajOTProjector, TrackMasterExtrapolator,
#                             TrackSimpleExtraSelector, TrackDistanceExtraSelector,
#                             TrackHerabExtrapolator,
#                             SimplifiedMaterialLocator, DetailedMaterialLocator,
#                             MeasurementProvider)

from TrackFitter.ConfiguredFitters import ( ConfiguredEventFitter )
from Configurables import ( TrackKalmanFilter, MeasurementProvider )

muonTrackFit = ConfiguredEventFitter( 'MuonTrackFitter', 'Rec/Track/Muon')
muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
muonTrackFit.Fitter.ZPositions = [12100]
muonTrackFit.Fitter.MeasProvider.IgnoreVelo = True 
muonTrackFit.Fitter.MeasProvider.IgnoreTT   = True 
muonTrackFit.Fitter.MeasProvider.IgnoreIT   = True 
muonTrackFit.Fitter.MeasProvider.IgnoreOT   = True 
muonTrackFit.Fitter.MeasProvider.IgnoreMuon = False 
muonTrackFit.Fitter.NodeFitter.BiDirectionalFit = False
muonTrackFit.Fitter.ErrorX2  = 1000000
muonTrackFit.Fitter.ErrorY2  = 1000000
muonTrackFit.Fitter.ErrorTx2 = 0.5
muonTrackFit.Fitter.ErrorTy2 = 0.5
muonTrackFit.Fitter.NumberFitIterations = 4
muonTrackFit.Fitter.MaxNumberOutliers   = 2
muonTrackFit.OutputLevel = 4



from Configurables import GaudiSequencer,MuonTrackMonitor,MuonEfficiencyMonitor,MuonAlignmentMonitor

muonMoniSeq = GaudiSequencer("MoniMUONSeq")

montracks = MuonTrackMonitor( "MuonTrackMonitor",
                              HistoTopDir = "Muon/"
                              )
muonMoniSeq.Members += [montracks]

moneffi = MuonEfficiencyMonitor( "MuonEfficiencyMonitor",
                                 HistoTopDir = "Muon/"
                                 )
#muonMoniSeq.Members += [moneffi]

monalig = MuonAlignmentMonitor( "MuonAlignmentMonitor",
                                 HistoTopDir = "Muon/"
                                 )
monalig.OutputLevel = 4
muonMoniSeq.Members += [monalig]
monalig.IsLongTrackState = "true"
monalig.LongToMuonMatch = "true"
monalig.pCut = 0
monalig.IsCosmics = "true"

# muonMoniSeq.Members = [muonTrackFit,  montracks, moneffi, monalig ]  
muonMoniSeq.Members = [muonTrackFit, montracks, monalig]  
 
