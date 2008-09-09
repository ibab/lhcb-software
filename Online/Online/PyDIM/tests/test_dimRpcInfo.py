#!/usr/bin/python
#############################################################################
# Python script to test the PyDimRpc and PyDimRpc classes
#############################################################################

import time, unittest, random
from threading import *
from debug import *

from dimcpp import * 
# importing for the dim_start_serving()
import dimc

##
# @addtogroup examples
# @{
#
class PyRpcInt (DimRpc):
    def __init__(self):
         DimRpc.__init__(self, 'testRpc1', 'I', 'I')
         self.counter = 0

    def rpcHandler(self):
         DEBUG('Getting value')
         i = self.getInt()
         DEBUG("getInt - received: ", i)
         i = self.getData()
         DEBUG("getData - received: ", i)
         self.counter += i
         DEBUG('Setting response data ', self.counter+1)
         self.setData(self.counter+1) 


class PyRpcIntInfo(DimRpcInfo):
    def __init__(self):
        DimRpcInfo.__init__(self, 'testRpc1', 'I', 'I', -1)
        self.value = -1
        self.count = 10

    def rpcInfoHandler(self):
        DEBUG("Getting value")
        i = self.getInt()
        DEBUG("Received value ", i)
        self.value = i
        
# ---------------------------------------------------------------------------
class PyRpcString (DimRpc):
    def __init__(self, name='testRpc2'):
         DimRpc.__init__(self, name, 'C', 'C')
         self.counter = 0

    def rpcHandler(self):
         s = self.getData()
         DEBUG("getData - received: ", s)
         l = [x for x in s]
         l.sort()
         s = ""
         for x in l: s += str(x)
         DEBUG('Setting response data ', s)
         self.setData(s)

class PyRpcStringInfo(DimRpcInfo):
    def __init__(self, name='testRpc2'):
        DimRpcInfo.__init__(self, name, 'C', 'C', 'Error')
        self.value = 'Error'

    def rpcInfoHandler(self):
        s = self.getString()
        DEBUG("Received value '"+s+"'")
        self.value = s

# ---------------------------------------------------------------------------
class PyRpcComposite (DimRpc):
    def __init__(self):
        self.format_in = 'I:1;F:2;D:2;X:1;C'
        self.format_out = 'C:10;I'
        DimRpc.__init__(self, 'testRpc3', self.format_in, self.format_out)
        self.counter = 0

    def rpcHandler(self):
         s = self.getData()
         DEBUG("getData - received: ", s)         
         tup = ("It's OK123", 1, 2, 3, 4, 5, 6, 7, 8, 9)
         DEBUG('Setting response data: ', tup)
         self.setData("It's OK123", 1, 2, 3, 4, 5, 6, 7, 8, 9)
         #self.setData(tup)

class PyRpcCompositeInfo(DimRpcInfo):
    def __init__(self):
        self.format_in = 'I:1;F:2;D:2;X:1;C'
        self.format_out = 'C:10;I'
        DimRpcInfo.__init__(self, 'testRpc3', self.format_in, 
                            self.format_out, None)
        self.value = None 

    def rpcInfoHandler(self):
        v = self.getData()
        DEBUG("Received value ", v)
        self.value = v

#############################################################################
# Implements test classes functionality
#############################################################################
myRpcInt = PyRpcInt()
myRpcIntCaller = PyRpcIntInfo()
myRpcString = PyRpcString()
myRpcStringCaller = PyRpcStringInfo()
myRpcComposite = PyRpcComposite()
myRpcCompositeCaller = PyRpcCompositeInfo()
#myRpcString = PyRpcString()
myRpcFailCaller = PyRpcStringInfo('testRcp4')
DEBUG("Starting DIM")
dimc.dic_disable_padding()
dimc.dis_start_serving()
dimc.dic_disable_padding()
time.sleep(1)

class TestDimRpc(unittest.TestCase):

    def testRpcInt(self):
        print('\n'+80*'-')
        print('Running RPC int test')
        print(80*'-')
        DEBUG('Running RPC int test')
        time.sleep(1)
        count = 1
        res = 0
        for i in range(1,10):
            count += i
            myRpcIntCaller.setData(i)
            res = myRpcIntCaller.getData()
            self.assertEqual(count, res) 
     
    def testRpcString(self):
        print('\n'+80*'-')
        print('Running RPC string test')
        print(80*'-'+'\n')
        time.sleep(1) # dim threads are not initialized imediately unfortunately 
        r=range(1,10)
        s1 = ""
        s2 = ""
        for x in r: s1 += str(x)
        random.shuffle(r)
        for x in r: s2 += str(x)
        myRpcStringCaller.setData(s2)
        res1 = myRpcStringCaller.getData() 
        res2 = myRpcStringCaller.value  
        self.assertEqual(s1, res1)
        self.assertEqual(s1, res2)
        
    def testRpcComposite(self):
        print('\n'+80*'-')
        print('Running RPC test using composite parameters')
        print(80*'-')
        tup = (1, 2.1, 2.2, 3.1, 3.2, 1234567890123, "Test string")
        DEBUG("Setting data", tup)
        myRpcCompositeCaller.setData(1, 2.1, 2.2, 3.1, 3.2, 1234567890123, "Test string")
        res1 = myRpcCompositeCaller.getData() 
        res2 = myRpcCompositeCaller.value
        DEBUG(res2)
        tup=("It's OK123", 1, 2, 3, 4, 5, 6, 7, 8, 9)
        self.assertEqual(tup, res1)
        self.assertEqual(tup, res2)

    def testRpcFailure(self):
        print('\n'+80*'-')
        print('Running RPC test using composite parameters')
        print(80*'-')
        myRpcFailCaller.setData('ALA BALA PORTOCALA')
        res1 = myRpcFailCaller.value
        res2 = myRpcFailCaller.getData()
        DEBUG(myRpcFailCaller.value)
        self.assertEqual('Error', res1)
        self.assertEqual(res1, res2)

##
# @}
#

#def suite():
#    tests = ['testRpcInt', 'testRpcString', 'testRpcComposite']
#    return unittest.TestSuite(map(WidgetTestCase, tests))


#############################################################################
# Execute tests
#############################################################################

if __name__ == '__main__':
    unittest.main()

