#
# Rare Decay Lines
#
# DiMuon stream

from GaudiKernel.SystemOfUnits import *


####################################################################
#
# Lines for radiative decay sub-WG
#
####################################################################


Beauty2XGamma = {
    'BUILDERTYPE' : 'Beauty2XGamma',
    'CONFIG' : {

     "ALL" : {  'TRCHI2DOF_MAX' : 3,
                      'PT_MIN'        : '300*MeV',
                      'P_MIN'         : '1000*MeV',
                      'MIPCHI2DV_MIN' : 16 },
           # Cuts made on the photon
           "GAMMA" : { 'PT_MIN'       : '2500*MeV',
                       'CL_MIN'       : 0.25
                     },
           # Cuts made on the converted photon
           "GAMMACONV" : { 'MM_MAX'        : '50*MeV',
                           'VCHI2DOF_MAX'  : 9  ,
                        },
           # Cuts made on all K shorts
           "KS0" : { 'PT_MIN'        : '1000*MeV',
                     'BPVVDCHI2_MIN' : 81 ,
                     #'MIPCHI2DV_MIN' : 4,
                     'VCHI2DOF_MAX'  : 9,
                     'MM_MIN'        : '480.*MeV',
                     'MM_MAX'        : '515.*MeV' },
           # Cuts made on all pi0's
           "Pi0" : { 'PT_MIN'        : '1200*MeV',
                     'P_MIN'         : '4000*MeV',
                     'CHILDCL1_MIN'  : 0.25,
                     'CHILDCL2_MIN'  : 0.25 },
           # Cuts made on all B's and Lb's used in all lines
           "B2X" : { 'SUMPT_MIN'     : '5000*MeV',
                     'VCHI2DOF_MAX'  : 9,
                     'BPVIPCHI2_MAX' : 9,
                     #'BPVLTIME_MIN'  : '0.0*ps',
                     'BPVDIRA_MIN'   : 0.9998,
                     'AM_MIN'        : '4000*MeV',
                     'AM_MAX'        : '7000*MeV',
                     #'BBDT_MIN'      : 0.0, 
                     },
           # Cuts for rho, K*, phi, omega
           "HH": { 'MASS_WINDOW'      : {'KST':'150*MeV','RHO':'250*MeV','PHI':'15*MeV','OMEGA':'30*MeV'},
                   'DAUGHTERS'        : {'PT_MIN':'500*MeV','P_MIN':'3000*MeV'},
                   'piLAMBDADAUGHTERS': {'PT_MIN':'300*MeV','P_MIN':'3000*MeV','PIDK_MAX':'2'},   # only for pH
                   'pLAMBDADAUGHTERS' : {'PT_MIN':'1200*MeV','P_MIN':'10000*MeV','PIDp_MIN':'5'}, # only for pH
                   'kLAMBDADAUGHTERS' : {'PT_MIN':'300*MeV','P_MIN':'3000*MeV','PIDK_MIN':'0'},    # only for pH
                   #'AMAXDOCA_MAX'  : '0.5*mm',
                   'VCHI2DOF_MAX'     : 9,
                   'BPVVDCHI2_MIN'    : 81,
                   #'BPVDIRA_MIN'   : 0.0,
                   'ASUMPT_MIN'       : '1500*MeV',
                   #'pP_MIN'        : '10000*MeV' # for pH only (obviously)
                 },
           # Cuts for omega -> 3 body decay
           "HHH": { 'MASS_WINDOW'   : {'OMEGA': '150*MeV', 'K1': ['1.0*GeV', '1.7*GeV']},
                    'DAUGHTERS'     : {'PT_MIN':'350*MeV','P_MIN':'2000*MeV'},
                    'AMAXDOCA_MAX'  : '0.50*mm',
                    'VCHI2DOF_MAX'  : 9,
                    'BPVVDCHI2_MIN' : 64,
                    #'BPVDIRA_MIN'   : 0.0,
                    'ASUMPT_MIN'    : '1600*MeV',
                    'PT_MIN'        : '500*MeV',
                    'MIPCHI2DV_MIN' : 0.0,
                    #'BPVVDRHO_MIN'  : '0.1*mm',
                    #'BPVVDZ_MIN'    : '2.0*mm',
                    #'PTMIN1'       : '300*MeV'},
                 },
           # PID cuts
           "PID" : { 'P'  : {'PIDp_MIN' : 0},
                     'PI' : {'PIDK_MAX' : 20},
                     'K'  : {'PIDK_MIN' : -10} },
    "Prescales"  : {},
    "GECNTrkMax" : 500

     

     },
    'WGs'    : ['RD'],
    'STREAMS' : [ 'Radiative' ] 
    }




####################################################################
#
# Lines for rare EW penguin sub-group 
#
####################################################################

# Main line builer for B+ / Bd / Bs / Lambdab / Bc -> mu mu + X 
# M. Kreps, P. Owen, T. Blake

B2XMuMu =  {
    'BUILDERTYPE' : 'B2XMuMuConf',
    'CONFIG' : {
      'KpiVXCHI2NDOF'	   : 9.0           # dimensionless
    , 'MuonPID'            : -3.0           # dimensionless
    , 'DimuonUPPERMASS'    : 7100.0        # MeV
    , 'Pi0MINPT'           : 800.0         # MeV
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV
    , 'KstarplusWINDOW'    : 300.0         # MeV
    , 'KsWINDOW'           : 30.0          # MeV
    , 'LambdaWINDOW'	   : 30.0          # MeV
    , 'LongLivedPT'        : 0.0          # MeV , used to be 500.0 MeV
    , 'LongLivedTau'        : 2          # ps

    , 'K1_Comb_MassLow'  :  720.0
    , 'K1_Comb_MassHigh' : 2450.0
    , 'K1_MassLow'	 :  750.0
    , 'K1_MassHigh'	 : 2400.0
    , 'K1_MinIPCHI2'     :    4.0
    , 'K1_FlightChi2'    :   25.0
    , 'K1_Dau_MaxIPCHI2' : 9.0
    
    ,'UseNoPIDsHadrons'          : True,

    # B cuts
    'B_Comb_MassLow'	  : 4800.0,
    'B_Comb_MassHigh'     : 7100.0,
    'B_MassLow'           : 4900.0,
    'B_MassHigh'          : 7000.0,
    'B_VertexCHI2'        :    8.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :    0.9999,
    'B_FlightCHI2'        :  121.0,
   'B_Dau_MaxIPCHI2'     : 9.0,

    # Daughter cuts
    'Dau_VertexCHI2'	  :   12.0,
    'Dau_DIRA'            :   -0.9,

    # Kstar cuts
    'Kstar_Comb_MassLow'  :  0.0,
    'Kstar_Comb_MassHigh' : 6200.0,
    'Kstar_MassLow'	  :  0.0,
    'Kstar_MassHigh'	  : 6200.0,
    'Kstar_MinIPCHI2'     :    0.0,
    'Kstar_FlightChi2'    :    9.0,
    'Kstar_Dau_MaxIPCHI2' : 9.0,

    # JPsi (dimu) cuts
    'Dimu_FlightChi2'     :   9.0,
    'Dimu_Dau_MaxIPCHI2'  :   9.0,

    # Track cuts
    'Track_CHI2nDOF'	  :    5.0,

    # Hadron cuts
    'Hadron_MinIPCHI2'    :    9.0,

    # Muon cuts
    'Muon_MinIPCHI2'	  :    9.0,
    'Muon_IsMuon'         :    True,
    'MuonNoPIDs_PIDmu'    :    0.0,

    # Wrong sign combinations
    'DimuonWS'            :   True,
    'HadronWS'            :   True,

    # GEC
    'SpdMult'             :  600
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }

# Bu2LLK ( B+ -> ee K and mu mu K )
# P. Koppenburg

Bu2LLK = {
    'BUILDERTYPE'         : 'Bu2LLKConf',
    'CONFIG'              : {
    'BFlightCHI2'         : 100        
    ,  'BDIRA'            : 0.9995     
    ,  'BIPCHI2'             : 25        
    ,  'BVertexCHI2'         : 9        
    ,  'DiLeptonPT'          : 0         # MeV (not used)
    ,  'DiLeptonFDCHI2'      : 16       
    ,  'DiLeptonIPCHI2'      : 0         
    ,  'LeptonIPCHI2'        : 16        
    ,  'LeptonPT'            : 500       # MeV
    ,  'KaonIPCHI2'          : 9        
    ,  'KaonPT'              : 800       
    ,  'UpperMass'           : 5500      # MeV
    ,  'Bu2eeKLinePrescale'  : 1
    ,  'Bu2mmKLinePrescale'  : 1
    ,  'Bu2meKLinePrescale'  : 1
     },
    'WGs'    : [ 'RD' ] ,
    'STREAMS' : ['Leptonic']
    }

# B+ -> K1 mu mu
# E. Gersabeck

BuToK1MuMu = {
    'BUILDERTYPE' : 'StrippingBuToK1MuMuConf' ,
    'STREAMS'     : [ 'Dimuon' ],
    'WGs'         : [ 'RD' ],
    'CONFIG'      : {
    'UseNoPIDsHadrons'          : True,
    'Prescale_BuToK1MuMu'    : 1.0,
    'Postscale_BuToK1MuMu'   : 1.0,
    'Prescale_BuToK1MuMuSS'  : 1.0,
    'Postscale_BuToK1MuMuSS' : 1.0,
    'B_Comb_MassLow'      : 4600.0,
    'B_Comb_MassHigh'     : 6000.0,
    'B_MassLow'           : 4850.0,
    'B_MassHigh'          : 5780.0,
    'B_VertexCHI2'        :    6.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :    0.014,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0,
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    'K1_Comb_MassLow'  :  720.0,
    'K1_Comb_MassHigh' : 2450.0,
    'K1_MassLow'       :  750.0,
    'K1_MassHigh'      : 2400.0,
    'K1_MinIPCHI2'     :    4.0,
    'K1_FlightChi2'    :   25.0,
    'K1_Dau_MaxIPCHI2' : 9.0,
    'Dimu_FlightChi2'     :   81.0,
    'Dimu_Dau_MaxIPCHI2'  : 9.0,
    'Track_CHI2nDOF'      :    2.5,
    'Hadron_MinIPCHI2'    :    16.0,
    'Hadron_PT'           :    500,
    'K_PIDK_min'          :      0,
    'pi_PIDK_max'        :      5,
    'Muon_MinIPCHI2'      :    16.0,
    'Muon_IsMuon'         :   False,
    'MuonNoPIDs_PIDmu'    :    0.0
    }
    }



### Lines for Bd2eeKstar analysis and control channels with Bd2mumuKstar

Bd2eeKstarBDT = {
    'BUILDERTYPE'             : 'Bd2eeKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeCuts'                  : "(VFASPF(VCHI2)<16) & (((MM>20*MeV) & (MM<1.5*GeV)) | ((MM>2.2*GeV) & (MM<4.2*GeV)))",
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  150.   ,  # MeV
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'
        },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }

Bd2eeKstar = {
    'BUILDERTYPE'             : 'Bd2eeKstarConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  300.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    2.25 ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 1500.   ,  # MeV
        'eeFD'                    :    1.   ,  # mm
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  300.   ,  # MeV
        'PionP'                   : 3000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  130.   ,  # MeV
        'KstarIPCHI2'             :    1.   , 
        'KstarFDCHI2'             :    1.   , 
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :    9.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BIPCHI2'                 :   64.   ,  # pointing
        'BFDCHI2'                 :    9.   , 
        'BDIRA'                   :    0.999, 
        'SumIPSCut'               : " & (SUMTREE(((ABSID=='K+') | (ABSID=='pi-') | (ID=='e+') | (ID=='e-')),sqrt(BPVIPCHI2()))>15)" 
           },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }

Bd2MuMuKstarBDT = {
    'BUILDERTYPE'             : 'Bd2MuMuKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -5.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 5200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK>-10, i.e., PIDK<10 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  150.   ,  # MeV
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.98 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Leptonic' ]
    }

Bd2MuMuKPiBDT = {
    'BUILDERTYPE'             : 'Bd2MuMuKstarBDTConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  200.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    1.   ,
        'ElectronPIDepi'          :   -5.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 5200.   ,  # MeV
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  250.   ,  # MeV
        'PionP'                   : 2000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK>-10, i.e., PIDK<10 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  1400.  ,  # MeV
        #
        'BComMassW'               :  550.   ,  # MeV
        'BVertexCHI2'             :   16.   ,  # /ndf
        'BMassW'                  :  500.   ,  # MeV  
        'BDIRA'                   :    0.999,
        'BDTCutValue'             :   -0.97 ,
        'BDTWeightsFile'          : '$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml'
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Leptonic' ]
    }


Bd2JpsieeKstar = {
    'BUILDERTYPE'             : 'Bd2eeKstarConf',
    'CONFIG'                  : {
        'LinePrescale'            :    1.   ,
        'LinePostscale'           :    1.   ,
        #
        'ElectronPT'              :  300.   ,  # MeV
        'ElectronTrackCHI2pNDOF'  :    5.   ,
        'ElectronIPCHI2'          :    2.25 ,
        'ElectronPIDepi'          :   -2.   ,          
        #
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV
        'eeFD'                    :    1.   ,  # mm
        #
        'KaonPT'                  :  400.   ,  # MeV 
        'KaonP'                   : 3000.   ,  # MeV  
        'KaonTrackCHI2pNDOF'      :    5.   , 
        'KaonIPCHI2'              :    4.   , 
        'KaonPIDKpi'              :   -5.   , 
        #
        'PionPT'                  :  300.   ,  # MeV
        'PionP'                   : 3000.   ,  # MeV 
        'PionTrackCHI2pNDOF'      :    5.   , 
        'PionIPCHI2'              :    4.   , 
        'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
        #
        'KstarVertexCHI2'         :   16.   , 
        'KstarMassW'              :  130.   ,  # MeV
        'KstarIPCHI2'             :    1.   , 
        'KstarFDCHI2'             :    1.   , 
        #
        'BComMassW'               : 1200.   ,  # MeV
        'BVertexCHI2'             :    9.   ,  # /ndf
        'BMassW'                  : 1000.   ,  # MeV  
        'BIPCHI2'                 :   64.   ,  # pointing
        'BFDCHI2'                 :    9.   , 
        'BDIRA'                   :    0.999, 
        'SumIPSCut'               : " & (SUMTREE(((ABSID=='K+') | (ABSID=='pi-') | (ID=='e+') | (ID=='e-')),sqrt(BPVIPCHI2()))>15)" 
           },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Radiative' ]
    }


####################################################################
#
# Lines for very rare decay sub-group 
#
####################################################################


# C. Elasser 
# Z -> tau tau 

Z02TauTauProng = {
    'BUILDERTYPE'  : 'Z02TauTauProngConf',
    'CONFIG'       : {
    'Z2TauTau_Prong_LinePrescale'  : 1.0,
    'Z2TauTau_Prong_LinePostscale' : 1.0,
    'TAU_MASS_LOW'                 : '600.0',    # MeV/c2
    'TAU_MASS_HIGH'                : '1600.0',   # MeV/c2
    'Z_MASS_LOW'                   : '45000.0',  # MeV/c2
    'PT_HAD_MIN'                   : '1500',     # MeV/c
    'TRACKCHI2_HAD_MAX'            : '10',       # dl
    'PT_TAU_MIN'                   : '8000',    # MeV/c
    'VCHI2_TAU_MAX'                : '20'        # dl
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'EW' ]
    }


# J. Albrecht + P. Seyfert 
#
Tau23Mu = {
    'BUILDERTYPE' : 'Tau23MuLinesConf',
    'STREAMS'     : ['Dimuon'],
    'WGs'         : ['RD'],
    'CONFIG'      : {
    'TauPrescale'         :1.,
    'TauPostscale'        :1.,
    'Ds23PiTISPrescale'   :0.0,
    'Ds23PiPrescale'      :0.0,
    'Ds2PhiPiPrescale'    :1.,
    'Tau25Prescale'       :1.,
    'Tau2PMuMuPrescale'   :1.
    }
    }

# J. Albrecht, M. Palutan
#

Bs2MuMuLines = {
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG'      : {
    
    'DefaultLinePrescale'    : 1,
    'DefaultPostscale'   : 1,
    'Bs2mmWideLinePrescale'  : 1,
    'LooseLinePrescale'      : 0.,
    'BuPrescale'    : 1,
    'BsPrescale'    : 1,
    'BdPrescale'    : 1,
    'JPsiLinePrescale'       : 1,
    'JPsiLooseLinePrescale'  : 0.1,
    'JPsiPromptLinePrescale' : 0.005,
    'SSPrescale'             : 1 ,
    
    'MuIPChi2_loose'        :  9,
    'MuTrChi2_loose'        : 10,
    'BIPChi2_loose'         : 64,
    'BFDChi2_loose'         : 100
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


# J. Albrecht, M. de Cian 
#
VeryDetachedJpsi = {
    'BUILDERTYPE'  : 'VDetJPsiLinesConf',
    'STREAMS'      : ['Dimuon'],
    'WGs'          : ['RD'],
    'CONFIG'       : {
    'VDetJPsiLinePrescale'    : 1,
    'VDetJPsiLinePostscale'   : 1,
    }
    }


# B2MuMuMuMuLines
# J. Albrecht, I. Sepp, K. Petridis

B2MuMuMuMuLines = {
    'BUILDERTYPE' : 'B2MuMuMuMuLinesConf',
    'CONFIG'      : {
     'B2MuMuMuMuLinePrescale'    : 1,
        'B2MuMuMuMuLinePostscale'   : 1,
        'D2MuMuMuMuLinePrescale'    : 1,
        'D2MuMuMuMuLinePostscale'   : 1,
        'B2TwoDetachedDimuonLinePrescale'  : 1,
        'B2TwoDetachedDimuonLinePostscale' : 1,
        'DetachedDiMuons': {
            'AMAXDOCA_MAX'  : '0.5*mm',
            'ASUMPT_MIN'    : '1000*MeV',
            'VCHI2DOF_MAX'  : 16,
            'BPVVDCHI2_MIN' : 16,
            },
        'B2DetachedDiMuons': {
            'SUMPT_MIN'        : '2000*MeV',
            'VCHI2DOF_MAX'     : 6,
            'BPVIPCHI2_MAX'    : 16,
            'BPVVDCHI2_MIN'    : 50,
            'BPVDIRA_MIN'      : 0.0,
            'MASS_MIN'         : {'B':'4600*MeV'},
            'MASS_MAX'         : {'B':'6000*MeV'}
            }
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


# TriMuons, including Bc
# V. Belyaev 

TriMuon = {
    'BUILDERTYPE'  : 'StrippingTriMuonsConf',
    'CONFIG'       :  {

    'GoodMuons'        : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 2  ) & ( BPVIPCHI2 () >  9 ) " ,
    'GoodMuonsForBc' : " ( BPVIPCHI2 () >  9 ) & ( TRCHI2DOF < 2  )" ,
    'TightMuons'   : " ( PT > 1.9 * GeV ) & ( BPVIPCHI2 () > 25 ) " ,
    #
    # Trigger 
    #
    'HLT'    : None ,  
    #
    # Prescale 
    #
    '3mu-Prescale' : 1.00 , 
    'Bc-Prescale'  : 1.00 , 
    'Tau-Prescale' : 1.00 , 
    #
    # Technicalities:
    #
    'Preambulo'    : [
    ## shortcut for chi2 of vertex fit 
    'chi2vx = VFASPF(VCHI2) '                               ,
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light"     ,
    "ctau    =                    BPVLTIME ( ) * c_light "  ,
    "ctauBc  = PDGM('B_c+') / M * BPVLTIME ( ) * c_light "  
    ]
    
    },
    'WGs'     : [ 'RD' ] ,
    'STREAMS' : [ 'Dimuon' ]
    }



# HyperCP lines
# V. Belyaew + A. Golutvin + I. Sepp 
HyperCP = {
    'BUILDERTYPE' : 'StrippingHyperCPXConf',
    'CONFIG' : {
    'SigmaPrescale'  : 1.00 ,
    'DsPrescale'     : 1.00 ,
    'Ds3PiPrescale'  : 1 ,
    'DplusPrescale'  : 1.00 ,
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


LFVLines = {
    'BUILDERTYPE' : 'LFVLinesConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ],
    'CONFIG'  : {
    'Postscale'           :1,
    'TauPrescale'         :1,
    'Tau2MuMuePrescale'         :1,
    'B2eMuPrescale'       :1,
    'B2eePrescale'        :1,
    'B2heMuPrescale'      :1,
    'B2pMuPrescale'       :1,
    'Bu2KJPsieePrescale'  :1,
    'B2TauMuPrescale'       :1,
    'B2hTauMuPrescale'      :1,
    }
    }


# J. Albrecht, K. Petridis + M. de Cian 
#
Inflaton2MuMu = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'Inflaton2MuMuLongPrescale'    : 0,
    'Inflaton2MuMuDownstreamPrescale'    : 1,
    'Inflaton2MuMuLongPostscale'    : 1,
    'Inflaton2MuMuDownstreamPostscale'    : 1,
    'Bu2InflatonKPrescale' : 1, 
    'Bu2InflatonKPostscale' : 1, 
    'Bs2InflatonPhiPrescale' : 1, 
    'Bs2InflatonPhiPostscale' : 1, 
    'Bd2InflatonKstPrescale' : 1, 
    'Bd2InflatonKstPostscale' : 1,
    'Bd2InflatonRhoPrescale' : 1, 
    'Bd2InflatonRhoPostscale' : 1
    }
    }

# J. Albrecht, K. Petridis + M. de Cian 
# Lines for micro-DST
Inflaton2MuMuForMicro = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : [ 'Leptonic' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'Inflaton2MuMuLongPrescale'    : 1,
    'Inflaton2MuMuDownstreamPrescale'    : 0,
    'Inflaton2MuMuLongPostscale'    : 1,
    'Inflaton2MuMuDownstreamPostscale'    : 0,
    'Bu2InflatonKPrescale' : 0, 
    'Bu2InflatonKPostscale' : 0, 
    'Bs2InflatonPhiPrescale' : 0, 
    'Bs2InflatonPhiPostscale' : 0, 
    'Bd2InflatonKstPrescale' : 0, 
    'Bd2InflatonKstPostscale' : 0,
    'Bd2InflatonRhoPrescale' : 0, 
    'Bd2InflatonRhoPostscale' : 0
    }
    }


# Diego + Xabier
# KS -> mu mu 
KS02MuMu = {
    'BUILDERTYPE' : 'K0s2MuMuLinesConf',
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Leptonic' ],
    'CONFIG' : {
    'NoMuIDLinePrescale'    : 1e-03,
    'NoMuIDLinePostscale'   : 1,
    'K0s2mmLinePrescale'  : 1,
    'K0s2mmLinePostscale'  : 1
    }
    }


# B -> X tau, including B -> tau tau
# C. Elsasser 

B2XTau =  {
    'BUILDERTYPE' : 'B2XTauConf',
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Bhadron' ],
    'CONFIG' : {
    'PT_HAD_ALL_FINAL_STATE'        : '200',  # MeV
    'PTMAX_HAD_ALL_FINAL_STATE'     : '500',  # MeV
    'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
    'IPCHI2_HAD_ALL_FINAL_STATE'    : '9',    # dimensionless
    'TRACKCHI2_HAD_ALL_FINAL_STATE' : '4',    # dimensionless
    'PID_HAD_ALL_FINAL_STATE'       : '5',    # dimensionless
    'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.35', # dimensionless
    #
    'PT_MU'                         : '1200', # MeV
    'P_MU'                          : '6000', # MeV 
    'IPCHI2_MU'                     : '16',   # MeV
    'TRACKCHI2_MU'                  : '4',    # dimensionless
    'TRGHOPROB_MU'                  : '0.35', # dimensionless
    #
    'PT_B_TT'                       : '5000', # MeV
    'PT_B_TT_HIGH'                  : '10000',# MeV 
    'PT_B_TM'                       : '2000', # MeV
    'PT_B_TM_HIGH'                  : '7500', # MeV  
    'VCHI2_B'                       : '100',  # dimensionless
    'FDCHI2_B'                      : '144',  # dimensionless
    'DIRA_B'                        : '0.99', # dimensionless
    'MASS_LOW_B'                    : '2000', # MeV  
    'MASS_HIGH_B'                   : '7000', # MeV (old value: 5750)
    'MCOR_LOW_B'                    : '4000', # MeV
    'MCOR_HIGH_B'                   :'10000', # MeV (old value: 7000)
    'MIPCHI2_B'                     : '150',  # dimensionless 
    'MIPCHI2_B_HIGH'                : '36',   # dimensionless  
    #
    'PT_TAU'                        : '1250', # MeV
    'VCHI2_TAU'                     : '12',   # dimensionless
    'IPCHI2_TAU'                    : '9',    # dimensionless
    'FDCHI2_TAU'                    : '16',   # dimensionless
    'FDRHO_TAU'                     : '0.1',  # mm
    'FDZ_TAU'                       : '2.0',  # mm
    'DOCA_TAU'                      : '0.4',  # mm
    'DIRA_TAU'                      : '0.98', # dimensionless
    'MASS_LOW_TAU'                  : '500',  # MeV
    'MASS_HIGH_TAU'                 : '1800', # MeV
    #
    'PT_DIPI'                       : '200',  # MeV
    'VCHI2_DIPI'                    : '20',   # dimensionless
    'IPCHI2_DIPI'                   : '1',    # dimensionless
    'FDCHI2_DIPI'                   : '9',    # dimensionless
    'FDRHO_DIPI'                    : '0.05', # mm
    'FDZ_DIPI'                      : '1.0',  # mm
    'DOCA_DIPI'                     : '0.4',  # mm
    'DIRA_DIPI'                     : '0.9',  # dimensionless
    #
    'PT_B_CHILD_BEST'               : '1800', # MeV
    'P_B_CHILD_BEST'                : '10000',# MeV
    'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
    'PT_B_TAU_CHILD_BEST'           : '3000', # MeV
    'IPCHI2_B_TAU_CHILD_BEST'       : '16',   # dimensionless
    #
    'MASS_LOW_D'                    : '1800', # MeV
    'MASS_HIGH_D'                   : '2030', # MeV  
    #
    'B2TauTau_TOSLinePrescale'          : 1,
    'B2TauTau_TOSLinePostscale'         : 1,
    'B2DD_TOSLinePrescale'              : 1,
    'B2DD_TOSLinePostscale'             : 1,
    'B2TauMu_TOSLinePrescale'           : 1,
    'B2TauMu_TOSLinePostscale'          : 1,
    'B2DMu_TOSLinePrescale'             : 0.2,
    'B2DMu_TOSLinePostscale'            : 1,
    'B2TauTau_TISLinePrescale'          : 1,
    'B2TauTau_TISLinePostscale'         : 1,
    'B2DD_TISLinePrescale'              : 1,
    'B2DD_TISLinePostscale'             : 1,
    'B2TauMu_TISLinePrescale'           : 1,
    'B2TauMu_TISLinePostscale'          : 1,
    'B2DMu_TISLinePrescale'             : 0.2,
    'B2DMu_TISLinePostscale'            : 1.
    }
    }
