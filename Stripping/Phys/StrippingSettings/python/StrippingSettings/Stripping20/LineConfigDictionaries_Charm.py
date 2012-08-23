"""
Line config dictionaries for the Charm WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""


from GaudiKernel.SystemOfUnits import *

# Removed in Stripping20 since redundant w.r.t. CharmForVub
### Micro-DST, Grant McGregor
### Modified from Strippinb13b
### Test:  Yes
#DstarD02KMuNu = { 
#    'BUILDERTYPE' : 'DstarD02KMuNuConf',
#    'CONFIG'      : {
#    'TRACK_Chi2'             :     3.,
#    'KAON_PIDK'              :     5.,
#    'KAON_MinPT'             :   600.,
#    'MUON_MinPT'             :   800.,
#    'PION_PIDmu'             :    -1.,
#    'PAIR_SumPTMin'          :  2800.,
#    'D0_MassMax'             :  1950.,
#    'D0_DOCA'                :     0.07,
#    'D0_MinP'                : 20000.,
#    'D0_FD'                  :     4.0,
#    'D0_MCORR_MIN'           :  1400.,
#    'D0_MCORR_MAX'           :  2700.,
#    'D0_BPVVDZ'              :     0.,
#    'D0_VtxChi2'             :     9.,
#    'Dstar_MassMin'          :   800.,
#    'Dstar_DOCA'             :     0.4,
#    'Dstar_VtxChi2'          :     9.,
#    'Dstar_DeltaMass'        :   200.,
#    'PrescaledLinePrescale'  :     1.0,
#    'PrescaledLinePostscale' :     1.0
#    },
#    'WGs' : [ 'Charm' ],
#    'STREAMS' : [ 'Charm' ]
#    }


## Micro-DST, Vanya BELYAEV
## Test:  Yes
PromptCharm = {
    'BUILDERTYPE' : 'StrippingPromptCharmConf', 
    'CONFIG'      : {
    ## PT-cuts 
    #
    'pT(D0)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D0
    'pT(D0) for D*+'   :  2.0 * GeV ,    ## pt-cut for  D0 from  D*+ 
    'pT(D+)'           :  3.0 * GeV ,    ## pt-cut for  prompt   D+
    'pT(Ds+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Ds+
    'pT(Ds+) for Bc+'  :  1.0 * GeV ,    ## pt-cut for  Ds+ from Bc+
    'pT(Lc+)'          :  3.0 * GeV ,    ## pt-cut for  prompt   Lc+
    #
    # Selection of basic particles 
    #
    'TrackCuts'       : ' ( TRCHI2DOF < 5 ) & ( PT > 250 * MeV ) & in_range  ( 2 , ETA , 5 ) ' , 
    'BasicCuts'       : ' & ( 9 < MIPCHI2DV() ) ' , 
    'KaonCuts'        : ' & ( 2 < PIDK  - PIDpi ) '                           , 
    'PionCuts'        : ' & ( 2 < PIDpi - PIDK  ) '                           , 
    'ProtonCuts'      : ' & ( 2 < PIDp  - PIDpi ) & ( 2 < PIDp - PIDK ) '     , 
    'SlowPionCuts'    : ' TRCHI2DOF < 5 '                                     ,
    'MuonCuts'        : ' ISMUON & ( PT > 650 * MeV ) & ( TRCHI2DOF < 5 ) '   , 
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
    "ctau   = BPVLTIME ( 9 ) * c_light "  , ## use the embedded cut for chi2(LifetimeFit)<9
    # dimuons:
    "psi           =   ADAMASS ('J/psi(1S)') < 150 * MeV"  ,
    "psi_prime     =   ADAMASS (  'psi(2S)') < 150 * MeV"  ,
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
    'SigmaCPrescale'         : 1.0 ,
    'DiCharmPrescale'        : 1.0 , 
    'DiMu&CharmPrescale'     : 1.0 , 
    'Chi&CharmPrescale'      : 1.0 , 
    'Ds&PsiPrescale'         : 1.0 
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : {
      'Charm' : [    
        'StrippingD02HHForPromptCharm',
        'StrippingDstarForPromptCharm',
        'StrippingDsForPromptCharm',
        'StrippingDForPromptCharm',
        'StrippingLambdaCForPromptCharm',
        'StrippingSigmaCForPromptCharm',
        'StrippingLambdaCstarForPromptCharm',
        'StrippingDiMuonAndCharmForPromptCharm',
        'StrippingChiAndCharmForPromptCharm',
        'StrippingDsPsiForPromptCharm',
        'StrippingCharmAndWForPromptCharm',
        'StrippingDiMuonAndWForPromptCharm',
        'StrippingChiAndWForPromptCharm'
        ],
      'CharmCompleteEvent' : [
        'StrippingDiCharmForPromptCharm'
        ]
      }
    }

## D0 -> h+h- (tagged & untagged).
## Responsible: Marco Gersabeck
## In S18, split into default & control parts. The default
## part requires an explicit TOS on HLT2 && HLT1.
D2hh = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 0.1,
           'UntaggedCFLinePostscale': 1.,
           'UntaggedSCSLinePrescale': 1.,
           'UntaggedSCSLinePostscale': 1.,
           'TaggedRSLinePrescale': 1.,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 1.,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 1.,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 1.,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.05,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': False,
           'RunTISLines': False,
           'RunDefault': True,
           'UseTOSFilter': True,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 }
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmToBeSwum' ]
    }

## D0 -> h+h- (tagged & untagged).
## Responsible: Marco Gersabeck
## Prescaled version that writes to full DST.
D2hhCompleteEvent = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 0.001,
           'UntaggedCFLinePostscale': 1.0,
           'UntaggedSCSLinePrescale': 0.01,
           'UntaggedSCSLinePostscale': 1.0,
           'TaggedRSLinePrescale': 0.03,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.0,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 0.03,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 0.01,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 0.01,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.0,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': False,
           'RunTISLines': False,
           'RunDefault': True,
           'UseTOSFilter': True,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 }
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


## Control version of above line.
## Responsible: Marco Gersabeck
## Does not require TOS.
PromptD2hh = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 1.0,
           'UntaggedCFLinePostscale': 1.,
           'UntaggedSCSLinePrescale': 1.,
           'UntaggedSCSLinePostscale': 1.,
           'TaggedRSLinePrescale': 1.,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 1.,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 1.,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 1.,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.05,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': True,
           'RunTISLines': True,
           'RunDefault': False,
           'UseTOSFilter': False,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 }
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Micro-DST, Sam Gregson
## Test:  Yes
D2KS0H = { 
    'BUILDERTYPE' : 'D2KS0HConf' ,
    'CONFIG'      : {
    'KS0DaugP'                  : 2000  # Cut in common particles
    ,'KS0DaugPT'                :  200
    ,'KS0DaugTrackChi2'         :    4  
    ,'KS0DaugMIPChi2'           :   40
    ,'KS0DaugPIDK'              :   10
    ,'KS0MassWindow'            :   35  # Done by stdLoose KS0s
    ,'KS0MIPChi2'               :    7  # Updated by Sam - 20/03/12
    ,'KS0MIPChi2DD'             :    0
    ,'KS0VertexChi2'            :   10  
    ,'KS0PT'                    : 1000
    ,'KS0BPVVDCHI2'             :  300
    ,'KS0BPVVDCHI2DD'           :  200
    ,'BachPionP'                : 2000               
    ,'BachPionPT'               :  200     
    ,'BachPionTrackChi2'        :    3
    ,'BachPionMIPChi2'          :   15
    ,'BachPionPIDK'             :    0
    ,'BachKaonP'                : 2000                 
    ,'BachKaonPT'               :  200     
    ,'BachKaonTrackChi2'        :    3
    ,'BachKaonMIPChi2'          :   15
    ,'BachKaonPIDK'             :    0
    ,'DMesonComboLowMass'       : 1760          
    ,'DMesonComboHighMass'      : 2080
    ,'DMesonComboDOCA'          :    0.6
    ,'DMesonAPT'                : 1000
    ,'DMesonADOCAChi2'          :   11
    ,'DMesonMotherLowMass'      : 1770
    ,'DMesonMotherHighMass'     : 2070
    ,'DMesonMotherVertexChi2'   :   10
    ,'DMesonMotherMIPChi2'      :   15  
    ,'DMesonMotherPT'           : 1000
    ,'KS0ZDiff'                 :   10
    ,'DMesonFlightDistChi2'     :    5 
    ,'PionLinePrescale'         :    1 
    ,'KaonLinePrescale'         :    1 
    ,'PionLinePrescaleDD'       :    1
    ,'KaonLinePrescaleDD'       :    1
    ,'PionLinePostscale'        :    1 
    ,'KaonLinePostscale'        :    1
    ,'PionLinePostscaleDD'      :    1 
    ,'KaonLinePostscaleDD'      :    1          
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Micro-DST, Diego Milanes
## Test:  Yes
CharmedAndCharmedStrangeSpectroscopy = {
    'BUILDERTYPE' : 'CharmedAndCharmedStrangeSpectroscopyConf' ,
    'CONFIG'      : {
      'DpKs_prescale'            : 1,
      'DstarpKs_prescale'        : 1,
      'D0K_prescale'             : 1,
      'DpPim_prescale'           : 1,
      'D0Pip_prescale'           : 1,
      'DstarpPim_prescale'       : 1,
      'DzP_prescale'             : 1,
      'DzPWS_prescale'           : 1,
      'DpP_prescale'             : 1,
      'DpPp_prescale'            : 1,
      'DsKs_prescale'            : 1,
      'DsKm_prescale'            : 1,
      'DstarD02K3PiKs_prescale'  : 1,
      'Dstar0K_prescale'         : 1,
      'Dstar0P_prescale'         : 1,
      'Dstar0KWS_prescale'       : 1,
      'Dstar0PWS_prescale'       : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Micro-DST, Erica Polycarpo
## Modified from Strippinb13b
## Test:  Yes
D2hhh = { 
    'BUILDERTYPE' : 'D2hhhConf' ,
    'CONFIG' : {
    'DaughterPT'                :  250.,
    'DaughterP'                 : 2000.,
    'DaughterIPChi2'            :    4.,
    'Daughter2IPChi2'           :   10.,
    'D2KKKDaughterIPChi2'       :    2.,
    'D2KKKDaughter2IPChi2'      :    4.,
    'KPIDK'                     :    7.,
    'piPIDK'                    :    3.,
    'PTSum'                     : 2800.,
    'DOCAChi2'                  :   50.,
    'DPt'                       : 1000.,
    'DIPChi2'                   :   15.,
    'DdcaFDChi2'                :  100.,
    'DDIRA'                     :    0.0,
    'DVXChi2NDOF'               :   10.,
    'MinMassPosFit'             : 1800.,
    'MaxMassPosFit'             : 2040.,
    'D2KPPMaxMassPosFit'        : 1940.,
    'Ds2KKPosMinMassPosFit'     : 1900.,
    'D2HHHIncMinMassPosFit'     : 1100.,
    'D2HHHIncMaxMassPosFit'     : 2070.,
    'MaxTracksInEvent'          :  500,
    'D2KKPLinePrescale'         :    1.0,
    'D2KKPLinePostscale'        :    1.0 ,
    'D2KPPLinePrescale'         :    1.0,
    'D2KPPLinePostscale'        :    1.0,
    'D2PPPLinePrescale'         :    1.0,
    'D2PPPLinePostscale'        :    1.0,
    'D2KPPosLinePrescale'       :    1.0,
    'D2KPPosLinePostscale'      :    1.0,
    'D2KKKLinePrescale'         :    1.0,
    'D2KKKLinePostscale'        :    1.0,
    'Ds2KKPosLinePrescale'      :    1.0,
    'Ds2KKPosLinePostscale'     :    1.0,
    'D2KPPosLinePrescale'       :    1.0,
    'D2KPPosLinePostscale'      :    1.0,
    'D2HHHIncLinePrescale'      :    0.04,
    'D2HHHIncLinePostscale'     :    1.0,
    'HLT'                       : None
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Full DST (control), Ibid.
## Heavily prescaled, Kpipi and KKpi control modes only.
## New in Stripping17.
## Added KKpi, changed name, loosened PT cut in S18.
D2hhhControl = { 
    'BUILDERTYPE' : 'D2hhhConf' ,
    'CONFIG' : {
    'DaughterPT'                :  250.,
    'DaughterP'                 : 2000.,
    'DaughterIPChi2'            :    4.,
    'Daughter2IPChi2'           :   10.,
    'D2KKKDaughterIPChi2'       :    2.,
    'D2KKKDaughter2IPChi2'      :    4.,
    'KPIDK'                     :    7.,
    'piPIDK'                    :    3.,
    'PTSum'                     : 1800.,
    'DOCAChi2'                  :   50.,
    'DPt'                       : 1000.,
    'DIPChi2'                   :   15.,
    'DdcaFDChi2'                :  100.,
    'DDIRA'                     :    0.0,
    'DVXChi2NDOF'               :   10.,
    'MinMassPosFit'             : 1800.,
    'MaxMassPosFit'             : 2040.,
    'D2KPPMaxMassPosFit'        : 1940.,
    'Ds2KKPosMinMassPosFit'     : 1900.,
    'D2HHHIncMinMassPosFit'     : 1100.,
    'D2HHHIncMaxMassPosFit'     : 2070.,
    'MaxTracksInEvent'          :  500,
    'D2KKPLinePrescale'         :    0.03,
    'D2KKPLinePostscale'        :    1.0 ,
    'D2KPPLinePrescale'         :    0.03,
    'D2KPPLinePostscale'        :    1.0,
    'D2PPPLinePrescale'         :    0.0,
    'D2PPPLinePostscale'        :    0.0,
    'D2KPPosLinePrescale'       :    0.0,
    'D2KPPosLinePostscale'      :    0.0,
    'D2KKKLinePrescale'         :    0.0,
    'D2KKKLinePostscale'        :    0.0,
    'Ds2KKPosLinePrescale'      :    0.0,
    'Ds2KKPosLinePostscale'     :    0.0,
    'D2KPPosLinePrescale'       :    0.0,
    'D2KPPosLinePostscale'      :    0.0,
    'D2HHHIncLinePrescale'      :    0.0,
    'D2HHHIncLinePostscale'     :    0.0,
    'HLT'                       : None
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


## Malcolm John and Benoit Viaud
## Modified from Strippinb13b
## Full DST needed for isolation cuts.
## Test:  Yes
D2XMuMu = {
   'BUILDERTYPE' : 'D2XMuMuConf',
    'CONFIG' : {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 500.  ,    #MeV
    'MuonMINIPCHI2' : 6  ,    #adimensional
    'MuonTRCHI2'    : 8     ,    #adimensional
    'PionP'         : 2000. ,    #MeV
    'PionPT'        : 300.  ,    #MeV
    'PionMINIPCHI2' : 6     ,    #adminensional
    'PionTRCHI2'    : 8  ,    #adminensional
    'KaonP'         : 2000. ,    #MeV
    'KaonPT'        : 300.  ,    #MeV
    'KaonPIDK'      : -1.   ,    #adimensional
    'KaonMINIPCHI2' : 6     ,    #adimensional
    'KaonTRCHI2'    : 8     ,    #adimensional
    'MINIPCHI2_CS_hhmumu' : 6. , #adimensional
    'PT_CS_hhmumu' : 400 , #MeV
    'DimuonMass'    : 250.  ,    #MeV
    'DVCHI2DOF'     : 5     ,    #adimeensional
    'DMAXDOCA'      : 0.15  ,    #mm
    'DdauMAXIPCHI2' : 20    ,    #adimensinal
    'DFDCHI2'       : 9     ,    #adimensional
    'DDIRA'         : 0.9999,    #adimensional
    'DIPCHI2'       : 30    ,    #adimensional
    'DMassWin'      : 200.  ,    #MeV, mass window
    'DMassWin_CS_hhmumu'  : 100.  ,    #MeV, mass window
    'DMassLow'      :1763.  ,    #MeV, low-mass veto
    'D2PiMuMuOSLinePrescale'  : 1 ,
    'D2PiMuMuOSLinePostscale' : 1 ,
    'D2PiMuMuSSLinePrescale'  : 1 ,
    'D2PiMuMuSSLinePostscale' : 1 ,
    'D2KMuMuOSLinePrescale'   : 1 ,
    'D2KMuMuOSLinePostscale'  : 1 ,
    'D2KMuMuSSLinePrescale'   : 1 ,
    'D2KMuMuSSLinePostscale'  : 1 ,
    'D2PiPiPiCalLinePrescale' : 0.01 ,
    'D2PiPiPiCalLinePostscale': 1 ,
    'D02KKMuMuLinePrescale'   : 1 ,
    'D02KKMuMuLinePostscale'  : 1 ,
    'D02PiPiMuMuLinePrescale'   : 1 ,
    'D02PiPiMuMuLinePostscale'  : 1,
    'D02KPiMuMuLinePrescale'   : 1 ,
    'D02KPiMuMuLinePostscale'  : 1,
    'D02KKPiPiLinePrescale'     :    0.01,
    'D02KKPiPiLinePostscale'    :    1,
    'D02K3PiLinePrescale'       :    0.01,
    'D02K3PiLinePostscale'      :    1,
    'D2K2PiLinePrescale'        :    0.01,
    'D2K2PiLinePostscale'       :    1,
    'D22KPiLinePrescale'        :    0.01,
    'D22KPiLinePostscale'       :    1
  } ,
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm' ]
  }

## Walter Bonivento and Francesco Dettori
## Modified from Strippinb13b
## Full DST needed for isolation cuts.
## Test:  Yes
DstarD02xx = {
    'BUILDERTYPE' : 'StrippingDstarD02xxConf',
    'CONFIG':  {
      'PrescalepipiBox'     : 1.
    , 'PrescalemumuBox'     : 1.
    , 'PrescaleKpiBox'    : 1.
    , 'PrescaleemuBox'    : 1.
    , 'PrescaleeKBox'    : 1.
    , 'PrescaleepiBox'    : 1.
    , 'PrescalepimuBox'    : 1.
    , 'PrescaleKmuBox'    : 1.
    , 'Prescalepipi_untagged_Box'     : 0.2
    , 'Prescalemumu_untagged_Box'     : 1.
    , 'PrescaleKpi_untagged_Box'    : 0.2
    , 'Prescalepimu_untagged_Box'    : 1.
    , 'PrescaleKmu_untagged_Box'    : 0.2
    , 'PrescaleKpi_untagged_BoxMB' : 1
    , 'Prescalepipi_untagged_BoxMB':1
    , 'PrescaleKpi_untagged_BoxMBTrEff' : 1
    ,'DMassWin'           : 70.       # MeV
    ,'DMassWinMuMuLow'    : -150.       #MeV
    ,'DMassWinMuMuHigh'   : 300       #MeV
    ,'DMassWinEMu'        : 300
    ,'doca'               : 0.1        # mm
    ,'XminPT'             : 750.       # MeV
    ,'XmaxPT'             : 1100.      # MeV
    ,'XminP'              : 4000.      # MeV
    ,'XTrackChi2'         : 5.        # adimensional
    ,'XTrackChi2Pi'         : 7.        # adimensional
    ,'XminIPChi2'         : 3        # adimensional
    ,'XmaxIPChi2'         : 8        # adimensional
    ,'DMinFlightChi2'    :  20.
    ,'DDira'              : 0.9997     # adimensional
    ,'D0MinPT'            : 1800.      # MeV
    ,'DMaxIPChi2'        :15.
    ,'DVChi2'            :10. 
    ,'PiMinPT'            : 110.       # MeV
    ,'PiMaxIPCHI2'        : 10.         # adimensional
    ,'DstMassWin'         : 300.       # MeV
    ,'DstD0DMWin'         : 10.        # MeV
    ,'DstD0DMWinMuMu'      : 30.        # MeV  
    ,'RequireHlt'         : 1          # 
    ,'prefix'         : '' 
                   },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


## Philip Xing and Joerg Marks
## Modified from Strippinb13b
## Test:  Yes
##
## Removed in Stripping19b by Mat, since these lines are
## redundant with Marco's and are no longer in use.
#DstarPromptWithD02HH = { 
#    'BUILDERTYPE' : 'StrippingDstarPromptWithD02HHConf',
#    'CONFIG'      : {
#        'DaugPt'            :  0.9   * GeV,
#        'DaugP'             :  5.    * GeV,
#        'DaugTrkChi2'       :  5.,         ## unitless
#        'D0Pt'              :  3.3   * GeV,
#        'D0MassWindowCentre':  1.865 * GeV,
#        'D0MassWindowWidth' :  0.075 * GeV,
#        'D0VtxChi2Ndof'     : 13,          ## unitless
#        'D0PVDisp'          :  0.9   * mm,
#        'SlowPiPt'          :  0.260 * GeV,
#        'DstarPt'           :  3.6   * GeV,
#        'DstarVtxChi2Ndof'  : 13,          ## unitless
#        'Pi_negPIDK_MIN'    :  5.0,        ## unitless
#        'K_PIDK_MIN'        :  8.0,        ## unitless
#        'RSLinePrescale'    :  1.,         ## unitless
#        'RSLinePostscale'   :  1.,         ## unitless
#        'WSLinePrescale'    :  1.,         ## unitless
#        'WSLinePostscale'   :  1.,         ## unitless
#        'HHLinePrescale'    :  1.,         ## unitless
#        'HHLinePostscale'   :  1.,         ## unitless
#    },
#    'WGs' : [ 'Charm' ],
#    'STREAMS' : [ 'Charm' ]
#    }


## muDST, Mat Charles + Patrick Spradlin
## Test:  Yes.
Xicc = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
     'LongTrackGEC'            : 150
    ,'controlPrescaleLc'       : 0.05
    ,'controlPrescaleXic'      : 0.3
    , 'controlPrescaleDp'      : 0.001
    , 'controlPrescaleD0'      : 0.001
    , 'controlPrescaleDsp'     : 0.00
    , 'signalPrescaleViaLc'    : 1.0
    , 'signalPrescaleViaXic'   : 1.0
    , 'signalPrescaleViaDp'    : 1.0
    , 'signalPrescaleViaD0'    : 1.0
    , 'signalPrescaleViaLcWC'  : 1.0
    , 'signalPrescaleViaLcDCS' : 1.0
    , 'signalPrescaleViaXicWC' : 1.0
    , 'signalPrescaleViaDpWC'  : 1.0
    , 'signalPrescaleViaD0WC'  : 1.0
    , 'signalPrescaleViaD0DCS' : 1.0
    , 'Hlt2TisTosSpec' : { 'Hlt2CharmHad.*Decision%TOS' : 0 }
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

## muDST, Mat Charles + Patrick Spradlin
## added in Stripping19b
XiccTIS = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
     'LongTrackGEC'            : 150
    ,'controlPrescaleLc'       : 0.0
    ,'controlPrescaleXic'      : 0.0
    , 'controlPrescaleDp'      : 0.0
    , 'controlPrescaleD0'      : 0.0
    , 'controlPrescaleDsp'     : 0.0
    , 'signalPrescaleViaLc'    : 0.10
    , 'signalPrescaleViaXic'   : 0.10
    , 'signalPrescaleViaDp'    : 0.10
    , 'signalPrescaleViaD0'    : 0.10
    , 'signalPrescaleViaLcWC'  : 0.0
    , 'signalPrescaleViaLcDCS' : 0.0
    , 'signalPrescaleViaXicWC' : 0.0
    , 'signalPrescaleViaDpWC'  : 0.0
    , 'signalPrescaleViaD0WC'  : 0.0
    , 'signalPrescaleViaD0DCS' : 0.0
    , 'Hlt2TisTosSpec' : { 'Hlt2Global%TIS' : 0 }
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

## Micro-DST, Philip Hunt and Benoit Viaud
## Modified from Strippinb13b
## Test:  Yes
DstarPromptWithD02HHHH = {
    'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG'      : {
    'CombDelmLower'     : -8.*MeV
    ,'CombDelmUpper'    : 14*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 13*MeV
    ,'CombD0MassWin'    : 68*MeV
    ,'D0MassWin'        : 65*MeV
    ,'DstarDOCA'        : 0.22*mm
    ,'D0MaxDOCA'        : 0.12*mm
    ,'D0DauPt'          : 350*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 120*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 3.0
    ,'D0FDChi2'         : 48
    ,'D0IPChi2'         : 10000.0   # Mat changed from 30 to a large number in S20
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 4
    ,'ApplyKaonPIDK'    : True
    ,'KaonPIDK'         : 0.0
    ,'ApplyPionPIDK'    : True
    ,'PionPIDK'         : 3 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 250
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'Prescale'         : 1
    ,'Postscale'        : 1
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }


## Micro-DST, Ibid.
DstarPromptWithD02HHHHNoPID = {
    'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG'      : {
    'CombDelmLower'     : -8*MeV
    ,'CombDelmUpper'    : 5.5*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 5*MeV
    ,'CombD0MassWin'    : 42*MeV
    ,'D0MassWin'        : 40*MeV
    ,'DstarDOCA'        : 0.22*mm
    ,'D0MaxDOCA'        : 0.12*mm
    ,'D0DauPt'          : 350*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 120*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 3.0
    ,'D0FDChi2'         : 48
    ,'D0IPChi2'         : 10000.0   # Mat changed from 30 to a large number in S20
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 4
    ,'ApplyKaonPIDK'    : False 
    ,'KaonPIDK'         : -100
    ,'ApplyPionPIDK'    : False 
    ,'PionPIDK'         : 100 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 250
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'Prescale'         : 0.10
    ,'Postscale'        : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## KShh. Mat Charles
## Test:  Yes
DstarD2KShh = {
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
    'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          :    0.9997   ## unitless
    ,'KSDDCutDIRA'          :    0.99995  ## unitless
    ,'KSLLCutMass'          :   20.0      ## in MeV -- Mat changed from 11.4 to 20.0 in S20
    ,'KSDDCutMass'          :   40.0      ## in MeV -- Mat changed from 24.9 to 40.0 in S20
    ,'KSLLCutFDChi2'        :  100        ## unitless
    ,'KSDDCutFDChi2'        :  100        ## unitless
    ,'KSCutZFDFromD'        :   10.0      ## in mm -- Mat added in S20
    ,'trackFromDCutP'       : 1500.0      ## in MeV
    ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     :   -1.0
    ,'kaonFromDCutPIDpi'    :   -3.0
    ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
    ,'DCutDIRA'             :    0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              :    0.2      ## ps -- Mat changed from 0.3 to 0.2 in S20
    ,'DCutVtxChi2_KK'       :   21.0      ## unitless
    ,'DCutVtxChi2_KP'       :   15.0      ## unitless
    ,'DCutVtxChi2_PP'       :   13.0      ## unitless
    ,'preFitDCutPT'         : 1500.0      ## in MeV
    ,'preFitDMassCut_LL'    :  130        ## in MeV
    ,'preFitDMassCut_DD'    :  270        ## in MeV
    ,'wideDMassCut_KKLL'    :   70.0      ## in MeV -- Mat changed from  57.2 to  70.0 in S20
    ,'wideDMassCut_KPLL'    :  105.0      ## in MeV -- Mat changed from  92.0 to 105.0 in S20
    ,'wideDMassCut_PPLL'    :  120.0      ## in MeV -- Mat changed from 110.0 to 120.0 in S20
    ,'wideDMassCut_KKDD'    :   85.0      ## in MeV -- Mat changed from  72.0 to  85.0 in S20
    ,'wideDMassCut_KPDD'    :  130.0      ## in MeV -- Mat changed from 120.0 to 130.0 in S20
    ,'wideDMassCut_PPDD'    :  150.0      ## in MeV -- Mat changed from 140.0 to 150.0 in S20
    ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
    ,'wideDMCutUpper'       :   15.0      ## in MeV -- Mat changed from 15.0 to 25.0 in S20
    ,'DstarCutPT_KK'        : 2200.0      ## in MeV
    ,'DstarCutPT_KP'        : 2200.0      ## in MeV
    ,'DstarCutPT_PP'        : 2200.0      ## in MeV
    ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
    ,'DstarCutChi2NDOF_KP'  :   20.0      ## unitless
    ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
    ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
    ,'KKLLPrescale'         :    1.0
    ,'KKLLPostscale'        :    1.0
    ,'KPLLPrescale'         :    1.0
    ,'KPLLPostscale'        :    1.0
    ,'PPLLPrescale'         :    1.0
    ,'PPLLPostscale'        :    1.0
    ,'KKDDPrescale'         :    1.0
    ,'KKDDPostscale'        :    1.0
    ,'KPDDPrescale'         :    1.0
    ,'KPDDPostscale'        :    1.0
    ,'PPDDPrescale'         :    1.0
    ,'PPDDPostscale'        :    1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmToBeSwum' ]
    }

## KShh. Mat Charles.
## Test:  Yes
## Control version of above, going to full DST
## For Stripping19 we bump the prescale of KSKK and KSpipi to 1 because
## the trigger is changing and we need to figure out how to swim them.
DstarD2KShhCompleteEvent = {
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
    'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          :    0.9997   ## unitless
    ,'KSDDCutDIRA'          :    0.99995  ## unitless
    ,'KSLLCutMass'          :   20.0      ## in MeV -- Mat changed from 11.4 to 20.0 in S20
    ,'KSDDCutMass'          :   40.0      ## in MeV -- Mat changed from 24.9 to 40.0 in S20
    ,'KSLLCutFDChi2'        :  100        ## unitless
    ,'KSDDCutFDChi2'        :  100        ## unitless
    ,'KSCutZFDFromD'        :   10.0      ## in mm -- Mat added in S20
    ,'trackFromDCutP'       : 1500.0      ## in MeV
    ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     :   -1.0
    ,'kaonFromDCutPIDpi'    :   -3.0
    ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
    ,'DCutDIRA'             :    0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              :    0.2      ## ps -- Mat changed from 0.3 to 0.2 in S20
    ,'DCutVtxChi2_KK'       :   21.0      ## unitless
    ,'DCutVtxChi2_KP'       :   15.0      ## unitless
    ,'DCutVtxChi2_PP'       :   13.0      ## unitless
    ,'preFitDCutPT'         : 1500.0      ## in MeV
    ,'preFitDMassCut_LL'    :  130        ## in MeV
    ,'preFitDMassCut_DD'    :  270        ## in MeV
    ,'wideDMassCut_KKLL'    :   57.2      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'wideDMassCut_KPLL'    :   92.0      ## in MeV -- Caution:	tighter	than CharmToBeSwum cut from S20
    ,'wideDMassCut_PPLL'    :  110.0      ## in MeV -- Caution:	tighter	than CharmToBeSwum cut from S20
    ,'wideDMassCut_KKDD'    :   72.0      ## in MeV -- Caution:	tighter	than CharmToBeSwum cut from S20
    ,'wideDMassCut_KPDD'    :  120.0      ## in MeV -- Caution:	tighter	than CharmToBeSwum cut from S20
    ,'wideDMassCut_PPDD'    :  140.0      ## in MeV -- Caution:	tighter	than CharmToBeSwum cut from S20
    ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
    ,'wideDMCutUpper'       :   15.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'DstarCutPT_KK'        : 2200.0      ## in MeV
    ,'DstarCutPT_KP'        : 2200.0      ## in MeV
    ,'DstarCutPT_PP'        : 2200.0      ## in MeV
    ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
    ,'DstarCutChi2NDOF_KP'  :   20.0      ## unitless
    ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
    ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
    ,'KKLLPrescale'         :    1.0
    ,'KKLLPostscale'        :    1.0
    ,'KPLLPrescale'         :    0.025
    ,'KPLLPostscale'        :    1.0
    ,'PPLLPrescale'         :    1.0
    ,'PPLLPostscale'        :    1.0
    ,'KKDDPrescale'         :    1.0
    ,'KKDDPostscale'        :    1.0
    ,'KPDDPrescale'         :    0.025
    ,'KPDDPostscale'        :    1.0
    ,'PPDDPrescale'         :    1.0
    ,'PPDDPostscale'        :    1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


## Sebastian Bachmann
## New line in Stripping15
## Moved to micro-DST in PreS18 by Mat.
## Test:  Yes
X2D0D0 = {
    'BUILDERTYPE' : 'X2D0D0Conf',
    'CONFIG'      : {
    'D0MassWin'             :   60.,   # MeV
    'D0PT'                  : 3300.,   # MeV
    'D0VtxChi2Ndof'         :    8.,
    'D0daughterPT'          :  900.,   # MeV
    'D0daughterP'           : 5000.,   # MeV
    'D0daughterTrkChi2'     :    5.,
    'D0daughterHighPIDK'    :    8.,
    'D0daughterLowPIDK'     :    0.,
    'XMassWin'              :    0.    # MeV
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Calibration stream, Vava Gligorov
## New line in Stripping15
## Test:  Yes
D2HHLTUnbiased = {
    'BUILDERTYPE' : 'StrippingD2HHLTUnbiasedConf',
    'CONFIG'      : {
        'Daug_PT_MIN'                 :  750.0     * MeV
        , 'Daug_TRCHI2DOF_MAX'        :    3.0
        , 'K_PIDK_MIN'                :    5.0
        , 'Pi_PIDK_MAX'               :    5.0
        , 'D0_APT_MIN'                : 2500.0     * MeV
        , 'D0_AP_MIN'                 : 5000.0     * MeV
        , 'D0_ADOCA_MAX'              :    0.1     * mm
        , 'D0_VCHI2VDOF_MAX'          :   10.0
        , 'D0_BPVLTIME_MIN'           :    0.00015 * ns
        , 'D0_ADAMASS_WIN'            :   50.      * MeV
        , 'D0_ADMASS_WIN'             :   50.      * MeV
        , 'Dstar_AMDiff_MAX'          :  160.0     * MeV
        , 'Dstar_MDiff_MAX'           :  155.0     * MeV
        , 'Dstar_VCHI2VDOF_MAX'       :  100.0
        , 'HltFilter'          : "HLT_PASS_RE('Hlt1(MB|CharmCalibrationNoBias).*')"
        , 'PrescaleD02HH'             :    1.0
        , 'PrescaleDstar2D0Pi_D02HH'  :    1.0
        , 'PostscaleD02HH'            :    1.0
        , 'PostscaleDstar2D0Pi_D02HH' :    1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

JpsiMuMuforD0MuMu = {
    'BUILDERTYPE'	: 'JpsiMuMuforD0MuMuConf',
    'CONFIG'	: { 'Jpsi2MuMuForD0mumu_Prescale'              :    0.5   ,
                    'Jpsi2MuMuForD0mumu_Postscale'             :     1.   ,
                    'Jpsi2MuMuForD0mumu_MuonPT'                :  750.    ,
                    'Jpsi2MuMuForD0mumu_MuonP'                 : 5000.    ,
                    'Jpsi2MuMuForD0mumu_MuonTRCHI2DOF'         :   5.     ,
                    'Jpsi2MuMuForD0mumu_MuonBPVIPCHI2'         :   3.     ,
                    'Jpsi2MuMuForD0mumu_MinMass'               : 2900     , 
                    'Jpsi2MuMuForD0mumu_MaxMass'               : 3280     ,
                    'Jpsi2MuMuForD0mumu_VCHI2PDOF'             :  10.     ,
                    'Jpsi2MuMuForD0mumu_PT'                    : 1800.    , 
                    'Jpsi2MuMuForD0mumu_BPVVDCHI2'             :   20.    ,
                    'Jpsi2MuMuForD0mumu_BPVDIRA'               : 0.9997   ,
                    'Jpsi2MuMuForD0mumu_BPVIPCHI2'             : 15.      ,
                    'Jpsi2MuMuForD0mumu_MuonMaxBPVIPCHI2'      : 8.       ,
                    'Jpsi2MuMuForD0mumu_MuonMaxPT'             : 1100.    
                    },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'Charm' ]
    }

# For measurement of Vub combining B & D decays. Responsible: P. Owen
# New in S17b.
CharmForVub = {
    'BUILDERTYPE' : 'CharmForVubConf',
    'CONFIG'     :  {
    'TRACK_MINIPCHI2' : 6,
    'KAON_PIDK' : 3.,
    'KAON_MinPT' : 600.,
    'Kstar_MinPT' : 600.,
    'Kstar_MassMax' : 1040.,
    'Kstar_MassMin' : 750.,
    'Rho_MinPT' : 600.,
    'Rho_MassMax' : 1020,
    'Rho_MassMin' : 530.,
    'MUON_MinPT' : 800.,
    'MUON_PIDmu' : 0,
    'PION_MinPT' : 600.,
    'PION_PIDmu' : -1.,
    'PION_PIDK' : 0,
    'PAIR_SumPTMin' : 2800.,
    'D_MassMax' : 1950.,
    'D_DOCA' : 0.07,
    'D_MinP' : 20000.,
    'D0_FD'  :  4.0,
    'Dplus_FD'  :  4.0,
    'D_MCORR_MIN'  : 1400.,
    'D_MCORR_MAX'  : 2700.,
    'D_BPVVDZ' : 0.,
    'D_VtxChi2' : 9.,
    'BtoD_DeltaMass_MIN' : 2700.,
    'BtoD_DeltaMass_MAX' : 3700.,
    'B_DIRA' : 0.9994,
    'B_FDCHI2' : 100,
    'Dstar_MassMin' : 300.,
    'Dstar_DOCA' : 0.4,
    'Dstar_VtxChi2' : 9.,
    'Dstar_DeltaMass' : 200.,
    'HLT_FILTER_VMuNu' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision'))",
    'HLT_FILTER_HMuNu' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2CharmHadD02HHXDst_.*Decision'))",
    #Pre and postscale
    'PrescaledVLinePrescale' : 1.0,
    'PrescaledHLinePrescale' : 1.0},
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm']
}

# Lambdac -> p h+ h-. Responsible: Patrick Spradlin
# New in S17b
# Note that this is for measurement of BFs, *not*
# cross-sections (even though it uses the same builder)
Lambdac2PHH = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
                  'Daug_All_PT_MIN'       :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'      : 1200.0 * MeV
                  , 'Daug_P_MIN'            : 3200.0 * MeV
                  , 'Daug_TRCHI2DOF_MAX'    :   5.0
                  , 'Daug_BPVIPCHI2_MIN'    :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   : 8.0
                  , 'Proton_PIDp_MIN'       :   5.0
                  , 'Pi_PIDK_MAX'           :    0.0
                  , 'K_PIDK_MIN'            :   5.0
                  , 'Comb_ADAMASS_WIN'      :   90.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'     :    0.1 * mm
                  , 'Lambdac_PT_MIN'        :    0.0 * MeV
                  , 'Lambdac_VCHI2VDOF_MAX' :   20.0
                  , 'Lambdac_BPVVDCHI2_MIN' :    16.0
                  , 'Lambdac_BPVDIRA_MIN'   :    0.9999
                  , 'Lambdac_BPVLTIME_MAX'  :    0.0012 * ns
                  , 'Lambdac_BPVLTIME_MIN'  :    0.0 * ns
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt2Global.*')"
                  , 'Hlt2TisTosSpec'     : { 'Hlt2Global%TIS' : 0,  'Hlt2CharmHad.*Decision%TOS' : 0 }
                  , 'PrescaleLambdac2PKPi'  :    1.0
                  , 'PostscaleLambdac2PKPi' :    1.0
                  , 'PrescaleLambdac2PKK'      : 1.0
                  , 'PostscaleLambdac2PKK'     : 1.0
                  , 'PrescaleLambdac2PPiPi'    : 1.0
                  , 'PostscaleLambdac2PPiPi'   : 1.0
                  , 'PrescaleLambdac2PPiKWS'   : 1.0
                  , 'PostscaleLambdac2PPiKWS'  : 1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'Charm' ]
    }

# Lambdac -> p h+ h-. Responsible: Patrick Spradlin
#Prescaled full dst
# New in S19b
Lambdac2PHHPrescFull = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
                  'Daug_All_PT_MIN'       :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'      : 1200.0 * MeV
                  , 'Daug_P_MIN'            : 3200.0 * MeV
                  , 'Daug_TRCHI2DOF_MAX'    :   5.0
                  , 'Daug_BPVIPCHI2_MIN'    :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   : 8.0
                  , 'Proton_PIDp_MIN'       :   5.0
                  , 'Pi_PIDK_MAX'           :    0.0
                  , 'K_PIDK_MIN'            :   5.0
                  , 'Comb_ADAMASS_WIN'      :   90.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'     :    0.1 * mm
                  , 'Lambdac_PT_MIN'        :    0.0 * MeV
                  , 'Lambdac_VCHI2VDOF_MAX' :   20.0
                  , 'Lambdac_BPVVDCHI2_MIN' :    16.0
                  , 'Lambdac_BPVDIRA_MIN'   :    0.9999
                  , 'Lambdac_BPVLTIME_MAX'  :    0.0012 * ns
                  , 'Lambdac_BPVLTIME_MIN'  :    0.0 * ns
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt2Global.*')"
                  , 'Hlt2TisTosSpec'     : { 'Hlt2Global%TIS' : 0,  'Hlt2CharmHad.*Decision%TOS' : 0 }
                  , 'PrescaleLambdac2PKPi'  :    0.1
                  , 'PostscaleLambdac2PKPi' :    1.0
                  , 'PrescaleLambdac2PKK'      : 0.1
                  , 'PostscaleLambdac2PKK'     : 1.0
                  , 'PrescaleLambdac2PPiPi'    : 0.1
                  , 'PostscaleLambdac2PPiPi'   : 1.0
                  , 'PrescaleLambdac2PPiKWS'   : 0.1
                  , 'PostscaleLambdac2PPiKWS'  : 1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'CharmCompleteEvent' ]
    }

# D0 -> phi gamma
# Responsible: Fatima Soomro
# Added: Stripping19
# Lives in StrippingD2XGamma.py
D2XGamma = {
    'BUILDERTYPE' : 'StrippingD2XGammaConf',
    'CONFIG' : {
    'TrChi2'            : 2.5
    ,'TrIPchi2Phi'      : 25.
    ,'TrPTPhi'          : 500.
    ,'kDLL'             : 2.0
    ,'PhiMassWinL'      : 1000.
    ,'PhiMassWin'       : 50.
    ,'PhiVCHI2'         : 16.
    ,'D0MassWin'        : 200.
    ,'D_BPVLTIME'       : 0.2 * picosecond ## not used ATM
    ,'D0_PT'            : 1000.
    ,'photonPT'         : 1700.
    ,'DstVCHI2'         : 25.
    ,'deltaMass'        : 160.0
    ,'deltaMassL'       : 130.0
    ,'pvRefit'          : False
    ,'D02PhiGammaPreScale'               : 1.0
    ,'D02PhiGammaPostScale'              : 1.0
    ,'PionLocation'   : "Phys/StdAllNoPIDsPions/Particles"
    ,'PhiLocation' : "Phys/StdLoosePhi2KK/Particles"
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : ['CharmCompleteEvent' ]
    }

# D0 -> h+ h- pi-
# Responsible: Marco Gersabeck
# Added: Stripping19
# Lives in StrippingDstarD2hhpi0.py
DstarD2hhpi0 = {
    'BUILDERTYPE' : 'DstarD2hhpi0Conf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1250.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2Loose': 9.,
           'DaugIPChi2': 25.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 1800.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 100.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'KstPt': 1800.,
           'KstP': 5000.,
           'KstVtxChi2Ndof': 10.,
           'KstFDChi2': 40.,
           'KstDOCA': 0.07,
           'KstMassWindowCentre': 1865.,
           'KstKPiMassWindowWidthHigh': 50.,
           'Daug_TRCHI2DOF_MAX': 3.,
           'Dstar_AMDiff_MAX': 185.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 180.,
           'Pi0MinPT_R':1000.,
           'Pi0MinPT_M':2000.,
           'ResPi0MinGamCL': 0.2,
           'TaggedRSLinePrescale': 1.,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 1.,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 1.,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 1.,
           'TaggedSCSSSLinePostscale': 1.,
           'RunSameSign': True,
           'RunDefault': True,
           'UseTOSFilter': True,
           'Hlt2TOS': { 'Hlt2CharmHadD02HHXDst_hhXDecision%TOS' : 0, 'Hlt2CharmHadD02HHXDst_hhXWideMassDecision%TOS' : 0, 'Hlt2Global%TIS' : 0 }
         },
    'WGs' : [ 'Charm' ],
    'STREAMS' : ['Charm' ] 
}

# D(s)+ -> KS h+ h+ h-
# Responsible: Hamish Gordon
# Added: Stripping20
# Lives in StrippingD2KS0HHH_conf.py
D2KS0HHH = {
    'BUILDERTYPE' : 'D2KS0HHHConf',
    'CONFIG'      : {
        'KS0DaugP'                  : 3000  
        ,'KS0DaugPT'                :  200
        ,'KS0DaugTrackChi2'         :    4  
        ,'KS0DaugMIPChi2'           :   40
        ,'KS0DaugPIDK'              :    5
        ,'KS0MassWindow'            :   35 
        ,'KS0MIPChi2'               :    7 
        ,'KS0MIPChi2DD'             :    0
        ,'KS0VertexChi2'            :    8  
        ,'KS0PT'                    :  600
        ,'KS0BPVVDCHI2'             :  300
        ,'KS0BPVVDCHI2DD'           :  200
        ,'BachPionP'                : 5000               
        ,'BachPionPT'               :  320     
        ,'BachPionTrackChi2'        :    3
        ,'BachPionMIPChi2'          :    5 
        ,'BachPionPIDK'             :    0
        ,'BachKaonP'                : 5000                 
        ,'BachKaonPT'               :  320     
        ,'BachKaonTrackChi2'        :    3
        ,'BachKaonMIPChi2'          :    5 
        ,'BachKaonPIDK'             :    8 
        ,'DMesonComboLowMass'       : 1760          
        ,'DMesonComboHighMass'      : 2080
        ,'DMesonComboDOCA'          :  1.0 
        ,'DMesonAPT'                : 1700
        ,'DMesonADOCAChi2'          :   20 
        ,'dau2PT'                   : 1200
        ,'dau2IPCHI2'               :   20
        ,'DMesonMotherLowMass'      : 1770
        ,'DMesonMotherHighMass'     : 2070
        ,'DMesonMotherVertexChi2'   :   20
        ,'DMesonMotherMIPChi2'      :   15  
        ,'DMesonMotherPT'           : 1800
        ,'KS0ZDiff'                 :   10
        ,'DMesonFlightDistChi2'     :   50
        ,'MaxTracksInEvent'         :  400
        ,'PiPiPiLinePrescale'       :    1 
        ,'KPiPiLinePrescale'        :    1
        ,'KKPiLinePrescale'         :    1
        ,'KPiPiOSLinePrescale'      :    1 
        ,'PiPiPiLinePrescaleDD'     :    1
        ,'KPiPiLinePrescaleDD'      :    1
        ,'KKPiLinePrescaleDD'       :    1
        ,'KPiPiOSLinePrescaleDD'    :    1
        ,'PiPiPiLinePostscale'      :    1 
        ,'KPiPiLinePostscale'       :    1
        ,'KKPiLinePostscale'        :    1
        ,'KPiPiOSLinePostscale'     :    1
        ,'PiPiPiLinePostscaleDD'    :    1 
        ,'KPiPiLinePostscaleDD'     :    1
        ,'KKPiLinePostscaleDD'      :    1
        ,'KPiPiOSLinePostscaleDD'   :    1
         },
    'WGs' : [ 'Charm' ],
    'STREAMS' : ['Charm' ]
}


# Cross-section lines.
# Imported in Stripping19, taking from the Stripping18 version of
# LineConfigDictionaries_CharmXsec.py, which was removed in Stripping18b.
# All of these require a min/micro/no-bias trigger and should have very low rate.
# Overall responsible: Patrick Spradlin
# Contributions from numerious people.

# Xsec: D0 -> K- pi- pi+ pi+
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
    'STREAMS' : [ 'CharmCompleteEvent' ] 
    }

# Xsec: D0 -> K- pi+ (geometrical selection)
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
    , 'PrescaleD02HH'             :   0.1
    , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
    , 'PostscaleD02HH'            :   1.0
    , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ] 
    }

# Xsec: D*+ -> D0 pi+, D0 -> K- pi+
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
    , 'PrescaleD02HH'             :   0.1
    , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
    , 'PostscaleD02HH'            :   1.0
    , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ] 
    }

# Xsec: Lc+ -> p K- pi+
Lambdac2PKPiForXSec = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
     'Daug_All_PT_MIN'       :  400.0 * MeV
     , 'Daug_1of3_PT_MIN'      : 1200.0 * MeV
     , 'Daug_P_MIN'            : 3200.0 * MeV
     , 'Daug_TRCHI2DOF_MAX'    :   10.0
     , 'Daug_BPVIPCHI2_MIN'    :    0.5
     , 'Daug_1of3_BPVIPCHI2_MIN'   : 4.0
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
     , 'Hlt2TisTosSpec'     : { }
     , 'PrescaleLambdac2PKPi'  :    1.0
     , 'PostscaleLambdac2PKPi' :    1.0
     , 'PrescaleLambdac2PKK'      : 1.0
     , 'PostscaleLambdac2PKK'     : 1.0
     , 'PrescaleLambdac2PPiPi'    : 1.0
     , 'PostscaleLambdac2PPiPi'   : 1.0
     , 'PrescaleLambdac2PPiKWS'   : 1.0
     , 'PostscaleLambdac2PPiKWS'  : 1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'CharmCompleteEvent' ] 
    }

# Xsec: D(s)+ -> phi pi+
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
    'STREAMS' : [ 'CharmCompleteEvent' ] 
    }

# Xsec: D(s)+ -> h+ h- h+
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
    , 'PrescaleD2KPP' : 0.1
    , 'PrescaleD2KKP' : 0.1
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
    'STREAMS' : [ 'CharmCompleteEvent' ] 
    }

# Xsec: D0 -> h+ h-
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
     , 'PrescaleD02HH'             :   0.1
     , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
     , 'PostscaleD02HH'            :   1.0
     , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ] 
    }

