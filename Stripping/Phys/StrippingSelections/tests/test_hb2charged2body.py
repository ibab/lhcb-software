#!/usr/bin/env python

from StrippingUtils import LineBuilderTests
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines as builder

config = { 'PrescaleB2Charged2Body'   : 1,      'PrescaleB2PPbar' : 1,
           'MinPTB2Charged2Body'      : 1100,    'MinPTB2PPbar' : 900,
           'MinIPB2Charged2Body'      : 0.15,
           'MinIPChi2B2Charged2Body'  : 100,     'MinIPChi2B2PPbar' : 9,
           'TrChi2'                   : 5,
           'PIDppi'                   : 0,
           'PIDpk'                    : -2,
           'MaxPTB2Charged2Body'      : 2700,   'MaxPTB2PPbar' : 2100,
           'MaxIPB2Charged2Body'      : 0.3,
           'MaxIPChi2B2Charged2Body'  : 200,    'MaxIPChi2B2PPbar' : 25,
           'CombMassLow'              : 4800,
           'CombMassHigh'             : 5800,   'CombMassWindow' : 200,
           'DOCA'                     : 0.08,   'VertexChi2B2PPbar' : 16,
           'BPT'                      : 1200,   'BPTB2PPbar' : 1000,
           'BIP'                      : 0.06,
           'BIPChi2B2Charged2Body'    : 12,     'BIPChi2B2PPbar' : 36,
           'BDIRA'                    : 0.9995,
           'BTAU'                     : 0.0009,
           'MassLow'                  : 4800,
           'MassHigh'                 : 5800
           }

def test_line_builder() :
    LineBuilderTests.test_line_builder( builder, config )
    
if __name__ == '__main__' :
    
    test_line_builder()
