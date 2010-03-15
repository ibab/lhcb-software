
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.12 2010-03-15 16:47:36 jonrob Exp $"
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
        
        seq = self.getProp("RecoSequencer")
        seq.Context = self.getProp("Context")

        # Charged Proto particles
        from Configurables import ( GaudiSequencer,
                                    ChargedProtoParticleMaker,
                                    ChargedProtoParticleAddRichInfo,
                                    ChargedProtoParticleAddMuonInfo,
                                    ChargedProtoParticleAddEcalInfo,
                                    ChargedProtoParticleAddBremInfo,
                                    ChargedProtoParticleAddHcalInfo,
                                    ChargedProtoParticleAddPrsInfo,
                                    ChargedProtoParticleAddSpdInfo,
                                    ChargedProtoParticleAddVeloInfo,
                                    ChargedProtoCombineDLLsAlg,
                                    DelegatingTrackSelector )
        cseq = GaudiSequencer("ChargedProtoParticles")
        seq.Members += [cseq]

        # Adapt cuts for early data. N.B. this should be changed to allow overriding of options
        if "earlyData" in self.getProp("SpecialData"):
          self.setProp("TrackTypes", [ "Long","Upstream","Downstream","Ttrack","Velo","VeloR" ])
          self.setProp("TrackCuts", { })
        
        # Make Charged ProtoParticles
        charged = ChargedProtoParticleMaker("ChargedProtoPMaker")
        charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
        tracktypes = self.getProp("TrackTypes")
        charged.TrackSelector.TrackTypes = tracktypes
        for type in tracktypes : self.setupTypeTrackSelector( type, charged.TrackSelector )
        # Add PID information
        rich = ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich")
        muon = ChargedProtoParticleAddMuonInfo("ChargedProtoPAddMuon")
        ecal = ChargedProtoParticleAddEcalInfo("ChargedProtoPAddEcal")
        brem = ChargedProtoParticleAddBremInfo("ChargedProtoPAddBrem")
        hcal = ChargedProtoParticleAddHcalInfo("ChargedProtoPAddHcal")
        prs  = ChargedProtoParticleAddPrsInfo("ChargedProtoPAddPrs")
        spd  = ChargedProtoParticleAddSpdInfo("ChargedProtoPAddSpd")
        velo = ChargedProtoParticleAddVeloInfo("ChargedProtoPAddVeloDEDX")
        # Fill the Combined DLL information in the charged protoparticles
        combine = ChargedProtoCombineDLLsAlg("ChargedProtoPCombDLLs")
        # Fill the sequence
        cseq.Members += [ charged,ecal,brem,hcal,prs,spd,velo ]
        cseq.Members += [ rich,muon,combine ]

        # Test NNs
        #from Configurables import ChargedProtoANNPIDAlg
        #electronNN = ChargedProtoANNPIDAlg("ElectronNNPID")
        #electronNN.Configuration = "GlobalPID_electron_ANN.txt"
        #cseq.Members += [electronNN]
        
        # Neutrals
        from Configurables import NeutralProtoPAlg
        nseq = GaudiSequencer("NeutralProtoParticles")
        seq.Members += [nseq]
        neutral = NeutralProtoPAlg("NeutralProtoPMaker")
        nseq.Members += [ neutral ]
        
        # Set output levels
        if self.isPropertySet("OutputLevel"):
            level = self.getProp("OutputLevel")
            charged.OutputLevel = level
            rich.OutputLevel = level
            muon.OutputLevel = level
            ecal.OutputLevel = level
            brem.OutputLevel = level
            hcal.OutputLevel = level
            prs.OutputLevel = level
            spd.OutputLevel = level
            velo.OutputLevel = level
            combine.OutputLevel = level
            neutral.OutputLevel = level

## @class GlobalRecoChecks
#  Configurable for the Global PID reconstruction MC based checking
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   26/02/2010
class GlobalRecoChecks(LHCbConfigurableUser):
    
    ## Options
    __slots__ = { "Sequencer"   : None    # The sequencer to add monitors to
                 ,"OutputLevel" : INFO    # The printout level to use
                 ,"Context":    "Offline" # The context within which to run
                  }

    ## Apply the configuration to the given sequence
    def applyConf(self):

        if not self.isPropertySet("Sequencer"):
            raise RuntimeError("ERROR : Sequencer not set")
            
        protoSeq = self.getProp("Sequencer")
        protoSeq.Context = self.getProp("Context")

        from Configurables import ( NTupleSvc, ChargedProtoParticleTupleAlg,
                                    ChargedProtoParticleAddRichInfo,
                                    ChargedProtoParticleAddMuonInfo,
                                    ChargedProtoCombineDLLsAlg )

        # Runs these to make sure ProtoParticles have full information available
        #addrich = ChargedProtoParticleAddRichInfo("CheckChargedProtoPAddRich")
        #addmuon = ChargedProtoParticleAddMuonInfo("CheckChargedProtoPAddMuon")
        #combine = ChargedProtoCombineDLLsAlg("CheckChargedProtoPCombDLLs")

        # The ntuple maker
        protoChecker = ChargedProtoParticleTupleAlg("ChargedProtoTuple")
        protoChecker.NTupleLUN = "PROTOTUPLE"

        # Fill sequence
        #protoSeq.Members += [addrich,addmuon,combine]
        protoSeq.Members += [protoChecker]

        # The output ntuple ROOT file
        NTupleSvc().Output += ["PROTOTUPLE DATAFILE='protoparticles.tuples.root' TYP='ROOT' OPT='NEW'"]

        # ANN training ntuple
        #from Configurables import ( ChargedProtoANNPIDTrainingTuple )
        #annTuple = ChargedProtoANNPIDTrainingTuple("ChargedProtoPIDANNTuple")
        #annTuple.NTupleLUN = "ANNPIDTUPLE"
        #protoSeq.Members += [annTuple]
        #NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.tuples.root' TYP='ROOT' OPT='NEW'"]

        # Set output levels
        if self.isPropertySet("OutputLevel"):
            level = self.getProp("OutputLevel")
            #addrich.OutputLevel = level
            #addmuon.OutputLevel = level
            #combine.OutputLevel = level
            protoChecker.OutputLevel = level
            
