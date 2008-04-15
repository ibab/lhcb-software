#from Gaudi.Configuration import *
from AlConfigurable import *
from Configurables import (TrackFilterAlg, GetElementsToBeAligned, AlignSelTool, TrajOTProjector)

outputLevel = INFO
useDrift    = False

trajOTProjector = TrajOTProjector( OutputLevel = DEBUG, UseDrift = useDrift )

trackFilterAlg  = TrackFilterAlg( "FilterTracks1",
                                  OutputLevel               = outputLevel,
                                  TracksInputContainer      = "Rec/Track/Best",
                                  TracksOutputContainer     = "Alignment/FilterTracks1",
                                  TrackType                 = "Long",
                                  StripUnwantedDetectorHits = False,
                                  KeepDetectorHits          = "OT",
                                  MinNHits                  = 0)

trackFilterAlg.addTool( GetElementsToBeAligned( OutputLevel   = outputLevel                                 ,
                                                Elements      = AlConfigurable().getProp( "ElementsToAlign" ) ),
                        name = "GetElementsToBeAligned" )

trackFilterAlg.addTool( AlignSelTool( OutputLevel  = outputLevel,
                                      BFieldStatus                  =    True ,
                                      TrackType                     =   "ALL" ,
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

AlConfigurable().filterSeq().Members.append( trackFilterAlg )

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
                                    inputLocation  = "Alignment/FilterTracks1", #"Alignment/FittedTracks",
                                    outputLocation = "Alignment/AlignmentTracks",
                                    copyFailures   = False )

trackContCopy.addTool( TrackSelector(), name = "Selector" )
trackContCopy.Selector.TrackTypes = [ "Velo", "Long", "Ttrack" ]
trackContCopy.Selector.MaxChi2Cut = 20.0

AlConfigurable().filterSeq().Members.append( trackContCopy  )

