from LbConfiguration.Project import getTarBallName

import unittest
import logging

class ProjectTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testGetTarBallName(self):
        self.assertEqual(getTarBallName("Gaudi", "v21r0"), "GAUDI_GAUDI_v21r0.tar.gz")
        self.assertEqual(getTarBallName("Gaudi", "v21r0", "slc4_ia32_gcc34"),
                                        "GAUDI_GAUDI_v21r0_slc4_ia32_gcc34.tar.gz")
        self.assertEqual(getTarBallName("Gaudi", "v21r0", "slc4_ia32_gcc28"), None)
        self.assertEqual(getTarBallName("bLah", "v21r0", "slc4_ia32_gcc34"), None)
        
if __name__ == '__main__':
    log = logging.getLogger()
    log.setLevel(logging.CRITICAL)
    console = logging.StreamHandler()
    console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    log.addHandler(console)
    unittest.main()