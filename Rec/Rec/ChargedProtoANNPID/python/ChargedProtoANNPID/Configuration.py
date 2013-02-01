## @package GlobalReco
#  Base configurables for the Global ANN Charged PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   03/12/2010

__version__ = "$Id: Configuration.py,v 1.12 2010-03-15 16:47:36 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class ChargedProtoANNPIDConf
#  Configurable for the Global ANN Charged PID reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   03/12/2010
class ChargedProtoANNPIDConf(LHCbConfigurableUser):

    ## Options
    __slots__ = { "Context":    "Offline"    # The context within which to run
                  ,"RecoSequencer" : None    # The sequencer to use
                  ,"OutputLevel" : INFO      # The printout level to use
                  ,"ProtoParticlesLocation" : None
                  ,"NetworkVersions" : { "2012" : "MC12TuneV1", 
                                         "2011" : "MC12TuneV1", 
                                         "2010" : "MC11aTuneV3",
                                         "2009" : "MC10TuneV1",
                                         "2008" : "MC10TuneV1",
                                         "MC09" : "MC10TuneV1" }
                  ,"DataType"   : "" # Type of data, propagated from application
                  ,"TrackTypes" : ["Long","Downstream","Upstream"]
                  ,"PIDTypes"   : ["Electron","Muon","Pion","Kaon","Proton","Ghost"]
                  ,"SuppressANNPrintout" : True
                  }

    ## Apply the configuration to the given sequence
    def applyConf(self):

        from Configurables import ANNGlobalPID__ChargedProtoANNPIDAlg

        # Check the sequencer to use is properly configured
        if not self.isPropertySet("RecoSequencer") :
            raise RuntimeError("ERROR : PROTO ANN PID Sequencer not set")
        nnpidseq = self.getProp("RecoSequencer")

        dataType   = self.getProp("DataType")
        annVersion = self.getProp("NetworkVersions")[dataType]

        # Loop over track types
        for track in self.getProp("TrackTypes") :

            # Loop over PID types
            for pid in self.getProp("PIDTypes") :

                # Network algorithm
                nn = ANNGlobalPID__ChargedProtoANNPIDAlg("ANNGPID"+track+pid)

                # Set configuration for this track and PID combination
                nn.Configuration = "GlobalPID_"+pid+"_"+track+"_ANN.txt"

                # Network version
                nn.NetworkVersion = annVersion

                # If configured, set the OutputLevel
                if self.isPropertySet("OutputLevel") :
                    nn.OutputLevel = self.getProp("OutputLevel")

                # If configured, set the ProtoParticle location
                if self.isPropertySet("ProtoParticlesLocation") :
                    nn.ProtoParticleLocation = self.getProp("ProtoParticlesLocation")

                # Suppress ANN Printout
                nn.SuppressANNPrintout = self.getProp("SuppressANNPrintout")

                # Add to sequencer
                nnpidseq.Members += [nn]
