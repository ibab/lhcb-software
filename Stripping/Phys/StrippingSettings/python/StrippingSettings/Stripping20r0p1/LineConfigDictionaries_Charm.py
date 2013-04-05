"""
Line config dictionaries for the Charm WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""


from GaudiKernel.SystemOfUnits import *

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
    'D2PiMuMuOSLinePrescale'     : 1 ,
    'D2PiMuMuOSLinePostscale'    : 1 ,
    'D2PiMuMuSSLinePrescale'     : 1 ,
    'D2PiMuMuSSLinePostscale'    : 1 ,
    'D2KMuMuOSLinePrescale'      : 1 ,
    'D2KMuMuOSLinePostscale'     : 1 ,
    'D2KMuMuSSLinePrescale'      : 1 ,
    'D2KMuMuSSLinePostscale'     : 1 ,
    'D2PiPiPiCalLinePrescale'    : 0.0075 ,
   'D2PiPiPiCalLinePostscale'   : 1 ,
    'D2K2PiLinePrescale'         : 0.005 ,
    'D2K2PiLinePostscale'        : 1 ,
    'D22KPiLinePrescale'         : 0.005 ,
    'D22KPiLinePostscale'        : 1,
    'D02KKMuMuLinePrescale'      : 1 ,
    'D02KKMuMuLinePostscale'     : 1 ,
    'D02PiPiMuMuLinePrescale'    : 1 ,
    'D02PiPiMuMuLinePostscale'   : 1,
    'D02KPiMuMuLinePrescale'     : 1 ,
    'D02KPiMuMuLinePostscale'    : 1,
    'D02KKPiPiLinePrescale'      : 0.01 ,
    'D02KKPiPiLinePostscale'     : 1,
    'D02K3PiLinePrescale'        : 0.003 ,
    'D02K3PiLinePostscale'       : 1,
    'D02PiPiPiPiLinePrescale'    : 0.0075 ,
    'D02PiPiPiPiLinePostscale'   : 1,
    'Lambdac2PMuMuLinePrescale'  : 1 ,
    'Lambdac2PMuMuLinePostscale' : 1 ,
    'Lambdac2PPiPiLinePrescale'  : 0.0075 ,
    'Lambdac2PPiPiLinePostscale' : 1
  } ,
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm' ]
  }

## Benoit Viaud
## Added in Stripping20
## Tagged D02H(H)Mumu.
## Lives in StrippingDstarPromptWithD02HHMuMu.py
DstarPromptWithD02HHMuMu = {
   'BUILDERTYPE' : 'DstarPromptWithD02HHMuMuConf',
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
  ,'ApplyGhostProbCut': False # !!!
  ,'GhostProbCut'     : 0.5 # !!!
  ,'Prescale'         : 1
  ,'Postscale'        : 1
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
  ,'ApplyGhostProbCut': False # !!!
  ,'GhostProbCut'     : 0.5 # !!!
  ,'Prescale'         : 0.01
  ,'Postscale'        : 1
  ,'RunK3pi'          : True
  ,'Run4pi'           : True
  ,'Run2K2pi'         : True
  ,'Run3Kpi'          : True
  } ,
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm' ]
  }
  
  
## Mat Charles
## Added in Stripping20r0/1p1
## Fully and partially reconstructed stripping lines for the following decay modes:
##
##  B-    -> Xi_c(2815)+  anti-Sigma_c(2455)--
##  B0bar -> Xi_c(2815)0  anti-Sigma_c(2455)0
##
## Uses in StrippingB2Xic2815Sc2455.py
B2Xic2815Sc2455 = {
   'BUILDERTYPE' : 'B2XicScBuilder',
    'CONFIG' : {
     'LongTrackGEC' : 250
    , 'signalPrescalePartial'  : 1.0
    , 'signalPrescaleFull'  : 1.0
    } ,
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'CharmCompleteEvent' ]
  }
  
## Micro-DST, Philip Hunt and Benoit Viaud
## Modified from Strippinb13b
## Test:  Yes
DstarPromptWithD02HHHH = {
    'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG'      : {
    'CombDelmLower'     : -8.*MeV
    ,'CombDelmUpper'    : 25*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 23*MeV
    ,'CombD0MassWin'    : 78*MeV
    ,'D0MassWin'        : 75*MeV
    ,'DstarDOCA'        : 0.22*mm
    ,'D0MaxDOCA'        : 0.12*mm
    ,'D0DauPt'          : 250*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 120*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 3.0
    ,'D0FDChi2'         : 36
    ,'D0IPChi2'         : 999999999999.9   # Mat changed from 30 to a large number in S20
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 4
    ,'ApplyKaonPIDK'    : True
    ,'KaonPIDK'         : -5.0
    ,'ApplyPionPIDK'    : True
    ,'PionPIDK'         : 3 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 250
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'ApplyGhostProbCut': True
    ,'GhostProbCut'     : 0.5
    ,'Prescale'         : 1
    ,'Postscale'        : 1
    ,'RunK3pi'          : False
    ,'Run4pi'           : False
    ,'Run2K2pi'         : False
    ,'Run3Kpi'          : True
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }


## Micro-DST, Ibid.
DstarPromptWithD02HHHHNoPID = {
    'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG'      : {
    'CombDelmLower'     : -8*MeV
    ,'CombDelmUpper'    : 25*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 23*MeV
    ,'CombD0MassWin'    : 78*MeV
    ,'D0MassWin'        : 75*MeV
    ,'DstarDOCA'        : 0.22*mm
    ,'D0MaxDOCA'        : 0.12*mm
    ,'D0DauPt'          : 250*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 120*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 3.0
    ,'D0FDChi2'         : 36
    ,'D0IPChi2'         : 999999999999.9   # Mat changed from 30 to a large number in S20
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 4
    ,'ApplyKaonPIDK'    : False 
    ,'KaonPIDK'         : -100
    ,'ApplyPionPIDK'    : False 
    ,'PionPIDK'         : 100 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 250
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'ApplyGhostProbCut': True
    ,'GhostProbCut'     : 0.5
    ,'Prescale'         : 0.05
    ,'Postscale'        : 1
    ,'RunK3pi'          : False
    ,'Run4pi'           : False
    ,'Run2K2pi'         : False
    ,'Run3Kpi'          : True
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }
    
## Walter Bonivento and Francesco Dettori
## Modified from Strippinb13b, D02emu only
## Full DST needed for isolation cuts.
## Test:  Yes
DstarD02xx = {
    'BUILDERTYPE' : 'StrippingDstarD02xxConf',
    'CONFIG':  {
      'PrescalepipiBox'     : 0.
    , 'PrescalemumuBox'     : 0.
    , 'PrescaleKpiBox'    : 0.
    , 'PrescaleemuBox'    : 1.
    , 'PrescaleeKBox'    : 0.
    , 'PrescaleepiBox'    : 0.
    , 'PrescalepimuBox'    : 0.
    , 'PrescaleKmuBox'    : 0.
    , 'Prescalepipi_untagged_Box'     : 0.
    , 'Prescalemumu_untagged_Box'     : 0.
    , 'PrescaleKpi_untagged_Box'    : 0.
    , 'Prescalepimu_untagged_Box'    : 0.
    , 'PrescaleKmu_untagged_Box'    : 0.
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
    ,'RequireHlt'         : 1          # 
    ,'prefix'         : '' 
                   },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

## KShh. Mat Charles
## Test:  Yes
DstarD2KShh = {
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
    'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          :    0.9997   ## unitless
    ,'KSDDCutDIRA'          :    0.99995  ## unitless
    ,'KSLLCutMass'          :   20.0      ## in MeV -- Mat changed from 11.4 to 20.0 in S20
    ,'KSDDCutMass'          :   40.0      ## in MeV -- Mat changed from 24.9 to 40.0 in S20
    ,'KSLLCutFDChi2'        :  100        ## unitless
    ,'KSDDCutFDChi2'        :  100        ## unitless
    ,'KSCutZFDFromD'        :   10.0      ## in mm -- Mat added in S20
    ,'trackFromDCutP'       : 1500.0      ## in MeV
    ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     :   -1.0
    ,'kaonFromDCutPIDpi'    :   -3.0
    ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
    ,'DCutDIRA'             :    0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              :    0.2      ## ps -- Mat changed from 0.3 to 0.2 in S20
    ,'DCutVtxChi2_KK'       :   21.0      ## unitless
    ,'DCutVtxChi2_KP'       :   15.0      ## unitless
    ,'DCutVtxChi2_PP'       :   13.0      ## unitless
    ,'preFitDCutPT'         : 1500.0      ## in MeV
    ,'preFitDMassCut_LL'    :  130        ## in MeV
    ,'preFitDMassCut_DD'    :  270        ## in MeV
    ,'wideDMassCut_KKLL'    :   70.0      ## in MeV -- Mat changed from  57.2 to  70.0 in S20
    ,'wideDMassCut_KPLL'    :  105.0      ## in MeV -- Mat changed from  92.0 to 105.0 in S20
    ,'wideDMassCut_PPLL'    :  120.0      ## in MeV -- Mat changed from 110.0 to 120.0 in S20
    ,'wideDMassCut_KKDD'    :   85.0      ## in MeV -- Mat changed from  72.0 to  85.0 in S20
    ,'wideDMassCut_KPDD'    :  130.0      ## in MeV -- Mat changed from 120.0 to 130.0 in S20
    ,'wideDMassCut_PPDD'    :  150.0      ## in MeV -- Mat changed from 140.0 to 150.0 in S20
    ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
    ,'wideDMCutUpper'       :   15.0      ## in MeV -- Mat changed from 15.0 to 25.0 in S20
    ,'DstarCutPT_KK'        : 2200.0      ## in MeV
    ,'DstarCutPT_KP'        : 2200.0      ## in MeV
    ,'DstarCutPT_PP'        : 2200.0      ## in MeV
    ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
    ,'DstarCutChi2NDOF_KP'  :   20.0      ## unitless
    ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
    ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
    ,'KKLLPrescale'         :    0.0
    ,'KKLLPostscale'        :    0.0
    ,'KPLLPrescale'         :    1.0
    ,'KPLLPostscale'        :    1.0
    ,'PPLLPrescale'         :    0.0
    ,'PPLLPostscale'        :    0.0
    ,'KKDDPrescale'         :    0.0
    ,'KKDDPostscale'        :    0.0
    ,'KPDDPrescale'         :    1.0
    ,'KPDDPostscale'        :    1.0
    ,'PPDDPrescale'         :    0.0
    ,'PPDDPostscale'        :    0.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }

## KShh. Mat Charles.
## Test:  Yes
## Control version of above, going to full DST
## For Stripping19 we bump the prescale of KSKK and KSpipi to 1 because
## the trigger is changing and we need to figure out how to swim them.
DstarD2KShhCompleteEvent = {
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
    'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          :    0.9997   ## unitless
    ,'KSDDCutDIRA'          :    0.99995  ## unitless
    ,'KSLLCutMass'          :   20.0      ## in MeV -- Mat changed from 11.4 to 20.0 in S20
    ,'KSDDCutMass'          :   40.0      ## in MeV -- Mat changed from 24.9 to 40.0 in S20
    ,'KSLLCutFDChi2'        :  100        ## unitless
    ,'KSDDCutFDChi2'        :  100        ## unitless
    ,'KSCutZFDFromD'        :   10.0      ## in mm -- Mat added in S20
    ,'trackFromDCutP'       : 1500.0      ## in MeV
    ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     :   -1.0
    ,'kaonFromDCutPIDpi'    :   -3.0
    ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
    ,'DCutDIRA'             :    0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              :    0.2      ## ps -- Mat changed from 0.3 to 0.2 in S20
    ,'DCutVtxChi2_KK'       :   21.0      ## unitless
    ,'DCutVtxChi2_KP'       :   15.0      ## unitless
    ,'DCutVtxChi2_PP'       :   13.0      ## unitless
    ,'preFitDCutPT'         : 1500.0      ## in MeV
    ,'preFitDMassCut_LL'    :  130        ## in MeV
    ,'preFitDMassCut_DD'    :  270        ## in MeV
    ,'wideDMassCut_KKLL'    :   57.2      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'wideDMassCut_KPLL'    :   92.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'wideDMassCut_PPLL'    :  110.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'wideDMassCut_KKDD'    :   72.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'wideDMassCut_KPDD'    :  120.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'wideDMassCut_PPDD'    :  140.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
    ,'wideDMCutUpper'       :   15.0      ## in MeV -- Caution: tighter than CharmToBeSwum cut from S20
    ,'DstarCutPT_KK'        : 2200.0      ## in MeV
    ,'DstarCutPT_KP'        : 2200.0      ## in MeV
    ,'DstarCutPT_PP'        : 2200.0      ## in MeV
    ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
    ,'DstarCutChi2NDOF_KP'  :   20.0      ## unitless
    ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
    ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
    ,'KKLLPrescale'         :    0.0
    ,'KKLLPostscale'        :    0.0
    ,'KPLLPrescale'         :    0.025
    ,'KPLLPostscale'        :    1.0
    ,'PPLLPrescale'         :    0.0
    ,'PPLLPostscale'        :    0.0
    ,'KKDDPrescale'         :    0.0
    ,'KKDDPostscale'        :    0.0
    ,'KPDDPrescale'         :    0.025
    ,'KPDDPostscale'        :    1.0
    ,'PPDDPrescale'         :    0.0
    ,'PPDDPostscale'        :    0.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

## Micro-DST, Diego Milanes
## Test:  Yes
CharmedAndCharmedStrangeSpectroscopy = {
    'BUILDERTYPE' : 'CharmedAndCharmedStrangeSpectroscopyConf' ,
    'CONFIG'      : {
       'DpKs_prescale'            : 0,
       'DstarpKs_prescale'        : 0,
       'D0K_prescale'             : 0,
       'DpPim_prescale'           : 0,
       'D0Pip_prescale'           : 0,
       'DstarpPim_prescale'       : 0,
       'DzP_prescale'             : 0,
       'DzPWS_prescale'           : 0,
       'DpP_prescale'             : 0,
       'DpPp_prescale'            : 0,
       'DsKs_prescale'            : 0,
       'DsKm_prescale'            : 0,
       'DstarD02K3PiKs_prescale'  : 0,
       'Dstar0K_prescale'         : 0,
       'Dstar0P_prescale'         : 0,
       'Dstar0KWS_prescale'       : 0,
       'Dstar0PWS_prescale'       : 0,
       'DstarpPp_prescale'        : 1,
       'DstarpPm_prescale'        : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

##Absolute Lc BF, Andrea Contu
##MDST
B2ppipiSigmacmm_Lcpi = {
    'BUILDERTYPE' : 'StrippingB2ppipiSigmacmm_Lcpi_Conf' ,
    'CONFIG'      : {
        'protonMINP':10.0*GeV
        ,'protonMINIPCHI2':6.0
        ,'protonMINPT': 500.0*MeV
        ,'protonMINPIDp':5.0
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
        ,'protonMINPIDp':5.0
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


## CharmFromBSemi Lines
## Micro-DST, Mika Vesterinen
## Moved to this file in Stripping20
## Lives in StrippingCharmFromBSemi.py
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
    'STREAMS' : {
    'Charm' : ['Strippingb2DstarMuXKsKPiLLCharmFromBSemiLine',
               'Strippingb2DstarMuXKsKPiDDCharmFromBSemiLine',
               'Strippingb2DstarMuXKsPiPiLLCharmFromBSemiLine',
               'Strippingb2DstarMuXKsPiPiDDCharmFromBSemiLine',
               'Strippingb2DstarMuXKsKKLLCharmFromBSemiLine',
               'Strippingb2DstarMuXKsKKDDCharmFromBSemiLine',
               'Strippingb2D0MuXKsKPiLLCharmFromBSemiLine',
               'Strippingb2D0MuXKsKPiDDCharmFromBSemiLine',
               'Strippingb2D0MuXKsPiPiLLCharmFromBSemiLine',
               'Strippingb2D0MuXKsPiPiDDCharmFromBSemiLine',
               'Strippingb2D0MuXKsKKLLCharmFromBSemiLine',
               'Strippingb2D0MuXKsKKDDCharmFromBSemiLine',
               'Strippingb2MuXLc2pKsLLCharmFromBSemiLine',
               'Strippingb2MuXLc2pKsDDCharmFromBSemiLine']},
    'WGs'    : ['Charm']
    }
##D+ -> pi+ (pi0->e+ e- gamma) lines, prompt and smmileptonic, Ed Greening, Brian Meadows
##FULLDST
D2PiPi0_eegamma = {
    'BUILDERTYPE' : 'D2PiPi0Conf' ,
    'CONFIG'      : {
      'Daug_IPCHI2_MIN'    : 9         #NOT USED as doesn't seem to be helpful in MC study, computes the chi^2 distance of track to PV, 9 is 3 sigma from PV
      ,'Daug_PT_MIN'       : 150  *MeV
      ,'Daug_P_MIN'        : 1000 *MeV  #ensures good range for RICH
      ,'Daug_TRCHI2DOF_MAX': 5          #to ensure good track quality
      ,'TRGHOSTPROB_MAX'   : 0.5
      ,'ElectronPID_MIN'   : 1
      ,'Comb_PT_Min'       : 2000 *MeV
      ,'DielectonMass_MAX' : 200  *MeV  #to ensure that dielectrons don't have mass exceeding pi0
      ,'CombMass_MIN'      : 70   *MeV
      ,'CombMass_MAX'      : 200  *MeV
      ,'DMass_MIN'         : 1700 *MeV
      ,'DMass_MAX'         : 2100 *MeV
      ,'VFASPF_MAX'        : 5          #computes the chi^2 on the end vertex of the particle
      ,'BPVLTIME_MIN'      : 50 *micrometer
      ,'DTF_CHI2NDOF_MAX'  : 5          #computes the chi^2 of the fit
      ,'MuonPID_MIN'       : 1
      ,'BMass_MIN'         : 2200 *MeV
      ,'BMass_MAX'         : 6300 *MeV
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

# Kshort2MuMuPiPi Lines
# Full-DST, Michael Sokoloff, Laurence Carson
# Moved to this file in Stripping20r1
# Lives in StrippingKshort2MuMuPiPi.py

Kshort2MuMuPiPi  = {
                        'BUILDERTYPE' : 'Kshort2PiPiMuMuConf',
                        'CONFIG' : {
                                'MuonP'         : 3000. ,    #MeV
                                'MuonPT'        : 500.  ,    #MeV
                                'MuonMINIPCHI2' : 2     ,    #adminensional
                                'MuonTRCHI2'    : 5     ,    #adminensional

                                'PionP'         : 2000. ,    #MeV
                                'PionPT'        : 300.  ,    #MeV
                                'PionMINIPCHI2' : 2     ,    #adminensional
                                'PionTRCHI2'    : 5     ,    #adminensional

                                #4body
                                'MINIPCHI2_hhmumu' : 4. ,  #adminensional --  for the individual pions
                                'PT_hhmumu'        : 300 , #MeV
                                'MuonPIDmu_hhmumu' : -1,   #adimensional
                                'DPT_hhmumu'           : 2500.,     #MeV
                                'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
                                'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
                                'KsDauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
                                'DFDCHI2_hhmumu'       : 9     ,   #adimensional
                                'DIPCHI2_hhmumu'       : 20    ,    #adimensional
                                'DDIRA'         : 0.9999,    #adimensional
                                'MaxDimuonMass'    : 260.  ,    #MeV
                                'MaxKsMass'                   : 550.  ,    #MeV, comb mass window
                                'KsMotherMassCut'             : 540.  ,    # MeV, mother mass cut
                                'Kshort2PiPiMuMuLinePrescale'    : 1 ,
                                'Kshort2PiPiMuMuLinePostscale'   : 1
                                },
                        'STREAMS' : ['CharmCompleteEvent'] ,
                        'WGs'    : ['Charm']
}

# ChargedHyperon Lines
# Full-DST, Michael Sokoloff, Laurence Carson
# Moved to this file in Stripping20r1
# Lives in StrippingChargedHyperons.py

ChargedHyperons  = {
                        'BUILDERTYPE' : 'ChargedHyperonsConf',
                        'CONFIG' : {
                                                        'TRCHI2DOFMax'    : 3.0
                                                , 'PionPIDK'        :  5.0
                                                , 'KaonPIDK'        :  -5.0
                                                , 'XiMassWindow'    :  20 * MeV
                                                , 'OmegaMassWindow' :  20 * MeV
                                                , 'LambdaLLMinDecayTime'  :  5.0 ##   (ps)
                                                , 'LambdaLLVtxChi2Max'    :   5.0
                                                , 'LambdaDDVtxChi2Max'    :   5.0
                                                , 'LambdaLLMassWin'  : 5.7 * MeV
                                                , 'LambdaDDMassWin'  : 5.7 * MeV
                                                , 'LambdaLLMinVZ'    : -100. * mm
                                                , 'LambdaLLMaxVZ'    :  500. * mm
                                                , 'LambdaDDMinVZ'    :  300. * mm
                                                , 'LambdaDDMaxVZ'    : 2275. * mm
                                                , 'TrGhostProbMax'   :  0.25
                                                , 'ProbNNkMin'       :  0.20
                                                , 'ProbNNpMinLL'     :  0.20
                                                , 'ProbNNpMinDD'     :  0.05
                                },
                        'STREAMS' : ['CharmCompleteEvent'] ,
                        'WGs'    : ['Charm']
}

