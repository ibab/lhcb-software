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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from HltLine.HltLine import bindMembers
from RichRecSys.Configuration import RichRecSysConf
###################################################################################
#
# Hlt2 PID
#
class Hlt2PID(LHCbConfigurableUser):
    __used_configurables__ = [  RichRecSysConf ]
    __slots__ = {
         "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
       , "Hlt2Tracks"                 : "Hlt/Track/Long"
         }

###################################################################################
    #
    # MuonID
    #
    def hlt2Muon(self):
        """
        Muon ID options
        """
        from MuonID import ConfiguredMuonIDs
        from Configurables import MuonRec, MuonIDAlg
        cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
        HltMuonIDAlg = cm.configureMuonIDAlg("HltMuonIDAlg")
        HltMuonIDAlg.TrackLocation = self.getProp("Hlt2Tracks") 
        HltMuonIDAlg.MuonIDLocation = "Hlt/Muon/MuonPID"
        HltMuonIDAlg.MuonTrackLocation = "Hlt/Track/Muon"
        
        return bindMembers ( None, [ MuonRec(), HltMuonIDAlg] )
    
    
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        from Configurables import NeutralProtoPAlg
        HltNeutralProtoPAlg = NeutralProtoPAlg('HltNeutralProtoPAlg')
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
        charged protoparticles 
        """
        protos = "Hlt/ProtoP/Charged"
        tracks = "Hlt/Track/Long"
        
        from Configurables import ChargedProtoPAlg, ChargedProtoCombineDLLsAlg, TrackSelector
        SeqHlt2ChargedProtos = GaudiSequencer("Hlt2ChargedProtos", Context ="HLT")
        
        Hlt2ChargedProtoPAlg = ChargedProtoPAlg('Hlt2ChargedProtoPAlg')
        Hlt2ChargedProtoPAlg.InputTrackLocation = tracks
        Hlt2ChargedProtoPAlg.OutputProtoParticleLocation = protos
        Hlt2ChargedProtoPAlg.addTool( TrackSelector, name="TrackSelector")
        Hlt2ChargedProtoPAlg.TrackSelector.AcceptClones = False
        Hlt2ChargedProtoPAlg.InputMuonPIDLocation = "Hlt/Muon/MuonPID"
        ## Calo PID
        Hlt2ChargedProtoPAlg.UseCaloSpdPID = True 
        Hlt2ChargedProtoPAlg.UseCaloPrsPID = True 
        Hlt2ChargedProtoPAlg.UseCaloEcalPID = True 
        Hlt2ChargedProtoPAlg.UseCaloHcalPID = True 
        Hlt2ChargedProtoPAlg.UseCaloBremPID = True 
        ##Hlt2ChargedProtoPAlg.UseRichPID = False  // Protos will NOT have any RICH information - HltRichPIDsKaons will not work
        Hlt2ChargedProtoPAlg.UseRichPID = True     ## Use this to add RICH info to the HLT protos, needed for HltRichPIDsKaons
        Hlt2ChargedProtoPAlg.UseMuonPID = True 
        Hlt2ChargedProtoPAlg.UseVeloPID = False
        
        Hlt2ChargedProtoCombDLL = ChargedProtoCombineDLLsAlg('Hlt2ChargedProtoCombDLL')
        Hlt2ChargedProtoCombDLL.ProtoParticleLocation = protos 

        return bindMembers ( None, [ Hlt2ChargedProtoPAlg, Hlt2ChargedProtoCombDLL ] )
