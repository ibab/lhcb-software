"""
SvnChecker module.

SvnChecker is a framework to implement hook scripts for Subversion.
It is mainly aimed to pre-commit checks but can be used for other hooks.

The framework is based on checker objects that can be combined with the '+' and
'*' operators (equivalent to 'or' and 'and' logic operators) or negated with the
unary '-' operator (equivalent to the 'not' operator). A Success and a Failure
checker classes are provided for convenience.

Checkers exist in two families:
 - transaction checkers: act on the whole transaction (commit)
 - path checkers: act on individual files in a transaction

Path checkers can be combined to transaction checkers after being wrapped in a
ForeachPath instance, possibly via the shortcuts AllPaths and AnyPath.
The wrappers use a regular expression or a callable to filter the paths to be
passed to the path checkers. If the transaction does't contain any path matching
the filter, the transaction is considered good (the logic can be reversed with
the unary '-' operator, negation).

A check script can be implemented calling the 'run' function (in the Core
module), passing to it the transaction checker to be used (usually a combination
of checkers).

Alternatively, a transaction instance can be created and passed to the checker
object (which is a callable object) which will return a tuple with boolean and a
string, respectively telling if the check passed or not and why.

Some generic checkers are provided to simplify the implementation of custom
checks.

The submodule LHCbCheckers contains the checker instances used in the LHCb
pre-commit script (https://svnweb.cern.ch/trac/admin/browser/lhcb/usr-hooks/svn-hooks/pre-commit-check.py).

Created on Nov 24, 2010
"""
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"
__version__ = "$Id$"

from Core import *
from StdCheckers import *
