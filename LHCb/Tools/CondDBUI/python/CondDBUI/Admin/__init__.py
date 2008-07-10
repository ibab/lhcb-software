"""
Python module for the administration of LHCb Conditions Database.
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id: __init__.py,v 1.1 2008-07-10 10:49:32 marcocle Exp $"

from Management import *
# fixme: to be moved into Management
from _internals import MergeAndTag, MakeDBFromFiles

from ReleaseNotes import *
