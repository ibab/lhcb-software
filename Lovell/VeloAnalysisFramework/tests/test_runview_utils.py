import os
import tempfile
import shutil

import unittest
import mock

from veloview.config import Config
from veloview.runview import utils
from veloview.utils.rundb import UP, DOWN


RUNS = [123987, 123998]
SENSORS = range(0, 47) + range(64, 107)

UP_RUN = RUNS[0]
DOWN_RUN = RUNS[1]
UP_REF_RUN = 123999
DOWN_REF_RUN = 123998


class MockedRunDB(object):
    def __init__(self):
        pass

    def polarity(self, run):
        if run == UP_RUN:
            return UP
        else:
            return DOWN


class MockedReferenceDatabase(object):
    def __init__(self, path):
        pass

    def reference_run_for_plot(self, run, plot, polarity):
        if run == UP_RUN:
            return UP_REF_RUN
        elif run == DOWN_RUN:
            return DOWN_REF_RUN


class TestRunViewUtils(unittest.TestCase):
    def setUp(self):
        """Use a temporary run directory for the tests."""
        self.old_rdd = Config().run_data_dir
        Config().run_data_dir = tempfile.mkdtemp()
        with open(Config().processed_run_list_path, "w") as f:
            for r in RUNS:
                f.write("{0}\n".format(r))

    def tearDown(self):
        """Remove the test run directory and restore the old paths."""
        shutil.rmtree(Config().run_data_dir)
        Config().run_data_dir = self.old_rdd

    def test_run_list(self):
        """Should return a list of high-low sorted run numbers as integers."""
        self.assertEqual(utils.run_list(), sorted(RUNS, reverse=True))

    def test_valid_run(self):
        """Should return True if run is present in the run number file."""
        for r in RUNS:
            self.assertTrue(utils.valid_run(r))

    def test_invalid_run(self):
        """Should return False if run isn't present in the run number file."""
        probe_run = 123321
        self.assertTrue(probe_run not in RUNS)
        self.assertFalse(utils.valid_run(probe_run))

    def test_sensor_list(self):
        """Should return a number between 0-46 and 64-106 inclusive."""
        self.assertEqual(SENSORS, utils.sensor_list())

    def test_valid_sensor(self):
        """Return True if sensor is a valid sensor number."""
        for s in SENSORS:
            self.assertTrue(utils.valid_sensor(s))

    def test_invalid_sensor(self):
        """Return False if sensor isn't a valid sensor number."""
        for s in range(0, 200):
            if s not in SENSORS:
                self.assertFalse(utils.valid_sensor(s))

    @mock.patch('veloview.runview.utils.ReferenceDatabase', MockedReferenceDatabase)
    @mock.patch('veloview.runview.utils.rundb.RunDB', MockedRunDB)
    def test_reference_run(self):
        """Return the reference run number for the plot and nominal run number."""
        self.assertEqual(UP_REF_RUN, utils.reference_run('ABC', UP_RUN))
        self.assertEqual(DOWN_REF_RUN, utils.reference_run('ABC', DOWN_RUN))

    def test_run_file_path(self):
        """Should return path to ROOT file for the given run."""
        self.assertEqual(
            utils.run_file_path(123987),
            os.path.join(Config().run_data_dir,
                         "100000s", "120000s", "123000s", "123900s", "123987")
        )


if __name__ == "__main__":
        unittest.main()
