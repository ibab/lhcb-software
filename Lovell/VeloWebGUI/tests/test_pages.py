import unittest2 as unittest

from tests import utils as test_utils
import velo_monitor


def setUpModule():
    test_utils.set_up_run_list()


def tearDownModule():
    test_utils.tear_down_run_list()


class TestPages(unittest.TestCase):
    def setUp(self):
        self.app = velo_monitor.create_app()
        self.client = self.app.test_client()

    def test_index_page(self):
        """The index page should show the app's info and the correct route."""
        rv = self.client.get('/')
        assert self.app.config['APP_NAME'] in rv.data
        assert self.app.config['APP_DESCRIPTION'] in rv.data


if __name__ == '__main__':
    unittest.main()
