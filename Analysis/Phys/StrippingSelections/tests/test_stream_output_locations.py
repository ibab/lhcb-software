#!/usr/bin/env python
from StrippingSelections.Streams import allStreams

def test_stream_locations() :
    streamLocations = {}
    for stream in allStreams :
        streamLocations[stream.name()] =  [stream.name() + '/' + loc for loc in stream.outputLocations()]
    return streamLocations

if __name__ == '__main__' :
    streamLocations = test_stream_locations()
    print '\n============================================================================='
    for streamName in streamLocations.keys() :
        print 'Stream', streamName
        print '['
        for loc in streamLocations[streamName] :
            print loc
        print ']'
        print '============================================================================='
