
def configuredCosmicsFitSequence( Name, TrackLocation,
                                  UseDriftTimes = True) :
    
    from Configurables import (GaudiSequencer)
    sequence = GaudiSequencer(Name)
    
    #from TrackSys.Configuration import TrackSys
    #TrackSys().fieldOff = True
    #TrackSys().expertTracking.append('simplifiedGeometry')
    #TrackSys().expertTracking += ['noDrifttimes' ]

    # fit the track with straight line fit
    from TrackFitter.ConfiguredFitters import (ConfiguredStraightLineFit)
    from Configurables import (TrackKalmanFilter,
                               TrajOTCosmicsProjector,TrajOTProjector,
                               TrackProjectorSelector,MeasurementProvider)
    
    trackprefit = ConfiguredStraightLineFit(Name + 'PreFit',TrackLocation,NoDriftTimes=True)
    #defaultOTNoDriftTimeProjector = TrajOTProjector("OTNoDrifttimesProjector")
    #defaultOTNoDriftTimeProjector.UseDrift = False
    trackprefit.Fitter.addTool( TrackKalmanFilter(), name = "NodeFitter" )
    #trackprefit.Fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
    #trackprefit.Fitter.NodeFitter.Projector.OT = "TrajOTProjector/" + defaultOTNoDriftTimeProjector.name()
    trackprefit.Fitter.NodeFitter.BiDirectionalFit = False
    trackprefit.Fitter.ErrorX2 = 10000
    trackprefit.Fitter.ErrorY2 = 10000
    trackprefit.Fitter.ErrorTx2 = 0.01
    trackprefit.Fitter.ErrorTy2 = 0.01
    trackprefit.Fitter.NumberFitIterations = 3
    trackprefit.Fitter.MaxNumberOutliers = 2
    trackprefit.Fitter.addTool( MeasurementProvider(), name = 'MeasProvider')
    trackprefit.Fitter.MeasProvider.IgnoreIT = True ;
    trackprefit.Fitter.MakeNodes = True ;
    sequence.Members.append( trackprefit )

    # remove odd clusters, load a calibration, initialize the per-event t0
    from Configurables import (OTCalibrationIO,TrackRemoveOddOTClusters,TrackSeedT0Alg)
    sequence.Members += [ TrackRemoveOddOTClusters(Name + 'RemoveOddOTClusters',TrackLocation = TrackLocation),
                          OTCalibrationIO(),
                          TrackSeedT0Alg(Name + 'TrackSeedT0',TrackLocation = TrackLocation) ]

    if UseDriftTimes :
        # refit the tracks with drift times
        trackfit = ConfiguredStraightLineFit(Name + 'DriftTimeFit',TrackLocation,NoDriftTimes=False)
        cosmicsOTProjector = TrajOTCosmicsProjector('OTCosmicsProjector')
        cosmicsOTProjector.UseDrift = True
        cosmicsOTProjector.FitEventT0 = True
        cosmicsOTProjector.UseConstantDriftVelocity = True
        trackfit.Fitter.addTool( TrackKalmanFilter(), name = "NodeFitter" )
        trackfit.Fitter.NodeFitter.addTool( TrackProjectorSelector(), "Projector" )
        trackfit.Fitter.NodeFitter.Projector.OT = cosmicsOTProjector
        trackfit.Fitter.ErrorX2 = 10000
        trackfit.Fitter.ErrorY2 = 10000
        trackfit.Fitter.ErrorTx2 = 0.01
        trackfit.Fitter.ErrorTy2 = 0.01
        trackfit.Fitter.ErrorP = [ 0, 100 ]
        trackfit.Fitter.NumberFitIterations = 3
        trackfit.Fitter.MaxNumberOutliers = 0
        sequence.Members.append( trackfit )
    else:
        trackfit = trackprefit.clone(Name + 'NoDriftTimeFit')
        trackfit.Fitter.MaxNumberOutliers = 0
        sequence.Members.append( trackfit )
            
    return sequence
