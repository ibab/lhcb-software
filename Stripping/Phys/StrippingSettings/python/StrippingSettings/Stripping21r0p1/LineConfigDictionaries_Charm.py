###########################################################################################
##                          S T R I P P I N G  2 1r1p1                                   ##
##                                                                                       ##
##  Configuration for Charm WG                                                           ##
##  Contact person: Eva Gersabeck & Jinlin Fu (evelina.gersabeck@cern.ch & j.fu@cern.ch) ##
###########################################################################################

from GaudiKernel.SystemOfUnits import *

#Mika Vesterinen, Alex Pearce
#StrippingCharmFromBSemi.py
CharmFromBSemi = {
    "BUILDERTYPE": "CharmFromBSemiAllLinesConf", 
    "CONFIG": {
        "BDIRA": 0.999, 
        "BVCHI2DOF": 6.0, 
        "CombMassHigh_HH": 1820, 
        "CombMassLow_HH": 0, 
        "D02HHGammaAMassWin": 220, 
        "D02HHGammaMassWin": 210, 
        "D02HHPi0AMassWin": 220, 
        "D02HHPi0DocaCut": 6.0, 
        "D02HHPi0MassWin": 210, 
        "D02KSHHPi0AMassWin": 220, 
        "D02KSHHPi0MassWin": 210, 
        "D02KSHHPi0_D0PT": 1000, 
        "D02KSHHPi0_D0PTComb": 1000, 
        "D02KSHHPi0_PTSUMLoose": 1000, 
        "DDocaChi2Max": 20, 
        "DZ": 0, 
        "DsAMassWin": 100.0, 
        "DsDIRA": 0.99, 
        "DsFDCHI2": 100.0, 
        "DsIP": 7.4, 
        "DsMassWin": 80.0, 
        "DsVCHI2DOF": 6.0, 
        "Dstar_Chi2": 8.0, 
        "Dstar_SoftPion_PIDe": 2.0, 
        "Dstar_SoftPion_PT": 80.0, 
        "Dstar_wideDMCutLower": 0.0, 
        "Dstar_wideDMCutUpper": 170.0, 
        "Dto4hADocaChi2Max": 7, 
        "Dto4h_AMassWin": 42.0, 
        "Dto4h_MassWin": 40.0, 
        "DtoXgammaADocaChi2Max": 10, 
        "GEC_nLongTrk": 250, 
        "HLT2": "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision')", 
        "KPiPT": 250.0, 
        "KSCutDIRA": 0.99, 
        "KSCutZFDFromD": 10.0, 
        "KSDDCutFDChi2": 100, 
        "KSDDCutMass": 30, 
        "KSDDPMin": 3000, 
        "KSDDPTMin": 250, 
        "KSDaugTrackChi2": 4, 
        "KSLLCutFDChi2": 100, 
        "KSLLCutMass": 30, 
        "KSLLPMin": 2000, 
        "KSLLPTMin": 250, 
        "KSVertexChi2": 6, 
        "KaonPIDK": 4.0, 
        "KaonPIDKloose": -5, 
        "LambdaCutDIRA": 0.99, 
        "LambdaDDCutFDChi2": 100, 
        "LambdaDDCutMass": 30, 
        "LambdaDDPMin": 3000, 
        "LambdaDDPTMin": 800, 
        "LambdaDaugTrackChi2": 4, 
        "LambdaLLCutFDChi2": 100, 
        "LambdaLLCutMass": 30, 
        "LambdaLLPMin": 2000, 
        "LambdaLLPTMin": 500, 
        "LambdaVertexChi2": 6, 
        "MINIPCHI2": 9.0, 
        "MINIPCHI2Loose": 4.0, 
        "MassHigh_HH": 1810, 
        "MassLow_HH": 0, 
        "MaxBMass": 6000, 
        "MaxConvPhDDChi": 9, 
        "MaxConvPhDDMass": 100, 
        "MaxConvPhLLChi": 9, 
        "MaxConvPhLLMass": 100, 
        "MaxVCHI2NDOF_HH": 10.0, 
        "MinBMass": 2500, 
        "MinConvPhDDPT": 800, 
        "MinConvPhLLPT": 800, 
        "MinVDCHI2_HH": 1000.0, 
        "MuonIPCHI2": 4.0, 
        "MuonPT": 800.0, 
        "PIDmu": -0.0, 
        "PTSUM": 1800.0, 
        "PTSUMLoose": 1400.0, 
        "PTSUM_HHGamma": 1800.0, 
        "PTSUM_HHPi0": 1800.0, 
        "PhotonCL": 0.25, 
        "PhotonPT": 1500, 
        "Pi0PMin": 3000, 
        "Pi0PtMin": 1000, 
        "PionPIDK": 10.0, 
        "PionPIDKTight": 4.0, 
        "TRCHI2": 4.0, 
        "TTSpecs": {
            "Hlt1.*Track.*Decision%TOS": 0, 
            "Hlt2.*SingleMuon.*Decision%TOS": 0, 
            "Hlt2Global%TIS": 0, 
            "Hlt2Topo.*Decision%TOS": 0
        }, 
        "TrGhostProbMax": 0.5
    }, 
    "STREAMS": [ "Charm" ], 
    "WGs": [ "Charm" ]
}


#Adam Davis,Liang Sun
#StrippingD2HMuNu.py
D2HMuNu = {
    'WGs'         : ['Charm'],
    'BUILDERTYPE' : 'D2HLepNuBuilder',
    'CONFIG'      :  {
       "KLepMassLow" : 500 , #MeV
       "KLepMassHigh" : 2000 , #MeV
       "DELTA_MASS_MAX" : 400  , #MeV
       "GEC_nLongTrk"        : 160.  , #adimensional
       "TRGHOSTPROB"         : 0.35    ,#adimensional
       #Muons
       "MuonGHOSTPROB"       : 0.35  ,#adimensional
       #	"MuonP"               : 3000. ,#MeV
       "MuonPT"              : 500. ,#MeV
       "MuonPIDK"            : 0.    ,#adimensional
       "MuonPIDmu"           : 3.    ,#adimensional
       "MuonPIDp"            : 0.    ,#adimensional
       "ElectronPIDe"  : 0.0 ,
       "ElectronPT"    : 500,#MeV
       #Xu
       #K channel
       "KaonP"               : 3000.  ,#MeV
       "KaonPT"              : 800.   ,#MeV
       "KaonPIDK"            : 5.     ,#adimensional 
       "KaonPIDmu"           : 5.     ,#adimensional
       "KaonPIDp"            : 5.     ,#adimensional
       "KaonMINIPCHI2"          : 9      ,#adimensional
       "BVCHI2DOF"       : 20    ,#adminensional
       "BDIRA"       : 0.999    ,#adminensional
       "BFDCHI2HIGH"         : 100.  ,#adimensional
       "BPVVDZcut"   : 0.0, #mm
       #slow pion
       "Slowpion_PT" : 300 #MeV
       ,"Slowpion_P" : 1000 #MeV
       ,"Slowpion_TRGHOSTPROB" : 0.35 #adimensional
       ,"Slowpion_PIDe" : 5 #adimensional
       ,"useTOS" : True #adimensional
       ,"TOSFilter" : { 'Hlt2CharmHad.*HHX.*Decision%TOS' : 0}  #adimensional
       },
    'STREAMS'     : ['CharmCompleteEvent']	  
 }


#Marco Pappagallo
#StrippingExcitedDsSpectroscopy.py
ExcitedDsSpectroscopy = {
    "BUILDERTYPE": "ExcitedDsSpectroscopyConf", 
    "CONFIG": {
        "Daughter2IPChi2": 10.0, 
        "DeltaMassDsg": 1300.0, 
        "DeltaMassDsgg": 1300.0, 
        "DeltaMassDsstar": 450.0, 
        "DsDIRA": 0.0, 
        "DsGammaLinePostscale": 1.0, 
        "DsGammaLinePrescale": 1.0, 
        "DsIPChi2": 9.0, 
        "DsPt": 1000.0, 
        "DsVXChi2NDOF": 10.0, 
        "DsstGammaLinePostscale": 1.0, 
        "DsstGammaLinePrescale": 1.0, 
        "Gamma2CL": 0.1, 
        "Gamma2PT": 0.0, 
        "GammaCL": 0.1, 
        "GammaPT": 400.0, 
        "KPIDK": 7.0, 
        "MaxMassDsst": 2300.0, 
        "MaxMassKstar": 956.0, 
        "MaxMassPhi": 1030.0, 
        "MaxTracksInEvent": 500, 
        "MinCosHelAngleKstar": 0.4, 
        "MinMassKstar": 836.0, 
        "MinMassPhi": 1010.0, 
        "PTSum": 2800.0, 
        "piPIDK": 3.0
    }, 
    "STREAMS": [ "Charm" ], 
    "WGs": [ "Charm" ]
}

#Marianna Fontana, Andrea Contu
#StrippingDstarD02xx.py
DstarD02xx ={
  'WGs' : ['Charm'],
  'BUILDERTYPE' : 'StrippingDstarD02xxConf',
  'STREAMS' : [ 'Charm' ],
  'CONFIG' : {    'PrescalepipiBox'     : 1.
                  , 'PrescalemumuBox'     : 1.
                  , 'PrescaleKpiBox'    : 1.
                  , 'PrescaleemuBox'    : 1.
                  , 'PrescaleeKBox'    : 1.
                  , 'PrescaleepiBox'    : 1.
                  , 'PrescalepimuBox'    : 1.
                  , 'PrescaleKmuBox'    : 1.
                  , 'Prescalepipi_untagged_Box'     : 1.0
                  , 'Prescalemumu_untagged_Box'     : 1.0
                  , 'PrescaleKpi_untagged_Box'    : 1.0
                  , 'Prescalepimu_untagged_Box'    : 1.0
                  , 'PrescaleKmu_untagged_Box'    : 0.5
                  , 'PrescaleKpi_untagged_BoxMB' : 0.
                  , 'Prescalepipi_untagged_BoxMB':0.
                  , 'PrescaleKpi_untagged_BoxMBTrEff' : 0.
                  ,'DMassWin'           : 70.       # MeV
                  ,'DMassWinMuMuLow'    : -150.       #MeV
                  ,'DMassWinMuMuHigh'   : 300       #MeV
                  ,'DMassWinEMu'        : 300
                  ,'doca'               : 0.1        # mm
                  ,'XminPT'             : 750.       # MeV
                  ,'XmaxPT'             : 1100.      # MeV
                  ,'XminP'              : 4000.      # MeV
                  ,'XTrackChi2'         : 5.        # adimensional
                  ,'XTrackChi2Pi'         : 7.        # adimensional
                  ,'XminIPChi2'         : 3        # adimensional
                  ,'XmaxIPChi2'         : 8        # adimensional
                  ,'ghostProbCut'       : 0.5      #added for Stripping20 by A Contu
                  ,'DMinFlightChi2'    :  20.
                  ,'DDira'              : 0.9997     # adimensional
                  ,'D0MinPT'            : 1800.      # MeV
                  ,'DMaxIPChi2'        :15.
                  ,'DVChi2'            :10. 
                  ,'PiMinPT'            : 110.       # MeV
                  ,'PiMaxIPCHI2'        : 10.         # adimensional
                  ,'DstMassWin'         : 300.       # MeV
                  ,'DstD0DMWin'         : 10.        # MeV
                  ,'DstD0DMWinMuMu'      : 30.        # MeV  
                  ,'RequireHlt'         : 1
                  #,'HLT2String'          :  "Hlt2RareCharmD02LAB1LAB2*Decision" #run2
                  ,'HLT2String'          :  "Hlt2Dst2PiD02LAB1LAB2*Decision" #run1
                  ,'HLT1MB'             : "HLT_PASS_RE('Hlt1(MB|L0).*Decision')"
                  ,'HLT2MB'             : "HLT_PASS_RE('Hlt2CharmHadMinBiasD02KPiDecision')"
                  #hltname = "Hlt2Dst2PiD02"+Xplus+Xminus+"*Decision"
                  #hltname = "Hlt2Dst2PiD02"+Xplus+Xminus+"*Decision"
                 ,'ConeAngles'     : {"08":0.8,"10":1.0,"12":1.2,"14":1.4}
                 ,'ConeVariables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                 ,'prefix'         : '' 
                 }
    
}

#Simone Stracka,
#StrippingD2PiPi0.py linebuilder
D2EtaH = {
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2EtaHConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
               # Minimum best primary vertex IP chi^2 for charged D daughters
               'Bachelor_IPCHI2_MIN'      : 25.0,        # 25 --> 16
               # Minimum PT for charged D daughters
               'Bachelor_PT_MIN'          : 600  *MeV,   # 600
               # Minimum momentum for charged D daughters
               'Bachelor_P_MIN'           : 1000 *MeV,
               # Bachelor particle PID requirement 
               'Bachelor_PIDK_MIN'        : -999.0, # not used
               # Minimum best primary vertex IP chi^2 for charged D daughters
               'Daug_IPCHI2_MIN'      : 16.0,        # 25 --> 4
               # Minimum PT for charged D daughters
               'Daug_PT_MIN'          : 500  *MeV,   # 600 --> 300
               # Minimum momentum for charged D daughters
               'Daug_P_MIN'           : 1000 *MeV,
               # Maximum momentum for charged D daughters
               'Daug_P_MAX'           : 100000 *MeV,
               # Minimum pseudorapidity for charged D daughters
               'Daug_ETA_MIN'         : 2.0,
               # Maximum pseudorapidity for charged D daughters
               'Daug_ETA_MAX'         : 5.0,
               # Track quality requirement for charged D daughters
               'Daug_TRCHI2DOF_MAX'   : 5,
               # Maximum ghost probability for charged D daughters
               'Daug_TRGHOSTPROB_MAX' : 0.5,
               # Maximum DLLK of intermediate resonance daughter (pion)
               'Pi_PIDK_MAX'        : 0.0,
               # Minimum PT for intermediate resonance neutral daughters
               'Neut_PT_MIN'        : 600  *MeV,
               # Mass window for intermediate resonance neutral daughters (pi0, eta)
               'Neut_Mass_Win'      :  60  *MeV,
               # Minimum dipion mass
               'Dipion_Mass_MIN'    : 200 *MeV,
               # Maximum dipion mass
               'Dipion_Mass_MAX'    : 750 *MeV,
               # Maximum DOCA chi2 for dipion combination
               'Dipion_DOCACHI2_MAX': 15,
               # Minimum mass for intermediate resonance
               'Res_Mass_MIN'       : 350   *MeV,  # 500 --> 400
               # Maximum mass for intermediate resonance
               'Res_Mass_MAX'       : 750  *MeV,   # 600 --> 700
               # Minimum D PT
               'D_PT_Min'           : 2000 *MeV,   # 
               # Minimum D mass
               'D_Mass_MIN'          : 1600 *MeV,
               # Maximum D mass
               'D_Mass_MAX'          : 2200 *MeV,
               # Maximum chi^2 on D end vertex
               'D_VCHI2PDOF_MAX'         : 4,
               # minimum flight time
               'D_BPVLTIME_MIN'       : 0.25*picosecond,
               # max chi^2 per DOF of DTF fit
               'DTF_CHI2NDOF_MAX'   : 5,    # not used
               # HLT filters, only process events firing triggers matching the RegEx
               'Hlt1Filter'         : None,
               'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadD2.*Decision')",
               # prescale and postscale
               'PrescaleD2PiEtaPPG' : 1,
               'PrescaleD2PiEta3HM' : 1,
               'PrescaleD2PiEta3HR' : 1,
               'PrescaleD2KEtaPPG'  : 1,
               'PrescaleD2KEta3HM'  : 1,
               'PrescaleD2KEta3HR'  : 1,
               'PostscaleD2PiEtaPPG': 1,
               'PostscaleD2PiEta3HM': 1,
               'PostscaleD2PiEta3HR': 1,
               'PostscaleD2KEtaPPG' : 1,
               'PostscaleD2KEta3HM' : 1,
               'PostscaleD2KEta3HR' : 1
               }
 }   
D2EtaPrimeH = {
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2EtaPrimeHConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 25.0,        # 25 
                # Minimum PT for charged D daughters
                'Bachelor_PT_MIN'          : 600  *MeV,   # 600
                # Minimum momentum for charged D daughters
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used 
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 16.0,        # 25 --> 16
                # Minimum PT for charged D daughters
                'Daug_PT_MIN'          : 500  *MeV,   # 600 --> 500
                # Minimum momentum for charged D daughters
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Maximum DLLK of intermediate resonance daughter (pion)
                'Pi_PIDK_MAX'        : 0,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 1000  *MeV,
                # Mass window for intermediate resonance neutral daughters (pi0, eta)
                'Neut_Mass_Win'      :  60  *MeV,
                # Minimum dipion mass
                'Dipion_Mass_MIN'    : 200 *MeV,
                # Maximum dipion mass
                'Dipion_Mass_MAX'    : 1200 *MeV,
                # Maximum DOCA chi2 for dipion combination
                'Dipion_DOCACHI2_MAX': 15,
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 650   *MeV,  # 900 --> 800
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 1200  *MeV,   # 990 --> 1050
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV,   # 2000
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2200 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 4,
                # minimum flight time
                'D_BPVLTIME_MIN'       : 0.25*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         :  None, 
                'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadD2.*Decision')",
                # prescale and postscale
                'PrescaleD2PiEtaPrimePPG' : 1,
                'PrescaleD2PiEtaPrime3HR' : 1,
                'PrescaleD2KEtaPrimePPG'  : 1,
                'PrescaleD2KEtaPrime3HR'  : 1,
                'PostscaleD2PiEtaPrimePPG': 1,
                'PostscaleD2PiEtaPrime3HR': 1,
                'PostscaleD2KEtaPrimePPG' : 1,
                'PostscaleD2KEtaPrime3HR' : 1
                }
}
D2PhiH = {
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingD2PhiHConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 25.0,        # 25 --> 16
                # Minimum PT for charged D daughters
                'Bachelor_PT_MIN'          : 600  *MeV,   # 600 
                # Minimum momentum for charged D daughters
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used 
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 25.0,        # 25 --> 16
                # Minimum PT for charged D daughters
                'Daug_PT_MIN'          : 500  *MeV,   # 600 --> 300
                # Minimum momentum for charged D daughters
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Maximum DLLK of intermediate resonance daughter (pion)
                'Pi_PIDK_MAX'        : 0,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 600  *MeV,
                # Mass window for intermediate resonance neutral daughters (pi0, eta)
                'Neut_Mass_Win'      :  60  *MeV,
                # Minimum dipion mass
                'Dipion_Mass_MIN'    : 200 *MeV,
                # Maximum dipion mass
                'Dipion_Mass_MAX'    : 1200 *MeV,
                # Maximum DOCA chi2 for dipion combination
                'Dipion_DOCACHI2_MAX': 15,
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 650   *MeV,  #  includes omega
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 950  *MeV,   #
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV,   #
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2200 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 4,
                # minimum flight time
                'D_BPVLTIME_MIN'       : 0.25*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         : None,
                'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadD2.*Decision')",
                # prescale and postscale
                'PrescaleD2PiPhi3HM' : 1,
                'PrescaleD2PiPhi3HR' : 1,
                'PrescaleD2KPhi3HM'  : 1,
                'PrescaleD2KPhi3HR'  : 1,
                'PostscaleD2PiPhi3HM': 1,
                'PostscaleD2PiPhi3HR': 1,
                'PostscaleD2KPhi3HM' : 1,
                'PostscaleD2KPhi3HR' : 1
                }
}
