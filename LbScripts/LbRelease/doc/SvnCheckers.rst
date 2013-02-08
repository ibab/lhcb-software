Deploy LHCB SVN Checkers
------------------------

To deploy the pre-commit hooks, you need to checkout the ``admin`` directory of the
LHCb SVN repository (as ``liblhcb``)::

    svn co svn+ssh://liblhcb@svn.cern.ch/reps/admin/lhcb
    cd lhcb/usr-hooks/svn-hooks

Then remove all the Python files in the SvnChecker directory and replace them
with the latest version in LbScripts::

    rm SvnChecker/*.py
    svn export --force svn+ssh://svn.cern.ch/reps/lhcb/LbScripts/trunk/LbRelease/python/LbRelease/SvnChecker
    svn add SvnChecker/*.py

If needed (for example there is a new checker to be added to the called ones)
modify the script ``pre-commit-check.py``.

To put the new code in production, just commit the changes.
