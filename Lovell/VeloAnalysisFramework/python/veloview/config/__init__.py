"""VeloAnalysisFramework configuration.

The Config object hold the various parameters available to configure the module
and its associated GUIs.

The most important configuration parameter is Config().run_data_dir, which
defines the prefix for all files that VeloAnalysisFramework reads and writes.
This can be changed either by editing its value with the Config class:

    >>> Config(run_data_dir='/foo/bar')
    # This is equivalent
    >>> c1 = Config()
    >>> c1.run_data_dir = '/foo/bar'

or by running Python with the RUN_DATA_DIR environment variable set:

    $ RUN_DATA_DIR=/foo/bar python VeloAnalysisFramework/scripts/foo.py
"""
import os

from veloview.config import run_view, analysis_config

# Default configuration values
# The keys define the properties of the Config object that can be get _and_ set
DEFAULTS = {
    # Default value can be set by the RUN_DATA_DIR environment variable
    'run_data_dir': (
        os.environ.get('RUN_DATA_DIR', None) or
        '/calib/velo/dqm/VeloView/VetraOutput'
    ),
    'processed_run_list': 'RunList.txt',
    'reference_db': 'ReferenceDB.sqlite',
    'run_view_pages': run_view.run_view_pages,
    'analysis_config': (analysis_config.analysis_config_branches, analysis_config.analysis_config_leaves),
    'grf_file_name': 'GRF.root',
    'grf_tree_name': 'DQTree',
}

# Only export the Config class
__all__ = [
    'Config'
]


class Config(object):
    """Container object to hold configuration parameters as properties.

    Each key in the DEFAULT dictionary will be available on this module as
    properties, which can be retrieved and set as usual.
    For example:

        >>> c = Config()
        >>> c.processed_run_list
        'RunList.txt'
        >>> c.reference_db = 'CustomDB.sqlite'
        >>> c.reference_db
        'CustomDB.sqlite'

    Config acts as a singleton, in that multiple instances will always have
    identical configuration values:

        >>> c1 = Config(some_config_key='SomeValue')
        >>> c2 = Config()
        >>> c1.some_config_key == c2.some_config_key
        True

    It does this by storing the configuring values as attributes on the
    _class_, that is Config, rather than within the instance dictionary.
    This allows for setting properties to values to also work as expected:

        >>> c1 = Config()
        >>> c1.some_config_key = 'SomeValue'
        >>> c2 = Config()
        >>> c1.some_config_key == c2.some_config_key
        True
    """
    def __init__(self, **kwargs):
        """Create the configuration object, using kwargs as values.

        If no kwargs are specified, the default values are used.
        """
        for config_key, default in DEFAULTS.iteritems():
            # If the key exists as a class attribute, use it, else set/update
            # kwargs over class attributes, class attributes over defaults
            try:
                class_value = getattr(Config, config_key)
            except AttributeError:
                class_value = default
            setattr(Config, config_key, kwargs.get(config_key, class_value))

    def __setattr__(self, name, value):
        """Set the class attribute, rather than that of the instance."""
        setattr(Config, name, value)

    @property
    def processed_run_list_path(self):
        """Return the full path of the list of processed runs."""
        return os.path.join(self.run_data_dir, self.processed_run_list)

    @property
    def reference_db_path(self):
        """Return the full path of the reference database."""
        return os.path.join(self.run_data_dir, self.reference_db)

    @property
    def grf_file_path(self):
        """Return the full path of the GRF."""
        return os.path.join(self.run_data_dir, self.grf_file_name)
