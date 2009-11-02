## @file Main.py
#  @brief Main module
#
#  This module contains the "main" function, which initializes the application
#  and starts the event loop.

__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

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
    # If we can use Oracle ...
    from os import environ as env
    if 'CORAL_LFC_BASEDIR' in env and 'LFC_HOST' in env and not 'COOL_IGNORE_LFC' in env:
        # ... add Oracle connection strings explicitly
        for c in [ "CondDB/DDDB", "CondDB/LHCBCOND", "CondDB/SIMCOND", "CondDBOnline/ONLINE" ]:
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

    __versionId__  = '$Id: Main.py,v 1.6 2009-11-02 19:05:57 marcocle Exp $'.split()
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

    mw = MainWindow()
    mw.setDefaultDatabases(getStandardConnectionStrings())

    # Use the first argument as name of the database to open
    if len(argv) > 1:
        db = argv[1]
        if db in mw.defaultDatabases:
            mw.openStandardDatabase(db)
        else:
            mw.openDatabase(db)
        
    mw.show()
    
    # Execute the event loop
    return app.exec_()
