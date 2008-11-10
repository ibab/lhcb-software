#!/usr/bin/env python

import unittest

from Gaudi.Configuration import *
import GaudiKernel.Configurable
from GaudiKernel.Configurable import purge, applyConfigurableUsers
from Configurables import CondDB, DDDBConf
from Configurables import (CondDBCnvSvc,
                           CondDBAccessSvc,
                           CondDBDispatcherSvc,
                           CondDBLayeringSvc,
                           CondDBTimeSwitchSvc,
                           CondDBSQLiteCopyAccSvc)

#import logging
#from GaudiKernel.ProcessJobOptions import InstallRootLoggingHandler
#InstallRootLoggingHandler(level = logging.DEBUG)

orig_reader = None
orig_dict = None
def equalConfigurable(lhs, rhs):
    return lhs.getFullName() == rhs.getFullName()

class DetCondConfigurationTest(unittest.TestCase):

    def assertEqualsConfig(self, lhs, rhs):
        self.assertEquals(lhs.getFullName(), rhs.getFullName())
    
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.DDDB = DDDBConf()
        self.CondDB = CondDB()
    
    def tearDown(self):
        self.DDDB = self.CondDB = None
        purge()
        GaudiKernel.Configurable._appliedConfigurableUsers_ = False
        unittest.TestCase.tearDown(self)

    def test_000_originalConfiguration(self):
        """Check the default configuration"""
        applyConfigurableUsers()
        global orig_reader, orig_dict
        orig_reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        self.assertEquals(orig_reader.__class__.__name__,
                          "CondDBDispatcherSvc")
        orig_dict = dict(orig_reader.Alternatives)
        
    def test_010_addCondDBLayer_1(self):
        """Add one layer from CondDBAccessSvc instance""" 
        # Add the layer
        layer = CondDBAccessSvc("layer")
        self.CondDB.addLayer(layer)
        
        applyConfigurableUsers()
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # check if we have a layering svc
        self.assertEquals(reader.__class__.__name__, "CondDBLayeringSvc")
        # check for the new layer...
        self.assertEqual(reader.Layers[0], layer)
        # ... plus the original one
        self.assertEqualsConfig(reader.Layers[1], orig_reader)
                          
    def test_010_addCondDBLayer_2(self):
        """Add layers from allowed Configurable instances""" 
        # Add the layers (one per type)
        types = [CondDBAccessSvc,
                 CondDBDispatcherSvc,
                 CondDBLayeringSvc,
                 CondDBTimeSwitchSvc,
                 CondDBSQLiteCopyAccSvc]
        layers = []
        for i in range(len(types)):
            layer = types[i]("layer_%d"%i)
            layers.append(layer)
            self.CondDB.addLayer(layer)
        
        applyConfigurableUsers()
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # check if we have a layering svc
        self.assertEquals(reader.__class__.__name__, "CondDBLayeringSvc")
        # correct size?
        self.assertEquals(len(reader.Layers), len(layers) + 1)
        # correct order (inverse of insertion order)...
        layers.reverse()
        for i in range(len(types)):
            self.assertEqual(reader.Layers[i], layers[i])
        # ... plus the original one
        self.assertEqualsConfig(reader.Layers[len(layers)], orig_reader)
        
    def test_020_addCondDBAlternative_1(self):
        """Add one alternative from CondDBAccessSvc instance"""
        # Add the alternative
        alternative = CondDBAccessSvc("alternative")
        self.CondDB.addAlternative(alternative, "/Test")

        applyConfigurableUsers()
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # the reader should not have changed
        self.assertEqualsConfig(reader, orig_reader)
        # correct size?
        self.assertEquals(len(reader.Alternatives), len(orig_dict) + 1)
        # check the previous alternatives
        for k in orig_dict:
            self.assertEqualsConfig(reader.Alternatives[k], orig_dict[k])
        # plus the new one
        self.assertEqualsConfig(reader.Alternatives["/Test"], alternative)
        
    def test_020_addCondDBAlternative_2(self):
        """Replace one alternative from CondDBAccessSvc instance"""
        path = orig_dict.keys()[0]
        
        # Add the alternative
        alternative = CondDBAccessSvc("alternative")
        self.CondDB.addAlternative(alternative, path)

        applyConfigurableUsers()
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # the reader should not have changed
        self.assertEqualsConfig(reader, orig_reader)
        # correct size?
        self.assertEquals(len(reader.Alternatives), len(orig_dict))
        # check the previous alternatives
        for k in orig_dict:
            if k != path:
                self.assertEqualsConfig(reader.Alternatives[k], orig_dict[k])
            else:
                self.assertEqualsConfig(reader.Alternatives[k], alternative)
        
if __name__ == '__main__':
    unittest.main(testRunner = unittest.TextTestRunner(stream=sys.stdout,verbosity=2))
