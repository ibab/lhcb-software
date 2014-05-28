###############################################################################
# (c) Copyright 2013 CERN                                                     #
#                                                                             #
# This software is distributed under the terms of the GNU General Public      #
# Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   #
#                                                                             #
# In applying this licence, CERN does not waive the privileges and immunities #
# granted to it by virtue of its status as an Intergovernmental Organization  #
# or submit itself to any jurisdiction.                                       #
###############################################################################
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import sys
assert sys.version_info >= (2, 6), "Python >= 2.6 is required"

def _defaultPath():
    '''
    Return the default search path, based on the variables CMAKE_PREFIX_PATH and
    CMTPROJECTPATH.
    '''
    import os
    env_vars = ['CMAKE_PREFIX_PATH', 'CMTPROJECTPATH', 'LHCBPROJECTPATH']
    is_set = lambda v: os.environ.get(v)
    var2path = lambda v: os.environ[v].split(os.pathsep)
    return sum(map(var2path, filter(is_set, env_vars)), [])

path = _defaultPath()

class Error(RuntimeError):
    '''
    Base class for SP2 exceptions.
    '''
