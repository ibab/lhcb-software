#!/usr/bin/env python
# =============================================================================
# $Id: Reconstruction.py,v 1.12 2010-05-20 09:47:06 odescham Exp $
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.12 $"
# =============================================================================
__all__ = (
    'clustersReco'   , 
    'photonsReco'    , 
    'mergedPi0sReco' 
    )
# =============================================================================
from Gaudi.Configuration import *

from Configurables import GaudiSequencer

from GaudiKernel.SystemOfUnits import MeV, GeV


from CaloKernel.ConfUtils import ( prntCmp        ,
                                   addAlgs        ,
                                   hltContext     ,
                                   setTheProperty ,
                                   onDemand       ,
                                   caloOnDemand   ,
                                   getAlgo        ) 

import logging
_log = logging.getLogger('CaloReco')

# =============================================================================
## prepare the digits for the recontruction
def digitsReco  ( context            ,
                  enableRecoOnDemand ,
                  createADC          ) :
    """
    Prepare the digits for the recontruction
    """

    from CaloDAQ.CaloDigits  import caloDigits

    _log.warning('CaloReco.digitsReco is deprecated, use CaloDigitConf instead')

    # warning : the caloDigits TES should be context-independent
    return caloDigits ( context            ,
                        enableRecoOnDemand ,
                        createADC          )

## ============================================================================
## define the recontruction of Ecal clusters
def clusterReco ( context , enableRecoOnDemand , clusterPt = 0.  , fastReco = False , external = '', makeTag=False,
                  noSpdPrs=False , masksE=[] , masksP=[] , mergedPi0Pt = 2* GeV) :
    """
    Define the recontruction of Ecal Clusters
    """
    
    from Configurables import ( CaloDigitFilterAlg       ,
                                CellularAutomatonAlg     ,
                                CaloShowerOverlap        ,
                                # CaloSharedCellAlg        , # obsolete
                                CaloClusterCovarianceAlg ,
                                CaloClusterizationTool) 

    # cluster TES  is 'almost' context-independent : single HLT TES whatever the HLT-type else 'offline' TES
    ## Warning MUST be synchronous with CaloAlgUtils TES settings
    ### Exception when external clusters location is given

    if external == '' :
        _cont = context
        if '' != context and 'OFFLINE' != _cont.upper() and _cont.upper().find( 'HLT' ) != -1 :
            context = 'Hlt'
        else :
            context = ''
    elif makeTag != '':
        context = makeTag
    

    ## Define the context-dependent sequencer
    seq   = getAlgo ( GaudiSequencer           , "ClusterReco", context , "Rec/Calo/EcalClusters" , enableRecoOnDemand )

    seq.Members[:]=[]
    filter= getAlgo ( CaloDigitFilterAlg       , "CaloDigitFilter",context)
    if noSpdPrs :
        filter.PrsFilter=0
        filter.SpdFilter=0

    clust = getAlgo ( CellularAutomatonAlg     , "EcalClust"  , context )
#   share = getAlgo ( CaloSharedCellAlg        , "EcalShare"  , context )   # obsolete
    share = getAlgo ( CaloShowerOverlap        , "EcalShare"  , context )  
    covar = getAlgo ( CaloClusterCovarianceAlg , "EcalCovar"  , context ) 

    if masksE != [] :
        share.EnergyTags = masksE
        covar.EnergyTags = masksE
    if masksP != [] :
        share.PositionTags = masksP
        covar.PositionTags = masksP

    if external != '' :                  # use non-default clusters 
        share.InputData = external
        covar.InputData = external
        if makeTag != '' :               # make the non-default clusters
            seq.Members += [ filter, clust ]
            clust.OutputData = external
    else :
        seq.Members += [ filter, clust ]        # make default clusters


    if clusterPt > 0 :
        from Configurables import CaloClusterizationTool
        clust.addTool(CaloClusterizationTool,'CaloClusterizationTool')
        clust.CaloClusterizationTool.ETcut = clusterPt
        clust.CaloClusterizationTool.withET = True

    seq.Members += [ share , covar ]
    setTheProperty ( seq , 'Context' , context )


    ## setup onDemand for SplitClusters
    if enableRecoOnDemand : 
        splitSeq   = mergedPi0Reco( context , enableRecoOnDemand , True, False, False,mergedPi0Pt)
        onDemand ( 'Rec/Calo/EcalSplitClusters' , splitSeq , context ) 


    ## printout
    _log.debug ( 'Configure Ecal Cluster Reco Seq : %s   for %s : ' % (seq.name()  , context) )
    if enableRecoOnDemand : 
        _log.info    ('ClusterReco onDemand')
    
    ##    
    return seq

# ============================================================================
## define the recontruction of  Single Photons
def photonReco ( context , enableRecoOnDemand, useTracks = True , useSpd = False, usePrs = False , trackLocations = [], neutralID = True,
                 photonPt=50.*MeV, fastReco = False, external = '',noSpdPrs=False) :
    """
    Define the recontruction of Single Photon Hypo
    """
    
    from Configurables import   CaloSinglePhotonAlg                
    from Configurables import   CaloHypoAlg     
    from Configurables import ( CaloSelectCluster                  , 
                                CaloSelectClusterWithPrs           ,
                                CaloSelectNeutralClusterWithTracks , 
                                CaloSelectNeutralClusterWithSpd    ,
                                CaloSelectChargedClusterWithSpd    , 
                                CaloSelectorNOT                    ,
                                CaloSelectorOR                     )
    
    from Configurables import ( CaloExtraDigits ,
                                CaloECorrection , 
                                CaloSCorrection , 
                                CaloLCorrection )
                                 
    
    
    ## build the context-dependent sequence (  TrackMatch + SinglePhotonRec )
    seq = getAlgo ( GaudiSequencer , "PhotonReco" , context , "Rec/Calo/Photons"  , enableRecoOnDemand   )
    seq.Members=[]

    # 1/ PhotonMatch from CaloPIDs (if tracking is requested)
    if useTracks : 
        from CaloPIDs.PIDs import trackMatch
        tm =  trackMatch ( context,enableRecoOnDemand, trackLocations , fastReco, external)
        addAlgs ( seq , tm ) 
    

    ##2/ SinglePhotonRec alg
    alg = getAlgo ( CaloSinglePhotonAlg, "SinglePhotonRec" , context )
    alg.PropertiesPrint = False
    if external != '' :
        alg.InputData = external
    
    # cluster selection tools:
    ### a/ generic selection (energy/multiplicity)
    alg.addTool ( CaloSelectCluster  , "PhotonCluster" )
    alg.SelectionTools = [ alg.PhotonCluster ]
    alg.PhotonCluster.MinEt     = photonPt
    alg.PhotonCluster.MinDigits = 2  # skip single-cell clusters
    
    ### b/ Neutral cluster (track-based and/or Spd/Prs-based)    
    if   useTracks     :
        alg.addTool ( CaloSelectNeutralClusterWithTracks , "NeutralCluster" )
        alg.NeutralCluster.MinChi2 = 4
        alg.SelectionTools += [ alg.NeutralCluster ]
        _log.debug    ('CaloReco/PhotonReco: Configure Neutral Cluster Selector with Tracks     : %s' %  alg.NeutralCluster.getFullName() )

    if usePrs :
        alg.addTool ( CaloSelectClusterWithPrs           , "ClusterWithPrs" )
        alg.ClusterWithPrs.MinEnergy = 10.*MeV
        alg.SelectionTools += [ alg.ClusterWithPrs ]
        _log.debug ('CaloReco/PhotonReco: Configure Cluster Selector with  Prs       : %s' %  alg.ClusterWithPrs.getFullName() )

    if  useSpd  :
        alg.addTool ( CaloSelectNeutralClusterWithSpd    , "NeutralClusterWithSpd"    )        
        alg.SelectionTools += [ alg.NeutralClusterWithSpd ]
        _log.debug ('CaloReco/PhotonReco: Configure Neutral Cluster Selector with !Spd : %s ' %   alg.NeutralClusterWithSpd.getFullName()  )
        
        
    ## hypo tools : add Spd/Prs digits

    if not noSpdPrs :
        alg.addTool ( CaloExtraDigits , 'SpdPrsExtraG' )
        alg.HypoTools = [ alg.SpdPrsExtraG ]
    else :
        alg.HypoTools = []
        
    ## correction tools : E/S/L
    alg.addTool ( CaloECorrection , 'ECorrection' )
    alg.addTool ( CaloSCorrection , 'SCorrection' )
    alg.addTool ( CaloLCorrection , 'LCorrection' )
    
    ecorr = alg.ECorrection
    scorr = alg.SCorrection
    lcorr = alg.LCorrection

    # tool configuration via condDB only (remove default configuration)
    alg.CorrectionTools2 = [ ecorr , scorr , lcorr ]                      

    # update the sequence
    addAlgs ( seq , alg ) 

    ## 3/ PhotonID
    from CaloPIDs.PIDs import PhotonID
    if neutralID : 
        pID =  PhotonID ( context,enableRecoOnDemand, useTracks )
        addAlgs ( seq , pID ) 

    ## global context
    setTheProperty ( seq , 'Context' , context )

    ## printout
    _log.debug ( 'Configure Photon Reco Seq : %s  for : %s' % (seq.name() , context ) )
    if enableRecoOnDemand : 
        _log.info    ('PhotonReco onDemand')
        
    return seq

# ============================================================================
## define the recontruction of Electorn Hypos
def electronReco ( context , enableRecoOnDemand , useTracksE = True , useSpdE = True, usePrsE = True, trackLocations = [] ,
                   electronPt=50.*MeV , fastReco = False, external = '' ,noSpdPrs=False) :
    """
    Define the reconstruction of
    """

    from Configurables import   CaloElectronAlg                
    from Configurables import   CaloHypoAlg 
    
    from Configurables import ( CaloSelectCluster                  , 
                                CaloSelectClusterWithPrs           ,
                                CaloSelectNeutralClusterWithTracks , 
                                CaloSelectNeutralClusterWithSpd    ,
                                CaloSelectChargedClusterWithSpd    , 
                                CaloSelectorNOT                    )
    
    from Configurables import ( CaloExtraDigits ,
                                CaloECorrection , 
                                CaloSCorrection , 
                                CaloLCorrection ) 
    

    ## build the context-dependent sequence (  TrackMatch + SingleElectronRec )
    seq = getAlgo ( GaudiSequencer , "ElectronReco" , context  , 'Rec/Calo/Electrons' , enableRecoOnDemand   ) 
    seq.Members=[]

    # 1/ ElectronMatch from CaloPIDs (if useTracks)
    if useTracksE :
        from CaloPIDs.PIDs import trackMatch
        tm =  trackMatch ( context,enableRecoOnDemand, trackLocations , fastReco , external)
        addAlgs ( seq , tm ) 

    ## 2/ Electron Rec alg
    alg = getAlgo ( CaloElectronAlg ,  'SingleElectronRec', context  ) 
    alg.PropertiesPrint = False
    if external != '' :
        alg.InputData = external
        
    # cluster selection tools:

    ## 1/ generic selection (energy/multiplicity)
    alg.addTool ( CaloSelectCluster               , "ElectronCluster" )
    alg.SelectionTools = [ alg.ElectronCluster ]
    alg.ElectronCluster.MinEt = electronPt
    alg.ElectronCluster.MinDigits = 2 # skip single-cell clusters

    ## 2/  hits in Spd
    if useSpdE : 
        alg.addTool ( CaloSelectChargedClusterWithSpd , "ChargedClusterWithSpd"  )
        alg.SelectionTools += [ alg.ChargedClusterWithSpd ]
        _log.debug    ('CaloReco/ElectronReco: Configure Charged Cluster Selector with Spd     : %s' %  alg.ChargedClusterWithSpd.getFullName() )

    ## 3/ energy in Prs
    if usePrsE :  
        alg.addTool ( CaloSelectClusterWithPrs        , "ClusterWithPrs"  )
        alg.ClusterWithPrs.MinEnergy = 10 * MeV
        alg.SelectionTools += [ alg.ClusterWithPrs ]
        _log.debug    ('CaloReco/ElectronReco: Configure Cluster Selector with Prs    : %s' %  alg.ClusterWithPrs.getFullName() )

    ## 4/  match with tracks
    if useTracksE :
        alg.addTool ( CaloSelectorNOT   , "ChargedCluster"  )
        clnot = alg.ChargedCluster
        clnot.addTool ( CaloSelectNeutralClusterWithTracks , "NotNeutralCluster" )
        clnot.NotNeutralCluster.MinChi2 = 25
        clnot.SelectorTools = [ clnot.NotNeutralCluster ]
        alg.SelectionTools += [ alg.ChargedCluster ]
        _log.debug   ('CaloReco/ElectronReco: Configure Charged Cluster Selector with Tracks     : %s' %  alg.ChargedCluster.getFullName() )


    ## hypo tools : add Spd/Prs digits
    if not noSpdPrs :
        alg.addTool ( CaloExtraDigits , 'SpdPrsExtraE' )
        alg.HypoTools = [ alg.SpdPrsExtraE ]
    else :
        alg.HypoTools = []

    ## correction tools : E/S/L
    alg.addTool ( CaloECorrection , 'ECorrection' )
    alg.addTool ( CaloSCorrection , 'SCorrection' )
    alg.addTool ( CaloLCorrection , 'LCorrection' )

    ecorr = alg.ECorrection
    scorr = alg.SCorrection
    lcorr = alg.LCorrection

    # tool configuration via condDB (remove default configuration)
    alg.CorrectionTools2 = [ ecorr , scorr , lcorr ]
    
    ## update the sequence
    addAlgs ( seq , alg ) 

    # global context 
    setTheProperty ( seq , 'Context' , context )

    ## printout
    _log.debug ( 'Configure Electron Reco Seq : %s  for : %s' % (seq.name() , context ))
    if enableRecoOnDemand : 
        _log.info    ('ElectronReco onDemand')

    return seq

    
# =============================================================================
## define the reconstruction of Merged Pi0s Hypos 
def mergedPi0Reco ( context , enableRecoOnDemand , clusterOnly = False , neutralID = True , useTracks = True,
                    mergedPi0Pt = 2* GeV ,fastReco = False, external = '',noSpdPrs=False,
                    masksE=[] , masksP=[] ) :

    """
    Define the recontruction of Merged Pi0s
    """

    from Configurables import   CaloMergedPi0 # CaloMergedPi0Alg is obsolete (replaced by CaloMergedPi0)
    from Configurables import   CaloHypoAlg 

    from Configurables import ( CaloExtraDigits ,
                                CaloECorrection , 
                                CaloSCorrection , 
                                CaloLCorrection 
                                ) 


    # build the sequences
    seq = getAlgo ( GaudiSequencer , 'MergedPi0Reco' , context )
    sseq = getAlgo ( GaudiSequencer , 'SplitClusterReco' , context )
        
    ## Merged Pi0
    if clusterOnly :
        pi0 = getAlgo ( CaloMergedPi0 , 'SplitClustersRec', context )
        pi0.CreateSplitClustersOnly = True
        sseq.Members=[pi0]
    else :
        pi0 = getAlgo ( CaloMergedPi0 , 'MergedPi0Rec', context )        

    if masksE != [] :
        pi0.EnergyTags = masksE
    if masksP != [] :
        pi0.PositionTags = masksP

    if external  != '' :
        pi0.InputData = external

    pi0.PropertiesPrint = False
    
    pi0.EtCut    = mergedPi0Pt

    if clusterOnly :
        setTheProperty ( sseq , 'Context' , context )
        return sseq
    
    ## MergedPi0 sequence
    
    ## 2/ SplitPhotons
    #    splitg = getAlgo ( CaloHypoAlg , 'PhotonFromMergedRec' , context )    
    #    splitg.HypoType = "SplitPhotons";
    #    splitg.PropertiesPrint = False   
    
    ## Add Prs/Spd digits
    if not noSpdPrs :
        pi0.addTool ( CaloExtraDigits , 'SpdPrsExtraS' )
        pi0.addTool ( CaloExtraDigits , 'SpdPrsExtraM' )
    ## Pi0 correction tools
    pi0.addTool ( CaloECorrection , 'ECorrection' )
    pi0.addTool ( CaloSCorrection , 'SCorrection' )
    pi0.addTool ( CaloLCorrection , 'LCorrection' )

    # tool configuration via condDB only (remove default configuration)
    if not noSpdPrs :
        pi0.PhotonTools = [ pi0.SpdPrsExtraS , pi0.ECorrection,pi0.SCorrection, pi0.LCorrection ]
        pi0.Pi0Tools    = [ pi0.SpdPrsExtraM ]
    else :       
        pi0.PhotonTools = [ pi0.ECorrection,pi0.SCorrection, pi0.LCorrection ]

    seq.Members=[pi0]
    
    ## 3/ (PhotonFrom)MergedID
    if neutralID : 
        from CaloPIDs.PIDs import (MergedID , PhotonFromMergedID )
        idSeq = getAlgo( GaudiSequencer, "MergedIDSeq",context)
        mID =  MergedID ( context,enableRecoOnDemand, useTracks )
        pmID =  PhotonFromMergedID( context, enableRecoOnDemand, useTracks )
        idSeq.Members = [ mID, pmID ]  
        addAlgs ( seq , idSeq ) 

    # propagate the context
    setTheProperty ( seq , 'Context' , context )


    ## onDemand
    if enableRecoOnDemand :
            onDemand ( 'Rec/Calo/SplitPhotons'      , seq , context ) 
            onDemand ( 'Rec/Calo/MergedPi0s'        , seq , context )
            ## onDemand ( 'Rec/Calo/EcalSplitClusters' , seq , context )  ## ??

    ## printout
    _log.debug ( 'Configure MergedPi0 Reco Seq : %s  for : %s' %( seq.name() , context ))
    if enableRecoOnDemand : 
        _log.info    ('MergedPi0Reco onDemand')

    return seq


# =============================================================================
if '__main__' == __name__ :
    print __doc__
