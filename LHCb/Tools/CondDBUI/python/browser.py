#!/usr/bin/env python

import sys
import qt, guiwin

def main(args):
    '''Run the Qt application.'''
    a = qt.QApplication(sys.argv)

    mw = guiwin.myWindow()
    mw.setGeometry(10,10,800,600)
    mw.splitter.setSizes([150, 650])
    mw.dbTable.setSizes([450, 200])

    a.setMainWidget(mw)
    mw.show()
        
    a.exec_loop()
    
if __name__=="__main__":
    main(sys.argv) 
