
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.4 2009-07-01 18:34:06 jonrob Exp $"
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
                  ,"TrackChi2PDOFCuts"    : {
                                              "Long"        : [0,50]
                                              ,"Upstream"   : [0,50]
                                              ,"Downstream" : [0,50]
                                              ,"Ttrack"     : [0,50]
                                              }
                  ,"TrackLikelihoodCuts"  : {
                                              #"Long"        : [-100,9e40]
                                              #,"Upstream"   : [-100,9e40]
                                              #,"Downstream" : [-100,9e40]
                                              #,"Ttrack"     : [-100,9e40]
                                              }
                  ,"GhostProbabilityCuts" : {
                                              #"Long"        : [0,0.99]
                                              #,"Upstream"   : [0,0.99]
                                              #,"Downstream" : [0,0.99]
                                              #,"Ttrack"     : [0,0.99]
                                              }
                  ,"CloneDistCuts"        : {
                                              #"Long"        : [-1e10,9e40]
                                              #,"Upstream"   : [-1e10,9e40]
                                              #,"Downstream" : [-1e10,9e40]
                                              #,"Ttrack"     : [-1e10,9e40]
                                              }
                  }

    ## Configure a track selector with the given name
    def setupTypeTrackSelector(self,tsname,selector):
        from Configurables import TrackSelector
        selector.addTool(TrackSelector,name=tsname)
        ts = getattr(selector,tsname)
        # Set Cuts
        ts.TrackTypes = [tsname]
        likCuts = self.getProp("TrackLikelihoodCuts")
        if tsname in likCuts :
            ts.MinLikelihoodCut = likCuts[tsname][0]
            ts.MaxLikelihoodCut = likCuts[tsname][1]
        ghostCuts = self.getProp("GhostProbabilityCuts")
        if tsname in ghostCuts :
            ts.MinGhostProbCut = ghostCuts[tsname][0]
            ts.MaxGhostProbCut = ghostCuts[tsname][1]
        chiCuts = self.getProp("TrackChi2PDOFCuts")
        if tsname in chiCuts :
            ts.MinChi2Cut = chiCuts[tsname][0]
            ts.MaxChi2Cut = chiCuts[tsname][1]
        cloneCuts = self.getProp("CloneDistCuts")
        if tsname in cloneCuts :
            ts.MinCloneDistCut = cloneCuts[tsname][0]
            ts.MaxCloneDistCut = cloneCuts[tsname][1]
        
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
        self.setupTypeTrackSelector( "Long",       charged.TrackSelector )
        self.setupTypeTrackSelector( "Upstream",   charged.TrackSelector )
        self.setupTypeTrackSelector( "Downstream", charged.TrackSelector )
        self.setupTypeTrackSelector( "Ttrack",     charged.TrackSelector )
        
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
