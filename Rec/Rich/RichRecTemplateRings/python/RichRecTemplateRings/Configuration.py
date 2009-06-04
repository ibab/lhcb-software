
## @package RichRecTemplateRings
#  High level Configuration tools for RICH Template Ring finder
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Sajan Easo   (Sajan.Easo@cern.ch)
#  @date   02/03/2009

__version__ = "$Id: Configuration.py,v 1.3 2009-06-04 15:26:53 jonrob Exp $"
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
        "Context"             : "Offline"   # The context within which to run
       ,"Sequencer"           : None        # The sequencer to add the RICH MCMC algorithms to
       ,"MakeNtuple"          : False       # Make development ntuple
        }

    ## @brief Apply the configuration to the given GaudiSequencer
    def __apply_configuration__(self):

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Rich Template Ring Finder Sequence not set")
        
        sequence = self.getProp("Sequencer")

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

            master.RichTemplateRingNtupProduce = True
            
