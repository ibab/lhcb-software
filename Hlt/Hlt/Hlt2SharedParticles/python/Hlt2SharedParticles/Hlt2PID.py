### @file
#
#  Hlt PID. Two (so far one) configurables and then generic implementation methods
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2009-08-03
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.8 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from RichRecSys.Configuration import RichRecSysConf
###################################################################################
#
# Hlt2 PID
#
class Hlt2PID(LHCbConfigurableUser):
    __slots__ = {
         "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
       , "Hlt2Tracks"                 : "Hlt/Track/Long"
       , "Prefix"                     : "Hlt"     # default prefix for all instance names (but common)
       , "UseRICH"                    : False 
         }
###################################################################################
    #
    # Charged ProtoParticles
    #
    def hlt2ChargedCaloProtos(self):
        """
        Charged protoparticles from Calo (=electrons)
        """
        return _hlt2ChargedCaloProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                     prefix = self.getProp("Prefix"))
###################################################################################
    #
    def hlt2ChargedHadronProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return _hlt2ChargedHadronProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                       prefix = self.getProp("Prefix"),
                                       UseRICH=False )
###################################################################################
    #
    def hlt2MuonProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return _hlt2MuonProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                              DataType = self.getProp("DataType"),
                              prefix = self.getProp("Prefix") )
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        return _hlt2NeutralProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"))
###################################################################################
    #
    # Tracking sequence
    #
    def hlt2Tracking(self):
        """
        Tracks
        """
        return _hlt2NeutralProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"))
        
#########################################################################################
#########################################################################################
#
# Actual implementation of the functions
#
#########################################################################################
#########################################################################################
#
# Electron Protos
#
def _hlt2ChargedCaloProtos(Hlt2Tracks = "Hlt/Track/Long",
                          prefix="Hlt" ):
    """
    Charged Calo protoparticles = electrons
    Requires chargedProtos
    """
    from Configurables import ( ChargedProtoParticleAddEcalInfo,
                                ChargedProtoParticleAddBremInfo,
                                ChargedProtoParticleAddPrsInfo,
                                ChargedProtoParticleAddSpdInfo,
#                                ChargedProtoParticleAddVeloInfo,
                                ChargedProtoCombineDLLsAlg )
    ecal = ChargedProtoParticleAddEcalInfo(prefix+"ChargedProtoPAddEcal")
    brem = ChargedProtoParticleAddBremInfo(prefix+"ChargedProtoPAddBrem")
    prs  = ChargedProtoParticleAddPrsInfo(prefix+"ChargedProtoPAddPrs")
    spd  = ChargedProtoParticleAddSpdInfo(prefix+"ChargedProtoPAddSpd")
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(prefix+"ChargedProtoPCombDLLsAfterCalo")
    # Fill the sequence
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix)
    from HltLine.HltLine import bindMembers
    return bindMembers( None, [ chargedProtos,ecal,brem,prs,spd,combine ])
    
#########################################################################################
#
# Muon Protos
#
def _hlt2MuonProtos(Hlt2Tracks = "Hlt/Track/Long",
                   DataType = "2009", 
                   prefix="Hlt" ):
    """
    Charged muon protoparticles
    Requires chargedProtos and muon ID
    """
    from Configurables import ( ChargedProtoParticleAddMuonInfo,
                                ChargedProtoCombineDLLsAlg )
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix)
    muon = ChargedProtoParticleAddMuonInfo(prefix+"ChargedProtoPAddMuon")
    muon.InputMuonPIDLocation = prefix+"/Muon/MuonPID"
    # Fill the Combined DLL information in the charged protoparticles
    #    combine = ChargedProtoCombineDLLsAlg(prefix+"ChargedProtoPCombDLLsAfterMuon")
    muonID = _hlt2Muon(Hlt2Tracks, DataType, prefix)
    from HltLine.HltLine import bindMembers
    return bindMembers( None, [ muonID, chargedProtos, muon ] )  # combine (not needed)

#########################################################################################
#
# Charged ProtoParticles
# Does not necessarily use RICH
#
def _hlt2ChargedHadronProtos(Hlt2Tracks = "Hlt/Track/Long",
                            prefix="Hlt",
                            UseRICH=False  ):
    """
    Charged hadron protoparticles = pion, kaon, proton
    If RICH is on, then requires rich, calo and does dll combination
    This is off by default
    Requires chargedProtos
    """
    from Configurables import ( ChargedProtoParticleAddMuonInfo,
                                ChargedProtoParticleAddHcalInfo,
                                ChargedProtoCombineDLLsAlg )
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix)
    from HltLine.HltLine import bindMembers
    if ( UseRICH ):
        # @todo : add RICH reco as dependency
        rich = ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich")
        hcal = ChargedProtoParticleAddHcalInfo(prefix+"ChargedProtoPAddHcal")
        combine = ChargedProtoCombineDLLsAlg(prefix+"ChargedProtoPCombDLLsAfterRich")
        caloID = _hlt2Calo(Hlt2Tracks, prefix)
        return bindMembers( None , [ caloID, chargedProtos, hcal, rich, combine ] )
    else :
        return bindMembers( None , [ chargedProtos ] )
    
#########################################################################################
#
# Charged ProtoParticles
#
def _hlt2ChargedProtos(Hlt2Tracks = "Hlt/Track/Long",
                      prefix="Hlt" ):
    """
    Charged protoparticles
    
    I don't think a prefix different to Hlt works.
    Requires tracks
    """
    protos = prefix+"/ProtoP/Charged"
    
    from Configurables import ( ChargedProtoParticleMaker,
                                DelegatingTrackSelector  )
    
    charged = ChargedProtoParticleMaker(prefix+'ChargedProtoPAlg')
    charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
    charged.TrackSelector.TrackTypes = [ "Long" ]  # only long so far
    
    charged.InputTrackLocation = Hlt2Tracks
    charged.OutputProtoParticleLocation = protos

    from HltLine.HltLine import bindMembers
    tracks = _hlt2Tracking(Hlt2Tracks, prefix)
    return bindMembers ( None, [ tracks , charged ] )

#########################################################################################
#
# Neutral ProtoParticles
#
def _hlt2NeutralProtos(Hlt2Tracks = "Hlt/Track/Long",
                      prefix="Hlt" ):
    """
    Neutral protoparticles 
    Requires caloID
    """
    from Configurables import NeutralProtoPAlg
    HltNeutralProtoPAlg = NeutralProtoPAlg(prefix+'NeutralProtoPAlg')
    # Overwrite some default offline settings with HLT special settings (
    # taken from CaloReco.opts)
    HltNeutralProtoPAlg.PhotonIDName = "HltPhotonPID"
    from Configurables import CaloPhotonEstimatorTool
    ToolSvc().addTool(CaloPhotonEstimatorTool, name="HltPhotonPID")
    ToolSvc().HltPhotonPID.TableLocation = "Hlt/Calo/ClusterMatch"
    importOptions( "$CALOPIDSROOT/options/HltPhotonPDF.opts" )
    
    caloID = _hlt2Calo(Hlt2Tracks, prefix)
    
    from HltLine.HltLine import bindMembers
    return bindMembers ( None, [ caloID, HltNeutralProtoPAlg ]) 
    
#########################################################################################
#
# MuonID
#
def _hlt2Muon(Hlt2Tracks = "Hlt/Track/Long"
           , DataType = '2009'
           , prefix="Hlt"):
    """
    Muon ID options 
    Requires tracks
    """
    from MuonID import ConfiguredMuonIDs
    from Configurables import MuonRec, MuonIDAlg
    cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=DataType)
    HltMuonIDAlg = cm.configureMuonIDAlg(prefix+"MuonIDAlg")
    HltMuonIDAlg.TrackLocation     = Hlt2Tracks
    HltMuonIDAlg.MuonIDLocation    = prefix+"/Muon/MuonPID"    # output
    HltMuonIDAlg.MuonTrackLocation = prefix+"/Track/Muon"
    # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
    #       a factor 3-4
    HltMuonIDAlg.FindQuality = False
    tracks = _hlt2Tracking(Hlt2Tracks, prefix)
    
    from HltLine.HltLine import bindMembers
    return bindMembers ( None, [ tracks, MuonRec(), HltMuonIDAlg ] )
#########################################################################################
#
# Calo ID
#
def _hlt2Calo(Hlt2Tracks = "Hlt/Track/Long"
           , prefix="Hlt"):
    """
    Calo ID
    Does calo reconstruction and Calo ID
    Requires tracks
    @todo : make this track location dependent
    """
    from CaloReco.Configuration import HltCaloRecoConf 
    from CaloPIDs.Configuration import HltCaloPIDsConf
    Hlt2CaloRecoSeq = GaudiSequencer("Hlt2CaloRecoSeq")
    Hlt2CaloPIDsSeq = GaudiSequencer("Hlt2CaloPIDsSeq")
    caloReco = HltCaloRecoConf(Sequence           = Hlt2CaloRecoSeq,
                               Context            = "HLT")
    caloPID  = HltCaloPIDsConf(Sequence           = Hlt2CaloPIDsSeq,
                               Context            = "HLT")
    Hlt2CaloSeq = GaudiSequencer("Hlt2CaloSeq", Context = "HLT" )
    from HltLine.HltLine import bindMembers
    tracks = _hlt2Tracking(Hlt2Tracks, prefix)
    return bindMembers( None, [ tracks, Hlt2CaloRecoSeq, Hlt2CaloPIDsSeq   ] )

#########################################################################################
#
# Track reconstruction
# @todo : move all Hlt2 stuff from HltReco to here
#
def _hlt2Tracking(Hlt2Tracks = "Hlt/Track/Long"
                , prefix="Hlt"):
    """
    Track reconstruction
    @todo allow to set tracks in reco sequence
    """
    from HltLine.HltLine import bindMembers
    from HltLine.HltReco import HltRecoSequence    
    #
    # Hacking of errors
    # @todo Needs to be revised
    #
    from Configurables import HltInsertTrackErrParam
    HltInsertTrackErrParam = HltInsertTrackErrParam()
    HltInsertTrackErrParam.InputLocation = Hlt2Tracks
    #
    # Reco sequence
    #
    return bindMembers( None, [ HltRecoSequence, HltInsertTrackErrParam  ] )
