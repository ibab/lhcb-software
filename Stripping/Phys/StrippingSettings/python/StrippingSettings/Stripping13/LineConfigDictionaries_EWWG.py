"""
Line config dictionaries for EW WG

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""


Z02MuMu = {
    'BUILDERTYPE'  : 'Z02MuMuConf',
    'CONFIG'       : {
    'Z02MuMuLinePrescale'    : 1.0,
    'Z02MuMuLinePostscale'   : 1.0,
    'Z0MinMass' : 40.,
    'mucut' : 15.    
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    
    }

#WMu
WMu = {
    'BUILDERTYPE'  : 'WMuConf',
    'CONFIG'       : {
    'WMuLinePrescale_ps'    : .1,
    'WMuLinePrescale'    : 1.0, 
    'WMuLinePostscale'   : 1.0,
    'mucuttight' : 20.,
    'mucut' : 15.,
    'mucutlow' : 5.,
    'trkpchi2' : 0.001
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

#DY2MuMu
DY2MuMu = {
    'BUILDERTYPE'  : 'DY2MuMuConf',
    'CONFIG'       : {
    'DY2MuMu1Line_psPrescale'    : .02,
    'DY2MuMu1Line_HltPrescale': .05,
    'DY2MuMu1Line_HltaPrescale': .5,
    'DY2MuMu1LinePostscale'   : 1.0,
    'DY2MuMu2LinePrescale'    : .5,
    'DY2MuMu2Line_HltPrescale': .1,
    'DY2MuMu2Line_HltaPrescale': 1.0,
    'DY2MuMu2LinePostscale'   : 1.0,
    'DY2MuMu3LinePrescale'    : 1.0,
    'DY2MuMu3LinePostscale'   : 1.0,
    'DY2MuMu4LinePrescale'    : 1.0,
    'DY2MuMu4LinePostscale'   : 1.0,
    'DY1MinMass' : 2.5,
    'DY1jpsiexclow' : 3.0,
    'DY1jpsiexchigh' : 3.2,
    'DY1MaxMass' : 5.,
    'DY2MinMass' : 5.,
    'DY2MaxMass' : 10.,
    'DY3MinMass' : 10.,
    'DY3MaxMass' : 20.,
    'DY4MinMass' : 20.,
    'DY4MaxMass' : 40.,
    'mupt1' : 800.,
    'mupt2' : 1.,
    'mutrkpchi2' : 0.001,
    'muipdvchi2ratio' : 5.,
    'mupidreq' : -3.,
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
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
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
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
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
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
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
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
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

#LowMult
LowMult = {
    'BUILDERTYPE'  : 'LowMultConf',
    'CONFIG'       : {
    'LowMultPrescale'    : 1.0,
    'LowMultPostscale'   : 1.0    
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

#DiPhotonDiMuon
DiPhotonDiMuon = {
    'BUILDERTYPE'  : 'PP2PPMuMuConf',
    'CONFIG'       : {
    'PP2PPMuMuLinePrescale'    : 1.0,
    'PP2PPMuMuLinePostscale'   : 1.0   
    },
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
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
    'STREAMS' : [ 'EW' ],
    'WGs'    : ['EW']
    }

DisplVertices = {
	'BUILDERTYPE'	: 'DisplVerticeLinesConf',
	'CONFIG'	: {'MaterialVeto': {'Double': 0,
                  'PreselVelo': 5,
                  'SingleDown': 0,
                  'SingleHighMass': 0,
                  'SingleLowMass': 5,
                  'SinglePS': 0},
 'MaxMass': {'Double': 14000000.0,
             'SingleDown': 14000000.0,
             'SingleHighMass': 14000000.0,
             'SingleLowMass': 12000.0,
             'SinglePS': 14000000.0},
 'MaxSumPt': {'Double': 14000000.0,
              'SingleDown': 14000000.0,
              'SingleHighMass': 14000000.0,
              'SingleLowMass': 14000000.0,
              'SinglePS': 14000000.0},
 'MinMass': {'Double': 3000.0,
             'PreselVelo': 3000.0,
             'SingleDown': 3000.0,
             'SingleHighMass': 12000.0,
             'SingleLowMass': 6800.0,
             'SinglePS': 5000.0},
 'MinR': {'Double': 0.29999999999999999,
          'PreselVelo': 0.29999999999999999,
          'SingleDown': 0.29999999999999999,
          'SingleHighMass': 0.29999999999999999,
          'SingleLowMass': 0.29999999999999999,
          'SinglePS': 0.29999999999999999},
 'MinSumPt': {'Double': 3000.0,
              'PreselVelo': 3000.0,
              'SingleDown': 3500.0,
              'SingleHighMass': 6000.0,
              'SingleLowMass': 6000.0,
              'SinglePS': 5000.0},
 'MinZ': {'Double': -10000.0,
          'SingleDown': 100.0,
          'SingleHighMass': -10000.0,
          'SingleLowMass': -10000.0,
          'SinglePS': -10000.0},
 'NCands': {'Double': 2,
            'SingleDown': 1,
            'SingleHighMass': 1,
            'SingleLowMass': 1,
            'SinglePS': 1},
 'NTracks': {'Double': 4,
             'PreselVelo': 4,
             'SingleDown': 4,
             'SingleHighMass': 5,
             'SingleLowMass': 6,
             'SinglePS': 5},
 'RCutMethod': 'FromUpstreamPV',
 'prescale': {'Double': 1.0,
              'SingleDown': 1.0,
              'SingleHighMass': 1.0,
              'SingleLowMass': 1.0,
              'SinglePS': 0.050000000000000003}
                           },
        'STREAMS' : [ 'EW' ],
        'WGs'    : ['Exotics']
}
