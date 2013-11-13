#!/usr/bin/env python
try:
    from pytz import utc, timezone
    print "Successfully imported the required names from pytz"
except ImportError:
    print "Could not import all names from pytz"
