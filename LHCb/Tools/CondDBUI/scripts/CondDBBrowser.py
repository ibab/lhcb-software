#!/usr/bin/env python

import sys
import re
import qt
import CondDBUI.guiwin as guiwin
import CondDBUI

_allowed_partitions = [ "DDDB", "LHCBCOND", "SIMCOND", "ONLINE" ]

def main(args):
    '''Run the Qt application.'''
    a = qt.QApplication(sys.argv)
    
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
    
    a.exec_loop()
    
if __name__=="__main__":
    main(sys.argv)
