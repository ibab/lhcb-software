#!/usr/bin/python
#############################################################################
# Python script to test the PyDimRpc and PyDimRpc classes
#############################################################################

import time
from threading import *
from debug import *

from dimcpp import * 
# for dim_start_serving()
import dimc


e = Event()


#############################################################################
# Implement test classes functionality
#############################################################################
class PyRpc (DimRpc):
    def __init__(self):
         DimRpc.__init__(self, 'testRpc', 'I', 'I')
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


class PyRpcInfo(DimRpcInfo):
    def __init__(self):
        DimRpcInfo.__init__(self, 'testRpc', 'I', 'I', None)
        self.value = 1

    def rpcHandler(self):
        DEBUG("Getting value")
        i = getInt()
        DEBUG("Received value %d", i)
        
    def run(self):
        DEBUG("Calling thread started")
        while not e.isSet():
            self.setData(self.value)
            if self.value % 2:
                # blocking   
                DEBUG("Calling get int")
                v = self.getInt()
                DEBUG("Got value %d", v)
            else:
                # non blocking
                pass
            time.sleep(1)

#############################################################################
# Execute tests
#############################################################################

if __name__ == '__main__':
    DEBUG("Creating PyDimRpc ...")
    myrpc = PyRpc()
    #DEBUG("Creating PyDimRpcInfo ...")
    #myrpcCaller = PyRpcInfo()

    DEBUG("Starting DIM ...")
    dimc.dis_start_serving()

    #t = Thread(target = myrpcCaller.run)
    #t.start()

    try:
        while True:
            time.sleep(1)
    except:
        DEBUG("Stopping test")
        e.set()
