#
#  Line config dictionaries for Semileptonic WG
#

B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' :  {
        "GEC_nLongTrk" : 250 # adimensional
        ,"TRGHOSTPROB" : 0.5 # adimensional
        ,"MuonGHOSTPROB" : 0.5 #adimensional
        ,"PrescalD0Mu"    : 1.0  # for D0->KPi line
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
        },
    'STREAMS' : {
        'Charm' : [
            'Strippingb2D0MuXB2DMuNuXLine',
            'StrippingDstarOfb2D0MuXB2DMuNuXLine'],
        'Semileptonic' : [
            'Strippingb2DsPi_PhiPi_fakesB2DMuNuXLine']
        },
    'WGs'    : ['Semileptonic']
    }

B2XuMuNu = {
    'BUILDERTYPE' : 'B2XuMuNuBuilder',
    'CONFIG' : {
        "GEC_nLongTrk"        : 250.  , #adimensional
        "TRGHOSTPROB"         : 0.5    ,#adimensional
        #Muons
        "MuonGHOSTPROB"       : 0.5    ,#adimensional
        "MuonTRCHI2"          : 4.    ,#adimensional
        "MuonP"               : 3000. ,#MeV
        "MuonPTight"          : 6000. ,#MeV
        "MuonPT"              : 1000. ,#MeV
        "MuonPTTight"         : 1500. ,#MeV
        "MuonPIDK"            : 0.    ,#adimensional
        "MuonPIDmu"           : 3.    ,#adimensional
        "MuonPIDp"            : 0.    ,#adimensional
        "MuonMINIPCHI2"       : 12    ,#adminensional
        #Xu
        #K channel
        "KaonTRCHI2"          : 6.   ,#adimensional
        "KaonP"               : 3000. ,#MeV
        "KaonPT"              : 800.  ,#MeV
        "KaonPIDK"            : 5.    ,#adimensional 
        "KaonPIDmu"           : 0.    ,#adimensional
        "KaonPIDp"            : 0.    ,#adimensional
        "KaonMINIPCHI2"       : 16     ,#adminensional
        #Rho channel
        "RhoMassWindow"       : 150.  ,#MeV
        "RhoMassWindowMin1SB" : 300.  ,#MeV
        "RhoMassWindowMax1SB" : 620.  ,#MeV
        "RhoMassWindowMin2SB" : 920.  ,#MeV
        "RhoMassWindowMax2SB" : 1200. ,#MeV
        "RhoVCHI2DOF"         : 6     ,#adimensional
        "RhoPT"               : 500.  ,#MeV
        "RhoLeadingPionPT"    : 800.  ,#MeV
        "RhoMINIPCHI2"        : 4     ,#adimensional
        "RhoChPionPT"         : 300.  ,#MeV
        "RhoChPionTRCHI2"     : 10.   ,#MeV
        "RhoChPionPIDK"       : -10.  ,#adminensional
        "RhoFDCHI2"           : 100.  ,#adimensional
        "RhoFD"               : 6     ,#mm
        "RhoIPMin"            : 0.3   ,#mm    
        "RhoDIRA"             : 0.9   ,#adimensional
        "RhoChPionMINIPCHI2"  : 9.    ,#adimensional
        #Kshort Daughter Cuts
        "KS0DaugP"            : 2000. ,#MeV
        "KS0DaugPT"           : 250.  ,#MeV
        "KS0DaugTrackChi2"    : 4.    ,#adimensional
        "KS0DaugMIPChi2"      : 50.   ,#adimensional
        #Kshort cuts
        "KSMajoranaCutFDChi2" : 100.  ,#adimensional
        "KS0VertexChi2"       : 10.   ,#adimensional
        "KS0PT"               : 700.  ,#adimensional
        "KsLLMaxDz"           : 650.  ,#mm
        "KsLLCutFD"           : 20.   ,#mm
        "KSLLMassLow"         : 456.  ,#MeV
        "KSLLMassHigh"        : 536.  ,#MeV
        "KSLLCutFDChi2"       : 100.  ,#adimensional
        "KS0MIPChi2"          : 8.    ,#adimensional
        #Kstar Pion Daughter Cuts
        "KstarChPionPT"       : 100.  ,#MeV
        "KstarChPionP"        : 2000. ,#MeV
        "KstarChPionTRCHI2"   : 10.   ,#adimensional
        "KstarChPionMINIPCHI2": 9.    ,#adimensional
        "KstarChPionPIDK"     : -10.  ,#adminensional
        #B Mother Cuts
        "BVCHI2DOF"           : 4.    ,#adminensional
        "BVCHI2DOFTight"      : 2.    ,#adminensional
        "BDIRA"               : 0.99  ,#adminensional
        "BDIRATight"          : 0.999  ,#adminensional
        "BFDCHI2HIGH"         : 100.  ,#adimensional
        "BFDCHI2Tight"        : 120.  ,#adimensional
        #B Mass Minima
        "KMuMassLowTight"     : 1500. ,#MeV
        "RhoMuMassLowTight"   : 2000. ,#MeV
        "KshMuMassLowTight"   : 3000. ,#MeV
        "KstarMuMassLowTight" : 2500. ,#MeV
        #
        "XMuMassUpper"        : 5500. ,#MeV
        "XMuMassUpperHigh"    : 6500. ,#MeV
        "Enu"                 : 1850. ,#MeV
        "EnuK"                : 2000. ,#MeV
        'KshZ'                : 0.    #mm
        },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic'] 
    }

PhiToKSKS = {
    'BUILDERTYPE' : 'PhiToKSKSAllLinesConf',
    'CONFIG' :  {
        "prescale_PhiToKK" : 0.01,
        "prescale_PhiToKsKs_LLLL" : 1.0,
        "prescale_PhiToKsKs_LLDD" : 1.0,
        "prescale_JPsiToKK" : 0.05,
        "prescale_JPsiToKsKs_LLLL" : 1.0,
        "prescale_JPsiToKsKs_LLDD" : 1.0,
        "GEC_nLongTrk" : 250,
        "DoDTF" : True,
        "GHOSTPROB_MAX" : 0.35, #adimensional
        "K_PTMIN" : 200, #MeV
        "K_PIDK_MIN" : 7, #adimensional
        "K_IPCHI2_MAX" : 9, #adimensional
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
    'STREAMS' : ['Charm'],
    'WGs'    : ['Semileptonic']
    }


