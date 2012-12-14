BetaS = {         
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiXforBeta_sConf',
    'CONFIG'    : {
                         'TRCHI2DOF'             :       5
                 ,       'BPVLTIME'              :       0.2
                 ,       'DaughterPT'            :       1000
                 ,       'VCHI2PDOF'             :       10
                 ,       'Jpsi2MuMuPrescale'     :       0.01
                 ,       'Bu2JpsiKPrescale'      :       0.1  #0.05
                 ,       'Bd2JpsiKstarPrescale'  :       0.1   #0.03
                 ,       'Bd2JpsiKsPrescale'     :       1.0   #0.3
                 ,       'Bs2JpsiPhiPrescale'    :       0.5   #0.15
                 #,       'Bs2JpsiEtaPrescale'    :       0.2 
                         },
    'STREAMS' : [ 'Dimuon' ]
    }

# Neus (update Stripping20)
BetaSPsi2S = {
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'   : 'B2Psi2SXConf',
    'CONFIG'    : {'PionsTRCHI2DOF': 5,
#                 'Psi2SJpsiMIPCHI2DV' : 9 ,
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
                 'PhiPT':1000, #before 500 #ara 1000
                 'PhiVFASPF':16,
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,#
                 'KstMassDown':  826,
                 'KstMassUp': 966,
                 'KstAPT':1500, #before 500
                 'KstVFASPF':16,
                 'KstTRCHI2DOF':4,
                 'KstPIDK': 0, #before -2
                 'KstPIDpi': 10, #new 10
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.1, #0.1 #before 0.5 #avui 0.2 
                 'incl_LinePostscale':1,
                 'BPVLTIME_detatched':0.15,
                 'BsMassCutDownPre':5050,#before 5000
                 'BsMassCutUpPre':5600,#before 5650
                 'BsMassCutDownPost':5150,#before 5100
                 'BsMassCutUpPost':5500, #before 5550
                 'BsVCHI2PDOF':10,
                 'sig_LinePrescale': 1,
                 'sig_LinePostscale': 1,
                 'ChKPT': 500 ,
                 'K_LinePrescale': 1, #0.5
                 'K_LinePostscale': 1,
                 'Kstar_LinePrescale': 1, #0.5 #before 1 #ara 0.5 
                 'Kstar_LinePostscale': 1,
                 'MINTREEPT2' : 1000,
                 'BKsVCHI2PDOF': 10,
                 'Ks_LinePrescale': 1,
                 'Ks_LinePostscale':1
                },
    'STREAMS' : [ 'Dimuon' ]
    }

# Neus (update Stripping20)
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
                 'PhiPT':1000, #before 500
                 'PhiVFASPF':16,
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,
                 'KstMassDown':  826,
                 'KstMassUp': 966,
                 'KstAPT':1200, #before 500
                 'KstVFASPF':16,
                 'KstTRCHI2DOF':4,
                 'KstPIDK': 0, # before -2
                 'KstPIDpi': 10, #new
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.05, #before 0.1
                 'incl_LinePostscale':1,
                 'BPVLTIME_detatched':0.2, #before 0.15
                 'incl_DetatchedLinePrescale':0.2, #before 0.5
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
#                 'sig_UnbiasedLinePrescale':1,
#                 'sig_UnbiasedLinePostscale':1,
                 'ChKPT':500,
                 'K_PrescaledLinePrescale':0.1, #before 0.3 #before 0.2
                 'K_PrescaledLinePostscale':1,
                 'K_DetatchedLinePrescale':1, #before 0.1
                 'K_DetatchedLinePostscale':1,
#                 'K_UnbiasedLinePrescale':0.7, #before 1.
#                 'K_UnbiasedLinePostscale':1,
                 'KstarPT': 2,
                 'Kstar_PrescaledLinePrescale':0.07, #before 0.1
                 'Kstar_PrescaledLinePostscale':1,
                 'Kstar_DetatchedLinePrescale':1, #before 1
                 'Kstar_DetatchedLinePostscale':1,
#                 'Kstar_UnbiasedLinePrescale':0.6, #before 1
#                 'Kstar_UnbiasedLinePostscale':1,
                 'BKsVCHI2PDOF':10,
                 'Ks_PrescaledLinePrescale':1,
                 'Ks_PrescaledLinePostscale':1,
                 'Ks_DetatchedLinePrescale':1,
                 'Ks_DetatchedLinePostscale':1
#                 'Ks_UnbiasedLinePrescale':1,
#                 'Ks_UnbiasedLinePostscale':1
                },
    'STREAMS' : [ 'Dimuon' ]
    }


# New line since Stripping17b (M reid) (update stripping20)
B2JpsiKshh = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2JpsiKShhConf',
    'CONFIG'    :  {
		  'Trk_Chi2'                 : 4.0,
                  'Jpsi_MassWindow'          : 80.0, # 80 by default is the StdMassConstrainedJpsi2MuMu mass window
                  'KS_DD_MassWindow'         : 30.0,
                  'KS_DD_VtxChi2'            : 18.0,
                  'KS_DD_DLS'                : 5.0,
                  'KS_LL_MassWindow'         : 30.0,
                  'KS_LL_VtxChi2'            : 20.0,
                  'KS_LL_DLS'                : 5.0,
                  'Bs_Mlow'                  : 520.0,
                  'Bs_Mhigh'                 : 750.0,
                  'BsDaug_DD_maxDocaChi2'    : 5.0,
                  'BsDaug_LL_maxDocaChi2'    : 5.0,
                  'Bs_PT'                    : 600.0,
                  'Bs_VtxChi2'               : 9.0,
                  'Bs_Dira'                  : 0.99992,
                  'Bs_LL_IPChi2PV'           : 75.0,
                  'Bs_DD_IPChi2PV'           : 75.0,
                  'Prescale'                 : 1.0,
                  'Prescale_SameSign'        : 1.0,
                  'Postscale'                : 1.0
    },
    'STREAMS'  : [ 'Dimuon' ]
}


#(Katarzyna) (new since Stripping20)
B2CharmoniumX_6H = {
    'WGs'       :       ['B2CC'],
    'BUILDERTYPE'       :       'B2CharmoniumX_6HAllLinesConf',
    'CONFIG'    :       {
		'Prescale'               : 1.0 ,
		'Postscale'              : 1.0 ,
		'TRCHI2'                 : 3,
		'DOCA'                   : 0.1,
		#K parameters
		'K_PT'                   : 750.,
		'K_IPCHI2'               : 4,
		'K_PIDK'                 : -3.,
		#pi parameters
		'pi_PT'                  : 750.,
		'pi_IPCHI2'              : 4.,  
		'pi_PIDK'                : 3.,  
		#'max' daughters
		'PT_maxetacdaughter'     : 1400.,
		'PT_maxchicdaughter'     : 1300.,
		'IPCHI2_maxdaughter'     : 9.,
		#phi parameters
		'phi_DIRA'               : 0.95,
		'phi_VDZ'                : 0.,
		'phi_PT'                 : 1900.,
		'phi_IPCHI2'             : 6.,
		'phi_VCHI2'              : 7.,  
		#Kst parameters
		'Kst_DIRA'               : 0.93,
		'Kst_VDZ'                : 0.,
		'Kst_PT'                 : 1600.,
		'Kst_IPCHI2'             : 4.,
		'Kst_VCHI2'              : 12.,
		#etac parameters
		'etac_DIRA'              : 0.9,
		'etac_VDZ'               : 0.,   
		'etac_IPCHI2'            : 4.,
		'etac_VCHI2'             : 9.,
		'etac_sumPT'             : 4000,
		#chic/etac parameters
		'chic_DIRA'              : 0.9,
		'chic_VDZ'               : 0.,   
		'chic_IPCHI2'            : 4.,
		'chic_VCHI2'             : 9.,
		'chic_sumPT'             : 3600,
		#Bs parameters
		'Bs_VDZ'                 : 0.,
		'Bs_DIRA'                : 0.9,
		'Bs_VCHI2'               : 25.
    },
    'STREAMS'  : [ 'Bhadron' ]
}

