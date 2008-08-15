
## @package SegmentCreator
#  RichRecSegment creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: SegmentCreator.py,v 1.2 2008-08-15 14:41:23 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__SegmentCreator )

# ----------------------------------------------------------------------------------

## @class RichSegmentCreatorConf
#  RichRecSegment creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichSegmentCreatorConf(RichConfigurableUser):

    ## Default number of energy bins for each radiator
    DefaultEnergyBins = { "Offline" : [ 5, 5, 5 ],
                          "HLT"     : [ 2, 2, 2 ] }

    ## Steering options
    __slots__ = {
        "context": "Offline"  # The context within which to run
       ,"energyBins": [ ]
        }

    ## @brief Apply the configuration
    #
    def applyConf(self):

        context = self.getProp("context")

        # Check settings
        self.checkRadData("energyBins",self.DefaultEnergyBins[context])

        nickname = "RichSegmentCreator"
        
        segCreator = Rich__Rec__SegmentCreator( "ToolSvc."+context+"_"+nickname )

        segCreator.EnergyBins = self.getProp("energyBins")
