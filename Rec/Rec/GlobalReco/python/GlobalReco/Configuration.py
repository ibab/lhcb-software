
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.1 2008-12-17 18:02:47 jonrob Exp $"
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

        seq = self.getProp("RecoSequencer")
        if seq == None : raise RuntimeError("ERROR : PROTO Sequencer not set")

        seq.Context = self.getProp("Context")
        
        # Charged Proto particles
        # For time being put all protoparticles in the same container
        seq.Members += [ "ChargedProtoPAlg" ]
        # Fill the Combined DLL information in the charged protoparticles
        seq.Members += [ "ChargedProtoCombineDLLsAlg" ]

        # Neutrals
        seq.Members += [ "NeutralProtoPAlg" ]
