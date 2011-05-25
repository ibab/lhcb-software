"""
Line config dictionaries for MiniBias.

Stripping1Point4TeV definitions.
Dictionaries have the name of the line builder instance.
"""


MiniBias = {
    'BUILDERTYPE'	:	'MiniBiasConf',
    'CONFIG'	: {
    'L0AnyLine_Prescale': 1.,
    'L0AnyLine_RE': "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
    'NoBiasLine_Prescale': 1.,
    'NoBiasLine_RE': "(HLT_PASS_RE('Hlt1MB.*Bias.*Decision'))"
    },
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'MiniBias' ]
    }

