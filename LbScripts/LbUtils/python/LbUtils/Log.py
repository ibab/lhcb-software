# $Id$
""" implement the default logging settings to the parser"""

import logging
import sys

_default_log_format = "%(levelname)-8s: %(message)s"

def setLogLevelCallBack(option, opt_str, value, parser):
    log = logging.getLogger()
    if opt_str == "--silent":
        log.setLevel(logging.CRITICAL)
        setattr(parser.values, option.dest, "CRITICAL")
    if opt_str == "--quiet":
        log.info("setting log level to ERROR")
        log.setLevel(logging.ERROR)
        setattr(parser.values, option.dest, "ERROR")
    if opt_str == "--verbose":
        log.setLevel(logging.INFO)
        log.info("setting log level to INFO")
        setattr(parser.values, option.dest, "INFO")
    if opt_str == "--debug":
        log.setLevel(logging.DEBUG)
        setattr(parser.values, option.dest, "DEBUG")
        log.info("setting log level to DEBUG")
        if not sys.version_info < (2,5,1,0,0) : # when funcName was not available
            formatter = logging.Formatter("%(levelname)-8s: %(funcName)-25s %(message)s")
            for h in log.handlers :
                h.setFormatter(formatter)
    if opt_str == "--log-level":
        log.setLevel(value)
        log.info("setting log level to %s" % value)
        setattr(parser.values, option.dest, value)

def addDefaultLogger(parser, log_format=None):
    log = logging.getLogger()
    has_streamhandler = False
    for h in log.handlers :
        if isinstance(h, logging.StreamHandler) :
            has_streamhandler = True
            break
    if not has_streamhandler :
        console = logging.StreamHandler()
        if not log_format:
            log_format = _default_log_format
        formatter = logging.Formatter(log_format)
        console.setFormatter(formatter)
        log.addHandler(console)
    log.setLevel(logging.WARNING)
    parser.set_defaults(log_level="WARNING")
    grp = parser.add_option_group("Logging")
    grp.add_option("--silent",
                   action="callback",
                   callback=setLogLevelCallBack,
                   dest="log_level",
                   help="silent mode. Only CRITICALS")
    grp.add_option("--quiet",
                   action="callback",
                   callback=setLogLevelCallBack,
                   dest="log_level",
                   help="quiet mode. Only ERRORS and CRITICALS")
    grp.add_option("--verbose",
                   action="callback",
                   callback=setLogLevelCallBack,
                   dest="log_level",
                   help="verbose mode. Everything down to INFOS")
    grp.add_option("--debug",
                   action="callback",
                   callback=setLogLevelCallBack,
                   dest="log_level",
                   help="debug mode. Everything")
    grp.add_option("--log-level",
                   action="callback",
                   callback=setLogLevelCallBack,
                   nargs=1,
                   dest="log_level",
                   help="default log level [default %default]")
    return log


class NullHandler(logging.Handler):
    """ null handler for libraries. This is usefull when a top application doesn't
    define any logger. This handler has to be added to each library logger like:
     h = NullHandler()
     logging.getLogger("foo").addHandler(h)
    """
    def emit(self, record):
        pass