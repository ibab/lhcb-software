import unittest2 as unittest

import velo_monitor


class TestPages(unittest.TestCase):
    def setUp(self):
        self.app = velo_monitor.create_app()
        self.client = self.app.test_client()

    def test_app_should_instantiate_with_resolvers(self):
        """App should instantiate with the correct job resolvers."""
        resolvers = [
            velo_monitor.job_resolvers.run_view_resolver
        ]
        self.assertEqual(len(self.app.job_resolvers()), 1)
        self.assertItemsEqual(self.app.job_resolvers(), resolvers)


if __name__ == '__main__':
    unittest.main()
