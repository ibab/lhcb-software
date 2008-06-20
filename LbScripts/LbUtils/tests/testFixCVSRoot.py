import unittest
import logging
import os

from LbUtils.CVS import fixRootFile, fixRoot
from LbUtils.Temporary import TempDir

class TestCase(unittest.TestCase):
    
    def setUp(self):
        log = logging.getLogger()
        log.addHandler(logging.StreamHandler())
        log.setLevel(logging.ERROR)
        unittest.TestCase.setUp(self)
        self.tmpdir = TempDir("testfixcvsroot")
        
        self.dir1 = os.path.join(self.tmpdir.getName(),"dir1")
        os.makedirs(self.dir1)
        
        self.dir2 = os.path.join(self.tmpdir.getName(),"dir2")
        os.makedirs(os.path.join(self.dir2,"CVS"))
        
        self.dir3 = os.path.join(self.tmpdir.getName(),"dir3")
        os.makedirs(os.path.join(self.dir3,"CVS"))
        f = open(os.path.join(self.dir3, "CVS", "Root"), "w")
        f.write("something")
        f.close()
        
        self.dir4 = os.path.join(self.tmpdir.getName(),"dir4")
        os.makedirs(os.path.join(self.dir4,"CVS"))
        f = open(os.path.join(self.dir4, "CVS", "Root"), "w")
        f.write(":ext:something")
        f.close()
        
        self.dir5 = os.path.join(self.tmpdir.getName(),"dir5")
        os.makedirs(os.path.join(self.dir5,"CVS"))
        f = open(os.path.join(self.dir5, "CVS", "Root"), "w")
        f.write(":kserver:something")
        f.close()

        self.dir6 = os.path.join(self.tmpdir.getName(),"dir6")
        os.makedirs(os.path.join(self.dir6,"CVS"))
        f = open(os.path.join(self.dir6, "CVS", "Root"), "w")
        f.write(":kserver:something")
        f.close()

        self.dir7 = os.path.join(self.tmpdir.getName(),"dir6", "dir7")
        os.makedirs(os.path.join(self.dir7,"CVS"))
        f = open(os.path.join(self.dir7, "CVS", "Root"), "w")
        f.write(":kserver:something")
        f.close()

        self.dir8 = os.path.join(self.tmpdir.getName(),"dir6", "dir7", "dir8")
        os.makedirs(os.path.join(self.dir8,"CVS"))
        f = open(os.path.join(self.dir8, "CVS", "Root"), "w")
        f.write(":kserver:something")
        f.close()

    def tearDown(self):
        unittest.TestCase.tearDown(self)
        
    def testSimpleEmptyDirectory(self):
        fixRootFile(self.dir1, ":kserver:", ":ext:")
        self.assertFalse(os.path.exists(os.path.join(self.dir1,"CVS")))
        self.assertFalse(os.path.exists(os.path.join(self.dir1,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir1,"CVS","Root.new")))
        self.assert_(not os.path.exists(os.path.join(self.dir1,"CVS","Root.bak")))

    def testSimpleDirectoryWithoutRoot(self):
        fixRootFile(self.dir2, ":kserver:", ":ext:")
        self.assertTrue(os.path.exists(os.path.join(self.dir2,"CVS")))
        self.assertFalse(os.path.exists(os.path.join(self.dir2,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir2,"CVS","Root.new")))
        self.assertFalse(os.path.exists(os.path.join(self.dir2,"CVS","Root.bak")))
        
    def testSimpleDirectoryWithFakeRoot(self):
        fixRootFile(self.dir3, ":kserver:", ":ext:")
        self.assertTrue(os.path.exists(os.path.join(self.dir3,"CVS")))
        self.assertTrue(os.path.exists(os.path.join(self.dir3,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir3,"CVS","Root.new")))
        self.assertFalse(os.path.exists(os.path.join(self.dir3,"CVS","Root.bak")))
        f = open(os.path.join(self.dir3, "CVS", "Root"), "r")
        self.assertTrue(f.read() == "something")
        f.close()

    def testSimpleDirectoryWithWrongRoot(self):
        fixRootFile(self.dir4, ":kserver:", ":ext:")
        self.assertTrue(os.path.exists(os.path.join(self.dir4,"CVS")))
        self.assertTrue(os.path.exists(os.path.join(self.dir4,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir4,"CVS","Root.new")))
        self.assertFalse(os.path.exists(os.path.join(self.dir4,"CVS","Root.bak")))
        f = open(os.path.join(self.dir4, "CVS", "Root"), "r")
        self.assertTrue(f.read() == ":ext:something")
        f.close()

    def testSimpleDirectoryWithRoot(self):
        fixRootFile(self.dir5, ":kserver:", ":ext:")
        self.assertTrue(os.path.exists(os.path.join(self.dir5,"CVS")))
        self.assertTrue(os.path.exists(os.path.join(self.dir5,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir5,"CVS","Root.new")))
        self.assertTrue(os.path.exists(os.path.join(self.dir5,"CVS","Root.bak")))
        f = open(os.path.join(self.dir5, "CVS", "Root"), "r")
        self.assertTrue(f.read() == ":ext:something")
        f.close()
        
    def testDirectory(self):
        fixRoot(self.dir6, ":kserver:", ":ext:", True)
        self.assertTrue(os.path.exists(os.path.join(self.dir6,"CVS")))
        self.assertTrue(os.path.exists(os.path.join(self.dir6,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir6,"CVS","Root.new")))
        self.assertTrue(os.path.exists(os.path.join(self.dir6,"CVS","Root.bak")))
        f = open(os.path.join(self.dir6, "CVS", "Root"), "r")
        self.assertTrue(f.read() == ":ext:something")
        f.close()
        self.assertTrue(os.path.exists(os.path.join(self.dir7,"CVS")))
        self.assertTrue(os.path.exists(os.path.join(self.dir7,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir7,"CVS","Root.new")))
        self.assertTrue(os.path.exists(os.path.join(self.dir7,"CVS","Root.bak")))
        f = open(os.path.join(self.dir7, "CVS", "Root"), "r")
        self.assertTrue(f.read() == ":ext:something")
        f.close()
        self.assertTrue(os.path.exists(os.path.join(self.dir8,"CVS")))
        self.assertTrue(os.path.exists(os.path.join(self.dir8,"CVS","Root")))
        self.assertFalse(os.path.exists(os.path.join(self.dir8,"CVS","Root.new")))
        self.assertTrue(os.path.exists(os.path.join(self.dir8,"CVS","Root.bak")))
        f = open(os.path.join(self.dir8, "CVS", "Root"), "r")
        self.assertTrue(f.read() == ":ext:something")
        f.close()
        

if __name__ == '__main__':
    unittest.main()