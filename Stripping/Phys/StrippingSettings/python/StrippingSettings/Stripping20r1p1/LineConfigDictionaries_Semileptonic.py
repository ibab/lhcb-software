#
#  Line config dictionaries for Semileptonic WG
#

B2DHHHForBXX = {
    'BUILDERTYPE' : 'B2DHHHForBXXLinesConf',
    'CONFIG' : {
    "PreScale"      : 1.0   # adimensional
    ,"GEC_nLongTrk"  : 250   # adimensional
    ,"TRCHI2"        : 3.0   # adimensional
    ,"GHOSTPROB_MAX" : 0.35  # adimensional
    ,"KaonPIDK"      : 4.0   # adimensional
    ,"PionPIDK"      : 2.0   # adimensional
    ,"dau_PT_MIN"    : 200   # MeV -> was 400
    ,"dau_MIPCHI2DV" :   5   # adimensional
    ,"D_MASS_MIN"    : 600   # MeV                 
    ,"D_MASS_MAX"    : 1400  # MeV                 
    ,"D_PT_MIN"      : 1500  # MeV                 
    ,"D_AMAXDOCA"    : 0.5   # mm                  
    ,"D_DOCACHI2_MAX":   6   # adimensional        
    ,"D_VFASPF"      :   4   # adimensional
    ,"D_BPVVDCHI2"   : 100   # adimensional
    ,"Lc_MASS_MIN"   :  700  # MeV
    ,"Lc_MASS_MAX"   : 1370  # MeV
    ,"Lc_PT_MIN"     : 1500  # MeV
    ,"Lc_AMAXDOCA"   : 0.5   # mm
    ,"Lc_DOCACHI2_MAX":  6   # adimensional
    ,"Lc_VFASPF"     :   4   # adimensional    
    ,"Lc_BPVVDCHI2"  : 100   # adimensional
    ,"A1_MASS_MIN"   : 400   # MeV
    ,"A1_MASS_MAX"   : 3400  # MeV
    ,"A1_PT_MIN"     : 1500  # MeV
    ,"A1_AMAXDOCA"   : 0.5   # mm              
    ,"A1_VFASPF"     :   4   # adimensional    
    ,"A1_BPVVDCHI2"  : 100   # adimensional    
    ,"B_MASS_MIN"    : 3400  # MeV
    ,"B_MASS_MAX"    : 4950  # MeV
    ,"B_PT_MIN"      : 1500  # MeV
    ,"B_DOCACHI2_MAX": 50    # adimensional     
    ,"B_VFASPF"      : 10    # adimensional
    ,"B_BPVVDCHI2"   : 100   # adimensional
    ,"B_DZ"          : 0.0   # mm
    ,"B_D_deltamass_MIN" : 2500 #MeV              
    ,"B_D_deltamass_MAX" : 4550 #MeV
    ,"PiPi_MASS_MAX"                     :  500  #MeV            (500)
    ,"PiPi_DOCACHI2_MAX"                 :   15  # adimensional  (15)
    ,"PiPi_SUMPT_MIN"                    :  200  #MeV            (600)
    ,"PiPi_CHI2NDF"                      :    4  # adimensional  (3)
    ,"Lb_MASS_MIN"                       : 2800  # MeV               
    ,"Lb_MASS_MAX"                       : 4800  # MeV               
    ,"Lb_PT_MIN"                         : 1500  # MeV
    ,"Lb_DOCACHI2_MAX"                   : 50    # adimensional
    ,"Lb_VFASPF"                         : 10    # adimensional
    ,"Lb_BPVVDCHI2"                      : 100   # adimensional
    ,"Lb_DZ"                             : 0.0   # mm
    ,"Lb_Lc_deltamass_MIN"               : 1900  #MeV                         
    ,"Lb_Lc_deltamass_MAX"               : 4150  #MeV             
    ,"LcStar_MASS_MIN"                   :    0  #
    ,"LcStar_MASS_MAX"                   :  700  #MeV            (700)
    },
    'STREAMS' : ['BhadronCompleteEvent'],
    'WGs'    : ['Semileptonic']
    }


B2XTauNu = {
    'BUILDERTYPE'       : 'B2XTauNuAllLinesConf',
    'CONFIG'    :{
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
    'STREAMS' : {
    'BhadronCompleteEvent' : ['StrippingBc2JpsiTauNuNonPhysTauForB2XTauNu',
                              'StrippingBd2DTauNuNonPhysTauForB2XTauNu',
                              'StrippingBd2DdoubleStarTauNuNonPhysTauForB2XTauNu',
                              'StrippingBd2DstarTauNuNonPhysTauForB2XTauNu',
                              'StrippingBs2DsTauNuNonPhysTauForB2XTauNu',
                              'StrippingBu2D0TauNuNonPhysTauForB2XTauNu',
                              ]
    },
    'WGs'    : ['Semileptonic']
    }
   

CharmFromBSemiForHadronAsy = {
    'BUILDERTYPE'       : 'CharmFromBSemiForHadronAsyAllLinesConf',
    'CONFIG'    :{
    "prescale_LbRS" : 1.0
    ,"prescale_LbWS" : 0.2 
    ,"prescale_D0to3piRS" : 1.0
    ,"prescale_D0to3piWS" : 0.2
    ,"prescale_D0toK2piRS" : 1.0
    ,"prescale_D0toK2piWS" : 0.2 
    #### common to all modes
    ,"GEC_nLongTrk" : 250. # adimensional
    ,"GHOSTPROB_MAX" : 0.35 #adimensional
    ,"Mu_PT" : 800. # MeV
    ,"H_PT"  : 250. # MeV
    ,"Pi_PIDKMax" : 6. # adimensional
    ,"K_PIDKMin"  : 6. # adimensional
    ,"Slowpi_PIDKMax" : 10. # adimensional
    ,"Slowpi_PIDeMax" : 99. # adimensional
    ,"Slowpi_PTMin"   : 200. # MeV
    ##### specific to D0 modes
    ,"MuPi_SUMPT_MIN" : 1300. # MeV
    ,"MuPi_DOCACHI2_MAX" : 8.0 # adimensional
    ,"MuPi_CHI2NDOF_MAX" : 3.0 # adimensional
    ,"MuPi_DIRA_MIN" : -99. # adimensional
    ,"MuPi_FDCHI2_MIN" : 20. # adimensional
    ,"D0to3H_REQUIRE_TOS" : True # bool
    ,"D0to3H_DOCACHI2_MAX" : 10. # adimensional
    ,"D0to3H_VCHI2NDF_MAX" : 3.0 # adimensional
    ,"D0to3H_SUMPT_MIN" : 1800. # MeV
    ,"D0to3H_DZ" : 2.0 # mm
    ,"D0to3H_3pi_DeltaMass_MAX" : 350. # MeV
    ,"D0to3H_K2pi_DeltaMass_MAX" : 250. # MeV
    ,"D0to3H_3pi_MASS_MIN" : 900. # MeV
    ,"D0to3H_3pi_MASS_MAX" : 1400. # MeV
    ,"D0to3H_K2pi_MASS_MIN" : 1300. # MeV
    ,"D0to3H_K2pi_MASS_MAX" : 1800. # MeV
    ,"D0to3H_B_MASS_MIN" : 1800. # MeV
    ,"D0to3H_B_MASS_MAX" : 4900. # MeV
    ,"D0to3H_B_DIRA_MIN" : 0.99 # adimensional
    ,"D0to3H_B_VCHI2NDF_MAX" : 15. # adimensional
    ,"D0to3H_B_DOCACHI2_MAX" : 50. # adimensional
    #### specific to Lambda_c+ modes
    ,"PiPi_MASS_MAX" : 450.  # MeV
    ,"PiPi_DOCACHI2_MAX" : 15. # adimensional
    ,"PiPi_CHI2NDF" : 3. # adimensional
    ,"PiPi_SUMPT_MIN" : 600.  # MeV
    ,"MuPiPi_DOCACHI2_MAX" : 15. 
    ,"MuPiPi_CHI2NDF": 3.
    ,"MuPiPi_FDCHI2_MIN" : 20. # adimensional
    ,"Lc2Kpi_REQUIRE_TOS" : True # bool
    ,"Lc2Kpi_DOCACHI2_MAX" : 10. # adimensional
    ,"Lc2Kpi_VCHI2NDF_MAX" : 3.0 # adimensional
    ,"Lc2Kpi_SUMPT_MIN" : 1500.  # MeV
    ,"Lc2Kpi_FDCHI2_MIN" : 20. # adimensional
    ,"Lc2Kpi_MASS_MIN" : 800.  # MeV
    ,"Lc2Kpi_MASS_MAX" : 1350. # MeV
    ,"Lc2Kpi_DeltaMass_MAX" : 700. # MeV
    ,"Lc2Kpi_DZ" : 1.0 # mm
    ,"Lc2Kpi_B_MASS_MIN" : 2200. # MeV
    ,"Lc2Kpi_B_MASS_MAX" : 4300. # MeV
    ,"Lc2Kpi_B_FDCHI2_MIN" : 20. # adimensional
    ,"Lc2Kpi_B_DIRA_MIN" : 0.99 # adimensional
    ,"Lc2Kpi_B_DOCACHI2_MAX" : 50. # adimensional
    ,"Lc2Kpi_B_VCHI2NDF_MAX" : 15. # adimensional
    },
    'STREAMS' : ['Charm'],
    'WGs'    : ['Semileptonic']
    }
   
B2DMuForTauMu = {
    'BUILDERTYPE'       : 'B2DMuForTauMuconf',
    'CONFIG'    :{
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
    'STREAMS' : {
    'Semileptonic' : ['Strippingb2D0MuXFakeB2DMuForTauMuLine']
    },
    'WGs'    : ['Semileptonic']
    }

Lb2pMuNu = {
    'BUILDERTYPE'       : 'Lb2pMuNuBuilder',
    'CONFIG'    :{
    "GEC_nLongTrk"        : 250.   ,#adimensional
    "TRGHOSTPROB"         : 0.5    ,#adimensional
    #Muon Cuts
    "MuonGHOSTPROB"       : 0.5    ,#adimensional
    "MuonTRCHI2"          : 4.     ,#adimensional
    "MuonP"               : 3000.  ,#MeV
    "MuonPT"              : 1600.  ,#MeV
    "MuonMINIPCHI2"       : 12     ,#adminensional
    #Proton Cuts
    "ProtonTRCHI2"        : 6.     ,#adimensional
    "ProtonP"             : 15000. ,#MeV
    "ProtonPT"            : 1000.  ,#MeV
    "ProtonPIDK"          : 0.     ,#adimensional
    "ProtonPIDp"          : 5.     ,#adimensional
    "ProtonMINIPCHI2"     : 16     ,#adminensional
    #B Mother Cuts
    "BVCHI2DOF"           : 4.     ,#adminensional
    "BDIRA"               : 0.999  ,#adminensional
    "BFDCHI2HIGH"         : 125.   ,#adimensional
    "BPVIPChi2"           : 25.    ,#adminensional
    "pMuMassLow"          : 2250.  ,#MeV
    "pMuMassLowTight"     : 2750.  ,#MeV
    "pMuMassUpper"        : 5600.  ,#MeV
    "pMuPT"               : 1500.  ,#MeV
    "PassymLow"           : -0.4   ,#adminensional
    "PassymLower"         : -0.65  ,#adimensional
    "PassymUpper"         : 0.2     #adimensional
    },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }
