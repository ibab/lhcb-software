#!/usr/bin/env python
# coding=utf-8

import os

if __name__ == '__main__':
    import sys

    # fiddle with sys.path so that package is importable
    if __file__.startswith('/'):
        sys.path.insert(0, os.path.join('/', *__file__.split('/')[:-3]))
    else:
        __path_to_script__ = __file__.split('/')[:-1]  # test directory
        __path_to_script__ += ['..', '..'] # package directory parent
        sys.path.insert(0, os.path.join(os.getcwd(), *__path_to_script__))

from veloview.analysis import (Median, Mean, Variance, MPV, Landau)
from veloview.analysis.trends import check_hist
from veloview.utils.rootutils import get_fns
from ROOT import TH1D
import unittest


class TestTrends(unittest.TestCase):

    def setUp(self):
        self.hist = TH1D('hist', '', 100, -10, 10)
        fn = get_fns('TMath::Gaus', (0, 3), (-10, 10))
        self.hist.FillRandom(fn[0], 10000)
        self.delta = 0.05       # 5%

    def tearDown(self):
        del self.hist

    def test_check_hist(self):
        @check_hist
        def dummy(hist): return hist
        self.assertRaises(ValueError, dummy, None)

    def test_Mean(self):
        res = Mean(self.hist)
        self.assertAlmostEqual(res['mean'], 0, delta = self.delta)

    def test_Median(self):
        res = Median(self.hist)
        self.assertEqual(res['median'], NotImplemented)

    def test_Variance(self):
        res = Variance(self.hist)
        self.assertAlmostEqual(res['var'], 3, delta = self.delta)

    def test_MPV(self):
        res = MPV(self.hist)
        self.assertEqual(res['mpv'], NotImplemented)

    def test_Landau(self):
        res = Landau(self.hist)
        self.assertEqual(res['landau'], NotImplemented)


if __name__ == '__main__':
    hdr_fmt = '='*5 + '{0:^{width}}' + '='*5
    print hdr_fmt.format('TestTrends', width=40)
    unittest.main()
