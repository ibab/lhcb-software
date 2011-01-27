#!/usr/bin/env python
#
#
from py.test import raises

from GaudiConfUtils import isIterable
from GaudiConfUtils import isConfigurable
from GaudiConfUtils import configurableExists
from GaudiConfUtils import filterConfigurables
from GaudiConfUtils import getConfigurablesFromModule
from GaudiKernel.Configurable import Configurable
class Dummy() :
    pass

class DummyConfigurable(Configurable) :
    def getGaudiType(self) : 
        pass
    def getDlls(self) :
        pass
    def getHandle(self) :
        pass

def test_isConfigurable() :
    assert isConfigurable(5.0) == False
    assert isConfigurable("bob") == False
    assert isConfigurable(Dummy()) == False
    assert isConfigurable( DummyConfigurable("TestConf") ) == True


def test_configurableExists() :
    assert configurableExists("bob") == False
    bob = DummyConfigurable("Bob")
    assert configurableExists("Bob") == True

def test_isIterable() :
    assert isIterable([1,2,3,4,5]) == True
    assert isIterable("Hello World") == False
    assert isIterable( (1,2,3,4,5,6) ) == True
    assert isIterable( { 'a' : 1, 'b' : 2, 'c' : 3 } ) == True
    assert isIterable(1) == False
    assert isIterable(1.0) == False
    assert isIterable(Dummy()) == False
    conf = DummyConfigurable("TestConf")
    assert isIterable(conf) == False

def test_filterConfigurables() :
    a = 1
    b = 2
    c = 3
    d = "hello"
    e = [1,2,3,4,5]
    f = 1.2345
    ca = DummyConfigurable('a')
    cb = DummyConfigurable('b')
    cc = DummyConfigurable('c')
    symbols = [a, ca, b, c ,cb, d, e, cc, f]
    assert filterConfigurables(symbols) == [ca, cb, cc]
    
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
