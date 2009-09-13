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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
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
        charged protoparticles 
        """
        prefix = self.getProp("Prefix")  
        protos = prefix+"/ProtoP/Charged"
        tracks = self.getProp("Hlt2Tracks") 
        
        from Configurables import ChargedProtoPAlg, ChargedProtoCombineDLLsAlg, TrackSelector
        SeqHlt2ChargedProtos = GaudiSequencer(prefix+"ChargedProtos", Context ="HLT")
        
        Hlt2ChargedProtoPAlg = ChargedProtoPAlg(prefix+'ChargedProtoPAlg')
        Hlt2ChargedProtoPAlg.InputTrackLocation = tracks
        Hlt2ChargedProtoPAlg.OutputProtoParticleLocation = protos
        Hlt2ChargedProtoPAlg.addTool( TrackSelector, name="TrackSelector")
        Hlt2ChargedProtoPAlg.TrackSelector.AcceptClones = False
        Hlt2ChargedProtoPAlg.InputMuonPIDLocation = prefix+"/Muon/MuonPID"
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
        if (prefix!='Hlt'): Hlt2ChargedProtoPAlg.NonStandardHltLocation = prefix
        
        Hlt2ChargedProtoCombDLL = ChargedProtoCombineDLLsAlg(prefix+'ChargedProtoCombDLL')
        Hlt2ChargedProtoCombDLL.ProtoParticleLocation = protos 

        from HltLine.HltLine import bindMembers
        return bindMembers ( None, [ Hlt2ChargedProtoPAlg, Hlt2ChargedProtoCombDLL ] )
