
## @package SegmentCreator
#  RichRecSegment creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: SegmentCreator.py,v 1.3 2008-08-26 19:50:32 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__SegmentCreator )

# ----------------------------------------------------------------------------------

## @class RichSegmentCreatorConf
#  RichRecSegment creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichSegmentCreatorConf(RichConfigurableUser):

    ## Steering options
    __slots__ = {
        "context": "Offline"  # The context within which to run
       ,"energyBins": None
        }

    ## Initialise 
    def initialise(self):
        ## Default number of energy bins for each radiator
        self.setRichDefaults( "energyBins", { "Offline" : [ 5, 5, 5 ],
                                              "HLT"     : [ 2, 2, 2 ] } )

    ## @brief Apply the configuration
    #
    def applyConf(self):
        
        context = self.getProp("context")

        nickname = "RichSegmentCreator"
        
        segCreator = Rich__Rec__SegmentCreator( "ToolSvc."+context+"_"+nickname )

        segCreator.EnergyBins = self.getProp("energyBins")
