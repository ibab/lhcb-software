import time
import os
import string


#from wxPython.wx import *
from qt import *
from GUImainPanel import *

from cdbVisCore import *
from objectclasses import *
# from selectPanel import *
from MiniatureWindow import *
# from visWindow import *
# from EnhancedStatusBar import *
# from CreateDeviceType import *
# from CreateDevice import *
# from CreateLinkType import *
# from CreateLink import *
# from LogWindow import *
# import helper

##########################################################################
# Main Panel class
##########################################################################

# Internal window IDs
ID_SELECT_WINDOW = 5101
ID_SELECT_SASH   = 5102
ID_VIS_WINDOW    = 5103
ID_WINDOW_LEFT1  = 5104
ID_RIGHT_SASH = 5106
ID_RIGHT_PANEL = 5107

class mainPanel(GUImainPanel, dbUpdate): #(wxPanel, dbUpdate):
    """ mainPanel contains the main panel within the main window
        It inherits from wxPanel as well as from dbUpdate, so that
        database changes are propagated to sub-windows.
    """   	
    
    def __init__(self,main = None,parent = None,name = None,modal = 0,fl = 0):
        """ Constructor.

            @parent - parent window (usually mainWindow)
            @id - window id set by parent
            @main - reference to the MainWindow instance
        """
        GUImainPanel.__init__(self,main,parent,name)
	self.setOpaqueResize(True)
	self.main = main
	
        # Sash window events
	#SELECT SASH
#         EVT_SASH_DRAGGED_RANGE(self, ID_SELECT_SASH, ID_SELECT_SASH, self.OnSashDrag)
	#RIGHT SASH
# 	EVT_SASH_DRAGGED_RANGE(self,ID_RIGHT_SASH,ID_RIGHT_SASH,self.OnSashDrag)
	#WINDOW SIZE, VISUAL AREA GETS IT ALL
#         EVT_SIZE(self, self.OnSize)

        # Visualization window
#         self.visWindow = visWindow(self, ID_VIS_WINDOW, self.main)
# 	self.visWindow.show()
	#make visWindow a dropable place for objects
# 	dt2 = helper.TextDropTarget(self.visWindow,self.main)
# 	self.visWindow.SetDropTarget(dt2)
        
        # Sashwindow to separate the two main windows (vis window and selection window)
#         win = QWidget(self) # wxSashLayoutWindow(self, ID_SELECT_SASH, wxDefaultPosition, wxSize(200, 200), wxNO_BORDER | wxSW_3D)
#         win.SetDefaultSize(wxSize(240, 1000))
#         win.SetOrientation(wxLAYOUT_VERTICAL)
#         win.SetAlignment(wxLAYOUT_LEFT)
#         win.SetSashVisible(wxSASH_RIGHT, True)

        # selection-Window
#         self.selectWindow = selectWindow(win, ID_SELECT_WINDOW, self.main) #win er eg. parent
#         self.selectPanel = win

# 	self.areawindow = SystemWindow(self,self.main)

    def GetCdb(self):
        """ Return reference to the ConfDB
	
	!return - reference to the ConfDB.
	"""
        return self.GetParent().GetCdb() # same as self.main.GetCdb()
    
    def GetVisWindow(self):
        """ Return the visual window instance
	
	!return - instance of the visual window
	"""
        return self.visWindow
        

    def GetSelectWindow(self):
        """ Return the selection window instance
	
	!return - instance of the selection window
	"""
        return self.selectWindow

    def GetAreaWindow(self):
	""" Return the miniature window instance

	!return - instance of the miniature window
	"""
	return self.areawindow

    def CloseAreaWindow(self,close):
	""" Close or open the miniature window.

	@close - True if the miniature window should be closed, False if it should be shown
	"""
	
	if close:
	    self.areawindow.Show(False)
	    
	else:
	    self.areawindow.Show(True)

    def OnDbUpdate(self, evtReason, cdb):
		""" Inform other windows that the state of the database changed
            Inherited from dbUpdate interface class

	    @evtReason - constant telling current state of ConfDB (DB_DISCONNECTED or DB_CONNECTED)
	    @cdb - reference to the ConfDB class
        """
		self.selectWindow.OnDbUpdate(evtReason, cdb)
		self.visWindow.OnDbUpdate(evtReason, cdb)
#		self.areawindow.OnDbUpdate(evtReason,cdb)

    def OnSashDrag(self, event):
        """ When the sash between the visual window and the selection window is being dragged;
	the size of the two windows are being resized.
        """

        if event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE:
            self.main.ShowError("The sash was dragged out of range.",ERR_ERROR)
            return False

        eID = event.GetId()

        if eID == ID_SELECT_SASH:
            self.selectPanel.SetDefaultSize(wxSize(event.GetDragRect().width, 1000))
            self.selectPanel.Refresh()
       
	#algorithm update 
        wxLayoutAlgorithm().LayoutWindow(self, self.visWindow)
        self.visWindow.Refresh()
       

    def OnSize(self, event):
        """ Resize the windows on resize event
            The visWindow should get all the space left.
        """

        wxLayoutAlgorithm().LayoutWindow(self, self.visWindow)
	
        

