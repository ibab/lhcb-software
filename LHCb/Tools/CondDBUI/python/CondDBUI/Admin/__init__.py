"""
Python module for the administration of LHCb Conditions Database.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: __init__.py,v 1.2 2008-09-25 22:56:16 marcocle Exp $"

from Management import *
# fixme: to be moved into Management
from _internals import MergeAndTag, MakeDBFromFiles, timegm, timeToValKey

from ReleaseNotes import *
