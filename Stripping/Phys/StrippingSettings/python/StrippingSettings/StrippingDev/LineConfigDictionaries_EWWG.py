"""
Line config dictionaries for the EW WG


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
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }


#WMu
WMu = {
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMuLinePrescale_ps'    : .1,
    'WMuLinePrescale_hps': .05,
    'WMuLinePrescale'    : 1.0, 
    'WMuLinePostscale'   : 1.0,
    'mucuttight' : 20.,
    'mucut' : 15.,
    'mucut_hps' : 10., 
    'mucutlow' : 5.,
    'trkpchi2' : 0.001
    },
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
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
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
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
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }


#Z02ee
Z02ee = {
    'BUILDERTYPE'  : 'Z02eeConf',
    'CONFIG'       : {
    'Z02eeLinePrescale'    : 1.0, 
    'Z02eeLinePostscale'   : 1.0,
    'Z0MinMass' : 40.,
    'ptcut' : 10.,
    'trkpchi2' : 0.001,
    'HCalMax' : 0.05,
    'ECalMin' : 0.1,
    'PrsCalMin' : 50.,
    'mipdvchi2ratio' : 5.
    },
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }

#We
We = {
    'BUILDERTYPE'  : 'WeConf',
    'CONFIG'       : {
    'WeLinePrescale'    : 1.0,
    'WeLinePostscale'   : 1.0,
    'WeLinePrescale_ps'    : .1,
    'ptcut' : 20.,
    'ptcut_ps' : 15.,
    'trkpchi2' : 0.001,
    'HCalMax' : 0.05,
    'ECalMin' : 0.1,
    'PrsCalMin' : 50.,
    'trkghostprob' : 0.6
    },
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }

#DY2ee
DY2ee = {
    'BUILDERTYPE'  : 'DY2eeConf',
    'CONFIG'       : {
    'DY2eeLine1Prescale'    : .05,
    'DY2eeLine2Prescale'   : .5,
    'DY2eeLine3Prescale'   : 1.0,
    'DY2eeLine4Prescale'   : 1.0,
    'DY2eeLinePostscale'   : 1.0,
    'DY1MinMass' : 2.5,
    'DY1MaxMass' : 5.,
    'DY2MinMass' : 5.,
    'DY2MaxMass' : 10.,
    'DY3MinMass' : 10.,
    'DY3MaxMass' : 20.,
    'DY4MinMass' : 20.,
    'DY4MaxMass' : 40.,
    'ptcut1' : 1.,
    'ptcut2' : 2.,
    'ptcut3' : 5.,
    'ePID'   : 1.,
    'trkpchi2' : 0.001,
    'HCalMax' : 0.05,
    'ECalMin' : 0.1,
    'PrsCalMin' : 50.,
    'mipdvchi2ratio' : 5.
    },
    'WGs' : [ 'EW' ],
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
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'    : 1.0,
    'LowMultPostscale'   : 1.0    
    },
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }

#DiPhotonDiMuon
DiPhotonDiMuon = {
    'BUILDERTYPE'  : 'PP2PPMuMuConf',
    'CONFIG'       : {
    'PP2PPMuMuLinePrescale'    : 1.0,
    'PP2PPMuMuLinePostscale'   : 1.0   
    },
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }


#SingleTrackTIS
SingleTrackTIS = {
    'BUILDERTYPE'  : 'SingleTrackTISConf',
    'CONFIG'       : {
    'LowPtPrescale'     : .01,
    'LowPtPostscale'  : 1.0,
    'HighPtPrescale'  : .1,
    'HighPtPostscale' : 1.0,
    'LowPtCut'  : 5.,
    'HighPtCut' : 15.,
    },
    'WGs' : [ 'EW' ],
    'STREAMS' : [ 'EW' ]
    }

#DisplVertices
DisplVertices = {
    'BUILDERTYPE'  : 'DisplVerticeLinesConf',
    'CONFIG'       : {
    'NCands':{
    'SinglePS': 1 ,
    'SingleLowMass': 1 ,
    'SingleHighMass': 1 ,
    'SingleDown': 1 ,
    'Double':2
    },
    'RCutMethod' : 'FromBeamSpot',
    'MinR':{
    'PreselVelo':     0.3*mm , 
    'SinglePS': 0.3*mm ,
    'SingleLowMass': 0.4*mm ,
    'SingleHighMass': 0.3*mm ,
    'SingleDown': 3.0*mm ,
    'Double': 0.3*mm
    },
    'MinMass':{
    'PreselVelo':     3.*GeV , 
    'SinglePS': 5.*GeV  ,
    'SingleLowMass': 7.5*GeV ,
    'SingleHighMass':  12.*GeV,
    'SingleDown':  4.*GeV,
    'Double': 3.*GeV 
    },
    'MinSumPt':{
    'PreselVelo':     3.*GeV ,
    'SinglePS': 5.*GeV  ,
    'SingleLowMass': 7.*GeV ,
    'SingleHighMass':  6.*GeV,
    'SingleDown':  4.*GeV,
    'Double': 3.5*GeV
    },
    'MaxMass':{
    'SinglePS': 14*TeV ,
    'SingleLowMass': 12.*GeV,
    'SingleHighMass':  14*TeV ,
    'SingleDown':  14*TeV,
    'Double': 14*TeV
    },
    'MaxSumPt':{
    'SinglePS': 14*TeV ,
    'SingleLowMass': 14*TeV,
    'SingleHighMass':  14*TeV ,
    'SingleDown':  14*TeV ,
    'Double': 14*TeV
    },
    'NTracks':{
    'PreselVelo':     4,
    'SinglePS': 5,
    'SingleLowMass': 6,
    'SingleHighMass':  5 ,
    'SingleDown':  4 ,
    'Double': 4
    },
    'MinZ':{
    'SinglePS': -10*m,
    'SingleLowMass':  -10*m,
    'SingleHighMass':   -10*m ,
    'SingleDown': 100.*mm  ,
    'Double': -10*m
    },
    'MaterialVeto':{
    'PreselVelo':     5 ,
    'SinglePS': 0,
    'SingleLowMass': 5,
    'SingleHighMass':  0 ,
    'SingleDown':  0 ,
    'Double': 0
    },
    'prescale':{
    'SinglePS': 0.05,
    'SingleLowMass': 1.,
    'SingleHighMass':  1. ,
    'SingleDown':  1. ,
    'Double': 1.
    }
    },
    'WGs' : [ 'Exotics' ],
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
    ,  'scale'            : 1.0
    },
    'WGs' : [ 'Exotics' ],
    'STREAMS' : [ 'EW' ]
    }
