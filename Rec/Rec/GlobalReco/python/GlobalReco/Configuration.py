
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.6 2009-07-06 15:54:52 jonrob Exp $"
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
                  ,"OutputLevel" : INFO      # The printout level to use
                  ,"TrackTypes"  : [ "Long", "Upstream", "Downstream" ]
                  ,"TrackCuts"   : {  "Long"       : { "Chi2Cut" : [0,50] }
                                     ,"Upstream"   : { "Chi2Cut" : [0,50] }
                                     ,"Downstream" : { "Chi2Cut" : [0,50] }
                                   }
                  }

    ## Configure a track selector with the given name
    def setupTypeTrackSelector(self,tsname,selector):
        from Configurables import TrackSelector
        selector.addTool(TrackSelector,name=tsname)
        ts = getattr(selector,tsname)
        # Set Cuts
        ts.TrackTypes = [tsname]
        cuts = self.getProp("TrackCuts")
        if tsname in cuts :
            for name,cut in cuts[tsname].iteritems() :
                ts.setProp("Min"+name,cut[0])
                ts.setProp("Max"+name,cut[1])
        
    ## Apply the configuration to the given sequence
    def applyConf(self):

        if not self.isPropertySet("RecoSequencer") :
            raise RuntimeError("ERROR : PROTO Sequencer not set")

        from Configurables import ( ChargedProtoPAlg, ChargedProtoCombineDLLsAlg,
                                    NeutralProtoPAlg, DelegatingTrackSelector )
        
        seq = self.getProp("RecoSequencer")
        seq.Context = self.getProp("Context")

        # Charged Proto particles
        charged = ChargedProtoPAlg()
        charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
        tracktypes = self.getProp("TrackTypes")
        charged.TrackSelector.TrackTypes = tracktypes
        for type in tracktypes : self.setupTypeTrackSelector( type, charged.TrackSelector )
        
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
