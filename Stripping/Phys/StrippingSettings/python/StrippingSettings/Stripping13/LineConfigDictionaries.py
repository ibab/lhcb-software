"""
Place holder for stripping dictionary configurations under development.
These will not be used for production, only for testing.
Dictionaries have the name of the line builder instance.
"""

# standard Bs->Jpsi/Phi prescaled and detatched lines
Bs2JpsiPhiPrescaledAndDetatched = {
    'BUILDERTYPE' : 'Bs2JpsiPhiPrescaledAndDetatchedConf',
    'CONFIG' : {'PhiPT' : 500.,
                'BsMassWin' : 300.,
                'BsLTFITCHI2' : 14.,
                'BsVCHI2' : 20.,
                'DetatchedBsLTIME' : 15.,
                'PrescaledLinePrescale' : 1.0,
                'PrescaledLinePostscale' : 0.1,
                'DetatchedLinePrescale' : 1.0,
                'DetatchedLinePostscale' : 1.0
                }
    }

# Prescaled Bs->Jpsi/Phi prescaled and detatched lines
# Copy standard parameters, change pre-scales.
Bs2JpsiPhiPrescaledAndDetatchedPrescaled = dict(Bs2JpsiPhiPrescaledAndDetatched)
Bs2JpsiPhiPrescaledAndDetatchedPrescaled['CONFIG']['PrescaledLinePrescale'] = 0.1
Bs2JpsiPhiPrescaledAndDetatchedPrescaled['CONFIG']['DetatchedLinePrescale'] = 0.1

################
# beta_s group #
################
# b2JpsiX lines (Greig)
BetaS = {         
	'WGs'		: ['betas'],
	'BUILDERTYPE' 	: 'B2JpsiXforBeta_sConf',
	'CONFIG' 	: {
 		'TRCHI2DOF'             :       4
        ,       'BPVLTIME'              :       0.2
        ,       'DaughterPT'            :       1000
        ,       'VCHI2PDOF'             :       10
        ,       'Jpsi2MuMuPrescale'     :       0.1
        ,       'Bu2JpsiKPrescale'      :       0.1
        ,       'Bd2JpsiKstarPrescale'  :       0.05
        ,       'Bd2JpsiKsPrescale'     :       0.5
        ,       'Bs2JpsiPhiPrescale'    :       0.2
        ,       'Bs2JpsiEtaPrescale'    :       0.2
        }
	}

# Neus
BetaSPsi2S = {
	'WGs'		: ['betas'],
        'BUILDERTYPE'   : 'B2Psi2SXConf',
	'CONFIG'	: {
		'PionsTRCHI2DOF': 5,
                 'Psi2SJpsiMIPCHI2DV' : 4 ,
                 'Psi2SPiMIPCHI2DV':4 ,
                 'Psi2SAM23down': 400,
                 'Psi2SAM23up': 800,
                 'Psi2SAPT': 500,
                 'Psi2SADAMASS': 30,
                 'Psi2SVFASPF': 16,
                 'ChKTRCHI2DOF':5,
                 'ChKPID':-2,#   
                 'PhiWin':20,
                 'PhiPT':500,
                 'PhiVFASPF':16,
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,#
                 'KstMassDown':  826,
                 'KstMassUp': 966,
                 'KstAPT':500,
                 'KstVFASPF':16,
                 'KstTRCHI2DOF':4,
                 'KstPIDK': -2,
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.5,
                 'incl_LinePostscale':1,
                 'BsMassCutDownPre':5000,
                 'BsMassCutUpPre':5650,
                 'BsMassCutDownPost':5100,
                 'BsMassCutUpPost':5550,
                 'BsVCHI2PDOF':10,
                 'sig_LinePrescale': 1,
                 'sig_LinePostscale': 1,
                 'ChKPT': 500 ,
                 'K_LinePrescale': 1,
                 'K_LinePostscale': 1,
                 'Kstar_LinePrescale': 1,
                 'Kstar_LinePostscale': 1,
                 'BKsVCHI2PDOF': 10,
                 'Ks_LinePrescale': 1,
                 'Ks_LinePostscale':1
                 }
	}

# Neus
BetaSPsi2SMuMu = {
        'WGs'           : ['betas'],
        'BUILDERTYPE'   : 'Bs2Psi2SPhiMuMuConf',
        'CONFIG'        : {
		'muPID':0.,
                 'Psi2SMassWin':60.,
                 'Psi2SADOCACHI2CUT':30.,
                 'Psi2SVFASPF':16,
                 'ChKTRCHI2DOF':5,
                 'ChKPID':-2,   
                 'PhiWin':20,
                 'PhiPT':500,
                 'PhiVFASPF':16,
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,
                 'KstMassDown':  826,
                 'KstMassUp': 966,
                 'KstAPT':500,
                 'KstVFASPF':16,
                 'KstTRCHI2DOF':4,
                 'KstPIDK': -2,
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.5,
                 'incl_LinePostscale':1,
                 'BPVLTIME_detatched':0.15,
                 'incl_DetatchedLinePrescale':1,
                 'incl_DetatchedLinePostscale':1,
                 'BsMassCutDownPre':5000,
                 'BsMassCutUpPre':5650,
                 'BsMassCutDownPost':5100,
                 'BsMassCutUpPost':5550,
                 'BsVCHI2PDOF':10,
                 'sig_PrescaledLinePrescale':1,
                 'sig_PrescaledLinePostscale':1,
                 'sig_DetatchedLinePrescale':1,
                 'sig_DetatchedLinePostscale':1,
                 'BPVLTIME_unbiased': None,
                 'MINTREEPT':1000,
                 'sig_UnbiasedLinePrescale':1,
                 'sig_UnbiasedLinePostscale':1,
                 'ChKPT':500,
                 'K_PrescaledLinePrescale':1,
                 'K_PrescaledLinePostscale':1,
                 'K_DetatchedLinePrescale':1,
                 'K_DetatchedLinePostscale':1,
                 'K_UnbiasedLinePrescale':1,
                 'K_UnbiasedLinePostscale':1,
                 'KstarPT': 2,
                 'Kstar_PrescaledLinePrescale':0.5,
                 'Kstar_PrescaledLinePostscale':1,
                 'Kstar_DetatchedLinePrescale':1,
                 'Kstar_DetatchedLinePostscale':1,
                 'Kstar_UnbiasedLinePrescale':1,
                 'Kstar_UnbiasedLinePostscale':1,
                 'BKsVCHI2PDOF':10,
                 'Ks_PrescaledLinePrescale':1,
                 'Ks_PrescaledLinePostscale':1,
                 'Ks_DetatchedLinePrescale':1,
                 'Ks_DetatchedLinePostscale':1,
                 'Ks_UnbiasedLinePrescale':1,
                 'Ks_UnbiasedLinePostscale':1
                 }
	}

# Bs2Jpsi(ee)Phi (Jibo and Artur)
BetaSBs2JpsieePhi = {
	'WGs'		: ['betas'],
	'BUILDERTYPE' 	: 'Bs2JpsieePhiConf',
	'CONFIG' 	: {
                  'ElectronPTLoose'            :   800.    # MeV
                , 'ElectronPIDLoose'           :     0.    # adimensional
                , 'ElectronTrackCHI2pDOFLoose' :    10.    # adimensional
                , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
                , 'JpsiMassMinLoose'           :  2700.    # MeV
                , 'JpsiMassMaxLoose'           :  3300.    # MeV
                , 'KaonTrackCHI2pDOFLoose'     :    10.    # adimensional
                , 'PhiPTLoose'                 :  1000.    # MeV
                , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
                , 'PhiMassMinLoose'            :   990.    # MeV
                , 'PhiMassMaxLoose'            :  1050.    # MeV
                , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
                , 'BsMassMinLoose'             :  4500.    # MeV
                , 'BsMassMaxLoose'             :  6000.    # MeV
                , 'BsDIRALoose'                :     0.99  # adimensional
                , 'ElectronPT'            :   800.    # MeV
                , 'ElectronPID'           :     2.    # adimensional
                , 'ElectronTrackCHI2pDOF' :     5.    # adimensional
                , 'JpsiVertexCHI2pDOF'    :    15.    # adimensional
                , 'JpsiMassMin'           :  2700.    # MeV
                , 'JpsiMassMax'           :  3300.    # MeV
                , 'KaonTrackCHI2pDOF'     :     5.    # adimensional
                , 'KaonPID'               :     0.    # adimensional
                , 'PhiPT'                 :  1000.    # MeV
                , 'PhiVertexCHI2pDOF'     :    15.    # adimensional
                , 'PhiMassMin'            :  1005.    # MeV
                , 'PhiMassMax'            :  1035.    # MeV
                , 'BsVertexCHI2pDOF'      :     6.    # adimensional
                , 'BsMassMin'             :  4500.    # MeV
                , 'BsMassMax'             :  6000.    # MeV
                , 'BsDIRA'                :     0.99  # adimensional
                } 
	}

# Quasi two-body (Fred Blanc)
BetaSQ2B = {
	'WGs'		: ['betas'],
	'BUILDERTYPE' 	: 'B2Quasi2Body4piConf',
	'CONFIG' 	: {
		 'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 1500.,
                 'Q2BResMinP'      : 10.,
                 'Q2BResMaxMass'   : 900.,
                 'Q2BResVtxChiDOF' : 4.,
                 'Q2BBMinM'        : 2800.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBVtxChi2DOF'  : 4.,
                 'Q2BBMinTrkPT'    : 1500.,
                 'Q2BBPVVDZ'       : 2. 
		}
	}

# Bs2Ph2PhiPhi (Sebastian)
BetaSBs2PhiPhi	= {
	'WGs'		: ['betas'],
	'BUILDERTYPE' 	: 'StrippingBs2PhiPhiConf',
	'CONFIG' 	: {
                  'KaonPT'              : 500      # MeV/c 
                , 'KaonIPCHI2'          : 3.5      # adimensional
                , 'PhiPT'               : 900      # MeV/c 
                , 'PhiPTsq'             : 2.0      # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'   : 25       # adimensional
                , 'PhiMassWindow'       : 25       # MeV/cc
                , 'PhiMassMax'          : 1090     # MeV/cc
                , 'BsVertexCHI2pDOF'    : 25       # adimensional
                , 'BsMassWindow'        : 300      # MeV/cc
                , 'WidePrescale'        : 0.05     # adimensional
		}
	} 

# Bs2KstKst (Paula)
BetaSBs2KstKst = {
	'WGs'		: ['betas'],
	'BUILDERTYPE' 	: 'StrippingBs2KstKstConf',
	'CONFIG' 	: {
          	"KaonPT"              : 500.0 # MeV
          ,     "KaonIPCHI2"            : 9.    # adimensional
          ,  	"PionPT"                : 500.0 # MeV    
          ,  	"PionIPCHI2"            : 9.       # adimensional
          ,     "KstarVCHI2"            : 9.0   # adimensional
          ,     "KstarPT"               : 900.0 # MeV 
          ,     "KaonPIDK"              : -5.0  # adimensional
          ,     "KstarMassWin"          : 150.0 # MeV 
          ,     "BMassWin"              : 500.0 # MeV
          ,     "BVCHI2"                : 15.0  # adimensional
          ,     "BDOCA"                 : 0.3   # mm
          ,  	"BIPCHI2"               : 25    # adimensional
          }
	}

# Cibran
BetaSBs2PhiKst = {
	'WGs'   	: ['betas'],
	'BUILDERTYPE'	: 'StrippingBs2PhiKstConf',
	'CONFIG'	: {	
          	"KaonPT"                : 500.0 # MeV
          ,     "KaonIPCHI2"            : 9.    # adimensional
          ,     "KaonPIDK"              : 0.    # adimensional
          ,     "PionPT"                : 500.0 # MeV
          ,     "PionIPCHI2"            : 9.    # adimensional
          ,     "PhiVCHI2"              : 9.    # adimensional
          ,     "PhiPT"                 : 900.0 # MeV
          ,     "PhiMassWin"            : 25.0  # MeV
          ,     "KstarVCHI2"            : 9.0   # adimensional
          ,     "KstarPT"               : 900.0 # MeV
          ,     "KstarMassWin"          : 150.0 # MeV
          ,     "BMassWin"              : 500.0 # MeV
          ,     "BVCHI2"                : 15.0  # adimensional
          ,     "BDOCA"                 : 0.3   # mm
          }
	}

# Bs2EtacPhi/Bs2ChicPhi (Katarzyna)
BetaSBs2EtacPhi = {
	'WGs'	:	['betas'],
	'BUILDERTYPE'	:	'Bs2EtacPhiConf',
	'CONFIG'	:	{
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 500.,
    'K_TRCHI2'                : 5.,
    'K_IPCHI2'                : 6.,
    #pi parameters
    'pi_PT'                   : 500.,
    'pi_TRCHI2'               : 5.,
    'pi_IPCHI2'               : 6.,
    #phi parameters
    'phi_VDZ'                 : 0.,
    'phi_PT'                  : 1000.,
    'phi_IPCHI2'              : 6.,
    'phi_VCHI2_VDOF'          : 16.,
    #rho (for eta_c decay) parameters
    'rho_etac_VDZ'            : 0.,
    'rho_etac_VCHI2_VDOF'     : 16.,
    #phi (for eta_c decay) parameters
    'phi_etac_VDZ'            : 0.,
    'phi_etac_VCHI2_VDOF'     : 16.,
    #eta_c parameters         
    'etac_VDZ'                : 0.,   
    'etac_PT'                 : 1000.,
    'etac_IPCHI2'             : 6., 
    'etac_VCHI2_VDOF'         : 16.,
    #Bs parameters            
    'Bs_VDZ'                  : 0.,
    'Bs_DIRA'                 : 0.999,
    'Bs_IPCHI2'               : 9.,
    'Bs_VCHI2_VDOF'           : 16.
    }
}

BetaSBs2ChicPhi_Chic2KKPiPi = {
	'WGs'	:	['betas'],
	'BUILDERTYPE'	:	'Bs2ChicPhi_Chic2KKPiPiConf',
	'CONFIG'	:	{
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #K parameters
    'K_PT'                   : 500.,
    'K_TRCHI2'               : 5.,
    'K_IPCHI2'               : 6.,
    #pi parameters
    'pi_PT'                  : 500.,
    'pi_TRCHI2'              : 5.,
    'pi_IPCHI2'              : 6.,
    #phi parameters
    'phi_VDZ'                : 0.,
    'phi_PT'                 : 1000.,
    'phi_IPCHI2'             : 6.,
    'phi_VCHI2_VDOF'         : 16.,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 16.,
    #phi (for chi_c decay) parameters
    'phi_chic_VDZ'           : 0.,
    'phi_chic_VCHI2_VDOF'    : 16.,
    #chi_c parameters        
    'chic_VDZ'               : 0.,   
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 16.,
    #Bs parameters           
    'Bs_VDZ'                 : 0.,
    'Bs_DIRA'                : 0.999,
    'Bs_IPCHI2'              : 9.,
    'Bs_VCHI2_VDOF'          : 16.
    }
}

BetaSBs2ChicPhi_Chic2PiPiPiPi = {
	'WGs'	:	['betas'],
	'BUILDERTYPE'	:	'Bs2ChicPhi_Chic2PiPiPiPiConf',
	'CONFIG'	: {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #phi parameters
    'K_PT'                   : 500.,
    'K_TRCHI2'               : 5.,
    'K_IPCHI2'               : 6.,
    #pi parameters
    'pi_PT'                  : 500.,
    'pi_TRCHI2'              : 5.,
    'pi_IPCHI2'              : 6.,
    #phi parameters
    'phi_VDZ'                : 0.,
    'phi_PT'                 : 1000.,
    'phi_IPCHI2'             : 9.,
    'phi_VCHI2_VDOF'         : 16.,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 16.,
    #chi_c parameters
    'chic_VDZ'               : 0.,
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 16.,
    #Bs parameters
    'Bs_VDZ'                  : 0.,
    'Bs_DIRA'                 : 0.999,
    'Bs_IPCHI2'               : 9.,
    'Bs_VCHI2_VDOF'           : 16.
    }
}
##############
# beta_s end #
##############


B2XGamma = {
    'BUILDERTYPE' : 'StrippingB2XGammaConf',
    'CONFIG' : {'TrIPchi2Phi'          : 10.       # Dimensionless
                ,'TrIPchi2Kst'         : 10.       # Dimensionless
                ,'PhiMassWin'          : 15.       # MeV
                ,'KstMassWin'          : 100.      # MeV
                ,'KstMassWinSB'        : 200.      # MeV
                ,'BsMassWin'           : 1000.     # MeV
                ,'B0MassWin'           : 1000.     # MeV
                ,'BMassWinSB'          : 2000.     # MeV
                ,'BsDirAngle'          : 0.02      # radians
                ,'B0DirAngle'          : 0.02      # radians
                ,'BDirAngleMoni'       : 0.06      # radians
                ,'BsPVIPchi2'          : 15.       # Dimensionless
                ,'B0PVIPchi2'          : 15.       # Dimensionless
                ,'photonPT'            : 2600.     # MeV
                ,'PhiVCHI2'            : 15.       # dimensionless
                ,'KstVCHI2'            : 15.       # dimensionless
                ,'TrChi2'              : 5.        # dimensionless
                # Pre- and postscales
                ,'Bs2PhiGammaWideBMassPreScale'      : 0.1
                ,'Bs2PhiGammaWideBMassPostScale'     : 1.0
                ,'Bs2PhiGammaLooseDiraPreScale'      : 0.1
                ,'Bs2PhiGammaLooseDiraPostScale'     : 1.0
                ,'Bs2PhiGammaPreScale'               : 1.0
                ,'Bs2PhiGammaPostScale'              : 1.0
                ,'Bd2KstGammaWideBMassPreScale'      : 0.05
                ,'Bd2KstGammaWideBMassPostScale'     : 1.0
                ,'Bd2KstGammaLooseDiraPreScale'      : 0.05
                ,'Bd2KstGammaLooseDiraPostScale'     : 1.0
                ,'Bd2KstGammaWideKstMassPreScale'    : 0.05
                ,'Bd2KstGammaWideKstMassPostScale'   : 1.0
                ,'Bd2KstGammaWidePreScale'           : 0.05
                ,'Bd2KstGammaWidePostScale'          : 1.0
                ,'Bd2KstGammaPreScale'               : 1.0
                ,'Bd2KstGammaPostScale'              : 1.0
                }
    }

B2hhLTUnbiased = {
    'BUILDERTYPE' : 'StrippingB2hhLTUnbiasedConf' ,
    'CONFIG' : {'TrackChi2'               :    5.0
                , 'DaughterPtMin'         :    1.5 # GeV
                , 'DaughterPtMax'         :    2.0 # GeV
                , 'DaughterPMin'          :   10.0 # GeV
                , 'DaughterPIDKMax'       :    0.1
                , 'BMassMin'              :    5.0 # GeV
                , 'BMassMax'              :    6.0 # GeV
                , 'DOCA'                  :    0.07
                , 'VertexChi2'            :   25.0
                , 'PrescaleLoose'         :    0.005
                , 'PostscaleLoose'        :    1.0
                , 'PrescaleNB'            :    1.0
                , 'PostscaleNB'           :    1.0
                , 'NetCut'                :    0.9
                }
    }

NeuroBayesMuMu = {
    'BUILDERTYPE' : 'StrippingNeuroBayesMuMuConf',
    'CONFIG' : {'trackChi2'              :    5.0
                , 'MuMuMassMin'          :    0.0
                , 'MuMuMassMax'          :   20.0
                , 'JPsiMassMin'          :    3.04 # GeV
                , 'JPsiMassMax'          :    3.14 # GeV
                , 'vertexChi2'           :   25.0
                , 'NBCutJPsi'            :    0.7
                , 'NBCutMuMu'            :    0.95
                , 'ExpertiseJPsi'        : 'Muon/mumu_net_noip.nb'
                , 'ExpertiseMuMu'        : 'Muon/mumu_net_full.nb'
                , 'NBVersionJPsi'        : "TuneSep2010"
                , 'NBVersionMuMu'        : "TuneSep2010"
                , 'PrescaleMuMu'         :  1.0
                , 'PostscaleMuMu'        :  1.0
                , 'PrescaleJPsi'         :  1.0
                , 'PostscaleJPsi'        :  1.0
                }
    }

B2twobody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {'nbody':              2,
                'MinBMass':        4500.0,
                'MaxBMass':        8000.0,
                'MinBPt':          2000.0,
                'MaxBVertChi2DOF':   10.0,
                'MinBPVVDChi2':     225.0,
                'MaxBPVIPChi2':      15.0,
                'MinBPVDIRA':         0.0,
                'MinNvc':             2,
                'doPi':            True,
                'doK':             True,
                'dop':             True,
                'doKs':            True,
                'doLm':            True,
                'doDz':            True,
                'doDp':            True,
                'doDs':            True,
                'doLc':            True,
                'doPh':            True,
                'doKS':            True,
                'doJp':            True,
                'doDS':            True,
                'MinPiPt':         1500.0,
                'MinPiIPChi2DV':     25.0,
                'MaxPiChi2':          5.0,
                'MinKPt':          1500.0,
                'MinKIPChi2DV':      25.0,
                'MaxKChi2':           5.0,
                'MinpPt':          1500.0,
                'MinpIPChi2DV':      25.0,
                'MaxpChi2':           5.0,
                'MaxKsDeltaM':       40.0,
                'MinKsPt':         1500.0,
                'MaxKsVertChi2DOF':  10.0,
                'MinKsPVVDChi2':    400.0,
                'MinKsIPChi2':        0.0,
                'MinKsDauPt':       200.0,
                'MinKsDauIPChi2':    16.0,
                'MaxKsDauTrkChi2':   10.0,
                'MaxLmDeltaM':       15.0,
                'MinLmPt':         1500.0,
                'MaxLmVertChi2DOF':  10.0,
                'MinLmPVVDChi2':    400.0,
                'MinLmIPChi2':        0.0,
                'MinLmPrtPt':       500.0,
                'MinLmPiPt':        100.0,
                'MinLmPrtIPChi2':     4.0,
                'MinLmPiIPChi2':     16.0,
                'MaxLmPrtTrkChi2':   10.0,
                'MaxLmPiTrkChi2':    10.0,
                'MaxDzDeltaM':      40.0,
                'MinDzPt':         1500.0,
                'MaxDzVertChi2DOF':  10.0,
                'MinDzPVVDChi2':    225.0,
                'MinDzIPChi2':        0.0,
                'MinDzDauPt':       250.0,
                'MinDzDauIPChi2':     9.0,
                'MaxDzDauTrkChi2':    5.0,
                'MinDzPIDK':         -5.0,
                'MaxDpDeltaM':       40.0,
                'MinDpPt':         1500.0,
                'MaxDpVertChi2DOF':  10.0,
                'MinDpPVVDChi2':    225.0,
                'MinDpIPChi2':        0.0,
                'MinDpDauPt':       250.0,
                'MinDpDauIPChi2':     9.0,
                'MaxDpDauTrkChi2':    5.0,
                'MinDpPIDK':         -5.0,
                'MaxDsDeltaM':       40.0,
                'MinDsPt':         1500.0,
                'MaxDsVertChi2DOF':  10.0,
                'MinDsPVVDChi2':    225.0,
                'MinDsIPChi2':        0.0,
                'MinDsDauPt':       250.0,
                'MinDsDauIPChi2':     9.0,
                'MaxDsDauTrkChi2':    5.0,
                'MinDsPIDK':         -5.0,
                'MaxLcDeltaM':       40.0,
                'MinLcPt':         1500.0,
                'MaxLcVertChi2DOF':  10.0,
                'MinLcPVVDChi2':    225.0,
                'MinLcIPChi2':        0.0,
                'MinLcDauPt':       250.0,
                'MinLcDauIPChi2':     9.0,
                'MaxLcDauTrkChi2':    5.0,
                'MinLcPIDK':         -5.0,
                'MinLcPIDp':         -5.0,
                'MaxPhDeltaM':       30.0,
                'MinPhPt':         1500.0,
                'MaxPhVertChi2DOF':  10.0,
                'MinPhPVVDChi2':    100.0,
                'MinPhIPChi2':       16.0,
                'MinPhDauPt':       500.0,
                'MinPhDauIPChi2':     9.0,
                'MaxPhDauTrkChi2':   10.0,
                'MaxKSDeltaM':      150.0,
                'MinKSPt':         2000.0,
                'MaxKSVertChi2DOF':  10.0,
                'MinKSPVVDChi2':    100.0,
                'MinKSIPChi2':       16.0,
                'MinKSDauPt':       500.0,
                'MinKSDauIPChi2':    16.0,
                'MaxKSDauTrkChi2':    5.0,
                'MaxJpDeltaM':     3000.0,
                'MinJpPt':         1000.0,
                'MaxJpVertChi2DOF':  10.0,
                'MinJpPVVDChi2':     16.0,
                'MinJpIPChi2':        0.0,
                'MinJpDauPt':       500.0,
                'MinJpDauIPChi2':     0.0,
                'MaxJpDauTrkChi2':   10.0,
                'MaxDSDeltaM':      153.0,
                'MinDSPt':         1500.0,
                'MaxDSVertChi2DOF':  10.0,
                'MinDSPVVDChi2':      0.0,
                'MinDSIPChi2':        0.0,
                'MinDSPiPt':        100.0,
                'MinDSPiIPChi2':      0.0,
                'MaxDSPiTrkChi2':     5.0,
                'MaxDSD0DeltaM':     40.0,
                'MinDSD0Pt':       1250.0,
                'MaxDSD0VertChi2DOF':10.0,
                'MinDSD0PVVDChi2':  225.0,
                'MinDSD0IPChi2':      0.0,
                'MinDSD0DauPt':     250.0,
                'MinDSD0DauIPChi2':   9.0,
                'MaxDSD0DauTrkChi2':  5.0
                }
    }

B2threebody=B2twobody.copy()
B2threebody['CONFIG']['nbody']=3
B2threebody['CONFIG']['doKS']=False

Bd2DstarMuNu = {
    'BUILDERTYPE' : 'Bd2DstarMuNuAllLinesConf',
    'CONFIG' :{ 'Loose' : { 'Prescale'    : 0.5 ,
                            'Postscale'   : 1.0 ,
                            'MuPT'        : 700, # 800  # MeV
                            'MuTrPID'     :-20,  # -5   
                            'MuIPChi2'    :  0,  # 4    
                            'KPT'         : 350, # 350  # MeV
                            'KIPChi2'     :  0,  # 4    
                            'KTrPID'      : -5,  #-5    
                            #pion parameters
                            'PiPT'        : 350, # MeV
                            'PiIPChi2'    :  0, # 4    
                            #D0-resonance parameters
                            'D0MassW'     : 60, # 40    # MeV
                            'D0_BPVVDCHI2': 50,
                            #Dstar-resonance parameters
                            'Dstar_PT'     : 1250, # 1250 # MeV
                            'Dstar_VCHI2'  : 25, # 20 # 
                            'DstarMassW'   : 80, # 50 # MeV
                            #Bd-resonance parameters
                            'B0d_VCHI2'    : 10, # 10 # 
                            'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
                            'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
                            },
                'LooseNoPresc' : { 'Prescale'    : 1. ,
                                   'Postscale'   : 1.0 ,
                                   #muon paramters # loose # Tight
                                   'MuPT'        : 700, # 800  # MeV
                                   'MuTrPID'     :-20,  # -5   
                                   'MuIPChi2'    :  0,  # 4    
                                   #kaon parameters
                                   'KPT'         : 350, # 350  # MeV
                                   'KIPChi2'     :  0,  # 4    
                                   'KTrPID'      : -5,  #-5    
                                   #pion parameters
                                   'PiPT'        : 350, # MeV
                                   'PiIPChi2'    :  0, # 4    
                                   #D0-resonance parameters
                                   'D0MassW'     : 60, # 40    # MeV
                                   'D0_BPVVDCHI2': 50,
                                   #Dstar-resonance parameters
                                   'Dstar_PT'     : 1250, # 1250 # MeV
                                   'Dstar_VCHI2'  : 25, # 20 # 
                                   'DstarMassW'   : 80, # 50 # MeV
                                   #Bd-resonance parameters
                                   'B0d_VCHI2'    : 10, # 10 # 
                                   'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
                                   'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
                                   },
                'Tight'   : {'Prescale'    : 1.0 ,
                             'Postscale'   : 1.0 ,
                             #muon paramters # 
                             'MuPT'        : 800, # MeV
                             'MuTrPID'     :-5,
                             'MuIPChi2'    : 4,
                             #kaon parameters
                             'KPT'         : 350, # MeV
                             'KIPChi2'     : 4,
                             'KTrPID'      :-5, 
                             #pion parameters
                             'PiPT'        : 350, # MeV
                             'PiIPChi2'    : 4,
                             #D0-resonance parameters
                             'D0MassW'     : 40, # MeV
                             'D0_BPVVDCHI2': 50,
                             #Dstar-resonance parameters
                             'Dstar_PT'     : 1250, #  MeV
                             'Dstar_VCHI2'  : 20, 
                             'DstarMassW'   : 50, 
                             #Bd-resonance parameters
                             'B0d_VCHI2'    : 10, 
                             'B0dD0_DeltaZVtx' : -2.5, # mm
                             'B0dPV_DeltaZVtx' : 0.5 #  mm
                             }
                }
    
    }

B0q2DplusMuX = {
    'BUILDERTYPE' : 'B0q2DplusMuXAllLinesConf',
    'CONFIG' : { 'Presel' : { 'Prescale'    : 0.02 ,
                              'Postscale'   : 1.0 ,
                              'MuPT'        : 500, #MeV
                              'MuPidPi'     : -5.,
                              'MuPidK'      : -10.,
                              'MuTrChi2'    : 9,
                              'KPT'         : 300, #MeV
                              'KIP'         : 0.03, #mm
                              'KPidPi'      : -5,
                              'KPidMu'      : -40,
                              'KPidP'       : -40,
                              'KTrChi2'     : 16,
                              'KIPChi2'     : 4,
                              'PiPidK'      : -40,
                              'PiIP'        : 0.01, #mm
                              'PiPidMu'     : -5,
                              'PiTrChi2'    : 16,
                              'PiIPChi2'    : 4,
                              'DPT'         : 1000, #MeV
                              'D_APT'       : 1000, #MeV
                              'D_VPCHI2'    : 0.000250,
                              'D_BPVVDCHI2' : 49,
                              'B_VPCHI2'    : 0.000250,
                              'B_BPVDIRA'   : 0.997
                              },
    
                 'Tuned'   : { 'Prescale'    : 1.0 ,
                               'Postscale'   : 1.0 ,
                               'MuPT'        : 500, #MeV
                               'MuPidPi'     : -1.,
                               'MuPidK'      : -5,
                               'MuTrChi2'    : 5,
                               'KPT'         : 400, #MeV
                               'KIP'         : 0.04, #mm
                               'KPidPi'      : 0,
                               'KPidMu'      : 5,
                               'KPidP'       : -10,
                               'KTrChi2'     : 5,
                               'KIPChi2'     : 4,
                               'PiPidK'      : -10,
                               'PiIP'        : 0.04, #mm
                               'PiPidMu'     : -5,
                               'PiTrChi2'    : 10,
                               'PiIPChi2'    : 7,
                               'DPT'         : 1500, #MeV
                               'D_APT'       : 1200, #MeV
                               'D_VPCHI2'    : 0.010,
                               'D_BPVVDCHI2' : 144,
                               'B_VPCHI2'    : 0.010,
                               'B_BPVDIRA'   : 0.9980
                               },
    
                 'MC09'   : { 'Prescale'    : 1.0 ,
                              'Postscale'   : 1.0 ,
                              'MuPT'        : 600, #MeV
                              'MuPidPi'     : 0.,
                              'MuPidK'      : -10,
                              'MuTrChi2'    : 2,
                              'KPT'         : 400, #MeV
                              'KIP'         : 0.03, #mm
                              'KPidPi'      : 7,
                              'KPidMu'      : 10,
                              'KPidP'       : -10,
                              'KTrChi2'     : 3,
                              'KIPChi2'     : 4,
                              'PiPidK'      : -5,
                              'PiIP'        : 0.01, #mm
                              'PiPidMu'     : -2,
                              'PiTrChi2'    : 9,
                              'PiIPChi2'    : 7,
                              'DPT'         : 1500, #MeV
                              'D_APT'       : 1200, #MeV
                              'D_VPCHI2'    : 0.0150,
                              'D_BPVVDCHI2' : 144,
                              'B_VPCHI2'    : 0.0150,
                              'B_BPVDIRA'   : 0.9980
                              }
                 
                 }
    }

Bd2DstarTauNu = {
    'BUILDERTYPE' : 'Bd2DstarTauNuAllLinesConf',
    'CONFIG' : { 'Loose' : { 'Prescale'    : 0.1 ,
                             'Postscale'   : 1.0 ,
                             #tau: single pi cuts
                             'SinglePiPT'  : 150 ,
                             'SinglePiIPChi2' : 0,
                             #tau: di pi cuts
                             'DiPiPT'      : 600,
                             'DiPiLowM'    : 600,
                             'DiPiHighM'   : 2000,
                             'DiPiVxChi2'  : 12,
                             'DiPiDira'    : 0.99,
                             'DiPiIPChi2'  : 0,
                             #tau: di pi combi cut
                             'DiPiMaxIPChi2' : 20,
                             #tau: tri pi cuts
                             'TriPiPT'     : 800,
                             'TriPiLowM'   : 1000,
                             'TriPiHighM'  : 2400,
                             'TriPiVxChi2' : 12,
                             'TriPiDira'   : 0.99,
                             'TriPiIPChi2' : 0,
                             #Bd cuts
                             'BdVxChi2'    : 0,
                             'BdDira'      : 0.99,
                             'VxOrder'     : 0,
                             #Bd combi cuts
                             'BdLowDM'     : -2279,
                             'BdHighDM'    : 500,
                             #kaon parameters
                             'KPT'         : 350, # 350  # MeV
                             'KTrChi2'     :  8,  # 10   
                             'KIPChi2'     :  0,  # 4 already in StdLooseD0
                             'KTrPID'      : -5,  #-5
                             'KIP'         :  0,
                             #pion parameters
                             'PiPT'        : 350, # MeV
                             'PiTrChi2'    :  8, # 10   
                             'PiIPChi2'    :  0, # 4 already in StdLooseD0
                             'PiIP'        :  0,
                             #D0-resonance parameters
                             'D0MassW'     : 60, # 40    # MeV
                             'D0_BPVVDCHI2': 50,
                             #Dstar-resonance parameters
                             'slowPiTrChi2' : 8, # 10 
                             'Dstar_PT'     : 1250, # 1250 # MeV
                             'Dstar_VCHI2'  : 25, # 20 # 
                             'DstarMassW'   : 80, # 50 # MeV
                             #Bd-resonance parameters
                             'B0d_VCHI2'    : 10, # 10 # 
                             'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
                             'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
                             },
                 'Tight'   : {'Prescale'    : 1.0 ,
                              'Postscale'   : 1.0 ,
                              #tau: single pi cuts
                              'SinglePiPT'  : 150 ,
                              'SinglePiIPChi2' : 4,
                              #tau: di pi cuts
                              'DiPiPT'      : 600,
                              'DiPiLowM'    : 750,
                              'DiPiHighM'   : 1500,
                              'DiPiVxChi2'  : 9,
                              'DiPiDira'    : 0.99,
                              'DiPiIPChi2'  : 5,
                              #tau: di pi combi cut
                              'DiPiMaxIPChi2' : 20,
                              #tau: tri pi cuts
                              'TriPiPT'     : 800,
                              'TriPiLowM'   : 1200,
                              'TriPiHighM'  : 1780,
                              'TriPiVxChi2' : 9,
                              'TriPiDira'   : 0.99,
                              'TriPiIPChi2' : 5,
                              #Bd cuts
                              'BdVxChi2'    : 0,
                              'BdDira'      : 0.99,
                              'VxOrder'     : 0,
                              #Bd combi cuts
                              'BdLowDM'     : -2279,
                              'BdHighDM'    : 0,
                              #kaon parameters
                              'KPT'         : 350, # MeV
                              'KTrChi2'     : 8,
                              'KIPChi2'     : 4,
                              'KTrPID'      :-5,
                              'KIP'         : 0.04,
                              #pion parameters
                              'PiPT'        : 350, # MeV
                              'PiTrChi2'    : 8,
                              'PiIPChi2'    : 4,
                              'PiIP'        : 0.04,
                              #D0-resonance parameters
                              'D0MassW'     : 40, # MeV
                              'D0_BPVVDCHI2': 50,
                              #Dstar-resonance parameters
                              'slowPiTrChi2' : 8, 
                              'Dstar_PT'     : 1250, #  MeV
                              'Dstar_VCHI2'  : 20, 
                              'DstarMassW'   : 50, 
                              #Bd-resonance parameters
                              'B0d_VCHI2'    : 10, 
                              'B0dD0_DeltaZVtx' : -2.5, # mm
                              'B0dPV_DeltaZVtx' : 0.5 #  mm
                              }
                 }

    }

B2XuMuNu = {
    'BUILDERTYPE' : 'B2XuMuNuBuilder',
    'CONFIG' :  {    "MuonTRCHI2"          : 6.   ,#adimensional
                     "MuonP"               : 3000. ,#MeV
                     "MuonPT"              : 1000. ,#MeV
                     "MuonPIDK"            : 0.    ,#adimensional
                     "MuonPIDmu"           : 3.    ,#adimensional
                     "MuonPIDp"            : 0.    ,#adimensional
                     #        "MuonMINIP"           : 0.05  ,#mm
                     "MuonMINIPCHI2"       : 12     ,#adminensional
                     #Xu
                     #Pi Channel
                     "PionTRCHI2"          : 6.   ,#adimensional
                     "PionP"               : 3000. ,#MeV
                     "PionPT"              : 800.  ,#MeV
                     "PionPIDK"            : 0.    ,#adimensional
                     "PionPIDmu"           : 0.    ,#adimensional
                     "PionPIDp"            : 0.    ,#adimensional
                     #        "PionMINIP"           : 0.05  ,#mm
                     "PionMINIPCHI2"       : 16     ,#adminensional
                     #K channel
                     "KaonTRCHI2"          : 6.   ,#adimensional
                     "KaonP"               : 3000. ,#MeV
                     "KaonPT"              : 800.  ,#MeV
                     "KaonPIDK"            : 5.    ,#adimensional // JUST CHANGED THIS
                     "KaonPIDmu"           : 0.    ,#adimensional
                     "KaonPIDp"            : 0.    ,#adimensional
                     #        "KaonMINIP"          : 0.05  ,#mm
                     "KaonMINIPCHI2"       : 16     ,#adminensional
                     #Rho channel
                     "RhoMassWindow"       : 190.  ,#MeV
                     "RhoVCHI2DOF"         : 6    ,#adimensional
                     "RhoPT"               : 500.  ,#MeV
                     "RhoLeadingPionPT"    : 800.  ,#MeV
                     #        "RhoMINIP"           : 0.05  ,#mm 
                     "RhoMINIPCHI2"        : 4     ,#adimensional
                     "RhoChPionPT"         : 300.  ,#MeV
                     "RhoChPionTRCHI2"     : 10.  ,#MeV
                     "RhoChPionPIDK"       : -10.   ,#adminensional
                     #        "RhoChPionMINIP"     : 0.05  ,#mm
                     "RhoChPionMINIPCHI2"  : 9     ,#adimensional
                     #K* channel
                     "KSMassWindow"        : 300.  ,#MeV
                     "KSVCHI2DOF"          : 6    ,#adimensional
                     "KSPT"                : 400.  ,#MeV
                     #        "KSMINIP"             : 0.05  ,#mm 
                     "KSMINIPCHI2"         : 4     ,#adimensional
                     "KstarMassWindow"     : 100.  ,#MeV
                     "KstarVCHI2DOF"       : 6    ,#adimensional
                     "KstarPT"             : 400.  ,#MeV
                     #        "KstarMINIP"          : 0.05  ,#mm 
                     "KstarMINIPCHI2"      : 4     ,#adimensional
                     "KstarChPionPT"       : 100.  ,#MeV
                     "KstarChPionTRCHI2"   : 10.  ,#MeV
                     "KstarChPionPIDK"     : -10.   ,#adminensional
                     #        "KstarChPionMINIP"    : 0.05  ,#mm
                     "KstarChPionMINIPCHI2": 9     ,#adimensional
                     "KstarLeadingPionPT"  : 800.  ,#MeV
                     #Xu
                     "PiMuMassLowLoose"    : 1230. ,#MeV
                     "KMuMassLowTight"     : 1370. ,#MeV
                     "RhoMuMassLowTight"   : 1360. ,#MeV
                     "KstarMuMassLowTight" : 1400. ,#MeV
                     #B
                     "BVCHI2DOF"           : 4     ,#adminensional
                     "BDIRA"               : 0.99,  #adminensional
                     "BFDCHI2LOW"             : 15,   #adimensional
                     "BFDCHI2HIGH"             : 225,   #adimensional
                     "XMuMassUpper"        : 5500 # MeV
                     }
    }

B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' :  { "MINIPCHI2"      : 9.0    # adimensiional
                  ,"TRCHI2"        : 5.0    # adimensiional
                  ,"KaonPIDK"      : 4.0    # adimensiional
                  ,"PionPIDK"      : 10.0   # adimensiional
                  ,"MuonIPCHI2"    : 4.00   # adimensiional
                  ,"MuonPT"        : 800.0  # MeV
                  ,"KPiPT"         : 300.0  # MeV
                  ,"DsDIRA"        : 0.99   # adimensiional
                  ,"DsFDCHI2"      : 100.0  # adimensiional
                  ,"DsMassWin"     : 80.0   # MeV
                  ,"DsAMassWin"    : 100.0  # MeV
                  ,"DsIP"          : 7.4    #mm
                  ,"DsVCHI2DOF"    : 6.0    # adimensiional
                  ,"PIDmu"         : -0.0   # adimensiional
                  ,"BDIRA"         : 0.999  #adimensiional
                  ,"BVCHI2DOF"     : 6.0    # adimensiional
                  ,"DZ"            : 0.0    #mm
                  ,"DDocaChi2Max"  : 20     #adimensiional
                  }
   
    }

####################################
# Dictionaries for PID Calib Lines #
# A. Powell                        #
####################################

# NoPID D* Line
# A. Powell & F. Xing
NoPIDDstarWithD02RSKPi = {
    'BUILDERTYPE' : 'NoPIDDstarWithD02RSKPiConf',
    'CONFIG' : {'DaugPt'             : 250       ## MeV
                , 'DaugP'            : 2000      ## MeV
                , 'DaugIPChi2'       : 16        ## unitless
                , 'DaugTrkChi2'      : 5         ## unitless
                , 'D0MassWin'        : 75        ## MeV
                , 'D0Pt'             : 1500      ## MeV
                , 'D0VtxChi2Ndof'    : 13        ## unitless
                , 'D0FDChi2'         : 49        ## unitless
                , 'D0BPVDira'        : 0.9999    ## unitless
                , 'D0IPChi2'         : 30        ## unit
                , 'SlowPiPt'         : 150       ## MeV
                , 'SlowPiTrkChi2'    : 5         ## unitless
                , 'DstarPt'          : 2200      ## MeV
                , 'DstarVtxChi2Ndof' : 13        ## unitless
                , 'DeltaM_Min'       : 130       ## MeV
                , 'DeltaM_Max'       : 155       ## MeV
                , 'DCS_WrongMass'    : 25        ## MeV (3 sigma veto)
                , 'KK_WrongMass'     : 25        ## MeV (3 sigma veto)
                , 'PiPi_WrongMass'   : 25        ## MeV (3 sigma veto)
                , 'Prescale'         : 0.45      ## unitless
                , 'Postscale'        : 1.0       ## unitless
                , 'Monitor'          : None      ## Monitoring?
                }
    }

# NoPID Inclusive Phi Lines
# A. Powell & S. Schleich
InclPhi={
    'BUILDERTYPE' : 'InclPhiConf',
    'CONFIG' : {'KaonPT'                : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.0015   # adimensional
                , 'LowPtPrescale'       : 0.0020   # adimensional
                , 'LDPrescale'          : 0.00002  # adimensional
                }
    }

# NoPID V0 Lines
# A. Powell
V0ForPID = {
    'BUILDERTYPE' : 'StrippingV0ForPIDConf',
    'CONFIG' : {'KS0LL_Prescale'   : 0.006,
                'KS0DD_Prescale'   : 0.006,
                'LamLL_Prescale'   : 0.025,
                'LamDD_Prescale'   : 0.022,
                'LamLLIsMUON_Prescale' : 0.72,
                'LamDDIsMUON_Prescale' : 0.47
                }
    }




# Bs2MuMuPhi
# P. Schaak 

Bs2MuMuPhi = {
    'BUILDERTYPE'          : 'Bs2MuMuPhiConf' ,
    'CONFIG'               : {
    'BsIPCHI2'             : 9.0           # dimensionless
    , 'BsLT'               : 0.0002        # ns
    , 'BsVertexCHI2'       : 40.0          # dimensionless
    , 'KaonPIDK'           : 0             # dimensionless
    , 'KaonMINIPCHI2'      : 9.0           # dimensionless
    , 'MuonMINIPCHI2'      : 9.0           # dimensionless
    }
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
    'B_Comb_MassLow'            : 4600.0,
    'B_Comb_MassHigh'           : 6000.0,
    'B_MassLow'                 : 4850.0,
    'B_MassHigh'                : 5780.0,
    'B_VertexCHI2'              : 6.0,
    'B_IPCHI2'                  : 16.0,
    'B_DIRA'                    : 0.014,
    'B_FlightCHI2'              : 121.0,
    'Dau_VertexCHI2'            : 12.0,
    'Dau_DIRA'                  : -0.9,
    'Kstar_Comb_MassLow'        : 550.0,
    'Kstar_Comb_MassHigh'       : 2200.0,
    'Kstar_MassLow'             : 600.0,
    'Kstar_MassHigh'            : 2000.0,
    'Kstar_MinIPCHI2'           : 4.0,
    'Kstar_FlightChi2'          : 25.0,
    'Dimu_FlightChi2'           : 81.0,
    'Track_CHI2nDOF'            : 5.0,
    'Hadron_MinIPCHI2'          : 9.0,
    'Muon_MinIPCHI2'            : 9.0,
    'Muon_IsMuon'               : False,
    'B_Dau_MaxIPCHI2'           : 9.0,
    'Kstar_Dau_MaxIPCHI2'       : 9.0,
    'Dimu_Dau_MaxIPCHI2'        : 9.0
    }
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
    }
    }

# B2XMuMu lines for Majorana searches
# S. Redford

B2XMuMuSS = {
    'BUILDERTYPE'   : 'B2XMuMuSSConf',
    'CONFIG'        : {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 500.  ,    #MeV
    'MuonMINIPCHI2' : 4     ,    #adminensional
    'PionP'         : 2000. ,    #MeV
    'PionPT'        : 500.  ,    #MeV
    'PionMINIPCHI2' : 4     ,    #adminensional
    'KaonP'         : 2000. ,    #MeV
    'KaonPT'        : 300.  ,    #MeV
    'KaonPIDK'      : -1.   ,    #adimensional
    'KaonMINIPCHI2' : 4     ,    #adminensional
    'DimuonMass'    : 250.  ,    #MeV
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
    }
    }




### Bd2eeKstar

Bd2eeKstar = {
    'BUILDERTYPE'             : 'Bd2eeKstarConf',
    'CONFIG'                  : {
    'LinePrescale'            :    1.   ,
    'LinePostscale'           :    1.   ,
    'ElectronPT'              :  350.   ,  # MeV
    'ElectronTrackCHI2pNDOF'  :  100.   ,
    'ElectronIPCHI2'          :    2.25 ,
    'ElectronPIDepi'          :   -2.   ,          
    'eeCombMinMass'           :    0.   ,  # MeV         
    'eeCombMaxMass'           : 1550.   ,  # MeV   
    'eeVertexCHI2'            :   15.   ,  
    'eeMinMass'               :   20.   ,  # MeV 
    'eeMaxMass'               : 1500.   ,  # MeV
    'eeFD'                    :    1.   ,  # mm
    'KaonPT'                  :  400.   ,  # MeV 
    'KaonP'                   : 3000.   ,  # MeV  
    'KaonTrackCHI2pNDOF'      :    5.   , 
    'KaonIPCHI2'              :    4.   , 
    'KaonPIDKpi'              :   -5.   , 
    'PionPT'                  :  300.   ,  # MeV
    'PionP'                   : 3000.   ,  # MeV 
    'PionTrackCHI2pNDOF'      :    5.   , 
    'PionIPCHI2'              :    4.   , 
    'PionPIDpiK'              :   10.   ,  # PIDpi-PIDK > -5, i.e., PIDK<5 
    'KstarComMassW'           :  200.   ,  # MeV  
    'KstarVertexCHI2'         :   25.   , 
    'KstarMassW'              :  130.   ,  # MeV
    'KstarIPCHI2'             :    1.   , 
    'KstarFDCHI2'             :    1.   , 
    'BComMassW'               : 1200.   ,  # MeV
    'BVertexCHI2'             :    9.   ,  # /ndf
    'BMassW'                  : 1000.   ,  # MeV  
    'BIPCHI2'                 :   64.   ,  # pointing
    'BFDCHI2'                 :    9.   , 
    'BDIRA'                   :    0.999, 
    'BIP'                     :    0.05    # mm
    }
    }

### Bu2LLK

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
    }
    }


### Bs2MuMuLines

Bs2MuMuLines = {
    'BUILDERTYPE' : 'Bs2MuMuLinesConf',
    'CONFIG'      : {
    'DefaultLinePrescale'    : 1,
    'DefaultLinePostscale'   : 1,
    'Bs2mmWideLinePrescale'  : 1,
    'Bs2mmWideLinePostscale' : 1,
    'LooseLinePrescale'      : 0.05,
    'LooseLinePostscale'     : 1,
    'JPsiLinePrescale'       : 1,
    'JPsiLinePostscale'      : 1,
    'JPsiLooseLinePrescale'  : 0.1,
    'JPsiLooseLinePostscale' : 1,
    'JPsiPromptLinePrescale' : 0.01,
    'JPsiPromptLinePostscale': 1,
    'MuIPChi2_loose'        :  9,
    'MuTrChi2_loose'        : 10,
    'BIPChi2_loose'         : 64,
    'BFDChi2_loose'         : 100
    }
    }

### B2MuMuMuMuLines

B2MuMuMuMuLines = {
    'BUILDERTYPE' : 'B2MuMuMuMuLinesConf',
    'CONFIG'      : {
    'B2MuMuMuMuLinePrescale'    : 1,
    'B2MuMuMuMuLinePostscale'   : 1,
    }
    }

### Stripping TriMuons

TriMuon = {
    'BUILDERTYPE'  : 'StrippingTriMuonsConf',
    'CONFIG'       : {
    'GoodMuons'    : " ( PT > 300 * MeV ) & ( TRCHI2DOF < 5  ) & ( BPVIPCHI2 () >  6 ) " ,
    'GoodMuonsForBc' : " ( BPVIPCHI2 () >  9 ) " ,
    'TightMuons'   : " ( PT > 1.5 * GeV ) & ( BPVIPCHI2 () > 16 ) " ,
    'HLT'          : " HLT_PASS_RE('Hlt.*(MicroBias|Muon|MuMu|DiMu).*Decision') " , 
    '3mu-Prescale' : 1.00 , 
    'Bc-Prescale'  : 1.00 , 
    'Tau-Prescale' : 1.00 , 
    'Preambulo'    : [
    'chi2vx = VFASPF(VCHI2) '                               ,
    "from GaudiKernel.PhysicalConstants import c_light"     ,
    "ctau    =                    BPVLTIME ( ) * c_light "  ,
    "ctauBc  = PDGM('B_c+') / M * BPVLTIME ( ) * c_light "  
    ]
    }
    }

