"""
Line config dictionaries for MiniBias.
"""


MiniBias = {
    'BUILDERTYPE'	:	'MiniBiasConf',
    'CONFIG'	: {
    'L0AnyLine_Prescale': 0.01,
    'L0AnyLine_RE': "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
    'NoBiasLine_Prescale': 0.01,
    'NoBiasLine_RE': "(HLT_PASS_RE('Hlt1.*NoBias.*Decision'))"
    },
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'MiniBias' ]
    }

