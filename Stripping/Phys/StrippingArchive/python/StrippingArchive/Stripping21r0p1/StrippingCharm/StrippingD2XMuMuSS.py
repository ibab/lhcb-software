
__author__ = ['Malcolm John', 'B. Viaud', 'O. Kochebina']
__date__ = '25/6/2012'
__version__ = '$Revision: 1.7 $'

'''
D->Xu(Xu) mu mu , Xu=pi/K channels and control samples.
'''
__all__ = ('StrippingD2XMuMuSSConf', 'makeD2PiPiPi', 'makeD2PiMuMuOS','makeD2PiEEOS' , 'makeD2PiEMuOS', 'makeD2PiMuEOS', 'makeD2PiMuMuSS', 'makeD2PiEESS', 'makeD2PiMuESS', 'makeD2KMuMuOS', 'makeD2KEEOS' , 'makeD2KEMuOS', 'makeD2KMuEOS', 'makeD2KMuMuSS', 'makeD2KEESS', 'makeD2KMuESS', 'makeD2K2Pi', 'makeD22KPi', 'makeD02KKMuMu', 'makeD02PiPiMuMu', 'makeD02KPiMuMu', 'makeD02KKPiPi', 'makeD02K3Pi','makeD02PiPiPiPi','makeLambdac2PMuMu','makeLambdac2PEE','makeLambdac2PEMu','makeLambdac2PMuE','makeLambdac2PPiPi','makeD2PiMuMuCal','makeD2PiEMuCal','makeD2PiEECal','makeD2KMuMuCal','makeD2KEMuCal','makeD2KEECal','default_config' )

default_config =  {
    'NAME' : 'D2XMuMuSS',
    'BUILDERTYPE' : 'StrippingD2XMuMuSSConf',
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ],
    'CONFIG':{
                  'MuonP'         : 3000. ,    #MeV
                  'MuonPT'        : 500.  ,    #MeV
                  'MuonMINIPCHI2' : 6     ,    #adminensional
                  'MuonTRCHI2'    : 5     ,    #adminensional
                  
                  'PionP'         : 2000. ,    #MeV
                  'PionPT'        : 300.  ,    #MeV
                  'PionMINIPCHI2' : 6     ,    #adminensional
                  'PionTRCHI2'    : 5     ,    #adminensional
                  
                  'KaonP'         : 2000. ,    #MeV
                  'KaonPT'        : 300.  ,    #MeV
                  'KaonPIDK'      : -1.   ,    #adimensional
                  'KaonMINIPCHI2' : 6     ,    #adminensional
                  'KaonTRCHI2'    : 5     ,    #adminensional

                  'ProtonP'         : 2000. ,    #MeV
                  'ProtonPT'        : 300.  ,    #MeV
                  'ProtonMINIPCHI2' : 6     ,    #adminensional
                  'ProtonTRCHI2'    : 5     ,    #adminensional

                  'EleP'         : 3000. ,    #MeV
                  'ElePT'        : 500.  ,    #MeV
                  'EleMINIPCHI2' : 6     ,    #adminensional
                  'EleTRCHI2'    : 5     ,    #adminensional
                  


                  #3 body
                  'DVCHI2DOF'     : 5     ,    #adminensional
                  'DMAXDOCA'      : 0.15  ,    #mm
                  'DIPCHI2'       : 25    ,    #adimensional
                  
                  #4body
                  'MINIPCHI2_hhmumu' : 2. ,  #adminensional
                  'PT_hhmumu'        : 300 , #MeV
                  'MuonPIDmu_hhmumu' : -1,   #adimensional
                  'DPT_hhmumu'           : 2500.,     #MeV
                  'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
                  'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
                  'DdauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
                  'DFDCHI2_hhmumu'       : 36     ,   #adimensional
                  'DIPCHI2_hhmumu'       : 20    ,    #adimensional

                  'DIPCHI2_4bodyCS'      : 16    ,    #adimensional
                  'MINIPCHI2_4bodyCS' : 3. ,  #adminensional

                  'DDIRA'         : 0.9999,    #adimensional
                  'DimuonMass'    : 250.  ,    #MeV
                  'DMassWin'                   : 200.  ,    #MeV, mass window
                  'DMassWin_CS_hhmumu'         : 100.  ,    #MeV, mass window
                  'LambdacMassWin'             : 100. , #MeV, mass window
                  'DMassLow'                   :1763.  ,    #MeV, low-mass veto

             
                  'GhostProbCut_hhmumu'     : 0.5,
                  
                  'D2PiMuMuOSLinePrescale'     : 1 ,
                  'D2PiMuMuOSLinePostscale'    : 1 ,

                  'D2PiEEOSLinePrescale'     : 1 ,
                  'D2PiEEOSLinePostscale'    : 1 ,

                  'D2PiEMuOSLinePrescale'     : 1 ,
                  'D2PiEMuOSLinePostscale'    : 1 ,

                  'D2PiMuEOSLinePrescale'     : 1 ,
                  'D2PiMuEOSLinePostscale'    : 1 ,

                  'D2PiMuMuSSLinePrescale'     : 1 ,
                  'D2PiMuMuSSLinePostscale'    : 1 ,

                  'D2PiEESSLinePrescale'      : 1 ,
                  'D2PiEESSLinePostscale'     : 1 ,

                  'D2PiMuESSLinePrescale'       : 1 ,
                  'D2PiMuESSLinePostscale'      : 1 ,

                  'D2KMuMuOSLinePrescale'      : 1 ,
                  'D2KMuMuOSLinePostscale'     : 1 ,

                  'D2KEEOSLinePrescale'     : 1 ,
                  'D2KEEOSLinePostscale'    : 1 ,

                  'D2KEMuOSLinePrescale'     : 1 ,
                  'D2KEMuOSLinePostscale'    : 1 ,

                  'D2KMuEOSLinePrescale'     : 1 ,
                  'D2KMuEOSLinePostscale'    : 1 ,

                  'D2KMuMuSSLinePrescale'      : 1 ,
                  'D2KMuMuSSLinePostscale'     : 1 ,

                  'D2KEESSLinePrescale'      : 1 ,
                  'D2KEESSLinePostscale'     : 1 ,

                  'D2KMuESSLinePrescale'      : 1 ,
                  'D2KMuESSLinePostscale'     : 1 ,

                  'D2PiPiPiCalLinePrescale'    : 0.01 ,
                  'D2PiPiPiCalLinePostscale'   : 1 ,
                  'D2K2PiLinePrescale'         : 0.005 ,
                  'D2K2PiLinePostscale'        : 1 ,
                  'D22KPiLinePrescale'         : 0.005 ,
                  'D22KPiLinePostscale'        : 1,
                  'D02KKMuMuLinePrescale'      : 1 ,
                  'D02KKMuMuLinePostscale'     : 1 ,
                  'D02PiPiMuMuLinePrescale'    : 1 ,
                  'D02PiPiMuMuLinePostscale'   : 1,
                  'D02KPiMuMuLinePrescale'     : 1 ,
                  'D02KPiMuMuLinePostscale'    : 1,
                  'D02KKPiPiLinePrescale'      : 0.01 ,
                  'D02KKPiPiLinePostscale'     : 1,
                  'D02K3PiLinePrescale'        : 0.005 ,
                  'D02K3PiLinePostscale'       : 1,
                  'D02PiPiPiPiLinePrescale'    : 0.01 ,
                  'D02PiPiPiPiLinePostscale'   : 1,
                  'Lambdac2PMuMuLinePrescale'  : 1 ,
                  'Lambdac2PMuMuLinePostscale' : 1 ,

                  'Lambdac2PEELinePrescale'  : 1 ,
                  'Lambdac2PEELinePostscale' : 1 ,

                  'Lambdac2PEMuLinePrescale'  : 1 ,
                  'Lambdac2PEMuLinePostscale' : 1 ,

                  'Lambdac2PMuELinePrescale'  : 1 ,
                  'Lambdac2PMuELinePostscale' : 1 ,

                  'Lambdac2PPiPiLinePrescale'  : 0.01 ,
                  'Lambdac2PPiPiLinePostscale' : 1,
                  'Lambdac2PPiPiLinePostscale' : 1 , 
                  
                  'D2PiMuMuCalLinePrescale'    : 1 ,
                  'D2PiMuMuCalLinePostscale'   : 1 ,
                  'D2PiEMuCalLinePrescale'    : 1 ,
                  'D2PiEMuCalLinePostscale'   : 1 ,
                  'D2PiEECalLinePrescale'    : 1 ,
                  'D2PiEECalLinePostscale'   : 1 ,
                  'D2KMuMuCalLinePrescale'    : 1 ,
                  'D2KMuMuCalLinePostscale'   : 1 ,
                  'D2KEMuCalLinePrescale'    : 1 ,
                  'D2KEMuCalLinePostscale'   : 1 ,
                  'D2KEECalLinePrescale'    : 1 ,
                  'D2KEECalLinePostscale'   : 1 ,

                  'RelatedInfoTools': [{
                      'Type'              : 'RelInfoVertexIsolation',
                      'Location'          : 'RelInfoVertexIsolation'
                  }, {
                      'Type'              : 'RelInfoVertexIsolationBDT',
                      'Location'          : 'RelInfoVertexIsolationBDT'
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.0,
                      'Location'          : 'RelInfoConeVariables_1.0',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 1.5,
                      'Location'          : 'RelInfoConeVariables_1.5',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoConeVariables',
                      'ConeAngle'         : 2.0,
                      'Location'          : 'RelInfoConeVariables_2.0',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoConeVariablesForEW',
                      'ConeAngle'         : 0.0,
                      'Location'          : 'RelInfoConeVariablesForEW_0.0',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoConeVariablesForEW',
                      'ConeAngle'         : 0.5,
                      'Location'          : 'RelInfoConeVariablesForEW_0.5',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoConeVariablesForEW',
                      'ConeAngle'         : 1.0,
                      'Location'          : 'RelInfoConeVariablesForEW_1.0',
                      'DaughterLocations' : None

                  }, {
                      'Type'              : 'RelInfoConeVariablesForEW',
                      'ConeAngle'         : 1.5,
                      'Location'          : 'RelInfoConeVariablesForEW_1.5',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoTrackIsolationBDT',
                      'Location'          : 'RelInfoTrackIsolationBDT',
                      'DaughterLocations' : None
                  }, {
                      'Type'              : 'RelInfoBstautauCDFIso',
                      'Location'          : 'RelInfoBstautauCDFIso'
                  }, {
                      'Type'              : 'RelInfoBs2MuMuTrackIsolations',
                      'Location'          : 'RelInfoBs2MuMuTrackIsolations',
                      'DaughterLocations' : None,
                      'tracktype'         : 3,
                      'angle'             : 0.27,
                      'fc'                : 0.60,
                      'doca_iso'          : 0.13,
                      'ips'               : 3.0,
                      'svdis'             : -0.15,
                      'svdis_h'           : 30.,
                      'pvdis'             : 0.5,
                      'pvdis_h'           : 40.,
                      'makeTrackCuts'     : False,
                      'IsoTwoBody'        : False
                  }]
                  }
}

import copy

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseKaons, StdLoosePions, StdLooseMuons, StdNoPIDsPions, StdNoPIDsKaons, StdLooseProtons, StdAllNoPIDsProtons
from StandardParticles import StdAllLooseKaons, StdAllLoosePions, StdAllLooseMuons, StdAllNoPIDsPions, StdAllNoPIDsKaons
from StandardParticles import StdAllLooseElectrons, StdAllNoPIDsElectrons

default_name = "D2XMuMuSS"

class StrippingD2XMuMuSSConf(LineBuilder) :
    """
    Builder for D2XMuMu, X = pi/K 
    """
    
    PiCalLine = None

    PiOSLine = None      
    Pi_EE_OSLine = None
    Pi_EMu_OSLine = None 
    Pi_MuE_OSLine = None 
    Pi_EE_SSLine = None
    Pi_MuE_SSLine = None

    KOSLine = None
    K_EE_OSLine = None
    K_EMu_OSLine = None 
    K_MuE_OSLine = None 
    K_EE_SSLine = None
    K_MuE_SSLine = None

    PiSSLine = None
    KSSLine = None

    PiPiLine = None
    KPiLine = None

    KKLine = None
    KKPiPiLine = None
    K3PiLine = None
    PiPiPiPiLine = None
    K2PiLine = None
    TwoKPiLine = None

    Lambdac2PMuMuLine = None
    Lambdac2PEELine = None
    Lambdac2PEMuLine = None
    Lambdac2PMuELine = None

    Lambdac2PPiPiLine = None
    
    PiMuMuCalLine = None
    PiEMuCalLine = None
    PiEECalLine = None
    KMuMuCalLine = None
    KEMuCalLine = None
    KEECalLine = None
        
    __configuration_keys__ = (  'MuonP'        ,    #MeV
                  'MuonPT'        ,    #MeV
                  'MuonMINIPCHI2' ,    #adminensional
                  'MuonTRCHI2'    ,    #adminensional
                  
                  'PionP'         ,    #MeV
                  'PionPT'        ,    #MeV
                  'PionMINIPCHI2' ,    #adminensional
                  'PionTRCHI2'    ,    #adminensional
                  
                  'KaonP'          ,    #MeV
                  'KaonPT'          ,    #MeV
                  'KaonPIDK'        ,    #adimensional
                  'KaonMINIPCHI2'   ,    #adminensional
                  'KaonTRCHI2'      ,    #adminensional

                  'ProtonP'         ,    #MeV
                  'ProtonPT'        ,    #MeV
                  'ProtonMINIPCHI2' ,    #adminensional
                  'ProtonTRCHI2'    ,    #adminensional

                  'EleP'         ,    #MeV
                  'ElePT'        ,    #MeV
                  'EleMINIPCHI2' ,    #adminensional
                  'EleTRCHI2'    ,    #adminensional
                  


                  #3 body
                  'DVCHI2DOF'     ,    #adminensional
                  'DMAXDOCA'      ,    #mm
                  'DIPCHI2'       ,    #adimensional
                  
                  #4body
                  'MINIPCHI2_hhmumu' ,  #adminensional
                  'PT_hhmumu'        , #MeV
                  'MuonPIDmu_hhmumu' ,   #adimensional
                  'DPT_hhmumu'           ,     #MeV
                  'DVCHI2DOF_hhmumu'     ,    #adminensional         
                  'DMAXDOCA_hhmumu'      ,    #mm
                  'DdauMAXIPCHI2_hhmumu' ,    #adimensinal
                  'DFDCHI2_hhmumu'       ,   #adimensional
                  'DIPCHI2_hhmumu'       ,    #adimensional

                  'DIPCHI2_4bodyCS'   ,    #adimensional
                  'MINIPCHI2_4bodyCS' ,  #adminensional

                  'DDIRA'         ,    #adimensional
                  'DimuonMass'    ,    #MeV
                  'DMassWin'                 ,    #MeV, mass window
                  'DMassWin_CS_hhmumu'       ,    #MeV, mass window
                  'LambdacMassWin'           , #MeV, mass window
                  'DMassLow'                 ,    #MeV, low-mass veto

             
                  'GhostProbCut_hhmumu'      ,
                  
                  'D2PiMuMuOSLinePrescale'   ,
                  'D2PiMuMuOSLinePostscale'  ,

                  'D2PiEEOSLinePrescale'     ,
                  'D2PiEEOSLinePostscale'    ,

                  'D2PiEMuOSLinePrescale'    ,
                  'D2PiEMuOSLinePostscale'   ,

                  'D2PiMuEOSLinePrescale'    ,
                  'D2PiMuEOSLinePostscale'   ,

                  'D2PiMuMuSSLinePrescale'   ,
                  'D2PiMuMuSSLinePostscale'  ,

                  'D2PiEESSLinePrescale'     ,
                  'D2PiEESSLinePostscale'    ,

                  'D2PiMuESSLinePrescale'    ,
                  'D2PiMuESSLinePostscale'   ,

                  'D2KMuMuOSLinePrescale'    ,
                  'D2KMuMuOSLinePostscale'   ,

                  'D2KEEOSLinePrescale'      ,
                  'D2KEEOSLinePostscale'     ,

                  'D2KEMuOSLinePrescale'     ,
                  'D2KEMuOSLinePostscale'    ,

                  'D2KMuEOSLinePrescale'     ,
                  'D2KMuEOSLinePostscale'    ,

                  'D2KMuMuSSLinePrescale'     ,
                  'D2KMuMuSSLinePostscale'    ,

                  'D2KEESSLinePrescale'       ,
                  'D2KEESSLinePostscale'      ,

                  'D2KMuESSLinePrescale'      ,
                  'D2KMuESSLinePostscale'     ,

                  'D2PiPiPiCalLinePrescale'   ,
                  'D2PiPiPiCalLinePostscale'  ,
                  'D2K2PiLinePrescale'        ,
                  'D2K2PiLinePostscale'       ,
                  'D22KPiLinePrescale'        ,
                  'D22KPiLinePostscale'       ,
                  'D02KKMuMuLinePrescale'     ,
                  'D02KKMuMuLinePostscale'    ,
                  'D02PiPiMuMuLinePrescale'   ,
                  'D02PiPiMuMuLinePostscale'  ,
                  'D02KPiMuMuLinePrescale'    ,
                  'D02KPiMuMuLinePostscale'   ,
                  'D02KKPiPiLinePrescale'     ,
                  'D02KKPiPiLinePostscale'    ,
                  'D02K3PiLinePrescale'       ,
                  'D02K3PiLinePostscale'      ,
                  'D02PiPiPiPiLinePrescale'   ,
                  'D02PiPiPiPiLinePostscale'  ,
                  'Lambdac2PMuMuLinePrescale' ,
                  'Lambdac2PMuMuLinePostscale',

                  'Lambdac2PEELinePrescale'  ,
                  'Lambdac2PEELinePostscale' ,

                  'Lambdac2PEMuLinePrescale' ,
                  'Lambdac2PEMuLinePostscale',

                  'Lambdac2PMuELinePrescale'  ,
                  'Lambdac2PMuELinePostscale' ,

                  'Lambdac2PPiPiLinePrescale' ,
                  'Lambdac2PPiPiLinePostscale',

                  'D2PiMuMuCalLinePrescale',
                  'D2PiMuMuCalLinePostscale',
                  'D2PiEMuCalLinePrescale',
                  'D2PiEMuCalLinePostscale',
                  'D2PiEECalLinePrescale',
                  'D2PiEECalLinePostscale',
                  'D2KMuMuCalLinePrescale',
                  'D2KMuMuCalLinePostscale',
                  'D2KEMuCalLinePrescale',
                  'D2KEMuCalLinePostscale',
                  'D2KEECalLinePrescale',
                  'D2KEECalLinePostscale', 

                  'RelatedInfoTools'
                                )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        
        PiCalLine_name = name+"_PiCal"
        PiOSLine_name = name+"_PiOS"
        Pi_EE_OSLine_name = name+"_Pi_EE_OS"
        Pi_EMu_OSLine_name = name+"_Pi_EMu_OS"
        Pi_MuE_OSLine_name = name+"_Pi_MuE_OS"
        Pi_EE_SSLine_name = name+"_Pi_EE_SS"
        Pi_MuE_SSLine_name = name+"_Pi_MuE_SS"

        PiSSLine_name = name+"_PiSS"
        KSSLine_name = name+"_KSS"

        KOSLine_name = name+"_KOS"
        K_EE_OSLine_name = name+"_K_EE_OS"
        K_EMu_OSLine_name = name+"_K_EMu_OS"
        K_MuE_OSLine_name = name+"_K_MuE_OS"
        K_EE_SSLine_name = name+"_K_EE_SS"
        K_MuE_SSLine_name = name+"_K_MuE_SS"

        K2PiLine_name = name+"_K2Pi"
        TwoKPiLine_name = name+"_2KPi"

        KKLine_name = name+"_KK"
        PiPiLine_name = name+"_PiPi"
        KPiLine_name = name+"_KPi"
        KKPiPiLine_name = name+"_KKPiPi"
        K3PiLine_name = name+"_K3Pi"
        PiPiPiPiLine_name = name+"_PiPiPiPi"
        
        Lambdac2PMuMuLine_name = name+"_Lambdac2PMuMu"
        Lambdac2PEELine_name = name+"_Lambdac2PEE"
        Lambdac2PEMuLine_name = name+"_Lambdac2PEMu"
        Lambdac2PMuELine_name = name+"_Lambdac2PMuE"

        Lambdac2PPiPiLine_name = name+"_Lambdac2PPiPi"
        
        PiMuMuCalLine_name = name+"_PiMuMuCal"
        PiEMuCalLine_name = name+"_PiEMuCal"
        PiEECalLine_name = name+"_PiEECal"
        KMuMuCalLine_name = name+"_KMuMuCal"
        KEMuCalLine_name = name+"_KEMuCal"
        KEECalLine_name = name+"_KEECal"
        
        # 1 : Make kaons
        selKaons = makeKaons(name="KaonsFor"+name
                             , KaonP = config['KaonP']
                             , KaonPT = config['KaonPT']
                             , KaonPIDK = config['KaonPIDK']
                             , KaonMINIPCHI2 = config['KaonMINIPCHI2']
                             , KaonTRCHI2 = config['KaonTRCHI2'])
        
        # 2 : Make pions
        selPions = makePions(name="PionsFor"+name
                             , PionP = config['PionP']
                             , PionPT = config['PionPT']
                             , PionMINIPCHI2 = config['PionMINIPCHI2']
                             , PionTRCHI2 = config['PionTRCHI2'])
        
        # 3 : Make muons
        selMuons = makeMuons(name="MuonsFor"+name
                             , MuonP = config['MuonP']
                             , MuonPT = config['MuonPT']
                             , MuonMINIPCHI2 = config['MuonMINIPCHI2']
                             , MuonTRCHI2 = config['MuonTRCHI2'])

        # 3b : Make muons candidates from pions
        pionsAsMuons = makePionsAsMuons(name="PionsAsMuonsFor"+name
                                        , MuonP = config['MuonP']
                                        , MuonPT = config['MuonPT']
                                        , MuonMINIPCHI2 = config['MuonMINIPCHI2']
                                        , MuonTRCHI2 = config['MuonTRCHI2'])

        # 4 : Make protons 
        selProtons = makeProtons(name="ProtonsFor"+name
                                 , ProtonP = config['ProtonP']
                                 , ProtonPT = config['ProtonPT']
                                 , ProtonMINIPCHI2 = config['ProtonMINIPCHI2']
                                 , ProtonTRCHI2 = config['ProtonTRCHI2'])



        # 5 : Make Electrons 
        selElectrons = makeElectrons(name="ElectronsFor"+name
                                 , EleP = config['EleP']
                                 , ElePT = config['ElePT']
                                 , EleMINIPCHI2 = config['EleMINIPCHI2']
                                 , EleTRCHI2 = config['EleTRCHI2'])
                                                             
                

        # 1a : Make Kaons for 3 body Control Samples
        selKaonsFor3bodyCS  = makeKaonsFor3bodyCS(name="HadFor3bodyCSAndFor"+name
                                             , KaonP = config['KaonP']
                                             , KaonPT = config['KaonPT']
                                             , KaonMINIPCHI2 = config['KaonMINIPCHI2']
                                             , KaonTRCHI2 = config['KaonTRCHI2'])
        
        # 2a : Make Pions for 3 body  Control samples 
        selPionsFor3bodyCS = makePionsFor3bodyCS(name="PionsFor3bodyCSAndFor"+name
                                                 , PionP = config['PionP']
                                                 , PionPT = config['PionPT']
                                                 , PionMINIPCHI2 = config['PionMINIPCHI2']
                                                 , PionTRCHI2 = config['PionTRCHI2'])

        # 4 : Make protons for 3 body  Control samples 
        selProtonsFor3bodyCS = makeProtonsFor3bodyCS(name="ProtonsFor3bodyCSAndFor"+name
                                 , ProtonP = config['ProtonP']
                                 , ProtonPT = config['ProtonPT']
                                 , ProtonMINIPCHI2 = config['ProtonMINIPCHI2']
                                 , ProtonTRCHI2 = config['ProtonTRCHI2'])



        # 5 : Make Muons for hhmumu 
        selMuonsForhhmumu = makeMuonsForhhmumu(name="MuonsForhhmumuAndFor"+name
                                               , MuonP = config['MuonP']
                                               , MuonPT = config['PT_hhmumu']
                                               , MuonMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                               , MuonTRCHI2 = config['MuonTRCHI2']
                                               , MuonPIDmu_CS_hhmumu = config['MuonPIDmu_hhmumu']
                                               , ghostProbCut = config['GhostProbCut_hhmumu'])

        # 6 : Make Kaons for hhmumu 
        selKaonsForhhmumu  = makeKaonsForhhmumu(name="KaonsForhhmumuAndFor"+name
                                                , KaonP = config['KaonP']
                                                , KaonPT = config['PT_hhmumu']
                                                , KaonPIDK = config['KaonPIDK']
                                                , KaonMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                                , KaonTRCHI2 = config['KaonTRCHI2']
                                                , ghostProbCut = config['GhostProbCut_hhmumu'])

        # 7 : Make Pions for hhmumu 
        selPionsForhhmumu = makePionsForhhmumu(name="PionsForForhhmumuAndFor"+name
                                               , PionP = config['PionP']
                                               , PionPT = config['PT_hhmumu']
                                               , PionMINIPCHI2 = config['MINIPCHI2_hhmumu']
                                               , PionTRCHI2 = config['PionTRCHI2']
                                               , ghostProbCut = config['GhostProbCut_hhmumu'])



        # 6a : Make Kaons for 4 body Control Samples
        selKaonsFor4bodyCS  = makeKaonsFor4bodyCS(name="HadFor4bodyCSAndFor"+name
                                                  , KaonP = config['KaonP']
                                                  , KaonPT = config['PT_hhmumu']
                                                  , KaonMINIPCHI2 = config['MINIPCHI2_4bodyCS']
                                                  , KaonTRCHI2 = config['KaonTRCHI2']
                                                  , ghostProbCut = config['GhostProbCut_hhmumu'])
        
        # 7a : Make Pions for 4 body Control samples 
        selPionsFor4bodyCS = makePionsFor4bodyCS(name="PionsFor4bodyCSAndFor"+name
                                                 , PionP = config['PionP']
                                                 , PionPT = config['PT_hhmumu']
                                                 , PionMINIPCHI2 = config['MINIPCHI2_4bodyCS']
                                                 , PionTRCHI2 = config['PionTRCHI2']
                                                 , ghostProbCut = config['GhostProbCut_hhmumu'])

      

        

                                        
        # 7 : Combine
        selD2PiPiPi   = self._makeD2PiPiPi(name=PiCalLine_name,
                                             pionSel = selPions,
                                             muonSel = pionsAsMuons,
                                             config = config)
        
        selD2PiMuMuOS = self._makeD2PiMuMuOS(name=PiOSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2PiEEOS = self._makeD2PiEEOS(name=Pi_EE_OSLine_name,
                                             pionSel = selPions,
                                             eleSel = selElectrons,
                                             config = config)

        selD2PiEMuOS = self._makeD2PiEMuOS(name=Pi_EMu_OSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             eleSel = selElectrons,
                                             config = config)

        selD2PiMuEOS = self._makeD2PiMuEOS(name=Pi_MuE_OSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             eleSel = selElectrons,
                                             config = config)

        selD2PiMuMuSS = self._makeD2PiMuMuSS(name=PiSSLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2PiEESS = self._makeD2PiEESS(name=Pi_EE_SSLine_name,
                                         pionSel = selPions,
                                         eleSel = selElectrons,
                                         config = config)

        selD2PiMuESS = self._makeD2PiMuESS(name=Pi_MuE_SSLine_name,
                                           pionSel = selPions,
                                           muonSel = selMuons,
                                           eleSel = selElectrons,
                                           config = config)

        selD2KMuMuOS = self._makeD2KMuMuOS(name=KOSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        selD2KEEOS = self._makeD2KEEOS(name=K_EE_OSLine_name,
                                             kaonSel = selKaons,
                                             eleSel = selElectrons,
                                             config = config)

        selD2KEMuOS = self._makeD2KEMuOS(name=K_EMu_OSLine_name,
                                             kaonSel = selKaons,
                                             muonSel = selMuons,
                                             eleSel = selElectrons,
                                             config = config)

        selD2KMuEOS = self._makeD2KMuEOS(name=K_MuE_OSLine_name,
                                             kaonSel = selKaons,
                                             muonSel = selMuons,
                                             eleSel = selElectrons,
                                             config = config)

        selD2KMuMuSS = self._makeD2KMuMuSS(name=KSSLine_name,
                                           kaonSel = selKaons,
                                           muonSel = selMuons,
                                           config = config)

        selD2KEESS = self._makeD2KEESS(name=K_EE_SSLine_name,
                                       kaonSel = selKaons,
                                       eleSel = selElectrons,
                                       config = config)

        selD2KMuESS = self._makeD2KMuESS(name=K_MuE_SSLine_name,
                                         kaonSel = selKaons,
                                         muonSel = selMuons,
                                         eleSel = selElectrons,
                                         config = config)

        selD2K2Pi = self._makeD2K2Pi(name=K2PiLine_name,
                                     kaonSel = selKaonsFor3bodyCS,
                                     pionSel = selPionsFor3bodyCS,
                                     config = config)
        
        selD22KPi = self._makeD22KPi(name=TwoKPiLine_name,
                                     kaonSel = selKaonsFor3bodyCS,
                                     pionSel = selPionsFor3bodyCS,
                                     config = config)
        
        
        
        selD02KKMuMu = self._makeD02KKMuMu(name=KKLine_name,
                                           kaonSel = selKaonsForhhmumu,
                                           muonSel = selMuonsForhhmumu,
                                           config = config)
        
        selD02PiPiMuMu = self._makeD02PiPiMuMu(name=PiPiLine_name,
                                           pionSel = selPionsForhhmumu,
                                           muonSel = selMuonsForhhmumu,
                                           config = config)
        
        selD02KPiMuMu = self._makeD02KPiMuMu(name=KPiLine_name,
                                             kaonSel = selKaonsForhhmumu,
                                             pionSel = selPionsForhhmumu,
                                             muonSel = selMuonsForhhmumu,
                                             config = config)
        
        selD02KKPiPi = self._makeD02KKPiPi(name=KKPiPiLine_name,
                                           kaonSel = selKaonsFor4bodyCS,
                                           pionSel = selPionsFor4bodyCS,
                                           config = config)

        selD02K3Pi = self._makeD02K3Pi(name=K3PiLine_name,
                                           kaonSel = selKaonsFor4bodyCS,
                                           pionSel = selPionsFor4bodyCS,
                                           config = config)

        selD02PiPiPiPi = self._makeD02PiPiPiPi(name=PiPiPiPiLine_name,
                                       pionSel = selPionsFor4bodyCS,
                                       config = config)

        selLambdac2PMuMu   = self._makeLambdac2PMuMu(name=Lambdac2PMuMuLine_name,
                                                     protonSel = selProtons,
                                                     muonSel = selMuons,
                                                     config = config)
        

        selLambdac2PEE = self._makeLambdac2PEE(name=Lambdac2PEELine_name,
                                             protonSel = selProtons,
                                             eleSel = selElectrons,
                                             config = config)

        selLambdac2PEMu = self._makeLambdac2PEMu(name=Lambdac2PEMuLine_name,
                                             protonSel = selProtons,
                                             muonSel = selMuons,
                                             eleSel = selElectrons,
                                             config = config)

        selLambdac2PMuE = self._makeLambdac2PMuE(name=Lambdac2PMuELine_name,
                                             protonSel = selProtons,
                                             muonSel = selMuons,
                                             eleSel = selElectrons,
                                             config = config)


        selLambdac2PPiPi   = self._makeLambdac2PPiPi(name=Lambdac2PPiPiLine_name,
                                                     protonSel = selProtonsFor3bodyCS,
                                                     muonSel = selPionsFor3bodyCS,
                                                     config = config)
        selD2PiMuMuCal = self._makeD2PiMuMuCal(name=PiMuMuCalLine_name,
                                             pionSel = selPions,
                                             muonSel = selMuons,
                                             config = config)

        selD2PiEMuCal = self._makeD2PiEMuCal(name=PiEMuCalLine_name,
                                             pionSel = selPions,
                                             eleSel = selElectrons,
                                             muonSel = selMuons,
                                             config = config)

        selD2PiEECal = self._makeD2PiEECal(name=PiEECalLine_name,
                                             pionSel = selPions,
                                             eleSel = selElectrons,
                                             config = config)

        selD2KMuMuCal = self._makeD2KMuMuCal(name=KMuMuCalLine_name,
                                             kaonSel = selKaons,
                                             muonSel = selMuons,
                                             config = config)

        selD2KEMuCal = self._makeD2KEMuCal(name=KEMuCalLine_name,
                                             kaonSel = selKaons,
                                             eleSel = selElectrons,
                                             muonSel = selMuons,
                                             config = config)
        selD2KEECal = self._makeD2KEECal(name=KEECalLine_name,
                                             kaonSel = selKaons,
                                             eleSel = selElectrons,
                                             config = config)

        
        # 8 : Declare Lines
        self.PiCalLine = StrippingLine(PiCalLine_name+"Line",
                                      prescale = config['D2PiPiPiCalLinePrescale'],
                                      postscale = config['D2PiPiPiCalLinePostscale'],
                                      selection = selD2PiPiPi,
                                      RelatedInfoTools = _format_related_info(selD2PiPiPi, config['RelatedInfoTools'])
                                       )

        self.PiOSLine = StrippingLine(PiOSLine_name+"Line",
                                      prescale = config['D2PiMuMuOSLinePrescale'],
                                      postscale = config['D2PiMuMuOSLinePostscale'],
                                      selection = selD2PiMuMuOS,
                                      RelatedInfoTools = _format_related_info(selD2PiMuMuOS, config['RelatedInfoTools'])
                                      )

        self.Pi_EE_OSLine = StrippingLine(Pi_EE_OSLine_name+"Line",
                                      prescale = config['D2PiEEOSLinePrescale'],
                                      postscale = config['D2PiEEOSLinePostscale'],
                                      selection = selD2PiEEOS,
                                      RelatedInfoTools = _format_related_info(selD2PiEEOS, config['RelatedInfoTools'])
                                      )
        self.Pi_EMu_OSLine = StrippingLine(Pi_EMu_OSLine_name+"Line",
                                      prescale = config['D2PiEMuOSLinePrescale'],
                                      postscale = config['D2PiEMuOSLinePostscale'],
                                      selection = selD2PiEMuOS,
                                      RelatedInfoTools = _format_related_info(selD2PiEMuOS, config['RelatedInfoTools'])
                                      )

        self.Pi_MuE_OSLine = StrippingLine(Pi_MuE_OSLine_name+"Line",
                                      prescale = config['D2PiMuEOSLinePrescale'],
                                      postscale = config['D2PiMuEOSLinePostscale'],
                                      selection = selD2PiMuEOS,
                                      RelatedInfoTools = _format_related_info(selD2PiMuEOS, config['RelatedInfoTools'])
                                      )
        
        self.PiSSLine = StrippingLine(PiSSLine_name+"Line",
                                      prescale = config['D2PiMuMuSSLinePrescale'],
                                      postscale = config['D2PiMuMuSSLinePostscale'],
                                      selection = selD2PiMuMuSS,
                                      RelatedInfoTools = _format_related_info(selD2PiMuMuSS, config['RelatedInfoTools'])
                                      )
        self.Pi_EE_SSLine = StrippingLine(Pi_EE_SSLine_name+"Line",
                                          prescale = config['D2PiEESSLinePrescale'],
                                          postscale = config['D2PiEESSLinePostscale'],
                                          selection = selD2PiEESS,
                                          RelatedInfoTools = _format_related_info(selD2PiEESS, config['RelatedInfoTools'])
                                          )
        self.Pi_MuE_SSLine = StrippingLine(Pi_MuE_SSLine_name+"Line",
                                           prescale = config['D2PiMuESSLinePrescale'],
                                           postscale = config['D2PiMuESSLinePostscale'],
                                           selection = selD2PiMuESS,
                                           RelatedInfoTools = _format_related_info(selD2PiMuESS, config['RelatedInfoTools'])
                                           )

        self.KOSLine = StrippingLine(KOSLine_name+"Line",
                                     prescale = config['D2KMuMuOSLinePrescale'],
                                     postscale = config['D2KMuMuOSLinePostscale'],
                                     selection = selD2KMuMuOS,
                                     RelatedInfoTools = _format_related_info(selD2KMuMuOS, config['RelatedInfoTools'])
                                     )

        self.K_EE_OSLine = StrippingLine(K_EE_OSLine_name+"Line",
                                      prescale = config['D2KEEOSLinePrescale'],
                                      postscale = config['D2KEEOSLinePostscale'],
                                      selection = selD2KEEOS,
                                      RelatedInfoTools = _format_related_info(selD2KEEOS, config['RelatedInfoTools'])
                                      )
        self.K_EMu_OSLine = StrippingLine(K_EMu_OSLine_name+"Line",
                                      prescale = config['D2KEMuOSLinePrescale'],
                                      postscale = config['D2KEMuOSLinePostscale'],
                                      selection = selD2KEMuOS,
                                      RelatedInfoTools = _format_related_info(selD2KEMuOS, config['RelatedInfoTools'])
                                      )

        self.K_MuE_OSLine = StrippingLine(K_MuE_OSLine_name+"Line",
                                      prescale = config['D2KMuEOSLinePrescale'],
                                      postscale = config['D2KMuEOSLinePostscale'],
                                      selection = selD2KMuEOS,
                                      RelatedInfoTools = _format_related_info(selD2KMuEOS, config['RelatedInfoTools'])
                                      )

        self.KSSLine = StrippingLine(KSSLine_name+"Line",
                                     prescale = config['D2KMuMuSSLinePrescale'],
                                     postscale = config['D2KMuMuSSLinePostscale'],
                                     selection = selD2KMuMuSS,
                                     RelatedInfoTools = _format_related_info(selD2KMuMuSS, config['RelatedInfoTools'])
                                     )

        self.K_EE_SSLine = StrippingLine(K_EE_SSLine_name+"Line",
                                         prescale = config['D2KEESSLinePrescale'],
                                         postscale = config['D2KEESSLinePostscale'],
                                         selection = selD2KEESS,
                                         RelatedInfoTools = _format_related_info(selD2KEESS, config['RelatedInfoTools'])
                                         )
        
        self.K_MuE_SSLine = StrippingLine(K_MuE_SSLine_name+"Line",
                                          prescale = config['D2KMuESSLinePrescale'],
                                          postscale = config['D2KMuESSLinePostscale'],
                                          selection = selD2KMuESS,
                                          RelatedInfoTools = _format_related_info(selD2KMuESS, config['RelatedInfoTools'])
                                          )

        self.K2PiLine = StrippingLine(K2PiLine_name+"Line",
                                      prescale = config['D2K2PiLinePrescale'],
                                      postscale = config['D2K2PiLinePostscale'],
                                      selection = selD2K2Pi,
                                      RelatedInfoTools = _format_related_info(selD2K2Pi, config['RelatedInfoTools'])
                                      )

        self.TwoKPiLine = StrippingLine(TwoKPiLine_name+"Line",
                                        prescale = config['D22KPiLinePrescale'],
                                        postscale = config['D22KPiLinePostscale'],
                                        selection = selD22KPi,
                                        RelatedInfoTools = _format_related_info(selD22KPi, config['RelatedInfoTools'])
                                        )

        
        
        self.KKLine = StrippingLine(KKLine_name+"Line",
                                    prescale = config['D02KKMuMuLinePrescale'],
                                    postscale = config['D02KKMuMuLinePostscale'],
                                    selection = selD02KKMuMu
                                    )
        
        self.PiPiLine = StrippingLine(PiPiLine_name+"Line",
                                    prescale = config['D02PiPiMuMuLinePrescale'],
                                    postscale = config['D02PiPiMuMuLinePostscale'],
                                    selection = selD02PiPiMuMu
                                    )
        
        self.KPiLine = StrippingLine(KPiLine_name+"Line",
                                     prescale = config['D02KPiMuMuLinePrescale'],
                                     postscale = config['D02KPiMuMuLinePostscale'],
                                     selection = selD02KPiMuMu
                                     )
        
        self.KKPiPiLine = StrippingLine(KKPiPiLine_name+"Line",
                                    prescale = config['D02KKPiPiLinePrescale'],
                                    postscale = config['D02KKPiPiLinePostscale'],
                                    selection = selD02KKPiPi
                                    )

        self.K3PiLine = StrippingLine(K3PiLine_name+"Line",
                                        prescale = config['D02K3PiLinePrescale'],
                                        postscale = config['D02K3PiLinePostscale'],
                                        selection = selD02K3Pi
                                        )

        self.PiPiPiPiLine = StrippingLine(PiPiPiPiLine_name+"Line",
                                        prescale = config['D02PiPiPiPiLinePrescale'],
                                        postscale = config['D02PiPiPiPiLinePostscale'],
                                        selection = selD02PiPiPiPi
                                        )

 
        self.Lambdac2PMuMuLine = StrippingLine(Lambdac2PMuMuLine_name+"Line",
                                               prescale = config['Lambdac2PMuMuLinePrescale'],
                                               postscale = config['Lambdac2PMuMuLinePostscale'],
                                               selection = selLambdac2PMuMu
                                               )

        self.Lambdac2PEELine = StrippingLine(Lambdac2PEELine_name+"Line",
                                               prescale = config['Lambdac2PEELinePrescale'],
                                               postscale = config['Lambdac2PEELinePostscale'],
                                               selection = selLambdac2PEE
                                               )
        self.Lambdac2PEMuLine = StrippingLine(Lambdac2PEMuLine_name+"Line",
                                               prescale = config['Lambdac2PEMuLinePrescale'],
                                               postscale = config['Lambdac2PEMuLinePostscale'],
                                               selection = selLambdac2PEMu
                                               )

        self.Lambdac2PMuELine = StrippingLine(Lambdac2PMuELine_name+"Line",
                                               prescale = config['Lambdac2PMuELinePrescale'],
                                               postscale = config['Lambdac2PMuELinePostscale'],
                                               selection = selLambdac2PMuE
                                               )



        self.Lambdac2PPiPiLine = StrippingLine(Lambdac2PPiPiLine_name+"Line",
                                               prescale = config['Lambdac2PPiPiLinePrescale'],
                                               postscale = config['Lambdac2PPiPiLinePostscale'],
                                               selection = selLambdac2PPiPi
                                               )
        
        self.PiMuMuCalLine = StrippingLine(PiMuMuCalLine_name+"Line",
                                      prescale = config['D2PiMuMuCalLinePrescale'],
                                      postscale = config['D2PiMuMuCalLinePostscale'],
                                      selection = selD2PiMuMuCal,
                                      RelatedInfoTools = _format_related_info(selD2PiMuMuCal, config['RelatedInfoTools'])
                                       )
        
        self.PiEMuCalLine = StrippingLine(PiEMuCalLine_name+"Line",
                                      prescale = config['D2PiEMuCalLinePrescale'],
                                      postscale = config['D2PiEMuCalLinePostscale'],
                                      selection = selD2PiEMuCal,
                                      RelatedInfoTools = _format_related_info(selD2PiEMuCal, config['RelatedInfoTools'])
                                       )

        self.PiEECalLine = StrippingLine(PiEECalLine_name+"Line",
                                      prescale = config['D2PiEECalLinePrescale'],
                                      postscale = config['D2PiEECalLinePostscale'],
                                      selection = selD2PiEECal,
                                      RelatedInfoTools = _format_related_info(selD2PiEECal, config['RelatedInfoTools'])
                                       )

        self.KMuMuCalLine = StrippingLine(KMuMuCalLine_name+"Line",
                                      prescale = config['D2KMuMuCalLinePrescale'],
                                      postscale = config['D2KMuMuCalLinePostscale'],
                                      selection = selD2KMuMuCal,
                                      RelatedInfoTools = _format_related_info(selD2KMuMuCal, config['RelatedInfoTools'])
                                       )
        
        self.KEMuCalLine = StrippingLine(KEMuCalLine_name+"Line",
                                      prescale = config['D2KEMuCalLinePrescale'],
                                      postscale = config['D2KEMuCalLinePostscale'],
                                      selection = selD2KEMuCal,
                                      RelatedInfoTools = _format_related_info(selD2KEMuCal, config['RelatedInfoTools'])
                                       )

        self.KEECalLine = StrippingLine(KEECalLine_name+"Line",
                                      prescale = config['D2KEECalLinePrescale'],
                                      postscale = config['D2KEECalLinePostscale'],
                                      selection = selD2KEECal,
                                      RelatedInfoTools = _format_related_info(selD2KEECal, config['RelatedInfoTools'])
                                       )

        # 9 : register Line
        self.registerLine( self.KSSLine )
        self.registerLine( self.KOSLine )
        self.registerLine( self.K_EE_OSLine )
        self.registerLine( self.K_EMu_OSLine )
        self.registerLine( self.K_MuE_OSLine )
        self.registerLine( self.K_EE_SSLine )
        self.registerLine( self.K_MuE_SSLine )

        self.registerLine( self.PiSSLine )
        self.registerLine( self.PiOSLine )
        self.registerLine( self.Pi_EE_OSLine )
        self.registerLine( self.Pi_EMu_OSLine )
        self.registerLine( self.Pi_MuE_OSLine )
        self.registerLine( self.Pi_EE_SSLine )
        self.registerLine( self.Pi_MuE_SSLine )

        self.registerLine( self.PiCalLine )
        self.registerLine( self.K2PiLine )
        self.registerLine( self.TwoKPiLine )
        
        self.registerLine( self.KKLine )
        self.registerLine( self.PiPiLine )
        self.registerLine( self.KPiLine )
        self.registerLine( self.KKPiPiLine )
        self.registerLine( self.K3PiLine )
        self.registerLine( self.PiPiPiPiLine )

        self.registerLine( self.Lambdac2PMuMuLine )
        self.registerLine( self.Lambdac2PEELine )
        self.registerLine( self.Lambdac2PEMuLine )
        self.registerLine( self.Lambdac2PMuELine )

        self.registerLine( self.Lambdac2PPiPiLine )
        
        self.registerLine( self.PiMuMuCalLine )
        self.registerLine( self.PiEMuCalLine )
        self.registerLine( self.PiEECalLine )
        self.registerLine( self.KMuMuCalLine )
        self.registerLine( self.KEMuCalLine )
        self.registerLine( self.KEECalLine )

#####################################################
    def _makeD2PiPiPi(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiPiPi
        """
        return makeD2PiPiPi(name
                            , pionSel
                            , muonSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , DMassWin = config['DMassWin']
                            , DMassLow = config['DMassLow']
                            , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuMuOS(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuOS
        """
        return makeD2PiMuMuOS(name
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiEEOS(self, name, pionSel, eleSel, config):
        """
        Handy interface for D2PiEEOS
        """
        return makeD2PiEEOS(name
                              , pionSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiEMuOS(self, name, pionSel, muonSel, eleSel, config):
        """
        Handy interface for D2PiEMuOS
        """
        return makeD2PiEMuOS(name
                              , pionSel
                              , muonSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuEOS(self, name, pionSel, muonSel, eleSel, config):
        """
        Handy interface for D2PiMuEOS
        """
        return makeD2PiMuEOS(name
                              , pionSel
                              , muonSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD2PiMuMuSS(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuSS
        """
        return makeD2PiMuMuSS(name
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiEESS(self, name, pionSel, eleSel, config):
        """
        Handy interface for D2PiEESS
        """
        return makeD2PiEESS(name
                            , pionSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , DMassWin = config['DMassWin']
                            , DMassLow = config['DMassLow']
                            , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuESS(self, name, pionSel, muonSel, eleSel, config):
        """
        Handy interface for D2PiMuESS
        """
        return makeD2PiMuESS(name
                            , pionSel
                            , muonSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , DMassWin = config['DMassWin']
                            , DMassLow = config['DMassLow']
                            , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuMuOS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2XMuMu
        """
        return makeD2KMuMuOS(name
                             , kaonSel
                             , muonSel
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD2KEEOS(self, name, kaonSel, eleSel, config):
        """
        Handy interface for D2KEEOS
        """
        return makeD2KEEOS(name
                              , kaonSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KEMuOS(self, name, kaonSel, muonSel, eleSel, config):
        """
        Handy interface for D2KEMuOS
        """
        return makeD2KEMuOS(name
                              , kaonSel
                              , muonSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuEOS(self, name, kaonSel, muonSel, eleSel, config):
        """
        Handy interface for D2KMuEOS
        """
        return makeD2KMuEOS(name
                              , kaonSel
                              , muonSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])




#####################################################
    def _makeD2KMuMuSS(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2XMuMu
        """
        return makeD2KMuMuSS(name
                             , kaonSel
                             , muonSel
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD2K2Pi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D2K2pi
        """
        return makeD2K2Pi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KEESS(self, name, kaonSel, eleSel, config):
        """
        Handy interface for D2KEESS
        """
        return makeD2KEESS(name
                            , kaonSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , DMassWin = config['DMassWin']
                            , DMassLow = config['DMassLow']
                            , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuESS(self, name, kaonSel, muonSel, eleSel, config):
        """
        Handy interface for D2KMuESS
        """
        return makeD2KMuESS(name
                            , kaonSel
                            , muonSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , DMassWin = config['DMassWin']
                            , DMassLow = config['DMassLow']
                            , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD22KPi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D22Kpi
        """
        return makeD22KPi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DMAXDOCA = config['DMAXDOCA']
                             , DVCHI2DOF = config['DVCHI2DOF']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2']
                             , DMassWin = config['DMassWin']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])





#####################################################
    def _makeD02KKMuMu(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D0KKMuMu
        """
        return makeD02KKMuMu(name
                             , kaonSel
                             , muonSel
                             , DMAXDOCA = config['DMAXDOCA_hhmumu']
                             , DPT = config['DPT_hhmumu']
                             , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                             , DFDCHI2= config['DFDCHI2_hhmumu']
                             , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2_hhmumu']
                             , DMassWin = config['DMassWin_CS_hhmumu']
                             , DMassLow = config['DMassLow']
                             , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeD02PiPiMuMu(self, name, pionSel, muonSel, config):
        """
        Handy interface for D0PiPiMuMu
        """
        return makeD02PiPiMuMu(name
                               , pionSel
                               , muonSel
                               , DMAXDOCA = config['DMAXDOCA_hhmumu']
                               , DPT = config['DPT_hhmumu']
                               , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                               , DFDCHI2= config['DFDCHI2_hhmumu']
                               , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                               , DDIRA = config['DDIRA']
                               , DIPCHI2 = config['DIPCHI2_hhmumu']
                               , DMassWin = config['DMassWin_CS_hhmumu']
                               , DMassLow = config['DMassLow']
                               , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD02KPiMuMu(self, name, kaonSel, pionSel, muonSel, config):
        """
        Handy interface for D0KPiMuMu
        """
        return makeD02KPiMuMu(name
                              , kaonSel
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA_hhmumu']
                              , DPT = config['DPT_hhmumu']
                              , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                              , DFDCHI2= config['DFDCHI2_hhmumu']
                              , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2_hhmumu']
                              , DMassWin = config['DMassWin_CS_hhmumu']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])
    
#####################################################
    def _makeD02KKPiPi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D0KKpipi
        """
        return makeD02KKPiPi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DMAXDOCA = config['DMAXDOCA_hhmumu']
                             , DPT = config['DPT_hhmumu']
                             , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                             , DFDCHI2= config['DFDCHI2_hhmumu']
                             , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2_4bodyCS']
                             , DMassWin = config['DMassWin_CS_hhmumu']
                             , DMassLow = config['DMassLow'])

#####################################################
    def _makeD02K3Pi(self, name, kaonSel, pionSel, config):
        """
        Handy interface for D0K3pi
        """
        return makeD02K3Pi(name
                             , kaonSel
                             , pionSel
                             , OneKaonPID = config['KaonPIDK']
                             , DPT = config['DPT_hhmumu'] 
                             , DMAXDOCA = config['DMAXDOCA_hhmumu']
                             , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                             , DFDCHI2= config['DFDCHI2_hhmumu']
                             , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                             , DDIRA = config['DDIRA']
                             , DIPCHI2 = config['DIPCHI2_4bodyCS']
                             , DMassWin = config['DMassWin_CS_hhmumu']
                             , DMassLow = config['DMassLow'])


#####################################################
    def _makeD02PiPiPiPi(self, name, pionSel, config):
        """
        Handy interface for D0pipipipi
        """
        return makeD02PiPiPiPi(name
                               , pionSel
                               , DMAXDOCA = config['DMAXDOCA_hhmumu']
                               , DPT = config['DPT_hhmumu']
                               , DdauMAXIPCHI2= config['DdauMAXIPCHI2_hhmumu']
                               , DFDCHI2= config['DFDCHI2_hhmumu']
                               , DVCHI2DOF = config['DVCHI2DOF_hhmumu']
                               , DDIRA = config['DDIRA']
                               , DIPCHI2 = config['DIPCHI2_4bodyCS']
                               , DMassWin = config['DMassWin_CS_hhmumu']
                               , DMassLow = config['DMassLow'])



#####################################################
    def _makeLambdac2PMuMu(self, name, protonSel, muonSel, config):
        """
        Handy interface for Lambdac2PMuMu
        """
        return makeLambdac2PMuMu(name
                            , protonSel
                            , muonSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , LambdacMassWin = config['LambdacMassWin']
                            , DimuonMass = config['DimuonMass'])



#####################################################
    def _makeLambdac2PEE(self, name, protonSel, eleSel, config):
        """
        Handy interface for Lambdac2PEE
        """
        return makeLambdac2PEE(name
                            , protonSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , LambdacMassWin = config['LambdacMassWin']
                            , DimuonMass = config['DimuonMass'])


#####################################################
    def _makeLambdac2PEMu(self, name, protonSel, muonSel, eleSel, config):
        """
        Handy interface for Lambdac2PEMu
        """
        return makeLambdac2PEMu(name
                            , protonSel
                            , muonSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , LambdacMassWin = config['LambdacMassWin']
                            , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeLambdac2PMuE(self, name, protonSel, muonSel, eleSel, config):
        """
        Handy interface for Lambdac2PMuE
        """
        return makeLambdac2PMuE(name
                            , protonSel
                            , muonSel
                            , eleSel
                            , DMAXDOCA = config['DMAXDOCA']
                            , DVCHI2DOF = config['DVCHI2DOF']
                            , DDIRA = config['DDIRA']
                            , DIPCHI2 = config['DIPCHI2']
                            , LambdacMassWin = config['LambdacMassWin']
                            , DimuonMass = config['DimuonMass'])



#####################################################
    def _makeLambdac2PPiPi(self, name, protonSel, muonSel, config):
        """
        Handy interface for Lambdac2PMuMu
        """
        return makeLambdac2PPiPi(name
                                 , protonSel
                                 , muonSel
                                 , DMAXDOCA = config['DMAXDOCA']
                                 , DVCHI2DOF = config['DVCHI2DOF']
                                 , DDIRA = config['DDIRA']
                                 , DIPCHI2 = config['DIPCHI2']
                                 , LambdacMassWin = config['LambdacMassWin']
                                 , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiMuMuCal(self, name, pionSel, muonSel, config):
        """
        Handy interface for D2PiMuMuCal
        """
        return makeD2PiMuMuCal(name
                              , pionSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiEMuCal(self, name, pionSel, eleSel, muonSel, config):
        """
        Handy interface for D2PiEMuCal
        """
        return makeD2PiEMuCal(name
                              , pionSel
                              , eleSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2PiEECal(self, name, pionSel, eleSel, config):
        """
        Handy interface for D2PiEECal
        """
        return makeD2PiEECal(name
                              , pionSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KMuMuCal(self, name, kaonSel, muonSel, config):
        """
        Handy interface for D2KMuMuCal
        """
        return makeD2KMuMuCal(name
                              , kaonSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KEMuCal(self, name, kaonSel, eleSel, muonSel, config):
        """
        Handy interface for D2KEMuCal
        """
        return makeD2KEMuCal(name
                              , kaonSel
                              , eleSel
                              , muonSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])

#####################################################
    def _makeD2KEECal(self, name, kaonSel, eleSel, config):
        """
        Handy interface for D2KEECal
        """
        return makeD2KEECal(name
                              , kaonSel
                              , eleSel
                              , DMAXDOCA = config['DMAXDOCA']
                              , DVCHI2DOF = config['DVCHI2DOF']
                              , DDIRA = config['DDIRA']
                              , DIPCHI2 = config['DIPCHI2']
                              , DMassWin = config['DMassWin']
                              , DMassLow = config['DMassLow']
                              , DimuonMass = config['DimuonMass'])


#####################################################
#
# Out of class
#####################################################
def makeD2PiPiPi(name, pionSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ (pi+ pi-)
    """
    rhos=Selection("pre"+name,
                   Algorithm = CombineParticles(DecayDescriptor = "rho(770)0 -> pi+ pi-",MotherCut = "ALL"),
                   RequiredSelections=[muonSel] )

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ rho(770)0]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ rhos, pionSel ] )

#####################################################
def makeD2PiMuMuOS(name, pionSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ mu+ mu- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel ] )


#####################################################
def makeD2PiEEOS(name, pionSel, eleSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ e+ e- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ e+ e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, pionSel ] )

#####################################################
def makeD2PiEMuOS(name, pionSel, muonSel, eleSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ e+ mu- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ e+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, pionSel, muonSel ] )

#####################################################
def makeD2PiMuEOS(name, pionSel, muonSel, eleSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ mu+ e- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ mu+ e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, pionSel, muonSel ] )


#####################################################
def makeD2PiMuMuSS(name, pionSel, muonSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> pi+ mu- mu-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> pi+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [  muonSel, pionSel ] )

#####################################################
def makeD2PiEESS(name, pionSel, eleSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> pi+ e- e-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> pi+ e- e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [  eleSel, pionSel ] )

#####################################################
def makeD2PiMuESS(name, pionSel, muonSel, eleSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> pi+ mu- e-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> pi+ mu- e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [  muonSel, pionSel, eleSel ] )

#####################################################
def makeD2KMuMuOS(name, kaonSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ mu+ mu-
    """
    
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )


#####################################################
def makeD2KEEOS(name, kaonSel, eleSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ e+ e- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ e+ e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, kaonSel ] )

#####################################################
def makeD2KEMuOS(name, kaonSel, muonSel, eleSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ e+ mu- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ e+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, kaonSel, muonSel ] )

#####################################################
def makeD2KMuEOS(name, kaonSel, muonSel, eleSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ mu+ e- 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ mu+ e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, kaonSel, muonSel ] )




#####################################################
def makeD2KMuMuSS(name, kaonSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> K+ mu- mu-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> K+ mu- mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )

#####################################################
def makeD2KEESS(name, kaonSel, eleSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> K+ e- e-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> K+ e- e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [  eleSel, kaonSel ] )

#####################################################
def makeD2KMuESS(name, kaonSel, muonSel, eleSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D- -> K+ mu- e-
    """
    
    _combcut = "(ADAMASS('D-') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D- -> K+ mu- e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [  muonSel, kaonSel, eleSel ] )

#####################################################
def makeD2K2Pi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    
    """
    Makes the D+ -> K- pi+ pi+ 
    """
    
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV) &" \
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  ))" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K- pi+ pi+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )



#####################################################
def makeD22KPi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    
    """
    Makes the D+ -> K- K+ pi+ 
    """
    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM23 > %(DimuonMass)s *MeV) &" \
               "(AM > %(DMassLow)s *MeV) &" \
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  ))" %locals()
    
    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ K- pi+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )






#####################################################
def makeD02KKMuMu(name, kaonSel, muonSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DimuonMass, DPT):
    
    """
    Makes the D0 -> K+ K- mu+ mu-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM34 > %(DimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()


    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> K+ K- mu+ mu-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel ] )


#####################################################
def makeD02PiPiMuMu(name, pionSel, muonSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DimuonMass, DPT):
    
    """
    Makes the D0 -> pi+ pi- mu+ mu-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM34 > %(DimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()


    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> pi+ pi- mu+ mu-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel] )

#####################################################
def makeD02KPiMuMu(name, kaonSel, pionSel, muonSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DimuonMass, DPT):
    
    """
    Makes the D0 -> K- pi+ mu+ mu-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AM34 > %(DimuonMass)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()


    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D0 -> K- pi+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, kaonSel, pionSel] )


#####################################################
def makeD02KKPiPi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DPT):
    
    """
    Makes the D0 -> K+ K- pi+ pi-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  )) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> K+ K- pi+ pi-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )



#####################################################
def makeD02K3Pi(name, kaonSel, pionSel, OneKaonPID, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DPT):
    
    """
    Makes the D0 -> K- pi+ pi+ pi-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AHASCHILD( (ABSID=='K+') & (PIDK-PIDpi > %(OneKaonPID)s)  )) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, pionSel ] )



#####################################################
def makeD02PiPiPiPi(name, pionSel, DMAXDOCA, DdauMAXIPCHI2, DFDCHI2, DVCHI2DOF, DDIRA, DIPCHI2, DMassWin, DMassLow, DPT):
    
    """
    Makes the D0 -> pi+ pi- pi+ pi-
    """
    
    _combcut = "(ADAMASS('D0') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM > %(DMassLow)s *MeV) &"\
               "(AHASCHILD( (MIPCHI2DV(PRIMARY)>%(DdauMAXIPCHI2)s)  )   )" %locals()

    
    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVVDCHI2>%(DFDCHI2)s) & (BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(PT > %(DPT)s *MeV) &"\
              "(BPVDIRA > %(DDIRA)s)" % locals()
    
    _Combine = CombineParticles(DecayDescriptor = "D0 -> pi+ pi- pi+ pi-",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ pionSel ] )

####################################################
def makeLambdac2PMuMu(name, protonSel, muonSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ mu+ mu-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
         "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ mu+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, muonSel ] )


####################################################
def makeLambdac2PEE(name, protonSel, eleSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ e+ e-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
         "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ e+ e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, eleSel ] )



####################################################
def makeLambdac2PEMu(name, protonSel, muonSel, eleSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ e+ mu-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
         "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ e+ mu-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, muonSel, eleSel ] )



####################################################
def makeLambdac2PMuE(name, protonSel, muonSel, eleSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ mu+ e-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
         "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ mu+ e-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, muonSel, eleSel ] )


####################################################
def makeLambdac2PPiPi(name, protonSel, muonSel,DMAXDOCA, LambdacMassWin, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the Lambda_c+ -> p+ pi+ pi-
    """

    _combcut = "(ADAMASS('Lambda_c+') < %(LambdacMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[Lambda_c+ -> p+ pi+ pi-]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ protonSel, muonSel ] )


#####################################################
def makeD2PiMuMuCal(name, pionSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ mu+ mu+ 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ mu+ mu+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, pionSel ] )

#####################################################
def makeD2PiEMuCal(name, pionSel, eleSel, muonSel,DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ e+ mu+ 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ e+ mu+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ muonSel, eleSel, pionSel ] )

#####################################################
def makeD2PiEECal(name, pionSel, eleSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> pi+ e+ e+ 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> pi+ e+ e+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ eleSel, pionSel ] )
                    
#####################################################
def makeD2KMuMuCal(name, kaonSel, muonSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ mu+ mu+ 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ mu+ mu+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, muonSel ] )

#####################################################
def makeD2KEMuCal(name, kaonSel, eleSel, muonSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ e+ mu+ 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ e+ mu+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, eleSel, muonSel ] )

#####################################################
def makeD2KEECal(name, kaonSel, eleSel, DMAXDOCA, DMassWin, DMassLow, DimuonMass, DVCHI2DOF, DIPCHI2, DDIRA):
    """
    Makes the D+ -> K+ e+ e+ 
    """

    _combcut = "(ADAMASS('D+') < %(DMassWin)s *MeV) & "\
               "(AMAXDOCA('')<%(DMAXDOCA)s) & " \
		           "(AM > %(DMassLow)s *MeV) &"\
               "(AM23 > %(DimuonMass)s *MeV)" % locals()

    _bcut   = "(VFASPF(VCHI2/VDOF) < %(DVCHI2DOF)s) & "\
              "(BPVIPCHI2()< %(DIPCHI2)s) & "\
              "(BPVDIRA > %(DDIRA)s)" % locals()

    _Combine = CombineParticles(DecayDescriptor = "[D+ -> K+ e+ e+]cc",
                                CombinationCut = _combcut,
                                MotherCut = _bcut)
    _Combine.ReFitPVs = True
    return Selection(name,
                     Algorithm = _Combine,
                     RequiredSelections = [ kaonSel, eleSel ] )

#####################################################
def makeKaons(name, KaonP, KaonPT, KaonPIDK, KaonMINIPCHI2, KaonTRCHI2):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
            "(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(PIDK-PIDpi > %(KaonPIDK)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseKaons ] )

#####################################################
def makePions(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    Pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
            "(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    
#####################################################
def makeMuons(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseMuons ] )


#####################################################
def makeElectrons(name, EleP, ElePT, EleMINIPCHI2, EleTRCHI2):
    """
    Electron selection
    """
    _code = "(TRCHI2DOF < %(EleTRCHI2)s) & "\
            "(P > %(EleP)s *MeV) & "\
            "(PT > %(ElePT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(EleMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseElectrons ] )



#####################################################
def makeProtons(name, ProtonP, ProtonPT, ProtonMINIPCHI2, ProtonTRCHI2):
    """
    Proton selection
    """
    _code = "(TRCHI2DOF < %(ProtonTRCHI2)s) & "\
            "(P > %(ProtonP)s *MeV) & "\
            "(PT > %(ProtonPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(ProtonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLooseProtons ] )
                        
#####################################################
def makePionsAsMuons(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
                    



#####################################################
def makeKaonsFor3bodyCS(name, KaonP, KaonPT, KaonMINIPCHI2, KaonTRCHI2):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
            "(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsKaons ] )


#####################################################
def makePionsFor3bodyCS(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2):
    """
    pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
            "(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsPions ] )


#####################################################
def makeProtonsFor3bodyCS(name, ProtonP, ProtonPT, ProtonMINIPCHI2, ProtonTRCHI2):
    """
    Proton selection
    """
    _code = "(TRCHI2DOF < %(ProtonTRCHI2)s) & "\
            "(P > %(ProtonP)s *MeV) & "\
            "(PT > %(ProtonPT)s *MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(ProtonMINIPCHI2)s)" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsProtons ] )




#####################################################
def makeKaonsForhhmumu(name, KaonP, KaonPT, KaonPIDK, KaonMINIPCHI2, KaonTRCHI2, ghostProbCut):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
            "(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(PIDK-PIDpi > %(KaonPIDK)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseKaons ] )


#####################################################
def makeMuonsForhhmumu(name, MuonP, MuonPT, MuonMINIPCHI2, MuonTRCHI2, MuonPIDmu_CS_hhmumu, ghostProbCut):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(MuonTRCHI2)s) & "\
            "(P > %(MuonP)s *MeV) & "\
            "(PT > %(MuonPT)s* MeV) & "\
            "(PIDmu-PIDpi > %(MuonPIDmu_CS_hhmumu)s) & "\
            "(MIPCHI2DV(PRIMARY) > %(MuonMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLooseMuons ] )

#####################################################
def makePionsForhhmumu(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2, ghostProbCut):
    """
    Muon selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
            "(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllLoosePions ] )



#####################################################
def makeKaonsFor4bodyCS(name, KaonP, KaonPT, KaonMINIPCHI2, KaonTRCHI2, ghostProbCut):
    """
    Kaon selection
    """
    _code = "(TRCHI2DOF < %(KaonTRCHI2)s) & "\
            "(P > %(KaonP)s *MeV) & "\
            "(PT > %(KaonPT)s *MeV) &"\
            "(MIPCHI2DV(PRIMARY) > %(KaonMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)

    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsKaons ] )


#####################################################
def makePionsFor4bodyCS(name, PionP, PionPT, PionMINIPCHI2, PionTRCHI2, ghostProbCut):
    """
    pion selection
    """
    _code = "(TRCHI2DOF < %(PionTRCHI2)s) & "\
            "(P > %(PionP)s *MeV) & "\
            "(PT > %(PionPT)s* MeV) & "\
            "(MIPCHI2DV(PRIMARY) > %(PionMINIPCHI2)s) & "\
            "( TRGHOSTPROB < %(ghostProbCut)s )" % locals()

    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm = _Filter,
                     RequiredSelections = [ StdAllNoPIDsPions ] )


def _format_related_info(selection, config):
    """Format the RelatedInfoTools configuation dictionary.

    Replaces any dictionaries which have 'DaughterLocations' set to `None` with
    the correct locations and decay descriptors.
    HACK
    """
    decay_descriptor = selection.algorithm().DecayDescriptor.replace('cc', 'CC')
    daughter_names = ['Dp_h', 'Dp_l1', 'Dp_l2']

    config = copy.deepcopy(config)
    for tool_config in config:
        if 'DaughterLocations' not in tool_config or tool_config['DaughterLocations'] is not None:
            continue

        tool_config['DaughterLocations'] = {}
        dd_index = decay_descriptor.find('->')+2
        daughter_index = 0
        while ' ' in decay_descriptor[dd_index:]:
            d = '{} ^{}'.format(decay_descriptor[:dd_index], decay_descriptor[dd_index+1:])
            tool_config['DaughterLocations'][d] = '{}_{}'.format(tool_config['Location'],
                                                                 daughter_names[daughter_index])
            daughter_index += 1
            dd_index = decay_descriptor.find(' ', dd_index+1)

    return config



'''


from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles 
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

class StrippingD2XMuMuSSConf(LHCbConfigurableUser):
    """
    Definition of nominal D->Xu mu nu stripping
    """
    
    __slots__ = {
  "prefix"  : "",
        #Muons
        "MuonTRCHI2"    : 10.   ,#adimensional
        "MuonP"         : 3000. ,#MeV
        "MuonPT"        : 500. ,#MeV
#        "MuonPIDK"      : -5.   ,#adimensional
#        "MuonPIDmu"     : -5.   ,#adimensional
#        "MuonPIDp"      : -5.   ,#adimensional
        "MuonMINIPCHI2" : 5     ,#adminensional
        #Xu
        #Pi Channel
        "PionTRCHI2"    : 10.   ,#adimensional
        "PionP"         : 2000. ,#MeV
        "PionPT"        : 300.  ,#MeV
        "PionPIDK"      : -10.  ,#adimensional
#        "PionPIDmu"     : -10.  ,#adimensional
#        "PionPIDp"      : -10.  ,#adimensional
        "PionMINIPCHI2" : 9     ,#adminensional
        #K Channel
        "KaonTRCHI2"    : 10.   ,#adimensional
        "KaonP"         : 2000. ,#MeV
        "KaonPT"        : 300.  ,#MeV
        "KaonPIDK"      : -1.   ,#adimensional
        "KaonMINIPCHI2" : 9     ,#adminensional
        #D
        "DVCHI2DOF"     : 8   ,#adminensional
        "DFDCHI2"       : 64   ,#adminensional
        "DDIRA"         : 0.9999 ,#adimensional
        'DMassWin'      : 200,   # MeV, high mass window
        'DMassLow'      : 1760   # MeV, high mass window
        }


    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
      
    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (PIDpi-PIDK> %(PionPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
      
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2PiMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._D2PiMuMuSS()])
    def PiOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2PiMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._pionFilter(),
                                      self._D2PiMuMuOS()])

    def KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2KMuMuSSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._D2KMuMuSS()])
    
    def KOS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self.getProp("prefix") + 'D2KMuMuOSLine', prescale = 1., 
                             algos = [self._muonFilter(),
                                      self._kaonFilter(),
                                      self._D2KMuMuOS()])

    def _muonFilter( self ):
        from Configurables import FilterDesktop
        _mu = FilterDesktop(self.getProp("prefix") + "Mu_forD2XMuMu",
                            InputLocations = ["Phys/StdLooseMuons"])
        _mu.Code = self._NominalMuSelection() % self.getProps()
        return _mu

    def _pionFilter( self ):
        from Configurables import FilterDesktop
        _pi = FilterDesktop(self.getProp("prefix") + "Pi_forD2XMuMu",
                            InputLocations = ["Phys/StdLoosePions"])
        _pi.Code = self._NominalPiSelection() % self.getProps()
        return _pi

    def _kaonFilter( self ):
        from Configurables import FilterDesktop
        _ka = FilterDesktop(self.getProp("prefix") + "K_forD2XMuMu",
                            InputLocations = ["Phys/StdLooseKaons"])
        _ka.Code = self._NominalKSelection() % self.getProps()
        return _ka
    
    def _D2PiMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles(self.getProp("prefix") + "D2PiMuMuSS")
        _PiMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
          "Phys/" + self.getProp("prefix") + "Pi_forD2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[D- -> pi+ mu- mu-]cc"]
        _PiMuMu.CombinationCut = " (AM>%(DMassLow)s*MeV) & (ADAMASS('D-') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
                          "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu

    def _D2PiMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _PiMuMu = CombineParticles(self.getProp("prefix") + "D2PiMuMuOS")
        _PiMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
          "Phys/" + self.getProp("prefix") + "Pi_forD2XMuMu"]
        _PiMuMu.DecayDescriptors = ["[D+ -> pi+ mu+ mu-]cc"]
        _PiMuMu.CombinationCut = "(AM>%(DMassLow)s*MeV)& (ADAMASS('D+') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _PiMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
                          "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _PiMuMu.ReFitPVs = True
        return _PiMuMu
    
    def _D2KMuMuSS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles(self.getProp("prefix") + "D2KMuMuSS")
        _KMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
         "Phys/" + self.getProp("prefix") + "K_forD2XMuMu"]
        _KMuMu.DecayDescriptors = ["[D- -> K+ mu- mu-]cc"]
        _KMuMu.CombinationCut = "(AM>%(DMassLow)s*MeV)& (ADAMASS('D-') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
            "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    def _D2KMuMuOS( self ):
        from Configurables import CombineParticles, OfflineVertexFitter
        _KMuMu = CombineParticles(self.getProp("prefix") + "D2KMuMuOS")
        _KMuMu.InputLocations = ["Phys/" + self.getProp("prefix") + "Mu_forD2XMuMu",
         "Phys/" + self.getProp("prefix") + "K_forD2XMuMu"]
        _KMuMu.DecayDescriptors = ["[D+ -> K+ mu+ mu-]cc"]
        _KMuMu.CombinationCut = " (AM>%(DMassLow)s*MeV)& (ADAMASS('D+') < %(DMassWin)s *MeV) & (AM23>250.*MeV)" % self.getProps()
        _KMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(DVCHI2DOF)s ) & (BPVVDCHI2 > %(DFDCHI2)s)"\
            "&(BPVDIRA> %(DDIRA)s )" % self.getProps()
        _KMuMu.ReFitPVs = True
        return _KMuMu
    
    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
        
'''
