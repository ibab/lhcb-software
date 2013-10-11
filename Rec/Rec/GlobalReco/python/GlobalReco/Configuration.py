
## @package GlobalReco
#  Base configurables for the Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.12 2010-03-15 16:47:36 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
from Configurables import ChargedProtoANNPIDConf

# ----------------------------------------------------------------------------------

## @class GlobalRecoConf
#  Configurable for the Global PID reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class GlobalRecoConf(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ ChargedProtoANNPIDConf ]
    
    ## Options
    __slots__ = { "SpecialData"  : []
                  ,"Context":    "Offline"   # The context within which to run
                  ,"RecoSequencer" : None    # The sequencer to use
                  ,"OutputLevel" : INFO      # The printout level to use
                  ,"TrackTypes"  : [ "Long", "Upstream", "Downstream" ]
                  ,"TrackCuts"   : {  "Long"       : { "Chi2Cut" : [0,5] }
                                     ,"Upstream"   : { "Chi2Cut" : [0,5] }
                                     ,"Downstream" : { "Chi2Cut" : [0,5] }
                                   }
                  ,"AddANNPIDInfo" : True
                  ,"DataType"     : "" # Type of data, propagated from application
                  ,"NoSpdPrs"     : False # Upgrade configuration with no SPd/Prs
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
        velo = ChargedProtoParticleAddVeloInfo("ChargedProtoPAddVeloDEDX")
        # Fill the Combined DLL information in the charged protoparticles
        combine = ChargedProtoCombineDLLsAlg("ChargedProtoPCombDLLs")
        # Fill the sequence
        cseq.Members += [ charged,ecal,brem,hcal]
        if not self.getProp("NoSpdPrs") :
            prs  = ChargedProtoParticleAddPrsInfo("ChargedProtoPAddPrs")
            spd  = ChargedProtoParticleAddSpdInfo("ChargedProtoPAddSpd")
            cseq.Members += [prs,spd ]
        cseq.Members += [ velo, rich,muon,combine ]
        
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
            rich.OutputLevel    = level
            muon.OutputLevel    = level
            ecal.OutputLevel    = level
            brem.OutputLevel    = level
            hcal.OutputLevel    = level
            if not self.getProp("NoSpdPrs") :
                prs.OutputLevel     = level
                spd.OutputLevel     = level
            velo.OutputLevel    = level
            combine.OutputLevel = level
            neutral.OutputLevel = level

        # ANN PID
        if self.getProp("AddANNPIDInfo") :
            nnpidseq = GaudiSequencer("ANNGPIDSeq")
            cseq.Members += [nnpidseq]
            annconf = ChargedProtoANNPIDConf()
            self.setOtherProps(annconf,["DataType","OutputLevel","Context"])
            annconf.RecoSequencer = nnpidseq

## @class GlobalRecoChecks
#  Configurable for the Global PID reconstruction MC based checking
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   26/02/2010
class GlobalRecoChecks(LHCbConfigurableUser):
    
    ## Options
    __slots__ = { "Sequencer"   : None    # The sequencer to add monitors to
                 ,"OutputLevel" : INFO    # The printout level to use
                 ,"Context":    "Offline" # The context within which to run
                 ,"AddANNPIDInfo" : True
                 ,"ProtoTupleName" : "protoparticles.tuples.root"
                 ,"ANNTupleName"   : "ProtoPIDANN.tuples.root"
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

        # The ntuple maker
        protoChecker = ChargedProtoParticleTupleAlg("ChargedProtoTuple")
        protoChecker.NTupleLUN = "PROTOTUPLE"

        # Fill sequence
        protoSeq.Members += [protoChecker]

        # The output ntuple ROOT file
        NTupleSvc().Output += ["PROTOTUPLE DATAFILE='"+self.getProp("ProtoTupleName")+"' TYP='ROOT' OPT='NEW'"]

        # ANN training ntuple
        if self.getProp("AddANNPIDInfo") :
            from Configurables import ( ANNGlobalPID__ChargedProtoANNPIDTrainingTuple,
                                        ANNGlobalPID__ChargedProtoANNPIDTupleTool )
            annTuple = ANNGlobalPID__ChargedProtoANNPIDTrainingTuple("ChargedProtoPIDANNTuple")
            annTuple.addTool( ANNGlobalPID__ChargedProtoANNPIDTupleTool, name = "Tuple" )
            annTuple.Tuple.NTupleLUN = "ANNPIDTUPLE"
            protoSeq.Members += [annTuple]
            NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='"+self.getProp("ANNTupleName")+"' TYP='ROOT' OPT='NEW'"]
            if self.isPropertySet("OutputLevel"):
                annTuple.OutputLevel = self.getProp("OutputLevel")

        # Set output levels
        if self.isPropertySet("OutputLevel"):
            level = self.getProp("OutputLevel")
            protoChecker.OutputLevel = level
            
