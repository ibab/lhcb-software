#!/usr/bin/env python
'''
 Test suite for StrippingConf configuration
'''
__author__ = 'Juan Palacios juan.palacios@cern.ch'

from py.test import raises

from StrippingConf.Configuration import StrippingLine, StrippingStream, StrippingConf

from Configurables import FilterDesktop


def test_dumplicated_outputLocation_in_single_stream_raises_Exception() :

    filterX = FilterDesktop('FilterX')

    lineX0 = StrippingLine('LineX0', algos = [filterX])

    lineX1 = StrippingLine('LineX1', algos = [filterX])

    filterY = FilterDesktop('FilterY')

    lineY0 = StrippingLine('LineY0', algos = [filterY])

    lineY1 = StrippingLine('LineY1', algos = [filterY])

    stream = StrippingStream('TestStream', Lines = [lineX0, lineX1, lineY0, lineY1])

    raises( Exception, StrippingConf, Streams = [stream] )


def test_dumplicated_outputLocation_in_different_streams_raises_Exception() :

    filter = FilterDesktop('FilterX')

    line0 = StrippingLine('Line0b', algos = [filter])

    line1 = StrippingLine('Line1b', algos = [filter])

    stream0 = StrippingStream('Stream0', Lines = [line0])

    stream1 = StrippingStream('Stream0', Lines = [line1])

    raises( Exception, StrippingConf, Streams = [stream0,stream1] )

                    
if '__main__' == __name__ :

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
