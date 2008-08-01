
"""
High level configuration tools for Brunel
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-08-01 09:35:38 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from Gaudi.Configuration import *

class RichRecSys(ConfigurableUser):

    # Steering options
    __slots__ = {
        "Mode": "FullOffilneGlobal"
        }

    def applyConf(self):
          importOptions( "$RICHRECSYSOPTS/RecoOfflineToolPara.opts" )
          importOptions( "$RICHRECSYSOPTS/RecoOfflineAlgs.opts" )
