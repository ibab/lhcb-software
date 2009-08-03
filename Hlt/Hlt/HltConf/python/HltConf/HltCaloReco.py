"""
 Script to Hlt calo reco sequence

 Translated from $CALORECORROT/options/Hlt*.opts 

 @author P. Koppenburg
 @date 2009-07-31
"""
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import GaudiSequencer as Sequence
###################################################################################
#
# Calo
#
def hlt2Calo(HltTracks):
    """
    Calorimeter options
    """
    
    caloSeq = Sequence('HltRecoCALOSeq', Context = 'HLT', IgnoreFilterPassed = True ) 
    #------- Decoding
    from HltConf.HltDecodeRaw import DecodeECAL, DecodeSPD, DecodePRS, DecodeHCAL
    caloSeq.Members = DecodeECAL.members() + DecodeSPD.members() + DecodePRS.members() + DecodeHCAL.members() 

    ## Options for Calo reconstruction

    from Configurables import CaloGetterTool
    ToolSvc().addTool(CaloGetterTool, name = 'CaloGetter' )
    ToolSvc().CaloGetter.Context = "HLT"

    # The main stream 
    # almost the same as in Brunel : just change the instance name and the default locations + CaloDigits on-Demand
    importOptions( "$CALODAQROOT/options/CaloDigitsOnDemand.opts" )

    from Configurables import CaloGetterInit
    caloSeq.Members += [ CaloGetterInit("CaloDigitGetter") ] # for the time being
    
    # OBSOLETE (as it seems) # CaloDigits.Members = [  "CaloZSupAlg/EcalZSup",                 
    # OBSOLETE (as it seems) #                         "CaloZSupAlg/HcalZSup",                  
    # OBSOLETE (as it seems) #                         "CaloDigitsFromRaw/PrsFromRaw",        
    # OBSOLETE (as it seems) #                         "CaloDigitsFromRaw/SpdFromRaw"]
    
    #------ Clusterize, clean & compute cluster parameters
    caloSeq.Members += [ hltCaloClusters() ]
    
    #------ Track-Cluster 2D matching for Neutral/Charged cluster selection
    caloSeq.Members += [ hltCaloTrackMatch(HltTracks) ]

    #------ Merged Pi0, Single Photon & Electron reconstruction
    caloSeq.Members += [ hltCaloRec() ]
    
    #------- Track- charged/neutral cluster 3D matching for electron/bremStrahlung evaluation
    caloSeq.Members += [ hltCaloPIDs(HltTracks) ]

    
    return caloSeq 
   
###############################################################################
#
def hltCaloClusters():
    """
    Clusterize, clean & compute cluster parameters
    """
    from Configurables import CellularAutomatonAlg, CaloSharedCellAlg, CaloClusterCovarianceAlg, CaloClusterCorrect3x3Position
    HltCaloClusters = Sequence("HltCaloClusters")
    HltCaloClusters.Members += [ CellularAutomatonAlg("EcalHltClust"),
                                 CaloSharedCellAlg("EcalHltShare"),
                                 CaloClusterCovarianceAlg("EcalHltCovar"),
                                 CaloClusterCorrect3x3Position("EcalHltPosition")]

    return HltCaloClusters
     
###############################################################################
#
def hltCaloAcceptance(HltTracks):
    """
    Track-Cluster 2D matching for Neutral/Charged cluster selection
    """
    HltCaloAcceptance = Sequence("HltCaloAcceptance")
    #/ Special online setting : use fast extrapolator for first alg.
    from Configurables import InSpdAcceptanceAlg, InSpdAcceptance, InPrsAcceptanceAlg, InEcalAcceptanceAlg, InHcalAcceptanceAlg, InBremAcceptanceAlg
    InSpdAcceptanceAlg("HltInSPD").Tool = "InSpdAcceptance/HltInSpd:PUBLIC"
    ToolSvc().addTool(InSpdAcceptance, name ="HltInSpd")
    ToolSvc().HltInSpd.Extrapolator = "TrackHerabExtrapolator"
    
    HltCaloAcceptance.Members = [ InSpdAcceptanceAlg("HltInSPD") ,
                                  InPrsAcceptanceAlg("HltInPRS") ,
                                  InEcalAcceptanceAlg("HltInECAL"), 
                                  InHcalAcceptanceAlg("HltInHCAL") ,
                                  InBremAcceptanceAlg("HltInBREM") ]
    InSpdAcceptanceAlg('HltInSPD').Inputs   =     HltTracks
    InPrsAcceptanceAlg('HltInPRS').Inputs   =     HltTracks
    InEcalAcceptanceAlg('HltInECAL').Inputs =     HltTracks
    InHcalAcceptanceAlg('HltInHCAL').Inputs =     HltTracks
    InBremAcceptanceAlg('HltInBREM').Inputs =     HltTracks

    return HltCaloAcceptance
    
###############################################################################
#
def hltCaloTrackMatch(HltTracks):
    """
    Track-Cluster 2D matching for Neutral/Charged cluster selection
    """
    HltCaloTrackMatch =  Sequence("HltCaloTrackMatch")
    from Configurables import PhotonMatchAlg
    HltCaloTrackMatch.Members += [ hltCaloAcceptance(HltTracks) ,
                                   PhotonMatchAlg("HltClusterMatch")   ] 
    PhotonMatchAlg('HltClusterMatch').Tracks = HltTracks

    return HltCaloTrackMatch
    
###############################################################################
#
def hltCaloRec():
    """
    Merged Pi0, Single Photon & Electron reconstruction
    """
    HltCaloRec = Sequence("HltCaloRec")
    from Configurables import CaloMergedPi0Alg, CaloHypoAlg, CaloSinglePhotonAlg, CaloElectronAlg
    HltCaloRec.Members += [CaloMergedPi0Alg("HltMergedPi0Rec"), 
                           CaloHypoAlg("HltPhotonFromMergedRec"),
                           CaloSinglePhotonAlg("HltSinglePhotonRec"),
                           CaloElectronAlg("HltElectronRec")
                           ]

    # Configure Tools
    #
    # @todo : make that a configurable as well
    #
    importOptions( "$HLTCONFROOT/options/units.opts" )
    importOptions( "$CALORECOOPTS/HltClusterSelection.opts"  )
    importOptions( "$CALORECOOPTS/HltMergedPi0Rec.opts" )
    importOptions( "$CALORECOOPTS/HltPhotonFromMergedRec.opts" )
    importOptions( "$CALORECOOPTS/HltSinglePhotonRec.opts"  )
    importOptions( "$CALORECOOPTS/HltElectronRec.opts"  )
    importOptions( "$CALORECOOPTS/HltClusterSelection.opts"  )
    importOptions( "$CALORECOOPTS/CaloECorrectionParam.opts"   )
    importOptions( "$CALORECOOPTS/CaloSCorrectionParam.opts"   )
    importOptions( "$CALORECOOPTS/CaloLCorrectionParam.opts" )

    return HltCaloRec
    
###############################################################################
#
def hltCaloPIDs(HltTracks):
    """
    Track- charged/neutral cluster 3D matching for electron/bremStrahlung evaluation
    """
    HltCaloMatch = Sequence("HltCaloMatch")
    from Configurables import ElectronMatchAlg, BremMatchAlg
    HltCaloMatch.Members += [ ElectronMatchAlg("HltElectronMatch"),
                              BremMatchAlg("HltBremMatch") ]    
    ElectronMatchAlg('HltElectronMatch').Tracks = HltTracks
    BremMatchAlg('HltBremMatch').Tracks =         HltTracks

    HltCaloEnergy = Sequence("HltCaloEnergy")
    from Configurables import Track2SpdEAlg, Track2PrsEAlg, Track2EcalEAlg, Track2HcalEAlg
    HltCaloEnergy.Members =   [  Track2SpdEAlg("HltSpdE"), 
                                 Track2PrsEAlg("HltPrsE"), 
                                 Track2EcalEAlg("HltEcalE"), 
                                 Track2HcalEAlg("HltHcalE") ]
    Track2SpdEAlg('HltSpdE').Inputs =             HltTracks
    Track2PrsEAlg('HltPrsE').Inputs =             HltTracks
    Track2EcalEAlg('HltEcalE').Inputs =           HltTracks
    Track2HcalEAlg('HltHcalE').Inputs =           HltTracks
    HltCaloDLLs = Sequence("HltCaloDLLs")
    
    from Configurables import EcalChi22ID, BremChi22ID, ClusChi22ID, PrsPIDeAlg, BremPIDeAlg, EcalPIDeAlg, HcalPIDeAlg, EcalPIDmuAlg, HcalPIDmuAlg
    HltCaloDLLs.Members +=   [   EcalChi22ID("HltEcalChi22ID"), 
                                 BremChi22ID("HltBremChi22ID"), 
                                 ClusChi22ID("HltClusChi22ID"), 
                                 PrsPIDeAlg("HltPrsPIDe" ), 
                                 BremPIDeAlg("HltBremPIDe"),  
                                 EcalPIDeAlg("HltEcalPIDe" ), 
                                 HcalPIDeAlg("HltHcalPIDe" ), 
                                 EcalPIDmuAlg("HltEcalPIDmu"),  
                                 HcalPIDmuAlg("HltHcalPIDmu")  ]
    EcalChi22ID('HltEcalChi22ID').Tracks =        HltTracks
    BremChi22ID('HltBremChi22ID').Tracks =        HltTracks
    ClusChi22ID('HltClusChi22ID').Tracks =        HltTracks
    
    HltCaloPIDs = Sequence("HltCaloPIDs")
    HltCaloPIDs.Members +=    [  HltCaloMatch,
                                 HltCaloEnergy,
                                 HltCaloDLLs ]
        
    importOptions( "$CALORECOOPTS/CaloPIDs.opts" )

    return HltCaloPIDs
