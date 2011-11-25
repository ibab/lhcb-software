#!/usr/bin/env python

from LbConfiguration.Version import CoreVersion, NotAVersion
from LbConfiguration.Version import sortVersions, extractVersion, sortStrings
from LbConfiguration.Version import LCGVersion
from LbUtils import versionSort

import unittest

class VersionTestCase(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testConstructor(self):
        v1 = "v1r0"
        self.assertEqual(CoreVersion(v1).name(), v1)
        self.assertEqual(CoreVersion(v1).version(), (1, 0, None, None))

        v2 = "v1r2"
        self.assertEqual(CoreVersion(v2).name(), v2)
        self.assertEqual(CoreVersion(v2).version(), (1, 2, None, None))
        self.assertNotEqual(CoreVersion(v2).version(), (1, 2, 0, None))
        self.assertNotEqual(CoreVersion(v2).version(), (1, 1, None, None))

        v3 = "v1r2p0"
        self.assertEqual(CoreVersion(v3).name(), v3)
        self.assertEqual(CoreVersion(v3).version(), (1, 2, 0, None))
        self.assertNotEqual(CoreVersion(v3).version(), (1, 2, 2, None))
        self.assertNotEqual(CoreVersion(v3).version(), (1, 2, None, None))
        self.assertNotEqual(CoreVersion(v3).version(), (1, 1, None, None))

        v4 = "v1r2p0g1"
        self.assertEqual(CoreVersion(v4).name(), v4)
        self.assertEqual(CoreVersion(v4).version(), (1, 2, 0, 1))
        self.assertNotEqual(CoreVersion(v4).version(), (1, 2, 2, None))
        self.assertNotEqual(CoreVersion(v4).version(), (1, 2, None, 0))
        self.assertNotEqual(CoreVersion(v4).version(), (1, 1, None, None))

        self.assertRaises(NotAVersion, CoreVersion, "v1r")
        self.assertRaises(NotAVersion, CoreVersion, "1r1")
        self.assertRaises(NotAVersion, CoreVersion, "v1r1p0l")
        self.assertRaises(NotAVersion, CoreVersion, None)

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

        vlist = ["v1r0","v1r1","v0r","v2r0",None,"v1r0p0"]
        self.assertRaises(NotAVersion, sortVersions, vlist)
        self.assertTrue(sortVersions(vlist, safe=True) == ["v1r0", "v1r0p0", "v1r1", "v2r0"])
        self.assertTrue(sortVersions(vlist, safe=True, reverse=True) == ["v2r0", "v1r1", "v1r0p0", "v1r0"])

    def testExtract(self):
        v1 = "wwe/GAUDI_v1r2.tar.gz"
        self.assertEqual(extractVersion(v1).name(), "v1r2")
        vlist = ["v1r0", "ddv2r0", "GAUDI_v0r0", v1, "blah"]
        vlistextr = [extractVersion(x) for x in vlist]


    def testStringSort(self):
        v1 = "wwe/GAUDI_v1r2.tar.gz"
        vlist = ["v1r0", "ddv2r0", "GAUDI_v0r0", v1, "blah"]
        self.assertRaises(AttributeError, sortStrings, vlist)
        self.assertEqual(sortStrings(vlist, safe=True),
                                     ["GAUDI_v0r0", "v1r0", "wwe/GAUDI_v1r2.tar.gz", "ddv2r0"])
        self.assertEqual(sortStrings(vlist, safe=True, reverse=True),
                                     ["ddv2r0", "wwe/GAUDI_v1r2.tar.gz", "v1r0", "GAUDI_v0r0"])

    def testLCGConstructor(self):
        v1 = "57a"
        self.assertEqual(LCGVersion(v1).name(), v1)
        self.assertEqual(LCGVersion(v1).version(), (57, "a"))
        self.assertNotEqual(LCGVersion(v1).version(), (57, "b"))
        self.assertNotEqual(LCGVersion(v1).version(), (55, "a"))
        self.assertRaises(NotAVersion, LCGVersion, "57aa")

        v2 = "59"
        self.assertEqual(LCGVersion(v2).name(), v2)
        self.assertEqual(LCGVersion(v2).version(), (59, ""))

    def testLCGString(self):
        v1 = "43w"
        self.assertEqual("%s" % LCGVersion(v1) , v1)

    def testLCGComp(self):
        v1 = LCGVersion("56")
        v2 = LCGVersion("57")
        v3 = LCGVersion("54a")
        v4 = LCGVersion("57b")
        v5 = LCGVersion("57c")
        self.assertTrue(v1 < v2)
        self.assertFalse(v1 > v2)
        self.assertFalse(v1 == v2)
        self.assertTrue(v1 > v3)
        self.assertTrue(v1 < v4)
        self.assertTrue(v1 < v5)
        self.assertTrue(v4 < v5)

    def testLCGExtract(self):
        v1 = "wwe/LCGCMT_56a.tar.gz"
        self.assertEqual(extractVersion(v1, versiontype=LCGVersion).name(), "56a")
        v2 = "CGCMT_58.tar.gz"
        self.assertEqual(extractVersion(v2, versiontype=LCGVersion).name(), "58")

    def testGenericSort(self):
        vlist = ["v1r0","v1r1","v0r1","v2r0","v1r0p1","v1r0p0"]
        self.assertTrue(versionSort(vlist) == ["v0r1", "v1r0","v1r0p0","v1r0p1", "v1r1", "v2r0"])
        vlist2 = ["GAUDI_v1r0","GAUDI_v1r1","GAUDI_v0r1","GAUDI_v2r0","GAUDI_v1r0p1","GAUDI_v1r0p0"]
        self.assertTrue(versionSort(vlist2) == ["GAUDI_v0r1", "GAUDI_v1r0","GAUDI_v1r0p0","GAUDI_v1r0p1", "GAUDI_v1r1", "GAUDI_v2r0"])
        vlist3 = ["54aa", "54ab", "56", "57", "54a", "57b", "57c", "54b"]
        self.assertTrue(versionSort(vlist3) == ["54a", "54aa", "54ab", "54b", "56", "57", "57b", "57c"])
        vlist4 = ["5.1.4", "5.1.1", "5.1", "5.1.0", "5.2.1", "5", "5.03.1", "5.1.1a", "5.1.010", "5.1.9"]
        self.assertTrue(versionSort(vlist4) == ["5", "5.1", "5.1.0", "5.1.1", "5.1.1a", "5.1.4", "5.1.9", "5.1.010", "5.2.1", "5.03.1"])


if __name__ == '__main__':
    unittest.main()