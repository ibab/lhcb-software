from LbRelease.TarBall import getTarBallNameItems, NotATarBall
from LbConfiguration.Version import NotAVersion

import unittest

class TarBallTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testLHCb(self):
        v1 = "GAUDI_GAUDI_v21r4.tar.gz"
        self.assertEqual(getTarBallNameItems(v1)[0], "Gaudi")
        self.assertEqual(getTarBallNameItems(v1)[1], "v21r4")
        self.assertEqual(getTarBallNameItems(v1)[2], None)
        v2 = "GAUDI_GAUDI_v21r4_slc4_ia32_gcc34.tar.gz"
        self.assertEqual(getTarBallNameItems(v2)[0], "Gaudi")
        self.assertEqual(getTarBallNameItems(v2)[1], "v21r4")
        self.assertEqual(getTarBallNameItems(v2)[2], "slc4_ia32_gcc34")
        v3 = "GAUDI_GAUDI_v21r4_slc4_ia32_gcc34_dbg.tar.gz"
        self.assertEqual(getTarBallNameItems(v3)[0], "Gaudi")
        self.assertEqual(getTarBallNameItems(v3)[1], "v21r4")
        self.assertEqual(getTarBallNameItems(v3)[2], "slc4_ia32_gcc34_dbg")
    def testRaise(self):
        v1 = "GAUDI_GAUDI_v21r4.ta.gz"
        self.assertRaises(NotATarBall, getTarBallNameItems, v1)
        v2 = "GAUDI_GAUDI_v21t4.tar.gz"
        self.assertRaises(NotAVersion, getTarBallNameItems, v2)
        v3 = "GAUDI_v21r4.tar.gz"
        self.assertRaises(NotATarBall, getTarBallNameItems, v3)
    def testBin(self):
        v1 = "GAUDI_GAUDI_v21r4_slc4_ia32_gcc34.ta.gz"
        self.assertRaises(NotATarBall, getTarBallNameItems, v1)
        v2 = "GAUDI_GAUDI_v21t4_slc4_ia32_gcc34.tar.gz"
        self.assertRaises(NotAVersion, getTarBallNameItems, v2)
        v3 = "GAUDI_v21r4_slc4_ia32_gcc34.tar.gz"
        self.assertRaises(NotATarBall, getTarBallNameItems, v3)

    def testLHCbPack(self):
        v1 = "DBASE_TCK_HltTCK_v1r0.tar.gz"
        self.assertEqual(getTarBallNameItems(v1)[0], "HltTCK")
        self.assertEqual(getTarBallNameItems(v1)[1], "v1r0")
        self.assertEqual(getTarBallNameItems(v1)[2], None)
    def testRaisePack(self):
        v1 = "DBASE_TCK_HltTCK_v1r0.ta.gz"
        self.assertRaises(NotATarBall, getTarBallNameItems, v1)
        v2 = "DBASE_TCK_HltTCK_v1t0.tar.gz"
        self.assertRaises(NotAVersion, getTarBallNameItems, v2)
        v3 = "DBASE_HltTCK_v1r0.tar.gz"
        self.assertRaises(NotATarBall, getTarBallNameItems, v3)

        
if __name__ == '__main__':
    unittest.main()