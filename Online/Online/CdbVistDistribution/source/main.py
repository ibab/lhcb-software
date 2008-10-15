import sys
from qt import *
from mainWindow import *

##########################################################################
# Application class
##########################################################################

class cdbVis(QApplication):
    """This class represents the application CdbVis. 
    Opens the mainWindow frame and shows it as the program window.
    """
    def __init__(self,args):
		QApplication.__init__(self,args)
		##self.connect(self, SIGNAL("aboutToQuit()"), self.quit);  # TODO : print the recover file before exit !!
		#splash screen
		splash=QSplashScreen(QPixmap.fromMimeSource("lhcbdetector.png"))
		splash.show()
		splash.message("Loading CdbVis ...",Qt.AlignRight | Qt.AlignTop, Qt.white)
		self.frame = mainWindow(self,self,"frame")
		self.frame.setGeometry(200,200,800,600)
		self.setMainWidget(self.frame)
		self.frame.show()
		splash.finish(self.frame)
		splash=None
		self.setCursorFlashTime(2)
		self.exec_loop()

#Main method
#Execute the cdbVis class
if __name__ == '__main__':
	#import pychecker.checker
	#os.environ['PYCHECKER'] = 'cdbVis.py'
	app = cdbVis(sys.argv)

 

