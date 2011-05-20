'''
 Line Config Dictionary for RD WG:

 Stripping 14 : 2010 re-stripping
'''



from GaudiKernel.SystemOfUnits             import mm, micrometer, MeV 

# B2XMuMu lines for Majorana searches
# S. Redford




B2XMuMuSS = {
    'BUILDERTYPE'   : 'B2XMuMuSSConf',
    'CONFIG'        : { 'MuonP'         : 3000. ,    #MeV
                  'MuonPT'        : 500.  ,    #MeV
                  'MuonMINIPCHI2' : 4     ,    #adminensional

                  'PionP'         : 2000. ,    #MeV
                  'PionPT'        : 500.  ,    #MeV
                  'PionMINIPCHI2' : 4     ,    #adminensional

                  'KaonP'         : 2000. ,    #MeV
                  'KaonPT'        : 500.  ,    #MeV
                  'KaonMINIPCHI2' : 4     ,    #adminensional

                  #'DimuonMass'    : 250.  ,    #MeV
                  'BVCHI2DOF'     : 7    ,    #adminensional                              
                  'BDIRA'         : 0.9995 ,    #adimensional
                  'BIPCHI2'       : 30    ,    #adimensional
                  'BMassWin'      : 400.  ,    #MeV, mass window

                  'B2PiMuMuOSLinePrescale'  : 1 ,
                  'B2PiMuMuOSLinePostscale' : 1 ,
                  'B2PiMuMuSSLinePrescale'  : 1 ,
                  'B2PiMuMuSSLinePostscale' : 1 ,
                  'B2KMuMuOSLinePrescale'   : 1 ,
                  'B2KMuMuOSLinePostscale'  : 1 ,
                  'B2KMuMuSSLinePrescale'   : 1 ,
                  'B2KMuMuSSLinePostscale'  : 1 
    },
    'WGs' : [ 'RD' ] ,
    'STREAMS' : [ 'Dimuon' ]
    }

# Bd2KstarMuMu for MVA
# J. Dickens, M. Patel 

Bd2KstarMuMu = {
    'BUILDERTYPE' : 'StrippingBdToKstarMuMuConf',
    'CONFIG' : { 
    'UseNoPIDsHadrons'          : True,
    'Prescale_BdToKstarMuMu'    : 1.0,
    'Postscale_BdToKstarMuMu'   : 1.0,
    'Prescale_BdToKstarMuMuSS'  : 1.0,
    'Postscale_BdToKstarMuMuSS' : 1.0,
    'Prescale_BuToKMuMu'        : 1.0,
    'Postscale_BuToKMuMu'       : 1.0,
    'Prescale_BuToKMuMuSS'      : 1.0,
    'Postscale_BuToKMuMuSS'     : 1.0,
    'B_Comb_MassLow'      : 4600.0,
    'B_Comb_MassHigh'     : 6000.0,
    'B_MassLow'           : 4850.0,
    'B_MassHigh'          : 5780.0,
    'B_VertexCHI2'        :    6.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :   0.9999,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0, 
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    'Kstar_Comb_MassLow'  :  550.0,
    'Kstar_Comb_MassHigh' : 2200.0,
    'Kstar_MassLow'       :  600.0,
    'Kstar_MassHigh'      : 2000.0,
    'Kstar_MinIPCHI2'     :    0.0,
    'Kstar_FlightChi2'    :   9.0, 
    'Kstar_Dau_MaxIPCHI2' : 9.0, 
    'Dimu_FlightChi2'     :   9.0, 
    'Dimu_Dau_MaxIPCHI2'  : 9.0, 
    'Track_CHI2nDOF'      :    5.0,
    'Hadron_MinIPCHI2'    :    9.0, 
    'Muon_MinIPCHI2'      :    9.0,
    'Muon_IsMuon'         :   False
    },
    'WGs' : [ 'RD' ] ,
    'STREAMS' : [ 'Dimuon' ]
    }


# HyperCP lines, Vanya + Andrei

HyperCP = {
    'BUILDERTYPE' : 'StrippingHyperCPXConf',
    'CONFIG' : {
    'ProtonCuts' : ' ( TRCHI2DOF < 5 ) & ( 0 < PIDp  - PIDpi ) & ( BPVIPCHI2() > 12 ) ' , 
    'MuonCuts'   : ' ( TRCHI2DOF < 5 ) & ISMUON                & ( BPVIPCHI2() > 12 ) ' ,                
    'PionCuts'   : ' ( TRCHI2DOF < 5 )                         & ( BPVIPCHI2() > 12 ) ' ,
    'SigmaCTau'  :   5 *         mm ,
    'SigmaMass'  : 250 *        MeV ,
    'DsCTau'     : 100 * micrometer ,
    'DsMass'     : 250 *        MeV ,
    'DplusCTau'  : 200 * micrometer ,
    'DplusMass'  : 250 *        MeV ,
    'PrimaryVertices' : True ,
    'Preambulo'       : [
    'chi2vx = VFASPF(VCHI2) '                    , 
    "from GaudiKernel.PhysicalConstants import c_light" , 
    "ctau   = BPVLTIME ( 9 ) * c_light "  ,
    "phi    = in_range ( 920 * MeV , AM23 , 1120 * MeV )"
    ] ,
    'SigmaPrescale' : 1.0 ,
    'DplusPrescale' : 1.0 ,
    'DsPrescale'    : 1.0 ,
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }


B2SameChargeMuon = {
    'BUILDERTYPE'  : 'StrippingB2SameChargeMuonConf',
    'CONFIG'  : { 
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }
