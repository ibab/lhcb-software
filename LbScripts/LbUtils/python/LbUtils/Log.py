# $Id: Log.py,v 1.6 2009-11-04 16:29:11 hmdegaud Exp $
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

def addDefaultLogger(parser, format=None):
    console = logging.StreamHandler()
    if not format:
        format = _default_log_format
    formatter = logging.Formatter(format)
    console.setFormatter(formatter)
    log = logging.getLogger()
    log.addHandler(console)
    log.setLevel(logging.WARNING)
    parser.set_defaults(loglevel="WARNING")
    grp = parser.add_option_group("Logging")
    grp.add_option("--silent",
                      action="callback",
                      callback=setLogLevelCallBack, 
                      dest="loglevel",
                      help="silent mode. Only CRITICALS")
    grp.add_option("--quiet",
                      action="callback",
                      callback=setLogLevelCallBack, 
                      dest="loglevel",
                      help="quiet mode. Only ERRORS and CRITICALS")
    grp.add_option("--verbose",
                      action="callback", 
                      callback=setLogLevelCallBack, 
                      dest="loglevel",
                      help="verbose mode [default: %default]. Everything down to INFOS")
    grp.add_option("--debug",
                      action="callback",
                      callback=setLogLevelCallBack,
                      dest="loglevel",
                      help="debug mode. Everything")
    return log
