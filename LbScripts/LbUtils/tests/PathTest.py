from LbUtils.Path import multiPathJoin, multiPathUpdate, cleanPath

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

    def testCleanPath(self):
        p1 = "/dab//blu//"
        self.assertEqual(cleanPath(p1), "/dab//blu")
        self.assertEqual(cleanPath(p1, normalize=True), "/dab/blu")
        p2 = "//dab//blu//"
        self.assertEqual(cleanPath(p2, normalize=True), "//dab/blu")
        p3 = "/dab/blu/df:"
        self.assertEqual(cleanPath(p3, normalize=True), "/dab/blu/df")
        p4 = "/dab/blu/df::"
        self.assertEqual(cleanPath(p4, normalize=True), "/dab/blu/df")
        p5 = "/dab/blu/df://tra:"
        self.assertEqual(cleanPath(p5, normalize=True), "/dab/blu/df://tra")
        p6 = ":/dab/blu/df:://tra:foo:bar/ssd/../bli"
        self.assertEqual(cleanPath(p6, normalize=True), "/dab/blu/df://tra:foo:bar/bli")
        p7 = ":/dab/blu/df:://tra:foo:bar/ssd/../bli"
        self.assertEqual(cleanPath(p7), "/dab/blu/df://tra:foo:bar/ssd/../bli")


if __name__ == '__main__':
    unittest.main()