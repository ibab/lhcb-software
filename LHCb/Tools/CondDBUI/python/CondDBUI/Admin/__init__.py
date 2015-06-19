"""
Python module for the administration of LHCb Conditions Database.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: __init__.py,v 1.3 2009-06-30 14:55:08 marcocle Exp $"

from Management import *
# fixme: to be moved into Management
from _internals import (MergeAndTag,
                        MakeDBFromFiles, CompareDBToFiles,
                        DumpToFiles,
                        timegm, timeToValKey)

from ReleaseNotes import *
