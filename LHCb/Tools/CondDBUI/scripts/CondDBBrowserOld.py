#!/usr/bin/env python

import sys
import re
from PyQt4 import Qt
import CondDBUI.guiwin as guiwin
import CondDBUI

_allowed_partitions = [ "DDDB", "LHCBCOND", "SIMCOND", "ONLINE" ]

#############################
#     General Variables     #
#############################
versionNumber = '$Name: not supported by cvs2svn $'.split()[1]
if versionNumber == "$":
    versionNumber = 'HEAD version'

versionId  = '$Id: CondDBBrowserOld.py,v 1.1 2009-12-07 21:39:43 marcocle Exp $'.split()
if len(versionId) < 4:
    versionDate = 'unknown'
else:
    versionDate = versionId[3]

def main(args):
    '''Run the Qt application.'''
    a = Qt.QApplication(sys.argv)
    a.setApplicationVersion("%s (%s)" % (versionNumber, versionDate) )
    a.setOrganizationName("LHCb")
    a.setOrganizationDomain("lhcb.cern.ch")
    
    if len(args) > 1:
        # the first argument can be the name of a partition
        if args[1] in _allowed_partitions or re.match(r"ONLINE_[0-9]{6}",args[1]):
            from Gaudi.Configuration import importOptions, allConfigurables
            importOptions("$SQLDDDBROOT/options/SQLDDDB.py")
            if args[1] in allConfigurables \
               and hasattr(allConfigurables[args[1]],"ConnectionString"):
                args[1] = allConfigurables[args[1]].ConnectionString
                print "Opening %s" % args[1]
        if len(args) > 2:
            rw = args[2].upper() == "RW"
        else:
            rw = False
        bridge = CondDBUI.CondDB(args[1],readOnly = not rw)
    else:
        bridge = None
    
    mw = guiwin.myWindow(bridge=bridge)
    mw.setGeometry(10,10,800,600)
    mw.splitter.setSizes([150, 650])
    mw.dbTable.setSizes([450, 200])
    
    a.setMainWidget(mw)
    mw.show()
    
    return a.exec_()
    
if __name__=="__main__":
    sys.exit(main(sys.argv))
