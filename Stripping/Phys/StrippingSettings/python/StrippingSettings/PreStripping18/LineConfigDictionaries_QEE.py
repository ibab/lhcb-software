"""
Line config dictionaries for the EW Stream. Mostly QEE WG lines


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""

from GaudiKernel.SystemOfUnits import *
#import GaudiKernel.SystemOfUnits as units


#Z02MuMu
Z02MuMu = {
    'BUILDERTYPE'  : 'Z02MuMuConf',
    'CONFIG'       : {
    'Z02MuMuLinePrescale'    : 1.0,
    'Z02MuMuLinePostscale'   : 1.0,
    'Z0MinMass' : 40.,
    'mucut' : 15.    
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : {'EW':['StrippingZ02MuMuLine','StrippingZ02MuMuNoPIDsLine'], 'Leptonic':['StrippingZ02MuMuLooseLine']} 
    }


#WMu
WMu = {
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMuLinePrescale_ps'    : .1
    ,  'WMuLinePrescale_hps': .05
    ,  'WMuLinePrescale'    : 1.0
    ,  'WMuLineLoosePrescale'    : 1.0
    ,  'WMuLineNoPIDsPrescale'    : 0.1
    ,  'WMuLinePostscale'   : 1.0
    ,  'mucuttight' : 20.
    ,  'mucut' : 15.
    ,  'mucut_hps' : 10. 
    ,  'mucutlow' : 5.
    ,  'trkpchi2' : 0.001
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : {'EW':['StrippingWMuLine','StrippingWMuSingleTrackNoBias'], 'Leptonic':['StrippingWMuLine_NoPIDsps','StrippingWMuLine_loose','StrippingWMuLine_looseps','StrippingWMuLowPTLine']}  
    }

#DY2MuMu
DY2MuMu = {
    'BUILDERTYPE'  : 'DY2MuMuConf',
    'CONFIG'       : {
    'DY2MuMu1Line_psPrescale'    : .02
    ,  'DY2MuMu1Line_HltPrescale': .2 
    ,  'DY2MuMu1Line_HltaPrescale': 1.0 
    ,  'DY2MuMu1LinePostscale'   : 1.0
    ,  'DY2MuMu2LinePrescale'    : .05 
    ,  'DY2MuMu2Line_HltPrescale': .4
    ,  'DY2MuMu2Line_HltaPrescale': 1.0
    ,  'DY2MuMu2LinePostscale'   : 1.0
    ,  'DY2MuMu3LinePrescale'    : 1.0
    ,  'DY2MuMu3LinePostscale'   : 1.0
    ,  'DY2MuMu4LinePrescale'    : 1.0
    ,  'DY2MuMu4LinePostscale'   : 1.0
    ,  'DY1MinMass' : 2.5
    ,  'DY1jpsiexclow' : 3.0
    ,  'DY1jpsiexchigh' : 3.2
    ,  'DY1MaxMass' : 5.
    ,  'DY2MinMass' : 5.
    ,  'DY2MaxMass' : 10.
    ,  'DY3MinMass' : 10.
    ,  'DY3MaxMass' : 20.
    ,  'DY4MinMass' : 20.
    ,  'DY4MaxMass' : 40.
    ,  'mupt1' : 800.
    ,  'mupt2' : 1.
    ,  'mutrkpchi2' : 0.001
    ,  'muipdvchi2ratio' : 5.
    ,  'mupidreq' : -3.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : {'EW':['StrippingDY2MuMuLine1_Hlt','StrippingDY2MuMuLine1_ps','StrippingDY2MuMuLine2','StrippingDY2MuMuLine2_Hlt','StrippingDY2MuMuLine3','StrippingDY2MuMuLine4'], 'Leptonic':['StrippingDY2MuMuLine1_Hlta','StrippingDY2MuMuLine2_Hlta']}  
    }

#Z02TauTau
Z02TauTau = {
    'BUILDERTYPE'  : 'Z02TauTauConf',
    'CONFIG'       : {
    'Z02TauTauLinePrescale'    : 1.0 
    ,  'Z02TauTauLinePostscale'   : 1.0
    ,  'Z0MinMass' : 14.
    ,  'mucut' : 10.
    ,  'picut' : 5.
    ,  'mutrpchi2' : 0.001
    ,  'pitrpchi2' : 0.001
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }


## #Z02TauTauProng
## Z02TauTauProng = {
##     'BUILDERTYPE'  : 'Z02TauTauProngConf',
##     'CONFIG'       : {
##     'Z2TauTau_Prong_LinePrescale'  : 1.0,
##     'Z2TauTau_Prong_LinePostscale' : 1.0,
##     ###
##     'TAU_MASS_LOW'                 : '400.0',    # MeV/c2
##     'TAU_MASS_HIGH'                : '1777.0',   # MeV/c2
##     ###
##     'Z_MASS_LOW'                   : '10000.0',  # MeV/c2
##     #'Z_MASS_HIGH'                 : not set
##     'VCHI2_Z_MAX'                  : '100',       # dl
##     ###
##     'PT_HAD_MIN'                   : '2000',     # MeV/c
##     'TRACKCHI2_HAD_MAX'            : '5',        # dl
##     ###
##     'PT_TAU_MIN'                   : '2000',     # MeV/c
##     'VCHI2_TAU_MAX'                : '20'       # dl
##     },
##     'WGs' : [ 'RD' ],
##     'STREAMS' : [ 'EW' ]
##     }



#Z02ee
Z02ee = {
    'BUILDERTYPE'  : 'Z02eeConf',
    'CONFIG'       : {
    'Z02eeLinePrescale'    : 1.0 
    ,  'Z02eeLinePostscale'   : 1.0
    ,  'Z0MinMass' : 40.
    ,  'ptcut' : 10.
    ,  'trkpchi2' : 0.0
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'mipdvchi2ratio' : 5.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#We
We = {
    'BUILDERTYPE'  : 'WeConf',
    'CONFIG'       : {
    'WeLinePrescale'    : 1.0 
    ,  'WeLinePostscale'   : 1.0
    ,  'WeLinePrescale_ps'    : .1 
    ,  'ptcut' : 20.
    ,  'ptcut_ps' : 15.
    ,  'trkpchi2' : 0.0
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'trkghostprob' : 0.6
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : {'EW':['StrippingWeLine'], 'Leptonic':['StrippingWeLine_ps']} 
    }

#DY2ee
DY2ee = {
    'BUILDERTYPE'  : 'DY2eeConf',
    'CONFIG'       : {
    'DY2eeLine1Prescale'    : .05 
    ,  'DY2eeLine2Prescale'   : .5 
    ,  'DY2eeLine3Prescale'   : 1.0
    ,  'DY2eeLine4Prescale'   : 1.0
    ,  'DY2eeLinePostscale'   : 1.0
    ,  'DY1MinMass' : 2.5
    ,  'DY1MaxMass' : 5.
    ,  'DY2MinMass' : 5.
    ,  'DY2MaxMass' : 10.
    ,  'DY3MinMass' : 10.
    ,  'DY3MaxMass' : 20.
    ,  'DY4MinMass' : 20.
    ,  'DY4MaxMass' : 40.
    ,  'ptcut1' : 1.
    ,  'ptcut2' : 2.
    ,  'ptcut3' : 5.
    ,  'ePID'   : 1.
    ,  'trkpchi2' : 0.0
    ,  'trkpchi2a' : 0.0
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    ,  'mipdvchi2ratio' : 5.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#MuMuSS
MuMuSS = {
    'BUILDERTYPE'  : 'MuMuSSConf',
    'CONFIG'       : {
    'MuMuSSLine1Prescale' : .01,
    'MuMuSSLine2Prescale' : .1,
    'MuMuSSLine3Prescale' : 1.0,
    'MuMuSSLine4Prescale' : 1.0,
    'MuMuSSLine1Postscale' : 1.0,
    'MuMuSSLine2Postscale' : 1.0,
    'MuMuSSLine3Postscale' : 1.0,
    'MuMuSSLine4Postscale' : 1.0,
    'MuMuSSLine1MinMass' : 2.5,
    'MuMuSSLine1jpsiexclow' : 3.0,
    'MuMuSSLine1jpsiexchigh' : 3.2,
    'MuMuSSLine1MaxMass' : 5.,
    'MuMuSSLine2MinMass' : 5.,
    'MuMuSSLine2MaxMass' : 10.,
    'MuMuSSLine3MinMass' : 10.,
    'MuMuSSLine3MaxMass' : 20.,
    'MuMuSSLine4MinMass' : 20.,
    'mucut' : 1.         
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'    : 1.0 
    ,  'LowMultPostscale'   : 1.0
    ,  'LowMultPrescale_ps'    : 0.005
    ,  'LowMultNoFilterPrescale':0.1 
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#DiPhotonDiMuon
DiPhotonDiMuon = {
    'BUILDERTYPE'  : 'PP2PPMuMuConf',
    'CONFIG'       : {
    'PP2PPMuMuLinePrescale'    : 1.0,
    'PP2PPMuMuLinePostscale'   : 1.0   
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }


#SingleTrackTIS
SingleTrackTIS = {
    'BUILDERTYPE'  : 'SingleTrackTISConf',
    'CONFIG'       : {
    'LowPtPrescale'     : .001
    , 'LowPtPostscale'  : 1.0
    , 'HighPtPrescale'  : .1
    , 'HighPtPostscale' : 1.0
    , 'LowPtCut'  : 5.
    , 'HighPtCut' : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : {'EW':['StrippingSingleTrackTISHighPt'], 'Leptonic':['StrippingSingleTrackTISLowPt']} 
    }

#DisplVertices
DisplVertices = {
    'BUILDERTYPE'  : 'DisplVerticeLinesConf',
    'CONFIG'       : {
    'NCands':{
       'SinglePS': 1 ,
       'JPsiHLT': 1 ,
       'MinBias': 1 ,
       'SingleDown': 1 ,
       'Double':2,
       'SingleHighMass':1,
       'SingleHighFD':1,
       'SingleMedium':1
    },
    'RCutMethod' : 'FromBeamSpot',
    'MinR':{
       'PreselVelo':     0.4*mm ,
       'SinglePS': 0.4*mm ,
       'JPsiHLT': 0.4*mm ,
       'MinBias': 0.4*mm ,
       'SingleDown': 3.0*mm ,
       'Double': 0.4*mm ,
       'SingleHighMass': 0.4*mm ,
       'SingleHighFD': 4.*mm,
       'SingleMedium': 2.5*mm
    },
    'MinMass':{
       'PreselVelo': 3.*GeV ,
       'SinglePS': 3.*GeV ,
       'JPsiHLT': 3.*GeV ,
       'MinBias': 3.*GeV ,
       'SingleDown': 4.*GeV,
       'Double': 3.5*GeV ,
       'SingleHighMass':  9.7*GeV,
       'SingleHighFD': 6.5*GeV,
       'SingleMedium': 8.5*GeV
    },
    'MinMass2':{
       'Double': 4.*GeV
    },
    'MinSumPt':{
       'PreselVelo': 3.*GeV ,
       'SinglePS': 3.*GeV ,
       'JPsiHLT': 3.*GeV ,
       'MinBias': 3.*GeV ,
       'SingleDown': 3.*GeV,
       'Double': 3.*GeV ,
       'SingleHighMass':  3.*GeV,
       'SingleHighFD': 3.*GeV,
       'SingleMedium': 3.*GeV
    },
    'NTracks':{
       'PreselVelo':     4,
       'SinglePS': 4 ,
       'JPsiHLT': 4 ,
       'MinBias': 4 ,
       'SingleDown': 4 ,
       'Double':4 ,
       'SingleHighMass': 5,
       'SingleHighFD': 5,
       'SingleMedium': 5
    },
    'MinZ':{
       'SingleDown': 100.*mm  ,
    },
    'MaterialVeto':{
       'PreselVelo': 5,
       'SinglePS': 0 ,
       'JPsiHLT': 0 ,
       'MinBias': 0 ,
       'SingleDown': 0 ,
       'Double': -5 ,
       'SingleHighMass':5,
       'SingleHighFD':5,
       'SingleMedium': 5
    },
    'prescale':{
       'SinglePS': 0.005 ,
       'JPsiHLT': 1. ,
       'MinBias': 1. ,
       'SingleDown': 1. ,
       'Double': 1. ,
       'SingleHighMass':1.,
       'SingleHighFD':1.,
       'SingleMedium': 1.
    },
    'HLT':{
       'SinglePS': '' ,
       'SingleDown': '' ,
       'Double': '' ,
       'SingleHighMass':'',
       'SingleHighFD':'',
       'SingleMedium':'',
       'JPsiHLT':"HLT_PASS_RE('Hlt2DiMuonJPsiDecision')",
       'MinBias':"HLT_PASS_RE('Hlt1MBNoBiasDecision')",
       'SingleHLTPS':"HLT_PASS_RE('Hlt2DisplVertices(Single|SingleMV)PostScaledDecision')",
       'DoubleHLTPS':"HLT_PASS_RE('Hlt2DisplVerticesDoublePostScaledDecision')"
    }
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

HighPtJets = {
    'BUILDERTYPE'  : 'HighPtJetsLinesConf',
    'CONFIG'       : {
    'MinWeightSeedCut' : 6
    ,  'ConeExtFactorCut' : 4.0
    ,  'MaxDeltaPhiCut'   : 5.4
    ,  'MaxDeltaEtaCut'   : 0.15
    ,  'Min1stJetPtCut'   : 10.0
    ,  'Min2ndJetPtCut'   : 7.0
    ,  'JetMultCut'       : 2
    ,  'MaxRPVCut'        : 0.8
    ,  'scale'            : 0.2
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

InclbJets = {
    'BUILDERTYPE'  : 'InclbJetsLinesConf',
    'CONFIG'       : {
    'NrPVsCut'         : 1
    ,  'NrSeedsCut'       : 2
    ,  'TrkChi2Cut'       : 3.0
    ,  'PrtMomCut'        : 2.0
    ,  'PrtPtCut'         : 0.8
    ,  'PrtIPSCut'        : 3.5
    ,  'DZSVPVCut'        : 1.0
    ,  'SumMomSVCut'      : 3.0
    ,  'VtxChi2Cut'       : 23.0
    ,  'scale'            : 0.1 
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }





A1MuMu = {
    'BUILDERTYPE'  : 'A1MuMuConf',
    'CONFIG'       : {
                'A1MuMu_LinePrescale'  : 1.0,
                'A1MuMu_LinePostscale'              : 1.0,
                'A1MuMu_checkPV'                    : False,
               ###
               'DIMUON_LOW_MASS'                   : '5000.0',    # MeV/c2
               #'DIMUON_HIGH_MASS'                 : not set,     # MeV/c2
               ###
               'PT_MUON_MIN'                       : '2500.0',    # MeV/c
               'P_MUON_MIN'                        : '2500.0',    # MeV/c (de facto no cut)
               'TRACKCHI2_MUON_MAX'                : '10',        # dl
               ###
               'PT_DIMUON_MIN'                     : '7500.0',    # MeV/c
               'VCHI2_DIMUON_MAX'                  : '12'         # dl
                 },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'Dimuon' ]
    }


