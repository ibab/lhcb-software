"""
Line config dictionaries for the Charm WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *

#D+ -> pi+ (pi0->e+ e- gamma) lines, prompt and smmileptonic, Ed Greening, Brian Meadows
#MDST
D2PiPi0_eegamma = {
    'BUILDERTYPE' : 'D2PiPi0Conf' ,
    'CONFIG'      : {
      'PromptHPi0' : {
        'Daug_IPCHI2_MIN'    : 25         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 350  *MeV
        ,'Daug_P_MIN'        : 1000 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 5          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 70   *MeV
        ,'ResMass_MAX'       : 210  *MeV
        ,'D_PT_Min'          : 2000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 5          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 5          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       :-1          # when kaon in the decay 
        },
    'PromptHEta' : {
        'Daug_IPCHI2_MIN'    : 25         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 600  *MeV
        ,'Daug_P_MIN'        : 1000 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 5          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 500   *MeV
        ,'ResMass_MAX'       : 600  *MeV
        ,'D_PT_Min'          : 2000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 5          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 5          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       :-1          # when kaon in the decay 
        },
    'PromptHEtaPrime' : {
        'Daug_IPCHI2_MIN'    : 25         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 1000  *MeV
        ,'Daug_P_MIN'        : 1000 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 5          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 900  *MeV
        ,'ResMass_MAX'       : 990  *MeV
        ,'D_PT_Min'          : 2000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 5          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 5          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       :-1          # when kaon in the decay 
        },
    'SLHPi0' : {
        'Daug_IPCHI2_MIN'    : 9         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 150  *MeV
        ,'Daug_P_MIN'        : 500 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 10          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 70   *MeV
        ,'ResMass_MAX'       : 210  *MeV
        ,'D_PT_Min'          : 1000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 15          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 10          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       : -1          # when kaon in the decay
        ,'MuonPID_MIN'       : 0
        ,'BMass_MIN'         : 2200 *MeV
        ,'BMass_MAX'         : 6300 *MeV
        }
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
    
    
##D+ -> pi+ (pi0->e+ e- gamma) lines, prompt and smmileptonic, Ed Greening, Brian Meadows
##FULLDST
DstarToHHPi0 = {
    'BUILDERTYPE' : 'DstarD0ToHHPi0AllLinesConf' ,
    'CONFIG'      : {
      "prescale_Pi0R_WIDEMASS" : 0.1 #adimensional
    ,"Pi0R_PT" : 500  # MeV
    ,"Pi0R_DMASS" : 15  # MeV
    ,"Pi0M_PT" : 1000 #MeV
    ,"Pi0_MassConstraint" : False # adimensional
    ,"D0_M_WINDOW" : 150 #MeV
    ,"DELTA_MASS_MAX" : 180 #MeV
    ,"Pion_PIDK" : 0 #adimensional
    ,"Kaon_PIDK" : 7 #adimensional
    ,"Pion_PT" : 500 #MeV
    ,"Kaon_PT" : 500 #MeV
    ,"Slowpion_PT" : 300 #MeV
    ,"Pion_TRGHOSTPROB" : 0.35 #adimensional
    ,"Kaon_TRGHOSTPROB" : 0.35 #adimensional
    ,"Slowpion_TRGHOSTPROB" : 0.35 #adimensional
    ,"Slowpion_PIDe" : 5 #adimensional
    ,"D0_APT" : 1400  #MeV
    ,"useTOS" : True  #adimensional
    ,"useHLT" : True  #adimensional
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
    
    
# CharmFromBSemi Lines
# Micro-DST, Mika Vesterinen, Dan Johnson (addition of KSpipi0 lines)
# Moved to this file in Stripping20
# THIS IS A SPECIAL VERSION FOR Stripping20rXp2 WHICH GOES TO FULLDST INSTEAD OF MDST!!!
# Lives in StrippingCharmFromBSemi.py
CharmFromBSemi = {
    'BUILDERTYPE' : 'CharmFromBSemiAllLinesConf',
    'CONFIG' :  {
    "GEC_nLongTrk"   : 250    # adimensional
    ,"TrGhostProbMax": 0.5    # adimensional
    ,"MinBMass"      : 2500   # MeV
    ,"MaxBMass"      : 6000   # MeV
    ,"MinBMassTight" : 2800   # MeV
    ,"MaxBMassTight" : 5100   # MeV
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.0    # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 250.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"Dto4h_MassWin" : 40.0   # MeV
    ,"Dto4h_AMassWin": 42.0   # MeV
    ,"DsIP"          : 7.4    # mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  # adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZ"            : 0      #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose": 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,'KSCutZFDFromD' :   10.0  #mm
    ,'KSLLPMin'         : 2000  ## MeV
    ,'KSLLPTMin'        : 250   ## MeV
    ,'KSDDPMin'         : 3000  ## MeV
    ,'KSDDPTMin'        : 250   ## MeV
    ,'KSLLCutMass'      : 30    ## MeV
    ,'KSDDCutMass'      : 30    ## MeV
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
    ,"Pi0PtMinLoose"    : 1000    # Minimum Pt of pi0 (MeV)
    ,"Pi0PtMin"         : 1300   # Minimum Pt of pi0 (MeV)
    ,"Pi0PMin"          : 3000   # Minimum P of pi0 (MeV)
    ,"PhotonCL"         : 0.25   # Confidence level for Pi0 photons
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 210  # MeV (mass window after vertex fit)
    ,"D02KSHHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02KSHHPi0MassWin"  : 210  # MeV (mass window after vertex fit)
    ,"D02KSHHPi0_D0PTComb"	: 1000 # MeV (PT of D0 combination)
    ,"D02KSHHPi0_D0PT"	: 1000 # MeV (PT of D0 combination)
    ,"D02KSHHPi0_PTSUMLoose"  : 1000 # MeV (sum PT of D0 daughters)
    ,"D02HHPi0DocaCut"  : 6.0    # mm
    ,"Dstar_Chi2"       :  8.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless
    ,"Dstar_SoftPion_PT" : 180. ## MeV ### 
    ,"Dstar_wideDMCutLower" : 0. ## MeV
    ,"Dstar_wideDMCutUpper" : 170. ## MeV
    ,"PTSUMLoose"  : 1400. ## MeV
    ,"PTSUM"       : 1800. ## MeV
    ,"PTSUM_HHPi0" : 1800. ## MeV
    },
    #'STREAMS' : ['CharmCompleteEvent'],
    'STREAMS' : {'CharmCompleteEvent' : [
      'Strippingb2D0MuXKsPiPiPi0MergedLLCharmFromBSemiLine',
			 'Strippingb2D0MuXKsPiPiPi0MergedDDCharmFromBSemiLine',
			 'Strippingb2D0MuXKsPiPiPi0ResolvedLLCharmFromBSemiLine',
			 'Strippingb2D0MuXKsPiPiPi0ResolvedDDCharmFromBSemiLine',
			 'Strippingb2DstarMuXKsPiPiPi0MergedLLCharmFromBSemiLine',
			 'Strippingb2DstarMuXKsPiPiPi0MergedDDCharmFromBSemiLine',
			 'Strippingb2DstarMuXKsPiPiPi0ResolvedLLCharmFromBSemiLine',
			 'Strippingb2DstarMuXKsPiPiPi0ResolvedDDCharmFromBSemiLine',
    ]} ,
    'WGs'    : ['Charm']
    }

## Malcolm John and Benoit Viaud
## Modified from Strippinb13b
## Full DST needed for isolation cuts.
## Test:  Yes
D2XMuMu = {
   'BUILDERTYPE' : 'D2XMuMuConf',
    'CONFIG' : {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 400.  ,    #MeV
    'MuonMINIPCHI2' : 6     ,    #adminensional
    'MuonTRCHI2'    : 3     ,    #adminensional
    'PionP'         : 3000. ,    #MeV
    'PionPT'        : 300.  ,    #MeV
    'PionMINIPCHI2' : 6     ,    #adminensional
    'PionTRCHI2'    : 3     ,    #adminensional
    'KaonP'         : 3000. ,    #MeV
    'KaonPT'        : 300.  ,    #MeV
    'KaonPIDK'      : -1.   ,    #adimensional
    'KaonMINIPCHI2' : 6     ,    #adminensional
    'KaonTRCHI2'    : 3     ,    #adminensional
    'ProtonP'         : 3000. ,    #MeV
    'ProtonPT'        : 300.  ,    #MeV
    'ProtonMINIPCHI2' : 6     ,    #adminensional
    'ProtonTRCHI2'    : 3     ,    #adminensional
    'DVCHI2DOF'     : 6     ,    #adminensional
    'DMAXDOCA'      : 0.25  ,    #mm
    'DIPCHI2'       : 30    ,    #adimensional
    'MINIPCHI2_hhmumu' : 2. ,  #adminensional
    'PT_hhmumu'        : 300 , #MeV
    'MuonPIDmu_hhmumu' : -5,   #adimensional
    'DPT_hhmumu'           : 2500.,     #MeV
    'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional
    'DMAXDOCA_hhmumu'      : 0.25  ,    #mm
    'DdauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
    'DFDCHI2_hhmumu'       : 36     ,   #adimensional
    'DIPCHI2_hhmumu'       : 25    ,    #adimensional
    'DIPCHI2_4bodyCS'      : 25    ,    #adimensional
    'MINIPCHI2_4bodyCS' : 3. ,  #adminensional
    'DDIRA'         : 0.9999,    #adimensional
    'DimuonMass'    : 250.  ,    #MeV
    'DMassWin'                   : 200.  ,    #MeV, mass window
    'DMassWin_CS_hhmumu'         : 80.  ,    #MeV, mass window
    'LambdacMassWin'             : 80. , #MeV, mass window
    'DMassLow'                   :1763.  ,    #MeV, low-mass veto
    'GhostProbCut_hhmumu'     : 0.5,
    'D2PiMuMuOSLinePrescale'     : 0 ,
    'D2PiMuMuOSLinePostscale'    : 0 ,
    'D2PiMuMuSSLinePrescale'     : 0 ,
    'D2PiMuMuSSLinePostscale'    : 0 ,
    'D2KMuMuOSLinePrescale'      : 0 ,
    'D2KMuMuOSLinePostscale'     : 0 ,
    'D2KMuMuSSLinePrescale'      : 0 ,
    'D2KMuMuSSLinePostscale'     : 0 ,
    'D2PiPiPiCalLinePrescale'    : 0 ,
    'D2PiPiPiCalLinePostscale'   : 0 ,
    'D2K2PiLinePrescale'         : 0 ,
    'D2K2PiLinePostscale'        : 0 ,
    'D22KPiLinePrescale'         : 0 ,
    'D22KPiLinePostscale'        : 0,
    'D02KKMuMuLinePrescale'      : 0 ,
    'D02KKMuMuLinePostscale'     : 0 ,
    'D02PiPiMuMuLinePrescale'    : 0 ,
    'D02PiPiMuMuLinePostscale'   : 0,
    'D02KPiMuMuLinePrescale'     : 0 ,
    'D02KPiMuMuLinePostscale'    : 0,
    'D02KKPiPiLinePrescale'      : 0.1 ,
    'D02KKPiPiLinePostscale'     : 1,
    'D02K3PiLinePrescale'        : 0.03 ,
    'D02K3PiLinePostscale'       : 1,
    'D02PiPiPiPiLinePrescale'    : 0.075 ,
    'D02PiPiPiPiLinePostscale'   : 1,
    'Lambdac2PMuMuLinePrescale'  : 0 ,
    'Lambdac2PMuMuLinePostscale' : 0 ,
    'Lambdac2PPiPiLinePrescale'  : 0,
    'Lambdac2PPiPiLinePostscale' : 0
  } ,
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm' ]
  }



## Benoit Viaud
## Added in Stripping20
## Control channels for Tagged D02H(H)Mumu.
## Uses in StrippingDstarPromptWithD02HHHH.py
DstarPromptWithD02HHMuMuControl = {
   'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG' : {
       'CombDelmLower'     : -9.*MeV
  ,'CombDelmUpper'    : 20*MeV
  ,'DelmLower'        : -8*MeV
  ,'DelmUpper'        : 18*MeV
  ,'CombD0MassWin'    : 120*MeV
  ,'D0MassWin'        : 100*MeV
  ,'DstarDOCA'        : 0.3*mm
  ,'D0MaxDOCA'        : 0.3*mm
  ,'D0DauPt'          : 300*MeV
  ,'D0DauP'           : 3*GeV
  ,'D0Pt'             : 2.*GeV
  ,'DstarPt'          : 2.*GeV
  ,'SlowPionPt'       : 120*MeV
  ,'DstarVtxChi2DOF'  : 20
  ,'D0VtxChi2DOF'     : 20
  ,'D0DauMaxIPChi2'   : 9
  ,'D0DauIPChi2'      : 3.0 # !!!
  ,'D0FDChi2'         : 30
  ,'D0IPChi2'         : 36
  ,'D0DIRA'           : 0.9998
  ,'TrackChi2DOF'     : 3
  ,'ApplyKaonPIDK'    : False
  ,'KaonPIDK'         : 0.
  ,'ApplyPionPIDK'    : False
  ,'PionPIDK'         : 3 # not applied to the slow pion
  ,'CheckPV'          : True # reject events with no PVs
  ,'ApplyGECs'        : False # set to False to apply no GECs
  ,'MaxLongTracks'    : 250
  ,'MaxVeloTracks'    : None
  ,'MaxSpdDigits'     : None
  ,'MaxITClusters'    : None
  ,'ApplyGhostProbCut': True # !!!
  ,'GhostProbCut'     : 0.5 # !!!
  ,'Prescale'         : 0.1
  ,'Postscale'        : 1
  ,'RunK3pi'          : True
  ,'Run4pi'           : True
  ,'Run2K2pi'         : True
  ,'Run3Kpi'          : True
  } ,
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm' ]
  }


## Micro-DST, Sam Gregson
## Test:  Yes
D2KS0H = { 
    'BUILDERTYPE' : 'D2KS0HConf' ,
    'CONFIG'      : {
    'KS0DaugP'                  : 2000  # Cut in common particles
    ,'KS0DaugPT'                :  200
    ,'KS0DaugTrackChi2'         :    4  
    ,'KS0DaugMIPChi2'           :   40
    ,'KS0DaugPIDK'              :   10
    ,'KS0MassWindow'            :   35  # Done by stdLoose KS0s
    ,'KS0MIPChi2'               :    7  # Updated by Sam - 20/03/12
    ,'KS0MIPChi2DD'             :    0
    ,'KS0VertexChi2'            :   10  
    ,'KS0PT'                    : 1000
    ,'KS0BPVVDCHI2'             :  300
    ,'KS0BPVVDCHI2DD'           :  200
    ,'BachPionP'                : 2000               
    ,'BachPionPT'               :  200     
    ,'BachPionTrackChi2'        :    3
    ,'BachPionMIPChi2'          :   15
    ,'BachPionPIDK'             :    3 # change from 0 by Hamish and Mika for Stripping20rXp2
    ,'BachKaonP'                : 2000                 
    ,'BachKaonPT'               :  200     
    ,'BachKaonTrackChi2'        :    3
    ,'BachKaonMIPChi2'          :   15
    ,'BachKaonPIDK'             :    0
    ,'DMesonComboLowMass'       : 1760          
    ,'DMesonComboHighMass'      : 2080
    ,'DMesonComboDOCA'          :    0.6
    ,'DMesonAPT'                : 1000
    ,'DMesonADOCAChi2'          :   11
    ,'DMesonMotherLowMass'      : 1770
    ,'DMesonMotherHighMass'     : 2070
    ,'DMesonMotherVertexChi2'   :   10
    ,'DMesonMotherMIPChi2'      :   15  
    ,'DMesonMotherPT'           : 1000
    ,'KS0ZDiff'                 :   10
    ,'DMesonFlightDistChi2'     :    5 
    ,'PionLinePrescale'         :    1 
    ,'KaonLinePrescale'         :    1 
    ,'PionLinePrescaleDD'       :    1
    ,'KaonLinePrescaleDD'       :    1
    ,'PionLinePostscale'        :    1 
    ,'KaonLinePostscale'        :    1
    ,'PionLinePostscaleDD'      :    1 
    ,'KaonLinePostscaleDD'      :    1
    ,"UseTOS" : True
    ,"TisTosSpecs_KS0Pi" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0PiDecision%TOS":0}
    ,"TisTosSpecs_KS0PiDD" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0DDPiDecision%TOS":0}
    ,"TisTosSpecs_KS0K" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0KDecision%TOS":0}
    ,"TisTosSpecs_KS0KDD" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0DDKDecision%TOS":0}
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : { 
        'Charm' : ['StrippingD2KS0HPionLine',
                   'StrippingD2KS0HPionLineDD']
        }
    }
    
    
##Absolute Lc BF, Andrea Contu
##MDST
B2ppipiSigmacmm_Lcpi = {
    'BUILDERTYPE' : 'StrippingB2ppipiSigmacmm_Lcpi_Conf' ,
    'CONFIG'      : {
        'protonMINP':10.0*GeV
        ,'protonMINIPCHI2':6.0
        ,'protonMINPT': 500.0*MeV
        ,'protonMINPIDp':10.0
        ,'protonMINPIDp_K':0.0
        ,'pionMINP':5.0*GeV
        ,'pionMINPT':500.0*MeV
        ,'pionMINIPCHI2':6.0
        ,'piSigmaMINP':2.0*GeV
        ,'piSigmaMINPT':150.0*MeV
        ,'piSigmaMAXP':10.0*GeV
        ,'piSigmaMAXPT':1.0*GeV
        ,'piSigmaMINIPCHI2':8.0
        ,'ApplyDeltaMCut' : False
        ,'DelmUpper':3.0*GeV
        ,'DelmLower':0.0
        ,'BMassWind':200.0*MeV
        ,'LcMassWind':100.0*MeV
        ,'threepartMassLow':1.5*GeV
        ,'threepartMassHigh':2.8*GeV
        ,'fourpartMassLow':1.8*GeV
        ,'fourpartMassHigh':3.0*GeV
        ,'threepartMAXDOCA':0.15*mm
        ,'fourpartMAXDOCA':0.15*mm
        ,'threepartVtxChi2DOF':5.0
        ,'fourpartVtxChi2DOF':5.0
        ,'BMAXDOCA':0.20*mm
        ,'SigmaMAXDOCA':0.20*mm
        ,'BVtxChi2DOF':5.0
        ,'fourpartFDChi2':49.0
        ,'threepartFDChi2':49.0
        ,'threepartMinIPChi2':5.0
        ,'fourpartMinIPChi2':5.0
        ,'threepartPT':1.0*GeV
        ,'fourpartPT':1.0*GeV
        ,'BFDChi2':100.0
        ,'SigmaFDChi2':36.0
        ,'SigmaVtxChi2DOF':10.0
        ,'SigmaLcDeltaMlow':0.0*MeV
        ,'SigmaLcDeltaMhigh':1.0*GeV
        ,'SigmaPT':0.0*GeV
        ,'LcFDChi2':36.0
        ,'LcVtxChi2DOF':5.0
        ,'BDIRA':0.998
        ,'TrackChi2DOF':3.0
        ,'ApplyPionPIDK':True
        ,'PionPIDK':0.0
        ,'CheckPV':True
        ,'ApplyGECs':False
        ,'MaxLongTracks':200
        ,'ApplyGhostProbCut' :True
        ,'GhostProbCut':0.5
        ,'Prescalefourpart':1.0
        ,'Postscalefourpart':1.0
        ,'PrescaleComplete':1.0
        ,'PostscaleComplete':1.0
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
    
##Absolute Lc BF, Andrea Contu
##FULLDST
B2ppipiSigmacmm_Lcpi_FULLDST = {
    'BUILDERTYPE' : 'StrippingB2ppipiSigmacmm_Lcpi_Conf' ,
    'CONFIG'      : {
        'protonMINP':10.0*GeV
        ,'protonMINIPCHI2':6.0
        ,'protonMINPT': 500.0*MeV
        ,'protonMINPIDp':10.0
        ,'protonMINPIDp_K':0.0
        ,'pionMINP':5.0*GeV
        ,'pionMINPT':500.0*MeV
        ,'pionMINIPCHI2':6.0
        ,'piSigmaMINP':2.0*GeV
        ,'piSigmaMINPT':150.0*MeV
        ,'piSigmaMAXP':10.0*GeV
        ,'piSigmaMAXPT':1.0*GeV
        ,'piSigmaMINIPCHI2':8.0
        ,'ApplyDeltaMCut' : False
        ,'DelmUpper':3.0*GeV
        ,'DelmLower':0.0
        ,'BMassWind':200.0*MeV
        ,'LcMassWind':100.0*MeV
        ,'threepartMassLow':1.5*GeV
        ,'threepartMassHigh':2.8*GeV
        ,'fourpartMassLow':1.8*GeV
        ,'fourpartMassHigh':3.0*GeV
        ,'threepartMAXDOCA':0.15*mm
        ,'fourpartMAXDOCA':0.15*mm
        ,'threepartVtxChi2DOF':5.0
        ,'fourpartVtxChi2DOF':5.0
        ,'BMAXDOCA':0.20*mm
        ,'SigmaMAXDOCA':0.20*mm
        ,'BVtxChi2DOF':5.0
        ,'fourpartFDChi2':49.0
        ,'threepartFDChi2':49.0
        ,'threepartMinIPChi2':5.0
        ,'fourpartMinIPChi2':5.0
        ,'threepartPT':1.0*GeV
        ,'fourpartPT':1.0*GeV
        ,'BFDChi2':100.0
        ,'SigmaFDChi2':36.0
        ,'SigmaVtxChi2DOF':10.0
        ,'SigmaLcDeltaMlow':0.0*MeV
        ,'SigmaLcDeltaMhigh':1.0*GeV
        ,'SigmaPT':0.0*GeV
        ,'LcFDChi2':36.0
        ,'LcVtxChi2DOF':5.0
        ,'BDIRA':0.998
        ,'TrackChi2DOF':3.0
        ,'ApplyPionPIDK':True
        ,'PionPIDK':0.0
        ,'CheckPV':True
        ,'ApplyGECs':False
        ,'MaxLongTracks':200
        ,'ApplyGhostProbCut' :True
        ,'GhostProbCut':0.5
        ,'Prescalefourpart':0.05
        ,'Postscalefourpart':1.0
        ,'PrescaleComplete':1.0
        ,'PostscaleComplete':1.0
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }
#Dan Johnson, Mika Vesterinen
#D* tagged D0 -> KShhpi0
#Added in Stripping20rXp2
DstarD0ToKsHHPi0 = {
  'BUILDERTYPE' : 'DstarD0ToKsHHPi0AllLinesConf' ,
    'CONFIG'      : {
        "prescale_Pi0R_WIDEMASS" : 1.0 #adimensional
        ,"Pi0R_PT" : 500  # MeV
        ,"Pi0R_DMASS" : 15  # MeV
        ,"Pi0M_PT" : 1000 #MeV
        ,"Pi0_MassConstraint" : False # adimensional
        ,'KSLLPMin'         : 2000  ## MeV
        ,'KSLLPTMin'        : 250   ## MeV
        ,'KSDDPMin'         : 3000  ## MeV
        ,'KSDDPTMin'        : 250   ## MeV
        ,'KSLLCutMass'      : 30    ## MeV
        ,'KSDDCutMass'      : 30    ## MeV
        ,'KSLLCutFDChi2'    : 100   ## unitless
        ,'KSDDCutFDChi2'    : 100   ## unitless
        ,'KSDaugTrackChi2'  : 4     ## max chi2/ndf for Ks daughter tracks
        ,'KSVertexChi2'     : 6     ## max chi2/ndf for Ks vertex
        ,'KSCutDIRA'        : 0.99  ## unitless
        ,"TrGhostProbMax": 0.5    # adimensional
        ,"D0_M_WINDOW" : 210 #MeV
        ,"DELTA_MASS_MAX" : 180 #MeV
        ,"Pion_PIDK" : 0 #adimensional 
        ,"Kaon_PIDK" : 7 #adimensional 
        ,"Pion_PT" : 500 #MeV 
        ,"Kaon_PT" : 500 #MeV
        ,"Slowpion_PT" : 300 #MeV 
        ,"Pion_TRGHOSTPROB" : 0.35 #adimensional
        ,"Kaon_TRGHOSTPROB" : 0.35 #adimensional
        ,"Slowpion_TRGHOSTPROB" : 0.35 #adimensional
        ,"Slowpion_PIDe" : 5 #adimensional
        ,"D0_APT" : 1400  #MeV
        ,"useTOS" : True  #adimensional
        ,"useHLT" : True  #adimensional
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
