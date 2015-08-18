import os
import tempfile
import shutil

from veloview.analysis import (FloorThreshold, CeilingThreshold,
                               MeanWidthDiffRef, AbsoluteBandRef,
                               ZeroCentredBandRef)
from veloview.core.score_manipulation import ERROR_LEVELS
from veloview.core.combiners import (BranchCombiner, RootCombiner)
from veloview.utils.rootutils import (get_fns, get_simple_fns)

# aliases
OK = ERROR_LEVELS.OK
WARNING = ERROR_LEVELS.WARNING
ERROR = ERROR_LEVELS.ERROR

from ROOT import TFile, TH1D, TMath
import unittest

class TestCombinersWToys(unittest.TestCase):

    def setUp(self):
        """Create dictionaries and ROOT files needed for testing."""

        self.tdir = tempfile.mkdtemp()
        self.rfdata = TFile(os.path.join(self.tdir, 'fdata.root'), 'recreate')
        self.rfref = TFile(os.path.join(self.tdir, 'fref.root'), 'recreate')

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
        comp_fns.append((FloorThreshold(), rho - 5*TMath.Sqrt(rho), 'pol0', 'pol0', OK, 'FT1'))    # 5 sigma
        comp_fns.append((FloorThreshold(), rho - TMath.Sqrt(rho), 'pol0', 'pol0', ERROR, 'FT2'))   # 1 sigma
        comp_fns.append((CeilingThreshold(), rho + 5*TMath.Sqrt(rho), 'pol0', 'pol0', OK, 'CT1'))  # 5 sigma
        comp_fns.append((CeilingThreshold(), rho + TMath.Sqrt(rho), 'pol0', 'pol0', ERROR, 'CT2')) # 1 sigma
        comp_fns.append((MeanWidthDiffRef(), 0.1, 'gauss03', 'gauss03', OK, 'MWDR1'))
        comp_fns.append((MeanWidthDiffRef(), 0.1, 'gauss03', 'gauss12', ERROR, 'MWDR2'))
        comp_fns.append((ZeroCentredBandRef(), 5, 'gauss01', 'gauss01', OK, 'ZCBR1'))        # 5 sigma
        comp_fns.append((ZeroCentredBandRef(), 3, 'gauss01', 'gauss02', ERROR, 'ZCBR2'))     # 3 sigma
        # comp_fns.append( , , 'landau03', 'landau03', )

        comb_dict = {}
        eval_dict = {}
        childNames = []
        self.results = []
        for i in comp_fns:
            name = '{}_{}_{}'.format(i[5], i[2], id(i)) # unique form: <name>_<fn>_<id>
            hname = 'hist_' + name
            cname = name + '_Combiner' # should be of the form: *Combiner
            childNames.append(cname)
            comb_dict[cname] = {"weight": 1.0, "maxError": 50, "maxWarning": 80, "path": hname}
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

        comb_dict['MasterCombiner'] = {"weight": 1.0, "minWW": 10, "minWE": 25, "minEW": 1, "minEE": 2, "children": childNames}
        self.results.append(('MasterCombiner', ERROR))

        # close files cleanly
        self.rfdata.Close()
        self.rfref.Close()

        self.mycombiner = RootCombiner(comb_dict, eval_dict, self.rfdata.GetName(), self.rfref.GetName(), 0)
        self.mycombiner.evaluate()

    def tearDown(self):
        shutil.rmtree(self.tdir)
        del self.mycombiner

    def for_each_combiner(self, combiner, res):
        """Call test on each node/leaf of combiner."""
        if isinstance(combiner, BranchCombiner):
            for child_combiner in combiner.children:
                self.for_each_combiner(child_combiner, res)
        # append my result
        res.append((combiner.getName(), combiner.getLevel()))
        return res

    def test_combiners(self):
        """Test all combiners recursively with toy monitoring files"""
        res = self.for_each_combiner(self.mycombiner, [])
        self.maxDiff = None
        self.assertSequenceEqual(sorted(self.results), sorted(res))


if __name__ == '__main__':
    unittest.main()
