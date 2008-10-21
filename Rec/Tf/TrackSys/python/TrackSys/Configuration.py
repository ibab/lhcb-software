
## @package TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.5 2008-10-21 19:32:05 jonrob Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *

## @class TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class TrackSys(LHCbConfigurableUser):

    # Steering options
    __slots__ = {
        "expertHistos": False # set to True to write out expert histos
       ,"specialData"  : []   # Various special data processing options.
       ,"expertTracking":  []  # list of expert Tracking options, see KnownExpertTracking
       ,"trackPatRecAlgorithms": []  # List of track pattern recognition algorithms to run
       ,"trackExtraInfoAlgorithms": []  # List of track 'extra info' algorithms to run
        }
    
    ## Possible expert options
    KnownExpertTracking        = ["noDrifttimes", "simplifiedGeometry", "kalmanSmoother"]
    ## Default track pattern recognition algorithms to run
    DefaultPatRecAlgorithms    = ["Velo","Forward","TsaSeed","Match","Downstream","VeloTT"]
    ## Default track 'extra info' algorithms to run
    DefaultExtraInfoAlgorithms = ["CloneFlagging","TrackLikelihood","GhostProbability"]

    ## Check the options are sane etc.
    def defineOptions(self):
        if len(self.getProp("trackPatRecAlgorithms")) == 0 :
            self.setProp("trackPatRecAlgorithms",self.DefaultPatRecAlgorithms)
        if len(self.getProp("trackExtraInfoAlgorithms")) == 0 :
            self.setProp("trackExtraInfoAlgorithms",self.DefaultExtraInfoAlgorithms)
        for prop in self.getProp("expertTracking"):
            if prop not in self.KnownExpertTracking:
                raise RuntimeError("Unknown expertTracking option '%s'"%prop)

    ## Shortcut to the fieldOff option
    def fieldOff(self) : return "fieldOff" in self.getProp("specialData")
    ## Shortcut to the veloOpen option
    def veloOpen(self) : return "veloOpen" in self.getProp("specialData")
    ## Shortcut to the cosmics option
    def cosmics(self)  : return "cosmics"  in self.getProp("specialData")
    ## Shortcut to the noDrifttimes option
    def noDrifttimes(self) : return "noDrifttimes" in self.getProp("expertTracking")
    ## Shortcut to the simplifiedGeometry option
    def simplifiedGeometry(self) : return "simplifiedGeometry" in self.getProp("expertTracking")
    ## Shortcut to the kalmanSmoother option
    def kalmanSmoother(self) : return "kalmanSmoother" in self.getProp("expertTracking")

    ## @brief Apply the configuration
    def applyConf(self):
        self.defineOptions()
        importOptions( "$TRACKSYSROOT/options/RecoTracking.py" )

    
