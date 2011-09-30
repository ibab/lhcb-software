from LbLegacy.install_project import calculateMD5, extractMD5SumFromFile
from LbLegacy.install_project import getMD5FileName
from LbUtils.File import computeMD5Sum

from LbUtils.Temporary import TempFile

import unittest

import time
import os

class MD5Test(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        self._tmpfile = TempFile()
        self._tmpfile._file.write(time.asctime())
        self._tmpfile._file.close
        self._md5tmpfile = TempFile()
        self._md5sum = "4752d4bee1e8792872732ba355e9285e"
        self._md5tmpfile._file.write("%s  DBASE_AppConfig_v3r112.tar.gz" % self._md5sum)
        self._md5tmpfile._file.close()
        self._md5tmpfile2 = TempFile()
        self._md5tmpfile2._file.write("%s DBASE_AppConfig_v3r112.tar.gz" % self._md5sum)
        self._md5tmpfile2._file.close()
        self._md5tmpfile3 = TempFile()
        self._md5tmpfile3._file.write("%s  DBASE_AppConfig_v3r112.tar.gz\n" % self._md5sum)
        self._md5tmpfile3._file.close()


    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testSameMD5(self):
        self.assertEqual(calculateMD5(self._tmpfile.name), computeMD5Sum(self._tmpfile.name))

    def testExtraction(self):
        self.assertEqual(self._md5sum, extractMD5SumFromFile(self._md5tmpfile.name))
        self.assertEqual(self._md5sum, extractMD5SumFromFile(self._md5tmpfile2.name))
        self.assertEqual("4752d4bee1e8792872732ba355e9285e", extractMD5SumFromFile(self._md5tmpfile3.name))

    def testMD5FileName(self):
        fn = "toto.tar.gz"
        self.assertEqual("toto.md5", getMD5FileName(fn))
        fn = os.path.join("/tmp", "blah", "toto.tar.gz")
        self.assertEqual(os.path.join("/tmp", "blah", "toto.md5"), getMD5FileName(fn))

        fn = "toto.html"
        self.assertEqual("toto.md5", getMD5FileName(fn))
        fn = os.path.join("/tmp", "blah", "toto.html")
        self.assertEqual(os.path.join("/tmp", "blah", "toto.md5"), getMD5FileName(fn))


if __name__ == '__main__':
    unittest.main()



