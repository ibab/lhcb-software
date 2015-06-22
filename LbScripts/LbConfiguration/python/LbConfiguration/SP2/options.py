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
Common functions to add common options to a OptionParser instance.
'''
__author__ = 'Marco Clemencic <marco.clemencic@cern.ch>'

import os
import re

from LbConfiguration.SP2 import SearchPathEntry, EnvSearchPathEntry, SearchPath

class LHCbDevPathEntry(EnvSearchPathEntry):
    def __init__(self):
        EnvSearchPathEntry.__init__(self, 'LHCBDEV')
    def __repr__(self):
        return '{0}()'.format(self.__class__.__name__)

class NightlyPathEntry(SearchPathEntry):
    def __init__(self, base, slot, day):
        self.base, self.slot, self.day = base, slot, day
    @property
    def path(self):
        return os.path.join(self.base, self.slot, self.day)
    def __str__(self):
        search_path = map(str, [self.path] + self.getNightlyExtraPath())
        return os.pathsep.join(search_path)
    def getNightlyExtraPath(self):
        extra_path = []
        path = self.path
        confSumm_file = os.path.join(path, 'confSummary.py')
        config_file = os.path.join(path, 'configuration.xml')
        if os.path.exists(confSumm_file): # Try with the python digested version
            data = {'__file__': confSumm_file}
            exec open(confSumm_file).read() in data #IGNORE:W0122
            # Get the list and convert it to strings
            extra_path = data.get('cmtProjectPathList',[])
        elif os.path.exists(config_file): # Try with the XML configuration
            from LbConfiguration.SetupProject import getNightlyCMTPROJECTPATH
            extra_path = getNightlyCMTPROJECTPATH(config_file,
                                                  self.slot, self.day)
        return map(SearchPathEntry, extra_path)
    def toCMake(self):
        return ('# Use the nightly builds search path if needed.\n'
                'if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/build.conf)\n'
                '  file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/build.conf build_conf)\n'
                '  foreach(l ${build_conf})\n'
                '    if(l MATCHES "^ *([a-zA-Z_][a-zA-Z0-9_]*)=([^ ]*) *$$")\n'
                '      set(${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")\n'
                '    endif()\n'
                '  endforeach()\n'
                'endif()\n'
                'if(NOT nightly_base)\n'
                '  set(nightly_base "$ENV{LHCBNIGHTLIES}")\n'
                'endif()\n'
                '\n'
                'if(nightly_slot)\n'
                '  if(EXISTS ${nightly_base}/${nightly_slot}/${nightly_day}/searchPath.cmake)\n'
                '    include(${nightly_base}/${nightly_slot}/${nightly_day}/searchPath.cmake)\n'
                '  list(INSERT CMAKE_PREFIX_PATH 0 "${nightly_base}/${nightly_slot}/${nightly_day}")\n'
                '  endif()\n'
                'endif()\n')
    def toCMT(self, shell='sh'):
        if shell == 'csh':
            return '''# Use the nightly builds search path if needed.
if ( -e build.conf ) then
    eval `sed -n '/^[^#]/{{s/^/set /;s/$/ ;/p}}' build.conf`
endif
if ( ! $?nightly_base ) then
    if ( $?LHCBNIGHTLIES ) then
        set nightly_base = "$LHCBNIGHTLIES"
    else
        set nightly_base = ""
    endif
endif

if ( $?nightly_slot && $?nightly_day ) then
    if ( $?CMTPROJECTPATH ) then
        set SAVED_CMTPROJECTPATH = ":$CMTPROJECTPATH"
    else
        set SAVED_CMTPROJECTPATH = ""
    endif
    if ( -e ${nightly_base}/${nightly_slot}/${nightly_day}/setupSearchPath.csh ) then
        source ${nightly_base}/${nightly_slot}/${nightly_day}/setupSearchPath.csh
    else
        setenv CMTPROJECTPATH "${nightly_base}/${nightly_slot}/${nightly_day}"
    endif
    # This a temporary work around because setupSearchPath.csh overrides CMTPROJECTPATH
    # instead of extending it.
    setenv CMTPROJECTPATH "${CMTPROJECTPATH}${SAVED_CMTPROJECTPATH}"
endif
'''
        return '''# Use the nightly builds search path if needed.
if [ -e ./build.conf ] ; then
    . ./build.conf
fi
if [ -z "$nightly_base" ] ; then
    nightly_base="$LHCBNIGHTLIES"
fi

if [ -e ${nightly_base}/${nightly_slot}/${nightly_day} ] ; then
    if [ -e ${nightly_base}/${nightly_slot}/${nightly_day}/setupSearchPath.sh ] ; then
        SAVED_CMTPROJECTPATH="${CMTPROJECTPATH:+:$CMTPROJECTPATH}"
        . ${nightly_base}/${nightly_slot}/${nightly_day}/setupSearchPath.sh
        # This a temporary work around because setupSearchPath.sh overrides CMTPROJECTPATH
        # instead of extending it.
        export CMTPROJECTPATH="${CMTPROJECTPATH}${SAVED_CMTPROJECTPATH}"
    else
        export CMTPROJECTPATH="${nightly_base}/${nightly_slot}/${nightly_day}${CMTPROJECTPATH:+:$CMTPROJECTPATH}"
    fi
fi
'''
    def __repr__(self):
        return '{0}({1!r}, {2!r}, {3!r})'.format(self.__class__.__name__,
                                                 self.base, self.slot, self.day)

def addSearchPath(parser):
    '''
    Common options used to extend the search path.
    '''
    from optparse import OptionValueError

    def dev_dir_cb(option, opt_str, value, parser):
        if value is None:
            try:
                value = LHCbDevPathEntry()
            except ValueError:
                raise OptionValueError('--dev used, but LHCBDEV is not defined')
        else:
            value = SearchPathEntry(value)
        parser.values.dev_dirs.append(value)

    parser.add_option('--dev', action='callback',
                      callback=dev_dir_cb,
                      help='prepend $LHCBDEV to the search path. '
                           'Note: the directories are searched in the '
                           'order specified on the command line.')
    parser.add_option('--dev-dir', action='callback', metavar='DEVDIR',
                      type='string', callback=dev_dir_cb,
                      help='prepend DEVDIR to the search path. '
                           'Note: the directories are searched in the '
                           'order specified on the command line.')

    def nightly_base(option, opt_str, value, parser):
        '''
        Callback for the --nightly-base and --nightly-cvmfs options.
        '''
        if parser.values.nightly:
            raise OptionValueError('%s specified after --nightly' % option)

        if option.get_opt_string() == '--nightly-cvmfs':
            path = '/cvmfs/lhcbdev.cern.ch/nightlies'
        else:
            path = value
        if not os.path.isdir(path):
            raise OptionValueError('"%s" is not a directory' % path)

        parser.values.nightly_bases.append(path)

    parser.add_option('--nightly-base', action='callback',
                      type='string', callback=nightly_base,
                      help='add the specified directory to the nightly builds '
                           'search path (must be specified before --nightly)')

    parser.add_option('--nightly-cvmfs', action='callback',
                      callback=nightly_base,
                      help='looks for nightly builds on CVMFS '
                           '(must be specified before --nightly)')

    def nightly_option(_option, opt_str, _value, _parser):
        # List of abbreviations for weekdays
        days = ('Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun', 'Today')
        day = 'Today'

        parser.values.dev = True
        rargs = parser.rargs

        try:
            slot = rargs.pop(0)
        except IndexError:
            raise OptionValueError('%s must be followed by the slot of the '
                                   'nightlies and optionally by the day'
                                   % opt_str)

        if rargs:
            if rargs[0].capitalize() in days:
                day = rargs.pop(0).capitalize()
            elif re.match(r'^\d*$', rargs[0]):
                day = rargs.pop(0)

        # Locate the requested slot in the know nightlies directories
        from os import environ
        nightly_bases = parser.values.nightly_bases
        nightly_bases += [environ.get('LHCBNIGHTLIES',
                                      '/afs/cern.ch/lhcb/software/nightlies'),
                          environ.get('LCG_nightlies_area',
                                      '/afs/cern.ch/sw/lcg/app/nightlies')]

        slot_dir = None
        for nightly_base, slot_id in [(nightly_base, slot_id)
                                      for slot_id in (slot, 'lhcb-' + slot)
                                      for nightly_base in nightly_bases]:
            slot_dir = os.path.join(nightly_base, slot_id)
            if os.path.isdir(slot_dir):
                break # exit from the loop as soon as the slot is found
        else:
            raise OptionValueError('Cannot find slot %s in %s. '
                                   'Check the values of the option %s'
                                   % (slot, nightly_bases, opt_str))
        # set slot to the actual slot name
        slot = slot_id

        path = os.path.join(slot_dir, day)
        if not os.path.isdir(path):
            raise OptionValueError('The directory %s does not exists. '
                                   'Check the values of the option %s'
                                   % (path, opt_str))

        parser.values.dev_dirs.append(NightlyPathEntry(nightly_base, slot, day))
        parser.values.nightly = (slot, day, nightly_base)

    parser.add_option('--nightly', action='callback',
                      metavar='SLOT [DAY]', type='string',
                      callback=nightly_option,
                      nargs=0,
                      help='Add the required slot of the LHCb nightly '
                           'builds to the list of DEV dirs. DAY must be '
                           'a 3 digit abbreviation of the weekday, '
                           'by default the current day. Special settings '
                           'of the CMTPROJECTPATH needed for the '
                           'nightly build slot are taken into account.')

    parser.add_option('--user-area', action='store',
                      help='Use the specified path as User_release_area instead of '
                           'the value of the environment variable.')

    parser.add_option('--no-user-area', action='store_true',
                      help='Ignore the user release area when looking for projects.')

    parser.set_defaults(dev_dirs=SearchPath([]),
                        user_area=os.environ.get('User_release_area'),
                        no_user_area=False,
                        nightly=None,
                        nightly_bases=[])

    return parser

def addOutputLevel(parser):
    '''
    Add options to get more or less messages.
    '''
    import logging
    parser.add_option('--verbose', action='store_const',
                      const=logging.INFO, dest='log_level',
                      help='print more information')
    parser.add_option('--debug', action='store_const',
                      const=logging.DEBUG, dest='log_level',
                      help='print debug messages')
    parser.add_option('--quiet', action='store_const',
                      const=logging.WARNING, dest='log_level',
                      help='print only warning messages (default)')

    parser.set_defaults(log_level=logging.WARNING)

    return parser

def addPlatform(parser):
    '''
    Add option to specify a platform.
    '''

    parser.add_option('-c', '--platform',
                      help='runtime platform [default: %default]')


    if 'BINARY_TAG' in os.environ:
        platform = os.environ['BINARY_TAG']
    elif 'CMTCONFIG' in os.environ:
        platform = os.environ['CMTCONFIG']
    else:
        # auto-detect
        from LbConfiguration.Platform import NativeMachine
        supported = NativeMachine().CMTSupportedConfig()
        if supported:
            platform = supported[0]
        else:
            parser.error('unknown system, set the environment or use --platform')

    parser.set_defaults(platform=platform)

    return parser


def addListing(parser):
    '''
    Add option to request the list of versions.
    '''

    parser.add_option('-l', '--list', action='store_true',
                      help='list the available versions of the requested '
                           'project and exit')

    parser.set_defaults(list=False)

    return parser
