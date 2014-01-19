#
#  Line config dictionaries for Semileptonic WG
#

Lb2pMuNu = {
    'BUILDERTYPE'       : 'Lb2pMuNuBuilder',
    'CONFIG'    :{
    "GEC_nLongTrk"        : 250.   ,#adimensional
    "TRGHOSTPROB"         : 0.35   ,#adimensional
    #Muon Cuts
    "MuonGHOSTPROB"       : 0.35   ,#adimensional
    "MuonTRCHI2"          : 4.     ,#adimensional
    "MuonP"               : 3000.  ,#MeV
    "MuonPT"              : 1600.  ,#MeV
    "MuonMINIPCHI2"       : 12.    ,#adminensional
    #Proton Cuts 
    "ProtonTRCHI2"        : 6.     ,#adimensional
    "ProtonP"             : 15000. ,#MeV
    "ProtonPT"            : 1000.  ,#MeV
    "ProtonPIDK"          : 0.     ,#adimensional 
    "ProtonPIDp"          : 5.     ,#adimensional
    "ProtonMINIPCHI2"     : 16.    ,#adminensional
    #B Mother Cuts
    "BVCHI2DOF"           : 4.     ,#adminensional
    "BDIRA"               : 0.999  ,#adminensional
    "BFDCHI2HIGH"         : 150.   ,#adimensional
    "BPVIPChi2"           : 25.    ,#adminensional  
    "pMuMassLow"          : 2250.  ,#MeV
    "pMuMassLowTight"     : 2900.  ,#MeV
    "pMuMassUpper"        : 5600.  ,#MeV
    "pMuPT"               : 1500.  ,#MeV
    "PassymLow"           : -0.4   ,#adminensional
    "PassymLower"         : -0.65  ,#adimensional
    "PassymUpper"         : 0.0     #adimensional
    },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }

B2DHForTauMu = {
    'BUILDERTYPE' : 'B2DHForTauMuconf',
    'CONFIG' :  {
        "MINIPCHI2"     : 45.0    # adimensiional 
        ,"MINIPCHI2LOOSE"     : 9.0    # adimensiional 
        ,"GhostProb"     : 0.5     # adimensional  
        ,"KaonPIDK"      : 4.0    # adimensiional
        ,"PionPIDKTight" : 2.0    # adimensiional
        ,"MuonIPCHI2"    : 45.00   # adimensiional
        ,"KPiPT"         : 300.0  # MeV
        ,"DSumPT"        : 1400.0 # MeV
        ,"DsDIRA"        : 0.9998   # adimensiional
        ,"DsFDCHI2"      : 250.0  # adimensiional
        ,"DsMassWin"     : 80.0   # MeV
        ,"DsAMassWin"    : 100.0  # MeV
        ,"DsVCHI2DOF"    : 4.0    # adimensiional
        ,"PIDmu"         : 2.0   # adimensiional
        ,"BDIRA"         : 0.9995  #adimensiional
        ,"BFDCHI2"         : 50.0  #adimensiional
        ,"BVCHI2DOF"     : 6.0    # adimensiional
        ,"SPDmax"        : 600    # adimensiional
        },
    'STREAMS' : ['BhadronCompleteEvent'],
    'WGs'    : ['Semileptonic']
    }

B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' :  {
        "GEC_nLongTrk" : 250 # adimensional
        ,"TRGHOSTPROB" : 0.5 # adimensional
        ,"MuonGHOSTPROB" : 0.5 #adimensional
        ,"PrescalD0Mu"    : 0.5  # for D0->KPi line
        ,"PrescalDsPi_fakes" : 1.0  # for Bs->(Ds->PhiPi)Pi for Fakes line
        ,"MINIPCHI2"     : 9.0    # adimensiional
        ,"TRCHI2"        : 4.0    # adimensiional
        ,"TRCHI2Loose"   : 5.0    # adimensiional
        ,"TRCHI2Tight"   : 3.0    # adimensiional    
        ,"KaonPIDK"      : 4.0    # adimensiional
        ,"KaonPIDKTight" : 8.0    # adimensiional
        ,"PionPIDK"      : 10.0   # adimensiional
        ,"PionPIDKTight" : 4.0    # adimensiional
        ,"MuonIPCHI2"    : 4.00   # adimensiional
        ,"MuonPT"        : 800.0  # MeV
        ,"KPiPT"         : 300.0  # MeV
        ,"KPiPTTight"    : 1500.0  # MeV
        ,"DsDIRA"        : 0.99   # adimensiional
        ,"DsFDCHI2"      : 100.0  # adimensiional
        ,"DsMassWin"     : 80.0   # MeV
        ,"DsAMassWin"    : 100.0  # MeV
        ,"D02HHHHMassWin": 60.0   # MeV
        ,"D02HHHHSumPT"  : 1800.0   # MeV
        ,"DsIP"          : 7.4    #mm
        ,"DsVCHI2DOF"    : 6.0    # adimensiional
        ,"PIDmu"         : -0.0   # adimensiional
        ,"BDIRA"         : 0.999  #adimensiional
        ,"BVCHI2DOF"     : 6.0    # adimensiional
        ,"DZLoose"       : -9999  #mm
        ,"DZ"            : 0  #mm
        ,"DDocaChi2Max"  : 20     #adimensiional
        ,"DDocaChi2MaxTight"  : 9.0     #adimensiional
        ,"MINIPCHI2Loose" : 4.0   #adimensiional
        ,"KaonPIDKloose" : -5     #adimensiional
        ,"PhiVCHI2"      :25.0    #adimensiional
        ,"PhiMassWin"    :40      #adimensiional
        ,"Dstar_Chi2"       :  8.0 ## unitless
        ,"Dstar_SoftPion_PIDe" : 2. ## unitless
        ,"Dstar_SoftPion_PT" : 180. ## MeV ###
        ,"Dstar_wideDMCutLower" : 0. ## MeV
        ,"Dstar_wideDMCutUpper" : 170. ## MeV,
        ,"FakePionP_Tight": 6.0    # GeV,
        ,"BMassMin" : 3.1 #GeV
        ,"BMassMax" : 5.1 #GeV
        ### new cuts for asls (2014-01-14-Marian Stahl)
        ### Muon cuts 
        ,"MuonPTloose"        : 600.0    #MeV
        #### Hadron cuts
        ,"KPiPTloose"         : 150.0    #MeV 
        ,"KPiPTOpt"           : 380.0    #MeV 
        ,"KPiP"               : 1.5      #GeV (was hidden in the script before, changed it from 2 to 1.5)
        ### Kaon cuts
        ,"KaonPIDKKKPiOpt"    : 5.0      #adimensional
        ,"KaonPIDKPhiPi"      : 0.0      #adimensional
        ### Pion cuts
        ,"PionPIDKloose"      : 20.0     #adimensional (changed from 10)
        ,"PionPIDKKKPiOpt"    : 12.0     #adimensional (changed from 10)
        ,"PionPIDK3PiOpt"     : 9.0     #adimensional (changed from 10)
        #### D combination cuts
        ,"DsFDCHI2loose"      : 20.0     #adimensional
        ,"DsVCHI2DOFOpt"      : 7.0      #adimensional
        ,"DsVCHI2DOFloose"    : 8.0      #adimensional
        ,"DDocaChi2Maxloose"  : 30       #adimensional
        ,"PhiMassMin"         : 979.455 #MeV
        ,"PhiMassMax"         : 1059.455 #MeV 
        ,"KStarMassMin"       : 805.94   #MeV
        ,"KStarMassMax"       : 985.94   #MeV 
        ##### B combination cuts
        ,"BDIRAloose"         : 0.99     #adimensional (changed from 0.999)
        ,"BVCHI2DOFloose"     : 20.0     #adimensional (changed from 6)
        ,"DZloose"            : -3.0     #mm (changed from 0)
        ,"DZKStarK"           : -1.0     #mm (changed from 0)
        ,"DZKKPiOpt"          : -0.2     #mm (changed from 0)
        ,"BMassMinloose"      : 0        #GeV  (changed from 3.1)
        ,"BMassMaxloose"      : 1000     #GeV  (changed from 5.1)
        ,"TISTOSSpecs"        : {'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0} #multidimensional
        ,"TISTOSSpecsPhiPi"   : {'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,'Hlt2.*IncPhi.*Decision%TOS':0} #multidimensional  
        },
    'STREAMS' : {
        'Semileptonic' : [
            'Strippingb2DsPhiPiMuXB2DMuNuXLine',
            'Strippingb2DsKStarKMuXB2DMuNuXLine',   
            'Strippingb2DsKKPiMuXB2DMuNuXLine',
            'Strippingb2Ds3PiMuXB2DMuNuXLine']
        },
    'WGs'    : ['Semileptonic']
    }

PhiToKSKS = {
    'BUILDERTYPE' : 'PhiToKSKSAllLinesConf',
    'CONFIG' :  {
        "prescale_PhiToKK" : 0.001,
        "prescale_PhiToKsKs" : 1.0,
        "prescale_PhiToMuMu" : 0.01,
        "prescale_JPsiToKK" : 0.01,
        "prescale_JPsiToKsKs" : 1.0,
        "GEC_nLongTrk" : 250,
        "DoDTF" : True,
        "GHOSTPROB_MAX" : 0.35, #adimensional 
        "K_PTMIN" : 200, #MeV                                                                                                                                                
        "K_PIDK_MIN" : 7, #adimensional                                                                                                                  
        "K_IPCHI2_MAX" : 9, #adimensional                                                                                                                                
        "Mu_PTMIN" : 200, #MeV                                                                                                                                   
        "Mu_PIDmu_MIN" : 0, #adimensional                                                                                                                 
        "Mu_IPCHI2_MAX" : 9, #adimensional   "prescale_PhiToKK" : 0.01,
        "KS_LL_PTMIN" : 200, #MeV
        "KS_LL_MASS_WINDOW" : 50, #MeV
        "KS_LL_FD_MIN" : 10.0, #mm
        "KS_LL_FDCHI2_MIN" : 100, #adimensional
        "KS_LL_VCHI2NDOF_MAX" : 4,#adimensional
        "KS_LL_DIRA_MIN" : 0.9999, #adimensional
        "KS_DD_PTMIN" : 200, #MeV
        "KS_DD_MASS_WINDOW" : 50, #MeV
        "KS_DD_FD_MIN" : 10.0, #mm
        "KS_DD_FDCHI2_MIN" : 100, #adimensional
        "KS_DD_VCHI2NDOF_MAX" : 4,#adimensional
        "KS_DD_DIRA_MIN" : 0.999, #adimensional
        "Phi_MASS_MAX" : 1100, # MeV
        "Phi_PT_MIN" : 400, # MeV
        "Phi_DOCACHI2_MAX" : 20, #adimensional
        "Phi_VCHI2NDOF_MAX" : 6, #adimensional
        "Phi_IPCHI2_MAX" : 9, #adimensional
        "JPsi_MASS_WIN" : 120, # MeV
        "JPsi_PT_MIN" : 500, # MeV
        "JPsi_DOCACHI2_MAX" : 20, #adimensional
        "JPsi_VCHI2NDOF_MAX" : 6, #adimensional
        "JPsi_IPCHI2_MAX" : 9 #adimensional
        },
    'STREAMS' : {
        'Charm':['StrippingPhiToKSKS_PhiToKKLine',
                 'StrippingPhiToKSKS_PhiToMuMuLine']
        },
    'WGs'    : ['Semileptonic']
    }


