#!/usr/bin/env python
#from StrippingSelections.Streams import allStreams
import sys

from StrippingConf.Configuration import StrippingConf
from StrippingSelections.Utils import buildStreams

allStreams = buildStreams('stripping20r2')


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

def test_locations_are_unique() :
    lineLocations = {}
    for stream in allStreams :
        for line in stream.lines :
            if line.outputLocation() :
                lineLocations[line.name()] = line.outputLocation()
    locations = lineLocations.values()
    badLines = []
    for loc in locations :
        if locations.count(loc) != 1 :
            badLines.append(loc)
    if len(badLines) > 0 :
        message = 'Found locations written out by more than one line:\n'+str(badLines)+'\n'
        sys.stderr.write(message)
        assert(False)
        
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

    import sys

    test_names = filter(lambda k : k.count('test_') > 0, locals().keys())

    __tests = filter( lambda x : x[0] in test_names, locals().items())
    

    message = ''
    summary = '\n'
    length = len(sorted(test_names,
                        cmp = lambda x,y : cmp(len(y),len(x)))[0]) +2
    
    for test in __tests :
        try :
            test[1]()
            message = 'PASS'
        except :
            message = "FAIL"
        summary += test[0].ljust(length) + ':' + message.rjust(10) + '\n'

    if summary.count('FAIL') > 0 :
        message = 'FAIL'
        wr = sys.stderr.write
    else :
        message = 'PASS'
        wr = sys.stdout.write

    summary += 'Global'.ljust(length) + ':' + message.rjust(10) + '\n\n'
    wr(summary)
        
