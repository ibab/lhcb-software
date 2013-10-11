#!/usr/bin/env python
# =============================================================================
# $Id: Monitor.py,v 1.8 2010/05/20 09:55:38 odescham Exp $
# =============================================================================
## The major building blocks of Calorimeter Monitoring
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
The major building blocks of Calorimeter Monitoring
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name:  $, version $Revision: 1.8 $"
# =============================================================================
__all__ = (
    'digitsMoni'     , 
    'eflowMoni'      , 
    'clustersMoni'   , 
    'hyposMoni'      , 
    'pi0Moni'        , 
    'pidsMoni'      
    )
# =============================================================================
from Gaudi.Configuration  import *

from Configurables        import   GaudiSequencer
from CaloKernel.ConfUtils import ( getAlgo        ,
                                   setTheProperty ) 

from GaudiKernel.SystemOfUnits import  ( MeV , GeV )


# =============================================================================
## prepare digit monitoring
def digitsMoni ( context ,noSpdPrs=False) :
    """
    
    Prepare digit monitoring
    
    """
    from Configurables import  ( CaloDigitMonitor ,
                                 SpdMonitor       ) 
    
    alg = getAlgo ( GaudiSequencer   , 
                    "CaloDigitsMoni" ,
                    context          )
    
    alg1 = getAlgo ( CaloDigitMonitor , "EcalDigitMon"  , context ) 
    alg2 = getAlgo ( CaloDigitMonitor , "HcalDigitMon"  , context ) 
    if not alg1.isPropertySet('histoList') :
        alg1.histoList   = [ "1", "2" , "3" , "6" , "7" ]
    if not alg2.isPropertySet('histoList') :
        alg2.histoList   = [ "1", "2" , "3" , "6" , "7" ]

    alg.Members = [alg1,alg2]

    if not noSpdPrs :
        alg3 = getAlgo ( CaloDigitMonitor , "PrsDigitMon"   , context ) 
        alg4 = getAlgo ( CaloDigitMonitor , "SpdDigitMon"   , context ) 
        alg5 =  getAlgo ( SpdMonitor , "SpdMon"   , context )
        if not alg3.isPropertySet('histoList') :
            alg3.histoList   = [ "1", "2" , "3" , "6" , "7" ]
        if not alg4.isPropertySet('histoList') :
            alg4.histoList   = [ "1", "2" , "3" , "6" ]
        alg.Members += [alg3,alg4,alg5]    
    setTheProperty ( alg , 'Context' , context )
    return alg


# ============================================================================
## Define Energy flow monitoring 
def eflowMoni ( context ) :
    """
    Define Energy flow omnitoring 
    """
    
    from Configurables import  CaloEFlowAlg
    
    seq = getAlgo ( GaudiSequencer  , 
                    'CaloEFlowMoni' ,
                    context         )
    
    alg = getAlgo ( CaloEFlowAlg    ,
                    'EcalEFlowMon'  ,
                    context         )

    if not alg.isPropertySet('histoList') :
        alg.histoList       = [ 'all' ]
    if not alg.isPropertySet('SplitAreas') :
        alg.SplitAreas      = False
    if not alg.isPropertySet('OneDimension') :
        alg.OneDimension    =  True
    if not alg.isPropertySet('EtFilterMax') :
        alg.EtFilterMax     =  1 * GeV
    if not alg.isPropertySet('EnergyFilterMax') :
        alg.EnergyFilterMax = 10 * GeV
    
    seq.Members = [ alg ] 

    setTheProperty ( alg , 'Context' , context )
    
    return seq

    
# =============================================================================
## prepare cluster monitoring
def clustersMoni ( context ) :
    """    
    Prepare clusters monitoring
    
    """
    
    from Configurables import  CaloClusterMonitor
    
    alg = getAlgo ( GaudiSequencer     , 
                    "CaloClustersMoni" ,
                    context            )
    
    alg1 = getAlgo ( CaloClusterMonitor ,
                     'EcalClusterMon'   ,
                     context            )
    
    alg2 = getAlgo ( CaloClusterMonitor      ,
                     'EcalSplitClusterMon'  ,
                     context                 )
 
    if not alg1.isPropertySet('histoList') :
        alg1.histoList = [ "1", "2" , "3" , "4" , "7" , "8" ,  "9" ]

#    delegate to CaloAlgUtils  
#    alg1.Input = 'Rec/Calo/EcalClusters'
#    alg2.Input = 'Rec/Calo/EcalSplitClusters'

#    alg.Members = [ alg1 , alg2 ]
    alg.Members = [ alg1 ] ## do not monitor splitCluster so far
    
    setTheProperty ( alg , 'Context' , context )
    
    return alg


# =============================================================================
## prepare hypo monitoring
def hyposMoni ( context ) :
    """    
    Prepare hypo monitoring
    
    """
    
    from Configurables import  CaloHypoMonitor

    alg = getAlgo ( GaudiSequencer    , 
                    "CaloHyposMoni"   ,
                    context           )
    
    alg1 = getAlgo ( CaloHypoMonitor  ,
                     'ElectronMon'    ,
                     context          )
    alg2 = getAlgo ( CaloHypoMonitor  ,
                     'PhotonMon'      ,
                     context          )
    alg3 = getAlgo ( CaloHypoMonitor  ,
                     'SplitPhotonMon' ,
                     context          )
    alg4 = getAlgo ( CaloHypoMonitor  ,
                     'MergedPi0Mon'   ,
                     context          )

    # Delegate I/O to CaloAlgUtils
#    alg1.Input = 'Rec/Calo/Electrons'
#    alg2.Input = 'Rec/Calo/Photons'
#    alg3.Input = 'Rec/Calo/SplitPhotons'
#    alg4.Input = 'Rec/Calo/MergedPi0s'


    if not alg1.isPropertySet('histoList') :
        alg1 .histoList = [ "1", "2" , "3" , "7" , "8" , "9" , "10" , "11" ,"14"]
    if not alg2.isPropertySet('histoList') :
        alg2 .histoList = [ "1", "2" , "3" , "7" , "8" , "9" , "10" , "11" ,"14"]
    if not alg3.isPropertySet('histoList') :
        alg3 .histoList = [ "1", "2" , "3" , "7" , "8" , "9" , "10" , "11" ]
    if not alg4.isPropertySet('histoList') :
        alg4 .histoList = [ "1", "2" , "3" , "4" , "7", "8" , "9","14"]

 
    alg.Members = [ alg1 , alg2 , alg3 , alg4 ]


    setTheProperty ( alg , 'Context' , context )
 
    return alg





# =============================================================================
## Define pi0 monitoring
def pi0sMoni ( context ) :
    """    
    Define pi0 monitoring
    
    """
    
    from Configurables import  CaloPi0Monitor


    alg = getAlgo ( GaudiSequencer    , 
                    "DiPhotonMoni"   ,
                    context           )

    alg1 = getAlgo  ( CaloPi0Monitor   ,
                     'ResolvedPi0Mon' ,
                     context          )

    alg2 = getAlgo  ( CaloPi0Monitor   ,
                      'EtaMon' ,
                      context          )
    
    if not alg1.isPropertySet('PhotonPtFilter') :
        alg1.PhotonPtFilter = 500.*MeV;

    if not alg1.isPropertySet('histoList') :
        alg1.histoList = [ "1", "2" , "3" , "4", "5", "6", "7" ]


    if not alg2.isPropertySet('PhotonMaxPtFilter') :
        alg2.PhotonMaxPtFilter = 1000.*MeV;

    if not alg2.isPropertySet('HistoMassMin') :
        alg2.HistoMassMin = 400.*MeV;

    if not alg2.isPropertySet('HistoMassMax') :
        alg2.HistoMassMax = 700.*MeV;

    if not alg2.isPropertySet('histoList') :
        alg2.histoList = [ "1", "2" , "3" , "4", "5", "6", "7" ]

    alg.Members = [ alg1 , alg2 ]
    setTheProperty ( alg , 'Context' , context )
    
    return alg

# =============================================================================
## Define pi0 monitoring
def protosMoni ( context ) :
    """    
    Define protoElectron monitoring
    
    """
    
    from Configurables import  CaloProtoElectronMonitor

    alg = getAlgo  ( CaloProtoElectronMonitor   ,
                     'ProtoElectronMon' ,
                     context          )


    if not alg.isPropertySet('histoList') :
        alg.histoList = [ "All" ]

    setTheProperty ( alg , 'Context' , context )
    
    return alg

# =============================================================================
## define CaloPIDs monitoring
def pidsMoni ( context, Histograms ) :
    """
    Define CaloPIDs monitoring
    """
    
    from Configurables import ( CaloClusterMatchMonitor ,
                                CaloHypoMatchMonitor    ,
                                CaloEMuPIDMon           ) 
    
    alg = getAlgo ( GaudiSequencer ,
                    'CaloPIDsMoni' ,
                    context        )

    alg1 = getAlgo ( CaloClusterMatchMonitor  ,
                     'PhotonMatchMon'         ,
                     context                  )
    alg2 = getAlgo ( CaloHypoMatchMonitor     ,
                     'ElectronMatchMon'       ,
                     context                  )
    alg3 = getAlgo ( CaloHypoMatchMonitor     ,
                     'BremMatchMon'           ,
                     context                  )

    alg5 = getAlgo ( CaloEMuPIDMon            ,
                     'CaloEMuPIDMonUncut'     ,
                     context                  )

    alg6 = getAlgo ( CaloEMuPIDMon            ,
                     'CaloEMuPIDMonSoft'      ,
                     context                  )

    alg7 = getAlgo ( CaloEMuPIDMon            ,
                     'CaloEMuPIDMonHard'      ,
                     context                  )

    ## Delegate I/O to CaloAlgUtils
#    alg1.Input  =   'Rec/Calo/ClusterMatch'
#    alg1.Inputs = [ 'Rec/Calo/EcalClusters' ]    
#    alg2.Input  =   'Rec/Calo/ElectronMatch'
#    alg2.Inputs = [ 'Rec/Calo/Electrons'    ]    
#    alg3.Input  =   'Rec/Calo/BremMatch'
#    alg3.Inputs = [ 'Rec/Calo/Photons'      ]

    if not alg5.isPropertySet(     'uncut') : alg5.uncut     = True
    if not alg5.isPropertySet('SplitSides') : alg5.SplitSides= True

    if not alg7.isPropertySet(     'pTmin') : alg7.pTmin     = 500.
    if not alg7.isPropertySet(  'RichDLLe') : alg7.RichDLLe  = 4.
    if not alg7.isPropertySet( 'maxEHcalE') : alg7.maxEHcalE = 1000.
    if not alg7.isPropertySet(   'minPrsE') : alg7.minPrsE   =-1.e10

    alg.Members = [ alg1 , alg2 , alg3 ,        alg5, alg6, alg7 ]


    setTheProperty ( alg , 'Context' , context )

    return alg

### ------ CHECKING ----- ###
def recoCheck( context ) :

    from Configurables import ( CaloClusterChecker,CaloPi0Checker)
    
    alg  = getAlgo ( GaudiSequencer    ,'CaloRecoCheck'     , context)
    alg1 = getAlgo ( CaloClusterChecker,'CaloClusterChecker',context)
    alg2 = getAlgo ( CaloPi0Checker    ,'CaloPi0Checker'    ,context)
    alg.Members=[alg1,alg2]
    setTheProperty(alg,'Context',context)
    return alg

def pidCheck( context,noSpdPrs=False ) :

    from Configurables import ( CaloPIDsChecker,CaloPhotonChecker)
    
    alg  = getAlgo ( GaudiSequencer    ,'CaloPIDsCheck'     , context)
    alg1 = pidCheckerConf('PIDeEcalChecker',context,11,"Rec/Calo/EcalPIDe",50.0*GeV,0.0,"Long", "Rec/Calo/InAccEcal" )
    alg2 = pidCheckerConf('PIDeHcalChecker',context,11,"Rec/Calo/HcalPIDe",50.0*GeV,0.0,"Long", "Rec/Calo/InAccHcal" )
    alg3 = pidCheckerConf('PIDeBremChecker',context,11,"Rec/Calo/BremPIDe",50.0*GeV,0.0,"Long", "Rec/Calo/InAccBrem" )
    alg4 = pidCheckerConf('PIDmEcalChecker',context,13,"Rec/Calo/EcalPIDmu",25.0*GeV,0.0,"Long", "Rec/Calo/InAccEcal" )
    alg5 = pidCheckerConf('PIDmHcalChecker',context,13,"Rec/Calo/HcalPIDmu",25.0*GeV,0.0,"Long", "Rec/Calo/InAccHcal" )    
    alg.Members=[alg1,alg2,alg3,alg4,alg5]
    if not noSpdPrs :
        alg6 = pidCheckerConf('PIDePrsChecker',context,11,"Rec/Calo/PrsPIDe",50.0*GeV,0.0,"Long", "Rec/Calo/InAccPrs" )
        alg.Members += [alg6]
        # caloPhotonChecker
        alg7 = getAlgo ( CaloPhotonChecker,'CaloPhotonChecker',context)
        alg7.Pdf=True
        alg7.EPrsBin=[50.,0.,200.]
        alg7.Chi2Bin=[26.,0.,104.]
        alg7.SeedBin=[50.,0.,1.]
#        alg.Members += [alg7]
    setTheProperty(alg,'Context',context)
    return alg

def pidCheckerConf(name,context,particle,input,norm,cut,type,acc) :
    from Configurables import ( CaloPIDsChecker )
    algo = getAlgo ( CaloPIDsChecker,name,context)
    algo.Particle        = particle
    algo.Input           = input
    algo.Normalization   = norm
    algo.Cut             = cut
    algo.TrackType       = type
    algo.TrackAcceptance = acc
    return algo

    
# =============================================================================
if '__main__' == __name__ :
    print __doc__
    
# =============================================================================
# The END
# =============================================================================
