B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' :  {
    "PrescalD0Mu"    : 0.5
    ,"MINIPCHI2"      : 9.0   # adimensiional     
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"TRCHI2Loose"   : 5.0    # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 0.0    # adimensiional
    ,"MuonIPCHI2"    : 4.00   # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV       
    ,"DsIPMin"       : 0.0498 #mm           
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional  
    ,"DZ"            : -9999    #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose" : 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,"PhiVCHI2"      :25.0    #adimensiional
    ,"PhiMassWin"    :50      #adimensiional
    ,'KSLLPMin'         : 2000  ## MeV
    ,'KSLLPTMin'        : 500   ## MeV
    ,'KSDDPMin'         : 3000  ## MeV
    ,'KSDDPTMin'        : 800   ## MeV
    ,'KSLLCutMass'      : 20    ## MeV
    ,'KSDDCutMass'      : 20    ## MeV
    ,'KSLLCutFDChi2'    : 100   ## unitless
    ,'KSDDCutFDChi2'    : 100   ## unitless
    ,'KSDaugTrackChi2'  : 4     ## max chi2/ndf for Ks daughter tracks
    ,'KSVertexChi2'     : 6     ## max chi2/ndf for Ks vertex
    ,'KSCutDIRA'        : 0.99  ## unitless
    ,'LambdaLLPMin'       : 2000 ## MeV
    ,'LambdaLLPTMin'      : 500  ## MeV
    ,'LambdaLLCutMass'    : 30   ## MeV
    ,'LambdaLLCutFDChi2'  : 100  ## unitless
    ,'LambdaDDPMin'       : 3000 ## MeV
    ,'LambdaDDPTMin'      : 800  ## MeV
    ,'LambdaDDCutMass'    : 30   ## MeV
    ,'LambdaDDCutFDChi2'  : 100  ## unitless
    ,'LambdaCutDIRA'      : 0.99 ## unitless
    ,'LambdaDaugTrackChi2': 4    ## unitless
    ,'LambdaVertexChi2'   : 6    ## max chi2/ndf for Lambda0 vertex    
    ,"Pi0PtMin"     : 1200         # Minimum Pt of pi0 (MeV)
    ,"Pi0PMin"      : 3000         # Minimum P of pi0 (MeV)
    ,"PhotonCL"     : 0.25         # Confidence level for Pi0 photons
    ,"D02HHPi0AMassWin" : 160  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 150  # MeV (mass window after vertex fit)
    ,"D02HHPi0DocaCut"  : 6    # mm
    ,"D02HHPi0PtCut"    : 2000 # MeV     
    },
    'STREAMS' : ['Semileptonic'] ,
    'WGs'    : ['Semileptonic']
    }

B2XuMuNu = {
    'BUILDERTYPE' : 'B2XuMuNuBuilder',
    'CONFIG' :  {
     #Muons
    "MuonTRCHI2"          : 4.    ,#adimensional
    "MuonP"               : 3000. ,#MeV
    "MuonPT"              : 1000. ,#MeV
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
    "BFDCHI2HIGH"         : 100.  ,#adimensional
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
