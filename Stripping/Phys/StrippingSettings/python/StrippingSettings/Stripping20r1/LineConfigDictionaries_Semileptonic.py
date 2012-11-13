JPsiForSL = {
    'BUILDERTYPE' : 'JPsiForSLAllLinesConf',
    'CONFIG' :  {
    "PrescaleSemiIncJpsi2mumu" : 1.0 ## adimensional
    ,"TrackGhostProbMax" : 0.35 # adimensional
    ,"MuonGhostProbMax" : 0.5 # adimensional
    ,"MuonPT" : 1200. # MeV
    ,"MuonP" : 6000. # MeV
    ,"MuonChi2" : 3. # adimensional
    ,"MuonIPChi2" : 4. # adimensional
    ,"TrackPT" : 500. # MeV
    ,"TrackP" : 3000. # MeV
    ,"TrackChi2" : 3. # adimensional
    ,"TrackIPChi2" : 9. # adimensional
    ,"PsiMasswinPreFit" : 150 # MeV
    ,"PsiDocaChi2Max" : 10 # adimensional
    ,"PsiVChi2NdofMax" : 4. # adimensional
    ,"PsiMasswin" : 140 # MeV
    ,"PsiPT" : 500 # MeV
    ,"PsiFDChi2Min" : 100 # adimensional
    ,"PsiDIRAMin" : 0.99 # adimensional
    ,"BCombMassMin" : 3400 # MeV
    ,"BCombMassMax" : 6000 # MeV
    ,"BDocaChi2Max" : 10. # adimensional
    ,"BVChi2NdofMax" : 3. # adimensional
    ,"BPTMin" : 800. # MeV
    ,"BFDChi2Min" : 200. # adimensional
    ,"BDIRAMin" : 0.9995 # adimensional
    ,"nLongTrackMax" : 250 # adimensional
    },
    'STREAMS' : ['Dimuon'] ,
    'WGs'    : ['Semileptonic']
    }


B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' : {
    "GEC_nLongTrk" : 250 # adimensional
    ,"TRGHOSTPROB" : 0.5 # adimensional
    ,"MuonGHOSTPROB" : 0.5 #adimensional
    ,"PrescalD0Mu"    : 0.5  # for D0->KPi line
    ,"PrescalDsPi_fakes" : 0.5  # for Bs->(Ds->PhiPi)Pi for Fakes line
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"TRCHI2Loose"   : 5.0    # adimensiional    
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"KaonPIDKTight" : 8.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.00   # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
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
    ,"Dstar_wideDMCutUpper" : 170. ## MeV
    },
    'STREAMS' : ['Semileptonic'] ,
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
    'CONFIG' : { 
    'Loose' : { 'Prescale'    : 0.3 ,
                'Postscale'   : 1.0 ,
                'TrGHOSTPROBCut' : 1, # loose cut   
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
                'B0d_VCHI2'    : 25, # 10 # 
                'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
                'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
                },
    'Tight'   : {'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 'TrGHOSTPROBCut' : 0.5, # 
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
                 'B0d_VCHI2'    : 25, 
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


B2XTauNu = {
    'BUILDERTYPE'       : 'B2XTauNuAllLinesConf',
    'CONFIG'    : {
    'Prescale_B0d2DstarTauNu'        : 1.0,
    'Prescale_B0d2DTauNu'            : 1.0,
    'Prescale_Bu2D0TauNu'            : 1.0,
    'Prescale_B0s2DsTauNu'           : 1.0,
    'Prescale_Bc2JpsiTauNu'          : 1.0,
    'Prescale_B0d2DdoubleStarTauNu'  : 1.0,
    'Postscale'   : 1.0 ,
    #B cuts
    'B_BPVDIRA'       : 0.999,
    #B combination cuts
    'B_DeltaM_low'       : -2579.0, #MeV
    'B_DeltaM_high'      : 300.0,   #MeV
    'B_DOCAMAX'          : 0.13,  #mm
    'B_upperDeltaM_low'  : 720.0,   #MeV
    'B_upperDeltaM_high' : 1721.0,  #MeV
    #D kaon cuts
    'D_K_PT'          : 150.0,  #MeV
    'Dplus_K_PT'      : 1500.0, #MeV
    'Ds_K_PT'         : 1500.0, #MeV
    'D_K_TRCHI2DOF'   : 3.0,
    'D_K_IPCHI2'      : 4.0,
    'D_K_PIDK'        : -5.0,
    'Dplus_K_PIDK'    : 0.0,
    'Dplus_K_TRPCHI2' : 0.1,
    #D pion cuts
    'D_Pi_PT'         : 150.0, # MeV
    'D_Pi_TRCHI2'     : 3.0,
    'D_Pi_IPCHI2'     : 4.0,
    'D_Pi_PIDK'       : 8.0,
    'Dplus_Pi_TRPCHI2' : 0.1,
    #D0-resonance parameters
    'D0_MassW'        : 40.0,      #MeV, 100 in StdLooseD0 
    'D0_BPVVDCHI2'    : 50.0,      #36 in StdLooseD0
    'D0_DIRA'         : 0.999,
    'D0_PT'           : 1600.0,    #MeV, 1.2 GeV in StdLooseD02KPi
    'D0_VCHI2'        : 10.0,      #10 in StdLooseD0
    #D0 combination cut
    'D0_DOCAMAX'      : 0.1,     #mm, 0.5 mm in StdLooseD0
    #D-resonance parameters
    'D_MassW'         : 40.0,      #MeV, 100 in StdLooseDplus
    'D_BPVVDCHI2'     : 50.0,      #36 in StdLooseDplus
    'Ds_BPVVDCHI2'    : 36.0,      #shorter lifetime than Dplus 
    'D_DIRA'          : 0.999,   #0.98 in StdLooseDplus
    'D_PT'            : 1600.0,    #MeV, 1.2 in StdLooseDplus
    'D_VCHI2'         : 10.0,      #30 in StdLooseDplus
    'D_MIPCHI2'       : 10.0,
    #Dstar-resonance parameters
    'slowPi_TRCHI2DOF' : 3.0,
    'slowPi_PT'        : 110.0,    #MeV
    'Dstar_PT'         : 1250.0,   #MeV, 1250 in StdLooseD*
    'Dstar_VCHI2'      : 10.0,     #MeV, 25 in StdLooseD*WithD02KPi 
    'Dstar_MassW'      : 50.0,     #MeV, 50 MeV in StdLooseD*WithD02KPi
    'Dstar_DeltaM_low'  : 135.0,
    'Dstar_DeltaM_high' : 160.0,   #MeV, 165.5 in StdLooseD*
    #Muon and J/psi cuts
    'Muon_TRCHI2DOF'    : 3.0,
    'Muon_PT'           : 1000.0,  #MeV
    'Jpsi_MassW'        : 80.0,    #MeV, 100 in StdLooseJpsi
    'Jpsi_VCHI2'        : 9.0,   #25 in StdLooseJpsi
    'Jpsi_PT'           : 2000.0,  #MeV
     #Cuts on single pions which are used to form the D_1(2420)0 from D* and pion
    'D1_Pi_IPCHI2' : 4.0,
    'D1_Pi_TRCHI2' : 3.0,
    'D1_Pi_PIDK'   : 8.0,
    #D1(2420) and D*2(2460) cuts
    'D1_VCHI2'         : 25.0,
    'D1_PT'            : 500.0,  #MeV
    'D1_DeltaM_low'    : 350.0,  #MeV
    'D1_DeltaM_high'   : 450.0,  #MeV
    'D1_MassW'         : 300.0,  #MeV
    #Ghost Prob cut on all tracks apart from slow pion
    'TRGHP'             : 0.4,
    #Slow pion ghost prob
    'TRGHP_slowPi'      : 0.6
    },
    'STREAMS' : ['BhadronCompleteEvent'],
    'WGs'    : ['Semileptonic'] 
    }


B2DMuForTauMu = {
    'BUILDERTYPE'       : 'B2DMuForTauMuconf',
    'CONFIG'    : {
     "MINIPCHI2"     : 45.0    # adimensiional 
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
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"SPDmax"        : 600    # adimensiional
     },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic'] 
    }



