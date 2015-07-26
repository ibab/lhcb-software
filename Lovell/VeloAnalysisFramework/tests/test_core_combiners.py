import os
import tempfile
import shutil

from veloview.core.analysis_config_wrapper import AnalysisConfigWrapper
from veloview.core.score_manipulation import Score, ERROR_LEVELS

# aliases
OK = ERROR_LEVELS.OK
WARNING = ERROR_LEVELS.WARNING
ERROR = ERROR_LEVELS.ERROR

from ROOT import TFile
import unittest
from veloview.utils.testutils import get_avg_hist, get_avg_trend


class TestCombiners(unittest.TestCase):

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

        hist_recipes = [
            (get_avg_trend, 'Vetra/NoiseMon/ADCCMSuppressed',
             ('RMSNoise_vs_ChipChannel', 'AvgRMSNoise_trend')),
            (get_avg_hist, 'Vetra/NoiseMon/ADCCMSuppressed',
             ('RMSNoise_vs_ChipChannel', 'AvgRMSNoise_all')),
            (get_avg_hist, 'Vetra/NoiseMon/ADCCMSuppressed',
             ('RMSNoise_vs_ChipChannel', 'AvgRMSNoise_R', 'r')),
            (get_avg_hist, 'Vetra/NoiseMon/ADCCMSuppressed',
             ('RMSNoise_vs_ChipChannel', 'AvgRMSNoise_Phi', 'p')),
            # (get_avg_hist, 'Vetra/VeloPedestalSubtractorMoni',
            #  ('Ped_Sub_ADCs_Profile', 'Ped_Sub_ADCs_all'))
        ]
        # histograms: make, save, and cleanup
        for recipe in hist_recipes:
            href = recipe[0](orfref.GetDirectory(recipe[1]), *recipe[2])
            self.rfref.WriteTObject(href)
            del href
            hdata = recipe[0](orfdata.GetDirectory(recipe[1]), *recipe[2])
            self.rfdata.WriteTObject(hdata)
            del hdata
        self.rfref.Close()
        self.rfdata.Close()

        configfile = os.path.join(os.path.dirname(__file__), 'analysis_config_test.py')
        with open(configfile, 'r') as inputFile:
            exec(inputFile.read())
        config = AnalysisConfigWrapper((analysis_config_branches, analysis_config_leaves))
        self.mycombiner = config.getTrunk(orfdata.GetName(), orfref.GetName())

        # Results to compare against
        self.results = {"score": Score(31.106987921869603), "lvl": ERROR}

    def tearDown(self):
        shutil.rmtree(self.tdir)
        del self.mycombiner

    def test_combiners(self):
        """Test all combiners recursively with real-life monitoring files"""

        print self.mycombiner
        self.mycombiner.evaluate()
        self.mycombiner.write_to_grf()

        self.assertEqual(self.mycombiner.results["lvl"], self.results["lvl"])
        #self.assertAlmostEqual(self.mycombiner.results["score"].value, self.results["score"].value)

if __name__ == '__main__':
    unittest.main()
