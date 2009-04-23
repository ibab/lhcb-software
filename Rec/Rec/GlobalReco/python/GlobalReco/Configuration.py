
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.2 2009-04-23 15:20:25 jonrob Exp $"
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
                  }

    ## Apply the configuration to the given sequence
    def applyConf(self):

        if not self.isPropertySet("RecoSequencer") :
            raise RuntimeError("ERROR : PROTO Sequencer not set")
        seq = self.getProp("RecoSequencer")
        
        seq.Context = self.getProp("Context")
        
        # Charged Proto particles
        # For time being put all protoparticles in the same container
        seq.Members += [ "ChargedProtoPAlg" ]
        # Fill the Combined DLL information in the charged protoparticles
        seq.Members += [ "ChargedProtoCombineDLLsAlg" ]

        # Neutrals
        seq.Members += [ "NeutralProtoPAlg" ]
