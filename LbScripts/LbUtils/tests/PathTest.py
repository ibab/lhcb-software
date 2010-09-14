from LbUtils.Path import multiPathJoin, multiPathUpdate

import unittest
import os

class PathTestCase(unittest.TestCase):
    
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.tpath = "mysiteroot1:mysiteroot2"
        self.sub1  = "lhcb"
        self.sub2  = os.path.join("lcg", "external")
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testMultiJoin(self):
        self.assertEqual(multiPathJoin(self.tpath, self.sub1), "mysiteroot1/lhcb:mysiteroot2/lhcb")
        self.assertEqual(multiPathJoin(self.tpath, self.sub2), "mysiteroot1/lcg/external:mysiteroot2/lcg/external")

    def testMultiUpdate(self):
        self.assertEqual(multiPathUpdate(self.tpath, [self.sub1, self.sub2]), 
                         "mysiteroot1/lhcb:mysiteroot2/lhcb:mysiteroot1/lcg/external:mysiteroot2/lcg/external")
        self.assertEqual(multiPathUpdate(self.tpath, [self.sub1, self.sub2], interleaved=False), 
                         "mysiteroot1/lhcb:mysiteroot2/lhcb:mysiteroot1/lcg/external:mysiteroot2/lcg/external")
        self.assertEqual(multiPathUpdate(self.tpath, [self.sub1, self.sub2], interleaved=True), 
                         "mysiteroot1/lhcb:mysiteroot1/lcg/external:mysiteroot2/lhcb:mysiteroot2/lcg/external")
        
        
if __name__ == '__main__':
    unittest.main()