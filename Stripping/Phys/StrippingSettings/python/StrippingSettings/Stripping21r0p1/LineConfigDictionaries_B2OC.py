#########################################################################################################
##                          S T R I P P I N G 21r0p1                                                      ##
##                                                                                                     ##
##  Configuration for B2OC WG                                                                          ##
##  Contact person: Wenbin Qian & Nicola Anne Skidmore (wenbin.qian@cern.ch & nicola.skidmore@cern.ch) ##
#########################################################################################################

from GaudiKernel.SystemOfUnits import *

Beauty2Charm = {
  'BUILDERTYPE' : 'Beauty2CharmConf',
  'CONFIG' : {
    "ALL" : { # Cuts made on all charged input particles in all lines (expt. upstream)
    'TRCHI2DOF_MAX' : 999.,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4., 
    'TRGHP_MAX'     : 0.4
    },
    "UPSTREAM" : { # Cuts made on all upstream particles
    'TRCHI2DOF_MAX' : 999.,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4.
    },
    "KS0" : { # Cuts made on all K shorts
    'PT_MIN'        : '250*MeV',
    'BPVVDCHI2_MIN' : 36,
    'MM_MIN'        : '467.*MeV',
    'MM_MAX'        : '527.*MeV'
    },
    "Lambda0" : { # Cuts made on all Lambda0's
    'PT_MIN'        : '250*MeV',
    'BPVVDCHI2_MIN' : 36,
    'MM_MIN'        : '1086.*MeV',
    'MM_MAX'        : '1146.*MeV'
    },
    "Pi0" : { # Cuts made on all pi0's
    'PT_MIN'        : '500*MeV',
    'P_MIN'         : '1000*MeV',
    'CHILDCL1_MIN'  : 0.25,
    'CHILDCL2_MIN'  : 0.25,
    'FROM_B_P_MIN'  : '2000*MeV',
    'TIGHT_PT_MIN'  : '1000*MeV'
    },
    "gamma" : { # Cuts made on all photons
    'PT_MIN'     : '800*MeV',
    'CL_MIN'     : 0.25,
    'ISNOTE_MIN' : -999.0,
    'PT_VLAPH'   : '90*MeV'
    },
    "D2X" : { # Cuts made on all D's and Lc's used in all lines 
    'ASUMPT_MIN'    : '1800*MeV',
    'ADOCA12_MAX'   : '0.5*mm',
    'ADOCA13_MAX'   : '0.5*mm',
    'ADOCA23_MAX'   : '0.5*mm',
    'ADOCA14_MAX'   : '0.5*mm',
    'ADOCA24_MAX'   : '0.5*mm',
    'ADOCA34_MAX'   : '0.5*mm',
    'ADOCA15_MAX'   : '0.5*mm',
    'ADOCA25_MAX'   : '0.5*mm',
    'ADOCA35_MAX'   : '0.5*mm',
    'ADOCA45_MAX'   : '0.5*mm',
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
    "Bc2DD" : { # Cuts made on Bc -> DD lines
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 10,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.05*ps',
    'BPVDIRA_MIN'   : 0.999,
    'AM_MIN'        : '4800*MeV', 
    'AM_MAX'        : '6800*MeV',
    'B2CBBDT_MIN'   : -999.9
    },
    "Bc2BX" : { # Cuts made on Bc -> BHH lines
    'SUMPT_MIN'     : '1000*MeV',
    'VCHI2DOF_MAX'  : 10,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.05*ps',
    'BPVDIRA_MIN'   : 0.999,
    'AM_MIN'        : '6000*MeV',
    'AM_MAX'        : '7200*MeV',
    'DZ1_MIN'       : '-1.5*mm',
    'B2CBBDT_MIN'   : -999.9
    },
    "Dstar" : { # Cuts made on all D*'s used in all lines 
    'ADOCA12_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '600*MeV', # was 50MeV
    'DELTAMASS_MAX' : '200*MeV',
    'DELTAMASS_MIN' : '90*MeV'
    },
    "HH": { # Cuts for rho, K*, phi, XHH Dalitz analyese, etc.
    'MASS_WINDOW'   : {'KST':'150*MeV','RHO':'150*MeV','PHI':'150*MeV'},
    'DAUGHTERS'     : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV'},
    'ADOCA12_MAX'  : '0.5*mm',
    'VCHI2DOF_MAX'  : 16,
    'BPVVDCHI2_MIN' : 16, 
    'BPVDIRA_MIN'   : 0,
    'ASUMPT_MIN'    : '1000*MeV',
    'pP_MIN'        : '5000*MeV' # for pH only (obviously)
    },
    "HHH": { # Cuts for PiPiPi, KPiPi analyese, etc.
    'MASS_WINDOW'   : {'A1':'3000*MeV','K1':'4000*MeV','PPH':'3600*MeV', 'PHH':'4000*MeV'},
    'KDAUGHTERS'    : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDK_MIN':'-2'},
    'PiDAUGHTERS'   : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDK_MAX':'10'},
    'pDAUGHTERS'    : {'PT_MIN':'100*MeV','P_MIN':'2000*MeV','PIDp_MIN':'-2'},
    'ADOCA12_MAX'   : '0.40*mm', 
    'ADOCA13_MAX'   : '0.40*mm',  
    'ADOCA23_MAX'   : '0.40*mm',    
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
    'TIGHTER1' : {    'P'  : {'PIDp_MIN' : 0},
                     'PI' : {'PIDK_MAX' : 10},
                     'K'  : {'PIDK_MIN' : -1}},                      
    'TIGHTER2' : {    'P'  : {'PIDp_MIN' : 5},
                      'PI' : {'PIDK_MAX' : 10},
                      'K'  : {'PIDK_MIN' : 0}},
    'SPECIAL' : {    'P'  : {'PIDp_MIN' : -5},
                     'PI' : {'PIDK_MAX' : 5},
                     'K'  : {'PIDK_MIN' : 5}},
    'SPECIALPI': {'P' : {'PIDp_MIN' : -10},
                  'PI': {'PIDK_MAX' : 12},
                  'K' : {'PIDK_MIN' : -10}}
    },
    'FlavourTagging':[

  'B02DstDD02K3PiBeauty2CharmLine',
 
    ],
    'RawEvents' : [
    
    'B02DsstarPiDsstar2DGammaD2HHHBeauty2CharmLine',
    'B02DsstarKDsstar2DGammaD2HHHBeauty2CharmLine'
    
    ],
    'MDSTChannels':[
   
  
    ],
    'RelatedInfoTools' : [
      { "Type" : "RelInfoConeVariables", 
        "ConeAngle" : 1.5, 
        "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
        "Location"  : 'P2ConeVar1'
      }, 
      { "Type" : "RelInfoConeVariables", 
        "ConeAngle" : 1.7, 
        "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
        "Location"  : 'P2ConeVar2'
      }, 
      { "Type" : "RelInfoConeVariables", 
        "ConeAngle" : 1.0, 
        "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
        "Location"  : 'P2ConeVar3'
      }, 
    ], 
    '2TOPO' : {'ANGLE_MIN': (2/57.),'M_MIN':19000,'DPHI_MIN':0},
    'BB' : {'ADDSUMPT':0,'COSANGLE_MAX':0.99,
            'COSDPHI_MAX':0,'M_MIN':0,'MAXPT_MIN': 4000,
            'TOPOCONESIZE':0.5,'TOPOCONEPT_MIN':10000,
            'TOPOCONETKCHI2NDOF_MAX':3.0,'TOPOCONETKGHOSTPROB_MAX':0.4,
            'TOPOCONETKIPCHI2_MAX':16.0,'TOPOCONETKDOCA_MAX':1.0},
    'D0INC' : {'PT_MIN' : 1000, 'IPCHI2_MIN': 100},
    "Prescales" : { # Prescales for individual lines
    'RUN_BY_DEFAULT' : True, # False = lines off by default
    'RUN_RE'         : ['.*'],  
    # Defaults are defined in, eg, Beauty2Charm_B2DXBuilder.py.  Put the full
    # line name here to override. E.g. 'B2D0HD2HHBeauty2CharmTOSLine':0.5.
    'B02DsstarPiDsstar2DGammaD2HHHBeauty2CharmLine' : 0.5,
    'B02DsstarKDsstar2DGammaD2HHHBeauty2CharmLine' : 0.
    },
    'GECNTrkMax'   : 500
  }, 
  'STREAMS' : { 
    'BhadronCompleteEvent' : [

    'StrippingB02DsstarPiDsstar2DGammaD2HHHBeauty2CharmLine',
    'StrippingB02DsstarKDsstar2DGammaD2HHHBeauty2CharmLine'


    
    ],  
    'Bhadron' : [


    #Basem's line
    'StrippingB02DstDD02K3PiBeauty2CharmLine',       
    
    #Snehas B2Dstar 
    'StrippingB2Dstar0KDst02D0Pi0D2HHHHMergedBeauty2CharmLine',
    'StrippingB2Dstar0PiDst02D0Pi0D2HHHHMergedBeauty2CharmLine',
    'StrippingB2Dstar0KDst02D0Pi0D2HHHHResolvedBeauty2CharmLine',  
    'StrippingB2Dstar0PiDst02D0Pi0D2HHHHResolvedBeauty2CharmLine',  
    'StrippingB2Dstar0KDst02D0GammaD2HHHHBeauty2CharmLine',  
    'StrippingB2Dstar0PiDst02D0GammaD2HHHHBeauty2CharmLine',  
    
    
    
    #Snehas B->dh
    
    'StrippingB2D0KD2HHHHBeauty2CharmLine', 
    'StrippingB2D0PiD2HHHHBeauty2CharmLine',  
    'StrippingB2D0KD2HHHHWSBeauty2CharmLine',  
    'StrippingB2D0PiD2HHHHWSBeauty2CharmLine', 
    'StrippingB2D0KsPiDDD2HHBeauty2CharmLine',  
    'StrippingB2D0KsPiLLD2HHBeauty2CharmLine',  
    'StrippingB2D0KsPiDDD2KSHHLLBeauty2CharmLine',  
    'StrippingB2D0KsPiLLD2KSHHLLBeauty2CharmLine',  
    'StrippingB2D0KsPiDDD2KSHHDDBeauty2CharmLine',  
    'StrippingB2D0KsPiLLD2KSHHDDBeauty2CharmLine',  
    'StrippingB2D0KsPiDDD2HHHHBeauty2CharmLine',  
    'StrippingB2D0KsPiLLD2HHHHBeauty2CharmLine',  
    'StrippingB2D0KsPiDDD2HHHHWSBeauty2CharmLine', 
    'StrippingB2D0KsPiLLD2HHHHWSBeauty2CharmLine',
    
    #Snehas WS lines
    
    'StrippingB2D0KsPiLLD2HHWSBeauty2CharmLine',
    'StrippingB2D0KsPiDDD2HHWSBeauty2CharmLine', 
    'StrippingB2D0KsPiLLD2KSHHLLWSBeauty2CharmLine',
    'StrippingB2D0KsPiLLD2KSHHDDWSBeauty2CharmLine',
    'StrippingB2D0KsPiDDD2KSHHLLWSBeauty2CharmLine',
    'StrippingB2D0KsPiDDD2KSHHDDWSBeauty2CharmLine', 
    
    #Oxford Bc lines
    'StrippingUpsilon2BBBeauty2CharmLine',
    
    'StrippingBc2BPiPiB2D0HD02HHBeauty2CharmLine', 
    'StrippingBc2BPiPiWSB2D0HD02HHBeauty2CharmLine', 
    'StrippingBc2BPiKB2D0HD02HHBeauty2CharmLine', 
    'StrippingBc2BPiKWSB2D0HD02HHBeauty2CharmLine', 
    'StrippingBc2BKPiB2D0HD02HHBeauty2CharmLine',
    'StrippingBc2BKKB2D0HD02HHBeauty2CharmLine',
    'StrippingBc2BKKWSB2D0HD02HHBeauty2CharmLine', 

    'StrippingBc2BPiPiB2D0HD02HHHHBeauty2CharmLine', 
    'StrippingBc2BPiPiWSB2D0HD02HHHHBeauty2CharmLine', 
    'StrippingBc2BPiKB2D0HD02HHHHBeauty2CharmLine', 
    'StrippingBc2BPiKWSB2D0HD02HHHHBeauty2CharmLine', 
    'StrippingBc2BKPiB2D0HD02HHHHBeauty2CharmLine', 
    'StrippingBc2BKKB2D0HD02HHHHBeauty2CharmLine', 
    'StrippingBc2BKKWSB2D0HD02HHHHBeauty2CharmLine', 

    'StrippingBc2BPiPiB2DHHD2HHHCFPIDBeauty2CharmLine', 
    'StrippingBc2BPiPiWSB2DHHD2HHHCFPIDBeauty2CharmLine', 
    'StrippingBc2BPiKB2DHHD2HHHCFPIDBeauty2CharmLine', 
    'StrippingBc2BPiKWSB2DHHD2HHHCFPIDBeauty2CharmLine', 
    'StrippingBc2BKPiB2DHHD2HHHCFPIDBeauty2CharmLine', 
    'StrippingBc2BKKB2DHHD2HHHCFPIDBeauty2CharmLine', 
    'StrippingBc2BKKWSB2DHHD2HHHCFPIDBeauty2CharmLine', 

    'StrippingBc2BPiPiB2D0HHHD02HHPIDBeauty2CharmLine', 
    'StrippingBc2BPiPiWSB2D0HHHD02HHPIDBeauty2CharmLine', 
    'StrippingBc2BPiKB2D0HHHD02HHPIDBeauty2CharmLine', 
    'StrippingBc2BPiKWSB2D0HHHD02HHPIDBeauty2CharmLine', 
    'StrippingBc2BKPiB2D0HHHD02HHPIDBeauty2CharmLine', 
    'StrippingBc2BKKB2D0HHHD02HHPIDBeauty2CharmLine', 
    'StrippingBc2BKKWSB2D0HHHD02HHPIDBeauty2CharmLine', 

    'StrippingBc2BPiPiB2D0DBeauty2CharmLine', 
    'StrippingBc2BPiPiWSB2D0DBeauty2CharmLine',
    'StrippingBc2BPiKB2D0DBeauty2CharmLine',
    'StrippingBc2BPiKWSB2D0DBeauty2CharmLine',
    'StrippingBc2BKPiB2D0DBeauty2CharmLine', 
    'StrippingBc2BKKB2D0DBeauty2CharmLine',
    'StrippingBc2BKKWSB2D0DBeauty2CharmLine',
    
    
    
    # Chris' Bc lines Bc+ -> D+ D0
    'StrippingBc2DD0D2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DD0D2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2KSHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DD0D2KSHD02KSHHBeauty2CharmLine',
    'StrippingBc2DD0D2HHHD02HHBeauty2CharmLine',
    'StrippingBc2DD0D2HHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DD0D2HHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DD0D2HHHD02KSHHBeauty2CharmLine',
    
    # Bc+ -> D*+ D0
    'StrippingBc2DstD0Dst2DPI0D2HHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2HHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2HHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2HHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2HHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2HHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2HHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DGammaD2HHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02HHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2D0PID02KSHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHHHD02KSHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02HHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02KHHHBeauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02HHPI0Beauty2CharmLine',
    'StrippingBc2DstD0Dst2DPI0D2KSHD02KSHHBeauty2CharmLine',

    # Bc+ -> D+ D*0
    'StrippingBc2DDst0D2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2KSHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DDst0D2HHHDst02D0GammaD02HHBeauty2CharmLine',

    # Bc+ -> D*+ D*0
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2HHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2HHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2HHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2HHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2HHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2HHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DGammaD2HHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02HHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2D0PID02KSHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHHHDst02D0GammaD02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0GammaD02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0GammaD02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0PI0D02HHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0PI0D02KHHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0PI0D02KSHHBeauty2CharmLine',
    'StrippingBc2DstDst0Dst2DPI0D2KSHDst02D0GammaD02HHBeauty2CharmLine',


  
]
  },
  'WGs' : [ 'B2OC' ]
}






B2DXD2HMuNu = {
    'WGs'         : ['B2OC'],
    'BUILDERTYPE' : 'B2DXD2HMuNuBuilder',
    'CONFIG'      :{
    "PT_MIN"          :  200,  #MeV
    "P_MIN"           :  3000, #MeV
    "MIPCHI2DV_MIN"   :  4.,
    "TRGHP_MAX"       :  0.4,
    "MuonPIDK"            : 0.    ,#adimensional
    "MuonPIDmu"           : 3.    ,#adimensional
    "MuonPIDp"            : 0.    ,#adimensional
    "KaonPIDK"            : 5.     ,#adimensional
    "KaonPIDmu"           : 5.     ,#adimensional
    "KaonPIDp"            : 5.     ,#adimensional
    "PionPIDK"            : 0.     ,#adimensional
    "PionPIDmu"           : 0.     ,#adimensional
    "PionPIDp"            : 0.     ,#adimensional
    "DMass_MAX"           : 1800, #MeV
    "Cos_MAX"             : 0.99999,
    "SUMPT_MIN"           : 1500, #MeV
    "ADOCA_MAX"           : 0.2, #mm
    "Plane_MAX"           : 0.02, #mm
    "VCHI2DOF_MAX"        : 10,
    "BPVVDCHI2_MIN"       : 36,
    "HH_MAX"              : 3800, #MeV
    "BMass_MAX"           : 5200, #MeV
    "GEC_nLongTrk"        : 160.   #adimensional
    },
    'STREAMS'     : {
    'B2OC': [
    'StrippingB2D0PiD02PiMuNuLine',
    'StrippingB2D0PiD02KMuNuLine',
    'StrippingB2D0PiD02KPiPiMuNuLine',
    'StrippingB2D0KD02PiMuNuLine',
    'StrippingB2D0KD02KMuNuLine',
    'StrippingB2D0KD02KPiPiMuNuLine',
    'StrippingB2D0KPiPiD02PiMuNuLine',
    'StrippingB2D0KPiPiD02KMuNuLine',
    'StrippingB2D0KPiPiD02KPiPiMuNuLine',
    'StrippingB02D0KPiD02PiMuNuLine',
    'StrippingB02D0KPiD02KMuNuLine',
    'StrippingB02D0KPiD02KPiPiMuNuLine',
    'StrippingB2D0PiD02PiMuNuSUPLine',
    'StrippingB2D0PiD02KMuNuSUPLine',
    'StrippingB2D0PiD02KPiPiMuNuSUPLine',
    'StrippingB2D0KD02PiMuNuSUPLine',
    'StrippingB2D0KD02KMuNuSUPLine',
    'StrippingB2D0KD02KPiPiMuNuSUPLine',
    'StrippingB2D0KPiPiD02PiMuNuSUPLine',
    'StrippingB2D0KPiPiD02KMuNuSUPLine',
    'StrippingB2D0KPiPiD02KPiPiMuNuSUPLine',
    'StrippingB02D0KPiD02PiMuNuSUPLine',
    'StrippingB02D0KPiD02KMuNuSUPLine',
    'StrippingB02D0KPiD02KPiPiMuNuSUPLine',
    'StrippingB2D0PiD02PiMuNuWSLine',
    'StrippingB2D0PiD02KMuNuWSLine',
    'StrippingB2D0PiD02KPiPiMuNuWSLine',
    'StrippingB2D0KD02PiMuNuWSLine',
    'StrippingB2D0KD02KMuNuWSLine',
    'StrippingB2D0KD02KPiPiMuNuWSLine',
    'StrippingB2D0KPiPiD02PiMuNuWSLine',
    'StrippingB2D0KPiPiD02KMuNuWSLine',
    'StrippingB2D0KPiPiD02KPiPiMuNuWSLine',
    'StrippingB02D0KPiD02PiMuNuWSLine',
    'StrippingB02D0KPiD02KMuNuWSLine',
    'StrippingB02D0KPiD02KPiPiMuNuWSLine',
    'StrippingB2D0PiD02PiMuNuWSSUPLine',
    'StrippingB2D0PiD02KMuNuWSSUPLine',
    'StrippingB2D0PiD02KPiPiMuNuWSSUPLine',
    'StrippingB2D0KD02PiMuNuWSSUPLine',
    'StrippingB2D0KD02KMuNuWSSUPLine',
    'StrippingB2D0KD02KPiPiMuNuWSSUPLine',
    'StrippingB2D0KPiPiD02PiMuNuWSSUPLine',
    'StrippingB2D0KPiPiD02KMuNuWSSUPLine',
    'StrippingB2D0KPiPiD02KPiPiMuNuWSSUPLine',
    'StrippingB02D0KPiD02PiMuNuWSSUPLine',
    'StrippingB02D0KPiD02KMuNuWSSUPLine',
    'StrippingB02D0KPiD02KPiPiMuNuWSSUPLine'
    ]
    }
    }


   
     
       
           
