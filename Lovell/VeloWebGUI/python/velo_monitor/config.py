import os

# Application name, used in page titles etc.
APP_NAME = 'VELO Monitor'

# Description, shown in the <meta> tag
APP_DESCRIPTION = 'A monitoring application for the LHCb Vertex Locator.'

# List of emails to send ERROR messages to
ADMINS = ['alex.pearce@cern.ch']

# Secret key for sessions
SECRET_KEY = (
    '\xd9;\n\x1fEX\x9dB}\xe0\xc9\xd0'
    '\xb8H\xd9\x1d\xcbW\xff\xf9\xdeS\x12\xdb'
    'y\xdd\x18<\xdc\xda\xd7\xc3\x915\xbe\xcc'
)

# Mappings of parent paths to their default children
# The key represents the visited path, the value is the page that's served
# For the dict below, a visited path of `examples` will show the
# `examples/table` page, as an example
DEFAULT_CHILDREN = {
    # VELO view
    '': 'velo_view',
    'velo_view': 'velo_view/overview',
    'velo_view/trends': 'velo_view/trends/nzs',
    # Run view
    'run_view': 'run_view/dq',
    # Special analysis
    'special_analyses': 'special_analyses/iv_scans',
}

try:
    DEBUG = True if os.environ['DEBUG'] else False
except:
    DEBUG = False
