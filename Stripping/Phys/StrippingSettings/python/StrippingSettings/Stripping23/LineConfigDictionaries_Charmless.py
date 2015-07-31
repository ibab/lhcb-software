################################################################################
##                          S T R I P P I N G  2 3                            ##
##                                                                            ##
##  Configuration for B2noC WG                                                ##
##  Contact person: Stefano Perazzini (Stefano.Perazzini@cern.ch)             ##
################################################################################

from GaudiKernel.SystemOfUnits import *

## StrippingB2CharmlessQuasi2Body.py
B2CharmlessQ2B = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2Quasi2Body',
    'CONFIG'      : { 'Q2BPrescale'     : 1.,
                      'Q2BTrkGhostProb' : 0.5,
                      'Q2BTrkMinIPChi2' : 16.,
                      'Q2BTrkMinPT'     : 400.,
                      'Q2BTrkMinHiPT'   : 1000.,
                      'Q2BResMinPT'     : 600.,
                      'Q2BResMinHiPT'   : 1000.,
                      'Q2BResMaxMass'   : 1100.,
                      'Q2BResVtxChi2DOF': 6.,
                      'Q2BBMinPT'       : 2500.,
                      'Q2BBMinM3pi'     : 4200.,
                      'Q2BBMinM4pi'     : 3500.,
                      'Q2BBMaxM3pi'     : 6700.,
                      'Q2BBMaxM4pi'     : 5700.,
                      'Q2BBMaxCorrM3pi' : 7000.,
                      'Q2BBMaxCorrM4pi' : 6000.,
                      'Q2BBVtxChi2DOF'  : 6.
                    },
    'STREAMS'     : ['BhadronCompleteEvent']
    }


## StrippingB2HHBDT.py

B2HHBDT = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2HHBDTLines',
    'CONFIG'      : { 'PrescaleB2HHBDT' : 1.,
                      'TrChi2'          : 3,
                      'TrGhostProb'     : 3,
                      'PionPT'          : 1000,
                      'SumPT'           : 4500,
                      'DOCACHI2'        : 9,
                      'BIPCHI2'         : 9,
                      'BDIRA'           : 0.99,
                      'BPT'             : 1200,
                      'BMassWinLow'     : 4700,
                      'BMassWinHigh'    : 6200,
                      'BMassLow'        : 4800,
                      'BMassHigh'       : 6200,
                      'PionIPCHI2'      : 16,
                      'BFDCHI2'         : 100,
                      'BDTCut'          : -0.3,
                      'BDTWeightsFile'  : "$TMVAWEIGHTSROOT/data/B2HH_BDT_v1r5.xml"
                    },
    'STREAMS'     : ['Bhadron']
    }

## StrippingB2HHPi0.py
B2HHPi0 = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'StrippingB2HHPi0Conf',
    'CONFIG'      : { 'PiMinPT'              : 500,
                      'PiMinP'               : 5000,
                      'PiMinTrackProb'       : 0.000001,
                      'PiMaxGhostProb'       : 0.5,
                      'PiMinIPChi2'          : 25,
                      'Pi0MinPT_M'           : 2500,
                      'Pi0MinPT_R'           : 1500,
                      'ResPi0MinGamCL'       : 0.2,
                      'BMinM'                : 4200,
                      'BMaxM'                : 6400,
                      'BMinPT_M'             : 3000,
                      'BMinPT_R'             : 2500,
                      'BMinVtxProb'          : 0.001,
                      'BMaxIPChi2'           : 9,
                      'BMinDIRA'             : 0.99995,
                      'BMinVVDChi2'          : 64,
                      'MergedLinePrescale'   : 1.,
                      'MergedLinePostscale'  : 1.,
                      'ResolvedLinePrescale' : 1.,
                      'ResolvedLinePostscale': 1.
                      },
    'STREAMS'     : ['Bhadron']
    }

## StrippingB2KShh.py
B2KShh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG'      : {'Trk_Chi2'                : 3.0,
                     'Trk_GhostProb'           : 0.3,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_DD_VtxChi2'           : 12.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_DD_Pmin'              : 6000.0,
                     'KS_LL_MassWindow'        : 20.0,
                     'KS_LL_VtxChi2'           : 12.0,
                     'KS_LL_FDChi2'            : 80.0,
                     'KS_LL_Pmin'              : 0.0,
                     'KS_LD_MassWindow'        : 25.0,
                     'KS_LD_VtxChi2'           : 12.0,
                     'KS_LD_FDChi2'            : 50.0,
                     'KS_LD_Pmin'              : 6000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_maxDocaChi2'    : 5.0,
                     'BDaug_LL_maxDocaChi2'    : 5.0,
                     'BDaug_LD_maxDocaChi2'    : 5.0,
                     'BDaug_DD_PTsum'          : 4200.0,
                     'BDaug_LL_PTsum'          : 3000.0,
                     'BDaug_LD_PTsum'          : 4200.0,
                     'B_DD_Dira'               : 0.999,
                     'B_LL_Dira'               : 0.999,
                     'B_LD_Dira'               : 0.999,
                     'KS_FD_Z'                 : 15.,
                     'B_DD_MVA'                : -0.20,
                     'B_LL_MVA'                : -0.20,   
                     'B_LD_MVA'                : -0.20,
                     'GEC_MaxTracks'           : 250,
                     'ConeAngle'               : 1.5, 
                     # 2012 Triggers
                     #'HLT1Dec'                 : 'Hlt1TrackAllL0Decision',
                     #'HLT2Dec'                 : 'Hlt2Topo[234]Body.*Decision',
                     # 2015 Triggers
                     'HLT1Dec'                 : 'Hlt1(Two)?TrackMVADecision',
                     'HLT2Dec'                 : 'Hlt2Topo[234]BodyDecision',
                     'Prescale'                : 1.0,
                     'Prescale_SameSign'       : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2KShh_DD_2015_OS_Line',
                                   'StrippingB2KShh_LL_2015_OS_Line',
                                   'StrippingB2KShh_LD_2015_OS_Line',
                                   'StrippingB2KShh_DD_2015_SS_Line',
                                   'StrippingB2KShh_LL_2015_SS_Line',
                                   'StrippingB2KShh_LD_2015_SS_Line',
                                   ] 
                      }
    }

## StrippingB2pphh.py
B2pphh = {
    'WGs'  : ['BnoC'],
    'BUILDERTYPE' : 'B2pphhConf',
    'CONFIG' : { 'MaxTrSIZE'             : 350 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
    'CombMass12Max_kk'         : 4700.0,
    'CombMass12Max_kpi'         : 5000.0,
    'CombMass12Max_pipi'         : 5350.0,
    'd_achi2doca12'             : 20.0,
    'd_achi2doca13'             : 20.0,
    'd_achi2doca14'             : 20.0,
    'd_achi2doca23'             : 20.0,
    'd_achi2doca24'             : 20.0,
    'd_achi2doca34'             : 20.0,
    'p_PTSUM'               : 750.0,
    'p_PSUM'               : 7000.0,
    'p_PTMIN'              : 400.0,
    'p_PMIN'              : 4.0,
    'p_PROBNNpPROD'        : 0.04,
    'CombMass123Max'         : 5600.0,
    'CombMassMax'         : 5.60,
    'CombMassMin'         : 5.00,
    'MassMax'             : 5.55,
    'MassMin'             : 5.05,
    '4h_PTSUM'            : 3000.0,
    'B_DIRA'              : .9999 ,
    '4h_AMAXDOCA'                : 0.3 ,
    'B_CHI2'              : 30.0 ,
    'B_PT'                 : 1000.,
    'B_MIPDV'              : 0.2,
    'PionCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 7.0) & (TRGHOSTPROB < 0.5) & (PROBNNpi > 0.10)",
    'KaonCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRGHOSTPROB < 0.5) & (PROBNNk > 0.08)",
    'ProtonCuts' : "(TRCHI2DOF < 3.0) & HASRICH & (P > 1500*MeV) & (PT > 300*MeV) & (MIPCHI2DV(PRIMARY) > 2.0) & (TRGHOSTPROB < 0.5) & (PROBNNp > 0.05)",
    'Prescaleppkk'  : 1.0,
    'Prescalepppipi'  : 1.0,
    'Prescaleppkpi'  : 1.0
    },
    'STREAMS' : {'Bhadron' : ['StrippingB2pphh_kkLine',
                              'StrippingB2pphh_kpiLine',
                              'StrippingB2pphh_pipiLine']}
    }

## StrippingBc2hhh_BnoC.py
Bc2hhh =  {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bc2hhhBuilder',
    'CONFIG'      : { 'MaxTrSIZE'             : 200,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      '_h_PT'                 : 250.,     ## tracks min PT
                      '_h_P'                  : 2500.,    ## tracks min P  
                      '_h_IPCHI2'             : 1.,       ## min tracks IP wrt OWNPV
                      '_h_TRCHI2DOF'          : 3.0,      ## max tracks CHI2DOF
                      '_h_TRGHP'              : .5,        ## Track PROBNNghost     
                      '_3h_DOCA'              : .2,       ## max DOCA between h and 2h 
                      '_3h_PTmax'             : 1500,     ## min PT of the 3h highest PT track
                      '_3h_DIRA'              : .9999,   ## min cos angle between 3h momentum and PV decay direction   
                      '_3h_FDCHI2'            : 150.,     ## min 3h FDCHI2 wrt best 3h PV  
                      '_3h_SVPV'              : 1.5,      ## Distance between sv and pv
                      '_3h_CHI2'              : 20.0,     ## max 3h vertex CHI2 
                      '_3h_IPCHI2'            : 10.,      ## max 3h IP CHI2 wrt best 3h PV
                      '_3h_PT'                : 1000.,    ## min 3h PT   
                      '_3h_PTsum'             : 4500.,    ## min of 3h tracks PT sum 
                      '_3h_Psum'              : 22000.,   ## min of 3h tracks P sum 
                      '_3h_PVIPCHI2sum'       : 200.,     ## min of the 3h tracks IP wrt best 3h PV
                      '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                      '_3h_Mmax'              : 6500.,     ## max 3h mass for exclusive lines
                      '_3h_Mmin'              : 6000.,     ## min 3h mass for exclusive KKK lines
                      '_bu3h_Mmax'            : 5500.,     ## min buto3h mass for exclusive lines
                      '_bu3h_Mmin'            : 5100.,     ## min buto3h mass for exclusive lines
                      '_probnnpi'             : .15,       ## PROBNNpi for pion tracks. WARNING: DO NOT APPLY THIS CUT FOR PIPIPI OR KPIPI LINES
                      '_probnnk'              : .20,       ## PROBNNk for kaon tracks.
                      '_probnnp'              : .05,       ## PROBNNp for proton tracks.
                      'pipipi_exclLinePrescale'  : 1.0,
                      'pipipi_exclLinePostscale'  : 1.0,
                      'Kpipi_exclLinePrescale'  : 1.0,
                      'Kpipi_exclLinePostscale'  : 1.0,
                      'KKpi_exclLinePrescale'  : 1.0,
                      'KKpi_exclLinePostscale'  : 1.0,
                      'KKK_exclLinePrescale'  : 1.0,
                      'KKK_exclLinePostscale'  : 1.0,
                      'pppi_exclLinePrescale'  : 1.0,
                      'pppi_exclLinePostscale'  : 1.0,
                      'ppK_exclLinePrescale'  : 1.0,
                      'ppK_exclLinePostscale'  : 1.0,
                    },
    'STREAMS'     : ['Bhadron']
    }

## StrippingBs2Kst_0Kst_0.py 
Bs2K0stK0st = {
     'WGs'         : ['BnoC'],
     'BUILDERTYPE' : 'StrippingBs2Kst_0Kst_0Conf',
     'CONFIG'      : {"KaonPT"         : 500.0,
                      "KaonIPCHI2"     : 9.,
                      "PionPT"         : 500.0,
                      "PionIPCHI2"     : 9.,
                      "PionPIDK"       : 0.,
                      "KstarVCHI2"     : 9.0,
                      "KstarPT"        : 900.0,
                      "KaonPIDK"       : 2.0,
                      "KstarMassWin"   : 680.0,
                      "KstarAPT"       : 800.0,
                      "BMassWin"       : 500.0,
                      "BVCHI2"         : 15.0,
                      "BDOCA"          : 0.3,
                      "BIPCHI2"        : 25.,
                      "BFDistanceCHI2" : 81.,
                      "SumPT"          : 5000,
                      "MaxGHOSTPROB"   : 0.8,
                      "BDIRA"          : 0.99
                                          },
     'STREAMS'     : ['Bhadron']
     }

## StrippingBu2hhh.py
Bu2hhh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bu2hhhBuilder',
    'CONFIG'      : { 'MaxTrSIZE'             : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      '_h_PT'                 : 100. ,     ## tracks min PT
                      '_h_P'                  : 1500. ,    ## tracks min P  
                      '_h_IPCHI2'             : 1. ,       ## min tracks IP wrt OWNPV
                      '_h_TRCHI2DOF'          : 3.0 ,      ## max tracks CHI2DOF
                      '_h_TRGHP'             : .5,       ## Track GhostProbability     
                      '_3h_DOCA'              : .2 ,       ## max DOCA between h and 2h 
                      '_3h_PTmax'             : 1500 ,     ## min PT of the 3h highest PT track
                      '_3h_DIRA'              : .99998 ,   ## min cos angle between 3h momentum and PV decay direction   
                      '_3h_FDCHI2'            : 500. ,     ## min 3h FDCHI2 wrt best 3h PV  
                      '_3h_PVDOCAmin'         : 3.0 ,      ## min value of the 3h doca wrt any PV
                      '_3h_CHI2'              : 12.0 ,     ## max 3h vertex CHI2 
                      '_3h_IPCHI2'            : 10. ,      ## max 3h IP CHI2 wrt best 3h PV
                      '_3h_PT'                : 1000. ,    ## min 3h PT   
                      '_3h_PTsum'             : 4500. ,    ## min of 3h tracks PT sum 
                      '_3h_Psum'              : 20000. ,   ## min of 3h tracks P sum 
                      '_3h_PVIPCHI2sum'       : 500. ,     ## min of the 3h tracks IP wrt best 3h PV
                      '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                      '_3h_CORRMmax'          : 7000. ,    ## max corrected mass for 3h candidate  
                      '_3h_CORRMmin'          : 4000. ,    ## min corrected mass for 3h candidate   
                      '_3hKKK_Mmax'           : 6300. ,    ## max 3h mass for inclusive KKK line       
                      '_3hKKK_Mmin'           : 5050. ,    ## min 3h mass for inclusive KKK line
                      '_3hpph_deltaMmax'      : 400,       ## max 3h mass difference for inclusive ppK line
                      '_3hpph_deltaMmin'      : 200,       ## min 3h mass difference for inclusive ppK line 
                      'KKK_inclLinePrescale'  : 1.0,
                      'KKK_inclLinePostscale' : 1.0,
                      'KpKpKp_inclLinePrescale'  : 1.0,
                      'KpKpKp_inclLinePostscale' : 1.0,
                      'pph_inclLinePrescale'  : 1.0,
                      'pph_inclLinePostscale' : 1.0
                    },
    'STREAMS'      : ['Bhadron']
  }

## StrippingHb2Charged2Body.py
Hb2Charged2Body = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {'PrescaleB2Charged2Body'  : 1,
                     'MinPTB2Charged2Body'     : 1000,
                     'MinIPChi2B2Charged2Body' : 12,
                     'TrChi2'                  : 3,
                     'TrGhostProb'             : 0.5,
                     'MaxPTB2Charged2Body'     : 1400,
                     'MaxIPChi2B2Charged2Body' : 40,
                     'CombMassLow'             : 4600,
                     'CombMassHigh'            : 6000,
                     'DOCA'                    : 0.08,
                     'BPT'                     : 1200,
                     'BIPChi2B2Charged2Body'   : 12,
                     'BTAU'                    : 0.0006,
                     'MassLow'                 : 4800,
                     'MassHigh'                : 5800
                    },
    'STREAMS'     : ['BhadronCompleteEvent']
    }

## StrippingLb2V0hh.py
Lb2V0h = {
    'WGs'  : ['BnoC'],
    'BUILDERTYPE' : 'Lb2V0hhConf',
    'CONFIG' : {'Trk_Chi2'                 : 3.0,
                'Trk_GhostProb'            : 0.5,
                'Lambda_DD_MassWindow'     : 25.0,
                'Lambda_DD_VtxChi2'        : 15.0,
                'Lambda_DD_FDChi2'         : 50.0,
                'Lambda_DD_FD'             : 300.0,
                'Lambda_DD_Pmin'           : 5000.0,
                'Lambda_LL_MassWindow'     : 20.0,
                'Lambda_LL_VtxChi2'        : 15.0,
                'Lambda_LL_FDChi2'         : 80.0,
                'Lambda_LD_MassWindow'     : 25.0,
                'Lambda_LD_VtxChi2'        : 15.0,
                'Lambda_LD_FDChi2'         : 50.0,
                'Lambda_LD_FD'             : 300.0,
                'Lambda_LD_Pmin'           : 5000.0,
                'Lb_Mlow'                  : 1319.0,
                'Lb_Mhigh'                 : 600.0,
                'Lb_2bodyMlow'             : 800.0,
                'Lb_2bodyMhigh'            : 800.0,
                'Lb_APTmin'                : 1000.0,
                'Lb_PTmin'                 : 800.0,
                'LbDaug_MedPT_PT'          : 800.0,
                'LbDaug_MaxPT_IP'          : 0.05,
                'LbDaug_DD_maxDocaChi2'    : 5.0,
                'LbDaug_LL_maxDocaChi2'    : 5.0,
                'LbDaug_LD_maxDocaChi2'    : 5.0,
                'LbDaug_DD_PTsum'          : 4200.0,
                'LbDaug_LL_PTsum'          : 3000.0,
                'LbDaug_LD_PTsum'          : 4200.0,
                'Lbh_DD_PTMin'             : 500.0,
                'Lbh_LL_PTMin'             : 500.0,
                'Lbh_LD_PTMin'             : 500.0,
                'Lb_VtxChi2'               : 12.0,
                'Lb_DD_Dira'               : 0.995,
                'Lb_LL_Dira'               : 0.995,
                'Lb_LD_Dira'               : 0.995,
                'Lb_DD_IPCHI2wrtPV'        : 15.0,
                'Lb_LL_IPCHI2wrtPV'        : 15.0,
                'Lb_LD_IPCHI2wrtPV'        : 15.0,
                'Lb_FDwrtPV'               : 1.0,
                'Lb_DD_FDChi2'             : 30.0,
                'Lb_LL_FDChi2'             : 30.0,
                'Lb_LD_FDChi2'             : 30.0,
                'GEC_MaxTracks'            : 250,
                # 2012 Triggers
                #'HLT1Dec'                  : 'Hlt1TrackAllL0Decision',
                #'HLT2Dec'                  : 'Hlt2Topo[234]Body.*Decision',
                # 2015 Triggers
                'HLT1Dec'                  : 'Hlt1(Two)?TrackMVADecision',
                'HLT2Dec'                  : 'Hlt2Topo[234]BodyDecision',
                'Prescale'                 : 1.0,
                'Postscale'                : 1.0,
                'RelatedInfoTools' : [    { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 1.7,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar17'
                                            },
                                          { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 1.5,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar15'
                                            },
                                          { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 1.0,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar10'
                                            },
                                          { "Type" : "RelInfoConeVariables",
                                            "ConeAngle" : 0.8,
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                            "Location"  : 'ConeVar08'
                                            },
                                          { "Type" : "RelInfoVertexIsolation",
                                            "Location" : "VtxIsolationVar"
                                            }
                                          ]
                },
    'STREAMS' : ['Bhadron']
    }

## StrippingBs2PhiPhi.py

BetaSBs2PhiPhi = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'StrippingBs2PhiPhiConf',
    'CONFIG'      : {'KaonPT'              : 400      # MeV/c
                     , 'KaonIPCHI2'          : 0.0      # adimensional
                     , 'PhiPT'               : 0        # MeV/c
                     , 'PhiPTsq'             : 1.2      # GeV*GeV/cc
                     , 'PhiVertexCHI2pDOF'   : 15       # adimensional
                     , 'PhiMassWindow'       : 25       # MeV/cc
                     , 'PhiMassMax'          : 1090     # MeV/cc
                     , 'BsVertexCHI2pDOF'    : 15       # adimensional
                     , 'BsMassWindow'        : 300      # MeV/cc
                     , 'WidePrescale'        : 0.15     # adimensional
                     },
    'STREAMS'     : { 'Bhadron' : ['StrippingBetaSBs2PhiPhiLine',
                                   'StrippingBetaSBs2PhiPhiUpLine',
                                   'StrippingBetaSBs2PhiPhiWideLine']
                    }
    }

