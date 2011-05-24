"""
Line config dictionaries for the RD WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import * 

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



# B2XMuMu for RD
# P. Schaak, M. Patel & T.Blake

B2XMuMu =  {
    'BUILDERTYPE' : 'B2XMuMuConf',
    'CONFIG' : {
    'BVXCHI2NDOF'        : 4.0           # dimensionless
    , 'BIPCHI2'            : 9.0           # dimensionless 
    , 'BDIRA'              : 0.999968      # dimensionless
    , 'BFDCHI2'            : 100.0         # dimensionless
    , 'KpiMINIPCHI2'       : 9.0           # dimensionless
    , 'KpiTRACKCHI2'       : 4.0           # dimensionless         
    , 'KpiVXCHI2NDOF'      : 9.0           # dimensionless       
    , 'MuonMINIPCHI2'      : 16.0           # dimensionless      
    , 'MuonTRACKCHI2'      : 4.0           # dimensionless    
    , 'MuonPID'            : 0.0           # dimensionless            
    , 'DimuonVXCHI2NDOF'   : 9.0           # dimensionless 
    , 'DimuonUPPERMASS'    : 5050.0        # MeV      
    , 'Pi0MINPT'           : 800.0         # MeV  
    , 'DplusLOWERMASS'     : 1600.0        # MeV
    , 'DplusUPPERMASS'     : 2300.0        # MeV 
    , 'KstarplusWINDOW'    : 300.0         # MeV
    },
    'WGs' : [ 'RD' ],
    'STREAMS' : [ 'Dimuon' ]
    }

