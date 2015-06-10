import unittest

from veloview.config import DEFAULTS, Config


class TestConfig(unittest.TestCase):
    # Define a static key and value for us to play around with while testing
    test_key = DEFAULTS.iterkeys().next()
    test_value = 'MyTestValue'

    def setUp(self):
        """Check that the value of test configuration key isn't the test value.

        If this isn't true, methods that test changing configuration values
        will always pass.
        """
        self.assertNotEqual(getattr(Config(), self.test_key), self.test_value)

    def tearDown(self):
        """Reset the configuration back to the DEFAULTS."""
        c = Config()
        for key in DEFAULTS:
            setattr(c, key, DEFAULTS[key])

    def test_defaults_defined(self):
        """DEFAULTS should be a non-empty dictionary."""
        self.assertIsNotNone(DEFAULTS)
        self.assertIsInstance(DEFAULTS, dict)
        self.assertGreater(len(DEFAULTS), 0)

    def test_default_values(self):
        """All values in DEFAULTS should not be None."""
        for value in DEFAULTS.itervalues():
            self.assertIsNotNone(value)

    def test_can_instantiate(self):
        """Config should instantiate within errors or warnings."""
        c = Config()
        # Duck typing: should be able to access a key of DEFAULT as a property
        try:
            self.assertIsNotNone(getattr(c, self.test_key))
        except AttributeError:
            self.fail('Could not access DEFAULTS members on Config()')

    def test_can_set_property_value_on_instantiation(self):
        """Should be able to override DEFAULTS with keyword arguments."""
        # Get some key to test with
        key = self.test_key
        value = 'MyTestValue'
        # Ensure the test value isn't the default value (sanity check)
        self.assertNotEqual(value, DEFAULTS[key])
        # Instantiate the Config object with the custom property value
        c = Config(**{key: value})
        self.assertEqual(getattr(c, key), value)

    def test_all_default_properties_are_available(self):
        """All keys on DEFAULTS should be properties on Config instances."""
        c = Config()
        for key in DEFAULTS:
            try:
                getattr(c, key)
            except:
                self.fail('Could not retrieve `{0}` from Config'.format(key))

    def test_default_properties_set_correctly(self):
        """The values in DEFAULTS should be correctly set on Config.

        This assumes that nothing else in VeloAnalysisFramework will modify
        Config.
        """
        c = Config()
        for key, value in DEFAULTS.iteritems():
            self.assertEqual(getattr(c, key), value)

    def test_property_values_persist_across_instances_on_init(self):
        """Multiple instances of Config should have equal property values."""
        key = self.test_key
        value = self.test_value
        # Instantiate the first Config with a custom property value
        Config(**{key: value})
        # Instantiate a second instance, which should have the custom value
        self.assertEqual(getattr(Config(), key), value)

    def test_property_values_persist_across_instances_on_assignment(self):
        """Multiple instances of Config should have equal property values."""
        key = self.test_key
        value = self.test_value
        # Instantiate the first Config, then set the custom value as a property
        c1 = Config()
        setattr(c1, key, value)
        # Instantiate a second instance, which should have the custom value
        self.assertEqual(getattr(Config(), key), value)


if __name__ == "__main__":
        unittest.main()
