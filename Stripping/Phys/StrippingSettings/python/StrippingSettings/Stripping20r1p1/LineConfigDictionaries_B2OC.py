#Line config dictionaries for the B2OC WG
Beauty2Charm = {
    'BUILDERTYPE' : 'Beauty2CharmConf',
    'CONFIG' : {
    "ALL" : { # Cuts made on all charged input particles in all lines (excpt. upstream)
    'TRCHI2DOF_MAX' : 3,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4, 
    'TRGHP_MAX'     : 0.4
    },
    "UPSTREAM" : { # Cuts made on all upstream particles
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4
    },
    "KS0" : { # Cuts made on all K shorts
    'PT_MIN'        : '250*MeV',
    'BPVVDCHI2_MIN' : 36,
    'MM_MIN'        : '467.*MeV',
    'MM_MAX'        : '527.*MeV'
    },
    "Pi0" : { # Cuts made on all pi0's
    'PT_MIN'        : '500*MeV',
    'P_MIN'         : '1000*MeV',
    'CHILDCL1_MIN'  : 0.25,
    'CHILDCL2_MIN'  : 0.25,
    'FROM_B_P_MIN'  : '2000*MeV'
    },
    "D2X" : { # Cuts made on all D's and Lc's used in all lines 
    'ASUMPT_MIN'    : '1800*MeV',
    'AMAXDOCA_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '100*MeV'
    },
    "B2X" : { # Cuts made on all B's and Lb's used in all lines
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 10,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.2*ps',
    'BPVDIRA_MIN'   : 0.999,
    'AM_MIN'        : '4750*MeV', # Lb->X sets this to 5200*MeV
    'AM_MAX'        : '7000*MeV', # B->Dh+-h0 sets this to 5800*MeV
    'B2CBBDT_MIN'   : 0.05
    },
    "Dstar" : { # Cuts made on all D*'s used in all lines 
    'AMAXDOCA_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '50*MeV'
    },
    "HH": { # Cuts for rho, K*, phi, XHH Dalitz analyese, etc.
    'MASS_WINDOW'   : {'KST':'150*MeV','RHO':'150*MeV','PHI':'150*MeV'},
    'DAUGHTERS'     : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV'},
    'AMAXDOCA_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 16,
    'BPVVDCHI2_MIN' : 16, 
    'BPVDIRA_MIN'   : 0,
    'ASUMPT_MIN'    : '1000*MeV',
    'pP_MIN'        : '5000*MeV' # for pH only (obviously)
    },
    "HHH": { # Cuts for PiPiPi, KPiPi analyese, etc.
    'MASS_WINDOW'   : {'A1':'3000*MeV','K1':'3000*MeV','PPH':'3600*MeV'},
    'KDAUGHTERS'    : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDK_MIN':'-5'},
    'PiDAUGHTERS'   : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDK_MAX':'10'},
    'pDAUGHTERS'    : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDp_MIN':'-5'},
    'AMAXDOCA_MAX'  : '0.40*mm',
    'VCHI2DOF_MAX'  : 8,
    'BPVVDCHI2_MIN' : 16, 
    'BPVDIRA_MIN'   : 0.98,
    'ASUMPT_MIN'    : '1250*MeV',
    'MIPCHI2DV_MIN' : 0.0,
    'BPVVDRHO_MIN'  : '0.1*mm',
    'BPVVDZ_MIN'    : '2.0*mm',
    'PTMIN1'       : '300*MeV',
    'PID'           : {'TIGHTERPI' : { 'P' : {'PIDp_MIN' : -10},
                                       'PI': {'PIDK_MAX' : 8},
                                       'K' : {'PIDK_MIN' : -10}},
                       'REALTIGHTK' : { 'P' : {'PIDp_MIN' : -10},
                                        'PI': {'PIDK_MAX' : 10},
                                        'K' : {'PIDK_MIN' : 4}}}
    },
    'PID' : {
    'P'  : {'PIDp_MIN' : -10},
    'PI' : {'PIDK_MAX' : 20},
    'K'  : {'PIDK_MIN' : -10},
    'TIGHT' : {    'P'  : {'PIDp_MIN' : -5},
                   'PI' : {'PIDK_MAX' : 10},
                   'K'  : {'PIDK_MIN' : -5}},
    'TIGHTER' : {    'P'  : {'PIDp_MIN' : 0},
                     'PI' : {'PIDK_MAX' : 10},
                     'K'  : {'PIDK_MIN' : 0}},
    'TIGHTPI' : { 'P' : {'PIDp_MIN' : -10},
                  'PI': {'PIDK_MAX' : 10},
                  'K' : {'PIDK_MIN' : -10}},
    'SPECIAL' : {    'P'  : {'PIDp_MIN' : -5},
                     'PI' : {'PIDK_MAX' : 5},
                     'K'  : {'PIDK_MIN' : 5}},
    'SPECIALPI': {'P' : {'PIDp_MIN' : -10},
                  'PI': {'PIDK_MAX' : 12},
                  'K' : {'PIDK_MIN' : -10}}
    },
    '2TOPO' : {'ANGLE_MIN': (2/57.),'M_MIN':19000,'DPHI_MIN':0},
    'BB' : {'ADDSUMPT':0,'COSANGLE_MAX':0.99,'COSDPHI_MAX':0,'M_MIN':0,'MAXPT_MIN': 4000},
    'D0INC' : {'PT_MIN' : 1000, 'IPCHI2_MIN': 100},
    "Prescales" : { # Prescales for individual lines
    'RUN_BY_DEFAULT' : True, # False = lines off by default
    'RUN_RE'         : ['.*'],  
    # Defaults are defined in, eg, Beauty2Charm_B2DXBuilder.py.  Put the full
    # line name here to override. E.g. 'B2D0HD2HHBeauty2CharmTOSLine':0.5.
    'B02D0PiPiD2HHBeauty2CharmLine'      : 1.0,
    'B02DHHWSD2HHBeauty2CharmLine'       : 0.1,
    'B2DPiPiD2HHHCFPIDBeauty2CharmLine'  : 1.0,
    'B2DHHOSD2HHHCFPIDBeauty2CharmLine'  : 1.0,
    'B02DPiNoIPDs2HHHPIDBeauty2CharmLine': 1.0,
    'Lb2XicPiXic2PKPiBeauty2CharmLine'     : 1.0,
    'Lb2XicKXic2PKPiBeauty2CharmLine'     : 1.0,
    'Lb2XicPiWSXic2PKPiBeauty2CharmLine' : 0.1,
    'Lb2XicKWSXic2PKPiBeauty2CharmLine'  : 0.1,
    'X2LcLcBeauty2CharmLine'    : 1.0,
    'X2LcLcWSBeauty2CharmLine'  : 0.1,
    'B02DKLTUBD2HHHBeauty2CharmLine' : 0.04,
    'B02D0D0Beauty2CharmLine'   : 1.0,
    'B02DDWSBeauty2CharmLine'   : 1.0
    },
    'GECNTrkMax'   : 250
    }, 
    'STREAMS' : { 
    'BhadronCompleteEvent' : [
    'StrippingB02D0PPbarD2HHFULLDSTBeauty2CharmLine'
    ]
    },
    'WGs' : [ 'B2OC' ]
    }
