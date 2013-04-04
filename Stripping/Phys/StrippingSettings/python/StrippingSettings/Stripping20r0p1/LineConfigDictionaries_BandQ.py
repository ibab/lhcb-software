# Line config dictionaries for BandQ WG
# https://twiki.cern.ch/twiki/bin/viewauth/LHCbPhysics/BandQStripping20rXp1RequiredLines

# 20r0p1 = 2012

from GaudiKernel.SystemOfUnits import *

HeavyBaryons = {
    'BUILDERTYPE'  : 'HeavyBaryonsConf',
    'CONFIG'       : {    'TRCHI2DOF'             :       4.,
                          'PionPIDK'              :       5.,
                          'KaonPIDK'              :      -5.,
                          'DLSForLongLived'       :       5.,
                          'XiMassWindow'          :      30.,
                          'OmegaMassWindow'       :      30.,
                          "XibminusMassWindow"    :     300.,
                          "XibzeroMassWindow"     :     500.,
                          "OmegabminusMassWindow" :     500.
                          },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : ['BandQ']
    }


ChiCJPsiGammaConv = {                                  
    'BUILDERTYPE'  : 'StrippingChiCJPsiGammaConvConf', 
    'CONFIG'       : {                                 
    'trackChi2'               :    3.0                 
    , 'MuPTMin'               :    300 #MeV            
    , 'JPsiMassMin'           :    3.0 # GeV           
    , 'JPsiMassMax'           :    3.2 # GeV           
    , 'JPsiPTMin'           :     1.9 # GeV            
    , 'JPsi_PIDmu'           :    0.                   
    , 'JPsiVertexChi2'        :   25.0                 
    , 'UpsilonMassMin'        :    9.0  #GeV           
    , 'UpsilonMassMax'        :    12.9 #GeV           
    , 'UpsilonVertexChi2'     :   25.0                 
    , 'UpsilonPTMin'          :    0.9 # GeV           
    , 'Upsilon_PIDmu'         :    0.                  
    , 'eDLLe'                 :   0.0                  
    , 'GammaEEMass'           :   100.0 #MeV           
    , 'GammaEEPt'             :   400.0 #MeV           
    , 'mMinChiCRaw'           :   2.9 #GeV             
    , 'mMaxChiCRaw'           :   4.8 #GeV             
    , 'mMinChiCFit'           :   3.0 #GeV             
    , 'mMaxChiCFit'           :   4.7 ##GeV            
    , 'mMinChiBRaw'           :   9.4 #GeV             
    , 'mMaxChiBRaw'           :   13.2 #GeV            
    , 'mMinChiBFit'           :   9.5 #GeV             
    , 'mMaxChiBFit'           :   13.0  #GeV           
    , 'PrescaleChi'          :   1.0                   
    , 'PostscaleChi'         :   1.0                   
    },                                                 
    'STREAMS' : { 'Dimuon': 
                    [
                      'StrippingChiCJPsiGammaConvChibLine',
                      'StrippingChiCJPsiGammaConvChicLine' 
                    ]
                },                          
    'WGs'    : [ 'BandQ' ]                             
    }                                                  


Betac2PhiP = {
    'BUILDERTYPE'  : 'StrippingBetac2PhiPConf',
    'CONFIG'       : {
                    'Daug_TRCHI2DOF_MAX'        : 10.     
                  , 'Daug_P_MIN'                : 1200.0*MeV 
                  , 'Daug_PT_MIN'               : 200.0*MeV
                  , 'Daug_MIPDV'                : 0.0      # mm
                  , 'Proton_PIDpi_MIN'          : 10.0
                  , 'Proton_PIDK_MIN'           :  0.0 
                  , 'Phi_WIN'                   : 20.0 * MeV
                  , 'Phi_PT'                    : 1700.0*MeV         
                  , 'Betac_AM_MIN'              : 1950.0 * MeV 
                  , 'Betac_AM_MAX'              : 3300.0 * MeV 
                  , 'Betac_BPVDIRA_MIN'         : 0.999   
                  , 'Betac_VCHI2VDOF_MAX'       : 10.0   
                  , 'Betac_BPVLTIME_MIN'        : 0.0 * ns
                  , 'Betac_BPVLTIME_MAX'        : 0.06 * ns
                  , 'Hlt2IncPhiDecision'        : "HLT_PASS_RE('Hlt2IncPhiDecsion')"
                  , 'PrescaleBetac2PhiP'        : 1.0
                  , 'PostscaleBetac2PhiP'       : 1.0
                 },
'STREAMS' : ['CharmCompleteEvent'],
'WGs' : ['BandQ']
}


BB2DMuNuX = {
    'BUILDERTYPE'       : 'StrippingBB2DMuNuXConf',
    'CONFIG'    : {
     "TRGHOSTPROB" : 0.5
    ,"MuonGHOSTPROB" : 0.5
    ,"PrescaleD0Mu"    : 1.0
    ,"MINIPCHI2"     : 25.0 
    ,"TRCHI2"        : 5.0
    ,"KaonPIDK"      : 4.0 
    ,"PionPIDK"      : 10.0
    ,"MuonIPCHI2"    : 25.00  
    ,"MuonPT"        : 800.0    # MeV   800
    ,"KPiPT"         : 300.0    # MeV   300
    ,"D0FDCHI2"      : 100.0     # 100
    ,"D0AMassWin"    : 100.0    # MeV   100
    ,"D0VCHI2DOF"    : 10.0            #6.0
    ,"PIDmu"         : 0.0   
    ,"BVCHI2DOF"     : 25.0            #6.0    
    ,"Comb_AM_MIN"   : 0.
    ,"Comb_AM_MAX"   : 90000.0  # MeV
     },
    'STREAMS' : {'Semileptonic':
                    [ 'Strippingbb2D0MuXBB2DMuNuXLine' ] 
                },
    'WGs'    : ['BandQ'] 
    }




FullDSTDiMuon = {
    'BUILDERTYPE'       :       'DiMuonConf',
    'CONFIG'    : {
        'MicroDST'                                 :  False   ,
                
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           : False    ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  3000.   ,  # MeV
        'DiMuon_MaxMass'                           :  4000.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV, no cut now 
        'DiMuon_PT_oldTh'                          :  2000.   ,  # MeV

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.05  ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   : False    ,

        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale'                 :     0.1  ,
        'DiMuonPrescaled_Postscale'                :     1.   ,
        'DiMuonPrescaled_checkPV'                  : False    ,
        
        'DiMuonPrescaled_MuonPT'                   :   650.   ,  # MeV
        'DiMuonPrescaled_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonPrescaled_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonPrescaled_MinMass'                  :  3000.   ,  # MeV
        'DiMuonPrescaled_MaxMass'                  :  4000.   ,  # MeV
        'DiMuonPrescaled_VCHI2PDOF'                :    20.   , 
        'DiMuonPrescaled_PT'                       : -1000.   ,  # MeV, no cut now 
        
        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     0.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   ,
        'DiMuonExclusive_MuonPIDCut'               :    ""    ,
        'DiMuonExclusive_MinMass'                  :  2900.   ,
        'DiMuonExclusive_VCHI2PDOF'                :    20.   ,
        'DiMuonExclusive_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonExclusive_DZ'                       :    -1.   ,  # mm, upstream of any PV

        # DiMuonNoPV line
        'DiMuonNoPV_Prescale'                      :     1.   ,
        'DiMuonNoPV_Postscale'                     :     1.   ,
        
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        'DiMuonHighMass_checkPV'                   : False    ,
        
        'DiMuonHighMass_MuonPT'                    :  1000.   , # MeV
        'DiMuonHighMass_MuonP'                     :  8000.   , # MeV 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   ,
        'DiMuonHighMass_MuonPIDCut'                : " & (MINTREE('mu+'==ABSID,PIDmu)>0)",
        'DiMuonHighMass_MinMass'                   :  8500.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     0.2  ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,
        'DiMuonHighMassSameSign_checkPV'           : False    ,

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     0.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        'DiMuonLowMass_checkPV'                    : False    , 
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   ,
        'DiMuonLowMass_MuonPIDCut'                 :    ""    , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        'Jpsi2MuMu_checkPV'                        : False    ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : 10000.   ,  # MeV
        'Jpsi2MuMu_MuonPIDmu'                      :     0.   , 
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3010.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3170.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  3000.   ,  # MeV
        'Jpsi2MuMu_TisTosSpecs'                    : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 },

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         : False    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :  1000.   ,  # MeV
        'Psi2MuMu_MuonP'                           : 10000.   ,  # MeV
        'Psi2MuMu_MuonPIDmu'                       :     0.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   100.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  3000.   ,   # MeV
        'Psi2MuMu_TisTosSpecs'                     : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 },
        
        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     0.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now
        'DiMuonDetached_MuonPIDmu'                :    -5.   ,
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_LTCuts'                   :   " & (BPVDLS>3)"    , 
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   550.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :     0.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2996.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3196.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_LTCuts'                         :    " & (BPVDLS>3) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)",
        'Jpsi2MuMuDetached_MinusLTCuts'                    :    " & (BPVDLS<-3) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)",

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   550.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :     0.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   100.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_LTCuts'                          :  " & (BPVDLS>5) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)",
        'Psi2MuMuDetached_MinusLTCuts'                     :  " & (BPVDLS<-3) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)"
        },
    'STREAMS' : { 'Dimuon':
                    [
                      'StrippingFullDSTDiMuonJpsi2MuMuDetachedMinusLine',
                      'StrippingFullDSTDiMuonPsi2MuMuDetachedMinusLine',
                      'StrippingFullDSTDiMuonDiMuonNoPVLine'    
                    ]
                },
    'WGs'    : [ 'BandQ' ]
    }



# The only line required is StrippingMicroDSTDiMuonJpsi2MuMuLine  
MicroDSTDiMuon = {
    'BUILDERTYPE' : 'DiMuonConf',
    'CONFIG' : {
        'MicroDST'                                 :   True   ,
    
        # DiMuon line
        'DiMuon_Prescale'                          :     0.5  ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           :  False   ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  3000.   ,  # MeV
        'DiMuon_MaxMass'                           :  4000.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                : -1000.   ,  # MeV
        'DiMuon_PT_oldTh'                          :  2000.   ,  # MeV

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.05 ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   :  False   ,

        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale'                 :     0.1  ,
        'DiMuonPrescaled_Postscale'                :     1.   ,
        'DiMuonPrescaled_checkPV'                  : False    ,
        
        'DiMuonPrescaled_MuonPT'                   :   650.   ,  # MeV
        'DiMuonPrescaled_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonPrescaled_MuonTRCHI2DOF'            :     5.   ,
        'DiMuonPrescaled_MinMass'                  :  3000.   ,  # MeV
        'DiMuonPrescaled_MaxMass'                  :  4000.   ,  # MeV
        'DiMuonPrescaled_VCHI2PDOF'                :    20.   , 
        'DiMuonPrescaled_PT'                       : -1000.   ,  # MeV, no cut now 

        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     1.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   ,
        'DiMuonExclusive_MuonPIDCut'               :    ""    ,
        'DiMuonExclusive_MinMass'                  :  2900.   ,
        'DiMuonExclusive_VCHI2PDOF'                :    20.   ,
        'DiMuonExclusive_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonExclusive_DZ'                       :    -1.   ,  # mm, upstream of any PV

        # DiMuonNoPV line
        'DiMuonNoPV_Prescale'                      :     1.   ,
        'DiMuonNoPV_Postscale'                     :     1.   ,
               
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        'DiMuonHighMass_checkPV'                   :  True    ,
        
        'DiMuonHighMass_MuonPT'                    :   650.   , # MeV
        'DiMuonHighMass_MuonP'                     : -8000.   , 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   ,
        'DiMuonHighMass_MuonPIDCut'                : " & (MINTREE('mu+'==ABSID,PIDmu)>0)",
        'DiMuonHighMass_MinMass'                   :  8000.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now 

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     1.   ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,
        'DiMuonHighMassSameSign_checkPV'           :  True    ,

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     1.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        'DiMuonLowMass_checkPV'                    :  True    , 
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonPIDCut'                 : ""       , 
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        'Jpsi2MuMu_checkPV'                        :  True    ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMu_MuonPIDmu'                      :    -5.   ,  
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  2976.916,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3216.916,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  3000.0  ,  # MeV
        'Jpsi2MuMu_TisTosSpecs'                    : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonJPsiHighPTDecision%TOS" : 0 },

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         :  True    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :   650.   ,  # MeV
        'Psi2MuMu_MuonP'                           : -8000.   ,  # MeV
        'Psi2MuMu_MuonPIDmu'                       :    -5.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  3000.   ,  # MeV
        'Psi2MuMu_TisTosSpecs'                     : { "L0.*Mu.*Decision%TOS":0,
                                                       "Hlt1DiMuonHighMassDecision%TOS" : 0,
                                                       "Hlt2DiMuonPsi2SHighPTDecision%TOS" : 0 }, 

        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     1.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now
        'DiMuonDetached_MuonPIDmu'                :    -5.   ,
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_LTCuts'                   :    " & (BPVDLS>5)"   ,  # mm, upstream of any PV
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   500.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :    -5.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2976.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3216.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_LTCuts'                         :   " & (BPVDLS>3)"  ,
        'Jpsi2MuMuDetached_MinusLTCuts'                    :    " & (BPVDLS<-3) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)",

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   500.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :    -5.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_LTCuts'                          :   " & (BPVDLS>5)",
        'Psi2MuMuDetached_MinusLTCuts'                     :  " & (BPVDLS<-3) & (MINTREE('mu+'==ABSID,BPVIPCHI2())>4)"
        }, 
    'STREAMS' : { 'Leptonic' :
                    [ 
#                      'StrippingMicroDSTDiMuonDiMuonIncLine', 
                      'StrippingMicroDSTDiMuonDiMuonIncLowPTLine'
                    ] 
                } ,
    'WGs'    : [ 'BandQ' ]
    }






## StrippingPromptCharm.py
PromptCharm = {
    'BUILDERTYPE' : 'StrippingPromptCharmConf',
    'CONFIG' : {
    #
    #
    ## PT-cuts
    #
    'pT(D0)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D0) for D*+'   :  2.0 * GeV ,    ## pt-cut for  D0 from  D*+
    'pT(D+)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Ds+) for Bc+'  :  1.0 * GeV ,    ## pt-cut for  Ds+ from Bc+
    'pT(Ds+) for Lb'   :  1.0 * GeV ,    ## pt-cut for  Ds+ from Lb0
    'pT(Lc+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Lc+
    'pT(Lc+) for Xicc' :  1.0 * GeV ,    ## pt-cut for  Lc+ from Xicc
    'pT(Lc+) for Lb'   :  1.0 * GeV ,    ## pt-cut for  Lc+ from Lb0
    'pT(Xicc+)'        :  2.0 * GeV ,    ## pt-cut for  Xicc+ 
    'pT(Xicc++)'       :  2.0 * GeV ,    ## pt-cut for  Xicc++
    #
    # Selection of basic particles
    #
    'TrackCuts'       : """
    ( TRCHI2DOF < 4     ) &
    ( PT > 250 * MeV    ) &
    ( TRGHOSTPROB < 0.5 ) & 
    in_range  ( 2 , ETA , 5 )
    """ ,
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' ,
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) '                           ,
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) '                           ,
    'ProtonCuts'      : ' & ( 2 < PIDp  - PIDpi ) & ( 2 < PIDp - PIDK ) '     ,
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                     ,
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) '   ,
    #
    ## photons from chi_(c,b)
    #
    'GammaChi'        : ' ( PT > 450 * MeV ) & ( CL > 0.05 ) '  ,
    #
    ## W+- selection
    #
    'WCuts'           : " ( 'mu+'== ABSID ) & ( PT > 15 * GeV )" ,
    #
    # Global Event cuts
    #
    'CheckPV'         : True ,
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # the D0 decay channels
    "pipi   = DECTREE ('[D0]cc -> pi- pi+   ') " ,
    "kk     = DECTREE ('[D0]cc -> K-  K+    ') " ,
    "kpi    = DECTREE ('[D0    -> K-  pi+]CC') " ,
    # number of kaons in final state (as CombinationCuts)
    "ak2    = 2 == ANUM( 'K+' == ABSID ) "       ,
    # shortcut for chi2 of vertex fit
    'chi2vx = VFASPF(VCHI2) '                    ,
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau     = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_9   = BPVLTIME (   9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau_16  = BPVLTIME (  16 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<16
    "ctau_25  = BPVLTIME (  25 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<25
    "ctau_100 = BPVLTIME ( 100 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<100
    "ctau_no  = BPVLTIME (     ) * c_light "  , ## no embedded cut for chi2(lifetimeFit)
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
    ## good proton
    "good_proton   = ( 'p+' == ABSID ) & HASRICH & in_range ( 8 * GeV , P , 150 * GeV )  " ,
    "good_proton   = good_proton & ( PIDp - PIDpi > 2 ) " ,
    "good_proton   = good_proton & ( PIDp - PIDK  > 2 ) " 
    ] ,
    ## monitoring ?
    'Monitor'     : False ,
    ## pescales
    'D0Prescale'             : 1.0 ,
    'D*Prescale'             : 1.0 ,
    'DsPrescale'             : 1.0 ,
    'D+Prescale'             : 1.0 ,
    'LambdaCPrescale'        : 1.0 ,
    'LambdaC*Prescale'       : 1.0 ,
    'Xicc+Prescale'          : 1.0 ,
    'Xicc++Prescale'         : 1.0 ,
    'SigmaCPrescale'         : 1.0 ,
    'DiCharmPrescale'        : 1.0 ,
    'DiMu&CharmPrescale'     : 1.0 ,
    'Chi&CharmPrescale'      : 1.0 ,
    'Ds&PsiPrescale'         : 1.0 ,
    'Ds&Lc+Prescale'         : 1.0 ,
    'Charm&WPrescale'        : 1.0 ,
    'DiMuon&WPrescale'       : 1.0 ,
    'Chi&WPrescale'          : 1.0 ,
    ## ========================================================================
    },
    'WGs' : [ 'BandQ' ],
    'STREAMS' : {
      'Charm' : [    
        'StrippingXicc+ForPromptCharm',
        'StrippingXicc++ForPromptCharm',
        'StrippingDsLamCForPromptCharm'
        ],
      'CharmCompleteEvent' : [
        'StrippingCharmAndWForPromptCharm',
        'StrippingDiMuonAndWForPromptCharm'
        ]
    }
}



#StrippingXicHHH.py
XicHHH = {
    'BUILDERTYPE' : 'StrippingXic2HHHConf',
    'CONFIG' : {
                    'Daug_All_PT_MIN'         : 400.0 * MeV
                  , 'Daug_1of3_PT_MIN'        : 700.0 * MeV
                  , 'Daug_P_MIN'              : 1200.0 * MeV
                  , 'Daug_TRCHI2DOF_MAX'      : 10.0
                  , 'Daug_BPVIPCHI2_MIN'      : 0.5
                  , 'Daug_1of3_BPVIPCHI2_MIN' : 5.0
                  , 'Proton_PIDp_MIN'         : 10.0
                  , 'K_IPCHI2_MIN'            : 20.0
                  , 'Pi_PIDK_MAX'             : 0.0
                  , 'K_PIDK_MIN'              : 5.0
                  , 'Comb_MASS_MIN'           : 1950.0 * MeV 
                  , 'Comb_MASS_MAX'           : 3000.0 * MeV 
                  , 'Comb_ADOCAMAX_MAX'       : 0.1 * mm
                  , 'Xic_PT_MIN'              : 1000.0 * MeV
                  , 'Xic_VCHI2VDOF_MAX'       : 10.0
                  , 'Xic_BPVVDCHI2_MIN'       : 5.0
                  , 'Xic_BPVDIRA_MIN'         : 0.9999
                  , 'Xic_BPVLTIME_MAX'        : 0.06 * ns
                  , 'Xic_BPVLTIME_MIN'        : 0.0 * ns
                  , 'HltFilter'               : "HLT_PASS('Hlt2CharmHadD2HHHDecision')"
                  , 'PrescaleXic2PKPi'        : 1.0
                  , 'PostscaleXic2PKPi'       : 1.0
                  , 'PrescaleXic2PKK'         : 1.0
                  , 'PostscaleXic2PKK'        : 1.0
                  , 'PrescaleXic2PV0'         : 1.0
                  , 'PostscaleXic2PV0'        : 1.0
                 },
    'WGs' : [ 'BandQ' ],
    'STREAMS' : [ 'Charm' ]
}


