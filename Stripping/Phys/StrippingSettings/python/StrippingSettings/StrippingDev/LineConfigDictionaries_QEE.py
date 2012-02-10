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


