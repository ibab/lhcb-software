#!/usr/bin/env python

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

from veloview.analysis import (FloorThreshold, CeilingThreshold,
                               MeanWidthDiffRef, AbsoluteBandRef,
                               ZeroCentredBandRef)
from veloview.core import Combiner
from veloview.utils.rootutils import get_simple_fns, get_fns
from veloview.core.combiner_description_dictionary import (STANDARD_BRANCH_DICT,
                                                           STANDARD_LEAF_DICT,
                                                           merge_dicts,
                                                           create_leaf_dict_with_path)
from veloview.core.score_manipulation import ERROR_LEVELS
# aliases
OK = ERROR_LEVELS.OK
WARNING = ERROR_LEVELS.WARNING
ERROR = ERROR_LEVELS.ERROR

from ROOT import TFile, TH1D, TF1
from math import sqrt
import unittest


def tearDownModule():
    for f in ['/tmp/fdata.root', '/tmp/fref.root']:
        if os.path.exists(f):
            os.remove(f)


class TestCombinersWToys(unittest.TestCase):

    def setUp(self):
        """Create dictionaries and ROOT files needed for testing."""

        self.rfdata = TFile('/tmp/fdata.root', 'recreate')
        self.rfref = TFile('/tmp/fref.root', 'recreate')

        fns = {}
        fns['pol0'] = get_simple_fns('5', (-10, 10))
        fns['gauss01'] = get_fns('TMath::Gaus', (0, 1), (-10, 10))
        fns['gauss02'] = get_fns('TMath::Gaus', (0, 2), (-10, 10))
        fns['gauss03'] = get_fns('TMath::Gaus', (0, 3), (-10, 10))
        fns['gauss12'] = get_fns('TMath::Gaus', (1, 2), (-10, 10))
        fns['landau03'] = get_fns('TMath::Landau', (0, 3), (-10, 10))

        N = 10000
        rho = N/100.0
        comp_fns = []
        comp_fns.append((FloorThreshold(), rho - 5*sqrt(rho), 'pol0', 'pol0', OK, 'FT1'))    # 5 sigma
        comp_fns.append((FloorThreshold(), rho - sqrt(rho), 'pol0', 'pol0', ERROR, 'FT2'))   # 1 sigma
        comp_fns.append((CeilingThreshold(), rho + 5*sqrt(rho), 'pol0', 'pol0', OK, 'CT1'))  # 5 sigma
        comp_fns.append((CeilingThreshold(), rho + sqrt(rho), 'pol0', 'pol0', ERROR, 'CT2')) # 1 sigma
        comp_fns.append((MeanWidthDiffRef(), 0.1, 'gauss03', 'gauss03', OK, 'MWDR1'))
        comp_fns.append((MeanWidthDiffRef(), 0.1, 'gauss03', 'gauss12', ERROR, 'MWDR2'))
        comp_fns.append((ZeroCentredBandRef(), 5, 'gauss01', 'gauss01', OK, 'ZCBR1'))        # 5 sigma
        comp_fns.append((ZeroCentredBandRef(), 3, 'gauss01', 'gauss02', ERROR, 'ZCBR2'))     # 3 sigma
        # comp_fns.append( , , 'landau03', 'landau03', )

        comb_dict = {}
        eval_dict = {}
        self.results = []
        for i in comp_fns:
            name = '{}_{}_{}'.format(i[5], i[2], id(i)) # unique form: <name>_<fn>_<id>
            hname = 'hist_' + name
            cname = name + '_Combiner' # should be of the form: *Combiner
            comb_dict[cname] = create_leaf_dict_with_path(hname)
            eval_dict[cname] = {'Function': i[0], 'Argument': i[1]}

            href = TH1D(hname, '', 100, -10 ,10)
            href.FillRandom(fns[i[2]][0], N)
            self.rfref.WriteTObject(href)
            del href
            hdata = TH1D(hname, '', 100, -10 ,10)
            hdata.FillRandom(fns[i[3]][0], N)
            self.rfdata.WriteTObject(hdata)
            del hdata

            self.results.append((cname, i[4]))

        comb_dict = {
            'MasterCombiner': merge_dicts(
                {"weight": 1.0, "minWW": 10, "minWE": 25, "minEW": 1, "minEE": 2},
                comb_dict)
        }
        self.results.append(('MasterCombiner', ERROR))

        # close files cleanly
        self.rfdata.Close()
        self.rfref.Close()

        self.mycombiner = Combiner(comb_dict, eval_dict,
                                   self.rfdata.GetName(), self.rfref.GetName())
        self.mycombiner.evaluate()

    def tearDown(self):
        del self.rfdata
        del self.rfref
        del self.mycombiner

    def for_each_combiner(self, combiner, res):
        """Call test on each node/leaf of combiner."""
        if combiner.children:
            for child_combiner in combiner.children:
                self.for_each_combiner(child_combiner, res)
        # append my result
        res.append((combiner.name, combiner.results['lvl']))
        return res

    def test_combiners(self):
        """Test all combiners recursively with toy monitoring files"""
        res = self.for_each_combiner(self.mycombiner, [])
        self.maxDiff = None
        self.assertSequenceEqual(sorted(self.results), sorted(res))


if __name__ == '__main__':
    hdr_fmt = '='*5 + '{0:^{width}}' + '='*5
    print hdr_fmt.format('TestCombinersWToys', width=40)
    from logging import basicConfig, DEBUG, INFO, WARNING
    basicConfig(format='%(levelname)s:%(module)s: %(message)s', level=WARNING)
    unittest.main()
