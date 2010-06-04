#!/usr/bin/env python
from StrippingSelections.Streams import allStreams

def test_stream_locations() :
    streamLocations = {}
    for stream in allStreams :
        streamLocations[stream.name()] =  [stream.name() + '/' + loc for loc in stream.outputLocations()]
    return streamLocations

def check_location_validity(loc) :
    splitLocation = loc.replace('/Event/', '').split('/')
    return splitLocation[0] == 'Phys' and len(splitLocation) > 1

def test_locations_are_valid() :
    '''
    Check that the stream outputLocations start with "/Event/Phys/" of "Phys/"
    '''
    summary = {}
    for stream in allStreams :
        for loc in stream.outputLocations() :
            valid = check_location_validity(loc)
            summary[stream.name()] = valid
            if not valid :
                print 'streamName has invalid OutputLocation', loc
    assert (not False in summary)
    
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
    test_locations_are_valid()
