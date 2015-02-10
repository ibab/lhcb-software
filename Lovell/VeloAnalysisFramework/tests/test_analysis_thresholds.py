from veloview.core.score_manipulation import ERROR_LEVELS, Score
from veloview.analysis import *
from veloview.utils.rootutils import get_fns
from ROOT import TH1D
import unittest


class TestThresholds(unittest.TestCase):

    def setUp(self):
        self.hdata = TH1D('hdata1', '', 100, -10 ,10)
        self.href = TH1D('href1', '', 100, -10, 10)

    def tearDown(self):
        del self.hdata
        del self.href
        
    def test_floor_both(self):
        """Test floor threshold"""
        self.hdata.FillRandom('pol0', 10000)
        cmpfn = FloorThreshold()
        # each bin has ~100 (=10000/100) entires, +/- 3*sqrt(100)=30
        dqscore = cmpfn.compare(self.hdata, self.href, 100-30)
        self.assertEqual(Score(100), dqscore['score'])
        self.assertEqual(ERROR_LEVELS.OK, dqscore['lvl'])
        dqscore = cmpfn.compare(self.hdata, self.href, 100-10)
        self.assertEqual(Score(0), dqscore['score'])
        self.assertEqual(ERROR_LEVELS.ERROR, dqscore['lvl'])

    def test_ceiling_both(self):
        """Test ceiling threshold"""
        self.hdata.FillRandom('pol0', 10000)
        cmpfn = CeilingThreshold()
        # each bin has ~100 (=10000/100) entires, +/- 3*sqrt(100)=30
        dqscore = cmpfn.compare(self.hdata, self.href, 100+30)
        self.assertEqual(Score(100), dqscore['score'])
        self.assertEqual(ERROR_LEVELS.OK, dqscore['lvl'])
        dqscore = cmpfn.compare(self.hdata, self.href, 100+10)
        self.assertEqual(Score(0), dqscore['score'])
        self.assertEqual(ERROR_LEVELS.ERROR, dqscore['lvl'])

    def test_mean_width_diff_ok(self):
        """Test mean and width check with reference (OK)"""
        fn = get_fns('TMath::Gaus', (0, 3), (-10, 10))
        self.hdata.FillRandom(fn[0], 20000)
        self.href.FillRandom(fn[0], 10000)
        cmpfn = MeanWidthDiffRef()
        # mean = 0, width = 3 (RMS)
        dqscore = cmpfn.compare(self.hdata, self.href, 0.1)
        self.assertTrue(dqscore['score'] > Score(90))
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.OK)

    def test_mean_width_diff_warn(self):
        """Test mean and width check with reference (WARNING)"""
        fn0 = get_fns('TMath::Gaus', (0, 3), (-10, 10))
        fn1 = get_fns('TMath::Gaus', (1, 3), (-10, 10))
        self.hdata.FillRandom(fn0[0], 20000)
        self.href.FillRandom(fn1[0], 10000)
        cmpfn = MeanWidthDiffRef()
        # mean = 0 & 1, width = 3 (RMS)
        dqscore = cmpfn.compare(self.hdata, self.href, 0.1)
        self.assertTrue(dqscore['score'] < Score(40)
                        and dqscore['score'] > Score(20))
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.WARNING)

    def test_mean_width_diff_error(self):
        """Test mean and width check with reference (ERROR)"""
        fn0 = get_fns('TMath::Gaus', (0, 3), (-10, 10))
        fn1 = get_fns('TMath::Gaus', (1, 2), (-10, 10))
        self.hdata.FillRandom(fn0[0], 20000)
        self.href.FillRandom(fn1[0], 10000)
        cmpfn = MeanWidthDiffRef()
        # mean = 0 & 1, width = 3 & 2 (RMS)
        dqscore = cmpfn.compare(self.hdata, self.href, 0.1)
        self.assertTrue(dqscore['score'] < Score(30))
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.ERROR)

    @unittest.skip('Comparison function not implemented')
    def test_absolute_band(self):
        """Test absolute band with reference"""
        pass

    def test_zero_centred_ok(self):
        """Test 0-centred band (OK)"""
        fn = get_fns('TMath::Gaus', (0, 1), (-10, 10))
        self.hdata.FillRandom(fn[0], 20000)
        self.href.FillRandom(fn[0], 10000)
        cmpfn = ZeroCentredBandRef()
        dqscore = cmpfn.compare(self.hdata, self.href, 5)
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.OK)
        dqscore = cmpfn.compare(self.hdata, self.href, None)
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.OK)

    def test_zero_centred_error(self):
        """Test 0-centred band (ERROR)"""
        fn0 = get_fns('TMath::Gaus', (0, 2), (-10, 10))
        fn1 = get_fns('TMath::Gaus', (0, 1), (-10, 10))
        self.hdata.FillRandom(fn0[0], 20000)
        self.href.FillRandom(fn1[0], 10000)
        cmpfn = ZeroCentredBandRef()
        dqscore = cmpfn.compare(self.hdata, self.href, 3)
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.ERROR)
        dqscore = cmpfn.compare(self.hdata, self.href, None)
        self.assertEqual(dqscore['lvl'], ERROR_LEVELS.ERROR)


if __name__ == '__main__':
    unittest.main()
