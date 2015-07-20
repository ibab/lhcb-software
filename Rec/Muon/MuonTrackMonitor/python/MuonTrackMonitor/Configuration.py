
from Gaudi.Configuration import *
from TrackFitter.ConfiguredFitters import ( ConfiguredEventFitter )
from TrackSys.Configuration import TrackSys
from Configurables import ( LHCbConfigurableUser, GaudiSequencer, TrackKalmanFilter, MeasurementProvider,
                            MuonTrackAligMonitor )

class MuonTrackMonitorConf(LHCbConfigurableUser):

## Properties -> "name": default value
    __slots__ = {
        "Histograms"    : "OfflineFull"
        ,"DataType"     : "2010"
        ,"OutputLevel"  : INFO    
        ,"Sequencer"    : GaudiSequencer("MoniMUONSeq")
        }

## List of ConfigurableUser manipulated
##    __used_configurables__ = [ OtherConf ]

    def applyConf(self):

        MSRossiAndGreisen = False
        # For 2015, we want the new multiple scattering
        if( self.getProp("DataType") == "2015" ):
            MSRossiAndGreisen = True


        muonTrackFit = ConfiguredEventFitter( 'MuonTrackFitter', 'Rec/Track/Muon', MSRossiAndGreisen = MSRossiAndGreisen)
        muonTrackFit.Fitter.addTool( TrackKalmanFilter , 'NodeFitter' )
        muonTrackFit.Fitter.addTool( MeasurementProvider, name = 'MeasProvider')
        muonTrackFit.Fitter.MeasProvider.IgnoreVelo = True 
        muonTrackFit.Fitter.MeasProvider.IgnoreTT   = True 
        muonTrackFit.Fitter.MeasProvider.IgnoreIT   = True 
        muonTrackFit.Fitter.MeasProvider.IgnoreOT   = True 
        muonTrackFit.Fitter.MeasProvider.IgnoreMuon = False 
        muonTrackFit.Fitter.MeasProvider.MuonProvider.clusterize    = True #=======
        #muonTrackFit.Fitter.MeasProvider.MuonProvider.OutputLevel    = DEBUG #=======
        muonTrackFit.Fitter.ErrorX  = 1000
        muonTrackFit.Fitter.ErrorY  = 1000
        muonTrackFit.Fitter.ErrorTx = 0.7
        muonTrackFit.Fitter.ErrorTy = 0.7
        muonTrackFit.Fitter.NumberFitIterations = 4
        muonTrackFit.Fitter.MaxNumberOutliers   = 0 #2
        muonTrackFit.OutputLevel = self.getProp("OutputLevel")


        
        muonMoniSeq = self.getProp("Sequencer")

        monalig = MuonTrackAligMonitor( "MuonTrackAligMonitor",
                                        HistoTopDir = "Muon/",
                                        HistoLevel = self.getProp("Histograms")
                                        )
        monalig.OutputLevel = self.getProp("OutputLevel")
        #monalig.IsLongTrackState = True
        monalig.LongToMuonMatch = True
        monalig.pCut = 6  # =========
        monalig.chi2nCut = 3
        monalig.chi2matchCut = 10
        monalig.IsCosmics = False

        # For 2015, we want the new multiple scattering
        if( self.getProp("DataType") == "2015" ):
            from Configurables import TrackMasterExtrapolator, DetailedMaterialLocator, StateThickMSCorrectionTool
            monalig.addTool(TrackMasterExtrapolator, name = "Extrapolator")
            monalig.Extrapolator.addTool(DetailedMaterialLocator, name="MaterialLocator")
            monalig.Extrapolator.MaterialLocator.addTool( StateThickMSCorrectionTool,  "StateMSCorrectionTool")
            monalig.Extrapolator.MaterialLocator.StateMSCorrectionTool.UseRossiAndGreisen = True
            
        muonMoniSeq.Members += [muonTrackFit, monalig]
        
