#!/usr/bin/env python
from LbUtils.Links import showLinks
from LbUtils.Temporary import TempDir

import sys

import unittest

class TestCase(unittest.TestCase):
    def setUp(self):
        unittest.TestCase.setUp(self)
        self._tmpdir = TempDir(suffix="tempdir", prefix="testlinks")

    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testMet1(self):
        pass

if __name__ == '__main__':
    unittest.main()




