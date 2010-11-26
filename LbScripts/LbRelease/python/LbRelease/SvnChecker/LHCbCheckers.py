'''
Module containing the standard LHCb checker instances.

The content of the module is used in the LHCb pre-commit script
(https://svnweb.cern.ch/trac/admin/browser/lhcb/usr-hooks/svn-hooks/pre-commit-check.py).

Created on Nov 25, 2010
'''
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

from StdCheckers import *

__all__ = ("validTag", "librarian", "nightlyConf")

# Standard tag validation policy
validTag = AllPaths(TagRemoval() + TagIntermediateDirs() + ProjectTag() + PackageTag(),
                    ".*/tags/.*")

# Check for the librarian account (used to give super user powers to it).
librarian = AllowedUsers(["liblhcb"])

# Run the XML checkers on the nightly build configuration.
nightlyConf = AllPaths(ValidXml(), r".*LHCbNightlyConf/trunk/configuration\.xml$")
