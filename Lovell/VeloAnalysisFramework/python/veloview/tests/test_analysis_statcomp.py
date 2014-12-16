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

from veloview.analysis import (Chi2Test, KolmogorovSmirnovTest)
from veloview.core import Combiner
from veloview.core.combiner_description_dictionary import (merge_dicts,
                                                           create_leaf_dict_with_path)
from veloview.core.score_manipulation import ERROR_LEVELS
# aliases
OK = ERROR_LEVELS.OK
WARNING = ERROR_LEVELS.WARNING
ERROR = ERROR_LEVELS.ERROR

from ROOT import TFile
import unittest
from veloview.utils.testutils import get_avg_hist, get_avg_trend


def tearDownModule():
    for f in ['/tmp/fdata.root', '/tmp/fref.root']:
        if os.path.exists(f):
            os.remove(f)


class TestStatsComparisonCombiners(unittest.TestCase):

    def setUp(self):
        """Create dictionaries needed by combiners."""

        dirname = os.path.dirname(__file__) + '/'
        orfdata = TFile(os.path.abspath(dirname+'data/dqm_data.root'), 'read')
        orfref = TFile(os.path.abspath(dirname+'data/dqm_ref.root'), 'read')

        # valid ROOT files
        assert(not orfdata.IsZombie())
        assert(not orfref.IsZombie())

        self.rfdata = TFile('/tmp/fdata.root', 'recreate')
        self.rfref = TFile('/tmp/fref.root', 'recreate')

        # histogram names: folder, old name, new name
        hist_recipes = [
            ('Velo/VeloClusterMonitor', '# VELO clusters', 'velo_clusters'),
            ('Velo/VeloClusterMonitor', '# VELO clusters (zoom)', 'velo_clusters_zoom'),
            ('Velo/VeloClusterMonitor', 'Cluster size', 'cluster_size')
        ]

        # comparison fns: fn, options, expected DQ status, histogram
        # to compare
        comp_fns = [
            (KolmogorovSmirnovTest(), '', OK, 0),      # Velo clusters
            (KolmogorovSmirnovTest(), '', OK, 1),
            # (KolmogorovSmirnovTest(), '', ERROR, 2), # cluster size
            (Chi2Test(), '', OK, 0),
            (Chi2Test(), '', OK, 1),
            (Chi2Test(), 'chi2/ndf', OK, 0),
            (Chi2Test(), 'chi2/ndf', OK, 1),
            # (Chi2Test(), '', ERROR, 2),
            # (Chi2Test(), 'chi2/ndf', ERROR, 2)
        ]
        # FIXME: Does not work with cluster size because of really
        # small errors; see commit message for detailed explanation.

        comb_dict = {}
        eval_dict = {}
        self.results = []
        for i in comp_fns:
            # last element is histogram name index
            hname = hist_recipes[i[-1]][-1]
            # should be of the form: *Combiner
            cname = '{}_{}_{}_Combiner'.format(hname, type(i[0]).__name__, i[1])
            comb_dict[cname] = create_leaf_dict_with_path(hname)
            eval_dict[cname] = {'Function': i[0], 'Argument': i[1]}
            # expected result
            self.results.append((cname, i[2]))

        comb_dict = {
            'MasterCombiner': merge_dicts(
                {"weight": 1.0, "minWW": 10, "minWE": 25, "minEW": 1, "minEE": 2},
                comb_dict)
        }
        self.results.append(('MasterCombiner', OK))

        # histograms: make, save, and cleanup
        for recipe in hist_recipes:
            href = orfref.Get('/'.join(recipe[:-1]))
            href.SetName(recipe[-1])
            self.rfref.WriteTObject(href)
            del href
            hdata = orfdata.Get('/'.join(recipe[:-1]))
            hdata.SetName(recipe[-1])
            self.rfdata.WriteTObject(hdata)
            del hdata
        self.rfref.Close()
        self.rfdata.Close()

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
        """Test all combiners recursively with real-life monitoring files"""

        res = self.for_each_combiner(self.mycombiner, [])
        self.maxDiff = None
        self.assertSequenceEqual(sorted(self.results), sorted(res))


if __name__ == '__main__':
    hdr_fmt = '='*5 + '{0:^{width}}' + '='*5
    print hdr_fmt.format('TestStatsComparisonCombiners', width=40)
    from logging import basicConfig, DEBUG, INFO, WARNING
    basicConfig(format='%(levelname)s:%(module)s: %(message)s', level=WARNING)
    unittest.main()
