#!/usr/bin/env python
# =============================================================================
# $Id: Reconstruction.py,v 1.1 2009-08-05 17:38:29 ibelyaev Exp $
# =============================================================================
## The major building blocks of Calorimeter Reconstruction
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
The major building blocks of Calorimeter Reconstruction
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'digitsReco'     , 
    'clustersReco'   , 
    'photonsReco'    , 
    'mergedPi0sReco' 
    )
# =============================================================================
from Gaudi.Configuration import *

from Configurables import GaudiSequencer
from Configurables import CellularAutomatonAlg
from Configurables import CaloSharedCellAlg
from Configurables import CaloClusterCovarianceAlg

from Configurables import CaloSinglePhotonAlg
from Configurables import CaloElectronAlg

from Configurables import CaloSelectCluster
from Configurables import CaloSelectClusterWithPrs
from Configurables import CaloSelectNeutralClusterWithTracks
from Configurables import CaloSelectNeutralClusterWithSpd 
from Configurables import CaloSelectChargedClusterWithSpd 
from Configurables import CaloSelectorNOT

from Configurables import CaloExtraDigits
from Configurables import CaloECorrection
from Configurables import CaloSCorrection
from Configurables import CaloLCorrection

from Configurables import PhotonMatchAlg
from Configurables import CaloHypoAlg
from Configurables import CaloMergedPi0Alg 

from Configurables import ( CaloZSupAlg       ,
                            CaloDigitsFromRaw ,
                            CaloGetterInit    ) 



from GaudiKernel.SystemOfUnits import MeV, GeV


from CaloKernel.ConfUtils import ( prntCmp        ,
                                   addAlgs        ,
                                   hltContext     ,
                                   setTheProperty ,
                                   onDemand       ,
                                   caloOnDemand   ,
                                   getAlgo        ) 



from CaloPIDs.PIDs import trackMatch

# =============================================================================
## prepare the digits for the recontruction
def digitsReco  ( context , enableRecoOnDemand ) :
    """
    Prepare the digits for the recontruction
    """
    alg = GaudiSequencer (
        'CaloDigits'      ,
        Context = context ,
        Members = [
        CaloZSupAlg       ( "EcalZSup"        ) ,
        CaloZSupAlg       ( "HcalZSup"        ) ,
        CaloDigitsFromRaw ( "PrsFromRaw"      ) ,
        CaloDigitsFromRaw ( "SpdFromRaw"      ) ,
        CaloGetterInit    ( "CaloDigitGetter" ) 
        ]
        )
    setTheProperty ( alg , 'Context' , context )
    log.debug ( 'Configure Ecal Digits  Reco  for         : %s' % alg.name() )
    ##
    return alg


## ============================================================================
## define the recontruction of Ecal clusters
def clusterReco ( context , enableRecoOnDemand ) :
    """
    Define the recontruction of Ecal Clusters
    """
    alg = getAlgo ( GaudiSequencer          , 
                    "ClusterReco"           ,
                    context                 ,
                    "Rec/Calo/EcalClusters" ,
                    enableRecoOnDemand      )

    clust = getAlgo ( CellularAutomatonAlg     , "EcalClust" , context )
    share = getAlgo ( CaloSharedCellAlg        , "EcalShare" , context ) 
    covar = getAlgo ( CaloClusterCovarianceAlg , "EcalCovar" , context )

    clust.PropertiesPrint = True
    share.PropertiesPrint = True
    covar.PropertiesPrint = True
    
    if hltContext ( context ) :
        clust.OutputData = 'Hlt/Calo/EcalClusters'
        share.InputData  = 'Hlt/Calo/EcalClusters'
        covar.InputData  = 'Hlt/Calo/EcalClusters'
        
    alg.Members = [
        clust ,
        share ,
        covar 
        ]
    
    setTheProperty ( alg , 'Context' , context )
    ##
    return alg

# ============================================================================
## define the recontruction of  Single Photons
def photonReco ( context , enableRecoOnDemand , useTracks ) :
    """
    Define the recontruction of Single Photon Hypo
    """
    alg = getAlgo ( CaloSinglePhotonAlg  ,
                    "SinglePhotonRec"    ,
                    context              ,
                    "Rec/Calo/Photons"   ,
                    enableRecoOnDemand   ) 
    
    alg.PropertiesPrint = False
    
    ## cluster selection tools:
    alg.addTool ( CaloSelectCluster  , "PhotonCluster" )
    ##
    if useTracks :
        alg.addTool ( CaloSelectNeutralClusterWithTracks , "NeutralCluster" )
        tool = alg.NeutralCluster
        tool . MinChi2 = 4
        log.debug ('Configure Neutral Cluster selector with Tracks: %s' % tool.getFullName() )
    else :
        alg.addTool ( CaloSelectNeutralClusterWithSpd    , "NeutralCluster" )
        tool = alg.NeutralCluster
        log.info  ('Configure Neutral Cluster selector with Spd   : %s' % tool.getFullName() )

    alg.SelectionTools = [
        alg.PhotonCluster  ,
        alg.NeutralCluster
        ]

    ## hypo tools
    alg.addTool ( CaloExtraDigits , 'SpdPrsExtraG' )
    alg.HypoTools = [
        alg.SpdPrsExtraG
        ]

    ## correction (private) tools
    alg.addTool ( CaloECorrection , 'ECorrection' )
    alg.addTool ( CaloSCorrection , 'SCorrection' )
    alg.addTool ( CaloLCorrection , 'LCorrection' )

    ecorr = alg.ECorrection
    scorr = alg.SCorrection
    lcorr = alg.LCorrection

    alg.CorrectionTools2 = [      ## V.B.: Is it an optimal/correct order?
        ecorr ,
        scorr ,
        lcorr
        ]

    from CaloReco.Corrections import eCorrection, sCorrection, lCorrection
    ecorr = eCorrection ( ecorr )
    scorr = sCorrection ( scorr )
    lcorr = lCorrection ( lcorr )
    
    if hltContext ( context ) :
        log.debug ( 'Configure Single Photon Reco for HLT     : %s' % alg.name() )
    else:
        log.debug ( 'Configure Single Photon Reco for Offline : %s' % alg.name() )

    seq = getAlgo ( GaudiSequencer , "PhotonReco" , context  )

    if useTracks : 
        addAlgs ( seq , trackMatch ( context , enableRecoOnDemand ) ) 

    addAlgs ( seq , alg ) 
        
    setTheProperty ( seq , 'Context' , context )
    
    if hltContext ( context ) :
        log.debug ( 'Configure Photon Reco Seq    for HLT     : %s' % seq.name() )
    else:
        log.debug ( 'Configure Photon Reco Seq    for Offline : %s' % seq.name() )

    return seq

# ============================================================================
## define the recontruction of Electorn Hypos
def electronReco ( context , enableRecoOnDemand ) :
    """
    Define the reconstruction of
    """
    alg = None
    if hltContext ( context ) :
        alg = getAlgo ( CaloElectronAlg      ,
                        'HltElectronHypoRec' ,
                        'Hlt/Calo/Electrons' ,
                        enableRecoOnDemand   ) 
    else:
        alg = getAlgo ( CaloElectronAlg      ,
                        'ElectronHypoRec'    ,
                        'Rec/Calo/Electrons' ,
                        enableRecoOnDemand   ) 
        
    alg.PropertiesPrint = False

    ## cluster selection tools:
    alg.addTool ( CaloSelectCluster               , "ElectronCluster" )
    # hits in Spd
    alg.addTool ( CaloSelectChargedClusterWithSpd , "ChargedWithSpd"  )
    # check energy in Prs
    alg.addTool ( CaloSelectClusterWithPrs        , "ClusterWithPrs"  )
    alg.ClusterWithPrs.MinEnergy = 10 * MeV
    # match with tracks
    alg.addTool ( CaloSelectorNOT   , "ChargedWithTracks"  )
    clnot = alg.ChargedWithTracks
    clnot.addTool ( CaloSelectNeutralClusterWithTracks , "NotNeutralCluster" )
    clnot.NotNeutralCluster.MinChi2 = 25
    
    alg.SelectionTools = [
        alg.ElectronCluster   ,
        alg.ChargedWithSpd    ,
        alg.ClusterWithPrs    ,
        alg.ChargedWithTracks
        ]

    ## hypo tools
    alg.addTool ( CaloExtraDigits , 'SpdPrsExtraE' )
    alg.HypoTools = [
        alg.SpdPrsExtraE
        ]

    ## correction (private) tools
    alg.addTool ( CaloECorrection , 'ECorrection' )
    alg.addTool ( CaloSCorrection , 'SCorrection' )
    alg.addTool ( CaloLCorrection , 'LCorrection' )

    ecorr = alg.ECorrection
    scorr = alg.SCorrection
    lcorr = alg.LCorrection

    alg.CorrectionTools2 = [      ## V.B.: Is it an optimal/correct order?
        ecorr ,
        scorr ,
        lcorr
        ]

    from CaloReco.Corrections import eCorrection, sCorrection, lCorrection
    ecorr = eCorrection ( ecorr )
    scorr = sCorrection ( scorr )
    lcorr = lCorrection ( lcorr )

    
    seq = getAlgo ( GaudiSequencer , "ElectronReco" , context )
    
    addAlgs ( seq , trackMatch ( context , enableRecoOnDemand ) ) 
    addAlgs ( seq , alg ) 
    
    setTheProperty ( seq , 'Context' , context )

    if hltContext (  context ) :
        log.debug ( 'Configure Electron Hypo Reco for HLT     : %s' % alg.name() )
    else:
        log.debug ( 'Configure Electron Hypo Reco for Offline : %s' % alg.name() )

    return seq

    
# =============================================================================
## define the reconstruction of Merged Pi0s Hypos 
def mergedPi0Reco ( context , enableRecoOnDemand ) :
    """
    Define the recontruction of Merged Pi0s
    """
    
    pi0 = getAlgo ( CaloMergedPi0Alg      ,
                    'MergedPi0Rec'        ,
                    context               )
    
    pi0.PropertiesPrint = False

    pi0.TrShOut_nospd = [ -0.0060,  2.4956,115.0827,  9.8842,  0.0320,  2.0982,  1.0302,  0.0409,  0.0030, -9.6135 ]
    pi0.TrShMid_nospd = [  0.0464,  2.0384, 36.5885,  8.0260,  0.0460,  2.3936,  1.0703,  0.1611,  0.0238, -5.8899 ]
    pi0.TrShInn_nospd = [  0.0981,  2.2529, 33.8837,  8.0532,  0.0654,  2.2046,  1.1092,  0.1645,  0.0248, -5.7248 ]
    
    pi0.TrShOut_spd   = [  0.0486,  2.7847, 68.4815,  9.0870,  0.0116,  1.2591,  1.0464, -0.0900,  0.0005,-12.9098 ]
    pi0.TrShMid_spd   = [  0.0882,  1.7910, 24.4324,  7.4802,  0.0258,  2.7719,  1.1294, -0.0802,  0.0024, -9.7273 ]
    pi0.TrShInn_spd   = [  0.1111,  1.7909, 18.0852,  7.1122,  0.0261,  1.3889,  1.1846, -0.0934,  0.0029, -8.9966 ]
    
    pi0.SPar     = [ 0.102 , 0.129 , 0.144  ]
    pi0.LPar_Al1 = [ 17.936       ,  19.053       , 22.305 ] 
    pi0.LPar_Al2 = [   1.15  ,  1.53  ,  2.04  ]
    pi0.LPar_Al3 = [ 0.0392 , 0.455  , 0.0281  ]
    pi0.LPar_Be1 = [ 54.54  , 60.87  , 58.04   ]
    pi0.LPar_Be2 = [ 4.02  ,  4.15  ,  4.38    ]
    pi0.LPar_Be3 = [  0.0308 , 0.0597 , 0.0247 ]
    pi0.LPar_Be0 = [   0.,0.,0. ] ## //unused
    pi0.LPar_z0  = [ 12566. ]
    
    pi0.EtCut    = 2. * GeV;

    splitg = getAlgo ( CaloHypoAlg , 'PhotonFromMergedRec' , context )
    
    splitg.PropertiesPrint = False
    
    if hltContext ( context ) :
        splitg.InputData = 'Hlt/Calo/SplitPhotons'
    else:
        splitg.InputData = 'Rec/Calo/SplitPhotons'
        
    splitg.addTool ( CaloExtraDigits , 'SpdExtraG' )
    ## correction (private) tools
    splitg.addTool ( CaloECorrection , 'ECorrection' )
    splitg.addTool ( CaloSCorrection , 'SCorrection' )
    splitg.addTool ( CaloLCorrection , 'LCorrection' )
    
    ecorr = splitg.ECorrection
    scorr = splitg.SCorrection
    lcorr = splitg.LCorrection
    
    from CaloReco.Corrections import eCorrection, sCorrection, lCorrection
    ecorr = eCorrection ( ecorr )
    scorr = sCorrection ( scorr )
    lcorr = lCorrection ( lcorr )

    splitg.Tools = [
        splitg.SpdExtraG ,
        ecorr ,
        scorr ,
        lcorr 
        ]
    
    seq = getAlgo ( GaudiSequencer        ,
                    'MergedPi0Reco'       ,
                    context               , 
                    'Rec/Calo/MergedPi0s' ,
                    enableRecoOnDemand    )
    
    seq.Members = [ pi0 , splitg ] 
    
    if enableRecoOnDemand :
        ## split photons
        if hltContext ( context ) :
            onDemand ( 'Hlt/Calo/EcalSplitClusters' , seq ) 
            onDemand ( 'Hlt/Calo/SplitPhotons'      , seq ) 
        else :
            onDemand ( 'Rec/Calo/EcalSplitClusters' , seq ) 
            onDemand ( 'Rec/Calo/SplitPhotons'      , seq ) 


    setTheProperty ( seq , 'Context' , context )

    if hltContext ( context ) :
        log.debug ( 'Configure Merged Pi0 Reco     for HLT     : %s' % seq.name() )
    else:
        log.debug ( 'Configure Merged Pi0 Reco     for Offline : %s' % seq.name() )

    return seq


# =============================================================================
if '__main__' == __name__ :
    print __doc__
    
# =============================================================================
# The END
# =============================================================================
