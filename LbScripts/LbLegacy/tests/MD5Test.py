from LbLegacy.install_project import calculateMD5, extractMD5SumFromFile
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

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testSameMD5(self):
        self.assertEqual(calculateMD5(self._tmpfile.name), computeMD5Sum(self._tmpfile.name))
        pass

    def textExtraction(self):
        pass

if __name__ == '__main__':
    unittest.main()



