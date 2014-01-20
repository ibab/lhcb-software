#!/usr/bin/env python
'''Tests of StrippingStreams instantiation.'''

__author__ = 'Juan Palacios juan.palacios@nikhef.nl'

def test_instantiate_StrippingConf_with_all_streams() :
    '''
    Test that Stripping Streams can be instantiated.
    This is the most basic test that should be run when modifying or
    adding Stripping Streams to StrippingSelections.Streams.allStreams.
    '''

    from StrippingConf.Configuration import StrippingConf
    from StrippingSelections.Utils import buildStreams

    #streams = buildStreams("stripping20r1p3")
    streams = buildStreams("stripping20r0p3")

    sc = StrippingConf (Streams = streams)

if __name__ == '__main__' :
    test_instantiate_StrippingConf_with_all_streams()
