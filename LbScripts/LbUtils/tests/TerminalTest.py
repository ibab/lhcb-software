from LbUtils.Terminal import getSize

import unittest

class TerminalTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
    
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testGetSize(self):
        self.assertEqual(getSize(),(80,25)) # These are the default value if nothing can be fount
        
if __name__ == '__main__':
    unittest.main()