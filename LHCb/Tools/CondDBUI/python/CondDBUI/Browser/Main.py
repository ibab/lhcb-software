## @file Main.py
#  @brief Main module
#
#  This module contains the "main" function, which initializes the application
#  and starts the event loop.

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

# Define few global variables to tune the behavior depending on the environment
from os import environ as env
from os.path import isdir, exists, join
__pit_special_dir__ = "/group/online/condb_viewer"
__pit_environment__ = isdir(__pit_special_dir__)
if __pit_environment__:
    env["CORAL_DBLOOKUP_PATH"] = env["CORAL_AUTH_PATH"] = __pit_special_dir__


__default_option_files__ = [
    "$SQLDDDBROOT/options/SQLDDDB.py",
    "$VETRAROOT/options/Velo/VeloCondDB.py"
    ]

import logging

## Imports an options file to find the configured conditions databases.
#  @return A dictionary associating names (of the services) to connection strings.
def getStandardConnectionStrings(optionFiles):
    from Gaudi.Configuration import importOptions, allConfigurables
    from GaudiKernel.ProcessJobOptions import ParserError
    # output dictionary
    data = {}
    for o in optionFiles:
        try:
            importOptions(o)
        except ParserError, x:
            # Ignore errors from the parser (e.g. file not found)
            logging.info("Problems importing %s: %s", o, x)
    for name in allConfigurables:
        if hasattr(allConfigurables[name],"ConnectionString"):
            data[name] = allConfigurables[name].ConnectionString
    # If we are at the PIT (we can use Oracle)
    if __pit_environment__:
        for c in [ "CondDB/DDDB", "CondDB/LHCBCOND", "CondDB/SIMCOND", "CondDBOnline/ONLINE",
                   "CondDBPrivate/PRIVATE" ]:
            data[c] = c
    return data

## Initialize and start the application.
#  If an argument is passed on the command line, it is used to open the corresponding database
#  retrieved from the options file.
#  @see getStandardConnectionStrings()
def main(argv = []):
    from PyQt4.QtGui import QApplication
    from MainWindow import MainWindow

    # Extract version informations from CVS keywords
    __versionNumber__ = '$Name: not supported by cvs2svn $'.split()[1]
    if __versionNumber__ == "$":
        __versionNumber__ = 'HEAD version'

    __versionId__  = '$Id: Main.py,v 1.8 2009-12-08 14:19:59 marcocle Exp $'.split()
    if len(__versionId__) < 4:
        __versionDate__ = 'unknown'
    else:
        __versionDate__ = __versionId__[3]

    # Initializes the Qt application
    app = QApplication(argv)
    app.setApplicationName(app.objectName())
    app.setApplicationVersion("%s (%s)" % (__versionNumber__, __versionDate__) )
    app.setOrganizationName("LHCb")
    app.setOrganizationDomain("lhcb.cern.ch")
    # @FIXME: I need a flexible way of defining the default style (command line, settings, ...)
    # app.setStyle("plastique")

    # Get the list of command line arguments after the filter applied by QApplication
    # and convert them to strings so that we can pass the to optparse
    argv = map(str, app.arguments())

    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [Qt-options] [options] [database]")
    parser.add_option("--rw", action = "store_true",
                      help = "Open the database specified in the command line in read/write mode")
    parser.add_option("--options", action = "append",
                      dest = "option_files",
                      help = "Option files to be used to detect the standard databases (can be specified multiple times")
    parser.add_option("--verbose", action = "store_true",
                      help = "Increase verbosity")
    parser.set_defaults(rw = False,
                        option_files = __default_option_files__)

    opts, args = parser.parse_args(argv[1:])

    logging.basicConfig(level = opts.verbose and logging.INFO or logging.WARNING)

    if len(args) > 1:
        parser.error("Only one database can be specified on the command line (see --help).")

    mw = MainWindow()
    mw.setDefaultDatabases(getStandardConnectionStrings(opts.option_files))

    # Use the first (and only) argument as name of the database to open
    if args:
        db = args[0]
        readOnly = not opts.rw
        if db in mw.defaultDatabases:
            mw.openStandardDatabase(db, readOnly = readOnly)
        else:
            mw.openDatabase(db, readOnly = readOnly)

    mw.show()

    # Execute the event loop
    return app.exec_()
