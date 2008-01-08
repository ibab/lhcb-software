from LbUtils.Temporary import TempDir



import unittest
class TestCase(unittest.TestCase):
    """ test case for the temporary directory class """
    def setUp(self):
        unittest.TestCase.setUp(self)
        self.samplebasename = "toto"
        self.tmpdir = TempDir(self.samplebasename)
    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testName(self):
        name1 = self.tmpdir
        name2 = self.tmpdir.getName()
        print name1,name2, " ",
#       self.assert_(name1 == name2)
if __name__ == '__main__':
    unittest.main()