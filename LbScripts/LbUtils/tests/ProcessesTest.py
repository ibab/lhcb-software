from LbUtils.Processes import callCommand

import logging
import unittest

class CallCommandTestCase(unittest.TestCase):

    def setUp(self):
        unittest.TestCase.setUp(self)
        thelog = logging.getLogger()
        thelog.setLevel(logging.DEBUG)
        thelog.addHandler(logging.StreamHandler())
    def tearDown(self):
        unittest.TestCase.tearDown(self)

    def testUnknowCommand(self):
        self.assertRaises(OSError, callCommand, "blah")

    def testUnknowCommandWithReport(self):
        self.assertRaises(OSError, callCommand, "blah", report_failure=True)

    def testGoodCommand(self):
        out, err, rc = callCommand("echo", "titi")
        self.assertEqual(out,"titi\n")
        self.assertEqual(err,"")
        self.assertEqual(rc,0)

    def testGoodCommandWithReport(self):
        out, err, rc = callCommand("echo", "titi", report_failure=True)
        self.assertEqual(out,"titi\n")
        self.assertEqual(err,"")
        self.assertEqual(rc,0)

    def testBadCommand(self):
        out, err, rc = callCommand("test")
        self.assertNotEqual(rc,0)

    def testBadCommandWithReport(self):
        out, err, rc = callCommand("test", report_failure=True)
        self.assertNotEqual(rc,0)

if __name__ == '__main__':
    unittest.main()