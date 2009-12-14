#!/usr/bin/env python

from LbConfiguration.Version import CoreVersion, NotAVersion, sortVersions

import unittest

class VersionTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testConstructor(self):
        v1 = "v1r0"
        self.assertEqual(CoreVersion(v1).name(), v1)
        self.assertEqual(CoreVersion(v1).version(), (1, 0, None))

        v2 = "v1r2"
        self.assertEqual(CoreVersion(v2).name(), v2)
        self.assertEqual(CoreVersion(v2).version(), (1, 2, None))
        self.assertNotEqual(CoreVersion(v2).version(), (1, 2, 0))
        self.assertNotEqual(CoreVersion(v2).version(), (1, 1, None))

        v3 = "v1r2p0"
        self.assertEqual(CoreVersion(v3).name(), v3)
        self.assertEqual(CoreVersion(v3).version(), (1, 2, 0))
        self.assertNotEqual(CoreVersion(v3).version(), (1, 2, 2))
        self.assertNotEqual(CoreVersion(v3).version(), (1, 2, None))
        self.assertNotEqual(CoreVersion(v3).version(), (1, 1, None))
        
        self.assertRaises(NotAVersion, CoreVersion, "v1r")
        self.assertRaises(NotAVersion, CoreVersion, "1r1")

    def testString(self):
        v1 = "v1r0"
        self.assertEqual("%s" % CoreVersion(v1) , v1)
        
    def testComp(self):
        v1 = CoreVersion("v1r0")
        v2 = CoreVersion("v1r1")
        v3 = CoreVersion("v2r0")
        v4 = CoreVersion("v1r0p0")
        v5 = CoreVersion("v1r0p1")
        self.assertTrue(v1 < v2)
        self.assertFalse(v1 > v2)
        self.assertFalse(v1 == v2)
        self.assertTrue(v1 < v3)
        self.assertTrue(v1 < v4)
        self.assertTrue(v1 < v5)
        self.assertTrue(v4 < v5)
    
    def testListSort(self):
        vlist = ["v1r0","v1r1","v0r1","v2r0","v1r0p1","v1r0p0"]
        self.assertTrue(sortVersions(vlist) == ["v0r1", "v1r0","v1r0p0","v1r0p1", "v1r1", "v2r0"])

        vlist = ["v1r0","v1r1","v0r","v2r0","v1r0p1","v1r0p0"]
        self.assertRaises(NotAVersion, sortVersions, vlist)
        self.assertTrue(sortVersions(vlist, safe=True) == ["v1r0", "v1r0p0", "v1r0p1", "v1r1", "v2r0"])

        
if __name__ == '__main__':
    unittest.main()