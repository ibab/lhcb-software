# Line config dictionaries for B2CC WG 2011 config dictionary

BetaS = {         
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiXforBeta_sConf',
    'CONFIG'    : {
                         'TRCHI2DOF'                 :       5
                 ,       'BPVLTIME'                  :       0.2
                 ,       'DaughterPT'                :       1000
                 ,       'VCHI2PDOF'                 :       10
                 ,       'Jpsi2MuMuPrescale'         :       0.02   #0.01
		 ,	 'Jpsi2MuMuDetachedPrescale' :       1.0
                 ,       'Bu2JpsiKPrescale'          :       0.08  #0.05
                 ,       'Bd2JpsiKstarPrescale'      :       0.08   #0.03
                 ,       'Bd2JpsiKsPrescale'         :       1.0   #0.3
                 ,       'Bs2JpsiPhiPrescale'        :       0.15   #0.5 #0.15
		 ,       'Bs2Jpsif0Prescale'         :       0.07   
                 ,       'Bs2JpsiEtaPrescale'        :       0.22 
		 ,       'Bs2JpsiEtapPrescale'       :       0.08 
		 ,       'Bs2JpsiPi0Prescale'        :       0.2
		 ,       'Bs2JpsiRho0Prescale'       :       0.05 
                         },
	'STREAMS' : { 
		'Dimuon' : [
		'StrippingBetaSJpsi2MuMuLine',
 		'StrippingBetaSJpsi2MuMuDetachedLine', 
 		'StrippingBetaSBu2JpsiKPrescaledLine',
 		'StrippingBetaSBs2JpsiPhiPrescaledLine', 
 		'StrippingBetaSBs2JpsiPhiDetachedLine',
 		'StrippingBetaSBd2JpsiKstarPrescaledLine', 
 		'StrippingBetaSBd2JpsiKstarDetachedLine',
 		'StrippingBetaSBd2JpsiKsPrescaledLine',
 		'StrippingBetaSBs2Jpsif0NoIPPrescaledLine',
 		'StrippingBetaSBs2Jpsif0NoIPDetachedLine',
 		'StrippingBetaSBs2Jpsi4PiLine',
 		'StrippingBetaSBs2JpsiEtaPrescaledLine',
 		'StrippingBetaSBs2JpsiEtaDetachedLine',
 		'StrippingBetaSBs2JpsiEtapPrescaledLine',
 		'StrippingBetaSBs2JpsiEtapDetachedLine',
 		'StrippingBetaSBd2JpsiPi0PrescaledLine',
 		'StrippingBetaSBd2JpsiRho0PrescaledLine',
 		'StrippingBetaSBd2JpsiRho0DetachedLine'
		]
	}	
    }


# Neus 
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
                 'PhiVFASPF':25,     ##ATTENTION before 16
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
                 'BsMassCutDownPre':5000,     #ATTENTION before 5050
                 'BsMassCutUpPre':5650,       #ATTENTION before 5600
                 'BsMassCutDownPost':5150,    #before 5100
                 'BsMassCutUpPost':5550,      #ATTENTION before 5550
                 'BsVCHI2PDOF':20,      ##ATTENTION before 10
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
	'STREAMS' : { 
		'Dimuon' : [
 		'StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine'
		]
	}	
    }

#Neus 
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
                 'PhiVFASPF':25,        ##ATTENTION before 16
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
                 'BsMassCutDownPre':5000, #before 5000
                 'BsMassCutUpPre':5650,  #before 5650
                 'BsMassCutDownPost':5150, #before 5100
                 'BsMassCutUpPost':5550,  #before 5550
                 'BsVCHI2PDOF':20,         ##ATTENTION before 10
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
	'STREAMS' : { 
		'Dimuon' : [
 		'StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetatchedLine',
 		'StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine'
		]
	}	
    }





