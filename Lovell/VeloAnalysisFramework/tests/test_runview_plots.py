import os
import unittest
import mock

import ROOT

from veloview.runview import plots


NOMINAL_RUN = 123000
NOMINAL_RUN_FILE = "/tmp/123000/nominal_run_file.root"
REFERENCE_RUN = 124000
REFERENCE_RUN_FILE = "/tmp/124000/reference_run_file.root"
# This file should not be created
NONEXISTENT_RUN_FILE = "/tmp/125000/nonexistent_run_file.root"


def tearDownModule():
    """Delete temporary test files after this module has run."""
    for f in [NOMINAL_RUN_FILE, REFERENCE_RUN_FILE]:
        if os.path.exists(f):
            os.remove(f)


def mocked_run_file_path(run):
    return "/tmp/{0}".format(run)


def mocked_reference_run(plot, run):
    if run == NOMINAL_RUN:
        return REFERENCE_RUN


@mock.patch('veloview.runview.utils.run_file_path', mocked_run_file_path)
class TestRunViewPlots(unittest.TestCase):
    def setUp(self):
        if not os.path.exists(os.path.dirname(NOMINAL_RUN_FILE)):
            os.mkdir(os.path.dirname(NOMINAL_RUN_FILE))
        if not os.path.exists(os.path.dirname(REFERENCE_RUN_FILE)):
            os.mkdir(os.path.dirname(REFERENCE_RUN_FILE))

        # Create two temporary 'run files', one nominal and one reference
        nom = ROOT.TFile(NOMINAL_RUN_FILE, 'recreate')
        ROOT.TH1F('h', 'h_nom', 10, -1, 1).Write()
        nom.Write()
        nom.Close()
        ref = ROOT.TFile(REFERENCE_RUN_FILE, 'recreate')
        ROOT.TH1F('h', 'h_ref', 10, -1, 1).Write()
        ref.Write()
        ref.Close()

    def test_get_run_plot(self):
        """Should return plot from the correct run file."""
        plot = plots.get_run_plot('h', NOMINAL_RUN)
        self.assertEqual(plot['data']['title'], 'h_nom')

    @mock.patch('veloview.runview.utils.reference_run', mocked_reference_run)
    def test_get_run_plot_reference(self):
        """Should return the reference plot if reference is True."""
        plot = plots.get_run_plot('h', NOMINAL_RUN, reference=True)
        self.assertEqual(plot['data']['title'], 'h_ref')

    def test_get_run_plot_no_key(self):
        """Should raise KeyError if plot doesn't exist in run file."""
        with self.assertRaises(KeyError):
            plots.get_run_plot('fake_plot', NOMINAL_RUN)

    def test_get_run_plot_no_file(self):
        """Should raise IOError if run file doesn't exist."""
        with self.assertRaises(IOError):
            # Suppress ROOT error warnings about nonexistent file
            old_level = ROOT.gErrorIgnoreLevel
            ROOT.gErrorIgnoreLevel = ROOT.kFatal
            plots.get_run_plot('fake_plot', 999000)
            ROOT.gErrorIgnoreLevel = old_level


if __name__ == "__main__":
        unittest.main()
