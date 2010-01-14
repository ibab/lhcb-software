from LbConfiguration.Platform import NativeMachine
import unittest

class PlatformTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testFlavour(self):
        machine = NativeMachine()
        self.assertEqual(machine.OSFlavour(), "Fedora")
        self.assertEqual(machine.OSFlavour(teststring="Scientific Linux CERN SLC release 5.4 (Boron)"), "Scientific Linux")
        print machine.OSVersion()
    def testVersion(self):
        pass

if __name__ == '__main__':
    unittest.main()