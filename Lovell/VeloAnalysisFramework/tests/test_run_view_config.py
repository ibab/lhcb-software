import copy
import unittest
from numbers import Real

from veloview.config import Config
# Copy the dict so we can safely mutate it
run_view_pages = copy.deepcopy(Config().run_view_pages)
# The IV scans dictionary is a special case, currently handled only by the
# offline GUI, so we'll ignore it in these tests
run_view_pages.pop('IV', None)

# Required keys for the page dictionary
REQ_PAGE_KEYS = ['title']

# Value types for a given key within a page dictionary
PAGE_VALUE_TYPES = {
    'title': str,
    'layout': tuple
}

# Key name that holds the plot dictionary within a page dictionary
PLOTS_KEY = 'plots'
# Key name that holds the plot options dictionary within a plot dictionary
OPTIONS_KEY = 'options'

# Required keys for the plot dictionary
REQ_PLOT_KEYS = ['title', 'name']

# Value types for a given key within a plot dictionary
PLOT_VALUE_TYPES = {
    'title': str,
    'name': str,
    'short': str,
    'sensor_dependent': bool,
    'normalise': bool,
    OPTIONS_KEY: dict
}

# Value types for a given option with plot options dictionary
PLOT_OPTIONS_TYPES = {
    'uncertainties': bool,
    'yAxisMinimum': Real,
    'yAxisMaximum': Real,
    'yAxisZeroSuppressed': bool,
    'zAxisMinimum': Real,
    'zAxisMaximum': Real,
    'asPoints': bool,
    'legend': bool,
    'asText': bool,
    'color': str,
    'marker': str
}


class TestRunViewConfig(unittest.TestCase):
    def test_every_top_level_key_has_a_dictionary(self):
        """Every top level key should be a (page) dictionary."""
        for page in run_view_pages.itervalues():
            self.assertIsInstance(page, dict)

    def test_every_page_dictionary_defines_required_keys(self):
        """Each page dictionary must define REQ_PAGE_KEYS."""
        for page in run_view_pages.itervalues():
            for key in REQ_PAGE_KEYS:
                self.assertIn(key, page)

    def test_every_page_dictionary_defines_correct_value_types(self):
        """Each page dictionary values must be of types PLOT_VALUE_TYPES."""
        for page in run_view_pages.itervalues():
            for key, key_type in PAGE_VALUE_TYPES.iteritems():
                if key not in page:
                    continue
                self.assertIsInstance(page[key], key_type)

    def test_layout_key_is_two_tuple(self):
        """The optional `layout` page key must be a two-tuple of integers."""
        for page in run_view_pages.itervalues():
            if 'layout' in page:
                self.assertIsInstance(page['layout'], tuple)
                self.assertEqual(len(page['layout']), 2)
                for el in page['layout']:
                    self.assertIsInstance(el, int)

    def test_every_plot_dictionary_defines_required_keys(self):
        """Each page dictionary must define REQ_PLOT_KEYS."""
        for page in run_view_pages.itervalues():
            for plot in page.get(PLOTS_KEY, []):
                for key in REQ_PLOT_KEYS:
                    self.assertIn(key, plot)

    def test_every_plot_dictionary_defines_correct_value_types(self):
        """Each plot dictionary values must be of types PLOT_VALUE_TYPES."""
        for page in run_view_pages.itervalues():
            for plot in page.get(PLOTS_KEY, []):
                for key, key_type in PLOT_VALUE_TYPES.iteritems():
                    if key not in plot:
                        continue
                    self.assertIsInstance(plot[key], key_type)

    def test_every_plot_options_dictionary_key_is_valid(self):
        """Each plot option must be a key in PLOT_OPTIONS_TYPES."""
        for page in run_view_pages.itervalues():
            for plot in page.get(PLOTS_KEY, []):
                for key in plot.get(OPTIONS_KEY, []):
                    self.assertIn(key, PLOT_OPTIONS_TYPES)

    def test_every_plot_options_dictionary_defines_correct_value_types(self):
        """Each plot option must be of type PLOT_OPTIONS_TYPES."""
        for page in run_view_pages.itervalues():
            for plot in page.get(PLOTS_KEY, []):
                if OPTIONS_KEY not in plot:
                    continue
                for key, key_type in PLOT_OPTIONS_TYPES.iteritems():
                    if key not in plot[OPTIONS_KEY]:
                        continue
                    key_val = plot[OPTIONS_KEY][key]
                    err = 'Expected type {0} for key {1}.{2}, got {3}'.format(
                        key_type, plot['name'], key, type(key_val)
                    )
                    self.assertIsInstance(key_val, key_type, err)


if __name__ == "__main__":
        unittest.main(verbosity=0)
