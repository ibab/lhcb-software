#!/usr/bin/env python
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
Script to initialize a project for the build.
'''
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import os
import sys
import logging

from optparse import OptionParser
from LbUtils.Script import Script
from LbConfiguration import initProject

parser = OptionParser(usage='%prog [options] [project_root_dir]',
                      description='Initialize a directory for building a '
                                  'project, useful when checking out a project '
                                  'without using getpack (e.g. with git). '
                                  'If the argument project_root_dir is not '
                                  'specified, the required files are created '
                                  'in the current directory.')
parser.add_option('--overwrite', action='store_true',
                  help='overwrite existing files [default: %default]')
parser.add_option('-q', '--quiet',
                  action='store_const', dest='loglevel',
                  const=logging.WARNING,
                  help='be quiet (default)')
parser.add_option('-v', '--verbose',
                  action='store_const', dest='loglevel',
                  const=logging.INFO,
                  help='be more verbose')
parser.add_option('-d', '--debug',
                  action='store_const', dest='loglevel',
                  const=logging.DEBUG,
                  help='print debug messages')

parser.set_defaults(overwrite=False,
                    loglevel=logging.WARNING)

options, args = parser.parse_args()

logging.basicConfig(level=options.loglevel)

if not args:
    path = os.curdir
elif len(args) == 1:
    path = args[0]
else:
    parser.error('wrong number of arguments')

logging.debug("using project root '%s'", path)

initProject(path, options.overwrite)
