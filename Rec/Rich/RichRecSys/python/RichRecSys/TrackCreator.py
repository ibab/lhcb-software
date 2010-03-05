
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
        "Context":       "Offline"  # The context within which to run
       ,"Radiators": [True,True,True] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"SpecialData"  : []   # Various special data processing options. See KnownSpecialData in RecSys for all options
       ,"InputTracksLocation" : "" # The input location for tracks
       ,"OutputLevel"   : INFO    # The output level to set all algorithms and tools to use
       ,"TrackTypes"    : [ "Forward","Match","Seed","VeloTT","KsTrack" ]
       ,"TrackCuts"     : { } # { "Chi2Cut" : [0,50] }
        }

    ## @brief Set OutputLevel 
    def setOutputLevel(self,conponent):
        if self.isPropertySet("OutputLevel") :
            conponent.OutputLevel = self.getProp("OutputLevel")

    ## @brief Apply the configuration
    #
    def applyConf(self):

        # segments
        segConf = self.getRichCU(RichSegmentCreatorConf)
        self.setOtherProp(segConf,"Context")

        # Configure the tracking tools
        nickname = "RichTrackCreator"
        
        # Track creator
        trackCr = self.richTools().trackCreator(nickname)

        # OutputLevel
        self.setOutputLevel(trackCr)

        # Input tracks
        if not self.isPropertySet("InputTracksLocation"):
            raise RuntimeError("ERROR : Input Track Location not set" )
        trackCr.TracksLocation = self.getProp("InputTracksLocation")
            
        # Track selector
        trselname = "TrackSelector"
        trackCr.addTool( self.richTools().trackSelector(trselname), name=trselname )

        # Track Selector Cuts
        if "earlyData" in self.getProp("SpecialData") and 0 == len(self.getProp("TrackCuts")):
            cuts = { "Chi2Cut"    : [0,100],
                     "Likelihood" : [-999999,999999] }
            self.setProp("TrackCuts",cuts)

        trackCr.TrackSelector.TrackAlgs = self.getProp("TrackTypes")
        cuts = self.getProp("TrackCuts")
        for name,cut in cuts.iteritems() :
            trackCr.TrackSelector.setProp("Min"+name,cut[0])
            trackCr.TrackSelector.setProp("Max"+name,cut[1])
            
        # Segment maker
        if trackCr.getType() != "Rich::Rec::DelegatedTrackCreatorFromRecoTracks" :
            trSegNickName = "TrSegMaker"
            trackCr.TrackSegmentTool = trSegNickName
            trSeg = self.richTools().trSegMaker()
            trSeg.UseRadiators = self.getProp("Radiators")
            trackCr.addTool( trSeg, trSegNickName )

        # Printout
        self.printInfo(segConf)
