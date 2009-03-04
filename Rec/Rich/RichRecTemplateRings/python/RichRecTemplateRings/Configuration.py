
## @package RichRecTemplateRings
#  High level Configuration tools for RICH Template Ring finder
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Sajan Easo   (Sajan.Easo@cern.ch)
#  @date   02/03/2009

__version__ = "$Id: Configuration.py,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichTemplateRingFinderConf
#  High level Configuration tools for RICH Template Ring finder
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichTemplateRingFinderConf(RichConfigurableUser):

        ## Steering options
    __slots__ = {
        "Context"             :    "Offline"   # The context within which to run
       ,"Sequencer"           : GaudiSequencer("RichTemplateDefaultSequencer")  # The sequencer to add the RICH MCMC algorithms to
        }

    ## @brief Apply the configuration to the given GaudiSequencer
    def __apply_configuration__(self):

        sequence = self.getProp("Sequencer")
        if sequence.name() == "RichTemplateDefaultSequencer" :
            raise RuntimeError("ERROR : Rich Template Ring Finder Sequence not set")

        from Configurables import ( RichRingRecAlgBase,
                                    RichTargetDataConfigAlg,
                                    RichRingMasterAlg )

        base   = RichRingRecAlgBase()
        config = RichTargetDataConfigAlg()
        master = RichRingMasterAlg()

        sequence.Members += [ base, config, master ]
