## @file Main.py
#  @brief Main module
#
#  This module contains the "main" function, which initializes the application
#  and starts the event loop.

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

# Define few global variables to tune the behavior depending on the environment
from os import environ as env
__lfc_replica_svc__ = 'CORAL_LFC_BASEDIR' in env and 'LFC_HOST' in env and not 'COOL_IGNORE_LFC' in env

from os.path import isdir
__pit_special_dir__ = "/group/online/condb_viewer"
__pit_environment__ = isdir(__pit_special_dir__)
if __pit_environment__:
    env["CORAL_DBLOOKUP_PATH"] = env["CORAL_AUTH_PATH"] = __pit_special_dir__


## Imports an options file to find the configured conditions databases.
#  @return A dictionary associating names (of the services) to connection strings.
def getStandardConnectionStrings(optionFile = "$SQLDDDBROOT/options/SQLDDDB.py"):
    from Gaudi.Configuration import importOptions, allConfigurables
    from GaudiKernel.ProcessJobOptions import ParserError
    # output dictionary
    data = {}
    try:
        importOptions(optionFile)
        for name in allConfigurables:
            if hasattr(allConfigurables[name],"ConnectionString"):
                data[name] = allConfigurables[name].ConnectionString
    except ParserError:
        # Ignore errors from the parser (e.g. file not found)
        pass
    # If we are at the PIT or we can use the offline Oracle ...
    if __pit_environment__ or __lfc_replica_svc__:
        # ... add Oracle connection strings explicitly
        for c in [ "CondDB/DDDB", "CondDB/LHCBCOND", "CondDB/SIMCOND", "CondDBOnline/ONLINE" ]:
            data[c] = c
    if __pit_environment__: # these are only for the PIT
        for c in [ "CondDBPrivate/PRIVATE" ]:
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
    parser.set_defaults(rw = False)
    opts, args = parser.parse_args(argv[1:])
    
    if len(args) > 1:
        parser.error("Only one database can be specified on the command line (see --help).")

    mw = MainWindow()
    mw.setDefaultDatabases(getStandardConnectionStrings())
    
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
