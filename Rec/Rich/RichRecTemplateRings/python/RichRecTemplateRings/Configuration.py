
## @package RichRecTemplateRings
#  High level Configuration tools for RICH Template Ring finder
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Sajan Easo   (Sajan.Easo@cern.ch)
#  @date   02/03/2009

__version__ = "$Id: Configuration.py,v 1.2 2009-03-19 17:50:46 seaso Exp $"
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
       ,"MakeNtuple"          : False
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

        if self.getProp("MakeNtuple") :

            from Configurables import NTupleSvc
            NTupleSvc().Output = ["RICHTUPLE1 DATAFILE='Ntuple_RichTemplateRings.root' TYP='ROOT' OPT='NEW'"]

            master.RichTemplateRingNtupProduce=True
        
