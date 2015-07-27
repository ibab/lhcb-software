import os
import tempfile
import shutil

from veloview.analysis import (Chi2Test, KolmogorovSmirnovTest)
from veloview.core.analysis_config_wrapper import AnalysisConfigWrapper
from veloview.core.score_manipulation import ERROR_LEVELS
from veloview.core.combiners import BranchCombiner

# aliases
OK = ERROR_LEVELS.OK
WARNING = ERROR_LEVELS.WARNING
ERROR = ERROR_LEVELS.ERROR

from ROOT import TFile
import unittest

class TestStatsComparisonCombiners(unittest.TestCase):

    def setUp(self):
        """Create dictionaries needed by combiners."""

        dirname = os.path.join(os.path.dirname(__file__), 'fixtures')
        orfdata = TFile(
            os.path.abspath(os.path.join(dirname, 'dqm_data.root')),
            'read'
        )
        orfref = TFile(
            os.path.abspath(os.path.join(dirname, 'dqm_ref.root')),
            'read'
        )

        # valid ROOT files
        assert(not orfdata.IsZombie())
        assert(not orfref.IsZombie())

        self.tdir = tempfile.mkdtemp()
        self.rfdata = TFile(os.path.join(self.tdir, 'fdata.root'), 'recreate')
        self.rfref = TFile(os.path.join(self.tdir, 'fref.root'), 'recreate')

        configfile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'analysis_config_test.py'))
        with open(configfile, 'r') as inputFile:
            exec(inputFile.read())
        config = AnalysisConfigWrapper((analysis_config_branches, analysis_config_leaves)) 
        self.mycombiner = config.getTrunk(orfdata.GetName(), orfref.GetName(), r"pedestal\S*")
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
        """Test all combiners recursively with real-life monitoring files"""

        res = self.for_each_combiner(self.mycombiner, [])
        self.maxDiff = None
        self.assertSequenceEqual(sorted(res),\
                [('MasterCombiner', WARNING), ('pedestal_data', ERROR), ('pedestal_quality', OK), ('pedestals', WARNING)])

if __name__ == '__main__':
    unittest.main()
