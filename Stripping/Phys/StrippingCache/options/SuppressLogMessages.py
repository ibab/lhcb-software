# -*- coding: utf-8 -*-
from Gaudi.Configuration import log
import logging
import re

IGNORED_MESSAGES = map(re.compile,
                       (
                        # errors
                        r'The default_config of .* (is not a dictionary|has incorrect keys)',
                        r'The key .* is not allowed',
                        r'Allowed keys are:',
                        # warnings
                        r'Line .* is declared but not appended to any stream',
                        r'uses plain Gaudi configurable',
                        r'has non-default configuration',
                        r'    .*\(default =',
                        r'The usage of unique HLT is deprecated',
                        r'Requests Flavor Tagging but is not assigned',
                        r'has prescale <= 0',
                        r'share the same output location',
                        r'how to add new bank',
                        r'Using default tag.*for partition',
                        r'The lines going to MDST.DST are'
                       ))

_orig_log_filter = log.filter
def old_stripping_filter(record):
    '''
    Hide known warnings and errors in old Stripping configurations.
    '''
    if record.levelno >= logging.WARNING:
        if any(exp.search(record.msg) for exp in IGNORED_MESSAGES):
            return False
        if not record.msg.strip():
            return False # why should anyone want to print an empty warning?
    return _orig_log_filter(record)

log.filter = old_stripping_filter
