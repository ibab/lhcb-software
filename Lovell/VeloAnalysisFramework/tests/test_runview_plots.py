from collections import OrderedDict
import os
import unittest
import mock
import tempfile
import shutil

import ROOT

from veloview.runview import plots


TMPDIR = tempfile.mkdtemp()
NOMINAL_RUN = 123000
NOMINAL_RUN_FILE = os.path.join(TMPDIR, "123000/nominal_run_file.root")
REFERENCE_RUN = 124000
REFERENCE_RUN_FILE = os.path.join(TMPDIR, "124000/reference_run_file.root")
# This file should not be created
NONEXISTENT_RUN_FILE = os.path.join(TMPDIR, "125000/nonexistent_run_file.root")


def tearDownModule():
    """Delete temporary test files after this module has run."""
    shutil.rmtree(TMPDIR)


def mocked_run_file_path(run):
    return os.path.join(TMPDIR, str(run))


def mocked_reference_run(plot, run):
    if run == NOMINAL_RUN:
        return REFERENCE_RUN


mocked_run_view_config = OrderedDict([
    ('page1', {
        'title': 'Page One',
        'plots': [
            {'title': 'h', 'name': 'h'}
        ]
    })
])


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

    # Need to patch the run view configuration so that the dummy plot 'h' has
    # an entry in some page
    @mock.patch('veloview.runview.plots.run_view_pages', mocked_run_view_config)
    def test_get_run_plot(self):
        """Should return plot from the correct run file."""
        plot = plots.get_run_plot('h', NOMINAL_RUN)
        self.assertEqual(plot['data']['title'], 'h_nom')

    @mock.patch('veloview.runview.utils.reference_run', mocked_reference_run)
    @mock.patch('veloview.runview.plots.run_view_pages', mocked_run_view_config)
    def test_get_run_plot_reference(self):
        """Should return the reference plot if reference is True."""
        reference = plots.get_run_plot('h', NOMINAL_RUN, reference=True)
        self.assertEqual(reference['data']['title'], 'h_ref')

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
