from PyQt4.QtGui import QApplication

from MainWindow import createMainWindow

#############################
#     General Variables     #
#############################
versionNumber = '$Name: not supported by cvs2svn $'.split()[1]
if versionNumber == "$":
    versionNumber = 'HEAD version'

versionId  = '$Id: Main.py,v 1.1 2009-05-29 07:43:10 marcocle Exp $'.split()
if len(versionId) < 4:
    versionDate = 'unknown'
else:
    versionDate = versionId[3]

def getStandardConnectionStrings():
    from Gaudi.Configuration import importOptions, allConfigurables
    from GaudiKernel.ProcessJobOptions import ParserError
    try:
        importOptions("$SQLDDDBROOT/options/SQLDDDB.py")
        data = {}
        for name in allConfigurables:
            if hasattr(allConfigurables[name],"ConnectionString"):
                data[name] = allConfigurables[name].ConnectionString
        return data
    except ParserError:
        return {}

def main(argv = []):
    app = QApplication(argv)
    app.setApplicationVersion("%s (%s)" % (versionNumber, versionDate) )
    app.setOrganizationName("LHCb")
    app.setOrganizationDomain("lhcb.cern.ch")
    
    mw = createMainWindow()
    mw.setDefaultDatabases(getStandardConnectionStrings())
    
    mw.show()
    return app.exec_()
