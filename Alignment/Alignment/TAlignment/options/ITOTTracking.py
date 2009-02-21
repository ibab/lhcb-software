from AlConfigurable import *

from Configurables import ( TrackFilterAlg, GetElementsToBeAligned, AlignSelTool, TrajOTProjector,
                            TrackContainerCleaner )
## Some default values
outputLevel  = INFO
trackThrough = "ALL"
bField       = True

## OT traj projector (global) 
trajOTProjector = TrajOTProjector( UseDrift = True )

## Track filter algorithm for alignment
trackFilterAlg                           = TrackFilterAlg( "FilterTracks" )
trackFilterAlg.OutputLevel               = outputLevel
trackFilterAlg.TracksInputContainer      = "Rec/Track/Best"
trackFilterAlg.TracksOutputContainer     = "Alignment/FilteredTracks"
trackFilterAlg.TrackType                 = "Long"
trackFilterAlg.StripUnwantedDetectorHits = False
trackFilterAlg.KeepDetectorHits          = "IT"
trackFilterAlg.MinNHits                  = 0

## Add GetElementsToBeAligned tool to track filter algorithm
trackFilterAlg.addTool( GetElementsToBeAligned, name = "GetElementsToBeAligned" )
trackFilterAlg.GetElementsToBeAligned.OutputLevel = WARNING
trackFilterAlg.GetElementsToBeAligned.Elements    = AlConfigurable().getProp( "ElementsToAlign" )

## Add track selector tool to track filter algorithm
trackFilterAlg.addTool( AlignSelTool, name = "Selector" )
trackFilterAlg.Selector.OutputLevel                   = outputLevel
trackFilterAlg.Selector.BFieldStatus                  = bField
trackFilterAlg.Selector.TrackType                     = trackThrough
trackFilterAlg.Selector.TracksLocation                = "Rec/Track/Best"
trackFilterAlg.Selector.IsolatedTrackNStripsTolerance = 2
trackFilterAlg.Selector.IsolatedTrackNStrawsTolerance = 1
trackFilterAlg.Selector.MultiplicityMaxCut            = 999999
trackFilterAlg.Selector.NITClustersMaxCut             = 999999
trackFilterAlg.Selector.NVeloClustersMaxCut           = 999999
trackFilterAlg.Selector.EtaMaxCut                     = 999999
trackFilterAlg.Selector.MomentumMinCut                = -999999
trackFilterAlg.Selector.FitMatchChi2MaxCut            = 999999
trackFilterAlg.Selector.Chi2PerDoFMaxCut              = 999999
trackFilterAlg.Selector.Chi2ProbMinCut                = -999999
trackFilterAlg.Selector.NHolesMaxCut                  = 999999
trackFilterAlg.Selector.NSharedHitsMaxCut             = 999999
trackFilterAlg.Selector.NCloseHitsMaxCut              = 999999

## Now add it to the filter sequencer
AlConfigurable().filterSeq().Members.append( trackFilterAlg )

## Setting algorithm for evolving (fit match) chi2 cut
trackChi2Cleaner             = TrackContainerCleaner( "CleanChi2" )
trackChi2Cleaner.OutputLevel = outputLevel
if AlConfigurable().getProp( "Pat" ) == True:
    trackChi2Cleaner.inputLocation = "Alignment/FilteredTracks"
else:
    trackChi2Cleaner.inputLocation = "Alignment/AlignmentTracks"

trackChi2Cleaner.addTool( AlignSelTool , name = "Selector" )
# Need one extra line to really add the tool. Since property name differs from option name.
trackChi2Cleaner.selectorName = trackChi2Cleaner.Selector
trackChi2Cleaner.Selector.OutputLevel = WARNING
if AlConfigurable().getProp( "Pat" ) == True:
    AlConfigurable().filterSeq().Members.append( trackChi2Cleaner )

# If the pattern recognition is not run, need to define the fit sequence.
elif AlConfigurable().getProp( "Pat" ) == False:
    from Configurables import ( TrackEventFitter, TrackMasterFitter,
                                TrackMasterExtrapolator, TrackKalmanFilter,
                                TrackRemoveDoubleHits)

    # Algorithm to remove double TT/IT/OT hits
    trackRemoveDoubleHits = TrackRemoveDoubleHits( TracksLocation = "Alignment/FilteredTracks" )
    AlConfigurable().filterSeq().Members.append( trackRemoveDoubleHits )

    trackFitAlg                    = TrackEventFitter( "FitTracks" )
    trackFitAlg.TracksInContainer  = "Alignment/FilteredTracks"
    trackFitAlg.TracksOutContainer = "Alignment/AlignmentTracks"

    applyMS = AlConfigurable().getProp( "ApplyMS" )
        
    trackFitAlg.addTool( TrackMasterFitter, name = "Fitter" )
    trackFitAlg.Fitter.ApplyMaterialCorrections = applyMS
    trackFitAlg.Fitter.MaxNumberOutliers        = 2
    trackFitAlg.Fitter.NumberFitIterations      = 3
    
    trackFitAlg.Fitter.addTool( TrackMasterExtrapolator, name = "Extrapolator" )
    trackFitAlg.Fitter.Extrapolator.ApplyMultScattCorr          = applyMS
    trackFitAlg.Fitter.Extrapolator.ApplyEnergyLossCorr         = applyMS
    trackFitAlg.Fitter.Extrapolator.ApplyElectronEnergyLossCorr = applyMS
    
    trackFitAlg.Fitter.Extrapolator.addTool( TrackKalmanFilter, name = "NodeFitter")
    
    trackFitAlg.Fitter.Extrapolator.NodeFitter.addTool(TrackMasterExtrapolator, name = "Extrapolator" )
    trackFitAlg.Fitter.Extrapolator.NodeFitter.Extrapolator.ApplyMultScattCorr          = applyMS
    trackFitAlg.Fitter.Extrapolator.NodeFitter.Extrapolator.ApplyEnergyLossCorr         = applyMS
    trackFitAlg.Fitter.Extrapolator.NodeFitter.Extrapolator.ApplyElectronEnergyLossCorr = applyMS
    
    AlConfigurable().fitSeq().Members.append( trackFitAlg      )
    AlConfigurable().fitSeq().Members.append( trackChi2Cleaner )
