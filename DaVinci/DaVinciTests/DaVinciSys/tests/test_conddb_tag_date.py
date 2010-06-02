#!/usr/bin/env python
'''
Unit tests DaVinci.Configuration.isNewCondDBTag function.
'''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

from DaVinci.Configuration import isNewCondDBTag

def test_new_tags() :
    tags = ["sim-20100510-vc10mm-mu100",
            "sim-20100510-vc10mm-md100",
            "sim-20100510-vc-mu100",
            "sim-20100510-vc-moff",
            "sim-20100510-vc-md100",
            "hlt-20100509",
            "head-20100504",
            "MC09-20100430-vc-md100",
            "MC09-20100430-vc-moff",
            "MC09-20100430-vc-mu100",
            "MC09-20100430-vo-mu100",
            "MC09-20100430-vo-moff",
            "MC09-20100430-vo-md100",
            "hlt-20100430",
            "head-20100430",
            "sim-20100429-vc-md100",
            'default',
            '']

    for tag in tags :
        assert isNewCondDBTag(tag) == True

def test_old_tags() :
    tags = ["hlt-20100414",
            "head-20100414",
            "MC-20100412-vc15mm-md100",
            "sim-20100412-vc-mu100"]

    for tag in tags :
        assert isNewCondDBTag(tag) == False

def test_reference_date_is_old() :
    assert isNewCondDBTag('xxx-20100414-xxx-xxx-xxx') == False

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
