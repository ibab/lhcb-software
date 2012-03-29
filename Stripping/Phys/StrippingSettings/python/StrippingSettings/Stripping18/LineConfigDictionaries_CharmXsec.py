"""
Line config dictionaries for the Charm cross-section


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *


# Philip Hunt
# Test: 

D02K3PiForXSec = {
    'BUILDERTYPE' : 'D02K3PiForXSecConf',
    'CONFIG'      : {
    'D0Cuts' : { 'CombMassWin' : 80*MeV
                 ,'MassWin' : 75*MeV
                 ,'MaxDOCA' : 0.25*mm
                 ,'DauPtAllTracks' : 250*MeV
                 ,'DauPtThreeTracks' : 300*MeV
                 ,'DauPtTwoTracks' : 350*MeV
                 ,'DauPtOneTrack' : 400*MeV
                 ,'DauP' : 3*GeV
                 ,'DauIPchi2AllTracks' : 1.5
                 ,'DauIPchi2ThreeTracks' : 3.5
                 ,'DauIPchi2TwoTracks' : 7
                 ,'DauIPchi2OneTrack' : 7
                 ,'FDchi2' : 16
                 ,'DIRA' : 0.99975
                 ,'VtxChi2DOF' : 20
                 ,'DauTrackChi2DOF' : 4
                 ,'ApplyKaonPIDK' : True
                 ,'KaonPIDK' : 5
                 ,'ApplyPionPIDK' : False
                 ,'PionPIDK' : 0
                 }
    ,'DstarCuts' : { 'CombDeltaMLower' : -8.25*MeV
                     ,'CombDeltaMUpper' : 16.5*MeV
                     ,'DeltaMLower' : -7.5*MeV
                     ,'DeltaMUpper' : 15*MeV
                     ,'DOCA' : 0.45*mm
                     ,'SlowPiTrackChi2DOF' : 4
                     ,'VtxChi2DOF' : 100 }
    ,'HltFilter' : {'Tagged' : "HLT_PASS_RE('Hlt1MB.*')"
                    ,'Untagged' : "HLT_PASS_RE('Hlt1MB.*')"
                    }
    ,'CheckPV' : {'Tagged' : True 
                  ,'Untagged' : True }
    ,'Prescale' : {'Tagged' : 1.0
                   ,'Untagged' : 1.0 }
    ,'Postscale' : {'Tagged' : 1
                    ,'Untagged' : 1 }
    ,'ApplyGECs' : {'Tagged' : False
                    ,'Untagged' : False }
    ,'MaxLongTracks' : {'Tagged' : None 
                        ,'Untagged' : None},
    'MaxSpdDigits' : {'Tagged' : None
                      ,'Untagged' : None },
    'MaxITClusters' : {'Tagged' : None
                       ,'Untagged' : None },
    'MaxVeloTracks' : {'Tagged' : None
                       , 'Untagged' : None },    
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'MiniBias' ] 
    }



# Patrick Spradlin
# Test:

D02KPiGeoForXSec = {
    'BUILDERTYPE'  : 'StrippingD02KPiGeoForXSecConf',
    'CONFIG'       : {
    'D0_ADAMASS_WIN'      : 250.0 * MeV
    , 'D0_AMAXDOCA_MAX'     :  10.0 * mm
    , 'D0_ADMASS_WIN'       : 125.0 * MeV
    , 'D0_NU_2PT_MIN'       :  14.0
    , 'D0_BPVVDSIGN_MIN'    :   1.0 * mm
    , 'SPi_TRCHI2DOF_MAX'   :   9.0
    , 'Dstar_AMDiff_MAX'    : 160.0 * MeV
    , 'Dstar_VCHI2VDOF_MAX' : 100.0
    , 'Dstar_MDiff_MAX'     : 155.0 * MeV
    , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
    , 'PrescaleD02HH'             :   1.0
    , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
    , 'PostscaleD02HH'            :   1.0
    , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'MiniBias' ] 
    }
    

Dstar2D0Pi_D02KPiForXSec = {
    'BUILDERTYPE' : 'StrippingDstar2D0Pi_D02KPiForXSecConf',
    'CONFIG'      : {
    'Daug_TRCHI2DOF_MAX'        :  10.0
    , 'Daug_BPVIPCHI2_MIN'        :   6.0
    , 'K_PIDK_MIN'                :   0.0
    , 'Pi_PIDK_MAX'               :   0.0
    , 'D0_ADAMASS_WIN'            :  80.0 * MeV
    , 'D0_ADMASS_WIN'             :  75.0 * MeV
    , 'D0_VCHI2VDOF_MAX'          :  25.0
    , 'D0_BPVLTIME_MIN'           :   0.2 * picosecond
    , 'D0_BPVLTFITCHI2_MAX'       :   2.0e+05
    , 'Dstar_AMDiff_MAX'          : 160.0 * MeV
    , 'Dstar_VCHI2VDOF_MAX'       : 100.0
    , 'Dstar_MDiff_MAX'           : 155.0 * MeV
    , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
    , 'PrescaleD02HH'             :   1.0
    , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
    , 'PostscaleD02HH'            :   1.0
    , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'MiniBias' ] 
    }
    

Lambdac2PKPiForXSec = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
     'Daug_All_PT_MIN'       :  400.0 * MeV
     , 'Daug_1of3_PT_MIN'      : 1200.0 * MeV
     , 'Daug_P_MIN'            : 3200.0 * MeV
     , 'Daug_TRCHI2DOF_MAX'    :   10.0
     , 'Daug_BPVIPCHI2_MIN'    :    0.5
     , 'Proton_PIDp_MIN'       :   10.0
     , 'Pi_PIDK_MAX'           :    0.0
     , 'K_PIDK_MIN'            :   10.0
     , 'Comb_ADAMASS_WIN'      :   90.0 * MeV
     , 'Comb_ADOCAMAX_MAX'     :    0.1 * mm
     , 'Lambdac_PT_MIN'        :    0.0 * MeV
     , 'Lambdac_VCHI2VDOF_MAX' :   20.0
     , 'Lambdac_BPVVDCHI2_MIN' :    8.0
     , 'Lambdac_BPVDIRA_MIN'   :    0.9999
     , 'Lambdac_BPVLTIME_MAX'  :    0.0012 * ns
     , 'Lambdac_BPVLTIME_MIN'  :    0.0 * ns
     , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
     , 'PrescaleLambdac2PKPi'  :    1.0
     , 'PostscaleLambdac2PKPi' :    1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'MiniBias' ] 
    }



D2PhiPiForXSec = {
    'BUILDERTYPE' : 'StrippingD2PhiPiForXSecConf',
    'CONFIG'      : {
    'Daug_TRCHI2DOF_MAX'        :   10.0
    , 'K_MIPCHI2DV_MIN'           :    1.0
    , 'Pi_MIPCHI2DV_MIN'          :    1.0
    , 'K_PIDK_MIN'                :    5.0
    , 'Phi_AM_MIN'                : 1000.0 * MeV
    , 'Phi_AM_MAX'                : 1040.0 * MeV
    , 'D_AM_MIN'                  : 1770.0 * MeV
    , 'D_AM_MAX'                  : 2070.0 * MeV
    , 'D_BPVDIRA_MIN'             :    0.9999
    , 'D_BPVVDCHI2_MIN'           :   30.0
    , 'D_VCHI2VDOF_MAX'           :   10.0
    , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
    , 'PrescaleD2PhiPi'           :    1.0
    , 'PostscaleD2PhiPi'          :    1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'MiniBias' ] 
    }
    



D2HHHForXSec = {
    'BUILDERTYPE' : 'StrippingD2HHHForXSecConf',
    'CONFIG'      : { 
    'Daug_All_IPCHI2_MIN' : 2.0
    , 'Daug_2of3_IPCHI2_MIN' : 8.0
    , 'Daug_1of3_IPCHI2_MIN' : 30.0
    , 'Daug_All_PT_MIN' : 200.0*MeV
    , 'Daug_2of3_PT_MIN' : 400.0*MeV
    , 'Daug_P_MIN' : 3200.0*MeV
    , 'Daug_P_MAX' : 1.0e5 * MeV
    , 'Daug_TRCHI2DOF_MAX' : 10.0
    , 'K_PIDK_MIN' : 3.0
    , 'Pi_PIDK_MAX' : 12.0
    , 'Comb_AM_MIN' : 1580.0 *MeV
    , 'Comb_AM_MAX' : 2260.0 *MeV
    , 'D_M_MIN' : 1780.0 *MeV
    , 'D_M_MAX' : 2060.0 *MeV
    , 'D_BPVDIRA_MIN' : 0.9999
    , 'D_BPVVDCHI2_MIN' : 60.0
    , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
    , 'PrescaleD2KPP' : 1.0
    , 'PrescaleD2KKP' : 1.0
    , 'PrescaleD2KKK' : 1.0
    , 'PrescaleD2PPP' : 1.0
    , 'PrescaleD2KPPDCS' : 1.0
    , 'PostscaleD2KPP' : 1.0
    , 'PostscaleD2KKP' : 1.0
    , 'PostscaleD2KKK' : 1.0
    , 'PostscaleD2PPP' : 1.0
    , 'PostscaleD2KPPDCS' : 1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'MiniBias' ] 
    }

D02HHForXSec = { 
    'BUILDERTYPE' : 'StrippingD02HHForXSecConf',
    'CONFIG'      : {
     'Daug_PT_MIN'               : 250.0*MeV
     , 'Daug_TRCHI2DOF_MAX'        :   9.0
     , 'Daug_MIPCHI2DV_MIN'        :   9.0
     , 'D0_BPVVDCHI2_MIN'          :  16.0
     , 'D0_BPVDIRA_MIN'            :   0.9999
     , 'D0_BPVIPCHI2_MAX'          :   2.0e+05
     , 'D0_ADAMASS_WIN'            :  80.0*MeV
     , 'D0_ADMASS_WIN'             :  75.0*MeV
     , 'Dstar_AMDiff_MAX'          : 160.0*MeV
     , 'Dstar_MDiff_MAX'           : 155.0*MeV
     , 'Dstar_VCHI2VDOF_MAX'       : 100.0
     , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
     , 'PrescaleD02HH'             :   1.0
     , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
     , 'PostscaleD02HH'            :   1.0
     , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'MiniBias' ] 
    }


