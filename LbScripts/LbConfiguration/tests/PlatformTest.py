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
        print machine.OSVersion()
        print machine.CMTSystem()
        print machine.CMTOSFlavour()
        print machine.CMTOSEquivalentFlavour()
        print machine.CMTCompatibleConfig()
        print machine.CMTCompatibleConfig(debug=True)
        print machine.CMTSupportedConfig()
        print machine.CMTSupportedConfig(debug=True)
        print machine.nativeCompilerVersion()
        print machine.nativeCompiler()
        print machine.CMTNativeConfig()
        print machine.CMTNativeConfig(debug=True)
        print "================================================================"
        self.assertEqual(machine.OSFlavour(teststring="Scientific Linux CERN SLC release 5.4 (Boron)"), "Scientific Linux")
        print machine.OSVersion()
        print machine.CMTSystem()
        print machine.CMTOSFlavour()
        print machine.CMTOSEquivalentFlavour()
        print machine.CMTCompatibleConfig()
        print machine.CMTCompatibleConfig(debug=True)
        print machine.CMTSupportedConfig()
        print machine.CMTSupportedConfig(debug=True)
        print machine.nativeCompilerVersion()
        print machine.nativeCompiler()
        print machine.CMTNativeConfig()
        print machine.CMTNativeConfig(debug=True)
    def testVersion(self):
        pass

if __name__ == '__main__':
    unittest.main()