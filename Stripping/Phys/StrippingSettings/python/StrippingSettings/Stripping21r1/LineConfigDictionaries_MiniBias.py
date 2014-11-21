################################################################################
##                          S T R I P P I N G  2 1                            ##
##                                                                            ##
##  Configuration for B2noC WG                                                ##
##  Contact person:                                                           ##
################################################################################
"""
Line config dictionaries for MiniBias.
"""


MiniBias = {
    'BUILDERTYPE'	:	'MiniBiasConf',
    'CONFIG'   :{"NoBiasLine_RE"       : "(HLT_PASS('Hlt1MBNoBiasDecision'))",
                 "NoBiasLine_Rate"     : 1,
                 "NoBiasLine_Limiter"  : "Hlt1MBNoBiasODINFilter",
                 "NoBiasLine_CondDB"   : False , # False before 2012
                 "L0AnyRLLine_RE"      : "(HLT_PASS('Hlt1L0AnyRateLimitedDecision'))",
                 "L0AnyRLLine_Rate"    : 1,
                 "L0AnyRLLine_Limiter" : "Hlt1L0AnyRateLimitedPostScaler",
                 "L0AnyPSLine_RE"      : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                 "L0AnyPSLine_PS"      : 0.1, 
                  }, 
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'MiniBias' ]
    }


BadEvents = {
    'BUILDERTYPE'       :       'BadEventsConf',
    'CONFIG'   :{"GEC"       : "((TrSOURCE('/Event/Rec/Track/Best', TrLONG) >> (sum(TrPT,TrP<1000000))>1000000))"
                  ,"prescale": 1.0
                  ,"postscale": 1.0
                  },  
    'WGs' : [ 'ALL' ],
    'STREAMS' : [ 'MiniBias' ]
}
