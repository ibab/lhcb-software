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
    return dict(comp.environment.items())
