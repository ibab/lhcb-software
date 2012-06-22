#
# Rare Decay Lines
#
# DiMuon stream

from GaudiKernel.SystemOfUnits import *

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
    'BFlightCHI2'         : 100       # adimentional 
    ,  'BDIRA'               : 0.9995     # adimentional    TIGHTENED
    ,  'BIPCHI2'             : 25        # adimentional  
    ,  'BVertexCHI2'         : 16        # adimentional
    ,  'DiLeptonPT'          : 0         # MeV (not used)
    ,  'DiLeptonFDCHI2'      : 16        # adimentional
    ,  'DiLeptonIPCHI2'      : 9         # adimentional
    ,  'LeptonIPCHI2'        : 16        # adimentional      TIGHTENED
    ,  'LeptonPT'            : 800       # MeV               
    ,  'KaonIPCHI2'          : 16        # adimentional      TIGHTENED
    ,  'KaonPT'              : 800       # MeV               LOOSENED
    ,  'UpperMass'           : 5500      # MeV (Higher bound of signal box)
    ,  'Bu2eeKLinePrescale'  : 1
    ,  'Bu2eeKLinePostscale' : 1
    ,  'Bu2mmKLinePrescale'  : 1
    ,  'Bu2mmKLinePostscale' : 1
     },
    'WGs'    : [ 'RD' ] ,
    'STREAMS' : ['Dimuon']
    #{ 'Dimuon' : [ 'StrippingBu2LLK_mmLine' ] , 'Dielectron' : [ 'StrippingBu2LLK_eeLine' ]  }
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
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               :   20.   ,  # MeV 
        'eeMaxMass'               : 1500.   ,  # MeV
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
    'STREAMS' : [ 'Dimuon' ]
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
    'STREAMS' : [ 'Dimuon' ]
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


Bd2JpsieeKstarBDT = {
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
        'eeVertexCHI2'            :   16.   ,  
        'eeMinMass'               : 2200.   ,  # MeV 
        'eeMaxMass'               : 4200.   ,  # MeV
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








####################################################################
#
# Lines for very rare decay sub-group 
#
####################################################################



# Same-sign searches
# S. Redford & Wenbin

#B2XMuMuSS = {
#    'BUILDERTYPE'   : 'B2XMuMuSSConf',
#    'CONFIG'        : {
#    'MuonP'         : 3000. ,    #MeV
#    'MuonPT'        : 500.  ,    #MeV
#    'MuonMINIPCHI2' : 5     ,    #adminensional
#    'PionP'         : 2000. ,    #MeV
#    'PionPT'        : 500.  ,    #MeV
#    'PionMINIPCHI2' : 5     ,    #adminensional
#    'KaonP'         : 2000. ,    #MeV
#    'KaonPT'        : 500.  ,    #MeV
#    'KaonMINIPCHI2' : 5     ,    #adminensional
#    'DimuonMass'    : 0.  ,    #MeV
#    'BVCHI2DOF'     : 7    ,    #adminensional                              
#    'BDIRA'         : 0.9998 ,    #adimensional
#    'BIPCHI2'       : 30    ,    #adimensional
#    'BMassWin'      : 400.  ,    #MeV, mass window
#    'B2PiMuMuOSLinePrescale'  : 1 ,
#    'B2PiMuMuOSLinePostscale' : 1 ,
#    'B2PiMuMuSSLinePrescale'  : 1 ,
#    'B2PiMuMuSSLinePostscale' : 1 ,
#    'B2KMuMuOSLinePrescale'   : 1 ,
#    'B2KMuMuOSLinePostscale'  : 1 ,
#    'B2KMuMuSSLinePrescale'   : 1 ,
#    'B2KMuMuSSLinePostscale'  : 1
#    },
#    'WGs' : [ 'RD' ] ,
#    'STREAMS' : [ 'Dimuon' ]
#    }

# HyperCP lines
# Vanya + Andrei

HyperCP = {
    'BUILDERTYPE' : 'StrippingHyperCPXConf',
    'CONFIG' : {
    'ProtonCuts' : ' ( TRCHI2DOF < 5 ) & ( 0 < PIDp  - PIDpi ) & ( BPVIPCHI2() > 12 ) ' ,
    'MuonCuts'   : ' ( TRCHI2DOF < 5 ) & ISMUON                & ( BPVIPCHI2() > 12 ) ' ,                
    'PionCuts'   : ' ( TRCHI2DOF < 5 )                         & ( BPVIPCHI2() > 12 ) ' ,
    'MuonCuts_forTau23Mu'   : ' ( PT > 300 * MeV ) & ( TRCHI2DOF < 5 ) & ISMUON                & ( BPVIPCHI2() > 9 ) ' ,                
    'PionCuts_forTau23Mu'   : ' ( PT > 300 * MeV ) & ( TRCHI2DOF < 5 )                         & ( BPVIPCHI2() > 9 ) ' ,
    #
    'SigmaCTau'  :   5 *         mm ,
    'SigmaMass'  : 250 *        MeV ,
    #
    'DsCTau'     : 200 * micrometer ,
    'Ds23PiMass'     : 80 *        MeV ,
    'Ds2PhiPiMass'     : 250 *        MeV,
    #
    'DplusCTau'  : 200 * micrometer ,
    'DplusMass'  : 250 *        MeV ,
    #
    # ``Global Event Cuts''
    #
    'PrimaryVertices' : True ,
    #
    # Technicalities:
    #
    'Preambulo'       : [
    # shortcut for chi2 of vertex fit
    'chi2vx = VFASPF(VCHI2) '                    ,
    # shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    ## use the embedded cut for chi2(LifetimeFit)<9
    "ctau   = BPVLTIME ( 9 ) * c_light "  ,
    "ctau_forDs   = BPVLTIME ( 225 ) * c_light "  ,
    ## phi(1020) mass-window
    "phi    = in_range ( 920 * MeV , AM23 , 1120 * MeV )"
    ] ,
    #
    # Prescales
    #
    'SigmaPrescale' : 1.0 ,
    'DplusPrescale' : 1.0 ,
    'DsPrescale'    : 1.0 ,
    'Ds3PiPrescale' : 0.2
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


# B2MuMuMuMuLines
# J. Albrecht

B2MuMuMuMuLines = {
    'BUILDERTYPE' : 'B2MuMuMuMuLinesConf',
    'CONFIG'      : {
    'B2MuMuMuMuLinePrescale'    : 1,
    'B2MuMuMuMuLinePostscale'   : 1,
    'D2MuMuMuMuLinePrescale'    : 1,
    'D2MuMuMuMuLinePostscale'   : 1,
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


# Stripping TriMuons
# Vanya

TriMuon = {
    'BUILDERTYPE'  : 'StrippingTriMuonsConf',
    'CONFIG'       :  {
    'GoodMuons'        : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5  ) & ( BPVIPCHI2 () >  6 ) " ,
    'GoodMuonsForBc' : " ( BPVIPCHI2 () >  9 ) " ,
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



Bs2MuMuLines = {
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG'      : {

    'DefaultLinePrescale'    : 1,
    'DefaultLinePostscale'   : 1,
    'Bs2mmWideLinePrescale'  : 1,
    'Bs2mmWideLinePostscale' : 1,
    'LooseLinePrescale'      : 0.0,
    'LooseLinePostscale'     : 1,
    'BuPrescale'    : 1,
    'BuPostscale'   : 1,
    'BsPrescale'    : 1,
    'BsPostscale'   : 1,
    'BdPrescale'    : 1,
    'BdPostscale'   : 1,
    'JPsiLinePrescale'       : 1,
    'JPsiLinePostscale'      : 1,
    'JPsiLooseLinePrescale'  : 0.1,
    'JPsiLooseLinePostscale' : 1,
    'JPsiPromptLinePrescale' : 0.005,
    'JPsiPromptLinePostscale': 1,
    'MuIPChi2_loose'        :  9,
    'MuTrChi2_loose'        : 10,
    'BIPChi2_loose'         : 64,
    'BFDChi2_loose'         : 100
    
    },
    'WGs'    : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }

# Diego + Xabier

KS02MuMu = {
    'BUILDERTYPE' : 'K0s2MuMuLinesConf',
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ],
    'CONFIG' : {
    'NoMuIDLinePrescale'    : 1e-03,
    'NoMuIDLinePostscale'   : 1,
    'K0s2mmLinePrescale'  : 1,
    'K0s2mmLinePostscale'  : 1
    }
    }

Tau2PMuMu = {
    'BUILDERTYPE' : 'StrippingTau2PMuMuConf',
    'STREAMS'     : ['Dimuon'],
    'WGs' : [ 'RD' ] ,
    'CONFIG' : {
    #
    # Selection of basic muons and protons
    #
    'GoodMuons' : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3 ) & ( PIDmu > -5 ) & ( (PIDmu - PIDK) > 0 )" ,
    'GoodProtons' : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 3 )" ,
    #
    # Prescale
    #
    'pmumu-Prescale' : 1.00 ,
    #
    # Technicalities:
    #
    'Preambulo' : [
    ## shortcut for chi2 of vertex fit
    'chi2vx = VFASPF(VCHI2) ' ,
    ## shortcut for the c*tau
    "from GaudiKernel.PhysicalConstants import c_light" ,
    "ctau = BPVLTIME ( ) * c_light "  
    ]
    #
    }
    }

Tau23Mu = {
    'BUILDERTYPE' : 'Tau23MuLinesConf',
    'STREAMS'     : ['Dimuon'],
    'WGs'         : ['RD'],
    'CONFIG'      : {
    'TauPrescale'         :1,
    'TauPostscale'        :1,
    'Ds23PiTISPrescale'   :0.0,
    'Ds23PiTISPostscale'  :0.0,
    'Ds23PiPrescale'      :0.0,
    'Ds23PiPostscale'     :0.0,
    'Ds2PhiPiPrescale'    :1,
    'Ds2PhiPiPostscale'   :1,        
    'Tau25Prescale'       :1,
    'Tau25Postscale'      :1      
    }
    }


LFVLines = {
    'BUILDERTYPE' : 'LFVLinesConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ],
    'CONFIG'  : {
    'Postscale'           :1,
    'TauPrescale'         :1,
    'Tau2MuMuePrescale'   :1,
    'B2eMuPrescale'       :1,
    'B2eePrescale'        :1,
    'B2heMuPrescale'      :1,
    'B2pMuPrescale'       :1,
    }
    }

# J. Albrecht
# Searches for highly displaced dimuons
VeryDetachedJpsi = {
    'BUILDERTYPE'  : 'VDetJPsiLinesConf',
    'STREAMS'      : ['Dimuon'],
    'WGs'          : ['RD'],
    'CONFIG'       : {
    'VDetJPsiLinePrescale'    : 1,
    'VDetJPsiLinePostscale'   : 1,
    }
    }


Inflaton2MuMu = {
    'BUILDERTYPE' : 'StrippingInflaton2MuMuConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG'  : {
    'Inflaton2MuMuLongPrescale'    : 1,
    'Inflaton2MuMuDownstreamPrescale'    : 1,
    'Inflaton2MuMuLongPostscale'    : 1,
    'Inflaton2MuMuDownstreamPostscale'    : 1,
    'Bu2InflatonKPrescale' : 1,
    'Bu2InflatonKPostscale' : 1,
    'Bs2InflatonPhiPrescale' : 1,
    'Bs2InflatonPhiPostscale' : 1,
    'Bd2InflatonKstPrescale' : 1,
    'Bd2InflatonKstPostscale' : 1
    }
    }



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

# includes Bs -> mu mu gamma

B2MuMuX = {
    'BUILDERTYPE' : 'B2MuMuXConf' ,
    'STREAMS' : [ 'Dimuon' ],
    'WGs'     : [ 'RD' ] ,
    'CONFIG' : { 
    'MuonsLoose': {
        'ISMUON' : True,
        'TRCHI2DOF_MAX' : 5,
        'MIPCHI2DV_MIN' : 4,
        'InAccMuon'  : True
        },
    'MuonsNoPid': {
        'ISMUON' : False,
        'TRCHI2DOF_MAX' : 5,
        'MIPCHI2DV_MIN' : 4,
        'InAccMuon'  : True
        },
    'DiMuons': {
        'AMAXDOCA_MAX'  : '0.5*mm',
        'ASUMPT_MIN'    : '1000*MeV',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 16,
        'BPVDIRA_MIN'   : 0.0
        },
    'DiMuonsLowM': {
        'AMAXDOCA_MAX'  : '0.5*mm',
        'ASUMPT_MIN'    : '1000*MeV',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 16,
        'BPVDIRA_MIN'   : 0.0
        },
    'DiMuonsHighM': {
        'AMAXDOCA_MAX'  : '0.5*mm',
        'ASUMPT_MIN'    : '1000*MeV',
        'VCHI2DOF_MAX'  : 16,
        'BPVVDCHI2_MIN' : 0,
        'BPVDIRA_MIN'   : 0
        },
    'DiMuonsCorrM': {
        'AMAXDOCA_MAX'  : '0.3*mm',
        'ASUMPT_MIN'    : '2000*MeV',
        'VCHI2DOF_MAX'  : 8,
        'BPVVDCHI2_MIN' : 225,
        'BPVIPCHI2_MAX' : 17,
        'BPVDIRA_MIN'   : 0.4,
        'CORRM_MIN'     : '4800*MeV',
        'CORRM_MAX'     : '6500*MeV',
        'PTMU'          : '900*MeV'
        },
    'Photons': {
        'PT_MIN' : '1600*MeV',
        'CL_MIN' : '0.25'
        },    
    "V0s": { # Cuts for rho, K*, phi
        'MASS_MIN'      : {'KST':'700*MeV','RHO':'600*MeV','PHI':'900*MeV'},
        'MASS_MAX'      : {'KST':'1100*MeV','RHO':'1000*MeV','PHI':'1100*MeV'},
        'DAUGHTERS'     : {'PT_MIN':'150*MeV','P_MIN':'2000*MeV',
                           'MIPCHI2DV_MIN' : 4, 'TRCHI2DOF_MAX' : 4},
        'AMAXDOCA_MAX'  : '0.5*mm',
        'VCHI2DOF_MAX'  : 16,
        'BPVIPCHI2_MAX' : 16,
        'BPVVDCHI2_MIN' : 25, 
        'BPVDIRA_MIN'   : 0,
        'ASUMPT_MIN'    : '1000*MeV'
        },    
    "B2X3BODY" : { 
        'SUMPT_MIN'     : '5000*MeV',
        'VCHI2DOF_MAX'  : 10,
        'BPVIPCHI2_MAX' : 20,
        'BPVVDCHI2_MIN' : 25,
        'BPVDIRA_MIN'   : 0.9, #0.0
        'MASS_MIN'      : {'B':'4300*MeV'},
        'MASS_MAX'      : {'B':'6400*MeV'},
        },
    "B2X3BODYLOWM" : { 
        'SUMPT_MIN'     : '3900*MeV',
        'VCHI2DOF_MAX'  : 10,
        'BPVIPCHI2_MAX' : 25,
        'BPVVDCHI2_MIN' : 25,
        'BPVDIRA_MIN'   : 0.9, #0.0
        'MASS_MIN'      : {'J':'2600*MeV'},
        'MASS_MAX'      : {'J':'3600*MeV'},
        },
    "B2X3BODYHIGHM" : { 
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 10,
    'BPVDIRA_MIN'   : 0.9,
    'MASS_MIN'      : {'Y':'9000*MeV'},
    'MASS_MAX'      : {'Y':'10000*MeV'},
    },
    "B2X4BODY" : { 
        'SUMPT_MIN'     : '5000*MeV',
        'VCHI2DOF_MAX'  : 6,
        'BPVIPCHI2_MAX' : 16,
        'BPVVDCHI2_MIN' : 50,
        'BPVDIRA_MIN'   : 0.0,
        'MASS_MIN'      : {'B':'4600*MeV'},
        'MASS_MAX'      : {'B':'6000*MeV'}
        },
    "Prescales" : {
        'OS'            : 1.0,
        'SS'            : 0.5
        },
    'GECNTrkMax'   : 500
    }    
    }



# Hadronic lines for B -> X tau (tau tau, D* tau nu etc) 
# N. Serra, Vava to be put back in the future 




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
    "GAMMA" : { 'PT_MIN'       : '2500*MeV',
                'CL_MIN'       : 0.25
                },
    "KS0" : { 'PT_MIN'        : '1000*MeV',
              'BPVVDCHI2_MIN' : 81 ,
              'VCHI2DOF_MAX'  : 9,
              'MM_MIN'        : '480.*MeV',
              'MM_MAX'        : '515.*MeV' },
    "Pi0" : { 'PT_MIN'        : '1000*MeV',
              'P_MIN'         : '4000*MeV',
              'CHILDCL1_MIN'  : 0.25,
              'CHILDCL2_MIN'  : 0.25 },
    "B2X" : { 'SUMPT_MIN'     : '5000*MeV',
              'VCHI2DOF_MAX'  : 9,
              'BPVIPCHI2_MAX' : 9,
              'BPVDIRA_MIN'   : 0.9998,
              'AM_MIN'        : '4000*MeV',
              'AM_MAX'        : '7000*MeV',
              },
    "HH": { 'MASS_WINDOW'      : {'KST':'150*MeV','RHO':'250*MeV','PHI':'15*MeV','OMEGA':'30*MeV'},
            'DAUGHTERS'        : {'PT_MIN':'500*MeV','P_MIN':'3000*MeV'},
            'piLAMBDADAUGHTERS': {'PT_MIN':'300*MeV','P_MIN':'3000*MeV','PIDK_MAX':'5'},   # only for pH
            'pLAMBDADAUGHTERS' : {'PT_MIN':'1200*MeV','P_MIN':'10000*MeV','PIDp_MIN':'5'}, # only for pH
            'kLAMBDADAUGHTERS' : {'PT_MIN':'300*MeV','P_MIN':'3000*MeV','PIDK_MIN':'0'},    # only for pH
            'VCHI2DOF_MAX'     : 9,
            'BPVVDCHI2_MIN'    : 81,
            'ASUMPT_MIN'       : '1500*MeV',
            },
    
    "HHH": { 'MASS_WINDOW'   : {'OMEGA': '150*MeV'},
             'DAUGHTERS'     : {'PT_MIN':'300*MeV','P_MIN':'2000*MeV'},
             'VCHI2DOF_MAX'  : 9,
             'BPVVDCHI2_MIN' : 64,
             'ASUMPT_MIN'    : '1600*MeV',
             },
    "PID" : { 'P'  : {'PIDp_MIN' : 0},
              'PI' : {'PIDK_MAX' : 20},
              'K'  : {'PIDK_MIN' : -10} },
    "Prescales"  : {},
    "GECNTrkMax" : 500 },
    'WGs'    : ['RD'],
    'STREAMS' : [ 'Radiative' ] 
    }

