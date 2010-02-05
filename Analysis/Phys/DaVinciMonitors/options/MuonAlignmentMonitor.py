from Gaudi.Configuration import *
from Configurables import MuonAlignmentMonitor, TrackKalmanFilter, MeasurementProvider, DaVinci

from TrackSys.Configuration import TrackSys
from TrackFitter.ConfiguredFitters import ConfiguredEventFitter 

muonTrackFit = ConfiguredEventFitter( 'MuonTrackFitter', 'Rec/Track/Muon')
muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
#muonTrackFit.Fitter.MeasProvider.MuonProvider.clusterize = True   # compatible only with REC_v8r0
#muonTrackFit.Fitter.MeasProvider.MuonProvider.OutputLevel = 4 # compatible only with REC_v8r0
muonTrackFit.Fitter.MeasProvider.IgnoreVelo = True 
muonTrackFit.Fitter.MeasProvider.IgnoreTT   = True 
muonTrackFit.Fitter.MeasProvider.IgnoreIT   = True 
muonTrackFit.Fitter.MeasProvider.IgnoreOT   = True 
muonTrackFit.Fitter.MeasProvider.IgnoreMuon = False 
muonTrackFit.Fitter.NodeFitter.BiDirectionalFit = False
muonTrackFit.Fitter.ErrorX  = 1000
muonTrackFit.Fitter.ErrorY  = 1000
muonTrackFit.Fitter.ErrorTx = 0.7
muonTrackFit.Fitter.ErrorTy = 0.7
muonTrackFit.Fitter.NumberFitIterations = 4
muonTrackFit.Fitter.MaxNumberOutliers   = 2
muonTrackFit.OutputLevel = 4

monalig = MuonAlignmentMonitor( "MuonAlignmentMonitor",
                                 HistoTopDir = ""
                                 )
monalig.OutputLevel = 4
monalig.IsLongTrackState = "true"
monalig.LongToMuonMatch = "true"
monalig.pCut = 0
monalig.IsCosmics = "false"


DaVinci().MoniSequence += [ muonTrackFit, monalig ]

