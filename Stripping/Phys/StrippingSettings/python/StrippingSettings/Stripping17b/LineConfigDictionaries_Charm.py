from GaudiKernel.SystemOfUnits import *

JpsiMuMuforD0MuMu = {
    'BUILDERTYPE'   : 'JpsiMuMuforD0MuMuConf',
    'CONFIG'   : { 'Jpsi2MuMuForD0mumu_Prescale'              :     0.5   ,
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

CharmedAndCharmedStrangeSpectroscopy = {
    'BUILDERTYPE' : 'CharmedAndCharmedStrangeSpectroscopyConf' ,
    'CONFIG'      : {
    'DpKs_prescale'           : 0,
    'DstarpKs_prescale'       : 0,
    'D0K_prescale'            : 0,
    'DpPim_prescale'          : 0,
    'D0Pip_prescale'          : 0,
    'DstarpPim_prescale'      : 0,
    'DzP_prescale'            : 1,
    'DpP_prescale'            : 1,
    'DsKs_prescale'           : 1,
    'DsKm_prescale'           : 1,
    'DstarD02K3PiKs_prescale' : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

Xicc = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
     'LongTrackGEC' : 150
    ,'controlPrescaleLc' : 0.05
    ,'controlPrescaleXic' : 1.0
    , 'controlPrescaleDp' : 0.001
    , 'controlPrescaleD0' : 0.001
    , 'controlPrescaleDsp' : 0.00
    , 'signalPrescaleViaLc' : 1.0
    , 'signalPrescaleViaXic' : 1.0
    , 'signalPrescaleViaDp'  : 1.0
    , 'signalPrescaleViaDsp' : 0.0
    , 'signalPrescaleViaD0'  : 1.0
    , 'Hlt2TisTosSpec' : { 'Hlt2CharmHad.*Decision%TOS' : 0, 'Hlt1TrackAllL0Decision%TOS' : 0, 'Hlt2Global%TIS' : 0 }
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


CharmForVub = {
    'BUILDERTYPE' : 'CharmForVubConf',
    'CONFIG'     :  {
    'TRACK_Chi2' : 3.,
    'KAON_PIDK' : 5.,
    'KAON_MinPT' : 600.,
    'Kstar_MinPT' : 600.,
    'Kstar_MassMax' : 992.,
    'Kstar_MassMin' : 792.,
    'Rho_MinPT' : 600.,
    'Rho_MassMax' : 970,
    'Rho_MassMin' : 570.,
    'MUON_MinPT' : 800.,
    'MUON_PIDmu' : 0,
    'PION_MinPT' : 600.,
    'PION_PIDmu' : -1.,
    'PION_PIDK' : -3,
    'PAIR_SumPTMin' : 2800.,
    'D_MassMax' : 1950.,
    'D_DIRA' : 0.9994,
    'D_DOCA' : 0.07,
    'D_MinP' : 20000.,
    'D0_FD'  :  4.0,
    'Dplus_FD'  :  8.0,
    'D_MCORR_MIN'  : 1400.,
    'D_MCORR_MAX'  : 2700.,
    'D_BPVVDZ' : 0.,
    'D_VtxChi2' : 9.,
    'Dstar_MassMin' : 300.,
    'Dstar_DOCA' : 0.4,
    'Dstar_VtxChi2' : 9.,
    'Dstar_DeltaMass' : 200.,
    'PrescaledKstarLinePrescale' : 0.25,
    'PrescaledRhoLinePrescale' : 1.0,
    'PrescaledPiLinePrescale' : 0.5,
    'PrescaledLinePostscale' : 1.0
          },
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm']
}


D2XMuMu = {
    'BUILDERTYPE' : 'D2XMuMuConf',
    'CONFIG' : {
    'MuonP'                     : 3000.,        # MeV
    'MuonPT'                    :  500.,        # MeV
    'MuonMINIPCHI2'             :    4,         # adminensional
    'MuonTRCHI2'                :    8,         # adminensional
    'PionP'                     : 2000.,        # MeV
    'PionPT'                    :  300.,        # MeV
    'PionMINIPCHI2'             :    6,         # adminensional
    'PionTRCHI2'                :    8,         # adminensional
    'KaonP'                     : 2000.,        # MeV
    'KaonPT'                    :  300.,        # MeV
    'KaonPIDK'                  :   -1.,        # adimensional
    'KaonMINIPCHI2'             :    6,         # adminensional
    'KaonTRCHI2'                :    8,         # adminensional
    'MINIPCHI2_CS_hhmumu'       :    1,         # adminensional
    'PT_CS_hhmumu'              :  400,         # MeV
    'DimuonMass'                :  250.,        # MeV
    'DVCHI2DOF'                 :    5,         # adminensional
    'DMAXDOCA'                  :    0.25,      # mm
    'DdauMAXIPCHI2'             :   20,         # adimensinal
    'DFDCHI2'                   :    9,         # adimensional
    'DDIRA'                     :    0.9999,    # adimensional
    'DIPCHI2'                   :   30,         # adimensional
    'DMassWin'                  :  200.,        # MeV, mass window
    'DMassWin_CS_hhmumu'        :  100.,        # MeV, mass window
    'DMassLow'                  : 1763.,        # MeV, low-mass veto
    'D2PiMuMuOSLinePrescale'    :    0,
    'D2PiMuMuOSLinePostscale'   :    0,
    'D2PiMuMuSSLinePrescale'    :    0,
    'D2PiMuMuSSLinePostscale'   :    0,
    'D2KMuMuOSLinePrescale'     :    0,
    'D2KMuMuOSLinePostscale'    :    0,
    'D2KMuMuSSLinePrescale'     :    0,
    'D2KMuMuSSLinePostscale'    :    0,
    'D2PiPiPiCalLinePrescale'   :    0.,
    'D2PiPiPiCalLinePostscale'  :    0,
    'D02KKMuMuLinePrescale'     :    1,
    'D02KKMuMuLinePostscale'    :    1,
    'D02PiPiMuMuLinePrescale'   :    1,
    'D02PiPiMuMuLinePostscale'  :    1,
    'D02KKPiPiLinePrescale'     :    0,
    'D02KKPiPiLinePostscale'    :    0,
    'D02K3PiLinePrescale'       :    0,
    'D02K3PiLinePostscale'      :    0,
    'D2K2PiLinePrescale'        :    0,
    'D2K2PiLinePostscale'       :    0,
    'D22KPiLinePrescale'        :    0,
    'D22KPiLinePostscale'       :    0
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

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
                  , 'Hlt2TisTosSpec'     : { 'Hlt2Global%TIS' : 0 }
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

PromptD2hh = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
          'DaugPtMin': 800.,
          'DaugPtMax': 1500.,
          'DaugPtLoose': 500.,
          'DaugP': 5000.,
          'DaugPLoose': 3000.,
          'DaugIPChi2Loose': 4.,
          'DaugIPChi2': 9.,
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
          'UntaggedCFLinePrescale': 1.,
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
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

D2hhhMicroKKPi = { 
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
   'D2KKPLinePrescale'         :    0.01,
   'D2KKPLinePostscale'        :    1.0,
   'D2KPPLinePrescale'         :    0.0,
   'D2KPPLinePostscale'        :    0.0,
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

DstarD2KShh = {
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
    'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          :    0.9997   ## unitless
    ,'KSDDCutDIRA'          :    0.99995  ## unitless
    ,'KSLLCutMass'          :   11.4      ## in MeV
    ,'KSDDCutMass'          :   24.9      ## in MeV
    ,'KSLLCutFDChi2'        :  100        ## unitless
    ,'KSDDCutFDChi2'        :  100        ## unitless
    ,'trackFromDCutP'       : 1500.0      ## in MeV
    ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     :   -1.0
    ,'kaonFromDCutPIDpi'    :   -3.0
    ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
    ,'DCutDIRA'             :    0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              :    0.3      ## ps
    ,'DCutVtxChi2_KK'       :   21.0      ## unitless
    ,'DCutVtxChi2_KP'       :   15.0      ## unitless
    ,'DCutVtxChi2_PP'       :   13.0      ## unitless
    ,'preFitDCutPT'         : 1500.0      ## in MeV
    ,'preFitDMassCut_LL'    :  130        ## in MeV
    ,'preFitDMassCut_DD'    :  270        ## in MeV
    ,'wideDMassCut_KKLL'    :   57.2      ## in MeV
    ,'wideDMassCut_KPLL'    :   92.0      ## in MeV
    ,'wideDMassCut_PPLL'    :  110.0      ## in MeV
    ,'wideDMassCut_KKDD'    :   72.0      ## in MeV
    ,'wideDMassCut_KPDD'    :  120.0      ## in MeV
    ,'wideDMassCut_PPDD'    :  140.0      ## in MeV
    ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       :    0.0      ## in MeV
    ,'wideDMCutUpper'       :   15.0      ## in MeV
    ,'DstarCutPT_KK'        : 2200.0      ## in MeV
    ,'DstarCutPT_KP'        : 2200.0      ## in MeV
    ,'DstarCutPT_PP'        : 2200.0      ## in MeV
    ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
    ,'DstarCutChi2NDOF_KP'  :   20.0      ## unitless
    ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
    ,'SoftPionCutPIDe'      :    5.0      ## unitless
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
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

