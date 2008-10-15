#################################
# Miniature window module       #
#################################
# This module and its two classes are responsible for showing the miniature window of the visual window.

from qt import *
from visWindow import *	# We need to import this module to get the rest of the imports from cdbVis. We cannot import cdbVis because this module will try to initialize the SystemWindow class before it is available


##########################################################################
# SystemWindow class							##
##########################################################################

#CONSTANTS -  for this class; GUI constants that does not make sense to collect with all the others in CdbVisCore
SYSTEM_PANEL = 6001
SYSTEM_WINDOW_HEIGHT = 200
SYSTEM_WINDOW_WIDTH = 200

class SystemWindow(QDialog ,dbUpdate): #wxMiniFrame, dbUpdate):
    """ This class is responsible for displaying the window with the Miniature Window inside; 
    	which shows the whole visual window in a miniature size (zoomed out). The window will 
	stay on top of the parent window, but it is not modal; it floats on top of it as what
	you are used to with floating tool bars.

	TODO: At the moment it does only make sense to show this window if we are in the
	device level, but at the moment it is shown for every level...but only showing objects
	when in device level.

	Inherits wxMiniFrame for the graphical display, and dbUpdate (implements the interface)
	to include the methods included in that class.
	
    """

    # Constructor
    def __init__(self, main, parent):
	""" The constructor: Initializes the System Window.

	    Parameters:
	    @self - 	the instance object of this class
            @parent -	the parent window (usually mainWindow)
            TURNING OBSELETE -> @id - 	 ID of _this_ window, that the parent sets
            @main - 	Our communication object (mainWindow)
        """
        
	# We get the size and position of the parent window, so that we can position this window
	# relative to that; we put it close to the bottom right corner of the parent window.
	parent_pos = main.pos() #main.GetPosition()
	parent_size = main.size() #main.GetSize()

	# Call the constructor to wxMiniFrame which we inherit from; so that our window can be drawn as a frame.
        #wxMiniFrame.__init__(self, parent, id, "Miniature Window",pos=wxPoint(parent_pos.x+parent_size.GetWidth()-SYSTEM_WINDOW_WIDTH,parent_pos.y+parent_size.GetHeight()-SYSTEM_WINDOW_HEIGHT),size=wxSize(SYSTEM_WINDOW_WIDTH,SYSTEM_WINDOW_HEIGHT),style=wxDEFAULT_FRAME_STYLE)
        QDialog.__init__(self, parent)

	# Set the communication object to a member variable so that it is easily accessible within every method of this class
	self.main = main

	# We fill our frame (window) with the SystemPanel class, which shows the miniature of the visual window
        self.systemPanel = SystemPanel(self,self.main)
	
	EVT_CLOSE(self,self.OnClose)

    def OnClose(self,event):
	''' We need to know when this window closes to update the view menu in the main window; 
	    remove the hatch telling us whether this window is shown or not.

	    Parameters:
	    @event - additional information sent from the close event trigger in this object
	'''

	self.main.ViewSystemsClick(False) 

    def DrawSystem(self):
	''' Whenever something is changed in Visual Window, we need to redraw the contents of 
	    the miniature window as well. This method is then called from the paint method in
	    Visual Window to inform this window that the miniature window needs a repaint. We
	    send the information further to the "real" miniature window: the system panel,
	    which actually shows the contents.
	'''
	
	self.systemPanel.DrawSystem(None,True)
                                                                                                                             
    def OnDbUpdate(self, evtReason, cdb):
        """ Whenever the status of the database connection changes, this needs to be informed
	    to "all" the classes, so it is this method that is called from its parent to tell
	    this. We need to tell the SystemPanel about this as well. At the moment we don't
	    need to take any action from this.

	    Parameters:
	    @evtReason - A constant defining what kind of database connection change that occured.
	    @cdb - Reference to the database connection object.
        """
	
        self.systemPanel.OnDbUpdate(evtReason, cdb)


##########################################################################
# SystemPanel class
##########################################################################

# Constants for SystemPanel
CLOSE_BUTTON = 101
CREATE_SUBSYSTEM = 8001
CLOSE_WINDOW = 8002
MODIFY_SUBSYSTEM = 8003
SET_AS_ACTIVE = 8004

class SystemPanel(QWidget, dbUpdate): #wxPanel,dbUpdate):
    """ This is the window in the SystemWindow that actually shows miniature versions of the 
    	visual objects.

	Inherits wxPanel which is just a widget to contain other widgets... and implements of
	course dbUpdate.
    """


    def __init__(self, main, parent):
	''' Constructor.
	
	    Parameters:
	    @parent - parent of this panel (usually SystemWindow)
	    @id     - id of this window (for widget reference), set by the parent
	    @main   - reference to the communication object (MainWindow)
	
	'''
	
	#wxPanel.__init__(self,parent,id)
	QWidget.__init__(self,parent)
	self.main = main
	
	# We use black as background colour for the miniature window; as in contrast to the 
	# white background colour of the visual window.
#	self.SetBackgroundColour(wxColour(0,0,0))

	# We set the width and height of this window in member variables to use them when we
	# draw the miniature window. They are changed if the window is resized, so that it's
	# always correct relative positioning and size of the miniature object tot the real
	# visual objects
	self.width,self.height = self.GetSize()
	self.centerX = self.width/2
	self.centerY = self.height/2

#	EVT_PAINT(self, self.DrawSystem)
#	EVT_SIZE(self,self.OnSize)

#	EVT_LEFT_DOWN(self,self.OnMouseMove)
#	EVT_LEFT_UP(self,self.OnMouseMove)
#	EVT_MOTION(self,self.OnMouseMove)
#	EVT_LEAVE_WINDOW(self,self.OnLeftWindow) # Whenever the cursor leaves the window...

	# Member variables needed to keep track of the red rectangle in our window; which
	# actually shows the shown part of the visual window.
	self.zoomarea = None 	# The rectangle object, size and position
	self.dragMode = -1	# Whether we drag (mousebutton down) or move (mousebutton up) while moving mouse cursor
	self.moveOriginX = -1   # Origin X of rectangle's last position
	self.moveOriginY = -1	# Origin Y of rectangle's last position
	self.movedX = -1	# Dragged X from rectangle's last position
	self.movedY = -1	# Dragged Y from rectangle's last position

    def OnDbUpdate(self, evtReason, cdb):
    	""" Whenever the status of the database connection changes, this needs to be informed
	    to "all" the classes, so it is this method that is called from its parent to tell
	    this. We clear the miniature window. (and hide it? TBC)

	    Parameters:
	    @evtReason - A constant defining what kind of database connection change that occured.
	    @cdb - Reference to the database connection object.
        """

	if evtReason == DB_DISCONNECTED:
		self.DrawSystem(None,False)
	else:
		self.DrawSystem(None,True) #systems has to be redrawn each time the db is updated..

    def OnLeftWindow(self,event):
	''' Whenever the cursor leaves the miniature window; we have to change it to the normal
	    cursor again; as we may have changed it to something else when dragging or resizing.

	    Parameters:
	    @event - event object with information about the event
	'''
	self.SetCursor(wxStockCursor(wxCURSOR_ARROW))

    def OnMouseMove(self,event):
	''' When the mouse cursor is moved around in this window (on the SystemPanel, which covers the
	    whole SystemWindow), we have to detect whether the we just move the mouse cursor or if we
	    also hold down the left mousebutton, and if it is the latter one, if the mouse cursor is
	    within the red rectangle. If all those are true: the user can move the red rectangle around,
	    and when he/she releases the mouse button the view in the visual window is changed.

	    @event - event object; we make use of the x and y positions of the mouse cursor, and the
	    actions with the left mouse button (mouse down and mouse up)
	'''
	
	setscrollbars = False # We do not change the view in the visual window as default, only if
			      # the mouse button is released (mouse button up event)
			  
	# If the red rectangle object is not initialized yet, we ignore this call and return.
	if self.zoomarea == None:
		return False

	# A left mouse button down event starts the dragging (if the mouse cursor is within the red rectangle area)
	if event.LeftDown():
		mouse_x,mouse_y = event.GetX(),event.GetY()
		
		# Check if the x and y position of the mouse cursor is inside the red rectangle's area
		if mouse_x >= self.zoomarea.GetPosition().x and mouse_x <= (self.zoomarea.GetPosition().x+self.zoomarea.GetSize().GetWidth()) and mouse_y >= self.zoomarea.GetPosition().y and mouse_y <= (self.zoomarea.GetPosition().y+self.zoomarea.GetSize().GetHeight()):
			self.dragMode = 1 # All right, it was inside the red rectangle; now we start dragging.

			#Figure out where we clicked in the red rectangle, relatively to its upper left corner
			self.moveOriginX,self.moveOriginY = mouse_x,mouse_y

	if event.Dragging(): # If we hold down the mouse button while we move the mouse cursor, we are dragging.
			     # We then need to recalculate the new position of the red rectangle in the miniature 
			     # window as we drag
		if self.dragMode == 1: # Dragging started all right.
			mouse_x,mouse_y = event.GetX(),event.GetY()
			
			rel_x = mouse_x - self.moveOriginX
			rel_y = mouse_y - self.moveOriginY

			self.moveOriginX,self.moveOriginY = mouse_x,mouse_y
			self.zoomarea.SetRelativePosition(wxPoint(rel_x,rel_y))

			print "X: " + str(mouse_x) + " Y: " + str(mouse_y)

	if event.LeftUp(): # We released the mouse button, thus dragging stopped (if any), and we then need to recalculate
			   # the new position of the scroll bars in the visual window according to the new placement of the
			   # red rectangle in the miniature window
		if self.dragMode == 1:
			self.dragMode = -1
			setscrollbars = True # Oh yeah, now we need to change the visual window view

	if not (event.LeftDown() or event.Dragging() or event.LeftUp()):
		pass # None of the events that we were looking for occured...here we can optionally change the mouse cursor..
	else:
		self.DrawSystem(None,False,setscrollbars) # Ok, a mouse event of our taste occured; action -> reaction
							  # draw the new stuff.

    def DrawSystem(self,event,donotchangearea=True,setScrollBars=False):
	""" This is the paint method for this class disguised as a method to draw the system 
	    in a miniature view. This method can be called from the system (paint event) or 
	    an user (if we just need to repaint the window of some reason).

	    Parameters:
	    @event 	     - not used
	    @donotchangearea - (default: True), if we scroll on the scroll bars in the visual window,
	    			then we would like to update the relative position of the red rectangle
				to the rest of the SystemPanel as well. Only set when the scroll bars 
				change in the visual window by the user or by code.
	    @setScrollBars   - (default: False), whether we should update the scroll bars of
	    			the visual window or not (only on mouse button up after a drag
				of the red rectangle)
	"""

	# We have to calculate the proportion between the size of the miniature window and the 
	# visual window (the whole visual window with the hidden part included) so that we can
	# draw everything else in the correct proportions as well
	zoomwin_size = self.GetSize() # Size of the SystemPanel (miniature window)
	viswin_size = self.main.GetVisWindow().GetSize() # Size of the Visual Window, the visible part
	height_factor = float(zoomwin_size.GetHeight())/float(viswin_size.GetHeight())
	width_factor = float(zoomwin_size.GetWidth())/float(viswin_size.GetWidth())

	viswin_maxwidth,viswin_maxheight = self.main.GetVisWindow().GetMaxSize() # Size of the Visual Window, the whole thing
	viswin_pos_x,viswin_pos_y = self.main.GetVisWindow().CalcUnscrolledPosition(0,0) # Get current scroll position

	height_max_factor = float(zoomwin_size.GetHeight())/float(viswin_maxwidth)
	width_max_factor = float(zoomwin_size.GetWidth())/float(viswin_maxheight)

	visobjects = self.main.GetVisWindow().GetContents()
	#selobjects = self.main.GetVisWindow().GetSelection() # If we want to draw the selected visual 
							      # objects with another colour etc, it's accessible


	if donotchangearea:
		if self.zoomarea == None:
			self.zoomarea = RectangleArea(wxPoint(viswin_pos_x*width_max_factor,viswin_pos_y*height_max_factor), wxSize(viswin_size.GetWidth()*width_max_factor,viswin_size.GetHeight()*height_max_factor))
		else:
			self.zoomarea.SetPosition(wxPoint(viswin_pos_x*width_max_factor,viswin_pos_y*height_max_factor))
			self.zoomarea.SetSize(wxSize(viswin_size.GetWidth()*width_max_factor,viswin_size.GetHeight()*height_max_factor))

	else:
		# Check if we tried to move the red rectangle out of the window, if so, set new position to edge of area
		if self.zoomarea.GetPosition().x + self.zoomarea.GetSize().GetWidth() > self.GetSize().GetWidth():
			self.zoomarea.SetX(self.GetSize().GetWidth()-self.zoomarea.GetSize().GetWidth())
		elif self.zoomarea.GetPosition().x < 0:
			self.zoomarea.SetX(0)
		if self.zoomarea.GetPosition().y + self.zoomarea.GetSize().GetHeight() > self.GetSize().GetHeight():
			self.zoomarea.SetY(self.GetSize().GetHeight()-self.zoomarea.GetSize().GetHeight())
		elif self.zoomarea.GetPosition().y < 0:
			self.zoomarea.SetY(0)

	if setScrollBars: #set scrollbars in visual window to the area the user changed to in this window
		new_vis_x = self.zoomarea.GetPosition().x / width_max_factor
		new_vis_y = self.zoomarea.GetPosition().y / height_max_factor
		self.main.GetVisWindow().DoScroll(new_vis_x,new_vis_y) #upper left corner

	if(event==None):
		dc = wxClientDC(self) # We explicitly requested an update
	else:
		dc = wxPaintDC(self)  # The system thought that we would have need of an update

	dc.BeginDrawing()
	dc.SetPen(wxBLACK_PEN) # Line colour
	dc.SetBrush(wxBLACK_BRUSH) # Fill colour
	
	dc.DrawRectangle(0,0,zoomwin_size.GetWidth(),zoomwin_size.GetHeight()) # Whole area, covers whole SystemPanel
	
	dc.SetPen(wxRED_PEN)

	# Draw the red rectangle
	position = self.zoomarea.GetPosition()
	size = self.zoomarea.GetSize()
	dc.DrawRectangle(position.x,position.y,size.GetWidth(),size.GetHeight())

	# Draw miniature versions of objects (nodes/devices only) in visual window; in our miniature window
	for obj in visobjects:
		if obj.GetType() == obj_NODE: #only for nodes in device level (-1)
			dc.SetPen(wxPen(wxColour(255,255,0),width=1,style=wxSOLID)) # Yellow
			dc.SetBrush(wxBrush(wxColour(255,255,0),wxSOLID))
			x,y = obj.GetPosition()
			w,h = obj.GetSize()

			# If the miniature versions of our visual objects are to small, then we set a minimum size
			# of the miniature objects so that they are always visible for the user (if he/she is not blind)
			if w*width_max_factor < 1:
				w = 1
			else:
				w = w*width_max_factor
			if h*height_max_factor < 1:
				h = 1
			else:
				h = h*height_max_factor

			dc.DrawRectangle(x*width_max_factor,y*height_max_factor, w, h) # Draw the node!

	dc.EndDrawing()

    def OnSize(self,event):
	""" If we resize the miniature window, we redraw the contents with the new proportion 
	    factor to the visual window.
	"""
	
	self.width,self.height = self.GetSize()
	self.centerX = self.width/2
	self.centerY = self.height/2
	
    	self.DrawSystem(None,donotchangearea=True,setScrollBars=False) # Needed because it is not intelligent enough to tell
								       # that a resize needs a repaint...true enough.

class RectangleArea:
	""" An object to store size and position of the red rectangle (which represents the visible visual window)
	"""
	def __init__(self,position,size):
		""" Constructor.
		    Parameters:
		    @position - x and y position of the object in a wxPoint() object
		    @size     - width and height of the object in a wxSize() object
		"""
		self.size = size
		self.position = position

	def GetSize(self):
		""" Get the size of the rectangle.
	 	    !returns a wxSize() object with width and height of the rectangle
		"""
		return self.size
	
	def SetSize(self,size):
		""" Set the size of the rectangle.
		    Parameters:
		    @size - wxSize(w,h) object
		"""
		self.size = size
		
	def GetPosition(self):
		""" Get the position of the rectangle.
		    !returns a wxPoint() object width x and y position
		"""
		return self.position
	
	def SetPosition(self,position):
		""" Set the absolute position of the rectangle.
		    Parameters:
		    @position - wxPoint(x,y) object
		"""
		self.position = position
		
	def SetRelativePosition(self,position):
		""" Set the relative position of the rectangle.
		    Parameters:
		    @position - wxPoint(x,y) object
		"""
		self.position = wxPoint(self.position.x + position.x,self.position.y + position.y)
		
	def SetX(self,x):
		""" Set the absolute x position of the rectangle.
		    Parameters:
		    @x - x position of the rectangle
		"""
		self.position = wxPoint(x,self.position.y)
		
	def SetY(self,y):
		""" Set the absolute y position of the rectangle.
		    Parameters:
		    @y - y position of the rectangle
		"""
		self.position = wxPoint(self.position.x,y)

