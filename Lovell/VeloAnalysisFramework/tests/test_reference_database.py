import unittest
import tempfile
import os
from operator import itemgetter

from veloview.runview import reference_database


# Runs as boundary run mapped to (magnet up, magnet down) reference runs
RUNS = {
    123: (126, 128),
    131: (143, 150),
    151: (152, 155)
}

# Plot
PLOTS = [
    {
        'plot_name': 'plot_A',
        'magnet_up_run_id': 124,
        'magnet_down_run_id': 128,
        'boundary_run_id': RUNS.keys()[0]
    },
    {
        'plot_name': 'plot_B',
        'magnet_up_run_id': 125,
        'magnet_down_run_id': 129,
        'boundary_run_id': RUNS.keys()[0]
    },
    {
        'plot_name': 'plot_B',
        'magnet_up_run_id': 153,
        'magnet_down_run_id': 157,
        'boundary_run_id': RUNS.keys()[-1]
    }
]


class TestReferenceDatabase(unittest.TestCase):
    def setUp(self):
        # Create a database
        self.path = os.path.join(tempfile.mkdtemp(), 'reference_db.sqlite')
        self.db = reference_database.ReferenceDatabase(self.path)

        # Fill it with dummy data
        for boundary, references in RUNS.iteritems():
            self.db.conn.execute((
                'INSERT INTO run_boundaries('
                'run_id, magnet_up_run_id, magnet_down_run_id'
                ') VALUES (?, ?, ?)'
            ), (boundary, references[0], references[1]))
        for plot in PLOTS:
            self.db.conn.execute((
                'INSERT INTO boundary_plots('
                'plot_name, magnet_up_run_id, magnet_down_run_id, '
                'boundary_run_id'
                ') VALUES (?, ?, ?, ?)'
            ), (
                plot['plot_name'],
                plot['magnet_up_run_id'],
                plot['magnet_down_run_id'],
                plot['boundary_run_id']
            ))

    def tearDown(self):
        if os.path.exists(self.path):
            os.remove(self.path)

    def _has_table(self, name):
        """Return True if self.db has the named table."""
        return self.db.conn.execute((
            'SELECT name FROM sqlite_master '
            "WHERE name=? AND type='table' "
            'LIMIT 1'
        ), (name,)).fetchone() is not None

    def _run_boundaries(self):
        """Return a list of run boundaries and their reference runs."""
        # Explicit specfiy columns to guarantee column ordering
        res = self.db.conn.execute((
            'SELECT run_id, magnet_up_run_id, magnet_down_run_id '
            'FROM run_boundaries'
        ))
        return res.fetchall()

    def _plots(self):
        """Return a list of plots and their reference and boundary runs."""
        # Explicit specfiy columns to guarantee column ordering
        res = self.db.conn.execute((
            'SELECT plot_name, magnet_up_run_id, magnet_down_run_id, '
            'boundary_run_id '
            'FROM boundary_plots'
        ))
        return res.fetchall()

    def test_tables_loaded_on_creation(self):
        """Tables defined in TABLE_SCHEMAS should be loaded correctly."""
        for table in reference_database.TABLE_SCHEMAS:
            self.assertTrue(self._has_table(table))

    def test_run_boundaries(self):
        """Should return list of all run boundaries."""
        self.assertListEqual(RUNS.keys(), self.db.run_boundaries())

    def test_adding_run_boundary(self):
        """Can add a valid run boundary with reference runs."""
        self.assertEqual(len(self._run_boundaries()), len(RUNS))
        self.db.add_run_boundary(120, 122, 123)
        self.assertEqual(len(self._run_boundaries()), len(RUNS) + 1)

    def test_adding_existing_run_boundary(self):
        """Should throw an exception is an existing boundary is added."""
        self.assertEqual(len(self._run_boundaries()), len(RUNS))
        with self.assertRaises(reference_database.InvalidBoundary):
            self.db.add_run_boundary(RUNS.keys()[0], 1, 2)
        # Nothing should have changed
        self.assertEqual(len(self._run_boundaries()), len(RUNS))

    def test_adding_invalid_run_boundary_type(self):
        """Should throw an exception if a non-integer run boundary is added."""
        self.assertEqual(len(self._run_boundaries()), len(RUNS))

        # Test invalid negative run_ids
        with self.assertRaises(reference_database.InvalidBoundary):
            self.db.add_run_boundary(-1, 1, 2)
        with self.assertRaises(reference_database.InvalidBoundary):
            self.db.add_run_boundary(999, -1, 2)
        with self.assertRaises(reference_database.InvalidBoundary):
            self.db.add_run_boundary(999, 1, -1)

        # Nothing should have changed
        self.assertEqual(len(self._run_boundaries()), len(RUNS))

    def test_removing_run_boundary(self):
        """Can remove an existing run boundary."""
        self.db.remove_run_boundary(RUNS.keys()[0])
        self.assertEqual(len(self._run_boundaries()), len(RUNS) - 1)

    def test_removing_nonexistent_run_boundary(self):
        """Can remove a non-existing run boundary without drama."""
        # Check the run isn't in the dummy data
        run = 999
        self.assertNotIn(run, RUNS)
        self.db.remove_run_boundary(run)
        # Nothing should have changed
        self.assertEqual(len(self._run_boundaries()), len(RUNS))

    def test_updating_boundary(self):
        """Can edit boundary reference runs."""
        run, old_up, old_down = self._run_boundaries()[0]
        new_up = 998
        new_down = 999

        # Test single updating (magnet up)
        self.db.update_run_boundary(run, up_run=new_up)
        self.assertEqual(new_up, self._run_boundaries()[0][1])

        # Test single updating (magnet down)
        self.db.update_run_boundary(run, down_run=new_down)
        self.assertEqual(new_down, self._run_boundaries()[0][2])

        # Test combined updating (magnet up and magnet down)
        self.db.update_run_boundary(run, up_run=old_up, down_run=old_down)
        self.assertEqual(old_up, self._run_boundaries()[0][1])
        self.assertEqual(old_down, self._run_boundaries()[0][2])

    def test_surrounding_boundaries(self):
        """Should return correct surrounding boundaries."""
        sorted_runs = sorted(RUNS.keys())
        lowest, highest = sorted_runs[0], sorted_runs[-1]
        run_range = range(lowest, highest)

        # For runs below the lowest run boundary, should return (0, lowest)
        self.assertListEqual([0, lowest],
                             self.db.surrounding_boundaries(lowest - 1))

        # Loop over every run in between the lowest and highest, compute their
        # nearest run boundaries, and check the DB gets the same correct result
        for run in run_range:
            for idx, boundary in enumerate(sorted_runs):
                if boundary <= run < sorted_runs[idx + 1]:
                    break
            lo, hi = self.db.surrounding_boundaries(run)
            self.assertEqual(lo, sorted_runs[idx])
            self.assertEqual(hi, sorted_runs[idx + 1])

        # For runs below the lowest run boundary, should return (highest, inf)
        self.assertListEqual([highest, float('inf')],
                             self.db.surrounding_boundaries(highest + 1))

    def test_reference_run_resolution(self):
        """Should resolve the correct reference run for a given run."""
        sorted_runs = sorted(self._run_boundaries(), key=itemgetter(0))
        lowest, highest = sorted_runs[0], sorted_runs[-1]
        run_range = range(lowest[0], highest[0])

        # For runs below the lowest run boundary, should return 0
        self.assertEqual(0, self.db.reference_run(lowest[0] - 1, up=True))
        self.assertEqual(0, self.db.reference_run(lowest[0] - 1, down=True))

        # Loop over every run in between the lowest and highest, compute their
        # reference run, and check the DB gets the same correct result
        for run in run_range:
            for idx, (boundary, _, _) in enumerate(sorted_runs):
                if boundary <= run < sorted_runs[idx + 1][0]:
                    break
            self.assertEqual(sorted_runs[idx][1],
                             self.db.reference_run(run, up=True))
            self.assertEqual(sorted_runs[idx][2],
                             self.db.reference_run(run, down=True))

        # For runs below the lowest run boundary, should return (highest, inf)
        self.assertEqual(highest[1],
                         self.db.reference_run(highest[0] + 1, up=True))
        self.assertEqual(highest[2],
                         self.db.reference_run(highest[0] + 1, down=True))

    def test_ambiguous_reference_run_resolution(self):
        """Should raise a value error if up and down are True."""
        with self.assertRaises(ValueError):
            self.db.reference_run(RUNS.keys()[0], up=True, down=True)
        with self.assertRaises(ValueError):
            self.db.reference_run(RUNS.keys()[0], up=False, down=False)

    def test_plots(self):
        """Should return 3-tuple of plot name, up run, and down run."""
        for boundary in RUNS:
            def on_boundary(plot):
                return plot['boundary_run_id'] == boundary
            plots = [
                (
                    p['plot_name'],
                    p['magnet_up_run_id'],
                    p['magnet_down_run_id']
                )
                for p in filter(on_boundary, PLOTS)
            ]
            self.assertListEqual(plots, self.db.boundary_plots(boundary))

    def test_adding_plot(self):
        """Should add a valid plot."""
        self.assertEqual(len(self._plots()), len(PLOTS))
        self.db.add_boundary_plot(RUNS.keys()[0], 'plot', 1, 2)
        self.assertEqual(len(self._plots()), len(PLOTS) + 1)

    def test_adding_plot_on_nonexistent_boundary(self):
        """Should raise exception on nonexistent boundary."""
        run = 999
        self.assertNotIn(run, [b[0] for b in self._run_boundaries()])
        with self.assertRaises(reference_database.InvalidPlot):
            self.db.add_boundary_plot(run, 'fake_plot', 1, 2)

    def test_adding_duplicate_boundary_plot(self):
        """Should raise exception if boundary plot exists."""
        plot = PLOTS[0]
        with self.assertRaises(reference_database.InvalidPlot):
            self.db.add_boundary_plot(
                plot['boundary_run_id'], plot['plot_name'], 1, 2)

    def test_removing_boundary_plot(self):
        """Can remove an existing boundary plot."""
        p = PLOTS[0]
        self.db.remove_boundary_plot(p['boundary_run_id'], p['plot_name'])
        self.assertEqual(len(self._plots()), len(PLOTS) - 1)

    def test_removing_nonexistent_boundary_plot(self):
        """Can remove a non-existing boundary plot without drama."""
        # Check the plot isn't in the dummy data
        boundary = 999
        name = 'fake_plot'
        for plot in PLOTS:
            self.assertTrue(boundary != plot['boundary_run_id'])
            self.assertTrue(name != plot['plot_name'])
        self.db.remove_boundary_plot(boundary, name)
        self.assertEqual(len(self._plots()), len(PLOTS))

    def test_updating_boundary_plot(self):
        """Can edit boundary plot reference runs."""
        plot, old_up, old_down, run = self._plots()[0]
        new_up = 998
        new_down = 999

        # Test single updating (magnet up)
        self.db.update_boundary_plot(run, plot, up_run=new_up)
        self.assertEqual(new_up, self._plots()[0][1])

        # Test single updating (magnet down)
        self.db.update_boundary_plot(run, plot, down_run=new_down)
        self.assertEqual(new_down, self._plots()[0][2])

        # Test combined updating (magnet up and magnet down)
        self.db.update_boundary_plot(
            run, plot, up_run=old_up, down_run=old_down
        )
        self.assertEqual(old_up, self._plots()[0][1])
        self.assertEqual(old_down, self._plots()[0][2])

    def test_reference_run_for_plot_resolution(self):
        """Should return correct reference run for given run and plot."""
        # Shortcut
        plot_ref = self.db.reference_run_for_plot

        sorted_runs = sorted(self._run_boundaries(), key=itemgetter(0))
        lowest, highest = sorted_runs[0], sorted_runs[-1]
        run_range = range(lowest[0], highest[0])

        # For runs below the lowest run boundary, should return 0 regardless
        # of the plot name because there's no boundary to define plots on
        self.assertEqual(0, plot_ref(lowest[0] - 1, 'fake_plot', up=True))
        self.assertEqual(0, plot_ref(lowest[0] - 1, 'fake_plot', down=True))

        # Loop over every run in between the lowest and highest, compute their
        # boundary run, find all plots defined on that boundary, and check the
        # DB correctly return each plot's reference run
        for run in run_range:
            for idx, (boundary, _, _) in enumerate(sorted_runs):
                if boundary <= run < sorted_runs[idx + 1][0]:
                    break
            plots = filter(lambda p: p['boundary_run_id'] == boundary, PLOTS)
            for plot in plots:
                self.assertEqual(plot['magnet_up_run_id'],
                                 plot_ref(run, plot['plot_name'], up=True))
                self.assertEqual(plot['magnet_down_run_id'],
                                 plot_ref(run, plot['plot_name'], down=True))
            # Check a plot not defined on the boundary
            self.assertEqual(sorted_runs[idx][1],
                             plot_ref(run, 'fake_plot', up=True))
            self.assertEqual(sorted_runs[idx][2],
                             plot_ref(run, 'fake_plot', down=True))

    def test_ambiguous_reference_run_for_plot_resolution(self):
        """Should raise a value error if up and down are True."""
        with self.assertRaises(ValueError):
            self.db.reference_run_for_plot(
                RUNS.keys()[0], PLOTS[0]['plot_name'], up=True, down=True)
        with self.assertRaises(ValueError):
            self.db.reference_run_for_plot(
                RUNS.keys()[0], PLOTS[0]['plot_name'], up=False, down=False)


if __name__ == "__main__":
        unittest.main(verbosity=0)
