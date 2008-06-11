import logging

def setLogLevelCallBack(option, opt_str, value, parser):
    log = logging.getLogger()
    if opt_str == "--silent":
        log.setLevel(logging.CRITICAL)
        setattr(parser.values, option.dest, "CRITICAL")    
    if opt_str == "-q" or opt_str == "--quiet":
        log.info("setting log level to ERROR")
        log.setLevel(logging.ERROR)
        setattr(parser.values, option.dest, "ERROR")
    if opt_str == "-v" or opt_str == "--verbose":
        log.setLevel(logging.INFO)
        log.info("setting log level to INFO")
        setattr(parser.values, option.dest, "INFO")
    if opt_str == "--debug":
        log.setLevel(logging.DEBUG)
        setattr(parser.values, option.dest, "DEBUG")
        log.info("setting log level to DEBUG")
        formatter = logging.Formatter("%(levelname)-8s: %(funcName)-25s %(message)s")
        for h in log.handlers :
            h.setFormatter(formatter)

def addDefaultLogger(parser, format=None):
    console = logging.StreamHandler()
    if not format:
        format = "%(levelname)-8s: %(message)s"
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
                      help="silent mode")
    grp.add_option("-q", "--quiet",
                      action="callback",
                      callback=setLogLevelCallBack, 
                      dest="loglevel",
                      help="quiet mode")
    grp.add_option("-v", "--verbose",
                      action="callback", 
                      callback=setLogLevelCallBack, 
                      dest="loglevel",
                      help="verbose mode [default: %default]")
    parser.set_defaults(debug=False)
    grp.add_option("--debug",
                      action="callback",
                      callback=setLogLevelCallBack,
                      dest="loglevel",
                      help="debug mode")
    return log
