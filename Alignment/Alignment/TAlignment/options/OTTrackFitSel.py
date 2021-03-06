from Configurables import (TrackFilterAlg, GetElementsToBeAligned, AlignSelTool, TrajOTProjector)
from Configurables import TAlignment
from TAlignment import *
INFO=3
DEBUG=2
outputLevel = INFO
useDrift    = False
trackThrough = "OT"

trajOTProjector = TrajOTProjector( OutputLevel = DEBUG, UseDrift = useDrift )

trackFilterAlg  = TrackFilterAlg( "FilterTracks1",
                                  OutputLevel               = outputLevel,
                                  TracksInputContainer      = "Rec/Track/Best", #"Rec/Track/Tsa", 
                                  TracksOutputContainer     = "Alignment/FilterTracks1",
                                  TrackType                 = "Long", #"Ttrack", #"Long",
                                  StripUnwantedDetectorHits = False,
                                  KeepDetectorHits          = "OT",
                                  MinNHits                  = 0)

elements = TAlignment().getProp( "ElementsToAlign" )

trackFilterAlg.addTool( GetElementsToBeAligned( OutputLevel     = outputLevel
                                                , Elements      = elements )
                        , name = "GetElementsToBeAligned" )

trackFilterAlg.addTool( AlignSelTool( OutputLevel  = outputLevel,
                                      BFieldStatus                  =    True ,
                                      TrackType                     =   trackThrough,
                                      IsolatedTrackNStripsTolerance =       2 ,
                                      IsolatedTrackNStrawsTolerance =       1 ,
                                      #MattCuts                      =   False ,
                                      MomentumMinCut                = -999999 ,
                                      PtMinCut                      = -999999 ,
                                      FitMatchChi2MaxCut            =  999999 ,
                                      Chi2PerDoFMaxCut              =  999999 ,
                                      Chi2ProbMinCut                = -999999 ,
                                      NHolesMaxCut                  =  999999 ,
                                      NSharedHitsMaxCut             =  999999 ,
                                      NCloseHitsMaxCut              =  999999  ),
                        name = "Selector"                          )

TAlignment().filterSeq().Members.append( trackFilterAlg )

from Configurables import ( TrajOTProjector, TrackEventFitter, TrackMasterFitter, TrackMasterExtrapolator, TrackKalmanFilter, TrackSimpleExtraSelector )



# trackFitAlg                           = TrackEventFitter("FitTracks",
#                                                          OutputLevel        = INFO,
#                                                          TracksInContainer  = "Alignment/FilterTracks1",
#                                                          TracksOutContainer = "Alignment/FittedTracks")
# trackFitAlg.addTool(TrackMasterFitter("Fitter"))
# trackMasterFitter = allConfigurables["FitTracks.Fitter"]
# trackMasterFitter.addTool(TrackMasterExtrapolator("Extrapolator"))
# trackMasterExtrapolator = allConfigurables["FitTracks.Fitter.Extrapolator"]
# trackMasterFitter.addTool(TrackKalmanFilter("NodeFitter"))
# trackMasterNodeFitter = allConfigurables["FitTracks.Fitter.NodeFitter"]
# trackMasterNodeFitter.addTool(TrackMasterExtrapolator("Extrapolator"))
# trackMasterNodeFitterExtrapolator = allConfigurables["FitTracks.Fitter.NodeFitter.Extrapolator"]
# importOptions("$TALIGNMENTROOT/options/BFieldOff.opts")

#AlConfigurable().filterSeq().Members.append( trackFitAlg )

from Configurables import ( TrackContainerCopy, TrackSelector )
trackContCopy = TrackContainerCopy( "FilterTracks2",
                                    OutputLevel    = INFO,
                                    inputLocations  = ["Alignment/FilterTracks1"], #"Alignment/FittedTracks",
                                    outputLocation = "Alignment/AlignmentTracks",
                                    copyFailures   = False )

trackContCopy.addTool( TrackSelector(), name = "Selector" )
trackContCopy.Selector.TrackTypes = [ "Long", "Ttrack" ]
trackContCopy.Selector.MaxChi2Cut = 40.0

TAlignment().filterSeq().Members.append( trackContCopy  )

