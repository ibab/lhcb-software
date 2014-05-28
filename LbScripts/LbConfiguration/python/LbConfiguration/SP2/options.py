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

class SearchPathEntry(object):
    def __init__(self, path):
        self.path = path
    def __str__(self):
        return str(self.path)

class LHCbDevPathEntry(object):
    def __str__(self):
        return str(os.environ['LHCBDEV'])

class NightlyPathEntry(object):
    def __init__(self, base, slot, day):
        self.base, self.slot, self.day = base, slot, day
    def __str__(self):
        return str(os.path.join(self.base, self.slot, self.day))

def getNightlyExtraPath(path, slot, day):
    extra_path = []
    confSumm_file = os.path.join(path, 'confSummary.py')
    config_file = os.path.join(path, 'configuration.xml')
    if os.path.exists(confSumm_file): # Try with the python digested version
        data = {}
        exec open(confSumm_file).read() in data #IGNORE:W0122
        # Get the list and convert it to strings
        extra_path = filter(str, data.get('cmtProjectPathList',[]))
    elif os.path.exists(config_file): # Try with the XML configuration
        from LbConfiguration.SetupProject import getNightlyCMTPROJECTPATH
        extra_path = getNightlyCMTPROJECTPATH(config_file, slot, day)
    return extra_path

def addSearchPath(parser):
    '''
    Common options used to extend the search path.
    '''
    from optparse import OptionValueError

    def dev_dir_cb(option, opt_str, value, parser):
        if value is None:
            if 'LHCBDEV' in os.environ:
                value = LHCbDevPathEntry()
            else:
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

    def nightly_option(_option, opt_str, _value, _parser):
        # List of abbreviations for weekdays
        days = ('Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun')
        import datetime
        day = days[datetime.date.today().weekday()]

        parser.values.dev = True
        rargs = parser.rargs

        try:
            slot = rargs.pop(0)
        except IndexError:
            raise OptionValueError('%s must be followed by the slot of the nightlies and optionally by the day' % opt_str)

        if rargs and rargs[0].capitalize() in days:
            day = rargs.pop(0).capitalize()

        # Locate the requested slot in the know nightlies directories
        nightly_bases = [os.environ.get('LHCBNIGHTLIES', '/afs/cern.ch/lhcb/software/nightlies'),
                         os.path.normpath(os.path.join(os.environ.get('LCG_release_area', '/afs/cern.ch/sw/lcg/app/releases'), os.pardir, 'nightlies'))]

        slot_dir = None
        for nightly_base in nightly_bases:
            slot_dir = os.path.join(nightly_base, slot)
            if os.path.isdir(slot_dir): break # exit from the loop as soon as the slot is found
        else:
            raise OptionValueError('Cannot find slot %s in %s. Check the values of the option %s' % (slot, nightly_bases, opt_str))

        path = os.path.join(slot_dir, day)
        if not os.path.isdir(path):
            raise OptionValueError('The directory %s does not exists. Check the values of the option %s' % (path, opt_str))

        parser.values.dev_dirs.append(NightlyPathEntry(nightly_base, slot, day))
        parser.values.nightly = (slot, day)

        # Get the extra CMTPROJECTPATH entries needed for the nightlies
        extra_path = getNightlyExtraPath(path, slot, day)
        if extra_path:
            parser.values.dev_dirs += map(SearchPathEntry, extra_path)

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

    parser.set_defaults(dev_dirs=[],
                        user_area=os.environ.get('User_release_area'),
                        no_user_area=False,
                        nightly=None)

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