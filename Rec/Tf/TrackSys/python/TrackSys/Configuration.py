"""
High level configuration for Tracking
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-07-28 16:13:10 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *

class TrackSys(ConfigurableUser):

    # Steering options
    __slots__ = {
        "expertHistos": False # set to True to write out expert histos
       ,"fieldOff":     False # set to True for magnetic field off data
       ,"veloOpen":     False # set to True for Velo open data
       ,"expertTracking": []  # list of expert Tracking options, see KnownExpertTracking
        }

    KnownExpertTracking = ["usePatSeeding", "noDrifttimes", "simplifiedGeometry"]

    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def defineOptions(self):
        for prop in self.getProp("expertTracking"):
            if prop not in self.KnownExpertTracking:
                raise RuntimeError("Unknown expertTracking option '%s'"%prop)

    def applyConf(self):
        self.defineOptions()
