
## @package SegmentCreator
#  RichRecSegment creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: SegmentCreator.py,v 1.5 2009-09-16 13:37:28 jonrob Exp $"
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
        "Context"     : "Offline"  # The context within which to run
       ,"EnergyBins"  : None
       ,"OutputLevel" : INFO    # The output level to set all algorithms and tools to use
        }

    ## Initialize 
    def initialize(self):
        
        ## Default number of energy bins for each radiator
        self.setRichDefaults( "EnergyBins", { "Offline" : [ 5, 5, 5 ],
                                              "HLT"     : [ 2, 2, 2 ] } )

    ## @brief Apply the configuration
    def applyConf(self):
        
        context = self.getProp("Context")

        nickname = "RichSegmentCreator"
        
        segCreator = Rich__Rec__SegmentCreator( "ToolSvc."+context+"_"+nickname )

        segCreator.EnergyBins = self.getProp("EnergyBins")

        if self.isPropertySet("OutputLevel") :
            segCreator.OutputLevel = self.getProp("OutputLevel")
