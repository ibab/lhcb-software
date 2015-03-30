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
import os

def _cmakeStr(txt):
    return ' '.join('"%s"' % entry.replace('"', '\\"')
                    for entry in str(txt).split(os.pathsep))

_CMTPROJECTPATH_FMT_SH = ('export CMTPROJECTPATH="{0}'
                          '${{CMTPROJECTPATH:+:${{CMTPROJECTPATH}}}}"\n')
_CMTPROJECTPATH_FMTS = {
'csh': '''if ( $?CMTPROJECTPATH ) then
  setenv CMTPROJECTPATH "{0}:${{CMTPROJECTPATH}}"
else
  setenv CMTPROJECTPATH "{0}"
endif
''',
'sh': _CMTPROJECTPATH_FMT_SH
}

class SearchPathEntry(object):
    def __init__(self, path):
        self.path = path
    def __str__(self):
        return str(self.path)
    def toCMake(self):
        return 'list(INSERT CMAKE_PREFIX_PATH 0 {0})\n'.format(_cmakeStr(self))
    def toCMT(self, shell='sh'):
        # FIXME: should we escape quotes?
        return _CMTPROJECTPATH_FMTS[shell].format(self)
    def __repr__(self):
        return '{0}({1!r})'.format(self.__class__.__name__, self.path)

class EnvSearchPathEntry(SearchPathEntry):
    def __init__(self, envname, default=None):
        if default is None and not envname in os.environ:
            raise ValueError('environment variable %s not defined' % envname)
        self.envname = envname
        self.default = default
    def __str__(self):
        return str(os.environ.get(self.envname, self.default))
    def toCMake(self):
        return (('if(NOT ENV{{{0}}})\n'
                 '  # use a default value\n'
                 '  set(ENV{{{0}}} {1})\n'
                 'endif()\n'
                 'list(INSERT CMAKE_PREFIX_PATH 0 "$ENV{{{0}}}")\n')
                .format(self.envname, _cmakeStr(self)))
    def toCMT(self, shell='sh'):
        if self.envname == 'CMAKE_PREFIX_PATH':
            return '' # ignore CMAKE_PREFIX_PATH for CMT builds
        if shell == 'csh':
            fmt = ('if ( ! $?{0} ) then\n'
                   '  # use a default value\n'
                   '  setenv {0} "{1}"\n'
                   'endif\n')
        else:
            fmt = ('if [ -z "${0}" ] ; then\n'
                   '  # use a default value\n'
                   '  export {0}="{1}"\n'
                   'fi\n')
        # FIXME: should we escape quotes?
        return (fmt.format(self.envname, self) +
                _CMTPROJECTPATH_FMTS[shell].format('${' + self.envname+ '}'))
    def __repr__(self):
        return '{0}({1!r}, {2!r})'.format(self.__class__.__name__,
                                          self.envname, str(self))


def _toEntry(entry):
    '''ensure that the entry is a SearchPathEntry'''
    if isinstance(entry, SearchPathEntry):
        return entry
    return SearchPathEntry(entry)

class SearchPath(object):
    def __init__(self, entries):
        self.entries = list(entries)
    def __iter__(self):
        def generator():
            for entry in self.entries:
                paths = str(entry).split(os.pathsep)
                for path in paths:
                    yield path
        return generator()
    def insert(self, index, value):
        return self.entries.insert(index, value)
    def append(self, value):
        return self.entries.append(value)
    def __setitem__(self, index, value):
        if isinstance(value, SearchPath):
            value = value.entries
        return self.entries.__setitem__(index, value)
    def __str__(self):
        return os.pathsep.join(self)
    def __nonzero__(self):
        return bool(self.entries)
    def __add__(self, other):
        if isinstance(other, SearchPath):
            other = other.entries
        return SearchPath(self.entries + other)
    def toCMake(self):
        # we need to add them in reverse order because the 'prepend'
        return '\n'.join(_toEntry(entry).toCMake()
                         for entry in self.entries[::-1])
    def toCMT(self, shell='sh'):
        # we need to add them in reverse order because the 'prepend'
        return '\n'.join(_toEntry(entry).toCMT(shell)
                         for entry in self.entries[::-1])

    def __repr__(self):
        return '{0}({1!r})'.format(self.__class__.__name__,
                                   self.entries)

def _defaultPath():
    '''
    Return the default search path, based on the variables CMAKE_PREFIX_PATH and
    CMTPROJECTPATH.
    '''
    env_vars = ['CMTPROJECTPATH', 'LHCBPROJECTPATH', 'CMAKE_PREFIX_PATH']
    path = []
    for name in env_vars:
        try:
            path.append(EnvSearchPathEntry(name))
        except ValueError:
            pass # ignore undefined variables
    return path

path = SearchPath(_defaultPath())

class Error(RuntimeError):
    '''
    Base class for SP2 exceptions.
    '''
