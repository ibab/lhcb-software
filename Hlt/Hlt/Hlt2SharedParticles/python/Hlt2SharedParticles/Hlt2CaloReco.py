"""
 Script to Hlt calo reco sequence

 Translated from $CALORECORROT/options/Hlt*.opts 

 @author P. Koppenburg
 @date 2009-07-31
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.4 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
from HltLine.HltLine import bindMembers
###################################################################################
#
# Hlt2 PID
#
class Hlt2CaloReco(LHCbConfigurableUser):
    __used_configurables__ = [  ]
    __slots__ = {
         "DataType"                   : "2009"    # datatype is one of 2009, MC09, DC06...
       , "Hlt2Tracks"                 : "Hlt/Track/Long"
       , "Prefix"                     : "Hlt"     # default prefix for all instance names (but common)
         }  

###################################################################################
    #
    # Calo
    #
    def hlt2Calo(self):
        """
        Calorimeter options
        """
        caloSeq = Sequence("HltRecoCALOSeq", Context = "HLT", IgnoreFilterPassed = True ) 
        
        ## Options for Calo reconstruction
        caloSeq.Members += [ self.commonCaloOptions() ]
                                
        Hlt2Tracks = [ self.getProp("Hlt2Tracks") ]
        prefix = self.getProp("Prefix")
        
        #------ Track-Cluster 2D matching for Neutral/Charged cluster selection
        caloSeq.Members += [ self.hltCaloTrackMatch(Hlt2Tracks,prefix) ]
        
        #------ Merged Pi0, Single Photon & Electron reconstruction
        caloSeq.Members += [ self.hltCaloRec(prefix) ]
        
        #------- Track- charged/neutral cluster 3D matching for electron/bremStrahlung evaluation
        caloSeq.Members += [ self.hltCaloPIDs(Hlt2Tracks,prefix) ]
    
        return caloSeq 
   
###############################################################################
    #
    def hltCaloTrackMatch(self,Hlt2Tracks,prefix):
        """
        Track-Cluster 2D matching for Neutral/Charged cluster selection
        """
        HltCaloTrackMatch =  Sequence(prefix+"CaloTrackMatch")
        from Configurables import PhotonMatchAlg
        HltClusterMatch = PhotonMatchAlg(prefix+"ClusterMatch")
        HltCaloTrackMatch.Members += [ self.hltCaloAcceptance(Hlt2Tracks,prefix) ,
                                       HltClusterMatch   ] 
        HltClusterMatch.Tracks = Hlt2Tracks
        if (prefix!="Hlt"): HltClusterMatch.Output = prefix+"/Calo/PhotonMatch"
        # else, use default from algorithm + context()
        
        return HltCaloTrackMatch
    
###############################################################################
    #
    def hltCaloAcceptance(self,Hlt2Tracks,prefix):
        """
        Track-Cluster 2D matching for Neutral/Charged cluster selection
        """
        HltCaloAcceptance = Sequence(prefix+"CaloAcceptance")
        # Special online setting : use fast extrapolator for first alg.
        from Configurables import InSpdAcceptanceAlg, InSpdAcceptance, InPrsAcceptanceAlg
        from Configurables import InEcalAcceptanceAlg, InHcalAcceptanceAlg, InBremAcceptanceAlg
        InSpdAcceptanceAlg(prefix+"InSPD").Tool = "InSpdAcceptance/HltInSpd:PUBLIC"
        ToolSvc().addTool(InSpdAcceptance, name =prefix+"InSpd")
        ToolSvc().HltInSpd.Extrapolator = "TrackHerabExtrapolator"
        
        HltCaloAcceptance.Members = [ InSpdAcceptanceAlg(prefix+"InSPD") ,
                                      InPrsAcceptanceAlg(prefix+"InPRS") ,
                                      InEcalAcceptanceAlg(prefix+"InECAL"), 
                                      InHcalAcceptanceAlg(prefix+"InHCAL") ,
                                      InBremAcceptanceAlg(prefix+"InBREM") ]
        InSpdAcceptanceAlg(prefix+"InSPD").Inputs   =     Hlt2Tracks
        if (prefix!="Hlt"): InSpdAcceptanceAlg(prefix+"InSPD").Output = prefix+"/Calo/InAccSpd"
        InPrsAcceptanceAlg(prefix+"InPRS").Inputs   =     Hlt2Tracks
        if (prefix!="Hlt"): InPrsAcceptanceAlg(prefix+"InPRS").Output = prefix+"/Calo/InAccPrs"
        InEcalAcceptanceAlg(prefix+"InECAL").Inputs =     Hlt2Tracks
        if (prefix!="Hlt"): InEcalAcceptanceAlg(prefix+"InECAL").Output = prefix+"/Calo/InAccEcal"
        InHcalAcceptanceAlg(prefix+"InHCAL").Inputs =     Hlt2Tracks
        if (prefix!="Hlt"): InHcalAcceptanceAlg(prefix+"InHCAL").Output = prefix+"/Calo/InAccHcal"
        InBremAcceptanceAlg(prefix+"InBREM").Inputs =     Hlt2Tracks
        if (prefix!="Hlt"): InBremAcceptanceAlg(prefix+"InBREM").Output = prefix+"/Calo/InAccBrem"
        
        return HltCaloAcceptance
    
###############################################################################
    #
    def hltCaloRec(self,prefix):
        """
        Merged Pi0, Single Photon & Electron reconstruction
        """
        HltCaloRec = Sequence("HltCaloRec")
        from Configurables import CaloMergedPi0Alg, CaloHypoAlg, CaloSinglePhotonAlg, CaloElectronAlg
        HltCaloRec.Members += [ self.hltMergedPi0Rec(prefix), 
                                self.hltPhotonFromMergedRec(prefix),
                                self.hltSinglePhotonRec(prefix),
                                self.hltElectronRec(prefix)
                               ]
        
        # Parameters
        #        
        importOptions( "$CALORECOOPTS/CaloECorrectionParam.opts" )
        importOptions( "$CALORECOOPTS/CaloSCorrectionParam.opts" )
        importOptions( "$CALORECOOPTS/CaloLCorrectionParam.opts" )
        
        return HltCaloRec
###############################################################################
    #
    def hltMergedPi0Rec(self,prefix):
        """
        Settings for pi0
        """
        from Configurables import CaloMergedPi0Alg
        importOptions( "$CALORECOOPTS/HltMergedPi0Rec.opts" ) # just numbers, better keep under Calo control
        return CaloMergedPi0Alg("HltMergedPi0Rec")
    
###############################################################################
    #
    def hltElectronRec(self,prefix):
        """
        Settings for electrons
        """
        from Configurables import CaloElectronAlg
        HltElectronRec = CaloElectronAlg(prefix+"ElectronRec")
        HltElectronRec.HypoTools        = [ "CaloExtraDigits/SpdPrsExtraE"  ] 
        HltElectronRec.CorrectionTools2   = [ "CaloECorrection/ECorrection" ] 
        HltElectronRec.CorrectionTools2  += [ "CaloSCorrection/SCorrection" ] 
        HltElectronRec.CorrectionTools2  += [ "CaloLCorrection/LCorrection" ] 
        HltElectronRec.SelectionTools           =  [ 
            "CaloSelectCluster/"+prefix+"ElectronClusterSelect",
            "CaloSelectorNOT/"+prefix+"ChargedCluster",
            "CaloSelectNeutralClusterWithSpd/"+prefix+"ClusterWithSpd",
            "CaloSelectClusterWithPrs/"+prefix+"ClusterWithPrs"
            ]
        from Configurables import CaloSelectCluster, CaloSelectorNOT, CaloSelectNeutralClusterWithSpd
        from Configurables import CaloSelectClusterWithPrs, CaloSelectNeutralClusterWithTracks
        ToolSvc().addTool(CaloSelectorNOT, name =prefix+"ChargedCluster")
        ToolSvc().HltChargedCluster.SelectorTools =  [ "CaloSelectNeutralClusterWithTracks/"+prefix+"NotNeutralCluster"] 
        ToolSvc().addTool(CaloSelectNeutralClusterWithTracks, name =prefix+"NotNeutralCluster")
        ToolSvc().HltNotNeutralCluster.MinChi2 = 25 
        ToolSvc().HltNotNeutralCluster.Table = prefix+"/Calo/ClusterMatch"
        ToolSvc().addTool(CaloSelectClusterWithPrs,name=prefix+"ClusterWithPrs")
        from GaudiKernel.SystemOfUnits import MeV
        ToolSvc().HltClusterWithPrs.MinEnergy = 10.*MeV
        ToolSvc().addTool(CaloSelectCluster, name =prefix+"ElectronClusterSelect")
        ToolSvc().HltElectronClusterSelect.MinEnergy = 0
        
        return HltElectronRec
        
###############################################################################
    #
    def hltSinglePhotonRec(self,prefix):
        """
        Hypo tools for single photon
        """
        from Configurables import CaloSinglePhotonAlg
        HltSinglePhotonRec = CaloSinglePhotonAlg("HltSinglePhotonRec")
        HltSinglePhotonRec.HypoTools          = [ "CaloExtraDigits/SpdPrsExtraG" ] 
        HltSinglePhotonRec.CorrectionTools2   = [ "CaloECorrection/ECorrection"  ] 
        HltSinglePhotonRec.CorrectionTools2  += [ "CaloSCorrection/SCorrection"  ] 
        HltSinglePhotonRec.CorrectionTools2  += [ "CaloLCorrection/LCorrection"  ]
        HltSinglePhotonRec.SelectionTools   = [ 
            "CaloSelectCluster/"+prefix+"PhotonClusterSelect" ,
            "CaloSelectNeutralClusterWithTracks/"+prefix+"NeutralCluster"
            ] 
        from Configurables import CaloSelectCluster, CaloSelectNeutralClusterWithTracks
        ToolSvc().addTool(CaloSelectCluster, name =prefix+"PhotonClusterSelect")
        ToolSvc().HltPhotonClusterSelect.MinEnergy = 0
        ToolSvc().addTool(CaloSelectNeutralClusterWithTracks, name =prefix+"NeutralCluster")
        ToolSvc().HltNeutralCluster.MinChi2 = 4 
        ToolSvc().HltNeutralCluster.Table = prefix+"/Calo/ClusterMatch"

        return HltSinglePhotonRec
        
###############################################################################
    #
    def hltPhotonFromMergedRec(self,prefix):
        """
        Hypo tools for merged photon

        @todo Need prefix?
        """
        from Configurables import CaloHypoAlg
        HltPhotonFromMergedRec = CaloHypoAlg("HltPhotonFromMergedRec")
        HltPhotonFromMergedRec.InputData =   "Hlt/Calo/SplitPhotons"        
        HltPhotonFromMergedRec.Tools    += [ "CaloECorrection/ECorrection" ]   # Ecorrection
        HltPhotonFromMergedRec.Tools    += [ "CaloSCorrection/SCorrection" ]   # Scorrection
        HltPhotonFromMergedRec.Tools    += [ "CaloLCorrection/LCorrection" ]   # Lcorrection
        return HltPhotonFromMergedRec
                
###############################################################################
    #
    def hltCaloPIDs(self,Hlt2Tracks,prefix):
        """
        Track- charged/neutral cluster 3D matching for electron/bremStrahlung evaluation
        """
        HltCaloMatch = Sequence(prefix+"CaloMatch")
        from Configurables import ElectronMatchAlg, BremMatchAlg
        HltCaloMatch.Members += [ ElectronMatchAlg(prefix+"ElectronMatch"),
                                  BremMatchAlg(prefix+"BremMatch") ]    
        ElectronMatchAlg(prefix+"ElectronMatch").Tracks = Hlt2Tracks
        if (prefix!="Hlt"):
            ElectronMatchAlg(prefix+"ElectronMatch").Output = prefix+"/Calo/ElectronMatch"
        BremMatchAlg(prefix+"BremMatch").Tracks =         Hlt2Tracks
        
        HltCaloEnergy = Sequence(prefix+"CaloEnergy")
        from Configurables import Track2SpdEAlg, Track2PrsEAlg, Track2EcalEAlg, Track2HcalEAlg
        HltCaloEnergy.Members =   [  Track2SpdEAlg(prefix+"SpdE"), 
                                     Track2PrsEAlg(prefix+"PrsE"), 
                                     Track2EcalEAlg(prefix+"EcalE"), 
                                     Track2HcalEAlg(prefix+"HcalE") ]
        Track2SpdEAlg(prefix+"SpdE").Inputs =             Hlt2Tracks
        Track2PrsEAlg(prefix+"PrsE").Inputs =             Hlt2Tracks
        Track2EcalEAlg(prefix+"EcalE").Inputs =           Hlt2Tracks
        Track2HcalEAlg(prefix+"HcalE").Inputs =           Hlt2Tracks
        HltCaloDLLs = Sequence(prefix+"CaloDLLs")
        
        from Configurables import EcalChi22ID, BremChi22ID, ClusChi22ID, PrsPIDeAlg
        from Configurables import BremPIDeAlg, EcalPIDeAlg, HcalPIDeAlg, EcalPIDmuAlg, HcalPIDmuAlg
        HltCaloDLLs.Members +=   [   EcalChi22ID(prefix+"EcalChi22ID"), 
                                     BremChi22ID(prefix+"BremChi22ID"), 
                                     ClusChi22ID(prefix+"ClusChi22ID"), 
                                     PrsPIDeAlg(prefix+"PrsPIDe" ), 
                                     BremPIDeAlg(prefix+"BremPIDe"),  
                                     EcalPIDeAlg(prefix+"EcalPIDe" ), 
                                     HcalPIDeAlg(prefix+"HcalPIDe" ), 
                                     EcalPIDmuAlg(prefix+"EcalPIDmu"),  
                                     HcalPIDmuAlg(prefix+"HcalPIDmu")  ]
        EcalChi22ID(prefix+"EcalChi22ID").Tracks =        Hlt2Tracks
        BremChi22ID(prefix+"BremChi22ID").Tracks =        Hlt2Tracks
        ClusChi22ID(prefix+"ClusChi22ID").Tracks =        Hlt2Tracks

        # refine outputs (not needed for defaulr)
        if (prefix!="Hlt"):
            BremMatchAlg(prefix+"BremMatch").Output = prefix+"/Calo/BremMatch"
            Track2PrsEAlg(prefix+"PrsE").Ouput = prefix+"/Calo/PrsE"
            Track2EcalEAlg(prefix+"EcalE").Ouput = prefix+"/Calo/EcalE"
            Track2HcalEAlg(prefix+"HcalE").Ouput = prefix+"/Calo/HcalE"
            Track2SpdEAlg(prefix+"SpdE").Ouput = prefix+"/Calo/SpdE"
            EcalChi22ID(prefix+"EcalChi22ID").Output = prefix+"/Calo/EcalChi2"
            EcalChi22ID(prefix+"EcalChi22ID").Output = prefix+"/Calo/BremChi2"
            EcalChi22ID(prefix+"EcalChi22ID").Output = prefix+"/Calo/ClusChi2"
            PrsPIDeAlg(prefix+"PrsPIDe").Input = prefix+"/Calo/PrsE"
            PrsPIDeAlg(prefix+"PrsPIDe").Output = prefix+"/Calo/PrsPIDe"
            BremPIDeAlg(prefix+"BremPIDe").Input = prefix+"/Calo/BremChi2"
            BremPIDeAlg(prefix+"BremPIDe").Output = prefix+"/Calo/BremPIDe"
            EcalPIDeAlg(prefix+"EcalPIDe").Input = prefix+"/Calo/EcalChi2"
            EcalPIDeAlg(prefix+"EcalPIDe").Output = prefix+"/Calo/EcalPIDe"
            HcalPIDeAlg(prefix+"HcalPIDe").Input = prefix+"/Calo/HcalE"
            HcalPIDeAlg(prefix+"HcalPIDe").Output = prefix+"/Calo/HcalPIDe"
            EcalPIDmuAlg(prefix+"EcalPIDmu").Input = prefix+"/Calo/EcalE"
            EcalPIDmuAlg(prefix+"EcalPIDmu").Output = prefix+"/Calo/EcalPIDmu"
            HcalPIDmuAlg(prefix+"HcalPIDmu").Input = prefix+"/Calo/HcalE"
            HcalPIDmuAlg(prefix+"HcalPIDmu").Output = prefix+"/Calo/HcalPIDmu"
            
            
        HltCaloPIDs = Sequence(prefix+"CaloPIDs")
        HltCaloPIDs.Members +=    [  HltCaloMatch,
                                     HltCaloEnergy,
                                     HltCaloDLLs ]
        
        importOptions( "$CALORECOOPTS/CaloPIDs.opts" )
        
        return HltCaloPIDs
    
###############################################################################
    #
    def commonCaloOptions(self):
        """
        Calorimeter options, needed only once
        """
        from HltLine.HltDecodeRaw import DecodeECAL, DecodeSPD, DecodePRS, DecodeHCAL
        HltCaloClusters = Sequence("HltCaloClusters")
        HltCaloClusters.Members = DecodeECAL.members() + DecodeSPD.members() + DecodePRS.members() + DecodeHCAL.members() 
        
        from Configurables import CaloGetterTool
        ToolSvc().addTool(CaloGetterTool, name = "CaloGetter" )
        ToolSvc().CaloGetter.Context = "HLT"

        # The main stream 
        # almost the same as in Brunel : just change the instance name and the default locations + CaloDigits on-Demand
        importOptions( "$CALODAQROOT/options/CaloDigitsOnDemand.opts" )
        
        from Configurables import CaloGetterInit
        HltCaloClusters.Members += [ CaloGetterInit("CaloDigitGetter") ] # for the time being

        #------ Clusterize, clean & compute cluster parameters
        from Configurables import CellularAutomatonAlg, CaloSharedCellAlg, CaloClusterCovarianceAlg, CaloClusterCorrect3x3Position
        HltCaloClusters.Members += [  CellularAutomatonAlg("EcalHltClust"),
                                      CaloSharedCellAlg("EcalHltShare"),
                                      CaloClusterCovarianceAlg("EcalHltCovar"),
                                      CaloClusterCorrect3x3Position("EcalHltPosition")]
        
        return HltCaloClusters
    
