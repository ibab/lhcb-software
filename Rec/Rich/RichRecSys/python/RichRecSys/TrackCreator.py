
## @package TrackCreator
#  Track creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: TrackCreator.py,v 1.11 2009-12-11 13:43:08 cattanem Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm, GeV
from SegmentCreator import *

# ----------------------------------------------------------------------------------

## @class RichRecSysConf
#  Configurable for RICH reconstruction track creator
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichTrackCreatorConfig(RichConfigurableUser):

       ## Possible used Configurables
    __used_configurables__ = [ (RichSegmentCreatorConf,None) ]

    # Steering options
    __slots__ = {
        "Context"        : "Offline"  # The context within which to run
       ,"DataType"       : ""         # Type of data, propagated from application
       ,"Radiators": [True,True,True] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"SpecialData"    : []   # Various special data processing options. See KnownSpecialData in RecSys for all options
       ,"InputTracksLocation" : "" # The input location for tracks
       ,"OutputLevel"    : INFO    # The output level to set all algorithms and tools to use
       ,"TrackCuts"      : None
       ,"MaxInputTracks" : None
       ,"MaxUsedTracks"  : None
        }
    
    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefaults ( "MaxInputTracks", { "Offline" : 99999, 
                                                   "HLT"     : 99999 } )
        self.setRichDefaults ( "MaxUsedTracks",  { "Offline" : 1000, 
                                                   "HLT"     : 500 } )
        self.setRichDefault ( "TrackCuts", "Offline",
                              { "Forward" : { "Chi2Cut" : [0,5], "PCut" : [0,9999999] },
                                "Match"   : { "Chi2Cut" : [0,5], "PCut" : [0,9999999] },
                                "Seed"    : { "Chi2Cut" : [0,5], "PCut" : [1,9999999] },
                                "VeloTT"  : { "Chi2Cut" : [0,5], "PCut" : [1,9999999] },
                                "KsTrack" : { "Chi2Cut" : [0,5], "PCut" : [1,9999999] } } )
        # If a ghost prob cut is needed, add "GhostProbCut" : [-999,0.5]
        
        self.setRichDefault ( "TrackCuts", "HLT",
                              { "Forward" : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "Match"   : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "Seed"    : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "VeloTT"  : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "KsTrack" : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] } } )

    ## @brief Set OutputLevel 
    def setOutputLevel(self,conponent):
        if self.isPropertySet("OutputLevel") :
            conponent.OutputLevel = self.getProp("OutputLevel")

    ## @brief Apply any tweeks to the default configuration that vary by DataType
    def dataTypeTweeks(self):

        # Get the DataType
        dataType = self.getProp("DataType")

        # Different cuts for early data
        if dataType == "2009" or dataType == "2010":
            
            if not self.isPropertySet("MaxUsedTracks") :
                if self.getProp("Context") == "Offline" :
                    self.setProp( "MaxUsedTracks", 500 )
                else:
                    self.setProp( "MaxUsedTracks", 400 )

        ## elif dataType == "Upgrade" :

        ##     # Temporarily add ghost prob cuts
        ##     if not self.isPropertySet("TrackCuts") :
        ##         self.setProp( "TrackCuts", { "Forward" : { "GhostProbCut": [0.0,0.3], "Chi2Cut" : [0,5], "PCut" : [0,9999999] } ,
        ##                                      "Match"   : { "GhostProbCut": [0.0,0.3], "Chi2Cut" : [0,5], "PCut" : [0,9999999] } ,
        ##                                      "Seed"    : { "GhostProbCut": [0.0,0.3], "Chi2Cut" : [0,5], "PCut" : [0,9999999] } ,
        ##                                      "VeloTT"  : { "GhostProbCut": [0.0,0.3], "Chi2Cut" : [0,5], "PCut" : [0,9999999] } ,
        ##                                      "KsTrack" : { "GhostProbCut": [0.0,0.3], "Chi2Cut" : [0,5], "PCut" : [0,9999999] } } )
            

    ## @brief Apply the configuration
    #
    def applyConf(self):

        # segments
        segConf = self.getRichCU(RichSegmentCreatorConf)
        self.setOtherProp(segConf,"Context")

        # DataType specific tweeks
        self.dataTypeTweeks()

        # Configure the tracking tools
        nickname = "RichTrackCreator"
        
        # Track creator
        trackCr = self.richTools().trackCreator(nickname)

        # Track cuts
        trackCr.MaxInputTracks = self.getProp("MaxInputTracks")
        trackCr.MaxUsedTracks  = self.getProp("MaxUsedTracks")

        # Input tracks
        if not self.isPropertySet("InputTracksLocation"):
            raise RuntimeError("ERROR : Input Track Location not set" )
        trackCr.TracksLocation = self.getProp("InputTracksLocation")
            
        # Track selector
        trselname = "TrackSelector"
        trackCr.addTool( self.richTools().trackSelector(trselname), name=trselname )
        import TrackSelectorTools
        TrackSelectorTools.configureTrackSelectorCuts(trackCr.TrackSelector,self.getProp("TrackCuts"))
           
        # Segment maker
        if trackCr.getType() != "Rich::Rec::DelegatedTrackCreatorFromRecoTracks" :
            trSegNickName = "TrSegMaker"
            trackCr.TrackSegmentTool = trSegNickName
            trSeg = self.richTools().trSegMaker()
            trSeg.UseRadiators = self.getProp("Radiators")
            trackCr.addTool( trSeg, trSegNickName )

        # OutputLevels
        self.setOutputLevel(trackCr)
        self.setOutputLevel(self.richTools().expectedTrackSignal())

        # Printout
        self.printInfo(segConf)
