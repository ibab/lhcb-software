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
    'Z02MuMu_Prescale'  : 1.0,
    'Z02MuMu_Postscale' : 1.0,
    'pT'    : 15.,
    'MMmin' : 40.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }


#WMu
WMu = {
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMu_Prescale'    : 1.0,
    'WMuLow_Prescale' : 0.1,
    'WMu_Postscale'   : 1.0,
    'pT'     : 20.,
    'pTlow'  : 15.,
    'pTvlow' :  5.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ] 
    }

#DY2MuMu
DY2MuMu = {
    'BUILDERTYPE'  : 'DY2MuMuConf',
    'CONFIG'       : {
    'DY2MuMu1LinePrescale'    : 0.05,
    'DY2MuMu1LineHltPrescale' : 0.50,
    'DY2MuMu2LinePrescale'    : 0.25,
    'DY2MuMu2LineHltPrescale' : 1.0,
    'DY2MuMu3LinePrescale'    : 1.0,
    'DY2MuMu4LinePrescale'    : 1.0,
    'DY2MuMuLinePostscale'    : 1.0,
    'DY1MinMass' :  3.2,
    'DY1MaxMass' :  5.,
    'DY2MinMass' :  5.,
    'DY2MaxMass' : 10.,
    'DY3MinMass' : 10.,
    'DY3MaxMass' : 20.,
    'DY4MinMass' : 20.,
    'DY4MaxMass' : 40.,
    'p'             : 10.,
    'pT1'           :  1.5,
    'pT2'           :  3.,
    'trkpchi2'      :  0.001,
    'pid'           : -3.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#Z02TauTau
Z02TauTau = {
    'BUILDERTYPE'  : 'Z02TauTauConf',
    'CONFIG'       : {
    'Z02TauTauLinePrescale'  : 1.0,
    'Z02TauTauLinePostscale' : 1.0,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.1,
    'HCalMax'   :  0.05,
    'trkpchi2'  :  0.001,
    'mupT'      : 15.,
    'epT'       : 15.,
    'pipT'      :  5.,
    'MMmin'     : 20.
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
    'Z02ee_Prescale'  : 1.0,
    'Z02ee_Postscale' : 1.0,
    'PrsCalMin'      : 50.,
    'ECalMin'        :  0.1,
    'HCalMax'        :  0.05,
    'pT'             : 10.,
    'MMmin'          : 40.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#We
We = {
    'BUILDERTYPE'  : 'WeConf',
    'CONFIG'       : {
    'We_Prescale'    : 1.0,
    'WeLow_Prescale' : 0.1,
    'We_Postscale'   : 1.0,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.10,
    'HCalMax'   :  0.05,
    'pT'        : 20.,
    'pTlow'     : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ] 
    }

#DY2ee
DY2ee = {
    'BUILDERTYPE'  : 'DY2eeConf',
    'CONFIG'       : {
    'DY2eeLine3Prescale' : 1.0,
    'DY2eeLine4Prescale' : 1.0,
    'DY2eeLinePostscale' : 1.0,
    'DY3MinMass' : 10.,
    'DY3MaxMass' : 20.,
    'DY4MinMass' : 20.,
    'DY4MaxMass' : 40.,
    'ePID'      :  1.,
    'PrsCalMin' : 50.,
    'ECalMin'   :  0.1,
    'HCalMax'   :  0.05,
    'pT3'        : 2.,
    'pT4'        : 5.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }

#MuMuSS
MuMuSS = {
    'BUILDERTYPE'  : 'MuMuSSConf',
    'CONFIG'       : {
    'MuMuSSLine1Prescale' :  .1,
    'MuMuSSLine2Prescale' : 1.0,
    'MuMuSSLine3Prescale' : 1.0,
    'MuMuSSLine4Prescale' : 1.0,
    'MuMuSSLinePostscale' : 1.0,
    'MuMuSSLine1MinMass' :  3.2,
    'MuMuSSLine1MaxMass' :  5.,
    'MuMuSSLine2MinMass' :  5.,
    'MuMuSSLine2MaxMass' : 10.,
    'MuMuSSLine3MinMass' : 10.,
    'MuMuSSLine3MaxMass' : 20.,
    'MuMuSSLine4MinMass' : 20.,
    'p'   : 10.,
    'pT1' :  1.5,
    'pT2' :  3.
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
    'PP2PPMuMuLinePrescale'    : 1.0 
    ,  'PP2PPMuMuLinePostscale'   : 1.0
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ]
    }


#SingleTrackTIS
SingleTrackTIS = {
    'BUILDERTYPE'  : 'SingleTrackTISConf',
    'CONFIG'       : {
    'SingleTrackTIS_Prescale'    : 0.10,
    'SingleTrackTISLow_Prescale' : 0.01,
    'SingleTrackTIS_Postscale'   : 1.00,
    'pT'    : 20.,
    'pTlow' : 15.
    },
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ] 
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
    'SingleHighMass': 0.6*mm ,
    'SingleHighFD': 4.*mm,
    'SingleMedium': 2.7*mm
    },
    'MinMass':{
    'PreselVelo': 3.*GeV ,
    'SinglePS': 3.*GeV ,
    'JPsiHLT': 3.*GeV ,
    'MinBias': 3.*GeV ,
    'SingleDown': 4.5*GeV,
    'Double': 3.5*GeV ,
    'SingleHighMass':  10.*GeV,
    'SingleHighFD': 6.7*GeV,
    'SingleMedium': 8.7*GeV
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
    'SingleDown': 150.*mm  ,
    },
    'MaterialVeto':{
    'PreselVelo': 5,
    'SinglePS': 0 ,
    'JPsiHLT': 0 ,
    'MinBias': 0 ,
    'SingleDown': 0 ,
    'Double': 5 ,
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

