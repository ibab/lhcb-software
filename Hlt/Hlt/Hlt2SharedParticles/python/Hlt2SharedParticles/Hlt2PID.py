### @file
#
#  Basic particles
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2009-08-03
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.7 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from RichRecSys.Configuration import RichRecSysConf
###################################################################################
#
# Hlt2 PID
#
class Hlt2PID(LHCbConfigurableUser):
#    __used_configurables__ = [  RichRecSysConf ]  # !!!! If I uncomment that I get a circluar dependency. Why ?
    __slots__ = {
         "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
       , "Hlt2Tracks"                 : "Hlt/Track/Long"
       , "Prefix"                     : "Hlt"     # default prefix for all instance names (but common)
       , "UseRICH"                    : False 
         }

###################################################################################
    #
    # MuonID
    #
    def hlt2Muon(self):
        """
        Muon ID options
        """
        prefix = self.getProp("Prefix")
        from MuonID import ConfiguredMuonIDs
        from Configurables import MuonRec, MuonIDAlg
        cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
        HltMuonIDAlg = cm.configureMuonIDAlg(prefix+"MuonIDAlg")
        HltMuonIDAlg.TrackLocation     = self.getProp("Hlt2Tracks") 
        HltMuonIDAlg.MuonIDLocation    = prefix+"/Muon/MuonPID"    # output
        HltMuonIDAlg.MuonTrackLocation = prefix+"/Track/Muon"
        # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
        #       a factor 3-4
        HltMuonIDAlg.FindQuality = False
        
        from HltLine.HltLine import bindMembers
        return bindMembers ( None, [ MuonRec(), HltMuonIDAlg ] )
    
    
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        prefix = self.getProp("Prefix")  
        from Configurables import NeutralProtoPAlg
        HltNeutralProtoPAlg = NeutralProtoPAlg(prefix+'NeutralProtoPAlg')
        # Overwrite some default offline settings with HLT special settings (taken from CaloReco.opts)
        HltNeutralProtoPAlg.PhotonIDName = "HltPhotonPID"
        from Configurables import CaloPhotonEstimatorTool
        ToolSvc().addTool(CaloPhotonEstimatorTool, name="HltPhotonPID")
        ToolSvc().HltPhotonPID.TableLocation = "Hlt/Calo/ClusterMatch"
        importOptions( "$CALOPIDSROOT/options/HltPhotonPDF.opts" )

        return HltNeutralProtoPAlg
        
        
###################################################################################
    #
    # Charged ProtoParticles
    #
    def hlt2ChargedProtos(self):
        """
        Charged protoparticles

        @todo : do different things when rich is there. See GlobalReco
        I don't think a prefix different to Hlt works.
        """
        prefix = self.getProp("Prefix")  
        protos = prefix+"/ProtoP/Charged"
        tracks = self.getProp("Hlt2Tracks") 
        
        from Configurables import ( ChargedProtoParticleMaker,
                                    DelegatingTrackSelector,
                                    ChargedProtoParticleAddRichInfo,
                                    ChargedProtoParticleAddMuonInfo,
                                    ChargedProtoParticleAddEcalInfo,
                                    ChargedProtoParticleAddBremInfo,
                                    ChargedProtoParticleAddHcalInfo,
                                    ChargedProtoParticleAddPrsInfo,
                                    ChargedProtoParticleAddSpdInfo,
                                    ChargedProtoParticleAddVeloInfo,
                                    ChargedProtoCombineDLLsAlg )
#        SeqHlt2ChargedProtos = GaudiSequencer(prefix+"ChargedProtos", Context ="HLT")

        charged = ChargedProtoParticleMaker(prefix+'ChargedProtoPAlg')
        charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
        charged.TrackSelector.TrackTypes = [ "Long" ]  # only long so far
#        charged.TrackSelector.AcceptClones = False
        
        charged.InputTrackLocation = tracks
        charged.OutputProtoParticleLocation = protos
        pidList = [ charged ]
        
        # muon
        # Add PID information
        if ( self.getProp("UseRICH")):
            rich = ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich")
            pidList += [ rich ]
        muon = ChargedProtoParticleAddMuonInfo(prefix+"ChargedProtoPAddMuon")
        muon.InputMuonPIDLocation = prefix+"/Muon/MuonPID"
        ecal = ChargedProtoParticleAddEcalInfo(prefix+"ChargedProtoPAddEcal")
        brem = ChargedProtoParticleAddBremInfo(prefix+"ChargedProtoPAddBrem")
        hcal = ChargedProtoParticleAddHcalInfo(prefix+"ChargedProtoPAddHcal")
        prs  = ChargedProtoParticleAddPrsInfo(prefix+"ChargedProtoPAddPrs")
        spd  = ChargedProtoParticleAddSpdInfo(prefix+"ChargedProtoPAddSpd")
        # Fill the Combined DLL information in the charged protoparticles
        combine = ChargedProtoCombineDLLsAlg(prefix+"ChargedProtoPCombDLLs")
        # Fill the sequence
        pidList += [ muon,ecal,brem,hcal,prs,spd,combine ]

        from HltLine.HltLine import bindMembers
        return bindMembers ( None, pidList  )
