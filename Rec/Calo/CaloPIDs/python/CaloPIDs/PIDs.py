#!/usr/bin/env python
# =============================================================================
# $Id: PIDs.py,v 1.2 2009-09-01 11:28:12 ibelyaev Exp $
# =============================================================================
## The major building blocks of Calorimeter PID
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
The major building blocks of Calorimeter PID
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
from Gaudi.Configuration  import *

from Configurables        import    GaudiSequencer

from CaloKernel.ConfUtils import ( hltContext          ,
                                   getAlgo             ,
                                   setTheProperty      )


# =============================================================================
## define "inEcalAcceptance" algorithm
def inEcalAcc ( context , enableRecoOnDemand ) :
    """
    
    define 'inEcalAcceptance' algorithm
    
    """

    from Configurables import  ( InEcalAcceptance    , 
                                 InEcalAcceptanceAlg ) 
    
    ## check if the track is in Ecal acceptance 
    inEcal = getAlgo ( InEcalAcceptanceAlg  ,
                       'InECAL'             ,
                       context              ,
                       "Rec/Calo/InAccEcal" ,
                       enableRecoOnDemand   )
    
    if hltContext ( context ) :
        inEcal.addTool ( InEcalAcceptance , 'InECAL' )
        tool = inEcal.InECAL
        tool.Extrapolator = 'TrackHerabExtrapolator'
        inEcal.Tool = tool
        
    return inEcal

## ============================================================================
## define the minimal track match sequnce for photon recontruction
def trackMatch ( context , enableRecoOnDemand ) :
    """

    Define the minimal track match sequnce for photon reconstruction

    """

    from Configurables import PhotonMatchAlg      
    

    ## check if the track is in Ecal acceptance 
    inEcal = inEcalAcc ( context , enableRecoOnDemand ) 

    ## perform the actual track <-> cluster match 
    clmatch = getAlgo  (  PhotonMatchAlg          ,
                          'ClusterMatch'          ,
                          context                 ,
                          'Rec/Calo/ClusterMatch' ,
                          enableRecoOnDemand      )
    
    seq = getAlgo ( GaudiSequencer , 'CaloTrackMatch' , context )
    seq.Members = [ inEcal , clmatch ] 
    
    setTheProperty ( seq , 'Context' , context )
    
    if hltContext ( context ) :
        log.debug ( 'Configure Cluster Track Match for HLT     : %s' % seq.name() )
    else:
        log.debug ( 'Configure Cluster Track Match for Offline : %s' % seq.name() )

    return seq


# =================================================================================
## define various Calo PIDs evaluation
def caloPIDs ( context , enableRecoOnDemand ) :
    """
    Define various Calo PIDs evaluation
    """
    
    from Configurables import ( InSpdAcceptance     ,
                                InSpdAcceptanceAlg  ,
                                InPrsAcceptance     ,
                                InPrsAcceptanceAlg  ,
                                InEcalAcceptance    ,
                                InEcalAcceptanceAlg ,
                                InBremAcceptance    ,
                                InBremAcceptanceAlg , 
                                InHcalAcceptance    ,
                                InHcalAcceptanceAlg ) 
    
    from Configurables import ( ElectronMatchAlg    ,
                                BremMatchAlg        )    
    
    from Configurables import ( Track2SpdEAlg       ,
                                Track2PrsEAlg       ,
                                Track2EcalEAlg      ,
                                Track2HcalEAlg      )
    
    
    from Configurables import ( EcalChi22ID         ,
                                BremChi22ID         ,
                                ClusChi22ID         ,
                                PrsPIDeAlg          ,
                                EcalPIDeAlg         ,
                                BremPIDeAlg         ,
                                HcalPIDeAlg         ,
                                EcalPIDmuAlg        ,
                                HcalPIDmuAlg        )
    
    inSPD = getAlgo ( InSpdAcceptanceAlg    ,
                      'InSPD'               , 
                      context               ,
                      "Rec/Calo/InAccSpd"   ,
                      enableRecoOnDemand    )
    inPRS = getAlgo ( InPrsAcceptanceAlg    ,
                      'InPRS'               , 
                      context               ,
                      "Rec/Calo/InAccPrs"   ,
                      enableRecoOnDemand    )
    inHCAL = getAlgo ( InHcalAcceptanceAlg  ,
                       'InHCAL'             , 
                       context              ,
                       "Rec/Calo/InAccHcal" ,
                       enableRecoOnDemand   )
    inBREM = getAlgo ( InBremAcceptanceAlg  ,
                       'InBREM'             , 
                       context              ,
                       "Rec/Calo/InAccBrem" ,
                       enableRecoOnDemand   )

    
    inECAL = inEcalAcc ( context , enableRecoOnDemand ) 
                          
    if hltContext ( context ) :
        #
        inSPD.addTool  ( InSpdAcceptance , 'InSPD' )
        tool = inSPD.InSPD
        tool.Extrapolator = 'TrackHerabExtrapolator'
        inSPD.Tool = tool
        #
        inPRS.addTool  ( InPrsAcceptance , 'InPRS' )
        tool = inPRS.InPRS
        tool.Extrapolator = 'TrackHerabExtrapolator'
        inPRS.Tool = tool
        #
        inHCAL.addTool ( InHcalAcceptance , 'InHCAL' )
        tool = inHCAL.InHCAL
        tool.Extrapolator = 'TrackHerabExtrapolator'
        inHCAL.Tool = tool
        #
        
    inAcc = getAlgo ( GaudiSequencer     ,
                      'InCaloAcceptance' ,
                      context            )
    
    inAcc.Members = [
        inSPD  ,
        inPRS  ,
        inECAL ,
        inHCAL ,
        inBREM 
        ]

    match    = getAlgo ( GaudiSequencer  ,
                         'CaloMatch'     ,
                         context         )

    cluster  = trackMatch ( context , enableRecoOnDemand )

    
    electron = getAlgo ( ElectronMatchAlg         ,
                         "ElectronMatch"          ,
                         context                  ,
                         'Rec/Calo/ElectronMatch' ,
                         enableRecoOnDemand       ) 
    brem     = getAlgo ( BremMatchAlg             ,
                         "BremMatch"              ,
                         context                  ,
                         'Rec/Calo/BremMatch'     ,
                         enableRecoOnDemand       )
    
    match.Members = [
        cluster  , 
        electron ,
        brem ]
    

    energy = getAlgo ( GaudiSequencer ,
                       'CaloEnergy'   ,
                       context        )
    
    spdE   = getAlgo ( Track2SpdEAlg      ,
                       'SpdE'             , 
                       context            ,
                       'Rec/Calo/SpdE'    ,
                       enableRecoOnDemand )
    
    prsE   = getAlgo ( Track2PrsEAlg      ,
                       'PrsE'             , 
                       context            ,
                       'Rec/Calo/PrsE'    ,
                       enableRecoOnDemand )
    
    ecalE  = getAlgo ( Track2EcalEAlg     ,
                       'EcalE'            , 
                       context            ,
                       'Rec/Calo/EcalE'   ,
                       enableRecoOnDemand )
    
    hcalE  = getAlgo ( Track2HcalEAlg     ,
                       'HcalE'            , 
                       context            ,
                       'Rec/Calo/HcalE'   ,
                       enableRecoOnDemand )

    energy.Members = [ spdE , prsE , ecalE , hcalE ]


    dlls  = getAlgo ( GaudiSequencer ,
                      'CaloDLLs'     ,
                      context        )

    dlls.Members = [
        ## convert tables into plain Chi2's
        getAlgo ( EcalChi22ID          , 
                  'EcalChi22ID'        ,
                  context              ,
                  'Rec/Calo/EcalChi2'  ,
                  enableRecoOnDemand   ) ,
        getAlgo ( BremChi22ID          , 
                  'BremChi22ID'        ,
                  context              , 
                  'Rec/Calo/BremChi2'  ,
                  enableRecoOnDemand   ) ,
        getAlgo ( ClusChi22ID          , 
                  'ClusChi22ID'        ,
                  context              ,
                  'Rec/Calo/ClusChi2'  ,
                  enableRecoOnDemand   ) ,
        ## evaluate proper DLL
        getAlgo ( PrsPIDeAlg           ,
                  'PrsPIDe'            ,
                  context              ,
                  'Rec/Calo/PrsPIDe'   ,
                  enableRecoOnDemand   ) ,
        getAlgo ( EcalPIDeAlg          ,
                  'EcalPIDe'           ,
                  context              ,
                  'Rec/Calo/EcalPIDe'  ,
                  enableRecoOnDemand   ) ,
        getAlgo ( BremPIDeAlg          ,
                  'BremPIDe'           ,
                  context              ,
                  'Rec/Calo/BremPIDe'  ,
                  enableRecoOnDemand   ) ,
        getAlgo ( HcalPIDeAlg          ,
                  'HcalPIDe'           ,
                  context              ,
                  'Rec/Calo/HcalPIDe'  ,
                  enableRecoOnDemand   ) ,
        getAlgo ( EcalPIDmuAlg         ,
                  'EcalPIDmu'          ,
                  context              ,
                  'Rec/Calo/EcalPIDmu' ,
                  enableRecoOnDemand   ) ,
        getAlgo ( HcalPIDmuAlg         ,
                  'HcalPIDmu'          ,
                  context              ,
                  'Rec/Calo/HcalPIDmu' ,
                  enableRecoOnDemand   ) 
        ]

    
    seq = getAlgo ( GaudiSequencer  ,
                    'CaloPIDs'      ,
                    context         )
    
    seq.Members = [
        inAcc , match , energy , dlls 
        ]

    setTheProperty ( seq , 'Context' , context )
    
    if hltContext ( context ) :
        log.debug ( 'Configure Calo PIDs  Reco     for HLT     : %s' % seq.name() )
    else:
        log.debug ( 'Configure Calo PIDs  Reco     for Offline : %s' % seq.name() )
        
    return seq

# =================================================================================
    
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    
# =============================================================================
# The END
# =============================================================================
