"""
Line config dictionaries for pA 2015 stripping
"""

from GaudiKernel.SystemOfUnits import *

HeavyIonDiMuon =  {
    'WGs'             : ['ALL'],
    'STREAMS'         : ['ALL'],
    'BUILDERTYPE'     : 'HeavyIonDiMuonConf',
    'CONFIG'          : {
        "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"],
        'CheckPV'    :  False,

        "VCHI2VDOF_max"       : 25,
        "Track_CHI2"       : 3,
        "DOCA_max"            : 0.5,
        "PTmu_min"        : 700,   # MeV
        "AM"              : 2900,   # MeV

        'Prescale'            :  1.0,
        'Postscale'           : 1.0,

        'Hlt1Filter'          : "HLT_PASS_RE('Hlt1DiMuonHighMassDecision')",
        'Hlt2Filter'          : None,
    }
    }

HeavyIonNoPIDDstar =  {
    'WGs'        : ['ALL'],
    'BUILDERTYPE': 'HeavyIonDst2D0KPiForPIDConf',
    'CONFIG'     :{

      'DaugPt'           : 250 * MeV      ## GeV
    , 'DaugP'            : 2.0 * GeV      ## GeV
    , 'DaugIPChi2'       : 16             ## unitless
    , 'DaugTrkChi2'      : 3              ## unitless
    , 'D0MassWin'        : 85 * MeV       ## MeV
    , 'D0Pt'             : 1.5 * GeV      ## GeV
    , 'D0VtxChi2Ndof'    : 10             ## unitless
    , 'D0FDChi2'         : 49             ## unitless
    , 'D0BPVDira'        : 0.9999         ## unitless
    , 'D0IPChi2'         : 20             ## unit
    , 'SlowPiPt'         : 100 * MeV      ## MeV
    , 'SlowPiTrkChi2'    : 3              ## unitless
    , 'DstarPt'          : 2.2 * GeV      ## GeV
    , 'DstarVtxChi2Ndof' : 15             ## unitless
    , 'DeltaM_Min'       : 130 * MeV      ## MeV
    , 'DeltaM_Max'       : 155 * MeV      ## MeV
    ##
    , 'DCS_WrongMass'    : 25 * MeV       ## MeV (3 sigma veto)
    , 'KK_WrongMass'     : 25 * MeV       ## MeV (3 sigma veto)
    , 'PiPi_WrongMass'   : 25 * MeV       ## MeV (3 sigma veto)
    ##
    , 'Prescale'         : 1.00          ## unitless
    , 'Postscale'        : 1.00           ## unitless
    ,"odin": ["NoBeam","Beam1","Beam2","BeamCrossing"]
    , 'Hlt1Filter'          : None  # "HLT_PASS_RE('Hlt1SMOGKPiDecision')"
    ,'Hlt2Filter'          : "HLT_PASS_RE('Hlt2PIDD02KPiTagDecision')"   # no events in 1000, The 'name' is doubly checked correct, because rate too low?

    ##
      },
    'STREAMS'     :['ALL']

}
    
HeavyIonMiniBias =  {
    'WGs'             : ['ALL'],
    'STREAMS'         : ['ALL'],
    'BUILDERTYPE'     : 'HeavyIonMiniBiasConf',
    'CONFIG'          : {
        "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"],
        'CheckPV'    :  False,


        'MicroBiasPrescale'            :  1.0,
        'MicroBiasPostscale'           :  1.0,
        "MicroBiasHlt1Filter"         : "(HLT_PASS('Hlt1MBMicroBiasVeloDecision'))", #(HLT_PASS('Hlt1MB.*Decision')) #Hlt1MBMicroBiasVelo
        "MicroBiasHlt2Filter"         : "(HLT_PASS('Hlt2PassThroughDecision'))",

        'MicroBiasLowMultPrescale'            :  1.0,
        'MicroBiasLowMultPostscale'           :  1.0,
        "MicroBiasLowMultHlt1Filter"         : "(HLT_PASS('Hlt1MBMicroBiasLowMultVeloDecision'))", #(HLT_PASS('Hlt1MB.*Decision')) #Hlt1MBMicroBiasVelo
        "MicroBiasLowMultHlt2Filter"         : "(HLT_PASS('Hlt2PassThroughDecision'))",

        'NoBiasPrescale'            :  1.0,
        'NoBiasPostscale'           :  1.0,
        "NoBiasHlt1Filter"         : "(HLT_PASS('Hlt1MBNoBiasLeadingCrossingDecision'))", #(HLT_PASS('Hlt1MB.*Decision')) #Hlt1MBMicroBiasVelo
        "NoBiasHlt2Filter"         : "(HLT_PASS('Hlt2PassThroughDecision'))",

      }
    }

HeavyIonOpenCharm = {
    'WGs': ['ALL'],
    'BUILDERTYPE': 'HeavyIonOpenCharmConf',
    'STREAMS': ['ALL'],
    'CONFIG': {
        "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"], #"( ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2) | (ODIN_BXTYP == LHCb.ODIN.BeamCrossing) | (ODIN_BXTYP == LHCb.ODIN.NoBeam)",        ##[ 'NoBeam', 'BeamCrossing','Beam1','Beam2']
        'Track_CHI2'         : 3,         # Minimum track chi2
        #
        #####################
        ###  D0 -> K- pi+   #
        #####################
        'D0_Daug_All_PT_MIN'         : 250.0*MeV,         # Minimum transverse momentum all D0 daughters must satisfy before fit
        'D0_Daug_AllA_PT_MIN'         : 500.0*MeV,         # Minimum transverse momentum at least one D0 daughters must satisfy after fit
        'D0_Daug_P_MIN'              : 3.0*GeV,           # Minimum D0 daughter momentum
        'D0_Daug_P_MAX'              : 1.E6*GeV,          # Maximum D0 daughter momentum,       'do it offline"
        'D0_Daug_ETA_MIN'            : 0.0,               # Minimum D0 daughter pseudorapidity, 'do it offline'
        'D0_Daug_ETA_MAX'            : 10.0,              # Maximum D0 daughter pseudorapidity, 'do it offline'
        'D0_K_PIDK_MIN'              : 5.0,               # Minimum Dpm daughter kaon DLLK       'in trigger of pAr hlt2'
        'D0_Pi_PIDK_MAX'             : 5.0,              # Maximum Dpm daughter pion DLLK,       'in trigger of pAr hlt2'
        'D0_Comb_ADOCAMAX_MAX'       : 0.5,               # DOCA
        'D0_Daug_All_BPVIPCHI2_MIN'  : 4.0,               # Minimum best primary vertex IP chi^2 all D0 daughters must satisfy
        'D0_VCHI2VDOF_MAX'           : 10.0,              # Maximum Dpm vertex chi^2 per vertex fit DoF 
        'D0_acosBPVDIRA_MAX'         : 35.0*mrad,         # Maximum angle between D0 momentum and D0 direction of flight, '10 at HLT2 of pAr'

        'D0_PVDispCut'               : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.100*picosecond))', # Primary vertex displacement requirement
        'D0_ADAMASS_WIN'             : 100.0*MeV,         # D0 mass window around the nominal D0 mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'D0_Prescale':  1.0,
        'D0_Postscale': 1.0, 

        'D0_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOGSingleTrackDecision')",   #Hlt1SMOGD0KPiDecision ?
        'D0_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGD02KPiDecision')",


        ##################################
        ###  D*+ -> (D0 -> K- pi+) pi+   #
        ##################################
        'Dst_AMDiff_MAX'        : 160.0*MeV,         # Maximum delta mass value m(D*{0,+}) - m(D0)
        'Dst_VCHI2VDOF_MAX'     : 10.0,              # Maximum D*+ vertex chi^2 per vertex DoF (_not_ applied to D*0)

        'Dst_Prescale'     : 1.0, 
        'Dst_Postscale'    : 1.0,

        'Dst_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOGSingleTrackDecision')",   #Hlt1SMOGD0KPiDecision ?
        'Dst_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGD02KPiDecision')",


        ######################################
        # D0 -> K pi No PV line, to estimate PV efficiency for open charm analysis 
        ######################################
        'NoPVD0_Daug_All_PT_MIN'         : 800.0*MeV,         # Minimum transverse momentum all D0 daughters must satisfy
        'NoPVD0_Daug_P_MIN'              : 5.0*GeV,           # Minimum D0 daughter momentum
        'NoPVD0_Daug_P_MAX'              : 200.*GeV,          # Maximum D0 daughter momentum,       'do it offline"
        'NoPVD0_Daug_ETA_MIN'            : 2.0,               # Minimum D0 daughter pseudorapidity, 'do it offline'
        'NoPVD0_Daug_ETA_MAX'            : 5.0,              # Maximum D0 daughter pseudorapidity, 'do it offline'
        'NoPVD0_K_PIDK_MIN'              : 5.0,               # Minimum Dpm daughter kaon DLLK       'in trigger of pAr hlt2'
        'NoPVD0_Pi_PIDK_MAX'             : 5.0,              # Maximum Dpm daughter pion DLLK,       'in trigger of pAr hlt2'
        'NoPVD0_VCHI2VDOF_MAX'           : 6.0,              # Maximum Dpm vertex chi^2 per vertex fit DoF 
        'NoPVD0_Comb_ADOCAMAX_MAX'       : 0.5,               # DOCA

        'NoPVD0_ADAMASS_WIN'             : 100.0*MeV,         # D0 mass window around the nominal D0 mass before the vertex fit, '90 MeV at HLT2 of pAr'
            'NoPVD0_PT'                      : 1000.0*MeV,
            'NoPVD0_VTX_Xmin'                      : -5.0*mm,
            'NoPVD0_VTX_Xmax'                      : +5.0*mm,
            'NoPVD0_VTX_Ymin'                      : -5.0*mm,
            'NoPVD0_VTX_Ymax'                      : +5.0*mm,
            'NoPVD0_VTX_Zmin'                      : -1000.*mm,
            'NoPVD0_VTX_Zmax'                      : +1000.*mm,
            'NoPVD0_odin'                      : ["NoBeam","Beam1","Beam2"],

        'NoPVD0_Prescale':  1.0,
        'NoPVD0_Postscale': 1.0, 

        'NoPVD0_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOGSingleTrackDecision')",
        'NoPVD0_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGD02KPiDecision')",

        ########################################
        ### D+ -> K- pi+ pi+, Ds+ -> K- K+ pi+, Lambda_c+ -> p+ K- pi+ #
        ########################################
        'D_Daug_All_PT_MIN'             : 200.0*MeV,         # Minimum transverse momentum all D daughters must satisfy
        'D_Daug_2of3_PT_MIN'            : 400.0*MeV, # Minimum transverse momentum at least 2 D daughters must satisfy
        'D_Daug_1of3_PT_MIN'            : 1000.0*MeV, # Minimum transverse momentum at least 1 D daughter must satisfy
        'D_Daug_All_BPVIPCHI2_MIN'      : 2.0,               # Minimum best primary vertex IP chi^2 all D daughters must satisfy
        'D_Daug_2of3_BPVIPCHI2_MIN'     : 4.0, # Minimum best PV IP chi^2 at least 2 D daughters must satisfy
        'D_Daug_1of3_BPVIPCHI2_MIN'     : 9.0, # Minimum best PV IP chi^2 at least 1 D daughter must satisfy
        'D_Daug_P_MIN'                  : 3.0*GeV,           # Minimum D daughter momentum
        'D_Daug_P_MAX'                  : 1.E6*GeV,          # Maximum D daughter momentum,       'do it offline"
        'D_Daug_ETA_MIN'                : 0.0,               # Minimum D daughter pseudorapidity, 'do it offline'
        'D_Daug_ETA_MAX'                : 10.0,              # Maximum D daughter pseudorapidity, 'do it offline'
        'D_K_PIDK_MIN'                  : 5.0,               # Minimum D daughter kaon DLLK       'in trigger of pAr hlt2'
        'D_Pi_PIDK_MAX'                 : 5.0,              # Maximum D daughter pion DLLK,       'in trigger of pAr hlt2'
        'D_P_PIDp_MIN'                  : 5.0,              # Maximum D (Lc) daughter proton DLLp,       'in trigger of pAr hlt2'
        'D_P_PIDpPIDK_MIN'              : 5.0,              # Maximum D (Lc) daughter proton DLLp-DLLK,       'in trigger of pAr hlt2'
        'D_Comb_ADOCAMAX_MAX'           : 0.5,         # Maximum distance of closest approach of D daughters
        #'D_Comb_ADOCAMAX_MAX'          : 0.5*mm,         # Maximum distance of closest approach of D daughters
        'D_VCHI2VDOF_MAX'               : 25.0,              # Maximum Dpm vertex chi^2 per vertex fit DoF
        'D_acosBPVDIRA_MAX'             : 35.0*mrad,         # Maximum angle between Dpm momentum and Dpm direction of flight

        'Dp_PVDispCut'            : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.1*picosecond))', # Primary vertex displacement requirement
        'Dp_ADAMASS_WIN'          : 100.0*MeV,         # D+ mass window around the nominal D+ mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'Dp_Prescale': 1.0,
        'Dp_Postscale': 1.0, 

        'Dp_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOGSingleTrackDecision')",   #Hlt1SMOGDpmKKPiDecision ?
        'Dp_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGDpm2KPiPiDecision')",
       

        'Ds_PVDispCut'            : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.1*picosecond))', # Primary vertex displacement requirement
        'Ds_ADAMASS_WIN'          : 100.0*MeV,         # Ds+ mass window around the nominal Ds+ mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'Ds_Prescale': 1.0,
        'Ds_Postscale': 1.0, 

        # HLT filters, only process events firing triggers matching the RegEx
        'Ds_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOGSingleTrackDecision')",   #Hlt1SMOGDpmKPiDecision ?
        'Ds_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGDs2KKPiDecision')",


        'Lc_PVDispCut'            : '((BPVVDCHI2 > 9.0)|(BPVLTIME() > 0.1*picosecond))', # Primary vertex displacement requirement
        'Lc_ADAMASS_WIN'          : 100.0*MeV,         # Lc+ mass window around the nominal Lc+ mass before the vertex fit, '90 MeV at HLT2 of pAr'

        'Lc_Prescale': 1.0,
        'Lc_Postscale': 1.0, 

        # HLT filters, only process events firing triggers matching the RegEx
        'Lc_Hlt1Filter': "HLT_PASS_RE('Hlt1SMOGSingleTrackDecision')",   #Hlt1SMOGDpmKPiDecision ?
        'Lc_Hlt2Filter': "HLT_PASS_RE('Hlt2SMOGLc2KPPiDecision')",
    }
}

HeavyIonPIDCalib = {
    'WGs'               : [ 'ALL' ],
    'BUILDERTYPE'       : 'HeavyIonPIDCalibConf',
    'STREAMS'           : [ 'ALL' ],
    'CONFIG'            : {
      "odin": ["NoBeam","Beam1","Beam2","BeamCrossing"], #"( ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2) | (ODIN_BXTYP == LHCb.ODIN.BeamCrossing) | (ODIN_BXTYP == LHCb.ODIN.NoBeam)",        ##[ 'NoBeam', 'BeamCrossing','Beam1','Beam2']

      'Jpsi2MuMuPrompt'       : { # Replaces old StrippingMuIDCalib_JpsiKFromBNoPIDNoMip
        'Prescale'      : 1.0
        , 'CheckPV'     : False
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None # "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None # "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "J/psi(1S) -> mu+ mu-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdAllNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(ISLONG) & (P>3*GeV) & (PT>500*MeV) & (TRCHI2DOF<3)"
          }
        , 'TagAndProbeCut'     : "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV)" + "& (ACHILD(PIDmu, #tag)>0.)"
        , 'CombinationCut'     : "(in_range(3096.916-160, AM, 3096.916+160)) & (ACHI2DOCA(1,2) < 10)"
        , 'MotherCut'          : "(VFASPF(VCHI2)<5)"
      },


      'Jpsi2MuMu'       : { # Replaces old StrippingMuIDCalib_JpsiKFromBNoPIDNoMip
        'Prescale'      : 1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : "HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : "HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "J/psi(1S) -> mu+ mu-"
        , 'TagAndProbeIndices' : [ 1, 2 ]
        , 'InputTES'    : [ 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(ISLONG) & (P>3*GeV) & (PT>800*MeV) & (MIPCHI2DV(PRIMARY)>10) & (TRCHI2DOF<3)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'CombinationCut'     :
            "(in_range(3096-200, AM, 3096+200)) & (ACHI2DOCA(1,2) < 10)"
        , 'MotherCut'          :
            "(VFASPF(VCHI2)<5) & (BPVVDCHI2 > 225)"
      },



      'L02ppi' : {
        'Prescale'      : .14
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[Lambda0 -> p+ pi-]cc"
        , 'InputTES'    : [ 'Phys/StdAllNoPIDsPions/Particles', 
                                   'Phys/StdAllNoPIDsProtons/Particles' ]
        , 'DaughtersCuts'       : {
            'p+'               : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )",
            'pi-'              : "( 2.0 * GeV < P ) &  ( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 35 )"
          }
        , 'CombinationCut'     :
            "(AM < 1.5 * GeV) & (ACHI2DOCA(1,2) < 20)"
        , 'MotherCut'          : (
            "   ( ADMASS ( 'Lambda0') < 25 ) " + 
            " & ( in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) )" + 
            " & ( VFASPF ( VZ ) < 2200 ) "  +
            " & ( in_range ( 0 , BPVLTFITCHI2() , 49 ) )" + 
            " & ( BPVLTIME() * c_light > 5  ) " +
            " & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') ) > 20 )"
          )
      },

      'L02ppiHighPT' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(PT > 3.5*GeV) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiVeryHighPT' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(PT > 10*GeV) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiIsMuon' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : 1.0
        , 'DaughtersCuts'       : {
            'p+'               : "(ISMUON) & ( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )",
            'pi-'              : "( 2.0*GeV < P ) &( TRCHI2DOF < 5 ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
      },

      'L02ppiDD' : {
        'CloneLine' : "L02ppi"
        , 'Prescale'  : .14
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDHighPT' : {
        'CloneLine' : "L02ppiHighPT"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },

      'L02ppiDDVeryHighPT' : {
        'CloneLine' : "L02ppiVeryHighPT"
        , 'Prescale'  : 1.0
        , 'InputTES'    : [ 'Phys/StdNoPIDsDownPions/Particles', 
                                   'Phys/StdNoPIDsDownProtons/Particles' ]
      },


      'Bu2Jpsi_mumuK' :  {
        'Prescale'       : 1.0
        , 'CheckPV'      : True
        , 'RawEvent'    : ['Calo']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'    : "[B+ -> J/psi(1S) K+]cc"
        , 'DecayDescriptorJpsi': "J/psi(1S) -> mu+ mu-"
        , 'InputTES'    : [ 'Phys/StdNoPIDsKaons/Particles' ]
        , 'InputTESJpsi': [ 'Phys/StdNoPIDsMuons/Particles' ]
        , 'DaughtersCuts'       : {
            'mu+' : "(BPVIPCHI2()> 10.0) ",
            'K+'  : "(MIPCHI2DV(PRIMARY)>25)"
          }
        , 'TagAndProbeCut'     :
            "(ACHILDCUT(ISMUON, #tag)) & (ACHILD(P, #tag) > 6*GeV) & (ACHILD(PT, #tag) > 1.5*GeV) & (ACHILD(MIPCHI2DV(PRIMARY), #tag) > 25)"
        , 'JpsiCombinationCut' :
            "(ADAMASS('J/psi(1S)')<200*MeV)"
        , 'JpsiMotherCut'      :
            "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) "
        , 'CombinationCut'     :
            "ADAMASS('B+') < 500.*MeV"
        , 'MotherCut'          :
            "(BPVIPCHI2()<25)"
      },


      'Ks02pipi'      :{
        'Prescale'      : 0.024
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'DecayDescriptor'       : "KS0 -> pi+ pi-"
        , 'InputTES'              : ['Phys/StdNoPIDsPions/Particles']
        , 'DaughtersCuts'         : {
            'pi+' : " ( 2.0 * GeV < P ) & ( MIPCHI2DV(PRIMARY) > 25 )"
          }
        , 'CombinationCut'        : " ( ADAMASS('KS0') < 100 ) & (ACHI2DOCA(1,2) < 20 )"
        , 'MotherCut'             : (
            "( ADMASS ( 'KS0') < 50 ) & "+
            " in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) & " + 
            " ( VFASPF ( VZ ) < 2200 ) & " + 
            " (BPVVDCHI2 > 25) & " +
            " ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 9 ) & " + 
            " ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 9 )"
          )
      },
    
      'Ks02pipiDD' : {
        'CloneLine'    : 'Ks02pipi'
        , 'Prescale'   : 0.024
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'InputTES'   : ['Phys/StdNoPIDsDownPions/Particles']
        , 'MotherCut'             : (
            "( ADMASS ( 'KS0') < 50 ) & "+
            " in_range ( 0 , VFASPF ( VCHI2 ) , 16 ) & " + 
            " ( VFASPF ( VZ ) < 2200 ) & " + 
            " (BPVVDCHI2 > 25) & " +
            " ( ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) > 18 ) & " + 
            " ( ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > 18 )"
        )
      },
    
      'Lc2pKpi' : {
        'Prescale':   1.0
        , 'CheckPV'     : True
        , 'RawEvent'    : ['Muon']
        , 'MDST.DST'    : False
        , 'Hlt1Filter'        : None#"HLT_PASS_RE('Hlt1.*Decision')" 
        , 'Hlt2Filter'        : None#"HLT_PASS_RE('Hlt2.*Decision')" 
        , 'RefitPV'     : False
        , 'InputTES' : [  'Phys/StdNoPIDsKaons/Particles' , 
                          'Phys/StdNoPIDsPions/Particles' , 
                          'Phys/StdNoPIDsProtons/Particles' 
                       ]
        , 'DecayDescriptor' : "[Lambda_c+ -> K- p+ pi+]cc"
        , 'DaughtersCuts' : {
          'K+'    : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) ' 
          , 'p+'  : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) ' 
          , 'pi+' : '( PT>250*MeV ) & ( P>2*GeV ) & ( TRPCHI2>0.0001 ) & ( MIPCHI2DV(PRIMARY)>8. ) '
          }
        , 'Combination12Cut' : "( ACHI2DOCA(1,2) < 25 )"
        , 'CombinationCut'   : "( ADAMASS('Lambda_c+')<150*MeV ) & ( APT>1.*GeV ) & ( ADOCACHI2CUT(50, '') )"
        , 'MotherCut'        : (
            "( M > 2.240*GeV ) & " +
            " ( M<2.330*GeV ) & " +
            " ( VFASPF(VCHI2/VDOF)<8 ) & " +
            " ( BPVDIRA>0.99999 ) & " +
            " ( MIPCHI2DV(PRIMARY)<4. ) & " +
            " in_range( 0.85*GeV, M13, 0.95*GeV ) & " +
            " ( (WM( 'K-' , 'pi+' , 'pi+' )>1.89*GeV) | (WM( 'K-' , 'pi+' , 'pi+' )<1.80*GeV) )"
        )
    }
  }
}

HeavyIonTrackEffDownMuon = {
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'HeavyIonTrackEffDownMuonConf',
    'CONFIG'      : { 
                                'MuMom':                2000.   # MeV
                        ,       'MuTMom':               200.    # MeV
                        ,       'ZMuMom':               0.      # MeV
                        ,       'ZMuTMom':              20000.  # MeV
                        ,       'UpsilonMuMom':         0.      # MeV
                        ,       'UpsilonMuTMom':        500.    # MeV
                        ,       'ZMuMinEta':            2.      # adimensional
                        ,       'ZMuMaxEta':            4.5     # adimensional
                        ,       'TrChi2':               10.     # MeV
                        ,       'MassPreComb':          2000.   # MeV
                        ,       'MassPostComb':         200.    # MeV
                        ,       'ZMassPreComb':         100000. # MeV
                        ,       'ZMassPostComb':        1500.   # MeV
                        ,       'UpsilonMassPreComb':   100000. # MeV
                        ,       'UpsilonMassPostComb':  00.     # MeV
                        ,       'Doca':                 5.      # mm
                        ,       'VertChi2':             25.     # adimensional
                        ,       'SeedingMinP':          1500.   # MeV
                        ,       'DataType':             '2011'  # for ConfiguredMuonIDs configuration
                        ,       'NominalLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
                        ,       'NominalLinePostscale': 1.
                        ,       'ValidationLinePrescale':0.0015 #0.5 in stripping15: 0.1 gives 1.42% retention rate , ValidationLine further prescaled
                        ,       'ValidationLinePostscale': 1.
                        ,       'ZLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
                        ,       'ZLinePostscale':  1.  
                        ,       'UpsilonLinePrescale':  1. # proposal: 0.2 to stay below 0.15% retention rate 
                        ,       'UpsilonLinePostscale':  1. 
            ,   'JpsiHlt1Filter' : 'Hlt1.*Decision'
            ,   'JpsiHlt2Filter' : 'Hlt2.*Decision'
            #'JpsiHlt1Triggers'            :  { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0},
                        ,       'HLT1TisTosSpecs': { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
                        ,       'ZHLT1TisTosSpecs'      : { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
                        ,       'UpsilonHLT1TisTosSpecs': { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
                        ,       'HLT1PassOnAll': True
                        ,       'HLT2TisTosSpecs': { "Hlt2TrackEffDiMuonDownstream.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0} #reg. expression allowed
                        ,       'ZHLT2TisTosSpecs': { "Hlt2TrackEffDiMuonDownstream.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0} #reg. expression allowed
                        ,       'UpsilonHLT2TisTosSpecs': { "Hlt2TrackEffDiMuonDownstream.*Decision%TOS": 0, "Hlt2.*Decision%TOS" : 0} #reg. expression allowed
                        ,       'HLT2PassOnAll': False
                    },
    'STREAMS'     :  ["ALL"]
    }


HeavyIonTrackEffMuonTT = {
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'HeavyIonTrackEffMuonTTConf',
    'CONFIG'      : { 
                        'JpsiMassWin'                 : 500,
                        'UpsilonMassWin'              : 1500,
                        'ZMassWin'                    : 40000,
                        'BMassWin'                    : 500,
                        'JpsiMuonTTPT'                : 0,
                        'UpsilonMuonTTPT'             : 500,
                        'ZMuonTTPT'                   : 500,
                        'JpsiLongPT'                  : 1300,
                        'UpsilonLongPT'               : 1000,
                        'ZLongPT'                     : 10000,
                        'JpsiPT'                      : 1000,
                        'UpsilonPT'                   : 0,
                        'ZPT'                         : 0,
                        'JpsiLongMuonMinIP'           : 0.0, # removed, yanxi
                        'UpsilonLongMuonMinIP'        : 0,
                        'ZLongMuonMinIP'              : 0,
                        'JpsiMINIP'                   : 10000,   # make it dummy, yanxi
                        'UpsilonMINIP'                : 10000, #this is a dummy
                        'ZMINIP'                      : 10000, #this is a dummy
                        'BJpsiKMINIP'                 : 10000, #this is a dummy
                        'JpsiLongMuonTrackCHI2'       : 5,
                        'UpsilonLongMuonTrackCHI2'    : 5,
                        'ZLongMuonTrackCHI2'          : 5,
                        'VertexChi2'                  : 5,
                        'LongMuonPID'                 : 2,
            'JpsiHlt1Filter'              : 'Hlt1.*Decision',
            'JpsiHlt2Filter'              : 'Hlt2.*Decision',
            'JpsiHlt1Triggers'            :  { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0},
            'Hlt1PassOnAll'               : True,
                        'UpsilonHlt1Triggers'         :  { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0},
                        'ZHlt1Triggers'               :  { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0},
            'JpsiHlt2Triggers'            :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0},  # for test
                        #'JpsiHlt2Triggers'            :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0},
                        'UpsilonHlt2Triggers'         :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0},
                        'ZHlt2Triggers'               :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0},
                        'BJpsiKHlt2TriggersTUS'       :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TUS" : 0, "Hlt2.*Decision%TUS" : 0},
                        'BJpsiKHlt2TriggersTOS'       :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0, "Hlt2.*Decision%TIS" : 0},
                        'JpsiPrescale'                : 1,
                        'UpsilonPrescale'             : 1,
                        'ZPrescale'                   : 1,
                        'BJpsiKPrescale'              : 1,
                        'Postscale'                   : 1
                    },
    'STREAMS'     :   ["ALL"]
    }

HeavyIonTrackEffVeloMuon = {
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'HeavyIonTrackEffVeloMuonConf',
    'CONFIG'      : {
                        "TrChi2VeMu":           5.      # adimensional
                ,       "TrChi2LongMu":         3.      # adimensional
                ,       "JpsiPt":               0.5     # GeV
                ,       "ZPt":                  0.5     # GeV
                ,       "UpsilonPt":            0.5     # GeV
                ,       "TrPt":                 100.    # MeV
                ,       "TrP":                  5.      # GeV
                ,       "ZTrPt":                20000.  # MeV
                ,       "ZTrP":                 0.      # MeV
                ,       "UpsilonTrPt":          500.    # MeV
                ,       "UpsilonTrP":           0.      # MeV
                ,       "LongP":                7.      # GeV
                ,       "ZTrMinEta":            2.0     # adimensional
                ,       "ZTrMaxEta":            4.5     # adimensional
                ,       "MuDLL":                1.      # adimensional
                ,       "VertChi2":             2.      # adimensional
                ,       "ZVertChi2":            10000.  # adimensional, dummy
                ,       "UpsilonVertChi2":      10000.  # adimensional, dummy
                ,       "MassPreComb":          1000.   # MeV
                ,       "ZMassPreComb":         100000. # MeV
                ,       "UpsilonMassPreComb":   100000. # MeV
                ,       "MassPostComb":         400.    # MeV
                ,       "ZMassPostComb":        40000.  # MeV
                ,       "UpsilonMassPostComb":  1500.   # MeV
                ,       "Prescale":             1.      # adimensional
                ,       "ZPrescale":            1.      # adimensional
                ,       "UpsilonPrescale":      1.      # adimensional
                ,       "Postscale":            1.      # adimensional
                ,       "ZPostscale":           1.      # adimensional
                ,       "UpsilonPostscale":     1.      # adimensional
                ,       "JpsiHlt1Filter" : 'Hlt1.*Decision'
        ,               'JpsiHlt2Filter' : 'Hlt2.*Decision'
                ,       'HLT1TisTosSpecs'       : { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0} #no reg. expression allowed(see selHlt1Jpsi )
                ,       'ZHLT1TisTosSpecs'      : { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
                ,       'UpsilonHLT1TisTosSpecs': { "Hlt1GlobalDecision%TOS" : 0, "Hlt1GlobalDecision%TIS" : 0 } #no reg. expression allowed(see selHlt1Jpsi )
                ,       'HLT1PassOnAll'         : True
            #           'UpsilonHlt2Triggers'         :  { "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0, "Hlt2.*Decision%TIS" : 0},
                ,       'HLT2TisTosSpecs'       : { "Hlt2TrackEffDiMuonVeloMuon.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0} #reg. expression allowed
                ,       'ZHLT2TisTosSpecs'      : { "Hlt2TrackEffDiMuonVeloMuon.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0} #reg. expression allowed
                ,       'UpsilonHLT2TisTosSpecs': { "Hlt2TrackEffDiMuonVeloMuon.*Decision%TOS" : 0, "Hlt2.*Decision%TOS" : 0} #reg. expression allowed
                ,       'HLT2PassOnAll'         : False
         },
    'STREAMS'     :  ["ALL"]
    }



