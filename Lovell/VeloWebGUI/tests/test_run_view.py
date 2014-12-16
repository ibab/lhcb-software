from collections import OrderedDict
import unittest2 as unittest

from lxml.html import fromstring as parse_html

from tests import utils as test_utils
import velo_monitor
from veloview.core import run_view_config


test_utils.set_up_run_list()


def tearDownModule():
    test_utils.tear_down_run_list()


from velo_monitor import run_view


# Run view pages dictionary fixture
RUN_VIEW_PAGES = OrderedDict([
    ('start_page', {
        'title': 'Start Page',
        'plots': [
            # This plot is implicitly *not* sensor dependent
            {
                'title': 'Plot one',
                'name': 'plot_one'
            }
        ]
    }),
    ('other_page', {
        'title': 'Other Page',
        'plots': [
            {
                'title': 'Sensor plot one',
                'name': 'sensor_plot_one_{0:03d}',
                'sensor_dependent': True
            },
            {
                'title': 'Sensor plot two',
                'short': 'Snsr plot 2',
                'name': 'sensor_plot_two_{0}',
                'sensor_dependent': True
            },
            {
                'title': 'General plot',
                'name': 'general_plot'
            }
        ]
    }),
    # This page has no plots, and that's OK
    ('extra_page', {
        'title': 'Extra page'
    })
])

# Default children dictionary fixture, setting a default run_view page
DEFAULT_CHILDREN = {
    'run_view': 'run_view/start_page'
}


def mocked_run_list():
    return test_utils.RUNS


class TestRunView(unittest.TestCase):
    def setUp(self):
        self.app = velo_monitor.create_app()
        self.app.config['TESTING'] = True
        self.app.config['DEFAULT_CHILDREN'] = DEFAULT_CHILDREN
        self.client = self.app.test_client()

        self.old_rvd = run_view_config.run_view_pages
        run_view_config.run_view_pages = RUN_VIEW_PAGES

    def tearDown(self):
        run_view_config.run_view_pages = self.old_rvd

    def get(self, path):
        """Return an pq instance of the lxml parsed document at path."""
        rv = self.client.get(path, follow_redirects=True)
        return parse_html(rv.data)

    def test_sanitise_filter(self):
        """Filter lowercases and replaces non-alphanumeric characters with _."""
        s = "|P~2;-u)o.N4j]bwD^ql=vKK#'N9|Y]]hEAj:8;=9|Jj2[8=/'Y!"
        s_safe = "_P_2__u_o_N4j_bwD_ql_vKK__N9_Y__hEAj_8__9_Jj2_8___Y_".lower()
        self.assertEqual(run_view.sanitise(s), s_safe)

    def test_404_on_invalid_page(self):
        """Should show 404 page on page not in dictionary."""
        doc = self.get('/run_view/0/fake_page')
        header = doc.cssselect('#main > h1')[0].text_content()
        self.assertIn('404', header)
        self.assertIn('Page not found', header)

    def test_display_page(self):
        """Should display current page name as a header and pages in sidebar."""
        doc = self.get('/run_view/0/extra_page')
        header = doc.cssselect('#main > h1')[0].text_content()
        nav = doc.cssselect('.nav-sidebar li')
        self.assertEqual(len(nav), len(RUN_VIEW_PAGES.keys()))
        self.assertEqual(header, 'Extra page')

    def test_default_page(self):
        """Should display default page if none is specified."""
        doc = self.get('/run_view')
        header = doc.cssselect('#main > h1')[0].text_content()

        # Get the default page for the `run_view/` path and that page's title
        page_key = DEFAULT_CHILDREN['run_view'][len('run_view/'):]
        page_title = RUN_VIEW_PAGES[page_key]['title']

        self.assertNotIn('404', header)
        self.assertIn(page_title, header)

    def test_default_run(self):
        """Run number should be set to zero if none is specified."""
        doc = self.get('/run_view')
        number_header = doc.cssselect('.run-number')[0].text_content()
        self.assertIn('Run #{0}'.format(run_view.default_run()), number_header)

    def test_specified_run(self):
        """Run number should be set to that in the URL when given."""
        run = test_utils.RUNS[1]
        doc = self.get('/run_view/{0}'.format(run))
        number_header = doc.cssselect('.run-number')[0].text_content()
        self.assertIn('Run #{0}'.format(run), number_header)

    def test_invalid_run_number_get_params(self):
        """An invalid/empty run GET parameter should redirect to the default."""
        doc1 = self.get('/run_view/?run=')
        doc2 = self.get('/run_view/?run=abc!')
        number_header1 = doc1.cssselect('.run-number')[0].text_content()
        number_header2 = doc2.cssselect('.run-number')[0].text_content()
        self.assertIn('Run #{0}'.format(run_view.default_run()), number_header1)
        self.assertIn('Run #{0}'.format(run_view.default_run()), number_header2)


    def test_invalid_run(self):
        """Invalid run numbers show a warning, redirecting to the default"""
        invalid_run = 123
        doc = self.get('/run_view/{0}'.format(invalid_run))
        number_header = doc.cssselect('.run-number')[0].text_content()
        alert = doc.cssselect('.alert')[0].text_content()
        self.assertIn('Invalid run number "{0}"'.format(invalid_run), alert)
        self.assertIn('Run #{0}'.format(run_view.default_run()), number_header)

    def test_nearby_run_list(self):
        """Nearby run list should show correct runs."""
        current_run_idx = 5
        current_run = test_utils.RUNS[5]
        expected_distance = 3
        doc = self.get('/run_view/{0}'.format(current_run))
        newer_opts = doc.cssselect('optgroup:nth-child(1) option')
        current_opt = doc.cssselect('optgroup:nth-child(2) option')
        older_opts = doc.cssselect('optgroup:nth-child(3) option')
        # First assert that there's the expected number in each optgroup
        self.assertEqual(len(newer_opts), expected_distance)
        self.assertEqual(len(current_opt), 1)
        self.assertEqual(len(older_opts), expected_distance)
        # Then assert that the option's contents is correct
        for i, opt in enumerate(newer_opts):
            self.assertEqual(
                opt.text_content(),
                str(test_utils.RUNS[current_run_idx - expected_distance + i])
            )
        self.assertEqual(current_opt[0].text_content(), str(current_run))
        for i, opt in enumerate(older_opts):
            self.assertEqual(
                opt.text_content(),
                str(test_utils.RUNS[current_run_idx + i + 1])
            )

    def test_run_autocomplete(self):
        """Autocomplete list should be populated with all available runs."""
        num_runs = len(test_utils.RUNS)
        doc = self.get('/run_view/')
        run_options = doc.cssselect('#runs option')
        # First check the number of elements is correct
        self.assertEqual(len(run_options), num_runs)
        # Then check the list is displaying the right values
        for i, opt in enumerate(run_options):
            self.assertEqual(opt.text_content(), str(test_utils.RUNS[i]))

    def test_plot_per_tab(self):
        """Should display one tab per plot, each plot in its own tab pane.

        The short title should be display in the tab, if present, whilst the
        full title should be displayed in the pane header.
        """
        doc = self.get('/run_view/0/other_page')
        tabs = doc.cssselect('.run-view-tab')
        panes = doc.cssselect('.run-view-pane')
        plots = RUN_VIEW_PAGES['other_page']['plots']

        self.assertEqual(len(tabs), len(plots))
        self.assertEqual(len(panes), len(plots))
        for idx, (tab, pane) in enumerate(zip(tabs, panes)):
            title = plots[idx]['title']
            short = plots[idx].get('short', title)
            self.assertEqual(tab.text_content(), short)
            self.assertEqual(pane.cssselect('h1')[0].text_content(), title)

    def test_sensor_selector(self):
        """Sensor selector should be shown only when supported by a plot."""
        doc = self.get('/run_view/0/other_page')
        panes = doc.cssselect('.run-view-pane')
        plots = RUN_VIEW_PAGES['start_page']['plots']
        for idx, (pane, plot) in enumerate(zip(panes, plots)):
            selector = pane.cssselect('.sensor-selector:nth-child({0})'.format(idx))
            expected = int(plot.get('sensor_dependent', False))
            self.assertEqual(len(selector), expected)

    def test_default_sensor_number(self):
        """Sensor number should be set to zero if none is specified."""
        doc = self.get('/run_view/0/other_page')
        # We know the first plot is sensor dependent
        field = doc.cssselect('.run-view-pane:first-child .sensor-selector input')[0]
        self.assertEqual(field.value, '0')

    def test_invalid_sensor_numbers(self):
        """Invalid sensor numbers should be set to zero and an error shown."""
        sensor = 999
        run = run_view.default_run()
        doc = self.get('/run_view/{0}/other_page/{1}'.format(run, sensor))
        alert = doc.cssselect('.alert')[0].text_content()
        self.assertIn('Invalid sensor number "{0}"'.format(sensor), alert)
        field = doc.cssselect('.run-view-pane:first-child .sensor-selector input')[0]
        self.assertEqual(field.value, '0')

    def test_sensor_number_get_url(self):
        """Page should reflect the chosen sensor (with URL structure)."""
        sensor = 32
        doc = self.get('/run_view/0/other_page/{0}'.format(sensor))
        # We know the first plot is sensor dependent
        field = doc.cssselect('.run-view-pane:first-child .sensor-selector input')[0]
        self.assertEqual(field.value, str(sensor))

    def test_sensor_number_get_params(self):
        """Page should reflect the chosen sensor (with GET parameters)."""
        sensor = 12
        doc = self.get('/run_view/?page={0}&sensor={1}'.format('other_page', sensor))
        field = doc.cssselect('.run-view-pane:first-child .sensor-selector input')[0]
        self.assertEqual(field.value, str(sensor))

    def test_invalid_sensor_number_get_params(self):
        """Invalid sensor number should redirect to the default."""
        doc1 = self.get('/run_view/?page={0}&sensor='.format('other_page'))
        doc2 = self.get('/run_view/?page={0}&sensor=abc!'.format('other_page'))
        alert1 = doc1.cssselect('.alert')[0].text_content()
        alert2 = doc2.cssselect('.alert')[0].text_content()
        field1 = doc1.cssselect('.run-view-pane:first-child .sensor-selector input')[0]
        field2 = doc2.cssselect('.run-view-pane:first-child .sensor-selector input')[0]
        self.assertIn('Invalid sensor number ""', alert1)
        self.assertIn('Invalid sensor number "abc!"', alert2)
        self.assertEqual(field1.value, str(0))
        self.assertEqual(field2.value, str(0))
