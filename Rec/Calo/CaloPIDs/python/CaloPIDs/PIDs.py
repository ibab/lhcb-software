#!/usr/bin/env python
# =============================================================================
# $Id: PIDs.py,v 1.4 2010/03/08 01:31:33 odescham Exp $
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
__version__ = "CVS tag $Name: v5r6 $, version $Revision: 1.4 $"
# =============================================================================
from Gaudi.Configuration  import *
    
from Configurables        import    GaudiSequencer

from CaloKernel.ConfUtils import ( hltContext          ,
                                   getAlgo             ,
                                   setTheProperty      ,
                                   addAlgs
                                   )



# =============================================================================
## define "PhotonID" algorithm
def PhotonID ( context , enableRecoOnDemand, useTracks = True ) :
    """
    define 'PhotonID' algorithm
    """
    
    
    from Configurables import   CaloPhotonIdAlg
    
    ## photonID
    pid = getAlgo ( CaloPhotonIdAlg,
                    "PhotonID", 
                    context,
                    "Rec/Calo/PhotonID",
                    enableRecoOnDemand
                    )
    
    pid.Type = 'PhotonID'
    pid.Tracking = useTracks 
    return pid

# =============================================================================
## define "MergedID" algorithm
def MergedID ( context , enableRecoOnDemand, useTracks = True ) :
    """
    define 'MergedID' algorithm
    """
    
    from Configurables import   CaloPhotonIdAlg
    
    ## photonID
    mid = getAlgo ( CaloPhotonIdAlg,
                    "MergedID", 
                    context,
                    "Rec/Calo/MergedID",
                    enableRecoOnDemand
                    )
    
    mid.Type = 'MergedID'    
    mid.Tracking = useTracks
    return mid

# =============================================================================
## define "MergedID" algorithm
def PhotonFromMergedID ( context , enableRecoOnDemand, useTracks = True ) :
    """
    define 'PhotonFromMergedID' algorithm
    """
    
    from Configurables import   CaloPhotonIdAlg
    
    ## photonID
    pmid = getAlgo ( CaloPhotonIdAlg    ,
                     "PhotonFromMergedID", 
                     context,
                     "Rec/Calo/PhotonFromMergedID",
                     enableRecoOnDemand
                     )
    
    pmid.Type = 'PhotonFromMergedID'    
    pmid.Tracking = useTracks
    return pmid

# =============================================================================
## define "inEcalAcceptance" algorithm
def inEcalAcc ( context , enableRecoOnDemand, trackLocations = [] ) :
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

    if trackLocations :
        inEcal.Inputs   = trackLocations

    
        
    return inEcal

## ============================================================================
## define the minimal track match sequnce for photon recontruction
def trackMatch ( context , enableRecoOnDemand, trackLocations = [] , fastReco = False , external = '' ) :
    """

    Define the minimal track match sequnce for photon reconstruction

    """

    from Configurables import PhotonMatchAlg      
    
    seq = getAlgo ( GaudiSequencer , 'CaloTrackMatch' , context )


    ## perform the actual track <-> cluster match 
    clmatch = getAlgo  (  PhotonMatchAlg          ,
                          'ClusterMatch'          ,
                          context                 ,
                          'Rec/Calo/ClusterMatch' ,
                          enableRecoOnDemand      )

    if external != '' :
        clmatch.Calos =[ external ]
    

#    log.info(" ========= trackLocations = " + str(trackLocations) ) 

    ## check if the track is in Ecal acceptance 
    inEcal = inEcalAcc ( context , enableRecoOnDemand , trackLocations) 

    
    seq.Members = [ inEcal , clmatch ] 

    setTheProperty ( seq , 'Context' , context )


    if trackLocations :
        clmatch.Tracks = trackLocations
        
    log.debug ( "Configure Cluster Track Match : '%s' for  '%s'" %(seq.name(), context) )


    return seq


# =================================================================================
## define various Calo PIDs evaluation
def caloPIDs ( context , enableRecoOnDemand , list , trackLocations = []   , skipNeutrals = False , skipCharged = False ,
               fastPID = False , external = '', name = '', noSpdPrs=False) :
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
                                HcalPIDmuAlg        ,
                                CaloPhotonIdAlg
                                )

    ## global PID sequence
    _name = 'CaloPIDs'
    if name != context or context.upper() == 'OFFLINE': _name = _name+name
    seq = getAlgo ( GaudiSequencer, _name, context )
    seq.Members = []

    ## add Charged
    if not skipCharged :
        _name = 'ChargedPIDs'
        if name != context or context.upper() == 'OFFLINE': _name = _name+name

        charged = getAlgo( GaudiSequencer , _name , context )

        # inAcceptance
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
        inECAL = inEcalAcc ( context , enableRecoOnDemand , trackLocations )         
        inAcc = getAlgo ( GaudiSequencer     ,
                          'InCaloAcceptance' ,
                          context            )


        inAcc.Members = [ inECAL , inHCAL , inBREM ]
        if not noSpdPrs :
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
            inAcc.Members += [ inSPD  , inPRS  ]
        else :
            log.info('remove Spd/Prs from acceptance')

        # matching
        match    = getAlgo ( GaudiSequencer  ,
                             'CaloMatch'     ,
                             context         )
        cluster  = trackMatch ( context , enableRecoOnDemand , trackLocations,fastPID,external)
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


        match.Members = [cluster  , electron, brem ]

        # energy
        energy = getAlgo ( GaudiSequencer ,
                           'CaloEnergy'   ,
                           context        )
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
        energy.Members = [ ecalE , hcalE ]
        if not noSpdPrs :
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
            energy.Members += [ spdE , prsE ]
        else :
            log.info('remove Spd/Prs from energy estimators')


        # Chi2's
        chi2  = getAlgo ( GaudiSequencer , 'CaloChi2', context        )
        eChi2 = getAlgo ( EcalChi22ID, 'EcalChi22ID' , context, 'Rec/Calo/EcalChi2', enableRecoOnDemand   ) 
        bChi2 = getAlgo ( BremChi22ID, 'BremChi22ID' , context, 'Rec/Calo/BremChi2', enableRecoOnDemand   ) 
        cChi2 = getAlgo ( ClusChi22ID, 'ClusChi22ID' , context, 'Rec/Calo/ClusChi2', enableRecoOnDemand   ) 
        chi2.Members = [ eChi2, bChi2 , cChi2 ]


        # DLL
        dlle  = getAlgo ( GaudiSequencer ,
                          'CaloDLLe'     ,
                          context        )
        dllmu  = getAlgo ( GaudiSequencer ,
                          'CaloDLLmu'     ,
                          context        )
        ecale=getAlgo ( EcalPIDeAlg          ,
                        'EcalPIDe'           ,
                        context              ,
                        'Rec/Calo/EcalPIDe'  ,
                        enableRecoOnDemand   )
        breme=getAlgo ( BremPIDeAlg          ,
                        'BremPIDe'           ,
                        context              ,
                        'Rec/Calo/BremPIDe'  ,
                        enableRecoOnDemand   ) 
        hcale=getAlgo ( HcalPIDeAlg          ,
                        'HcalPIDe'           ,
                        context              ,
                        'Rec/Calo/HcalPIDe'  ,
                        enableRecoOnDemand   ) 
        ecalmu=getAlgo ( EcalPIDmuAlg         ,
                         'EcalPIDmu'          ,
                         context              ,
                         'Rec/Calo/EcalPIDmu' ,
                         enableRecoOnDemand   ) 
        hcalmu=getAlgo ( HcalPIDmuAlg         ,
                         'HcalPIDmu'          ,
                         context              ,
                         'Rec/Calo/HcalPIDmu' ,
                         enableRecoOnDemand   ) 
        dllmu.Members = [ ecalmu, hcalmu ]
        dlle.Members  = [ ecale , breme , hcale ]
        if not noSpdPrs :
            prse=getAlgo ( PrsPIDeAlg           ,
                           'PrsPIDe'            ,
                           context              ,
                           'Rec/Calo/PrsPIDe'   ,
                           enableRecoOnDemand   ) 
            dlle.Members  += [ prse ]
        else :
            log.info('remove Spd/Prs from dlle')



        # alternative sequence (per caloPID technique)
        ecalT  = getAlgo ( GaudiSequencer ,'EcalPID'     , context        )
        ecalT.Members = [ inECAL,  cluster, electron, ecalE, eChi2, cChi2, ecale, ecalmu ]

        hcalT  = getAlgo ( GaudiSequencer ,'HcalPID'     , context        )
        hcalT.Members = [ inHCAL,  hcalE, hcale, hcalmu ]


        if not noSpdPrs :
            prsT  = getAlgo ( GaudiSequencer ,'PrsPID'     , context        )
            prsT.Members = [ inPRS,  prsE, prse ]
 
            spdT  = getAlgo ( GaudiSequencer ,'SpdPID'     , context        )
            spdT.Members = [ inSPD,  spdE ]
 
        bremT  = getAlgo ( GaudiSequencer ,'BremPID'     , context        )
        bremT.Members = [ inBREM,  brem, bChi2, breme ]

        # Override CaloAlgUtils default track location
        if  trackLocations :
            electron.Tracks = trackLocations
            brem.Tracks     = trackLocations
            inBREM.Inputs   = trackLocations
            inHCAL.Inputs   = trackLocations
            inPRS.Inputs    = trackLocations
            inSPD.Inputs    = trackLocations
            eChi2.Tracks    = trackLocations
            bChi2.Tracks    = trackLocations
            cChi2.Tracks    = trackLocations
            spdE.Inputs     = trackLocations
            prsE.Inputs     = trackLocations
            ecalE.Inputs    = trackLocations
            hcalE.Inputs    = trackLocations            

        charged.Members = []
        # updatXe global sequence with charged
        if 'InAcceptance' in list : charged.Members += [ inAcc  ]
        if 'Match' in list : charged.Members        += [ match  ]
        if 'Energy' in list : charged.Members       += [ energy ]
        if 'Chi2' in list : charged.Members         += [ chi2   ]
        if 'DLL' in list or 'DLLe' in list : charged.Members          += [ dlle   ]
        if 'DLL' in list or 'DLLmu' in list : charged.Members         += [ dllmu  ]

        # alternative full sequence per technique
        if 'EcalPID' in list :  charged.Members        += [ ecalT  ]
        if 'BremPID' in list :  charged.Members        += [ bremT  ]
        if 'HcalPID' in list :  charged.Members        += [ hcalT  ]
        if 'PrsPID' in list and not noSpdPrs :  charged.Members         += [ prsT   ]
        if 'SpdPID' in list and not noSpdPrs :  charged.Members         += [ spdT   ]
        if charged.Members : addAlgs( seq , charged )
        

    ## Add Neutrals (! No neutralID so far for the noSpdPrs configuration )
    if not skipNeutrals  and not noSpdPrs:
        _name = 'NeutralPIDs'
        if name != context or context.upper() == 'OFFLINE': _name = _name+name
        neutrals = getAlgo( GaudiSequencer , _name , context )
        photonID = PhotonID( context , enableRecoOnDemand )
        mergedID = MergedID( context , enableRecoOnDemand )
        photonFromMergedID = PhotonFromMergedID( context , enableRecoOnDemand )
        # update global sequence with neutrals
        neutrals.Members = []
        if 'PhotonID' in list or 'NeutralPID' in list            : neutrals.Members += [ photonID  ]
        if 'MergedID' in list  or 'NeutralPID' in list           : neutrals.Members += [ mergedID  ]
        if 'PhotonFromMergedID' in list  or 'NeutralPID' in list : neutrals.Members += [ photonFromMergedID ]
        if neutrals.Members : addAlgs( seq , neutrals )



    setTheProperty ( seq , 'Context' , context )
    
    log.debug ( 'Configure Calo PIDs  Reco : %s    for context : %s' %( seq.name() , context) )
        
    return seq



def referencePIDs( dataType='' ) :

    """
    Define various reference Histograms on THS
    """
    hsvc = HistogramSvc ( 'HistogramDataSvc' )
    inputs = hsvc.Input 
    
    # photon PDF default
    pfound  = False   
    for line in inputs : 
        if 0 == line.find ( 'CaloNeutralPIDs') : pfound = True 

    if pfound : 
        log.info ("CaloPIDsConf: LUN 'CaloNeutralPIDs' has been defined already")  
    else: 
        hsvc.Input += [ "CaloNeutralPIDs DATAFILE='$PARAMFILESROOT/data/PhotonPdf.root' TYP='ROOT'" ] 


    # charged PDF default 
    found  = False   
    for line in inputs : 
        if 0 == line.find ( 'CaloPIDs') : found = True 
        
    if found : 
        log.info ("CaloPIDsConf: LUN 'CaloPIDs' has been defined already") 
    elif 'DC06' == dataType : 
        hsvc.Input += [ "CaloPIDs DATAFILE='$PARAMFILESROOT/data/CaloPIDs_DC06_v2.root' TYP='ROOT'" ]
    else: 
        hsvc.Input += [ "CaloPIDs DATAFILE='$PARAMFILESROOT/data/CaloPIDs_DC09_v1.root' TYP='ROOT'" ] 

    return

    
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    
