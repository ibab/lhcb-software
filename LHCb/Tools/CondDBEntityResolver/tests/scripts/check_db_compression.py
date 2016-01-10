#!/usr/bin/env python

import unittest

import CondDBCompression

#from Gaudi.Configuration import *
#import GaudiKernel.Configurable
#from GaudiKernel.Configurable import purge, applyConfigurableUsers
#from Configurables import CondDB, DDDBConf
#from Configurables import (CondDBCnvSvc,
#                           CondDBAccessSvc,
#                           CondDBDispatcherSvc,
#                           CondDBLayeringSvc,
#                           CondDBTimeSwitchSvc,
#                           CondDBSQLiteCopyAccSvc)

#import logging
#from GaudiKernel.ProcessJobOptions import InstallRootLoggingHandler
#InstallRootLoggingHandler(level = logging.DEBUG)
import sys

import string
import random

def str_generator(size=6, chars=string.printable):
    return ''.join(random.choice(chars) for x in range(size))

class DetCondCompressionTest(unittest.TestCase):

    def assertEqualsConfig(self, lhs, rhs):
        self.assertEquals(lhs.getFullName(), rhs.getFullName())

    def setUp(self):
        unittest.TestCase.setUp(self)
        self.testin = 'data'
        self.testout = ['0WFoAOAAABAAA/Td6WFoAAAFpIt42AgAhARwAAAAQz1jMAQADZGF0YQBj8/OtAAEYBGvp8KWQQpkNAQAAAAABWVo=',
                        '1QlpoOTFBWSZTWa/mnnIAAAEBgCQABAAgADDMDHqCcXckU4UJCv5p5yA=']
        self.Nmethods = 1 # number of methods

#    def tearDown(self):
#        unittest.TestCase.tearDown(self)

    def test_compression(self):
        """Check all compression methods"""
        for method in range(self.Nmethods):
            ret = CondDBCompression.compress(self.testin,method)
            self.assertEquals(self.testout[method], ret)

    def test_compression_default(self):
        """Check default compression method"""
        ret = CondDBCompression.compress(self.testin)
        self.assertEquals(self.testout[0], ret)

    def test_decompression(self):
        """Check decompression method"""
        for method in range(self.Nmethods):
            ret = CondDBCompression.decompress(self.testout[method])
            self.assertEquals(self.testin, ret)

    def test_decompression_2(self):
        """Check decompression method with random string
           should do nothing here"""
        rdnstr = "<" + str_generator(random.randint(1,10000))
        ret = CondDBCompression.decompress(rdnstr)
        self.assertEquals(rdnstr, ret)

    def test_both(self):
        """Check both compression and decompression methods with 100 random
           strings -- output should be identical to input"""
        for x in range(100):
            rdnstr = str_generator(random.randint(1,10000))
            for method in range(self.Nmethods):
                ret = CondDBCompression.compress(rdnstr, method)
                self.assertTrue(ret)
                ret = CondDBCompression.decompress(ret)
                self.assertEquals(rdnstr, ret)

if __name__ == '__main__':
    unittest.main(testRunner = unittest.TextTestRunner(stream=sys.stdout,verbosity=0))
