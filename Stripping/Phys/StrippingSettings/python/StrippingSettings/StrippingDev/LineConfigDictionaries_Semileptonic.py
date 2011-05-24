"""
Line config dictionaries for the Semileptonic WG


Dictionary names should have the same name as the corresponding
StrippingSelections file containing the line builder instance.

   e.g  StrippingPromptCharm linebuilder should be added as:

      PromptCharm = { ... }
"""


from GaudiKernel.SystemOfUnits import *

B0q2DplusMuX = {
    'BUILDERTYPE' : 'B0q2DplusMuXAllLinesConf',
    'CONFIG' : { 'Presel' : { 'Prescale'    : 0.02 ,
                              'Postscale'   : 1.0 ,
                              'MuPT'        : 500, #MeV
                              'MuPidPi'     : -5.,
                              'MuPidK'      : -10.,
                              'MuTrChi2'    : 9,
                              'KPT'         : 300, #MeV
                              'KIP'         : 0.03, #mm
                              'KPidPi'      : -5,
                              'KPidMu'      : -40,
                              'KPidP'       : -40,
                              'KTrChi2'     : 16,
                              'KIPChi2'     : 4,
                              'PiPidK'      : -40,
                              'PiIP'        : 0.01, #mm
                              'PiPidMu'     : -5,
                              'PiTrChi2'    : 16,
                              'PiIPChi2'    : 4,
                              'DPT'         : 1000, #MeV
                              'D_APT'       : 1000, #MeV
                              'D_VPCHI2'    : 0.000250,
                              'D_BPVVDCHI2' : 49,
                              'B_VPCHI2'    : 0.000250,
                              'B_BPVDIRA'   : 0.997
                              },
    
                 'Tuned'   : { 'Prescale'    : 1.0 ,
                               'Postscale'   : 1.0 ,
                               'MuPT'        : 500, #MeV
                               'MuPidPi'     : -1.,
                               'MuPidK'      : -5,
                               'MuTrChi2'    : 5,
                               'KPT'         : 400, #MeV
                               'KIP'         : 0.04, #mm
                               'KPidPi'      : 0,
                               'KPidMu'      : 5,
                               'KPidP'       : -10,
                               'KTrChi2'     : 5,
                               'KIPChi2'     : 4,
                               'PiPidK'      : -10,
                               'PiIP'        : 0.04, #mm
                               'PiPidMu'     : -5,
                               'PiTrChi2'    : 10,
                               'PiIPChi2'    : 7,
                               'DPT'         : 1500, #MeV
                               'D_APT'       : 1200, #MeV
                               'D_VPCHI2'    : 0.010,
                               'D_BPVVDCHI2' : 144,
                               'B_VPCHI2'    : 0.010,
                               'B_BPVDIRA'   : 0.9980
                               },
    
                 'MC09'   : { 'Prescale'    : 1.0 ,
                              'Postscale'   : 1.0 ,
                              'MuPT'        : 600, #MeV
                              'MuPidPi'     : 0.,
                              'MuPidK'      : -10,
                              'MuTrChi2'    : 2,
                              'KPT'         : 400, #MeV
                              'KIP'         : 0.03, #mm
                              'KPidPi'      : 7,
                              'KPidMu'      : 10,
                              'KPidP'       : -10,
                              'KTrChi2'     : 3,
                              'KIPChi2'     : 4,
                              'PiPidK'      : -5,
                              'PiIP'        : 0.01, #mm
                              'PiPidMu'     : -2,
                              'PiTrChi2'    : 9,
                              'PiIPChi2'    : 7,
                              'DPT'         : 1500, #MeV
                              'D_APT'       : 1200, #MeV
                              'D_VPCHI2'    : 0.0150,
                              'D_BPVVDCHI2' : 144,
                              'B_VPCHI2'    : 0.0150,
                              'B_BPVDIRA'   : 0.9980
                              }
                 
                 },
    'STREAMS' : [ 'Semileptonic' ],
    'WGs'    : ['Semileptonic'] 
    }

