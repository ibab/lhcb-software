from veloview.core.score_manipulation import ERROR_LEVELS, Score
from veloview.core.interface import *
from ROOT import TH1D
import unittest

class TestInterface(unittest.TestCase):

    def setUp(self):
        self.hdata = TH1D('hdata1', '', 100, -10 ,10)
        self.href = TH1D('href1', '', 100, -10, 10)

    def tearDown(self):
        del self.hdata
        del self.href

    def test_check_hists1(self):
        """Test decorator to check only valid data histogram"""
        class Dummy(ComparisonFunction):
            @classmethod
            @check_hists1
            def compare(cls, data_hist, ref_hist):
                return cls.create_final_dict(Score(100), ERROR_LEVELS.OK)
        result = Dummy.compare(self.hdata, None)
        self.assertEqual(result['lvl'], ERROR_LEVELS.OK)
        result = Dummy.compare(None, None)
        self.assertEqual(result['lvl'], ERROR_LEVELS.ERROR)

    def test_check_hists2(self):
        """Test decorator to check valid data and reference histograms"""
        class Dummy(ComparisonFunction):
            @classmethod
            @check_hists2
            def compare(cls, data_hist, ref_hist):
                return cls.create_final_dict(Score(100), ERROR_LEVELS.OK)
        result = Dummy.compare(self.hdata, self.href)
        self.assertEqual(result['lvl'], ERROR_LEVELS.OK)
        result = Dummy.compare(self.hdata, None)
        self.assertEqual(result['lvl'], ERROR_LEVELS.ERROR)

    def test_check_binning(self):
        """Test decorator to check consistent histogram binning"""
        class Dummy(ComparisonFunction):
            @classmethod
            @check_hists2
            @check_binning
            def compare(cls, data_hist, ref_hist):
                return cls.create_final_dict(Score(100), ERROR_LEVELS.OK)
        # all OK
        result = Dummy.compare(self.hdata, self.href)
        self.assertEqual(result['lvl'], ERROR_LEVELS.OK)
        # bad binning
        bad_href = self.href.Rebin(2, 'hdata_cl')
        result = Dummy.compare(self.hdata, bad_href)
        self.assertEqual(result['lvl'], ERROR_LEVELS.ERROR)
        # bad histogram
        result = Dummy.compare(self.hdata, None)
        self.assertEqual(result['lvl'], ERROR_LEVELS.ERROR)

    def test_base_class(self):
        """Test base class"""
        self.assertRaises(NotImplementedError, ComparisonFunction().compare,
                          self.hdata, self.href, None)

if __name__ == '__main__':
    unittest.main()
