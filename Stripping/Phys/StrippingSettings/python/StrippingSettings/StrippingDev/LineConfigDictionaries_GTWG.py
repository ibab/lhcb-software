"""
Line config dictionaries for the Gamma from Trees WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *

Beauty2Charm = {
  'BUILDERTYPE' : 'Beauty2CharmConf',
  'CONFIG' : {
    "ALL" : { # Cuts made on all charged input particles in all lines
    'TRCHI2DOF_MAX' : 4,
    'PT_MIN'        : '100*MeV',
    'P_MIN'         : '1000*MeV',
    'MIPCHI2DV_MIN' : 4
    },
    "KS0" : { # Cuts made on all K shorts
    'PT_MIN'        : '250*MeV',
    'MIPCHI2DV_MIN' : 4,
    'MM_MIN'        : '467.*MeV',
    'MM_MAX'        : '527.*MeV'
    },
    "Pi0" : { # Cuts made on all pi0's
    'PT_MIN'        : '500*MeV',
    'P_MIN'         : '2000*MeV',
    'CHILDCL1_MIN'  : 0.25,
    'CHILDCL2_MIN'  : 0.25
    },
    "D2X" : { # Cuts made on all D's and Lc's used in all lines 
    'ASUMPT_MIN'    : '1800*MeV',
    'AMAXDOCA_MAX'  : '1.0*mm',
    'VCHI2DOF_MAX'  : 10,
    'BPVVDCHI2_MIN' : 36,
    'BPVDIRA_MIN'   : 0, 
    'MASS_WINDOW'   : '100*MeV'
    },
    "B2X" : { # Cuts made on all B's and Lb's used in all lines
    'AMAXDOCA_MAX'  : '1.0*mm',
    'SUMPT_MIN'     : '5000*MeV',
    'VCHI2DOF_MAX'  : 10,
    'BPVIPCHI2_MAX' : 25,
    'BPVLTIME_MIN'  : '0.3*ps',
    'BPVDIRA_MIN'   : 0.999,
    'AM_MIN'        : '4750*MeV', # Lb->X sets this to 5200*MeV
    'AM_MAX'        : '7000*MeV',
    'B2CBBDT_MIN'   : 0.05,
    'NOIP_BPVVDCHI2_MIN' : 0
    },
    "Dstar" : { # Cuts made on all D*'s used in all lines 
    'AMAXDOCA_MAX'  : '1.0*mm',
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
    'PTMIN1'       : '300*MeV'
    },
    'PID' : {
    'P'  : {'PIDp_MIN' : -10},
    'PI' : {'PIDK_MAX' : 20},
    'K'  : {'PIDK_MIN' : -10}
    },
    "Prescales" : { # Prescales for individual lines
    # Defaults are defined in, eg, Beauty2Charm_B2DXBuilder.py.  Put the full
    # line name here to override. E.g. 'B2D0HD2HHBeauty2CharmTOSLine':0.5.
    #'B2DDKBeauty2CharmLine' : 0.1,
    #'B02D0DKBeauty2CharmLine' : 0.1
    },
    'GECNTrkMax'   : 500
    }, 
  'STREAMS' : [ 'Bhadron' ],
  'WGs' : [ 'GammaFromTrees' ]
}


DstarD02Kpipi0 = {
    'BUILDERTYPE' : 'DstarD02Kpipi0Conf',
    'CONFIG' : {
  	"RSPrescale" : 0.083, 
  	"WSPrescale" : 0.0083
	},
    'STREAMS' : [ 'CharmCompleteEvent' ],
    'WGs' : [ 'GammaFromTrees' ]
}

UnbiasedB2DPi = {
    'BUILDERTYPE' : 'UnbiasedB2DPiConf',
    'CONFIG' : {
    	    "D2hhhCuts" : {                  # D->hhh cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombDMass"    : 110.,         # mass window (MeV) for a combination
    	      "DMass"        : 100.,         # mass window (MeV) after vertex fit
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 36.,          # D flight distance chi2
	      "PtMin"        : 1500.,        # Minimum Pt of the D (MeV)
	      "MaxDauMIPChi2": 40.,          # IP chi2 for at least one track
	      "DIRAMin"	     : 0.9,          # minimum D DIRA
	      "DOCAMax"      : 0.6,          # maximum DOCA of track combinations (mm)
	      "IPChi2Min"    : 0.,           # Minimum IP chi2
    	    }, 
	    "UnbiasedBCuts" : {                     # Lifetime-unbiased B->D(hhh)h selection
    	      "BachelorChi2Max"    : 4.,            # maximum track chi2
    	      "BachelorPtMin"      : 500.,          # track Pt cut (MeV)
    	      "BachelorPMin"       : 5000.,         # track P cut (MeV)
    	      "BachelorMIPChi2Min" : 0.,            # minimum IP chi2 for a track
    	      "DMass" 		   : 100.,          # Mass window for a D combination (MeV)
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "IPChi2Max"          : 250.,          # chi2 of B impact parameter to the related PV
	      "LTMin"              : -1000.,        # Chi2 of B vertex separation from the related PV
	      "DIRAMin"            : -1.,           # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a B combination (MeV)
	    }, 
	    "Prescale"         : 0.05,              # Prescales for individual lines
	    "CheckPV"	       : True,              # PV requirement
	    "MaxTracksInEvent" : 500                # GECs for individual lines (max. number of long tracks)
	},
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}


Bu2D0h_D02KShh_NoPID = {
    'BUILDERTYPE' : 'StrippingBu2D0h_D02KShh_NoPIDConf',
    'CONFIG' : {#almost offline sel cuts 8/2/2011 (D daughter pi/K IPchi2s looser, Ks DD daughter pi PID cut looser)
                        'PrescaleLL'    : 1.0 
                        ,  'PostscaleLL'   : 1.0
                        #B cuts, LL seln
                       ,  'BFlightCHI2_LL'        : 169. 
                       ,  'BDIRA_LL'           : 0.99999     
                       ,  'BIPCHI2_LL'         : 9.         
                       ,  'BVertexCHI2_LL'     : 6.25
                       #Bachelor K cuts, LL seln
                       ,  'BachIPCHI2_LL'     : 16.
                       ,  'BachPt_LL'         : 1.0  
                       #D0 and D0 daughter pi cuts, LL seln
                       ,  'DVertexCHI2_LL'     : 4.     
                       ,  'DdaughterIPCHI2_LL' : 16.
                       #KS and KS daughter pi cuts, LL seln
                       ,  'KSFlightCHI2_LL'    : 4.
                       ,  'KSVertexCHI2_LL'    : 16.
                       ,  'KSdaughterPiIPCHI2_LL' : 9.
                       ,  'PrescaleDD'    : 1.0 
                       ,  'PostscaleDD'   : 1.0
                       #B cuts, DD seln
                       ,  'BFlightCHI2_DD'     : 100.
                       ,  'BDIRA_DD'           : 0.9999    
                       ,  'BIPCHI2_DD'         : 16.         
                       ,  'BVertexCHI2_DD'     : 9.
                       #Bachelor K cuts, DD seln
                       ,  'BachIPCHI2_DD'     : 6.25
                       ,  'BachPt_DD'         : 0.5 
                       #D0 and D0 daughter pi cuts, DD seln
                       ,  'DVertexCHI2_DD'     : 16.        
                       ,  'DdaughterIPCHI2_DD' : 9.
                        #KS and KS daughter pi cuts, DD seln
                       ,  'KSFlightCHI2_DD'    : 4.
                       ,  'KSVertexCHI2_DD'    : 16.
                       ,  'KSdaughterPiIPCHI2_DD' : 6.25
 	}, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}

Bu2D0h_D02KShh_NoPID_WS = {
    'BUILDERTYPE' : 'StrippingBu2D0h_D02KShh_NoPID_WSConf',
    'CONFIG' : {#almost offline sel cuts 8/2/2011 (D daughter pi/K IPchi2s looser)
                        'PrescaleLL'    : 0.15 
                        ,  'PostscaleLL'   : 1.0
                        #B cuts, LL seln
                       ,  'BFlightCHI2_LL'        : 169. 
                       ,  'BDIRA_LL'           : 0.99999     
                       ,  'BIPCHI2_LL'         : 9.         
                       ,  'BVertexCHI2_LL'     : 6.25
                       #Bachelor K cuts, LL seln
                       ,  'BachIPCHI2_LL'     : 16.
                       ,  'BachPt_LL'         : 1.0  
                       #D0 and D0 daughter pi cuts, LL seln
                       ,  'DVertexCHI2_LL'     : 4.     
                       ,  'DdaughterIPCHI2_LL' : 16.
                       #KS and KS daughter pi cuts, LL seln
                       ,  'KSFlightCHI2_LL'    : 4.
                       ,  'KSVertexCHI2_LL'    : 16.
                       ,  'KSdaughterPiIPCHI2_LL' : 9.
                       ,  'PrescaleDD'    : 0.15 
                       ,  'PostscaleDD'   : 1.0
                       #B cuts, DD seln
                       ,  'BFlightCHI2_DD'     : 100.
                       ,  'BDIRA_DD'           : 0.9999    
                       ,  'BIPCHI2_DD'         : 16.         
                       ,  'BVertexCHI2_DD'     : 9.
                       #Bachelor K cuts, DD seln
                       ,  'BachIPCHI2_DD'     : 6.25
                       ,  'BachPt_DD'         : 0.5 
                       #D0 and D0 daughter pi cuts, DD seln
                       ,  'DVertexCHI2_DD'     : 16.        
                       ,  'DdaughterIPCHI2_DD' : 9.
                        #KS and KS daughter pi cuts, DD seln
                       ,  'KSFlightCHI2_DD'    : 4.
                       ,  'KSVertexCHI2_DD'    : 16.
                       ,  'KSdaughterPiIPCHI2_DD' : 6.25
 		}, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
}


B2twobody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':              2,
                  'prescale':           0.5, 
                  'MinBMass':        4700.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          5000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MaxMass':         6000.0,
                  'MaxNtrk':              8,
                  'MinNvc':               2,
                  'doPi':              True,
                  'doK':               True,
                  'dop':               True,
                  'doKs':              True,
                  'doLm':              True,
                  'doDz':              True,
                  'doDp':              True,
                  'doDs':              True,
                  'doLc':              True,
                  'doPh':              True,
                  'doKS':             False,
                  'doJp':              True,
                  'doDS':              True,
                  'MinPiPt':         1000.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          4.0,
                  'MinPiPIDK':          2.0,
                  'MinPiPIDp':          2.0,
                  'MinKPt':          1000.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           4.0,
                  'MinKPIDPi':          2.0,
                  'MinKPIDp':           2.0,
                  'MinpPt':          1000.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           4.0,
                  'MinpPIDPi':          2.0,
                  'MinpPIDK':           2.0,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1000.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':    5.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1000.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':    5.0,
                  'MaxLmPiTrkChi2':     5.0,
                  'MaxDzDeltaM':       40.0,
                  'MinDzPt':         1000.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPiPIDK':       -5.0,
                  'MinDzKPIDPi':       -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1000.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        9.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpKPIDPi':       -5.0,
                  'MinDpPiPIDK':       -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1000.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        9.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsKmPIDPi':      -5.0,
                  'MinDsKpPIDPi':       0.0,
                  'MinDsPiPIDK':       -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1000.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        9.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcKPIDPi':       -5.0,
                  'MinLcpPIDPi':        0.0,
                  'MinLcpPIDK':         0.0,
                  'MinLcPiPIDK':       -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1000.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       300.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':    5.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MinKSKPIDPi':        0.0,
                  'MinKSPiPIDK':        0.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':    5.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1000.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':        500.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
    }

B2threebody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':              3,
                  'prescale':           0.5, 
                  'MinBMass':        4700.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          5000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MaxMass':         6000.0,
                  'MaxNtrk':              8,
                  'MinNvc':               2,
                  'doPi':              True,
                  'doK':               True,
                  'dop':               True,
                  'doKs':              True,
                  'doLm':              True,
                  'doDz':              True,
                  'doDp':              True,
                  'doDs':              True,
                  'doLc':              True,
                  'doPh':              True,
                  'doKS':             False,
                  'doJp':              True,
                  'doDS':              True,
                  'MinPiPt':         1000.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          4.0,
                  'MinPiPIDK':          2.0,
                  'MinPiPIDp':          2.0,
                  'MinKPt':          1000.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           4.0,
                  'MinKPIDPi':          2.0,
                  'MinKPIDp':           2.0,
                  'MinpPt':          1000.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           4.0,
                  'MinpPIDPi':          2.0,
                  'MinpPIDK':           2.0,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1000.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':    5.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1000.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':    5.0,
                  'MaxLmPiTrkChi2':     5.0,
                  'MaxDzDeltaM':       40.0,
                  'MinDzPt':         1000.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPiPIDK':       -5.0,
                  'MinDzKPIDPi':       -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1000.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        9.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpKPIDPi':       -5.0,
                  'MinDpPiPIDK':       -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1000.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        9.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsKmPIDPi':      -5.0,
                  'MinDsKpPIDPi':       0.0,
                  'MinDsPiPIDK':       -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1000.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        9.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcKPIDPi':       -5.0,
                  'MinLcpPIDPi':        0.0,
                  'MinLcpPIDK':         0.0,
                  'MinLcPiPIDK':       -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1000.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       300.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':    5.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MinKSKPIDPi':        0.0,
                  'MinKSPiPIDK':        0.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':    5.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1000.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':        500.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
    }

B2fourbody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':              4,
                  'prescale':           0.5, 
                  'MinBMass':        4700.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          5000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MaxMass':         6000.0,
                  'MaxNtrk':              8,
                  'MinNvc':               2,
                  'doPi':              True,
                  'doK':               True,
                  'dop':               True,
                  'doKs':              True,
                  'doLm':              True,
                  'doDz':              True,
                  'doDp':              True,
                  'doDs':              True,
                  'doLc':              True,
                  'doPh':              True,
                  'doKS':             False,
                  'doJp':              True,
                  'doDS':              True,
                  'MinPiPt':         1000.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          4.0,
                  'MinPiPIDK':          2.0,
                  'MinPiPIDp':          2.0,
                  'MinKPt':          1000.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           4.0,
                  'MinKPIDPi':          2.0,
                  'MinKPIDp':           2.0,
                  'MinpPt':          1000.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           4.0,
                  'MinpPIDPi':          2.0,
                  'MinpPIDK':           2.0,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1000.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':    5.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1000.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':    5.0,
                  'MaxLmPiTrkChi2':     5.0,
                  'MaxDzDeltaM':       40.0,
                  'MinDzPt':         1000.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPiPIDK':       -5.0,
                  'MinDzKPIDPi':       -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1000.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        9.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpKPIDPi':       -5.0,
                  'MinDpPiPIDK':       -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1000.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        9.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsKmPIDPi':      -5.0,
                  'MinDsKpPIDPi':       0.0,
                  'MinDsPiPIDK':       -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1000.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        9.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcKPIDPi':       -5.0,
                  'MinLcpPIDPi':        0.0,
                  'MinLcpPIDK':         0.0,
                  'MinLcPiPIDK':       -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1000.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       300.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':    5.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MinKSKPIDPi':        0.0,
                  'MinKSPiPIDK':        0.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':    5.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1000.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':        500.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  }, 
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : [ 'GammaFromTrees' ]
    }    

