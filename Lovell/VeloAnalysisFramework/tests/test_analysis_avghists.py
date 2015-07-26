import os
import tempfile
import shutil

from veloview.core.analysis_config_wrapper import AnalysisConfigWrapper
from veloview.core.score_manipulation import ERROR_LEVELS
from veloview.core.combiners import BranchCombiner

# aliases
OK = ERROR_LEVELS.OK
WARNING = ERROR_LEVELS.WARNING
ERROR = ERROR_LEVELS.ERROR

from ROOT import TFile
import unittest

class TestAvgHistCombiners(unittest.TestCase):

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

        configfile = os.path.abspath(os.path.join(os.path.dirname(__file__), 'analysis_config_test.py'))
        with open(configfile, 'r') as inputFile:
            exec(inputFile.read())
        config = AnalysisConfigWrapper((analysis_config_branches, analysis_config_leaves))
        self.mycombiner = config.getTrunk(orfdata.GetName(), orfref.GetName(), r"noise\S*")
        self.mycombiner.evaluate()

    def tearDown(self):
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
            [('MasterCombiner', OK), ('noise', OK), ('noise_phi', OK), ('noise_phi_TELL1_064', OK), ('noise_phi_TELL1_065', OK), ('noise_phi_TELL1_066', OK), ('noise_phi_TELL1_067', OK), ('noise_phi_TELL1_068', OK), ('noise_phi_TELL1_069', OK), ('noise_phi_TELL1_070', OK), ('noise_phi_TELL1_071', OK), ('noise_phi_TELL1_072', OK), ('noise_phi_TELL1_073', OK), ('noise_phi_TELL1_074', OK), ('noise_phi_TELL1_075', OK), ('noise_phi_TELL1_076', OK), ('noise_phi_TELL1_077', OK), ('noise_phi_TELL1_078', OK), ('noise_phi_TELL1_079', OK), ('noise_phi_TELL1_080', OK), ('noise_phi_TELL1_081', OK), ('noise_phi_TELL1_082', OK), ('noise_phi_TELL1_083', OK), ('noise_phi_TELL1_084', OK), ('noise_phi_TELL1_085', OK), ('noise_phi_TELL1_086', OK), ('noise_phi_TELL1_087', OK), ('noise_phi_TELL1_088', OK), ('noise_phi_TELL1_089', OK), ('noise_phi_TELL1_090', OK), ('noise_phi_TELL1_091', OK), ('noise_phi_TELL1_092', OK), ('noise_phi_TELL1_093', OK), ('noise_phi_TELL1_094', OK), ('noise_phi_TELL1_095', OK), ('noise_phi_TELL1_096', OK), ('noise_phi_TELL1_097', OK), ('noise_phi_TELL1_098', OK), ('noise_phi_TELL1_099', OK), ('noise_phi_TELL1_100', OK), ('noise_phi_TELL1_101', OK), ('noise_phi_TELL1_102', OK), ('noise_phi_TELL1_103', OK), ('noise_phi_TELL1_104', OK), ('noise_phi_TELL1_105', OK), ('noise_r', OK), ('noise_r_TELL1_000', OK), ('noise_r_TELL1_001', OK), ('noise_r_TELL1_002', OK), ('noise_r_TELL1_003', OK), ('noise_r_TELL1_004', OK), ('noise_r_TELL1_005', OK), ('noise_r_TELL1_006', OK), ('noise_r_TELL1_007', OK), ('noise_r_TELL1_008', OK), ('noise_r_TELL1_009', OK), ('noise_r_TELL1_010', OK), ('noise_r_TELL1_011', OK), ('noise_r_TELL1_012', OK), ('noise_r_TELL1_013', OK), ('noise_r_TELL1_014', OK), ('noise_r_TELL1_015', OK), ('noise_r_TELL1_016', OK), ('noise_r_TELL1_017', OK), ('noise_r_TELL1_018', OK), ('noise_r_TELL1_019', OK), ('noise_r_TELL1_020', OK), ('noise_r_TELL1_021', OK), ('noise_r_TELL1_022', OK), ('noise_r_TELL1_023', OK), ('noise_r_TELL1_024', OK), ('noise_r_TELL1_025', OK), ('noise_r_TELL1_026', OK), ('noise_r_TELL1_027', OK), ('noise_r_TELL1_028', OK), ('noise_r_TELL1_029', OK), ('noise_r_TELL1_030', OK), ('noise_r_TELL1_031', OK), ('noise_r_TELL1_032', OK), ('noise_r_TELL1_033', OK), ('noise_r_TELL1_034', OK), ('noise_r_TELL1_035', OK), ('noise_r_TELL1_036', OK), ('noise_r_TELL1_037', OK), ('noise_r_TELL1_038', OK), ('noise_r_TELL1_039', OK), ('noise_r_TELL1_040', OK), ('noise_r_TELL1_041', OK)])

if __name__ == '__main__':
    unittest.main()
