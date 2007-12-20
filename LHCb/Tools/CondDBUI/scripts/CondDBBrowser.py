#!/usr/bin/env python

import sys
import qt
import CondDBUI.guiwin as guiwin
import CondDBUI

def main(args):
    '''Run the Qt application.'''
    a = qt.QApplication(sys.argv)
    
    if len(args) > 1:
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
