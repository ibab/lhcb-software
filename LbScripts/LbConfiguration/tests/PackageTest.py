from LbConfiguration.Package import getPackage, DecFiles, AppConfig
from LbConfiguration.Package import _getSVNPackage

import logging
import unittest

class PackageTestCase(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testPackage(self):
        m1 = getPackage("DecFiles")
        self.assertEqual(m1, DecFiles)
        self.assertEqual(m1.Name(), "DecFiles")
        self.assertEqual(m1.hat(), "Gen")
        self.assertEqual(m1.FullName(), "Gen/DecFiles")
        self.assertEqual(m1.project(), "DBASE")

        m2 = getPackage("AppConfig")
        self.assertEqual(m2, AppConfig)
        self.assertNotEqual(m2, DecFiles)
        self.assertEqual(m2.Name(), "AppConfig")
        self.assertEqual(m2.hat(), None)
        self.assertEqual(m2.FullName(), "AppConfig")
        self.assertEqual(m1.project(), "DBASE")
    def testSVNPackage(self):
        p = _getSVNPackage("DecFiles")
        self.assertEqual("DecFiles", p.Name())
        self.assertEqual("Gen", p.hat())
        self.assertEqual("DBASE", p.project())
        self.assertEqual("DBASE_Gen_DecFiles_v1r0", p.tarBallName("v1r0"))

    def testTarBall(self):
        m1 = getPackage("DecFiles")

        m2 = getPackage("AppConfig")


    def testHLTConfig(self):
        m1 = getPackage("HLTConfig")
        self.assertEqual(m1.Name(), "HLTConfig")
        self.assertEqual(m1.hat(), "WG")
        self.assertEqual(m1.FullName(), "WG/HLTConfig")
        self.assertEqual(m1.project(), "DBASE")

if __name__ == '__main__':
    log = logging.getLogger()
    log.setLevel(logging.CRITICAL)
    console = logging.StreamHandler()
    console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    log.addHandler(console)
    unittest.main()