
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.3 2009-06-25 13:58:59 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class GlobalRecoConf
#  Configurable for the Global PID reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class GlobalRecoConf(LHCbConfigurableUser):
    
    ## Options
    __slots__ = { "SpecialData"  : []
                  ,"Context":    "Offline"   # The context within which to run
                  ,"RecoSequencer" : None    # The sequencer to add the ProtoParticle reconstruction algorithms to
                  ,"OutputLevel" : INFO     # The printout level to use
                  }

    ## Apply the configuration to the given sequence
    def applyConf(self):

        if not self.isPropertySet("RecoSequencer") :
            raise RuntimeError("ERROR : PROTO Sequencer not set")

        from Configurables import ( ChargedProtoPAlg, ChargedProtoCombineDLLsAlg,
                                    NeutralProtoPAlg )
        
        seq = self.getProp("RecoSequencer")
        seq.Context = self.getProp("Context")

        # Charged Proto particles
        charged = ChargedProtoPAlg()
        # Fill the Combined DLL information in the charged protoparticles
        combine = ChargedProtoCombineDLLsAlg()
        # Neutrals
        neutral = NeutralProtoPAlg()

        # Set output levels
        if self.isPropertySet("OutputLevel"):
            level = self.getProp("OutputLevel")
            charged.OutputLevel = level
            combine.OutputLevel = level
            neutral.OutputLevel = level

        # Add to sequencer
        seq.Members += [ charged,combine,neutral ]
