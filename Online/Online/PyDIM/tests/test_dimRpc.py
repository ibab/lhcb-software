#!/usr/bin/python
#############################################################################
# Python script to test the PyDimRpc and PyDimRpc classes
#############################################################################

import time
from threading import *
from debug import *

from pydim import DimRpc, DimRpcInfo, dis_start_serving



e = Event()
## 
# @addtogroup examples
# @{
# 
#############################################################################
# Implement some test classes functionality
#############################################################################
class PyRpc (DimRpc):
    def __init__(self):
         DimRpc.__init__(self, 'testRpc', 'I', 'I')
         self.counter = 0

    def rpcHandler(self):
         SAY('Server side funciton called. Getting parameters...')
         i = self.getInt()
         SAY("getInt - received: ", i)
         i = self.getData()
         SAY("getData - received: ", i)
         self.counter += i
         SAY('Setting response data ', self.counter+1)
         self.setData(self.counter+1) 


class PyRpcInfo(DimRpcInfo):
    def __init__(self):
        DimRpcInfo.__init__(self, 'testRpc', 'I', 'I', None)
        self.value = 1

    def rpcHandler(self):
        SAY("Non blocking RPC call executed")
        i = getInt()
        SAY("Received value %d", i)
        
    def run(self):
        DEBUG("Calling thread started")
        while not e.isSet():
            SAY('Calling server function') 
            self.setData(self.value)
            if self.value % 2:
                # blocking   
                SAY("Getting return value as int: ", self.getInt())
            else:
                # non blocking
                pass
            time.sleep(1)


##
# @}
#

#############################################################################
# Execute tests
#############################################################################

if __name__ == '__main__':
    DEBUG("Creating PyDimRpc ...")
    myrpc = PyRpc()
    DEBUG("Creating PyDimRpcInfo ...")
    myrpcCaller = PyRpcInfo()

    DEBUG("Starting DIM ...")
    dis_start_serving()

    t = Thread(target = myrpcCaller.run)
    t.start()

    try:
        while True:
            time.sleep(1)
    except:
        DEBUG("Stopping test")
        e.set()
