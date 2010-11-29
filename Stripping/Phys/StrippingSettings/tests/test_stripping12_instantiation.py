#!/usr/bin/env python
'''Tests of Stripping12 streams instantiation.'''

__author__ = 'Juan Palacios juan.palacios@nikhef.nl'

def test_instantiate_stripping12_streams() :
    '''
    Test that Stripping Streams can be instantiated.
    This is the most basic test that should be run when modifying or
    adding Stripping Streams to StrippingSelections.Streams.allStreams.
    '''
    from StrippingConf.Configuration import StrippingConf
    from StrippingSettings.Stripping12 import streams

    _streams = streams()

    sc = StrippingConf (Streams = _streams)

if __name__ == '__main__' :
    test_instantiate_stripping12_streams()
