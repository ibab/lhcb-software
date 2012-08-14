"""
Line config dictionaries for MiniBias.
"""


MiniBias = {
    'BUILDERTYPE'	:	'MiniBiasConf',
    'CONFIG'   : {"NoBiasLine_RE"       : "(HLT_PASS_RE('Hlt1MB.*NoBias.*Decision'))",
                  "NoBiasLine_Rate"     : 1,
                  "NoBiasLine_Limiter"  : "Hlt1MBNoBiasODINFilter",
                  "L0AnyLine_RE"        : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                  "L0AnyLine_Rate"      : 1,
                  "L0AnyLine_Limiter"   : "Hlt1L0AnyRateLimitedPostScaler"
                  },
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'MiniBias' ]
    }

