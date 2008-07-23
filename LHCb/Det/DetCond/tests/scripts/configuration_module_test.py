#!/usr/bin/env python

import unittest

from Gaudi.Configuration import *
from DetCond.Configuration import *
from GaudiKernel.Configurable import purge

class DetCondConfigurationTest(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
        importOptions("$DDDBROOT/options/DDDB.py")
    
    def tearDown(self):
        purge()
        unittest.TestCase.tearDown(self)
        
    def test_010_addCondDBLayer_1(self):
        """Add one layer from CondDBAccessSvc instance""" 
        orig_reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        self.assertEquals(orig_reader.__class__.__name__,
                          "CondDBDispatcherSvc")
        
        # Add the layer
        layer = CondDBAccessSvc("layer")
        addCondDBLayer(layer)
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # check if we have a layering svc
        self.assertEquals(reader.__class__.__name__, "CondDBLayeringSvc")
        # check for the new layer...
        self.assertEqual(reader.Layers[0], layer)
        # ... plus the original one
        self.assertEqual(reader.Layers[1], orig_reader)
                          
    def test_010_addCondDBLayer_2(self):
        """Add layers from allowed Configurable instances""" 
        orig_reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        self.assertEquals(orig_reader.__class__.__name__,
                          "CondDBDispatcherSvc")
        
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
            addCondDBLayer(layer)
        
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
        self.assertEqual(reader.Layers[len(layers)], orig_reader)
        
    def test_020_addCondDBAlternative_1(self):
        """Add one alternative from CondDBAccessSvc instance""" 
        orig_reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        self.assertEquals(orig_reader.__class__.__name__,
                          "CondDBDispatcherSvc")
        orig_dict = dict(orig_reader.Alternatives)
        
        
        # Add the alternative
        alternative = CondDBAccessSvc("alternative")
        addCondDBAlternative(alternative, "/Test")
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # the reader should not have changed
        self.assertEqual(reader, orig_reader)
        # correct size?
        self.assertEquals(len(reader.Alternatives), len(orig_dict) + 1)
        # check the previous alternatives
        for k in orig_dict:
            self.assertEqual(reader.Alternatives[k], orig_dict[k])
        # plus the new one
        self.assertEqual(reader.Alternatives["/Test"], alternative.getFullName())
        
    def test_020_addCondDBAlternative_2(self):
        """Replace one alternative from CondDBAccessSvc instance""" 
        orig_reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        self.assertEquals(orig_reader.__class__.__name__,
                          "CondDBDispatcherSvc")
        orig_dict = dict(orig_reader.Alternatives)
        path = orig_dict.keys()[0]
        
        # Add the alternative
        alternative = CondDBAccessSvc("alternative")
        addCondDBAlternative(alternative, path)
        
        reader = allConfigurables["CondDBCnvSvc"].CondDBReader
        # the reader should not have changed
        self.assertEqual(reader, orig_reader)
        # correct size?
        self.assertEquals(len(reader.Alternatives), len(orig_dict))
        # check the previous alternatives
        for k in orig_dict:
            if k != path:
                self.assertEqual(reader.Alternatives[k], orig_dict[k])
            else:
                self.assertEqual(reader.Alternatives[k], alternative.getFullName())
        
if __name__ == '__main__':
    unittest.main(testRunner = unittest.TextTestRunner(stream=sys.stdout,verbosity=2))
