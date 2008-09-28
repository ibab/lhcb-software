
## @package TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.4 2008-09-28 10:13:10 smenzeme Exp $"
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
       ,"fieldOff":     False # set to True for magnetic field off data
       ,"veloOpen":     False # set to True for Velo open data
       ,"expertTracking": []  # list of expert Tracking options, see KnownExpertTracking
        }

    KnownExpertTracking = ["usePatSeeding", "noDrifttimes", "simplifiedGeometry", "kalmanSmoother", "MuonAlignTracks"]

    def defineOptions(self):
        for prop in self.getProp("expertTracking"):
            if prop not in self.KnownExpertTracking:
                raise RuntimeError("Unknown expertTracking option '%s'"%prop)

    ## @brief Apply the configuration
    def applyConf(self):
        importOptions( "$TRACKSYSROOT/options/RecoTracking.py" )
        self.defineOptions()
