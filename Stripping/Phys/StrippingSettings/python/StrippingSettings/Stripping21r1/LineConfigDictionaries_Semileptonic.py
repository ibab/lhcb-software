################################################################################
##                          S T R I P P I N G  2 1r1                          ##
##                                                                            ##
##  Configuration for SL WG                                                   ##
##  Contact person: Mika Vesterinen                                           ##
################################################################################

from GaudiKernel.SystemOfUnits import *

B2DMuNuX = {
    'BUILDERTYPE' : 'B2DMuNuXAllLinesConf',
    'CONFIG' : {
        ##### global settings
        "prescales"     : {"b2DsPi_PhiPi_fakes":0.1,
                           "b2D0eX":1.0}
        ,"GEC_nLongTrk" : 250 # adimensional
        ,"TTSpecs"      : {'Hlt1.*Track.*Decision%TOS':0,'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,"Hlt2Global%TIS":0} 
        ,"HLT_FILTER"   : "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision')"
        ##### daughter particles
        ,"TRGHOSTPROB"   : 0.5   # adimensional
        ,"TRCHI2"        : 4     # adimensional
        ,"MuonGHOSTPROB" : 0.5   # adimensional
        ,"MINIPCHI2"     : 4.0   # adimensiional
        ,"ProtonPIDp"    : 0.0   # adimensiional 
        ,"KaonPIDK"      : -5.0  # adimensiional
        ,"PionPIDK"      : 20.0  # adimensiional
        ,"MuonPIDmu"     : 0.0   # adimensiional
        ,"ElectronPIDe"  : 0.0   # adimensiional
        ,"MuonIPCHI2"    : 4.00  # adimensiional
        ,"MuonPT"        : 800.0 # MeV
        ,"HadronPT"      : 250.0 # MeV
        ,"MuonP"         : 6.0   # GeV
        ,"HadronP"       : 2.0   # GeV
        ,"ProtonP"       : 9.0   # GeV
        ###### charm combination
        ,"D_BPVDIRA"     : 0.99  # adimensiional
        ,"D_FDCHI2"      : 25.0  # adimensiional
        ,"D_MassWin"     : 80.0  # MeV
        ,"D_AMassWin"    : 90.0  # MeV
        ,"D_VCHI2DOF"    : 6.0   # adimensiional
        ,"D_DocaChi2Max" : 20   # adimensiional
        ###### D-mu combination
        ,"B_DIRA"         : 0.999  #adimensiional
        ,"BVCHI2DOF"      : 6.0    # adimensiional
        ,"B_D_DZ"         : -0.05  #mm
        ,"BMassMin"       : 2.2 #GeV
        ,"BMassMax"       : 8.0 #GeV
        ,"B_DocaChi2Max"  : 10 #adimensiional
        ###### For D* decays
        ,"Dstar_Chi2"       :  8.0 ## unitless
        ,"Dstar_SoftPion_PIDe" : 2. ## unitless
        ,"Dstar_SoftPion_PT" : 180. ## MeV ###
        ,"Dstar_wideDMCutLower" : 0. ## MeV
        ,"Dstar_wideDMCutUpper" : 170. ## MeV,
        },
    'STREAMS' : {
        'Semileptonic' : [
            'Strippingb2D0MuXB2DMuNuXLine',
            'Strippingb2D0eXB2DMuNuXLine',
            'Strippingb2DpMuXB2DMuNuXLine',
            'Strippingb2DsMuXB2DMuNuXLine',
            'Strippingb2DsKStarKMuXB2DMuNuXLine',
            'Strippingb2DsPhiPiMuXB2DMuNuXLine',
            'Strippingb2LcMuXB2DMuNuXLine'],
        'Charm' : ['Strippingb2Ds3PiMuXB2DMuNuXLine',
                   'Strippingb2DsPi_PhiPi_fakesB2DMuNuXLine']
        },
    'WGs'    : ['Semileptonic']
    }

Bd2DstarMuNu =  {
    'BUILDERTYPE' : 'Bd2DstarMuNuAllLinesConf',
    'CONFIG': {
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
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }                 


B2XuMuNu = {
    'BUILDERTYPE': 'B2XuMuNuBuilder',
    'CONFIG': {
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
        "KaonPIDK_phi"        : 0.    ,#adimensional 
        "KaonPIDmu_phi"       : -2.    ,#adimensional
        "KaonPIDp_phi"        : -2.    ,#adimensional
        "KaonMINIPCHI2"       : 16     ,#adminensional
        
        
        "PhiMassWindow"       : 4000.  ,#MeV
        "PhiVCHI2DOF"         : 6     ,#adimensional
        "PhiPT"               : 500.  ,#MeV
        "PhiMINIPCHI2"        : 4     ,#adimensional
        "PhiDIRA"             : 0.9   ,#adimensional
        
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
        #    "RhoFDCHI2"           : 100.  ,#adimensional
        #    "RhoFD"               : 6     ,#mm
        #    "RhoIPMin"            : 0.3   ,#mm    
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
        "BFDCHI2ForPhi"       : 50.  ,#adimensional
        #B Mass Minima
        "KMuMassLowTight"     : 1500. ,#MeV
        "PhiMu_MCORR"         : 2500. ,#MeV
        "RhoMuMassLowTight"   : 2000. ,#MeV
        "KshMuMassLowTight"   : 3000. ,#MeV
        "KstarMuMassLowTight" : 2500. ,#MeV
        #
        "XMuMassUpper"        : 5500. ,#MeV
        "XMuMassUpperHigh"    : 6500. ,#MeV
        "Enu"                 : 1850. ,#MeV
        "EnuK"                : 2000. ,#MeV
        'KshZ'                : 0.    ,#mm
        'PiMuNu_prescale'     : 0.1    
        },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }                 


Lb2pMuNuVub = {
    'BUILDERTYPE' : 'Lb2pMuNuVubBuilder',
    'CONFIG':{
        "GEC_nLongTrk"        : 250.   ,#adimensional
        "TRGHOSTPROB"         : 0.35   ,#adimensional
        #Muon Cuts
        "MuonGHOSTPROB"       : 0.35   ,#adimensional
        "MuonTRCHI2"          : 4.     ,#adimensional
        "MuonP"               : 3000.  ,#MeV
        "MuonPT"              : 1500.  ,#MeV
        "MuonMINIPCHI2"       : 16.    ,#adminensional
        #Proton Cuts 
        "ProtonTRCHI2"        : 6.     ,#adimensional
        "ProtonP"             : 15000. ,#MeV
        "ProtonPT"            : 1000.  ,#MeV
        "ProtonPIDK"          : 10.     ,#adimensional 
        "ProtonPIDp"          : 10.     ,#adimensional
        "ProtonMINIPCHI2"     : 16.    ,#adminensional
        #B Mother Cuts
        "BVCHI2DOF"           : 4.     ,#adminensional
        "BDIRA"               : 0.994  ,#adminensional
        "BFDCHI2HIGH"         : 150.   ,#adimensional
        "pMuMassLower"        : 1000.  ,#MeV
        "pMuPT"               : 1500.  ,#MeV
        "pMuMCORR"            : 4000.   #MeV
        },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }                 



B23MuNu = {
    'BUILDERTYPE' : 'B23MuNuConf',
    'CONFIG':{
        #  (dimu) cuts
        'FlightChi2'      :    50.0,
        'DIRA'            :   0.999,
        'BPT'             :  2000.0,
        'VertexCHI2'      :     4.0,
        'LOWERMASS'       :     0.0, # MeV
        'UPPERMASS'       :  7500.0, # MeV
        'CORRM_MIN'       :  2500.0, # MeV
        'CORRM_MAX'       : 10000.0, # MeV
        # Track cuts
        'Track_CHI2nDOF'      :    3.0,
        'Track_GhostProb'     :    0.35,  
        # Muon cuts
        'Muon_MinIPCHI2'   :    9.0,
        'Muon_PIDmu'       :    0.0,
        'Muon_PIDmuK'      :    0.0,
        'Muon_PT'          :    0,
        # GEC
        'SpdMult'             :  600,
        },
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }                 


B2DMuNuForTauMu = {
    'BUILDERTYPE' : 'B2DMuForTauMuconf',
    'CONFIG':{
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


B2XTauNu = {
    'BUILDERTYPE' : 'B2XTauNuAllLinesConf',
    'CONFIG':{
        'Prescale_B0d2DstarTauNu'        : 1.0,
        'Prescale_B0d2DTauNu'            : 1.0,
        'Prescale_Bu2D0TauNu'            : 1.0,
        'Prescale_B0s2DsTauNu'           : 1.0,
        'Prescale_Bc2JpsiTauNu'          : 1.0,
        'Prescale_Lb2LcTauNu'            : 1.0,
        'Prescale_B0d2DdoubleStarTauNu'  : 1.0,
        'Prescale_NonPhys'               : 0.1,
        'Postscale'   : 1.0 ,
        #B cuts
        'B_BPVDIRA'       : 0.995 ,
        #B combination cuts
        'B_DeltaM_low'       : -2579.0, #MeV
        'B_DeltaM_high'      : 300.0,   #MeV
        'B_DOCAMAX'          : 0.15 , #mm
        'B_upperDeltaM_low'  : 720.0,   #MeV
        'B_upperDeltaM_high' : 1721.0,  #MeV
        #D kaon cuts
        'D_K_PT'          : 150.0,  #MeV
        'Dplus_K_PT'      : 1500.0, #MeV
        'Ds_K_PT'         : 1500.0, #MeV
        'D_K_TRCHI2DOF'   : 30.0,
        'D_K_IPCHI2'      : 10.0,
        'D_K_PIDK'        : -3 ,
        'Dplus_K_PIDK'    : 3,
        'Dplus_K_TRPCHI2' : 0.1,
        #D pion cuts
        'D_Pi_PT'         : 150.0, # MeV
        'D_Pi_TRCHI2'     : 3.0,
        'D_Pi_IPCHI2'     : 10.,
        'D_Pi_PIDK'       : 50.0,
        'Dplus_Pi_TRPCHI2' : 0.1,
        #Lc kaon cuts
        'Lc_K_PT'          : 150.0,  #MeV
        'Lc_K_TRCHI2DOF'   : 3.0,
        'Lc_K_IPCHI2'      : 10.0,
        'Lc_K_PIDK'        : 3.0,    
        'Lc_K_TRPCHI2' : 0.01,
        #Lc pion cuts
        'Lc_Pi_PT'         : 150.0, # MeV
        'Lc_Pi_TRCHI2'     : 3.0,
        'Lc_Pi_IPCHI2'     : 10.0,
        'Lc_Pi_PIDK'       : 50.0,
        'Lc_Pi_TRPCHI2' : 0.01,
        #Lc proton cuts
        'Lc_p_PT'          : 150.0,  #MeV
        'Lc_p_TRCHI2DOF'   : 3.0,
        'Lc_p_IPCHI2'      : 10.0,
        'Lc_p_PIDp'        : 5.0,
        'Lc_p_TRPCHI2' :0.01,
        
        
        #D0-resonance parameters
        'D0_MassW'        : 40.0,      #MeV, 100 in StdLooseD0 
        'D0_BPVVDCHI2'    : 36.0,      #36 in StdLooseD0
        'D0_DIRA'         : 0.995,
        'D0_PT'           : 1200.0,    #MeV, 1.2 GeV in StdLooseD02KPi
        'D0_VCHI2'        : 10.0,      #10 in StdLooseD0
        #D0 combination cut
        'D0_DOCAMAX'      : 0.5,     #mm, 0.5 mm in StdLooseD0
        #D-resonance parameters
        'D_MassW'         : 40.0,      #MeV, 100 in StdLooseDplus
        'D_BPVVDCHI2'     : 50.0,      #36 in StdLooseDplus
        'Ds_BPVVDCHI2'    : 36.0,      #shorter lifetime than Dplus 
        'D_DIRA'          : 0.995,   #0.98 in StdLooseDplus
        'D_PT'            : 1600.0,    #MeV, 1.2 in StdLooseDplus
        'D_VCHI2'         : 10.0,      #30 in StdLooseDplus
        'D_MIPCHI2'       : 10.0,
        #Dstar-resonance parameters
        'slowPi_TRCHI2DOF' : 30.0,
        'slowPi_PT'        : 50.0,    #MeV
        'Dstar_PT'         : 1250.0,   #MeV, 1250 in StdLooseD*
        'Dstar_VCHI2'      : 25.0,     #MeV, 25 in StdLooseD*WithD02KPi 
        'Dstar_MassW'      : 50.0,     #MeV, 50 MeV in StdLooseD*WithD02KPi
        'Dstar_DeltaM_low'  : 135.0,  #MeV
        'Dstar_DeltaM_high' : 150.0,   #MeV, 165.5 in StdLooseD*
        
        #Lc-resonance parameters
        'Lc_MassW'         : 30.0,      #MeV, 100 in StdLooseDplus
        'Lc_BPVVDCHI2'     : 50.0,      #36 in StdLooseDplus
        'Lc_DIRA'          : 0.995,   #0.98 in StdLooseDplus
        'Lc_PT'            : 1200.0,    #MeV, 1.2 in StdLooseDplus
        'Lc_VCHI2'         : 10.0,      #30 in StdLooseDplus
        'Lc_MIPCHI2'       : 10.0,
        # Lb Inverted vertex Cuts
        'LbInvVertD'       : 1. ,         #mm
        #Dstar Inverted cut
        'DInvVertD'        : 1. ,         #mm
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
        'TRGHP_slowPi'      : 0.6,
        #HLT1 trigger
        'TisTosSpecs'       : { 'Hlt1TrackAllL0Decision%TOS' : 0 }    
        },
    'STREAMS' : ['BhadronCompleteEvent'],
    'WGs'    : ['Semileptonic']
    }                 

CharmFromBSemiForHadronAsy = {
    'BUILDERTYPE' : 'CharmFromBSemiForHadronAsyAllLinesConf',
    'CONFIG':{
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
        ,"PiPi_MASS_MAX" : 500.  # MeV
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
    'STREAMS' : ['Semileptonic'],
    'WGs'    : ['Semileptonic']
    }                 


TrackEffD0ToK3Pi = {
    'BUILDERTYPE':'TrackEffD0ToK3PiAllLinesConf',
    'CONFIG':{
        "HLT" : "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
        "TTSpecs" : {'Hlt2.*CharmHadD02HHXDst.*Decision%TOS':0},
        "VeloLineForTiming":False,
        "VeloFitter":"SimplifiedGeometry",
        "DoVeloDecoding": False,
        "RequireDstFirst":False,
        "Dst_MAX_M":2.035*GeV,
        "Dst_MAX_DTFCHI2":3.0,
        "Sc_MAX_M":2.5*GeV,
        "Sc_MAX_DTFCHI2":3.0,
        "D0_MIN_FD":5.0*mm,
        "LC_MIN_FD":2.0*mm,
        "D0_MAX_DOCA":0.05*mm,
        "VeloMINIP":0.05*mm,
        "Kaon_MIN_PIDK":7,
        "Pion_MAX_PIDK":4
        },
    'STREAMS' : ['Charm'],
    'WGs'    : ['Semileptonic']
    }                 

D0forBXX = {
    'BUILDERTYPE':'D0forBXXLinesConf',
    'CONFIG':{
        "PreScale"      : 0.005   # adimensiional        
        ,"PreScaleRS"   : 1.0    # adimensiional
        ,"PreScaleWS"   : 0.2    # adimensiional
        ,"MINIPCHI2"     : 9.0   # adimensiional
        ,"TRCHI2"        : 3.0   # adimensiional
        ,"KaonPIDK"      : 4.0   # adimensiional
        ,"PionPIDK"      : 10.0   # adimensiional
        ,"PT"            : 300.0  # MeV
        ,"D0DIRA"        : 0.9999    # adimensiional
        ,"D0DIRAK3Pi"    : 0.9999    # adimensiional        
        ,"FDCHI2"        : 100.0   # adimensiional
        ,"D0AMassWin"    : 100.0   # MeV
        ,"D0MassWin"     : 80.0  # MeV
        ,"D0IP"          : 0.2    #mm
        ,"D0K3PiIP"      : 0.2    #mm        
        ,"D0VCHI2DOF"    : 6.0   # adimensiional
        ,"DDocaChi2Max" : 20   #adimensiional
        },
    'STREAMS' : ['CharmCompleteEvent'],
    'WGs'    : ['Semileptonic']
    }                 


JPsiForSL = {
    'BUILDERTYPE':'JPsiForSLAllLinesConf',
    'CONFIG':{
        "PrescaleSemiIncJpsi2mumu" : 1.0 ## adimensional
        ,"TrackGhostProbMax" : 0.35 # adimensional
        ,"MuonGhostProbMax" : 0.5 # adimensional
        ,"MuonPT" : 800. # MeV
        ,"MuonP" : 3000. # MeV
        ,"MuonChi2" : 3. # adimensional
        ,"MuonIPChi2" : 4. # adimensional
        ,"TrackPT" : 300. # MeV
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
        ,"BCombMassMin" : 3000 # MeV
        ,"BCombMassMax" : 6000 # MeV
        ,"BDocaChi2Max" : 10. # adimensional
        ,"BVChi2NdofMax" : 3. # adimensional
        ,"BPTMin" : 800. # MeV
        ,"BFDChi2Min" : 200. # adimensional
        ,"BDIRAMin" : 0.999 # adimensional
        ,"nLongTrackMax" : 250 # adimensional
        },
    'STREAMS' : ['PID'],
    'WGs'    : ['Semileptonic']
    }                 

PhiToKSKS = {
    'BUILDERTYPE':'PhiToKSKSAllLinesConf',
    'CONFIG':{
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
        "Mu_IPCHI2_MAX" : 9, #adimensional
        
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

    
ProtonAsym = {
    "BUILDERTYPE": "ProtonAsymBuilder",
        "CONFIG": {
        # Are the candidates to be stored on microDST, as opposed to full?
        "MicroDST":  True,

        # Prescales and postscales set the fraction of candidates to randomly
        # throw away before and after stripping, respectively
        "PrescaleBd2LcppipiLc2Kpi": 1.0,
        "PostscaleBd2LcppipiLc2Kpi": 1.0,
        "PrescaleBu2LcppipipiLc2Kpi": 1.0,
        "PostscaleBu2LcppipipiLc2Kpi": 1.0,
        "PrescaleLb2LcpiLc2Kpi": 1.0,
        "PostscaleLb2LcpiLc2Kpi": 1.0,
        "PrescaleLb2LcpipipiLc2Kpi": 1.0,
        "PostscaleLb2LcpipipiLc2Kpi": 1.0,
        "PrescaleSc2LcpipLc2Kpi": 1.0,
        "PostscaleSc2LcpipLc2Kpi": 1.0,
        "PrescaleSc2LcpimLc2Kpi": 1.0,
        "PostscaleSc2LcpimLc2Kpi": 1.0,
        "PrescaleLcst2LcpipiLc2Kpi": 1.0,
        "PostscaleLcst2LcpipiLc2Kpi": 1.0,

        # HLT filters require the events passing the line to have a positive
        # decision on the trigger line(s) matching the regex
        "HLTBd2LcppipiLc2Kpi": None,
        "HLTBu2LcppipipiLc2Kpi": None,
        "HLTLb2LcpiLc2Kpi": None,
        "HLTLb2LcpipipiLc2Kpi": None,
        "HLTSc2LcpipLc2Kpi": "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
        "HLTSc2LcpimLc2Kpi": "HLT_PASS_RE('Hlt2.*CharmHadD02HHXDst.*Decision')",
        "HLTLcst2LcpipiLc2Kpi": None,

        # Partial Lambda_c mass window, taken from TGenPhaseSpace studies
        "LcPartialMassMin": 700*MeV, #620 is the minimum -> cut a bit harder where most of bkg is
        "LcPartialMassMax": 1360*MeV,
        # Minimum sum of Lambda_c daughters' tranverse momentum
        "LcPartialPTSum": 1500.0*MeV,
        # Minimum Lambda_c flight distance fit quality per DOF
        "LcPartialFDCHI2": 36.0, #50
        # Maximum Lambda_c pairwise daughter distance of closest approach
        "LcPartialDOCACHI2": 15.0, #20
        # Maximum Lambda_c vertex fit quality per DOF
        "LcPartialVCHI2DOF": 10.0, #50

        # Partial Sigma_c mass window, taken from TGenPhaseSpace studies
        "ScPartialMassMin": 1000*MeV,
        "ScPartialMassMax": 1600*MeV,
        # Minimum Lambda_c vertex displacement wrt Sigma_c vertex along z
        "ScMinZ": 0.2*mm,
        # Sigma_c maximum vertex quality per DOF
        "ScVCHI2DOF": 10.0, #15
        # Maximum Sigma_c impact parameter chi2
        "ScBPVIPCHI2": 15.0, #20

        # Partial Lambda_c* mass window, taken from TGenPhaseSpace studies
        "LcstPartialMassMin": 1300*MeV,
        "LcstPartialMassMax": 1660*MeV,
        # Minimum Lambda_c vertex displacement wrt Lambda_c* vertex along z
        "LcstMinZ": 0.2*mm,
        # Lambda_c* maximum vertex quality per DOF
        "LcstVCHI2DOF": 10.0,
        # Maximum Lambda_c* impact parameter chi2
        "LcstBPVIPCHI2": 15.0, #20

        # Partial Lambda_b0 mass window, taken from TGenPhaseSpace studies
        "LbToLc1piPartialMassMin": 3000*MeV,
        "LbToLc3piPartialMassMin": 3500*MeV,
        "LbPartialMassMax": 4700*MeV,
        # Minimum transverse momentum of combined daughters
        "LbPTSum": 2000*MeV,
        # Lambda_b0 maximum vertex quality
        "LbVCHI2DOF": 20.0, #15.00
        # Minimum Lambda_c vertex displacement wrt Lambda_b vertex along z
        "LbMinZ": 0.2*mm,
        # Maximum Lambda_b impact parameter chi2
        "LbBPVIPCHI2": 15.0, #20
        # Minimum Lambda_b flight distance fit quality per DOF
        "LbFDCHI2": 36.0, #50    

        # Partial B0/B+ mass window, taken from TGenPhaseSpace studies
        "BPartialMassMin": 3500*MeV,
        "BPartialMassMax": 4350*MeV,
        # Minimum transverse momentum of combined daughters
        "BPTSum": 2000*MeV,
        # B0/B+ maximum vertex quality
        "BVCHI2DOF": 20.0, #15
        # Minimum Lambda_c vertex displacement wrt B0/B+ vertex along z
        "BMinZ": 0.2*mm,
        # Maximum B0/B+ impact parameter chi2
        "BBPVIPCHI2": 15.0, #20
        # Minimum B0/B+ flight distance fit quality per DOF
        "BFDCHI2": 36.0, #50

        # Minimum final state track momentum
        "TrackP": 1000.0*MeV, #3000
        # Minimum final state track transverse momentum
        "TrackPT": 100.0*MeV, #500
        # Maximum final state track impact parameter chi^2
        "TrackIPCHI2": 12.0, #20
        # Minimum Kaon ANN PID
        "ProbNNk": 0.2,
        # Minimum Pion ANN PID
        "ProbNNpi": 0.2,
        # Minimum Proton ANN PID
        "ProbNNp": 0.2,

        #Window for full reconstruction
        "LcMinWin": 2206*MeV,
        "LcMaxWin": 2366*MeV,

        "BuMinWin": 5000*MeV,
        "BuMaxWin": 6500*MeV,
        "LbMinWin": 5200*MeV,
        "LbMaxWin": 6500*MeV,
        "ScMinWin": 2053*MeV,
        "ScMaxWin": 2753*MeV,
        "LcstMinWin": 2292*MeV,
        "LcstMaxWin": 2892*MeV
        },
    "STREAMS": {
        "PID": [
            "StrippingBd2LcppipiLc2KpiProtonAsymLine",
            "StrippingBu2LcppipipiLc2KpiProtonAsymLine",
            "StrippingLb2LcpiLc2KpiProtonAsymLine",
            "StrippingLb2LcpipipiLc2KpiProtonAsymLine",
            "StrippingSc2LcpipLc2KpiProtonAsymLine",
            "StrippingSc2LcpimLc2KpiProtonAsymLine",
            "StrippingLcst2LcpipiLc2KpiProtonAsymLine",
            "StrippingBd2LcppipiLc2pKpiProtonAsymLine",
            "StrippingBu2LcppipipiLc2pKpiProtonAsymLine",
            "StrippingLb2LcpiLc2pKpiProtonAsymLine",
            "StrippingLb2LcpipipiLc2pKpiProtonAsymLine",
            "StrippingSc2LcpipLc2pKpiProtonAsymLine",
            "StrippingSc2LcpimLc2pKpiProtonAsymLine",
            "StrippingLcst2LcpipiLc2pKpiProtonAsymLine"
            ]
        },
    "WGs": ["Semileptonic"]
    }
