BetaS = {         
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiXforBeta_sConf',
    'CONFIG'    : {
                         'TRCHI2DOF'             :       5
                 ,       'BPVLTIME'              :       0.2
                 ,       'DaughterPT'            :       1000
                 ,       'VCHI2PDOF'             :       10
                 ,       'Jpsi2MuMuPrescale'     :       0.01
                 ,       'Bu2JpsiKPrescale'      :       0.05
                 ,       'Bd2JpsiKstarPrescale'  :       0.03
                 ,       'Bd2JpsiKsPrescale'     :       0.3
                 ,       'Bs2JpsiPhiPrescale'    :       0.15
                 ,       'Bs2JpsiEtaPrescale'    :       0.2
                         },
    'STREAMS' : [ 'Dimuon' ]
    }

# Neus
BetaSPsi2S = {
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'   : 'B2Psi2SXConf',
    'CONFIG'    : {'PionsTRCHI2DOF': 5,
                'Psi2SPiMIPCHI2DV':9 , #before 4
                'Psi2SAM23down': 400,
                'Psi2SAM23up': 800,
                'Psi2SAPT': 500,
                'Psi2SADAMASS': 30,
                'Psi2SVFASPF': 16,
                'PIDpi': 10, # new 10
                'ChKTRCHI2DOF':5,
                'ChKPID': 0 ,# before -2
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
                'KstPIDK': 0, #before -2
                'KstPIDpi': 10, #new 10
                'KsVFASPF':20,
                'KsBPVDLS':5,
                'incl_LinePrescale':0.5, #0.1
                'incl_LinePostscale':1,
                'BsMassCutDownPre':5000,
                'BsMassCutUpPre':5650,
                'BsMassCutDownPost':5100,
                'BsMassCutUpPost':5550,
                'BsVCHI2PDOF':10,
                'sig_LinePrescale': 1,
                'sig_LinePostscale': 1,
                'ChKPT': 500 ,
                'K_LinePrescale': 1, #0.5
                'K_LinePostscale': 1,
                'Kstar_LinePrescale': 1, #0.5
                'Kstar_LinePostscale': 1,
                'MINTREEPT2' : 1000,
                'BKsVCHI2PDOF': 10,
                'Ks_LinePrescale': 1,
                'Ks_LinePostscale':1
                },
    'STREAMS' : [ 'Dimuon' ]
    }

# Neus
BetaSPsi2SMuMu = {
    'WGs'           : ['B2CC'],
    'BUILDERTYPE'   : 'Bs2Psi2SPhiMuMuConf',
    'CONFIG'        : {'muPID':0.,
                'Psi2SMassWin':60.,
                'Psi2SADOCACHI2CUT':30.,
                'Psi2SVFASPF':16,
                'ChKTRCHI2DOF':5,
                'ChKPID':0, #before -2
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
                'KstPIDK': 0, # before -2
                'KstPIDpi': 10, #new
                'KsVFASPF':20,
                'KsBPVDLS':5,
                'incl_LinePrescale':0.1,
                'incl_LinePostscale':1,
                'BPVLTIME_detatched':0.15,
                'incl_DetatchedLinePrescale':0.5,
                'incl_DetatchedLinePostscale':1,
                'BsMassCutDownPre':5050, #before 5000
                'BsMassCutUpPre':5600,  #before 5650
                'BsMassCutDownPost':5150, #before 5100
                'BsMassCutUpPost':5500,  #before 5550
                'BsVCHI2PDOF':10,
                'sig_PrescaledLinePrescale':0.5, #before 1.
                'sig_PrescaledLinePostscale':1,
                'sig_DetatchedLinePrescale':1,
                'sig_DetatchedLinePostscale':1,
                'BPVLTIME_unbiased': None,
                'MINTREEPT':1000, #before 1000
                'MINTREEPT2':1500, #before 1000
                'ChKPT':500,
                'K_PrescaledLinePrescale':0.2, #before 0.3
                'K_PrescaledLinePostscale':1,
                'K_DetatchedLinePrescale':1,
                'K_DetatchedLinePostscale':1,
                'KstarPT': 2,
                'Kstar_PrescaledLinePrescale':0.1,
                'Kstar_PrescaledLinePostscale':1,
                'Kstar_DetatchedLinePrescale':1,
                'Kstar_DetatchedLinePostscale':1,
                'BKsVCHI2PDOF':10,
                'Ks_PrescaledLinePrescale':1,
                'Ks_PrescaledLinePostscale':1,
                'Ks_DetatchedLinePrescale':1,
                'Ks_DetatchedLinePostscale':1
                },
    'STREAMS' : [ 'Dimuon' ]
    }

# Bs2Jpsi(ee)Phi (Jibo and Artur)
BetaSBs2JpsieePhi = {
    'WGs'		: ['B2CC'],
    'BUILDERTYPE' 	: 'Bs2JpsieePhiConf',
    'CONFIG' 	: {
                 'ElectronPTLoose'            :   500.    # MeV
               , 'ElectronPIDLoose'           :     0.    # adimensional
               , 'ElectronTrackCHI2pDOFLoose' :     5.    # adimensional
               , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
               , 'JpsiMassMinLoose'           :  2500.    # MeV
               , 'JpsiMassMaxLoose'           :  3300.    # MeV
               , 'KaonTrackCHI2pDOFLoose'     :     5.    # adimensional
               , 'PhiPTLoose'                 :  1000.    # MeV
               , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
               , 'PhiMassMinLoose'            :   990.    # MeV
               , 'PhiMassMaxLoose'            :  1050.    # MeV
               , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
               , 'BsMassMinLoose'             :  4500.    # MeV
               , 'BsMassMaxLoose'             :  6000.    # MeV
               , 'LifetimeCut'                : " & (BPVLTIME()>0.3*ps)"
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
               },
    'STREAMS' : [ 'Radiative' ]
    }

B2EtacKst = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2EtacKstConf',
    'CONFIG'    :       {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 600.,
    'K_TRCHI2'                : 3.7,
    'K_IPCHI2'                : 6.,
    #pi parameters
    'pi_PT'                   : 600., 
    'pi_TRCHI2'               : 3.7,
    'pi_IPCHI2'               : 6.,
    #kst parameters
    'kst_VDZ'                 : 0., 
    'kst_PT'                  : 1000., 
    'kst_IPCHI2'              : 6.,
    'kst_VCHI2_VDOF'          : 16.,
    #rho (for eta_c decay) parameters
    'rho_etac_VDZ'            : 0.,
    'rho_etac_VCHI2_VDOF'     : 16.,
    #phi (for eta_c decay) parameters
    'phi_etac_VDZ'            : 0.,
    'phi_etac_VCHI2_VDOF'     : 16.,
    #eta_c parameters
    'etac_VDZ'                : 0.,    
    'etac_PT'                 : 600.,
    'etac_IPCHI2'             : 6., 
    'etac_VCHI2_VDOF'         : 16.,
    #B parameters
    'B_VDZ'                  : 0.,
    'B_DIRA'                 : 0.999,
    'B_IPCHI2'               : 9.,
    'B_VCHI2_VDOF'           : 16.
    },
    'STREAMS'  : [ 'Bhadron' ]
    }

# New line since Stripping17b
B2ChicK = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2ChicKConf',
    'CONFIG'    :       {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #K parameters
    'K_PT'                   : 600.,
    'K_TRCHI2'               : 3.7,
    'K_IPCHI2'               : 6.,
    #pi parameters
    'pi_PT'                  : 600., 
    'pi_TRCHI2'              : 3.7,
    'pi_IPCHI2'              : 6.,
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
    #B parameters
    'B_VDZ'                 : 0.,
    'B_DIRA'                : 0.999,
    'B_IPCHI2'              : 9.,
    'B_VCHI2_VDOF'          : 16.
    },
    'STREAMS'  : [ 'Bhadron' ]
    }

# New line since Stripping17b
B2JpsiKshh = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2JpsiKShhConf',
    'CONFIG'    :  {
	'Trk_Chi2'                 : 4.0,
	'Jpsi_MassWindow'          : 80.0, # 80 by default is the StdMassConstrainedJpsi2MuMu mass window
        'KS_DD_MassWindow'         : 30.0,
        'KS_DD_VtxChi2'            : 20.0,
        'KS_DD_DLS'                : 5.0,
        'KS_LL_MassWindow'         : 30.0,
        'KS_LL_VtxChi2'            : 20.0,
        'KS_LL_DLS'                : 5.0,
        'Bs_Mlow'                  : 520.0,
        'Bs_Mhigh'                 : 500.0,
        'BsDaug_DD_maxDocaChi2'    : 5.0,
        'BsDaug_LL_maxDocaChi2'    : 5.0,
        'Bs_VtxChi2'               : 10.0,
        'Bs_Dira'                  : 0.9999,
        'Prescale'                 : 1.0,
        'Postscale'                : 1.0
    },
    'STREAMS'  : [ 'Dimuon' ]
}

# Bs2EtacPhi/Bs2ChicPhi (Katarzyna)
BetaSBs2EtacPhi = {
    'WGs'	:	['B2CC'],
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
        },
    'STREAMS'  : [ 'Bhadron' ]
    }

BetaSBs2ChicPhi_Chic2KKPiPi = {
    'WGs'	:	['B2CC'],
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
    },
    'STREAMS' : [ 'Bhadron' ] 
    }

BetaSBs2ChicPhi_Chic2PiPiPiPi = {
    'WGs'	:	['B2CC'],
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
    },
    'STREAMS' : [ 'Bhadron' ]
    }

