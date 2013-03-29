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
    ,"D_MASS_MIN"    : 600   # MeV                   #  400 (safe)
    ,"D_MASS_MAX"    : 1400  # MeV                   # 1400 (safe)
    ,"D_PT_MIN"      : 1500  # MeV                 
    ,"D_AMAXDOCA"    : 0.5   # mm                  
    ,"D_DOCACHI2_MAX":   6   # adimensional        
    ,"D_VFASPF"      :   4   # adimensional
    ,"D_BPVVDCHI2"   : 100   # adimensional
    ,"Lc_MASS_MIN"   :  750  # MeV
    ,"Lc_MASS_MAX"   : 1350  # MeV
    ,"Lc_PT_MIN"     : 1500  # MeV
    ,"Lc_AMAXDOCA"   : 0.5   # mm
    ,"Lc_DOCACHI2_MAX":  6   # adimensional
    ,"Lc_VFASPF"     :   3   # adimensional    <-
    ,"Lc_BPVVDCHI2"  : 100   # adimensional
    ,"A1_MASS_MIN"   : 400   # MeV
    ,"A1_MASS_MAX"   : 3400  # MeV
    ,"A1_PT_MIN"     : 1500  # MeV
    ,"A1_AMAXDOCA"   : 0.3   # mm              <-
    ,"A1_VFASPF"     :   3   # adimensional    <-
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
    ,"Lb_MASS_MIN"    : 3100  # MeV               <-
    ,"Lb_MASS_MAX"    : 4800  # MeV               <-
    ,"Lb_PT_MIN"      : 1500  # MeV
    ,"Lb_DOCACHI2_MAX": 50    # adimensional
    ,"Lb_VFASPF"      : 10    # adimensional
    ,"Lb_BPVVDCHI2"   : 100   # adimensional
    ,"Lb_DZ"          : 1.0   # mm
    ,"Lb_Lc_deltamass_MIN" : 2000 #MeV             <-            
    ,"Lb_Lc_deltamass_MAX" : 4100 #MeV             <-         
    },
    'STREAMS' : ['BhadronCompleteEvent'],
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
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }
