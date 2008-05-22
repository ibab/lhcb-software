from AlConfigurable import *
from Configurables import ( TrackFilterAlg, GetElementsToBeAligned, AlignSelTool, TrajOTProjector,
                            TrackRemoveDoubleHits, TrackContainerCleaner )
## Some default values
outputLevel  = INFO
useDrift     = False
trackThrough = "OT"
bField       = True

## OT traj projector (global) 
trajOTProjector = TrajOTProjector()
trajOTProjector.UseDrift = useDrift

## Track filter algorithm for alignment
trackFilterAlg                           = TrackFilterAlg( "FilterTracks" )
trackFilterAlg.OutputLevel               = outputLevel
trackFilterAlg.TracksOutputContainer     = "Alignment/FilteredTracks"
trackFilterAlg.TrackType                 = "Long"
trackFilterAlg.StripUnwantedDetectorHits = False
trackFilterAlg.KeepDetectorHits          = "IT"
trackFilterAlg.MinNHits                  = 0                            
trackFilterAlg.MaxNormTrackChi2          = 999999 
               
## Add GetElementsToBeAligned tool to track filter algorithm
trackFilterAlg.addTool( GetElementsToBeAligned, name = "GetElementsToBeAligned" )
trackFilterAlg.GetElementsToBeAligned.OutputLevel = outputLevel
trackFilterAlg.GetElementsToBeAligned.Elements    = AlConfigurable().getProp( "ElementsToAlign" )

## Add track selector tool to track filter algorithm
trackFilterAlg.addTool( AlignSelTool, name = "Selector" )
trackFilterAlg.Selector.OutputLevel                   = outputLevel
trackFilterAlg.Selector.BFieldStatus                  = bField
trackFilterAlg.Selector.MattCuts                      = True
trackFilterAlg.Selector.TrackType                     = "All"    
trackFilterAlg.Selector.TracksLocation                = "Rec/Track/Best"
trackFilterAlg.Selector.IsolatedTrackNStripsTolerance = 2         
trackFilterAlg.Selector.IsolatedTrackNStrawsTolerance = 1
trackFilterAlg.Selector.MomentumMinCut                = -999999               
trackFilterAlg.Selector.FitMatchChi2MaxCut            =  999999                         
trackFilterAlg.Selector.Chi2PerDoFMaxCut              =  999999
trackFilterAlg.Selector.Chi2ProbMinCut                = -999999
trackFilterAlg.Selector.NHolesMaxCut                  =  999999
trackFilterAlg.Selector.NSharedHitsMaxCut             =  999999                                                
trackFilterAlg.Selector.NCloseHitsMaxCut              =  999999                                      

## Now add it to the filter sequencer
AlConfigurable().filterSeq().Members.append( trackFilterAlg )

## Something wrong here
trackRemoveDoubleHits = TrackRemoveDoubleHits( TracksLocation = "Alignment/FilteredTracks" )
#AlConfigurable().filterSeq().Members.append( trackRemoveDoubleHits )

trackChi2Cleaner = TrackContainerCleaner( "CleanChi2" )
trackChi2Cleaner.OutputLevel   = outputLevel
trackChi2Cleaner.inputLocation = "Alignment/FilteredTracks"

trackChi2Cleaner.addTool( AlignSelTool , name = "Selector" )
# Need one extra line to really add the tool. Since property name differs from option name.
trackChi2Cleaner.selectorName = trackChi2Cleaner.Selector
# Now we configure the tool
trackChi2Cleaner.Selector.OutputLevel        = outputLevel
trackChi2Cleaner.Selector.BFieldStatus       = bField
trackChi2Cleaner.Selector.TracksLocation     = "Alignment/FilteredTracks"
trackChi2Cleaner.Selector.TrackType          = "ALL"
trackChi2Cleaner.Selector.MomentumMinCut     = -999999
trackChi2Cleaner.Selector.FitMatchChi2MaxCut = 999999
trackChi2Cleaner.Selector.Chi2PerDoFMaxCut   = 999999
trackChi2Cleaner.Selector.NHolesMaxCut       = 999999
trackChi2Cleaner.Selector.NSharedHitsMaxCut  = 999999                                          
trackChi2Cleaner.Selector.NCloseHitsMaxCut   = 999999

AlConfigurable().filterSeq().Members.append( trackChi2Cleaner )
