JPsiForSL = {
    'BUILDERTYPE' : 'JPsiForSLAllLinesConf',
    'CONFIG' :  {
    "PrescaleSemiIncJpsi2mumu"    : 0.5
    ,"MuonPT" : 1200.
    ,"MuonP" : 6000.
    ,"MuonChi2" : 3.
    ,"MuonIPChi2" : 4. 
    ,"TrackPT" : 500.
    ,"TrackP" : 3000.
    ,"TrackChi2" : 3.
    ,"TrackIPChi2" : 5.
    ,"PsiMasswinPreFit" : 150
    ,"PsiDocaChi2Max" : 10
    ,"PsiVChi2NdofMax" : 10
    ,"PsiMasswin" : 140
    ,"PsiPT" : 500
    ,"PsiFDChi2Min" : 100
    ,"PsiDIRAMin" : 0.99
    ,"BCombMassMin" : 3400
    ,"BCombMassMax" : 6000
    ,"BDocaChi2Max" : 10.
    ,"BVChi2NdofMax" : 3.
    ,"BPTMin" : 800.
    ,"BFDChi2Min" : 200.
    ,"BDIRAMin" : 0.9995
    ,"nLongTrackMax" : 250
    },
    'STREAMS' : ['Dimuon'] ,
    'WGs'    : ['Semileptonic']
    }


CharmFromBSemi = {
    'BUILDERTYPE' : 'CharmFromBSemiAllLinesConf',
    'CONFIG' :  {
    "GEC_nLongTrk"    : 250   #adimensional
    ,"MINIPCHI2"      : 9.0    # adimensional
    ,"TRCHI2"        : 4.0    # adimensional
    ,"TRCHI2Loose"   : 5.0    # adimensional    
    ,"KaonPIDK"      : 4.0    # adimensional
    ,"PionPIDK"      : 10.0   # adimensional
    ,"PionPIDKTight" : 4.0    # adimensional
    ,"MuonIPCHI2"    : 4.00   # adimensional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensional
    ,"DsFDCHI2"      : 100.0  # adimensional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensional
    ,"PIDmu"         : -0.0   # adimensional
    ,"BDIRA"         : 0.999  #adimensional
    ,"BVCHI2DOF"     : 6.0    # adimensional
    ,"DZ"            : 0   #mm
    ,"DDocaChi2Max"  : 20     #adimensional
    ,"MINIPCHI2Loose": 4.0   #adimensional
    ,"KaonPIDKloose" : -5     #adimensional
    ,"PhiVCHI2"      :25.0    #adimensional
    ,"PhiMassWin"    :50      #adimensional
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
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 200  # MeV (mass window after vertex fit)
    ,"D02HHPi0DocaCut"  : 6    # mm
    ,"D02HHPi0PtCut"    : 2000 # MeV
    ,"Dstar_preFitMassCut" : 100 # MeV
    ,"Dstar_preFitMassCut_HHPi0" : 200 # MeV
    ,"Dstar_Chi2" :  10.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 5. ## unitless
    ,"Dstar_SoftPion_PT" : 200. ## MeV
    ,"Dstar_wideDMCutLower" : -2. ## MeV
    ,"Dstar_wideDMCutUpper" : 15. ## MeV
    ,"Dto4h_MassWin" : 40. ## MeV
    ,"Dto4h_AMassWin" : 42. ## MeV
    },
    'STREAMS' : ['Charm'] ,
    'WGs'    : ['Charm']
    }


B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' :  {
    "PrescalD0Mu"    : 0.5
    ,"PrescalDsPi_fakes"  : 0.5    # for Bs->(Ds->PhiPi)Pi for Fakes line
    ,"MINIPCHI2"      : 9.0   # adimensional    
    ,"TRCHI2"        : 4.0    # adimensional
    ,"TRCHI2Loose"   : 5.0    # adimensional
    ,"KaonPIDK"      : 4.0    # adimensional
    ,"PionPIDK"      : 10.0   # adimensional
    ,"PionPIDKTight" : 4.0    # adimensional
    ,"MuonIPCHI2"    : 4.00   # adimensional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensional
    ,"DsFDCHI2"      : 100.0  # adimensional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV      
    ,"DsIP"          : 7.4    #mm
    ,"DsVCHI2DOF"    : 6.0    # adimensional
    ,"PIDmu"         : -0.0   # adimensional
    ,"BDIRA"         : 0.999  #adimensional
    ,"BVCHI2DOF"     : 6.0    # adimensional 
    ,"DZ"            : 0     #mm
    ,"DZLoose"       : -9999  #mm
    ,"DDocaChi2Max"  : 20     #adimensional
    ,"MINIPCHI2Loose" : 4.0   #adimensional
    ,"KaonPIDKloose" : -5     #adimensional
    ,"PhiVCHI2"      :25.0    #adimensional
    ,"PhiMassWin"    :50      #adimensional
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


Bd2DstarMuNu = {
    'BUILDERTYPE' : 'Bd2DstarMuNuAllLinesConf',
    'CONFIG' :
    { 'Loose' : { 'Prescale'    : 0.3 ,
                'Postscale'   : 1.0 ,
                #muon paramters # loose # Tight
                'MuPT'        : 700, # 800  # MeV
                'MuTrPID'     :-20,  # -5  
                #kaon parameters
                'KPT'         : 350, # 350  # MeV
                'KTrPID'      : -5,  #-5    
                #pion parameters
                'PiPT'        : 350, # MeV
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
                 #kaon parameters
                 'KPT'         : 350, # MeV
                 'KTrPID'      :-5,
                 #pion parameters
                 'PiPT'        : 350, # MeV
                 #D0-resonance parameters
                 'D0MassW'     : 60, # MeV
                 'D0_BPVVDCHI2': 50,
                 #Dstar-resonance parameters
                 'Dstar_PT'     : 1250, #  MeV
                 'Dstar_VCHI2'  : 20,
                 'DstarMassW'   : 70,
                 #Bd-resonance parameters
                 'B0d_VCHI2'    : 10,
                 'B0dD0_DeltaZVtx' : -2.5, # mm
                 'B0dPV_DeltaZVtx' : 0.5 #  mm
                   }
      },
    'STREAMS' : ['Semileptonic'] ,
    'WGs'    : ['Semileptonic']
    }

B0q2DplusMuX = {
    'BUILDERTYPE' : 'B0q2DplusMuXAllLinesConf',
    'CONFIG' : {
        'Presel' : { 'Prescale'    : 0.02 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 500, #MeV
                 'MuPidPi'     : -5.,
                 'MuPidK'      : -10.,
                 'MuTrChi2'    : 9,
                 #kaon parameters
                 'KPT'         : 300, #MeV
                 'KIP'         : 0.03, #mm
                 'KPidPi'      : -5,
                 'KPidPi2'   : -5,
                 'KPidMu'      : -40,
                 'KPidP'       : -40,
                 'KTrChi2'     : 10, #set in the common particles
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -40,
                 'PiIP'        : 0.01, #mm
                 'PiPidMu'     : -5,
                 'PiTrChi2'    : 10, #set in the common particles
                 'PiIPChi2'    : 4,
                 #D-resonance parameters
                 'DPT'         : 1000, #MeV
                 'D_APT'       : 1000, #MeV
                 'D_VPCHI2'    : 0.000250,
                 'D_BPVVDCHI2' : 49,
                 #B-resonance parameters
                 'B_VPCHI2'    : 0.000250,
                 'B_BPVDIRA'   : 0.997
                 },

           'Tuned'   : { 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 500, #MeV
                 'MuPidPi'     : 1., #reduce pion feedthrough to 1%
                 'MuPidK'      : -5,
                 'MuTrChi2'    : 5,
                 #kaon parameters
                 'KPT'         : 400, #MeV
                 'KIP'         : 0.04, #mm
                 'KPidPi'      : 5,
                 'KPidPi2'     : 10, #Kaon with same charge as pion
                 'KPidMu'      : 5,
                 'KPidP'       : -10,
                 'KTrChi2'     : 5,
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -5,
                 'PiIP'        : 0.04, #mm
                 'PiPidMu'     : -5,
                 'PiTrChi2'    : 10,
                 'PiIPChi2'    : 7,
                 #D-resonance parameters
                 'DPT'         : 1500, #MeV
                 'D_APT'       : 1200, #MeV
                 'D_VPCHI2'    : 0.010,
                 'D_BPVVDCHI2' : 144,
                 #B-resonance parameters
                 'B_VPCHI2'    : 0.010,
                 'B_BPVDIRA'   : 0.9980
                 },
 
     'MC09'   : { 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 600, #MeV
                 'MuPidPi'     : 0.,
                 'MuPidK'      : -10,
                 'MuTrChi2'    : 2,
                 #kaon parameters
                 'KPT'         : 400, #MeV
                 'KIP'         : 0.03, #mm
                 'KPidPi'      : 7,
                 'KPidPi2'     : 7, #Kaon with same charge as pion
                 'KPidMu'      : 10,
                 'KPidP'       : -10,
                 'KTrChi2'     : 3,
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -5,
                 'PiIP'        : 0.01, #mm
                 'PiPidMu'     : -2,
                 'PiTrChi2'    : 9,
                 'PiIPChi2'    : 7,
                 #D-resonance parameters
                 'DPT'         : 1500, #MeV
                 'D_APT'       : 1200, #MeV
                 'D_VPCHI2'    : 0.0150,
                 'D_BPVVDCHI2' : 144,
                 #B-resonance parameters
                 'B_VPCHI2'    : 0.0150,
                 'B_BPVDIRA'   : 0.9980
                  }
        },
    'STREAMS' : [ 'Semileptonic' ],
    'WGs'    : ['Semileptonic']
    }

D0ForBXX = {
    'BUILDERTYPE'       : 'D0forBXXLinesConf',
    'CONFIG'    : {
    "PreScale"      : 0.005   # adimensional
    ,"PreScaleRS"   : 1.0    # adimensional
    ,"PreScaleWS"   : 0.2    # adimensional
    ,"MINIPCHI2"     : 9.0   # adimensional
    ,"TRCHI2"        : 3.0   # adimensional
    ,"KaonPIDK"      : 4.0   # adimensional
    ,"PionPIDK"      : 10.0   # adimensional
    ,"PT"            : 300.0  # MeV
    ,"D0DIRA"        : 0.9999    # adimensional
    ,"D0DIRAK3Pi"    : 0.9999    # adimensional
    ,"FDCHI2"        : 100.0   # adimensional
    ,"D0AMassWin"    : 100.0   # MeV
    ,"D0MassWin"     : 80.0  # MeV
    ,"D0IP"          : 0.2    #mm
    ,"D0K3PiIP"      : 0.2    #mm
    ,"D0VCHI2DOF"    : 6.0   # adimensional
    ,"DDocaChi2Max" : 20   #adimensional 
     },
    'STREAMS' : ['CharmCompleteEvent'],
    'WGs'    : ['Semileptonic'] 
    }
