"""
Line config dictionaries for the Charm WG
"""


from GaudiKernel.SystemOfUnits import *

# Re-run a subset of the Xicc lines which include a hyperon
# in the final state to take proper advantage of triggers
# introduced in 2012.
#
# Responsible: Mat Charles
# Output to micro-DST

Xicc = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
    'LongTrackGEC'            : 150
    ,'controlPrescaleLc'       : 0.0
    ,'controlPrescaleXic'      : 0.1
    , 'controlPrescaleDp'      : 0.0
    , 'controlPrescaleD0'      : 0.0
    , 'controlPrescaleDsp'     : 0.0
    , 'signalPrescaleViaLc'    : 0.0
    , 'signalPrescaleViaXic'   : 1.0
    , 'signalPrescaleViaDp'    : 0.0
    , 'signalPrescaleViaD0'    : 0.0
    , 'signalPrescaleViaLcWC'  : 0.0
    , 'signalPrescaleViaLcDCS' : 0.0
    , 'signalPrescaleViaXicWC' : 1.0
    , 'signalPrescaleViaDpWC'  : 0.0
    , 'signalPrescaleViaD0WC'  : 0.0
    , 'signalPrescaleViaD0DCS' : 0.0
    , 'Hlt2TisTosSpec' : { 'Hlt2.*Decision%TOS' : 0 }
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

