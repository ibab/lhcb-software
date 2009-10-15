#!/usr/bin/env python

from LbUtils.Match import IntMatch, DecMatch, FloatMatch, HexMatch, OctMatch
from LbUtils.Match import KeyWordMatch

import unittest

class TestCase(unittest.TestCase):
    """ Test Case for this module """
    def setUp(self):
        """ setup for the test case """
        unittest.TestCase.setUp(self)
        self.sampletext = """
          example 123 32 65:32. text for different 12345 matching 
          class 11223244 TOTO
          TATA FATAL blah foo ERROR 1.0  FATAL bar 1.02 18. 1e-3 spam
          SPAM .9 1e+3 1.7e2 0344 HFK 0x0FFFFF [44.0,55] jdkd[4]
          dd 0x00001111 44a 3.3b
          """
        self.othersampletext = """ spam SPAm 1.0 -2.e-3 4
          What ever. FATALbar FAtAL ERROR and so on error
          FATAL FATAL ERROR
          """
    def tearDown(self):
        """ teardown for the test case """
        unittest.TestCase.tearDown(self)
    def testKeyWord(self):
        """ Test for the keyword match """
        flt = KeyWordMatch(["FATAL", "ERROR", "bar"])
        testseq = [m for m in flt.getNext(self.sampletext)]
        self.assert_(testseq == ["FATAL", "ERROR", "FATAL", "bar"])
        testseq2 = [m for m in flt.getNext(self.othersampletext)]
        self.assert_(testseq2 == ["FATAL", "bar", "ERROR", 
                              "FATAL", "FATAL", "ERROR"])

    def testFloat(self):
        """ test for the float match """
        flt = FloatMatch(0.1)
        testseq = [m for m in flt.getNext(self.sampletext)]
        print testseq
    def testInt(self):
        """ test for the integer match """
        flt = IntMatch(0.1)
        testseq = [m for m in flt.getNext(self.sampletext)]
        print testseq
    def testDec(self):
        """ test for the decimal integer match """
        flt = DecMatch(0.1)
        testseq = [m for m in flt.getNext(self.sampletext)]
        print testseq
    def testHex(self):
        """ test for the hex integer match """
        flt = HexMatch(0.1)
        testseq = [m for m in flt.getNext(self.sampletext)]
        print testseq
    def testOct(self):
        """ test for the oct integer match """
        flt = OctMatch(0.1)
        testseq = [m for m in flt.getNext(self.sampletext)]
        print testseq
       
if __name__ == '__main__':
    unittest.main()