"""
Line config dictionaries for the Charm WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""


from GaudiKernel.SystemOfUnits import *


## D0 -> h+h- (tagged & untagged).
## Responsible: Marco Gersabeck
## In S18, split into default & control parts. The default
## part requires an explicit TOS on HLT2 && HLT1.
D2hh = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 0.1,
           'UntaggedCFLinePostscale': 1.,
           'UntaggedSCSLinePrescale': 1.,
           'UntaggedSCSLinePostscale': 1.,
           'TaggedRSLinePrescale': 1.,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 1.,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 1.,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 1.,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.05,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': False,
           'RunTISLines': False,
           'RunDefault': True,
           'UseTOSFilter': True,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0.*Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 },
           'RequiredRawEvents' : ["Muon","Calo","Rich","Velo","Tracker","Trigger"]
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmToBeSwum' ]
    }

## D0 -> h+h- (tagged & untagged).
## Responsible: Marco Gersabeck
## Prescaled version that writes to full DST.
D2hhCompleteEvent = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 0.001,
           'UntaggedCFLinePostscale': 1.0,
           'UntaggedSCSLinePrescale': 0.01,
           'UntaggedSCSLinePostscale': 1.0,
           'TaggedRSLinePrescale': 0.03,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 0.03,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 0.03,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 0.01,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 0.01,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.1,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': False,
           'RunTISLines': True,
           'RunDefault': True,
           'UseTOSFilter': True,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0.*Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 },
           'RequiredRawEvents' : ["Trigger"]
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

## D0 -> h+h- (tagged & untagged).
## Responsible: Marco Gersabeck + Joerg Marks
## Special variant for wrong-sign mode: tighter cuts, looser prescale
D2hhWSCompleteEvent = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 8.0,
           'LowPIDK': -5.0,
           'D0Pt': 3300.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 0.0,
           'UntaggedCFLinePostscale': 1.0,
           'UntaggedSCSLinePrescale': 0.0,
           'UntaggedSCSLinePostscale': 1.0,
           'TaggedRSLinePrescale': 0.03,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.0,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 0.0,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 0.0,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 0.0,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.0,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': False,
           'RunTISLines': False,
           'RunDefault': True,
           'UseTOSFilter': True,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0.*Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 },
           'RequiredRawEvents' : ["Trigger"]
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


## Control version of above line.
## Responsible: Marco Gersabeck
## Does not require TOS.
PromptD2hh = {
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
           'DaugPtMin': 800.,
           'DaugPtMax': 1500.,
           'DaugPtLoose': 500.,
           'DaugP': 5000.,
           'DaugPLoose': 3000.,
           'DaugIPChi2': 9.,
           'DaugIPChi2Loose': 4.,
           'DaugTrkChi2': 3.,
           'DaugTrkChi2Loose': 4.,
           'HighPIDK': 5.,
           'LowPIDK': 0.,
           'D0Pt': 2000.,
           'D0PtLoose': 1500.,
           'D0MassWindowCentre': 1865.,
           'D0MassWindowWidth': 100.,
           'D0KPiMassWindowWidthLow':  -100.,
           'D0KPiMassWindowWidthHigh': 200.,
           'D0PiPiMassWindowWidthLow':  -75.,
           'D0PiPiMassWindowWidthHigh': 200.,
           'D0KKMassWindowWidthLow': -100.,
           'D0KKMassWindowWidthHigh': 200.,
           'D0P': 5000.,
           'D0VtxChi2Ndof': 10.,
           'D0FDChi2': 40.,
           'D0BPVDira': 0.9999,
           'D0DOCA': 0.07,
           'Daug_TRCHI2DOF_MAX': 5.,
           'Dstar_AMDiff_MAX': 165.,
           'Dstar_VCHI2VDOF_MAX': 100.,
           'Dstar_MDiff_MAX': 160.,
           'UntaggedCFLinePrescale': 1.0,
           'UntaggedCFLinePostscale': 1.,
           'UntaggedSCSLinePrescale': 1.,
           'UntaggedSCSLinePostscale': 1.,
           'TaggedRSLinePrescale': 1.,
           'TaggedRSLinePostscale': 1.,
           'TaggedWSLinePrescale': 1.,
           'TaggedWSLinePostscale': 1.,
           'TaggedSCSLinePrescale': 1.,
           'TaggedSCSLinePostscale': 1.,
           'TaggedRSSSLinePrescale': 1.,
           'TaggedRSSSLinePostscale': 1.,
           'TaggedSCSSSLinePrescale': 1.,
           'TaggedSCSSSLinePostscale': 1.,
           'UntaggedTISLinePrescale': 0.05,
           'UntaggedTISLinePostscale': 1.,
           'UntaggedKpiOnly': False,
           'RunSameSign': True,
           'RunTISLines': True,
           'RunDefault': False,
           'UseTOSFilter': False,
           'AddPartialD': True,
           'Hlt1TOS': { 'Hlt1TrackAllL0.*Decision%TOS' : 0 },
           'Hlt2TOSKPi': { 'Hlt2CharmHadD02HH_D02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KPiDecision%TOS' : 0, 'Hlt2CharmHadD02KPiWideMassDecision%TOS' : 0 },
           'Hlt2TOSKK': { 'Hlt2CharmHadD02HH_D02KKDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02KKWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02KKDecision%TOS' : 0, 'Hlt2CharmHadD02KKWideMassDecision%TOS' : 0 },
           'Hlt2TOSPiPi': { 'Hlt2CharmHadD02HH_D02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02HH_D02PiPiWideMassDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiDecision%TOS' : 0, 'Hlt2CharmHadD02PiPiWideMassDecision%TOS' : 0 },
           'RequiredRawEvents' : ["Trigger"]
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Micro-DST, Sam Gregson
## Test:  Yes
D2KS0H = {
    'BUILDERTYPE' : 'D2KS0HConf' ,
    'CONFIG'	  : {
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
    ,'DMesonComboLowMass'	: 1760
    ,'DMesonComboHighMass'	: 2080
    ,'DMesonComboDOCA'          :    0.6
    ,'DMesonAPT'                : 1000
    ,'DMesonADOCAChi2'          :   11
    ,'DMesonMotherLowMass'	: 1770
    ,'DMesonMotherHighMass'     : 2070
    ,'DMesonMotherVertexChi2'   :   10
    ,'DMesonMotherMIPChi2'	:   15
    ,'DMesonMotherPT'           : 1000
    ,'KS0ZDiff'                 :   10
    ,'DMesonFlightDistChi2'     :    5
    ,'PionLinePrescale'         :    1
    ,'KaonLinePrescale'         :    1
    ,'PionLinePrescaleDD'	:    1
    ,'KaonLinePrescaleDD'	:    1
   ,'PionLinePostscale'        :    1
    ,'KaonLinePostscale'        :    1
    ,'PionLinePostscaleDD'	:    1
    ,'KaonLinePostscaleDD'	:    1
    ,"UseTOS" : True
    ,"TisTosSpecs_KS0Pi" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0PiDecision%TOS":0}
    ,"TisTosSpecs_KS0PiDD" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0DDPiDecision%TOS":0}
    ,"TisTosSpecs_KS0K" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0KDecision%TOS":0}
    ,"TisTosSpecs_KS0KDD" : {"Hlt1TrackAllL0Decision%TOS":0,"Hlt2CharmHadD2KS0H_D2KS0DDKDecision%TOS":0}
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : {
        'Charm' : ['StrippingD2KS0HPionLine']
        }
    }



## Micro-DST, Diego Milanes
## Test:  Yes
CharmedAndCharmedStrangeSpectroscopy = {
    'BUILDERTYPE' : 'CharmedAndCharmedStrangeSpectroscopyConf' ,
    'CONFIG'      : {
       'DpKs_prescale'            : 1.,
       'DstarpKs_prescale'        : 1.,
       'D0K_prescale'             : 1.,
       'DpPim_prescale'           : 1.,
       'D0Pip_prescale'           : 1.,
       'DstarpPim_prescale'       : 1.,
       'DzP_prescale'             : 1.,
       'DzPWS_prescale'           : 1.,
       'DpP_prescale'             : 1.,
       'DpPp_prescale'            : 1.,
       'DsKs_prescale'            : 1.,
       'DsKm_prescale'            : 1.,
       'DstarD02K3PiKs_prescale'  : 1.,
       'Dstar0K_prescale'         : 1.,
       'Dstar0P_prescale'         : 1.,
       'Dstar0KWS_prescale'       : 1.,
       'Dstar0PWS_prescale'       : 1.,
       'DstarpPp_prescale'        : 1.,
       'DstarpPm_prescale'        : 1.
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


#Absolute Lc BF, Andrea Contu
#MDST
B2ppipiSigmacmm_Lcpi = {
    'BUILDERTYPE' : 'StrippingB2ppipiSigmacmm_Lcpi_Conf' ,
    'CONFIG'      : {
    'protonMINP':10.0*GeV
    ,'protonMINIPCHI2':8.0
    ,'protonMINPT': 500.0*MeV
    ,'protonMINPIDp':5.0
    ,'protonMINPIDp_K':-5.0
    ,'pionMINP':5.0*GeV
    ,'pionMINPT':400.0*MeV
    ,'pionMINIPCHI2':8.0
    ,'piSigmaMINP':2.0*GeV
    ,'piSigmaMINPT':150.0*MeV
    ,'piSigmaMAXP':10000.0*GeV
    ,'piSigmaMAXPT':1000.0*GeV
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
    ,'threepartMinIPChi2':6.0
    ,'fourpartMinIPChi2':6.0
    ,'threepartPT':1.0*GeV
    ,'fourpartPT':1.0*GeV
    ,'BFDChi2':64.0
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
    ,'GhostProbCut':0.3
    ,'UseTOS': True
    ,'Hlt2TOS': { "Hlt2.*Decision%TOS" : 0 }
    ,'Prescalefourpart':1.0
    ,'Postscalefourpart':1.0
    ,'PrescaleComplete':1.0
    ,'PostscaleComplete':1.0
    ,'PrescaleCompleteNorm':1.0
    ,'PostscaleCompleteNorm':1.0
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }




## Micro-DST, Erica Polycarpo
## Modified from Strippinb13b
## Test:  Yes
D2hhh = { 
    'BUILDERTYPE' : 'D2hhhConf' ,
    'CONFIG' : {
    'DaughterPT'            : 250.,
    'DaughterP'             : 2000.,
    'DaughterIPChi2'        : 4.,
    'Daughter2IPChi2'       : 10.,
    'D2KKKDaughterIPChi2'   : 2.,
    'D2KKKDaughter2IPChi2'  : 4.,
    'KPIDK'                 : 7.,
    'piPIDK'                : 3.,
    'PTSum'                 : 2800.,
    'DOCAMax'               : 0.5,
    'DOCAChi2'              : 50.,
    'DPt'                   : 1000.,
    'DIPChi2'               : 12.,
    'DdcaFDChi2'            : 125.,
    'DDIRA'                 : 0.98,
    'DVXChi2NDOF'           : 10.,
    'MinMassPosFit'         : 1800.,
    'MaxMassPosFit'         : 2040.,
    'D2KPPMaxMassPosFit'    : 1940.,
    'Ds2KKPosMinMassPosFit' : 1900.,
    'D2HHHIncMinMassPosFit' : 600.,
    'D2HHHIncMaxMassPosFit' : 2070.,
    'MaxTracksInEvent'      : 500,
    'D2KKPLinePrescale'     : 1.0,
    'D2KKPLinePostscale'    : 1.0 ,
    'D2KPPLinePrescale'     : 0.3,
    'D2KPPLinePostscale'    : 1.0,
    'D2PPPLinePrescale'     : 1.0,
    'D2PPPLinePostscale'    : 1.0,
    'D2KKKLinePrescale'     : 1.0,
    'D2KKKLinePostscale'    : 1.0,
    'Ds2KKPosLinePrescale'  : 1.0,
    'Ds2KKPosLinePostscale' : 1.0,
    'D2KPPosLinePrescale'   : 1.0,
    'D2KPPosLinePostscale'  : 1.0,
    'D2HHHIncLinePrescale'  : 0.15,
    'D2HHHIncLinePostscale' : 1.0,
    'HLT'                   : None
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Full DST (control), Ibid.
## Heavily prescaled, Kpipi and KKpi control modes only.
## New in Stripping17.
## Added KKpi, changed name, loosened PT cut in S18.
D2hhhControl = { 
    'BUILDERTYPE' : 'D2hhhConf' ,
    'CONFIG' : {
    'DaughterPT'                :  250.,
    'DaughterP'                 : 2000.,
    'DaughterIPChi2'            :    4.,
    'Daughter2IPChi2'           :   10.,
    'D2KKKDaughterIPChi2'       :    2.,
    'D2KKKDaughter2IPChi2'      :    4.,
    'KPIDK'                     :    7.,
    'piPIDK'                    :    3.,
    'PTSum'                     : 1800.,
    'DOCAMax'			: 0.5,
    'DOCAChi2'                  :   50.,
    'DPt'                       : 1000.,
    'DIPChi2'                   :   12.,
    'DdcaFDChi2'                :  125.,
    'DDIRA'                     :    0.0,
    'DVXChi2NDOF'               :   10.,
    'MinMassPosFit'             : 1800.,
    'MaxMassPosFit'             : 2040.,
    'D2KPPMaxMassPosFit'        : 1940.,
    'Ds2KKPosMinMassPosFit'     : 1900.,
    'D2HHHIncMinMassPosFit'     : 1100.,
    'D2HHHIncMaxMassPosFit'     : 2070.,
    'MaxTracksInEvent'          :  500,
    'D2KKPLinePrescale'         :    0.03,
    'D2KKPLinePostscale'        :    1.0 ,
    'D2KPPLinePrescale'         :    0.03,
    'D2KPPLinePostscale'        :    1.0,
    'D2PPPLinePrescale'         :    0.0,
    'D2PPPLinePostscale'        :    0.0,
    'D2KPPosLinePrescale'       :    0.0,
    'D2KPPosLinePostscale'      :    0.0,
    'D2KKKLinePrescale'         :    0.0,
    'D2KKKLinePostscale'        :    0.0,
    'Ds2KKPosLinePrescale'      :    0.0,
    'Ds2KKPosLinePostscale'     :    0.0,
    'D2KPPosLinePrescale'       :    0.0,
    'D2KPPosLinePostscale'      :    0.0,
    'D2HHHIncLinePrescale'      :    0.0,
    'D2HHHIncLinePostscale'     :    0.0,
    'HLT'                       : None
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


## Malcolm John and Benoit Viaud
## Modified from Strippinb13b
## Test:  Yes

D2XMuMu = {
   'BUILDERTYPE' : 'D2XMuMuConf',
    'CONFIG' : {
    'MuonP'         : 3000. ,    #MeV
                  'MuonPT'        : 500.  ,    #MeV
                  'MuonMINIPCHI2' : 6     ,    #adminensional
                  'MuonTRCHI2'    : 5     ,    #adminensional
                  
                  'PionP'         : 2000. ,    #MeV
                  'PionPT'        : 300.  ,    #MeV
                  'PionMINIPCHI2' : 6     ,    #adminensional
                  'PionTRCHI2'    : 5     ,    #adminensional
                  
                  'KaonP'         : 2000. ,    #MeV
                  'KaonPT'        : 300.  ,    #MeV
                  'KaonPIDK'      : -1.   ,    #adimensional
                  'KaonMINIPCHI2' : 6     ,    #adminensional
                  'KaonTRCHI2'    : 5     ,    #adminensional

                  'ProtonP'         : 2000. ,    #MeV
                  'ProtonPT'        : 300.  ,    #MeV
                  'ProtonMINIPCHI2' : 6     ,    #adminensional
                  'ProtonTRCHI2'    : 5     ,    #adminensional

                  #3 body
                  'DVCHI2DOF'     : 5     ,    #adminensional
                  'DMAXDOCA'      : 0.15  ,    #mm
                  'DIPCHI2'       : 25    ,    #adimensional
                  
                  #4body
                  'MINIPCHI2_hhmumu' : 2. ,  #adminensional
                  'PT_hhmumu'        : 300 , #MeV
                  'MuonPIDmu_hhmumu' : -1,   #adimensional
                  'DPT_hhmumu'           : 2500.,     #MeV
                  'DVCHI2DOF_hhmumu'     : 8     ,    #adminensional         
                  'DMAXDOCA_hhmumu'      : 0.2  ,    #mm
                  'DdauMAXIPCHI2_hhmumu' : 15    ,    #adimensinal
                  'DFDCHI2_hhmumu'       : 36     ,   #adimensional
                  'DIPCHI2_hhmumu'       : 20    ,    #adimensional

                  'DIPCHI2_4bodyCS'      : 16    ,    #adimensional
                  'MINIPCHI2_4bodyCS' : 3. ,  #adminensional

                  'DDIRA'         : 0.9999,    #adimensional
                  'DimuonMass'    : 250.  ,    #MeV
                  'DMassWin'                   : 200.  ,    #MeV, mass window
                  'DMassWin_CS_hhmumu'         : 100.  ,    #MeV, mass window
                  'LambdacMassWin'             : 100. , #MeV, mass window
                  'DMassLow'                   :1763.  ,    #MeV, low-mass veto

             
                  'GhostProbCut_hhmumu'     : 0.5,
                  
                  'D2PiMuMuOSLinePrescale'     : 1 ,
                  'D2PiMuMuOSLinePostscale'    : 1 ,
                  'D2PiMuMuSSLinePrescale'     : 1 ,
                  'D2PiMuMuSSLinePostscale'    : 1 ,
                  'D2KMuMuOSLinePrescale'      : 1 ,
                  'D2KMuMuOSLinePostscale'     : 1 ,
                  'D2KMuMuSSLinePrescale'      : 1 ,
                  'D2KMuMuSSLinePostscale'     : 1 ,
                  'D2PiPiPiCalLinePrescale'    : 0.01 ,
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
                  'D02K3PiLinePrescale'        : 0.005 ,
                  'D02K3PiLinePostscale'       : 1,
                  'D02PiPiPiPiLinePrescale'    : 0.01 ,
                  'D02PiPiPiPiLinePostscale'   : 1,
                  'Lambdac2PMuMuLinePrescale'  : 1 ,
                  'Lambdac2PMuMuLinePostscale' : 1 ,
                  'Lambdac2PPiPiLinePrescale'  : 0.01 ,
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



## Walter Bonivento, Andrea Contu and Francesco Dettori
## Modified from Strippinb13b
## Full DST needed for isolation cuts.
## Test:  Yes
DstarD02xx = {
    'BUILDERTYPE' : 'StrippingDstarD02xxConf',
    'CONFIG':  {
    'PrescalepipiBox'     : 1.
                  , 'PrescalemumuBox'     : 1.
                  , 'PrescaleKpiBox'    : 1.
                  , 'PrescaleemuBox'    : 1.
                  , 'PrescaleeKBox'    : 1.
                  , 'PrescaleepiBox'    : 1.
                  , 'PrescalepimuBox'    : 1.
                  , 'PrescaleKmuBox'    : 1.
                  , 'Prescalepipi_untagged_Box'     : 0.2
                  , 'Prescalemumu_untagged_Box'     : 1.
                  , 'PrescaleKpi_untagged_Box'    : 0.2
                  , 'Prescalepimu_untagged_Box'    : 1.
                  , 'PrescaleKmu_untagged_Box'    : 0.2
                  , 'PrescaleKpi_untagged_BoxMB' : 1.
                  , 'Prescalepipi_untagged_BoxMB':1.
                  , 'PrescaleKpi_untagged_BoxMBTrEff' : 1.
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
                  ,'RequireHlt'         : 1
                 ,'ConeAngles'     : {"08":0.8,"10":1.0,"12":1.2,"14":1.4}
                 ,'ConeVariables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                 ,'prefix'         : '' 
                   },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }




## muDST, Mat Charles + Patrick Spradlin
## Test:  Yes.
Xicc = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
     'LongTrackGEC'            : 150
    ,'controlPrescaleLc'       : 0.05
    ,'controlPrescaleXic'      : 0.1
    , 'controlPrescaleDp'      : 0.001
    , 'controlPrescaleD0'      : 0.001
    , 'controlPrescaleDsp'     : 0.00
    , 'signalPrescaleViaLc'    : 1.0
    , 'signalPrescaleViaXic'   : 1.0
    , 'signalPrescaleViaDp'    : 1.0
    , 'signalPrescaleViaD0'    : 1.0
    , 'signalPrescaleViaLcWC'  : 1.0
    , 'signalPrescaleViaLcDCS' : 1.0
    , 'signalPrescaleViaXicWC' : 1.0
    , 'signalPrescaleViaDpWC'  : 1.0
    , 'signalPrescaleViaD0WC'  : 1.0
    , 'signalPrescaleViaD0DCS' : 1.0
    , 'LcHlt2TisTosSpec' : { 'Hlt2.*CharmHadLambdaC2KPPi.*Decision%TOS' : 0 }
    , 'DzHlt2TisTosSpec' : { 'Hlt2.*CharmHadD02.*Decision%TOS' : 0 }
    , 'DpHlt2TisTosSpec' : { 'Hlt2.*CharmHadD2HHH.*Decision%TOS' : 0 }
    , 'XiHlt2TisTosSpec' : { 'Hlt2.*ChargedHyperon.*Decision%TOS' : 0 }
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
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
    ,'RunK3pi'          : True
    ,'Run4pi'           : True
    ,'Run2K2pi'         : True
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
    ,'RunK3pi'          : True
    ,'Run4pi'           : True
    ,'Run2K2pi'         : True
    ,'Run3Kpi'          : True
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
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
    ,'KKLLPrescale'         :    1.0
    ,'KKLLPostscale'        :    1.0
    ,'KPLLPrescale'         :    1.0
    ,'KPLLPostscale'        :    1.0
    ,'PPLLPrescale'         :    1.0
    ,'PPLLPostscale'        :    1.0
    ,'KKDDPrescale'         :    1.0
    ,'KKDDPostscale'        :    1.0
    ,'KPDDPrescale'         :    1.0
    ,'KPDDPostscale'        :    1.0
    ,'PPDDPrescale'         :    1.0
    ,'PPDDPostscale'        :    1.0
    ,'RequiredRawEvents' : ["Muon","Calo","Rich","Velo","Tracker","Trigger"]
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmToBeSwum' ]
    }



## Sebastian Bachmann
## New line in Stripping15
## Moved to micro-DST in PreS18 by Mat.
## Test:  Yes
X2D0D0 = {
    'BUILDERTYPE' : 'X2D0D0Conf',
    'CONFIG'      : {
    'D0MassWin'             :   60.,   # MeV
    'D0PT'                  : 3300.,   # MeV
    'D0VtxChi2Ndof'         :    8.,
    'D0daughterPT'          :  900.,   # MeV
    'D0daughterP'           : 5000.,   # MeV
    'D0daughterTrkChi2'     :    5.,
    'D0daughterHighPIDK'    :    8.,
    'D0daughterLowPIDK'     :    0.,
    'XMassWin'              :    0.    # MeV
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


## Calibration stream, Vava Gligorov
## New line in Stripping21
## Test:  Yes
LTUnbCharm = {
    'BUILDERTYPE' : 'LTUnbCharmLines',
    'CONFIG'      : {
    'PrescaleLTUnbCharmD2KPiTISLine' : 1.,
    'PrescaleLTUnbCharmD2KPiTOSLine' : 1.,
    'PrescaleLTUnbCharmD2KPiPiLine'  : 1.,
    'PrescaleLTUnbCharmDs2KKPiLine'  : 1.,
    'PrescaleLTUnbCharmLc2pKPiLine'  : 1.,
    'MinPT3Body'         : 500.,
    'MinPT2Body'         : 800.,
    'PIDK_pi'            : 0,
    'PIDK_k'             : 5,
    'PIDp_pi'            : 0,
    'PIDp_p'             : 5,
    'TrChi2'             : 3,
    'TrGhostProb'        : 0.3,
    'MassWindow'         : 65,
    'DOCA'               : 0.1,
    'VTXCHI2'            : 10,
    'DPT'                : 2500,
    'DDIRA'              : 0.98,
    'DTAU'               : 0.00025
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

JpsiMuMuforD0MuMu = {
    'BUILDERTYPE'	: 'JpsiMuMuforD0MuMuConf',
    'CONFIG'	: { 'Jpsi2MuMuForD0mumu_Prescale'              :    0.5   ,
                    'Jpsi2MuMuForD0mumu_Postscale'             :     1.   ,
                    'Jpsi2MuMuForD0mumu_MuonPT'                :  750.    ,
                    'Jpsi2MuMuForD0mumu_MuonP'                 : 5000.    ,
                    'Jpsi2MuMuForD0mumu_MuonTRCHI2DOF'         :   5.     ,
                    'Jpsi2MuMuForD0mumu_MuonBPVIPCHI2'         :   3.     ,
                    'Jpsi2MuMuForD0mumu_MinMass'               : 2900     , 
                    'Jpsi2MuMuForD0mumu_MaxMass'               : 3280     ,
                    'Jpsi2MuMuForD0mumu_VCHI2PDOF'             :  10.     ,
                    'Jpsi2MuMuForD0mumu_PT'                    : 1800.    , 
                    'Jpsi2MuMuForD0mumu_BPVVDCHI2'             :   20.    ,
                    'Jpsi2MuMuForD0mumu_BPVDIRA'               : 0.9997   ,
                    'Jpsi2MuMuForD0mumu_BPVIPCHI2'             : 15.      ,
                    'Jpsi2MuMuForD0mumu_MuonMaxBPVIPCHI2'      : 8.       ,
                    'Jpsi2MuMuForD0mumu_MuonMaxPT'             : 1100.    
                    },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'Charm' ]
    }

# For measurement of Vub combining B & D decays. Responsible: P. Owen
# New in S17b.
CharmForVub = {
    'BUILDERTYPE' : 'CharmForVubConf',
    'CONFIG'     :  {
    'TRACK_MINIPCHI2' : 6,
    'KAON_PIDK' : 3.,
    'KAON_MinPT' : 600.,
    'Kstar_MinPT' : 600.,
    'Kstar_MassMax' : 1040.,
    'Kstar_MassMin' : 750.,
    'Rho_MinPT' : 600.,
    'Rho_MassMax' : 1020,
    'Rho_MassMin' : 530.,
    'MUON_MinPT' : 800.,
    'MUON_PIDmu' : 0,
    'PION_MinPT' : 600.,
    'PION_PIDmu' : -1.,
    'PION_PIDK' : 0,
    'PAIR_SumPTMin' : 2800.,
    'D_MassMax' : 1950.,
    'D_DOCA' : 0.07,
    'D_MinP' : 20000.,
    'D0_FD'  :  4.0,
    'Dplus_FD'  :  4.0,
    'D_MCORR_MIN'  : 1400.,
    'D_MCORR_MAX'  : 2700.,
    'D_BPVVDZ' : 0.,
    'D_VtxChi2' : 9.,
    'BtoD_DeltaMass_MIN' : 2700.,
    'BtoD_DeltaMass_MAX' : 3700.,
    'B_DIRA' : 0.9994,
    'B_FDCHI2' : 100,
    'Dstar_MassMin' : 300.,
    'Dstar_DOCA' : 0.4,
    'Dstar_VtxChi2' : 9.,
    'Dstar_DeltaMass' : 200.,
    'HLT_FILTER_VMuNu' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision'))",
    'HLT_FILTER_HMuNu' : "(HLT_PASS_RE('Hlt2CharmSemilepD02HMuNu_.*Decision') | HLT_PASS_RE('Hlt2CharmHadD02HHXDst_.*Decision'))",
    #Pre and postscale
    'PrescaledVLinePrescale' : 1.0,
    'PrescaledHLinePrescale' : 1.0},
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm']
}

# Lambdac -> p h+ h-. Responsible: Patrick Spradlin  NEW DICTIONARY IS NEEDED!
#Lc BFs lines - Patrick Spradlin, S Ogilvy
Lambdac2PHH = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
                    'Daug_All_PT_MIN'           :  400.0 * MeV
                  , 'Daug_2of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'          : 1200.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    4.0
                  , 'Daug_2of3_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   :    8.0
                  , 'Proton_PIDpPIDpi_MIN'      :    5.0
                  , 'Proton_PIDpPIDK_MIN'       :   -1.0e7
                  , 'Pi_PIDK_MAX'               :    0.0
                  , 'K_PIDK_MIN'                :    5.0
                  , 'Comb_ADAMASS_WIN'          :   90.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.1 * mm
                  , 'Lambdac_VCHI2VDOF_MAX'     :   20.0
                  , 'Lambdac_acosBPVDIRA_MAX'   :   14.1 * mrad
                  , 'Lambdac_PVDispCut'         : "(BPVVDCHI2 > 16.0)"
                  , 'HltFilter'                 : "HLT_PASS_RE('Hlt2Global.*')"
                  , 'PrescaleLambdac2PKPi'      :    1.0
                  , 'PostscaleLambdac2PKPi'     :    1.0
                  , 'PrescaleLambdac2PKK'       :    1.0
                  , 'PostscaleLambdac2PKK'      :    1.0
                  , 'PrescaleLambdac2PPiPi'     :    1.0
                  , 'PostscaleLambdac2PPiPi'    :    1.0
                  , 'PrescaleLambdac2PPiKWS'    :    1.0
                  , 'PostscaleLambdac2PPiKWS'   :    1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'Charm' ]
    }


Lambdac2PHHPrescFull = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
                    'Daug_All_PT_MIN'           :  400.0 * MeV
                  , 'Daug_2of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'          : 1200.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    4.0
                  , 'Daug_2of3_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   :    8.0
                  , 'Proton_PIDpPIDpi_MIN'      :    5.0
                  , 'Proton_PIDpPIDK_MIN'       :   -1.0e7
                  , 'Pi_PIDK_MAX'               :    0.0
                  , 'K_PIDK_MIN'                :    5.0
                  , 'Comb_ADAMASS_WIN'          :   90.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.1 * mm
                  , 'Lambdac_VCHI2VDOF_MAX'     :   20.0
                  , 'Lambdac_acosBPVDIRA_MAX'   :   14.1 * mrad
                  , 'Lambdac_PVDispCut'         : "(BPVVDCHI2 > 16.0)"
                  , 'HltFilter'                 : "HLT_PASS_RE('Hlt2Global.*')"
                  , 'PrescaleLambdac2PKPi'      :    0.1
                  , 'PostscaleLambdac2PKPi'     :    1.0
                  , 'PrescaleLambdac2PKK'       :    0.1
                  , 'PostscaleLambdac2PKK'      :    1.0
                  , 'PrescaleLambdac2PPiPi'     :    0.1
                  , 'PostscaleLambdac2PPiPi'    :    1.0
                  , 'PrescaleLambdac2PPiKWS'    :    0.1
                  , 'PostscaleLambdac2PPiKWS'   :    1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'CharmCompleteEvent' ]
    }
# D0 -> phi gamma
# Responsible: Fatima Soomro
# Added: Stripping19
# Lives in StrippingD2XGamma.py
D2XGamma = {
    'BUILDERTYPE' : 'StrippingD2XGammaConf',
    'CONFIG' : {
     'TrChi2'              : 2.5
    ,'TrGhostProb'        : 0.3
    ,'TrIPchi2Phi'        : 25.
    ,'TrPTPhi'            : 500.
    ,'kDLL'               : 2.0
    ,'PhiMassWinL'        : 1000.
    ,'PhiMassWin'         : 50.
    ,'PhiVCHI2'           : 16.
    ,'D0MassWin'          : 200.
    ,'D_BPVLTIME'         : 0.2 * picosecond ## not used ATM                                                                                                                                                                               
    ,'D0_PT'              : 1000.
    ,'photonPT'           : 1700.
    ,'DstVCHI2'           : 25.
    ,'deltaMass'          : 160.0
    ,'deltaMassL'         : 130.0
    ,'pvRefit'             : False
    ,'D02PhiGammaPreScale'               : 1.0
    ,'D02PhiGammaPostScale'              : 1.0
    ,'PionLocation'   : "Phys/StdAllNoPIDsPions/Particles"
    ,'PhiLocation' : "Phys/StdLoosePhi2KK/Particles"
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : ['Charm' ]
    }

# D0 -> h+ h- pi-

# D(s)+ -> KS h+ h+ h-
# Responsible: Hamish Gordon
# Added: Stripping20
# Lives in StrippingD2KS0HHH_conf.py
D2KS0HHH = {
    'BUILDERTYPE' : 'D2KS0HHHConf',
    'CONFIG'      : {
        'KS0DaugP'                  : 3000  
        ,'KS0DaugPT'                :  200
        ,'KS0DaugTrackChi2'         :    4  
        ,'KS0DaugMIPChi2'           :   40
        ,'KS0DaugPIDK'              :    5
        ,'KS0MassWindow'            :   35 
        ,'KS0MIPChi2'               :    7 
        ,'KS0MIPChi2DD'             :    0
        ,'KS0VertexChi2'            :    8  
        ,'KS0PT'                    :  600
        ,'KS0BPVVDCHI2'             :  300
        ,'KS0BPVVDCHI2DD'           :  200
        ,'BachPionP'                : 5000               
        ,'BachPionPT'               :  320     
        ,'BachPionTrackChi2'        :    3
        ,'BachPionMIPChi2'          :    5 
        ,'BachPionPIDK'             :    0
        ,'BachKaonP'                : 5000                 
        ,'BachKaonPT'               :  320     
        ,'BachKaonTrackChi2'        :    3
        ,'BachKaonMIPChi2'          :    5 
        ,'BachKaonPIDK'             :    8 
        ,'DMesonComboLowMass'       : 1760          
        ,'DMesonComboHighMass'      : 2080
        ,'DMesonComboDOCA'          :  1.0 
        ,'DMesonAPT'                : 1700
        ,'DMesonADOCAChi2'          :   20 
        ,'dau2PT'                   : 1200
        ,'dau2IPCHI2'               :   20
        ,'DMesonMotherLowMass'      : 1770
        ,'DMesonMotherHighMass'     : 2070
        ,'DMesonMotherVertexChi2'   :   20
        ,'DMesonMotherMIPChi2'      :   15  
        ,'DMesonMotherPT'           : 1800
        ,'KS0ZDiff'                 :   10
        ,'DMesonFlightDistChi2'     :   50
        ,'MaxTracksInEvent'         :  400
        ,'PiPiPiLinePrescale'       :    1 
        ,'KPiPiLinePrescale'        :    1
        ,'KKPiLinePrescale'         :    1
        ,'KPiPiOSLinePrescale'      :    1 
        ,'PiPiPiLinePrescaleDD'     :    1
        ,'KPiPiLinePrescaleDD'      :    1
        ,'KKPiLinePrescaleDD'       :    1
        ,'KPiPiOSLinePrescaleDD'    :    1
        ,'PiPiPiLinePostscale'      :    1 
        ,'KPiPiLinePostscale'       :    1
        ,'KKPiLinePostscale'        :    1
        ,'KPiPiOSLinePostscale'     :    1
        ,'PiPiPiLinePostscaleDD'    :    1 
        ,'KPiPiLinePostscaleDD'     :    1
        ,'KKPiLinePostscaleDD'      :    1
        ,'KPiPiOSLinePostscaleDD'   :    1
         },
    'WGs' : [ 'Charm' ],
    'STREAMS' : ['Charm' ]
}


# D02KSKS Lines
# Micro-DST, Markward Britsch
# Added: Stripping20
# Lives in StrippingD02KSKS.py 
D02KSKS = {
    'BUILDERTYPE' : 'D02KSKSConf',
    'CONFIG'      : {
      'D0_MassWindowBeforeFit'          : 150.0,
      'D0_MassWindow'                   : 100.0,
      'D0_DOCA_LL'                      : 1.0,
      'D0_DOCA_DD'                      : 4.0,
      'KS_LL_nu2prime'                  : 0.0,
      'KS_LL_signedFLchi2'              : 50.0,
      'KS_LL_TRGHOSTPROB'               : 1.0,              # Track ghost probability KSLL
      'KS_DD_nu2prime'                  : 0.0,
      'KS_DD_signedFLchi2'              : 50.0,
      'KS_DD_TRGHOSTPROB'               : 1.0,              # Track ghost probability KSDD
      'D0_vertexChi2_LL'                : 20.0,
      'D0_IPchi2_LL'                    : 50.0,
      'D0_KS_signedFLchi2_LL'           : 200.0,
      'D0_signedFLchi2_LL'              : -1.0,
      'D0_vertexChi2_LD'                : 20.0,
      'D0_IPchi2_LD'                    : 10.0,
      'D0_KSLL_signedFLchi2_LD'         : 500.0,
      'D0_signedFLchi2_LD'              : 1.0,
      'D0_vertexChi2_DD'                : 20.0,
      'D0_IPchi2_DD'                    : 10.0,
      'D0_KS_nu2prime'                  : 4.0,
      'D0_lnPt_DD'                      : 21.0,
      'slowPi_IPchi2_LL'                : 40.0,
      'slowPi_IPchi2_LD'                : 40.0,
      'slowPi_IPchi2_DD'                : 40.0,
      'slowPi_LLL_TRGHOSTPROB'          : 1.0,  # Track ghost probability slow pi LL
      'slowPi_LDL_TRGHOSTPROB'          : 1.0,  # Track ghost probability slow pi LD
      'slowPi_DDL_TRGHOSTPROB'          : 1.0,  # Track ghost probability slow pi DD
      'slowPi_LLU_TRGHOSTPROB'          : 1.0,  # Track ghost probability slow pi LL
      'slowPi_LDU_TRGHOSTPROB'          : 1.0,  # Track ghost probability slow pi LD
      'slowPi_DDU_TRGHOSTPROB'          : 1.0,  # Track ghost probability slow pi DD
      'Dst_mass_LL'                     : 200.,
      'Dst_mass_LD'                     : 200.,
      'Dst_mass_DD'                     : 200.,
      'Dst_vertexChi2_LL'               : 40.0,
      'Dst_vertexChi2_LD'               : 40.0,
      'Dst_vertexChi2_DD'               : 40.0,
      'Dst_massDiffLow_LL'              : 135.,
      'Dst_massDiffHigh_LL'             : 160.,
      'Dst_massDiffLow_LD'              : 135.,
      'Dst_massDiffHigh_LD'             : 160.,
      'Dst_massDiffLow_DD'              : 135.,
      'Dst_massDiffHigh_DD'             : 160.,
      'PrescaleLL'                        : 1.0,
      'PostscaleLL'                       : 1.0,
      'PrescaleLD'                        : 1.0,
      'PostscaleLD'                       : 1.0,
      'PrescaleDD'                        : 1.0,
      'PostscaleDD'                       : 1.0,
      'DstPrescaleLL'                        : 1.0,
      'DstPostscaleLL'                       : 1.0,
      'DstPrescaleLD'                        : 1.0,
      'DstPostscaleLD'                       : 1.0,
      'DstPrescaleDD'                        : 1.0,
      'DstPostscaleDD'                       : 1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' :  ['Charm']
    }


# Cross-section lines.
# Imported in Stripping19, taking from the Stripping18 version of
# LineConfigDictionaries_CharmXsec.py, which was removed in Stripping18b.
# All of these require a min/micro/no-bias trigger and should have very low rate.
# Overall responsible: Patrick Spradlin
# Contributions from numerious people.

# Xsec: D0 -> K- pi- pi+ pi+
D02K3PiForXSec = {
    'BUILDERTYPE' : 'StrippingD02K3PiForXSecConf',
    'CONFIG'      : { 'Daug_All_PT_MIN'           :  250.0 * MeV
                  , 'Daug_3of4_PT_MIN'          :  300.0 * MeV
                  , 'Daug_2of4_PT_MIN'          :  350.0 * MeV
                  , 'Daug_1of4_PT_MIN'          :  400.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    1.0
                  , 'Daug_3of4_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_2of4_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of4_BPVIPCHI2_MIN'   :    4.0
                  , 'K_PIDK_MIN'                :    0.0
                  #, 'Pi_PIDK_MAX'               :   0.0
                  , 'Comb_ADAMASS_WIN'          :  100.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.5 * mm
                  , 'D0_VCHI2VDOF_MAX'          :   25.0
                  , 'D0_acosBPVDIRA_MAX'        :   35.0 * mrad
                  , 'D0_PVDispCut'              : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'Dstar_AMDiff_MAX'          :  160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX'       :  100.0
                  #
                  , 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'                 : None
                  #
                  , 'PrescaleD02K3Pi'             :  0.3
                  , 'PrescaleDstar2D0Pi_D02K3Pi'  :  0.3
                  , 'PostscaleD02K3Pi'            :  1.0
                  , 'PostscaleDstar2D0Pi_D02K3Pi' :  1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ] 
    }

# Xsec: D0 -> K- pi+ (geometrical selection)
D02KPiGeoForm = {
    'BUILDERTYPE'  : 'StrippingD02KPiGeoForXSecConf',
    'CONFIG'       : {'D0_ADAMASS_WIN'      : 250.0 * MeV
                  , 'D0_AMAXDOCA_MAX'     :  10.0 * mm
                  , 'D0_ADMASS_WIN'       : 125.0 * MeV
                  , 'D0_NU_2PT_MIN'       :  14.0
                  , 'D0_BPVVDSIGN_MIN'    :   1.0 * mm
                  , 'Dstar_AMDiff_MAX'    : 160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX' : 100.0
                  , 'Dstar_MDiff_MAX'     : 155.0 * MeV
                  , 'HltFilter'           : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'           : None
                  #
                  , 'PrescaleD02HH'             :   0.1
                  , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0

    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ] 
    }

# Xsec: D*+ -> D0 pi+, D0 -> K- pi+
Dstar2D0Pi_D02KPiForXSec = {
    'BUILDERTYPE' : 'StrippingDstar2D0Pi_D02KPiForXSecConf',
    'CONFIG'      : {
    'Daug_TRCHI2DOF_MAX'        :  10.0
                  , 'Daug_BPVIPCHI2_MIN'        :   6.0
                  , 'K_PIDK_MIN'                :   0.0
                  , 'Pi_PIDK_MAX'               :   0.0
                  , 'D0_ADAMASS_WIN'            :  80.0 * MeV
                  , 'D0_ADMASS_WIN'             :  75.0 * MeV
                  , 'D0_VCHI2VDOF_MAX'          :  25.0
                  , 'D0_BPVLTIME_MIN'           :   0.2 * picosecond
                  , 'D0_BPVLTFITCHI2_MAX'       : 100.0
                  , 'Dstar_AMDiff_MAX'          : 160.0 * MeV
                  , 'Dstar_VCHI2VDOF_MAX'       : 100.0
                  , 'Dstar_MDiff_MAX'           : 155.0 * MeV
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  #
                  , 'PrescaleD02HH'             :   0.1
                  , 'PrescaleDstar2D0Pi_D02HH'  :   0.1
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ] 
    }

# Xsec: Lc+ -> p K- pi+
Lambdac2PKPiForXSec = {
    'BUILDERTYPE' : 'StrippingLambdac2PKPiForXSecConf',
    'CONFIG'      : {
     'Daug_All_PT_MIN'          :  200.0 * MeV
                   , 'Daug_2of3_PT_MIN'         :  400.0 * MeV
                   , 'Daug_1of3_PT_MIN'         : 1000.0 * MeV
                   , 'Daug_All_BPVIPCHI2_MIN'   :    1.0
                   , 'Daug_2of3_BPVIPCHI2_MIN'  :    1.0
                   , 'Daug_1of3_BPVIPCHI2_MIN'  :    4.0
                   , 'Proton_PIDpPIDpi_MIN'     :    0.0
                   , 'Proton_PIDpPIDK_MIN'      :    0.0
                   , 'K_PIDK_MIN'               :    0.0
                   , 'Pi_PIDK_MAX'              :    3.0
                   , 'Comb_ADAMASS_WIN'         :   90.0 * MeV
                   , 'Comb_ADOCAMAX_MAX'        :    0.5 * mm
                   , 'Lambdac_VCHI2VDOF_MAX'    :   25.0
                   , 'Lambdac_acosBPVDIRA_MAX'  :   35.0 * mrad
                   , 'Lambdac_PVDispCut'        : "((BPVVDCHI2 > 4.0)|(BPVLTIME() > 0.075 * picosecond))"
                   , 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                   #, 'HltFilter'                :  None
                   , 'PrescaleLambdac2PKPi'     :    1.0
                   , 'PostscaleLambdac2PKPi'    :    1.0
                   , 'PrescaleLambdac2PKK'      :   -1.0
                   , 'PostscaleLambdac2PKK'     :   -1.0
                   , 'PrescaleLambdac2PPiPi'    :   -1.0
                   , 'PostscaleLambdac2PPiPi'   :   -1.0
                   , 'PrescaleLambdac2PPiKWS'   :   -1.0
                   , 'PostscaleLambdac2PPiKWS'  :   -1.0
    },
    'WGs'          : ['Charm'],
    'STREAMS'      : [ 'Charm' ] 
    }

# Xsec: D(s)+ -> phi pi+
D2PhiPiForXSec = {
    'BUILDERTYPE' : 'StrippingD2PhiPiForXSecConf',
    'CONFIG'      : {
    'K_BPVIPCHI2_MIN'           :    1.0
                  , 'Pi_BPVIPCHI2_MIN'          :    1.0
                  , 'K_PIDK_MIN'                :    0.0
                  , 'Phi_M_MIN'                 : 1000.0 * MeV
                  , 'Phi_M_MAX'                 : 1040.0 * MeV
                  , 'D_AM_MIN'                  : 1770.0 * MeV
                  , 'D_AM_MAX'                  : 2070.0 * MeV
                  , 'D_VCHI2VDOF_MAX'           :   25.0
                  , 'D_acosBPVDIRA_MAX'         :   35.0 * mrad
                  , 'D_PVDispCut'               : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'                 : None
                  #
                  , 'PrescaleD2PhiPi'           :    1.0
                  , 'PostscaleD2PhiPi'          :    1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ] 
    }

## Xsec: D(s)+ -> h+ h- h+
D2HHHForXSec = {
    'BUILDERTYPE' : 'StrippingD2HHHForXSecConf',
    'CONFIG'      : { 
     'Daug_All_PT_MIN'           :  200.0 * MeV
                  , 'Daug_2of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_1of3_PT_MIN'          :  400.0 * MeV
                  , 'Daug_All_BPVIPCHI2_MIN'    :    1.0
                  , 'Daug_2of3_BPVIPCHI2_MIN'   :    4.0
                  , 'Daug_1of3_BPVIPCHI2_MIN'   :    4.0
                  , 'K_PIDK_MIN'                :    0.0
                  , 'Pi_PIDK_MAX'               :    3.0
                  , 'Comb_AM_MIN'               : 1580.0 * MeV
                  , 'Comb_AM_MAX'               : 2260.0 * MeV
                  , 'Comb_ADOCAMAX_MAX'         :    0.5 * mm
                  , 'D_VCHI2VDOF_MAX'           :   25.0
                  , 'D_acosBPVDIRA_MAX'         :   35.0 * mrad
                  , 'D_PVDispCut'               : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'HltFilter'                 : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'                 : None
                  , 'PrescaleD2KPP'             :    1.0
                  , 'PrescaleD2KKP'             :    1.0
                  , 'PrescaleD2KKK'             :   -1.0
                  , 'PrescaleD2PPP'             :   -1.0
                  , 'PrescaleD2KPPDCS'          :   -1.0
                  , 'PostscaleD2KPP'            :    1.0
                  , 'PostscaleD2KKP'            :    1.0
                  , 'PostscaleD2KKK'            :   -1.0
                  , 'PostscaleD2PPP'            :   -1.0
                  , 'PostscaleD2KPPDCS'         :   -1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ] 
    }

# Xsec: D0 -> h+ h-
D02HHForXSec = { 
    'BUILDERTYPE' : 'StrippingD02HHForXSecConf',
    'CONFIG'      : {
     'Daug_PT_MIN'               : 250.0 * MeV
                  , 'Daug_BPVIPCHI2_MIN'        :   4.0
                  , 'K_PIDK_MIN'                :   0.0
                  , 'Pi_PIDK_MAX'               :   3.0
                  , 'D0_ADAMASS_WIN'            :  80.0*MeV
                  , 'D0_VCHI2VDOF_MAX'          :  25.0
                  , 'D0_acosBPVDIRA_MAX'        :  35.0 * mrad
                  , 'D0_PVDispCut'              : "((BPVVDCHI2 > 16.0)|(BPVLTIME() > 0.150 * picosecond))"
                  , 'Dstar_AMDiff_MAX'          : 160.0*MeV
                  , 'Dstar_VCHI2VDOF_MAX'       : 100.0
                  #
                  , 'HltFilter'          : "HLT_PASS_RE('Hlt1MB.*')"
                  #, 'HltFilter'          : none
                  #
                  , 'PrescaleD02HH'             :   1.0
                  , 'PrescaleDstar2D0Pi_D02HH'  :   1.0
                  , 'PostscaleD02HH'            :   1.0
                  , 'PostscaleDstar2D0Pi_D02HH' :   1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ] 
    }


# CharmFromBSemi Lines
# Micro-DST, Mika Vesterinen
# Moved to this file in Stripping20
# Lives in StrippingCharmFromBSemi.py
CharmFromBSemi = {
    'BUILDERTYPE' : 'CharmFromBSemiAllLinesConf',
    'CONFIG' :  {
    "GEC_nLongTrk"   : 250    # adimensional                                                                                                                                      
    ,"TTSpecs"      : {'Hlt1.*Track.*Decision%TOS':0,'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0,"Hlt2Global%TIS":0}
    ,"HLT"   : "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision') | HLT_PASS_RE('Hlt2Topo(2|3|4)Body.*Decision')"
    ,"TrGhostProbMax": 0.5    # adimensional                                                                                                                                      
    ,"MinBMass"      : 2500   # MeV                                                                                                                                               
    ,"MaxBMass"      : 6000   # MeV                                                                                                                                               
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
    ,"Pi0PtMin"         : 1000   # Minimum Pt of pi0 (MeV)                                                                                                                        
    ,"Pi0PMin"          : 3000   # Minimum P of pi0 (MeV)                                                                                                                         
    ,"PhotonCL"         : 0.25   # Confidence level for Pi0 photons                                                                                                               
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)                                                                                                                
    ,"D02HHPi0MassWin"  : 210  # MeV (mass window after vertex fit)                                                                                                               
    ,"D02KSHHPi0AMassWin" : 220  # MeV (mass window for combination)                                                                                                              
    ,"D02KSHHPi0MassWin"  : 210  # MeV (mass window after vertex fit)                                                                                                             
    ,"D02KSHHPi0_D0PTComb"      : 1000 # MeV (PT of D0 combination)                                                                                                               
    ,"D02KSHHPi0_D0PT"  : 1000 # MeV (PT of D0 combination)                                                                                                                       
    ,"D02KSHHPi0_PTSUMLoose"  : 1000 # MeV (sum PT of D0 daughters)                                                                                                               
    ,"D02HHPi0DocaCut"  : 6.0    # mm                                                                                                                                             
    ,"Dstar_Chi2"       :  8.0 ## unitless                                                                                                                                        
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless                                                                                                                                       
    ,"Dstar_SoftPion_PT" : 80. ## MeV ###                                                                                                                                         
    ,"Dstar_wideDMCutLower" : 0. ## MeV                                                                                                                                           
    ,"Dstar_wideDMCutUpper" : 170. ## MeV                                                                                                                                         
    ,"PTSUMLoose"  : 1400. ## MeV                                                                                                                                                 
    ,"PTSUM"       : 1800. ## MeV                                                                                                                                                 
    ,"PTSUM_HHPi0" : 1800. ## MeV 
    },
    'STREAMS' : ['Charm'] ,
    'WGs'    : ['Charm']
    }


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
    
    
##D* tagged D0 -> hhpi0, Kevin Maguire
##muDST
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
    
#Denis Derkach
#StrippingD2KShh_samesign
#Added in Stripping20rXp3
D2KShh_samesign = {
  'BUILDERTYPE' : 'D2KShhBuilder' ,
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
        ,'DCutVtxChi2_KK'       :   21.0      ## unitless
        ,'DCutVtxChi2_PP'       :   13.0      ## unitless
        ,'preFitDCutPT'         : 1500.0      ## in MeV
        ,'preFitDMassCut_LL'    :  130        ## in MeV
        ,'preFitDMassCut_DD'    :  270        ## in MeV
        ,'wideDMassCut_KKLL'    :   70.0      ## in MeV -- Mat changed from  57.2 to  70.0 in S20
        ,'wideDMassCut_PPLL'    :  120.0      ## in MeV -- Mat changed from 110.0 to 120.0 in S20
        ,'wideDMassCut_KKDD'    :   85.0      ## in MeV -- Mat changed from  72.0 to  85.0 in S20
        ,'wideDMassCut_PPDD'    :  150.0      ## in MeV -- Mat changed from 140.0 to 150.0 in S20
        ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
        ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
        ,'wideDMCutUpper'       :   15.0      ## in MeV -- Mat changed from 15.0 to 25.0 in S20
        ,'DstarCutPT_KK'        : 2200.0      ## in MeV
        ,'DstarCutPT_PP'        : 2200.0      ## in MeV
        ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
        ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
        ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
        ,'KKLLPrescale'         :    0.01
        ,'KKLLPostscale'        :    1.00
        ,'PPLLPrescale'         :    0.01
        ,'PPLLPostscale'        :    1.0
        ,'KKDDPrescale'         :    0.01
        ,'KKDDPostscale'        :    1.0
        ,'PPDDPrescale'         :    0.01
        ,'PPDDPostscale'        :    1.0
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
