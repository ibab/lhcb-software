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
'''
Functions to be able to use the old SetupProject from within SP2.
'''
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import sys

def expandAllVars(env, iterations=10):
    '''
    Expand all variables in the environment dict.

    >>> env = {'MYDATA': '${DATA}/subdir', 'DATA': '/main/path'}
    >>> expandAllVars(env)
    {'MYDATA': '/main/path/subdir', 'DATA': '/main/path'}
    '''
    from string import Template
    def expandVars(value, env):
        '''expand variables in a string using env, ignoring format errors'''
        try:
            return Template(value).safe_substitute(env)
        except ValueError:
            return value
    new_env = dict((key, expandVars(value, env))
                   for key, value in env.iteritems())
    if new_env !=  env: # we did some change
        if iterations > 0:
            # let's see if there is something more to change
            return expandAllVars(new_env, iterations-1)
        else:
            raise ValueError('failed to resolve all the variables (loop?)')
    return new_env

def getOldEnvironment(args):
    from LbConfiguration.SetupProject import SetupProject
    class Comp(SetupProject):
        def _prepare_parser(self):
            # add some extra (ignored) options
            parser = super(Comp, self)._prepare_parser()
            parser.add_option("--sh",
                              action="store_const", const="sh", dest="outshell",
                              help="Print the environment as shell commands "
                                   "for 'sh'-derived shells.")
            parser.add_option("--csh",
                              action="store_const", const="csh",
                              dest="outshell",
                              help="Print the environment as shell commands "
                                   "for 'csh'-derived shells.")
            parser.add_option("--py",
                              action="store_const", const="py", dest="outshell",
                              help="Print the environment as Python "
                                   "dictionary.")
            return parser
        def _write_script(self, data):
            # disable writing of the shell script
            pass
    comp = Comp()
    rc = comp.main(['--shell', 'sh'] + args)
    if rc:
        sys.exit(rc)
    return expandAllVars(dict(comp.environment.items()))
