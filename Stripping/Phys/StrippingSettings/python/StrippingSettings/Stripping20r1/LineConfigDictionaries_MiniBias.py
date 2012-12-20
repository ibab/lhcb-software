"""
Line config dictionaries for MiniBias.
"""


MiniBias = {
    'BUILDERTYPE' : 'MiniBiasConf',
    'CONFIG'   :{ "NoBiasLine_RE"       : "(HLT_PASS('Hlt1MBNoBiasDecision'))",
                  "NoBiasLine_Rate"     : 1,
                  "NoBiasLine_Limiter"  : "Hlt1MBNoBiasODINFilter",
                  "NoBiasLine_CondDB"   : False,
                  "L0AnyRLLine_RE"      : "(HLT_PASS('Hlt1L0AnyRateLimitedDecision'))",
                  "L0AnyRLLine_Rate"    : 1,
                  "L0AnyRLLine_Limiter" : "Hlt1L0AnyRateLimitedPostScaler",
                  "L0AnyPSLine_RE"      : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                  "L0AnyPSLine_PS"      : 0.01, # changed too
                }, 
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'MiniBias' ]
    }
