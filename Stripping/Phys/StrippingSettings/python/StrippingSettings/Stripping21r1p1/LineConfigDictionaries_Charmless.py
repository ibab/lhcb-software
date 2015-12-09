################################################################################
##                          S T R I P P I N G  2 1r1p1                        ##
##                                                                            ##
##  Configuration for B2noC WG                                                ##
##  Contact person: Stefano Perazzini (Stefano.Perazzini@cern.ch)             ##
##                  Timothy Williams (timothy.williams@cern.ch)               ##
################################################################################

from GaudiKernel.SystemOfUnits import *

#StrippingBs2Kst_0Kst_0.py
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

#StrippingBs2PhiKst0.py
Bs2PhiKst = {
	"WGs"	      	: ['BnoC'],
	"BUILDERTYPE" 	: 'StrippingBs2PhiKstConf',
	"CONFIG"	: {"KaonPT"                : 500.0, 
                           "KaonIPCHI2"            : 9.,    
                           "KaonPIDK"              : 0.,    
                           "PionPT"                : 500.0, 
                           "PionIPCHI2"            : 9.,    
                           "PionPIDK"              : 10.,    
                           "PhiVCHI2"              : 9.,    
                           "PhiPT"                 : 900.0, 
                           "PhiMassWin"            : 25.0,  
                           "KstarVCHI2"            : 9.0,   
                           "KstarPT"               : 900.0, 
                           "KstarMassWin"          : 150.0, 
                           "BMassWin"              : 500.0, 
                           "BVCHI2"                : 15.0,  
                           "BDOCA"                 : 0.3,   
                           "BDIRA"                    : 0.99     
          					            },
	"STREAMS"	: ['Bhadron']
	}

#StrippingXb23ph.py
Xb23ph = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Xb23phConf',
    'CONFIG'      : {'Trk_MaxChi2Ndof'   : 4.0,
                     'Trk_MaxGhostProb'  : 0.4,
                     'Trk_MinIPChi2'     : 16.0,
                     'Trk_MinP'          : 1500.0,
                     'Trk_MinProbNNp'    : 0.05,
                     'Xb_MinSumPTppi'    : 1500.0,
                     'Xb_MinSumPTppipi'  : 2500.0,
                     'Xb_MinM_4body'     : 5195.0,
                     'Xb_MaxM_4body'     : 6405.0,
                     'Xb_MinSumPT_4body' : 3500.0,
                     'Xb_MinPT_4body'    : 1500.0,
                     'Xb_MaxDOCAChi2'    : 20.0,
                     'Xb_MaxVtxChi2'     : 20.0,
                     'Xb_MinFDChi2'      : 50.0,
                     'Xb_MaxIPChi2'      : 16.0,
                     'Xb_MinDira'        : 0.9999,
                     'Prescale'          : 1.0,
                     'Postscale'         : 1.0
                     },
    'STREAMS'     : ['Bhadron']
    }

#StrippingB2KShhh.py
B2KShhh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2KSHHHLines',
    'CONFIG'      : { 'PrescaleB2PiPiPiKSLL' : 1,
                      'PrescaleB2KPiPiKSLL'  : 1,
                      'PrescaleB2KKPiKSLL'   : 1,
                      'PrescaleB2KKKKSLL'    : 1,
                      'PrescaleB2PiPiPiKSDD' : 1,
                      'PrescaleB2KPiPiKSDD'  : 1,
                      'PrescaleB2KKPiKSDD'   : 1,
                      'PrescaleB2KKKKSDD'    : 1,
                      # KS (DD)
                      'MinDz_DD'             : 0 * mm,
                      'MaxDz_DD'             : 9999 * mm,
                      'KSCutDIRA_DD'         : 0.999,
                      'KSCutMass_DD'         : 50 * MeV,
                      'KSCutFDChi2_DD'       : 5,
                      # KS (LL)
                      'MinDz_LL'             : 0 * mm,
                      'MaxDz_LL'             : 9999 * mm,
                      'KSCutDIRA_LL'         : 0.999,
                      'KSCutMass_LL'         : 35 * MeV,
                      'KSCutFDChi2_LL'       : 5,
                      # B -> KSHHH
                      'TrChi2'               : 4,
                      'TrGhostProb'          : 0.4,
                      'MinTrkPT'             : 500 * MeV,
                      'MinTrkP'              : 1.5 * GeV,
                      'MinTrkIPChi2'         : 4,
                      'HighPIDK'             : 0,
                      'LowPIDK'              : 0,
                      'MaxADOCACHI2'         : 10.0,
                      'CombMassLow'          : 4500 * MeV,
                      'CombMassHigh'         : 7200 * MeV,
                      'MinCombPT'            : 1.5 * GeV,
                      'MinVPT'               : 1.5 * GeV,
                      'MaxVCHI2NDOF'         : 12.0,
                      'MinBPVDIRA'           : 0.99995,
                      'MinBPVTAU'            : 0.1 * picosecond,
                      'MassLow'              : 4700 * MeV,
                      'MassHigh'             : 7000 * MeV,
                      # HLT filters, only process events firing triggers matching the RegEx
                      # 2012 Triggers
                      'Hlt1Filter'          : 'Hlt1TrackAllL0Decision',
                      'Hlt2Filter'          : 'Hlt2Topo[234]Body.*Decision',
                      # 2015 Triggers
                      #'Hlt1Filter'           : 'Hlt1(Two)?TrackMVADecision',
                      #'Hlt2Filter'           : 'Hlt2Topo[234]BodyDecision',
                    },
    'STREAMS'     : ['Bhadron']
    }

#StrippingBc2hhh_BnoC.py
Bc2hhh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bc2hhhBuilder',
    'CONFIG'      : { 'MaxTrSIZE'             : 200,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      '_h_PT'                 : 300.,     ## tracks min PT
                      '_h_P'                  : 2500.,    ## tracks min P  
                      '_h_IPCHI2'             : 1.,       ## min tracks IP wrt OWNPV
                      '_h_TRCHI2DOF'          : 4.0,      ## max tracks CHI2DOF
                      '_h_TRGHP'              : .5,        ## Track Ghost Probability     
                      '_3h_DOCA'              : .2,       ## max DOCA between h and 2h 
                      '_3h_PTmax'             : 1500,     ## min PT of the 3h highest PT track
                      '_3h_DIRA'              : .9999,   ## min cos angle between 3h momentum and PV decay direction   
                      '_3h_FDCHI2'            : 150.,     ## min 3h FDCHI2 wrt best 3h PV  
                      '_3h_SVPV'              : 1.5,      ## Distance between sv and pv
                      '_3h_CHI2'              : 40.0,     ## max 3h vertex CHI2 
                      '_3h_IPCHI2'            : 10.,      ## max 3h IP CHI2 wrt best 3h PV
                      '_3h_PT'                : 1000.,    ## min 3h PT   
                      '_3h_PTsum'             : 4500.,    ## min of 3h tracks PT sum 
                      '_3h_Psum'              : 22000.,   ## min of 3h tracks P sum 
                      '_3h_PVIPCHI2sum'       : 200.,     ## min of the 3h tracks IP wrt best 3h PV
                      '_3h_TRKCHIDOFmin'      : 3.0,       ## max track CHI2DOF for the track with smalest CHI2DOF
                      '_3h_Mmax'              : 6502.,     ## max 3h mass for exclusive lines
                      '_3h_Mmin'              : 5998.,     ## min 3h mass for exclusive KKK lines
                      '_bu3h_Mmax'            : 5502.,     ## min buto3h mass for exclusive lines
                      '_bu3h_Mmin'            : 5098.,     ## min buto3h mass for exclusive lines
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

#StrippingBu2hhh.py
Bu2hhh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bu2hhhBuilder',
    'CONFIG'      : { 'MaxTrSIZE'             : 200 ,      ## GEC maximim recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG)
                      '_h_PT'                 : 100. ,     ## tracks min PT
                      '_h_P'                  : 1500. ,    ## tracks min P  
                      '_h_IPCHI2'             : 1. ,       ## min tracks IP wrt OWNPV
                      '_h_TRCHI2DOF'          : 4.0 ,      ## max tracks CHI2DOF
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

#StrippingBu2Ksthh.py
Bu2Ksthh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bu2KsthhConf',
    'CONFIG'      : {'Trk_Long_Chi2'           : 4.0,
                     'Trk_Long_GhostProb'      : 0.4,
                     'Trk_Down_Chi2'           : 4.0,
                     'Trk_Down_GhostProb'      : 0.5,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_DD_VtxChi2'           : 12.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_LL_MassWindow'        : 20.0,
                     'KS_LL_VtxChi2'           : 12.0,
                     'KS_LL_FDChi2'            : 80.0,
                     'Kstar_MassLo'            : 0.0,
                     'Kstar_MassHi'            : 5000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'B_PTmin'                 : 1500.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_maxDocaChi2'    : 5.0,
                     'BDaug_LL_maxDocaChi2'    : 5.0,
                     'BDaug_DD_PTsum'          : 3000.0,
                     'BDaug_LL_PTsum'          : 3000.0,
                     'B_DD_IPCHI2sum'          : 50.0,
                     'B_VtxChi2'               : 12.0,
                     'B_DD_Dira'               : 0.999,
                     'B_LL_Dira'               : 0.9999,
                     'B_DD_IPCHI2wrtPV'        : 8.0,
                     'B_LL_IPCHI2wrtPV'        : 8.0,
                     'B_DD_FDwrtPV'            : 1.0,
                     'B_LL_FDwrtPV'            : 1.0,
                     'B_DD_FDChi2'             : 50.0,
                     'B_LL_FDChi2'             : 50.0,
                     'GEC_MaxTracks'           : 250,
                     'ConeAngle10'             : 1.0, 
                     'ConeAngle15'             : 1.5, 
                     'ConeAngle17'             : 1.7, 
                     'HLT1Dec'                 : 'Hlt1TrackAllL0Decision', 
                     'HLT2Dec'                 : 'Hlt2Topo[234]Body.*Decision',
                     'Prescale'                : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS' : ['Bhadron']
    }
#StrippingB2Ksthh.py
B2Ksthh = {
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2KsthhConf',
    'CONFIG'      : {'Trk_Chi2'                : 3.0,
                     'Trk_GhostProb'           : 0.3,
                     'Kstar_MassLo'            : 0.0,
                     'Kstar_MassHi'            : 5000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'B_PTmin'                 : 1500.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_PTsum'             : 3200.0,
                     'B_IPCHI2sum'             : 50.0,
                     'B_VtxChi2'               : 12.0,
                     'B_Dira'                  : 0.9995,
                     'B_IPCHI2wrtPV'           : 8.0,
                     'B_FDwrtPV'               : 1.0,
                     'B_FDChi2'                : 50.0,
                     'GEC_MaxTracks'           : 250,
                     'ConeAngle10'             : 1.0, 
                     'ConeAngle15'             : 1.5, 
                     'ConeAngle17'             : 1.7, 
                     'HLT1Dec'                 : 'Hlt1TrackAllL0Decision', 
                     'HLT2Dec'                 : 'Hlt2Topo[234]Body.*Decision',
                     'Prescale'                : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS' : ['Bhadron']
    }

#StrippingB24pLines.py
B24p = {
    "WGs"         : [ "BnoC" ],
    "STREAMS"     : [ "Bhadron" ],
    "BUILDERTYPE" : "B24pLinesConf",
    "CONFIG"      : {
    "MDSTflag"           : False, # True or False
    # TrackCuts
    "MinTrIPChi2"        : 25.0,
    "MaxTrChi2Dof"       : 4.0,
    "MaxTrGhp"           : 0.3,
    # CommonCombiCuts
    "MaxDoca"            : 0.3, # (mm)
    "mDiffb"             : 400, # (MeV)
    # MassCuts
    "mJpsiMin"           : 2990, # (MeV)
    "mJpsiMax"           : 3200, # (MeV)
    "mKstMax"            : 1200, # (MeV)
    # MotherCuts
    "MaxIPChi2"          : 25,
    "MinVDChi2"          : 225,
    "MaxVtxChi2Dof"      : 9,
    "MinDira"            : 0.0,
    "MinTau"             : 1.0, # (ps)
    # scalings
    "Postscale"          : 1, 
    "B24pPrescale"       : 1, 
    "B2JpsiKpiPrescale"  : 1,
    "B2PhiKhPrescale"    : 1, 
    "CommonRelInfoTools" : [ { "Type": "RelInfoVertexIsolation", "Location":"VtxIsoInfo" },
                             { "Type": "RelInfoVertexIsolationBDT", "Location":"VtxIsoInfoBDT" },
                             { "Type" : "RelInfoBs2MuMuBIsolations",
                               "Variables" : [],
                               "Location"  : "BsMuMuBIsolation",
                               "tracktype" : 3,
                               "makeTrackCuts" : False, },
                             ] # closes CommonRelInfoTools    
    } # closes CONFIG
    } # closes default_config
