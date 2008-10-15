import copy 			#copy module to deep and shallow copy objects
from math import * 		#Math module, sin, cos etc.

# from wxPython.wx import *	#Widget library
from qt import *
from qtcanvas import *

from cdbVisCore import *	#Constants
from objectclasses import *	#data objects
import pdb
import random
import math
import gc
import sys
import inout
import time
import threading

#########################################
# GLOBALS				#
#########################################
zoom_factor = 100 		#global variable defining the current zoom
activeItem = 0
timer = None

class mySelecter(QCanvasRectangle):
	""" This class define the rectangle drawn when moving the mouse with de Shfit button pressed. It allows the user to select multiple devices"
	"""
	def __init__(self, x,y,w=1,h=1,canvas=None):
		QCanvasRectangle.__init__(self,x,y,w,h,canvas)
		self.setPen( QPen(QColor(0,0,0),1,Qt.DotLine) )
	def boundingRect(self):
		rect = QCanvasRectangle.boundingRect(self)
		return QRect(rect.x()-5,rect.y()-5,rect.width()+5,rect.height()+5)
		
class waiter(QDialog):
	""" This class should provide a Message box telling the user that the application is busy.
		The waiter class also execute the function which is time consuming.
		This class is still not proprely used in the program"""
	def __init__(self,parent,fct):
		QDialog.__init__(self,parent,"Waiter",0,0)
		self.image1 = QPixmap()
		self.image1.loadFromData(bigClock_data,"PNG")
		self.setName("Please Wait")
	        self.setIcon(self.image1)
	        Form1Layout = QGridLayout(self,1,1,11,6,"Form1Layout")
	        self.textLabel1 = QLabel(self,"textLabel1")
	        Form1Layout.addWidget(self.textLabel1,0,0)
	        self.pixmapLabel3 = QLabel(self,"pixmapLabel3")
	        self.pixmapLabel3.setSizePolicy(QSizePolicy(0,0,0,0,self.pixmapLabel3.sizePolicy().hasHeightForWidth()))
	        self.pixmapLabel3.setPixmap(self.image1)
	        self.pixmapLabel3.setScaledContents(1)
	        Form1Layout.addWidget(self.pixmapLabel3,0,1)
		
		self.setCaption("Please wait ...")
		self.textLabel1.setText("<h2>Processing ...</h2>")
	        self.resize(QSize(209,86).expandedTo(self.minimumSizeHint()))
	        self.clearWState(Qt.WState_Polished)		
		self.fct=fct

	def execute(self):
		self.fct() #exec(callback.fct)

def GetZoomFactor():		#global method to get the current zoom from another module
	return zoom_factor
def SetZoomFactor(value):	#global method to set the current zoom from another module
	global zoom_factor
	zoom_factor = value
	visWindow.main.myStatusbar.zoom.setText(str(int(zoom_factor))+" %")
def setActiveItem(item):
	global activeItem
	if(item!=activeItem):
		activeItem = item
def GetActiveItem():
	return activeItem
	
def distPoint2Line(sp,ep,mp):
	"""  Return the distance between the point mp(x,y) and the line given by the two points  sp=(sx,sy) and ep=(ex,ey)
	     this function is used wich link to select when the user click on the canvas.
	"""
	# METHOD 1 :
	# v = (ep.x()-sp.x(), ep.y()-sp.y());
	# w = (mp.x() - sp.x(), mp.y() - sp.y())
	# c1 = v[0]*w[0] + v[1]*w[1] #dotProduct(w,v);
	# c2 = v[0]*v[0] + v[1]*v[1] #dotProduct(v,v);
	# b = float(c1)/float(c2);
	# tmp = QPoint((v[0]*b)+sp.x(), (v[1]*b)+sp.y()); # ((v.x()*b)+sx, (v.y()*b)+sy)
	# dist1 = math.sqrt( (mp.x()-tmp.x())*(mp.x()-tmp.x()) + (mp.y()-tmp.y())*(mp.y()-tmp.y()) ) #distanceBetweenPoints(mp, tmp);
	
	#METHOD 2:
	x,y,sx,sy,ex,ey = mp.x(),mp.y(),sp.x(),sp.y(),ep.x(),ep.y()
	if sx == ex :
		return abs(x-sx)
	# calculating the parameters of the characteristic equation :  a*x + b*y + c = 0  of the line that pass through the two points : sp and ep
	b = -1
	a =  (sy-ey)/float(sx-ex)
	c =  ey - a*ex
	return abs(a*x+b*y+c)/sqrt(float(a*a+b*b))

#########################################
# CLASS : visWindow			#
#########################################

class visWindow(QCanvasView, dbUpdate): #(wxScrolledWindow, dbUpdate):
    """ This class provides a frame/window in which the devices saved in the configuration database are
        visually displayed. Methods for the visualization window consists of display methods for the objects.

	This class inherits from the wxScrolledWindow class, which is a class for displaying information in a DC with scrollbars on the sides
	This class extends from the dbUpdate interface, onDbUpdate method must be implemented so that we know whenever a dbupdate occurs
    """
    contents = []
    def __init__(self,main,canvas,parent,name="my CanvasView",flags=0):
        """ Constructor.
		@parent - name of the parent to visWindow, in our window it is the main panel in cdbVis module
		@id     - id of this window, only used for event handling in main panel
		@main   - a reference to the main window where we have most of the communication through to the other objects and modules
        """
        # wxScrolledWindow.__init__(self, parent, id, style = wxSUNKEN_BORDER) #Initializing the super class constructor with our data + set window style
 	QCanvasView.__init__(self,canvas,parent,name,flags)
	canvas.resize(5000,5000) # Standard/default size of the visible visual window
	self.dirtyCanvas = QCanvas(200,200)
	self.main = main
	visWindow.main = main
	self.canvas = canvas
        self.maxWidth, self.maxHeight = 5000, 5000 	# This is the size of the whole visual area, including what we must scroll to see
        self.centerX = self.maxWidth/2			# Center in the x-direction
        self.centerY = self.maxHeight/2			# Center in the y-direction
        self.scrollUnit = 1				# Distance (in pixels) we will move in the given direction when clicking on the arrow on the scrollbar

        self.setSizePolicy(QSizePolicy(0,0,0,0,self.sizePolicy().hasHeightForWidth()))
	QScrollView.setDragAutoScroll(self,True)
	# self.viewport().setMouseTracking(TRUE) ???
        self.viewMode = VIEW_CIRCLE			# Our own defined layout mode, make the nodes surround the "parent" node like a horse shoe
        self.xScale = 120				# Distance in x-direction for layout of our nodes
        self.yScale =  80				# Distance in y-direction for layout of our nodes
        self.radius = 200				# Radius of the horseshoe/circle for the "child" nodes around a node
        self.__absY = {} 				# We divide the visual window in several Y-layers (from top to bottom), starting with layer 0 in the center. This variable keeps track of nr of objects in each "layer" at all times
 

        self.curTool = None					# Keeps track of the current tool we are using, we have two different ones, one for navigation/selection and one for creation
# 	self.SetCurrentTool(tool_SELECT) 			# Navigation mode is default

        self.selection = []     				# Contains all the DrawingObject instances that are selected in our visual window
        self.dragMode  = drag_NONE 				# Current mouse-drag mode. Whether we are resizing, moving, selecting objects etc

        self._AdjustMenus()					# (Turning obsolete) Adjust popup menu to content and objects

        self.penColour  = Qt.black				# Set current line colour, for drawing lines of objects
        self.fillColour = Qt.white 				# Set current fill colour, for drawing fill of objects
        self.lineSize   = 1					# Set current line size (width)

	self.zoom = 100						# Set current zoom; 100% is of course default
	self.Center()						# We center the scroll bars

	self.missing_parentnodes = []				#In undo and redo, if a link is added back to the visual window before the node it was connected to, we add the names of the devices it was connected to in this list, so when we add the devices we first check this list if there are any links we should connect to

	self.prevseldragobj = None				# For use with dragging a link to a new device, the previous device the link was dragged over is stored here, to cope with the line colouring of the selected device
	self.startpointsel = False				# For use with dragging a link to a new device, the start point

	self.levelzoom = 0					# We start with zoom level 0, which is the view of the whole LHCb detector
	
	pic=QPixmap("lhcbdetector.png")
	self.label=QLabel(None)
	self.label.setPixmap(pic)
	self.label.setGeometry(0,0,670,365)
	
	self.mem_image = None					# We store the bitmap image of the loaded image of the LHCb detector, we do not load it on every paintevent as that would be terribly inefficient + flickering
	self.prevsystemover = ""				# Name of the previous system we had our mouse cursor moved over in zoom level 0
	self.levelselections = {}				# We store a "history" of devices as we go from level 0 and downwards, so that we can use the up arrow to go back again but also to fetch information about the level in other modules
	self.macroobjects = []					# Similar to the list storing the visual objects, this one stores generic objects in the different levels
	self.chamber_found_at_location = False			# Variable to store whether we found a chamber at a given location in a quadrant or if it was not associated to any yet.
	
	self.autozoom = True # Whether contents should be zoomed to fit window or not
	self.vw_updated = True # Whether we just updated vw with new contents
	
	self.__moving=0
	self.__moving_start= 0
	self.pendingItem = 0
	self.activeItem = 0  # IS NOT USED : to Remove
	self.lastPos = 0
	self.minZ, self.maxZ = 0, 20
	
	self.inOutPutDialog = None
	i = 0
	self.linkConnect = QCanvasLine(self.canvas) 
	self.selecter = mySelecter(0,0,1,1,self.canvas)
	self.keptItems = []

#     ####################################
#     #           METHODS                #
#     ####################################
    def ZoomIn(self,factor=-1,default=False):
	wm=QWMatrix()
	prevZoom = GetZoomFactor()/100
	prevw = self.width()/2
	prevh = self.height()/2
	if default:  # Set zoom to 100%
		newZoom = 1.0
	else:
		if factor == -1:
			factor = 1.5
		newZoom = factor*prevZoom
		wm.scale( newZoom, newZoom)
	self.setWorldMatrix( wm )
	SetZoomFactor(newZoom*100)
	self.setContentsPos(newZoom*2500-prevw,newZoom*2500 - prevh)
	#self.setContentsPos(newZoom*self.contentsX()-prevw,newZoom*self.contentsX() - prevh)

    def ZoomOut(self,factor=-1,default=False):
	wm=QWMatrix()
	prevZoom = GetZoomFactor()/100
	prevw = self.width()/2
	prevh = self.height()/2
	if default:  # Set zoom to 100%
		newZoom = 1.0
	else:
		if factor == -1:
			factor = 2/3.0
		newZoom = factor*prevZoom
	wm.scale( newZoom, newZoom)
	self.setWorldMatrix( wm )
	SetZoomFactor(newZoom*100)
	self.setContentsPos(newZoom*2500-prevw,newZoom*2500 - prevh)

    def keep(self):
	if self.__moving:
		self.keptItems = [self.__moving.GetObject()] #+self.__moving.GetPortList(True)+self.__moving.GetPortList(False)
		print self.__moving.GetObject().GetName()
	
    def contentsContextMenuEvent(self,e):
	if self.selection:
		contextMenu = QPopupMenu(self.main)
		contextMenu.insertItem(QString(self.__moving.GetName()))
		contextMenu.insertSeparator()
		self.main.aModify.addTo(contextMenu)
		if self.keptItems==[]:
			self.main.aKeep.addTo(contextMenu)
		contextMenu.insertSeparator()
		self.main.aDelete.addTo(contextMenu)
		contextMenu.exec_loop(e.globalPos())
	elif self.main.GetActiveLink():
		contextMenu = QPopupMenu(self.main)
		contextMenu.insertItem(QString("Link : "+self.main.GetActiveLink().GetObject().GetName()))
		contextMenu.insertSeparator()
		self.main.aModify.addTo(contextMenu)
		contextMenu.insertSeparator()
		self.main.aDelete.addTo(contextMenu)
		contextMenu.exec_loop(e.globalPos())

    def noMoveEventAfter(self):
	for item in self.selection:
		item.setActive(False)
	self.selection = [self.__moving]
	self.__moving.setActive(False)
			
    def contentsMousePressEvent(self,e): # QMouseEvent e
	point = self.inverseWorldMatrix().map(e.pos())
	i=0
	self.__moving = None
	self.__moving_start=point
	links_selected = []

	ilist = self.canvas.collisions(point) #QCanvasItemList ilist
	# Preselection of the links on wich we might have clicked
	for each_item in ilist:
		if isinstance(each_item,myLine):
			links_selected.insert(0,each_item)

	for each_item in ilist:
		#if each_item.rtti()==1001:
		if isinstance(each_item,Box):  # I had problems while using rtti function on box
			self.__moving=each_item
			self.main.GetSelectWindow().UpdateInfo(each_item.GetObject())

			if self.main.aCreateLinkMode.isOn(): # A remplacer avec la condition que je veux !! if mode creation connexion ON ?
				self.linkConnect.setPoints(point.x(),point.y(),point.x(),point.y())  
				self.linkConnect.setPen(QPen(Qt.blue,3))
				self.linkConnect.show()

			if e.state()==Qt.ControlButton:
				if each_item in self.selection:
					self.selection.remove(each_item)
					each_item.setActive(False)
				else:
					self.selection.insert(0,each_item)
					each_item.setActive(True)
			else:
				# ......................
				if each_item in self.selection:
					global timer
					timer = QTimer(self)
					self.connect(timer,SIGNAL("timeout()"),self.noMoveEventAfter)
					timer.start( 100, True )
				else:
					timer = None
					if isinstance(self.main.GetActiveDevice(),Box):
						self.main.GetActiveDevice().setActive(False)
					self.selection = [each_item]
					each_item.setActive(True)
					self.main.SetActiveDevice(each_item)
					# deselect the active Link too:
					if self.main.GetActiveLink():
						self.main.GetActiveLink().setActive(False)
					self.main.SetActiveLink(None)
			self.canvas.update() #To redraw the activated device
			return

	if e.state() == Qt.ShiftButton and not self.__moving:
		self.selecter.setSize(1,1)
		self.selecter.move(point.x(),point.y())
		self.selecter.show()
		return

	# deselect the active Link too:
	if self.main.GetActiveLink():
		self.main.GetActiveLink().setActive(False)
	self.main.SetActiveLink(None)
	for item in self.selection:  
		item.setActive(False)
	self.selection = []
	if self.__moving:
		self.selection=[self.__moving]
		self.__moving.setActive(True)
		
	# Now we define wich link to select
	dist = 3
	link_selected=None
	for link in links_selected: #links_selected:
	   if isinstance(link,QCanvasLine):
		dl = distPoint2Line(link.startPoint(), link.endPoint(), point)
		if dl < dist:
			dl =dist
			link_selected = link
	if link_selected:
		self.main.panel.selectWindow.UpdateInfo(link_selected.object)
		if isinstance(self.main.GetActiveLink(),myLine):
			self.main.GetActiveLink().setActive(False)
		self.main.SetActiveLink(link_selected)
		link_selected.setActive(True)
		print link_selected.GetName()
	self.__moving_start=point
	self.__moving=0
	
	self.canvas.update()  # To refresh the activated items
		
    def contentsMouseDoubleClickEvent(self, event):
	point = self.inverseWorldMatrix().map(event.pos())
	ilist = self.canvas.collisions(point) #QCanvasItemList ilist
	tmp = False
	for each_item in ilist:
		if not ( isinstance(each_item,QCanvasText) or isinstance(each_item,QCanvasLine) ):
			tmp = True
			break
	if tmp:
		self.OnDeviceSelected(each_item.object)
		#self.main.panel.selectWindow.UpdateInfo(device)
	if event.button()==2:
		pdb.set_trace()


    def contentsMouseMoveEvent(self,e):
        point = self.inverseWorldMatrix().map(e.pos());
	global timer
	if timer:
		timer.stop()
		timer = None
		print "timer stopeed"
	if self.selecter.isVisible():
		self.selecter.setSize(point.x()-self.__moving_start.x(),point.y()-self.__moving_start.y())
	elif self.linkConnect.isVisible():
		self.linkConnect.setPoints(self.linkConnect.startPoint().x(),self.linkConnect.startPoint().y(),point.x(),point.y())
		self.canvas.update()
		return
	elif self.selection:
	    for item in self.selection:
		item.moveBy(point.x() - self.__moving_start.x(),point.y() - self.__moving_start.y())
            self.__moving_start = point
        else:
		self.setCursor(QCursor(Qt.PointingHandCursor))
		#self.scrollBy((-point.x() + self.__moving_start.x()),(-point.y() + self.__moving_start.y()))
		self.scrollBy(GetZoomFactor()/100*(-point.x() + self.__moving_start.x()),GetZoomFactor()/100*(-point.y() + self.__moving_start.y()))
        self.canvas.update()
    def contentsMouseReleaseEvent(self,e):
	self.unsetCursor()
	if self.selecter.isVisible():
		for item in self.selecter.collisions(True):
			if isinstance(item,Box):
				self.selection.append(item)
				item.setActive(True)
		self.selecter.setVisible(False)
		
	if self.linkConnect.isVisible():
		point = self.inverseWorldMatrix().map(e.pos())
		ilist = self.canvas.collisions(point)
		for item in ilist: # We keep only Devices in the list
			if isinstance(item,Box) and item.GetObject().GetName()!=self.__moving.GetObject().GetName(): 
				self.main.CreateLinkClick(self.__moving.GetObject(),item.GetObject())
				self.main.aCreateLinkMode.setOn(False)
				break;
		self.linkConnect.setVisible(False)
	self.canvas.update()


    def EnableAutoZoom(self,enable=True):
		if(OFF):
			pass
		self.autozoom = enable

    def ResetMacroObjects(self):
		self.macroobjects = []

    def OnScroll(self,event=None):
	''' Called on scrollevent, will redraw the visible regionarea in the zoomarea/miniaturearea window '''
	self.main.GetAreaWindow().DrawSystem()
	
    def GetMaxSize(self):
	''' Get the size of the whole visual window, also what we don't see if we do not scroll'''
	return self.maxWidth,self.maxHeight

    def IsChamberFoundAtLocation(self):
	''' Returning value of the variable telling whether a chamber was found at a given location or not, MUON specific. '''
	return self.chamber_found_at_location

    def SetZoomFactor(self,value):
	''' Set the zoom factor, when changed by user or code. 
	
	@value - zoom value in percent.
	'''
	SetZoomFactor(value) # Set global zoomfactor variable
	self.zoom = value

	 
    def GetAbsY(self, position):
        """ Return absolute number of objects in layer position.
            Absolut amount of objects on every 'layer' is saved to position the different objects in
            this layer proerly. Layers can be seen as horicontically divided reions of the screen.
            Layer 0 is in the middle, the positive axes goes downwards. The absoltue amount must be set
            before elements are positioned in this layer so that they are arranged properly.

	    @position - id of Y layer

	    !return - number of objects in given Y layer
        """
        return self.__absY.get(position, 0) # return total amount, if nothing is saved, 0 is returned.

    def SetAbsY(self, position, value):
        """ Set absolute number of objects on layer position.
            See GetAbsY, too.

	    @position - id of Y layer
	    @value - number of objects in given layer
        """
        self.__absY[position] = value

    def ResetAbsY(self):
        """ Reset absolute number of all layers to 0.
            See GetAbsY, too
        """
        self.__absY.clear()

    def Clear(self):
        """ Clear the frame and delete all visual objects on it
	"""
        self.DoDeleteAll(refresh=True)

   
    def AddToContents(self,obj,paintevent=True):
	''' If visual objects is stored with the undo/redo information, they can easily be added back to the visual window on undo/redo. This is for nodes only, as the links are to be connected to the nodes once they are added.

	@obj - object that will be added to the visual window
	@paintevent - whether we manually should trigger the paint event or not
	'''

	#WAL_URG visWindow.contents.append(obj)

	if obj.GetType() == obj_NODE:
		# Copy information of links connected to this node, deep copy, cause we are clearing the port list right after
		tmp_portinlist = obj.GetPortList(port_IN)[:]
		tmp_portoutlist = obj.GetPortList(port_OUT)[:]

		obj.ClearPorts()

		# 1. If only the node was undone/removed, and the links are still in the visual window (obsolete? since links cannot exist withour 2 parent nodes?)

		# Then we add the links connected to the inports of this node, if any
		for port in tmp_portinlist:
			if port != None:
				tmp_link = self.FindByName(port.GetName())
				if tmp_link:
					self.CreateConnectedLink(tmp_link.GetObject())

		# Then we add the links connected to the outports of this node, if any
		for port in tmp_portoutlist:
			if port != None:
				tmp_link = self.FindByName(port.GetName())
				if tmp_link:
					self.CreateConnectedLink(tmp_link.GetObject())


		# 2. If we added a link to the visual window and the parent node was not here
		i = 0
		for itm in self.missing_parentnodes: #missing_parentnodes is a list of lists, each sublist containing to entries: [0]:link_name and [1]:node_name
			if itm[1] == obj.GetName():
				tmp_link = self.FindByName(itm[0])
				if tmp_link:
					self.CreateConnectedLink(tmp_link.GetObject())

				self.missing_parentnodes.pop(i)
			i += 1

	if paintevent:
		self.paintevent(None)
	
    def AddNode(self,object,index=-1,portnumber=0,portIN=True):
	"""
	index = -1 for central device
	
	"""
	deviceInfo = object.GetObjectInfo(True)
	node = self.FindByName(deviceInfo[DEV_NAME])
	number_created = 0
	if not node:
		node = self.CreateNode(0, 0, object,True)
		number_created = 1
		
	if index == -1:
		node.move(2500,2500)
		node.show()	
		return (node, number_created)
		
	if self.main.GetViewMode() == ID_PATHS_MODE:
		node.move(2500,2500-40*index)
		node.show()	
		return (node, number_created)
		
	parent_x, parent_y = 2500, 2500
	teta = pi/(portnumber+4)
	index += 2
	# x = parent_x - 300*math.cos(index*teta)
	# if portIN:
		# y = parent_y - 200*math.sin(index*teta)  # put minus if input and plus if output 
	# else:
		# y = parent_y + 200*math.sin(index*teta)
	cosinus, sinus = math.cos(index*teta),math.sin(index*teta)
	x_factor = 200*cosinus
	y_factor = 100*sinus
	x_delta = 0.1*x_factor
	y_delta = 0.1*y_factor
	x =  - x_factor + parent_x
	if portIN:
		y = - y_factor + parent_y
	else:
		y = y_factor + parent_y	
	node.move(x,y)
	node.show()
	while([elt.__class__ for elt in node.collisions(False)].count(QCanvasText)>1):
		
		x_factor += x_delta
		y_factor += y_delta

		x =  - x_factor + parent_x
		if portIN:
			y = - y_factor + parent_y
			# node.move(x_factor - parent_x,y)
		else:
			y = y_factor + parent_y	
			# node.move(x_factor - parent_x,y)
		node.move(x,y)
	
	
	return (node, number_created)

    def Center(self):
        """ Focus on the center of the visualization window. The scroll bars are moved so that the
	center of the whole visual window is shown.	
	"""
	
	scroll_x = self.maxWidth/2 - self.width()/2
	scroll_y = self.maxHeight/2 - self.height()/2
	self.setContentsPos(scroll_x,scroll_y) #self.Scroll(scroll_x,scroll_y)

    def CenterToObject(self,obj):
        """ Center the shown visual window so that the object in the parameter list is in center.
	The centering is done by scrolling the scrollbar to a position such that the object is in
	the center of the visual window.

	@obj - object which we will center to.
	"""
	self.setContentsPos(GetZoomFactor()*25-self.width()/2,GetZoomFactor()*25-self.height()/2)
	return
	# Find size of object, to be changed as we will have to limit max-size, 
	# and also consider size of the name of the object
	pin = obj.GetPorts(port_IN)
	pout = obj.GetPorts(port_OUT)
	maxp = 0
	if pin > pout:
		maxp = pin
	else:
		maxp = pout
	if maxp < 5:
		maxp = 5 #minimum size
	if maxp > 100:
		maxp = 100 #maximum size
			
	new_height = NODE_HEIGHT
	new_width = NODE_WIDTH * maxp
	if maxp >= 20:
		new_height += maxp/20/10


	obj_pos = obj.GetPosition()
	
	#The object position is given using the objects position in the upper left corner.
	#The scroll scrolls to the position given in the upper right corner. Therefore we 
	#have to scroll right and down half the window size, and left and up half of the size of the object
	scroll_x = obj_pos.x() - (self.size().width()/2) + (obj.GetSize().width()/2)
	scroll_y = obj_pos.y() - (self.size().height()/2) + (obj.GetSize().height()/2)
	self.setContentsPos(scroll_x,scroll_y)

    def CenterToScreen(self):
	''' Used only in the Zoom(...) to center to the same center of the visual window as in the previous zoom. 
	So that the objects after a zoomin event have the same relative distance between themselves and the center
	of the visual window
	'''

	x, y = self.contentsX(), self.contentsY() #self.CalcUnscrolledPosition(0,0)
	x, y = x + self.GetSize().GetWidth()/2, y + self.GetSize().GetHeight()/2

	new_zoom = float(self.zoom)/100.0
	prev_zoom = float(self.centervalue)/100.0
	
	new_x = (float(x - self.maxWidth/2) / prev_zoom * new_zoom) + self.maxWidth/2
	new_y = (float(y - self.maxHeight/2) / prev_zoom * new_zoom) + self.maxHeight/2

	new_x,new_y = new_x - self.GetSize().width()/2, new_y - self.GetSize().height()/2

	new_x /= self.scrollUnit
	new_y /= self.scrollUnit

	self.setContentsPos(new_x,new_y) #self.Scroll(new_x,new_y)
   

    '''def DoScroll(self, x_upperleft,y_upperleft):
	""" Move the scrollbars (scroll), so that the x and y coordinates given
	in the parameter list is located at the upper left corner in the visible
	visual window.

	@x_upperleft - integer defining the x position in the upper left corner
	@y_upperleft - integer defining the y position in the upper left corner
	"""
	
	x_upperleft /= self.scrollUnit
	y_upperleft /= self.scrollUnit
	self.setContentsPos(x_upperleft,y_upperleft) #self.Scroll(x_upperleft,y_upperleft)

    def ScrollOutOfBounds(self):
	""" Scroll to position 0.
	"""

        self.setContentsPos(0, 0)'''

    def OnDeviceSelected(self, deviceobj,clear=True,subsystemview=False,treenumber_x=0,treenumber_y=0,refresh=True):
        """ Select a device and show the links and devices connected to it.

            The method should be called to show a device and all links connected to it including the 
            nodes connected to these links. First, it finds all these connected elements by asking the 
            confDB instance, after that, it creates the visual objects and places them on the screen.

	    @deviceobj - the device object that is selected, and that will be added to the visual window
	    with its closest neighbors (direct neighbors)
	    @clear - whether the windows should be cleared (all objects deleted)before adding the 
	    new visual objects or not (for neighbour view True, for the other views False)
	    @subsystemview - whether we add the visual objects in a subsystem view or not
	    @treenumber_x - if this is the first visual object to be added in a tree, we need the x coordinate
	    to place it away from other trees of visual objects.
	    @treenumber_y - as with @treenumber_x, but for y coordinate
	    @refresh - whether we should manually trigger a paint event or not

	    !return -
        """
	self.ZoomIn(-1,True)
	prev_zoom = 100
	##tmp = waiter(None,"WAITING FOR ON DEVICE SLETCED",300,100)
        def _CountNodes(linkList):
            """ Return the number of linked nodes of the given linkList. This is a private
	    method inside OnDeviceSelected(...) and hence not visible outside.

	    @linkList - a list of link objects for a device

	    !return - an integer telling the number of other devices this device is connected
	    to in the given link list for a device
            """


            nodes = {}
            for linkobj in linkList:
		link = linkobj.GetObjectInfo(True) # Link already loaded with data, just retrieve it
                node = link[DEV_SWITCH_FROM]	#device from
                nodes[node] = node		
                node = link[DEV_SWITCH_TO]	#device to
                nodes[node] = node
            # Return all nodes (if no node is linked, there is still the node from 
            # which we are searching!)
            return max(len(nodes), 1)
        
	
				
        viewMode = self.main.GetViewMode()
        if (viewMode == ID_SUCCESSORS_MODE) or subsystemview: # neighbor or subsystem
	    #remove all visible content in visual window
	    if clear:
		    self.DoDeleteAll(refresh=False)

	    # ADD kept object:
	    if self.keptItems: # and not visWindow.contents:
		# Move item :
		for item in self.keptItems:
			kept,node = self.AddNode(item, -1) #treenumber_x, treenumber_y,refresh=False)
			kept.move(2400,2400) # Todo : place the item in an appropriat position
			kept.show()
		self.keptItems = []
		
	    # Get input links for our current Device
	    inputLinks = deviceobj.GetLinks(False)	#Previous in path
	    if inputLinks == False:
			self.main.ShowError(str(deviceobj.GetErrorMessage()),ERR_ERROR)
			# if prev_zoom != 100:
				# self.main.OnZoom(prev_zoom,False)
			return ([],[])

	    outputLinks = deviceobj.GetLinks(True)	#Next in path
	    if outputLinks == False:
			self.main.ShowError(str(deviceobj.GetErrorMessage()),ERR_ERROR)
			# if prev_zoom != 100:
				# self.main.OnZoom(prev_zoom,False)
			return ([],[])

	    #if inputLinks == [] and outputLinks == [] and subsystemview:
	    #	return ([],[])
         
	    self.SetAbsY(0, 1) # set 1 object in layer 0
	    origShape, newNodes = self.AddNode(deviceobj)#treenumber_x, treenumber_y,refresh=False) # add the first device
	    origShape.SetExpanded(2) #Totally expanded

	    # layer and position of the "parent" device
	    ylayer_pos = origShape.GetLayer()
	    node_pos = origShape.GetPosition()
	    parent_x_pos_center = node_pos.x() + (origShape.GetSize().width()/2)

	    #max ports
	    #find whether maximum number of nodes are of in or out type, to correctly calculate the length in AddNode()
	    if (deviceobj.GetPortsOut() > deviceobj.GetPortsIn()):
		    max_ports = deviceobj.GetPortsOut()
	    else:
		    max_ports = deviceobj.GetPortsIn()
	    
	    if len(outputLinks) > 30 or len(inputLinks) > 30:   # we show a dialog containing the devices
		if not self.inOutPutDialog:
			self.inOutPutDialog = inout.Input_output(self.main,self)
		self.inOutPutDialog.clear()
		self.inOutPutDialog.setData(inputLinks,outputLinks,origShape)
		for i in range(len(outputLinks)):
			tmp_link = outputLinks[i].GetObjectInfo(True)
			self.inOutPutDialog.insertOutput(tmp_link[DEV_SWITCH_TO],i)

		j,factor = 1,1.0
		for i in range(len(inputLinks)):
			tmp_link = inputLinks[i].GetObjectInfo(True)
			self.inOutPutDialog.insertInput(tmp_link[DEV_SWITCH_FROM],i)
			
		self.inOutPutDialog.show()
	    else:
		# Add each device in output links of the current device to the visual window, both the node and the link
		position = 1
		nr_out_added = 0
		devices_on_output = {}

		for i in range(len(outputLinks)):
			tmp_link = outputLinks[i].GetObjectInfo(True)
			tmp_devobj = Device(self.main.GetActiveSystem(),tmp_link[DEV_SWITCH_TO],False)
			index,portnr = origShape.FindCorrespondingPortIndex(outputLinks[i].GetPortFrom(),outputLinks[i].GetPortTypeFrom(),port_OUT)
			port_pos = origShape.GetPortPosition(index,port_OUT)
			shape,newNodes = self.AddNode(tmp_devobj,index,len(outputLinks),False)
			
			self.CreateConnectedLink(outputLinks[i])
		# Add each device in input links of the current device to the visual window, both the node and the link
		position = 1
		nr_in_added = 0
		devices_on_input = {}
		for i in range(len(inputLinks)):
			tmp_link = inputLinks[i].GetObjectInfo(True)
			tmp_devobj = Device(self.main.GetActiveSystem(),tmp_link[DEV_SWITCH_FROM],False)
			#shape,newNodes = self.AddNode(tmp_devobj,position,-1,False,len(inputLinks),node_pos.x,node_pos.y,refresh=False) #-1 from parentnode (y layer -1)
			index,portnr = origShape.FindCorrespondingPortIndex(inputLinks[i].GetPortTo(),inputLinks[i].GetPortTypeTo(),port_IN)
			port_pos = origShape.GetPortPosition(index,port_IN)
			print tmp_devobj.GetName(),"  ---> ",index,portnr,deviceobj.GetPortsIn()
			shape,newNodes = self.AddNode(tmp_devobj,index,len(inputLinks),True)

			self.CreateConnectedLink(inputLinks[i])

		if not subsystemview:
			self.Select(origShape,refresh=False)
			self.CenterToObject(origShape)

        elif viewMode == ID_PATHS_MODE: # Path view
	    #Prepare for path view; clear if needed
	    if clear:
	            self.DoDeleteAll(refresh=False)
            self.Center()
	    inputLinks,outputLinks = [],[]
	    # do not set anything here yet, we will do that in the pathselect method when user chooses a path
	
	else: #dynamic link view
	    if clear:
	            self.DoDeleteAll(refresh=False)

            self.SetAbsY(0, 1)
            origShape, newNodes = self.AddNode(deviceobj, 0, 0,refresh=False) # add current device to visual window

	    if origShape != None: #it should have been added, this should never go wrong
		    self.ShowNeighbourLinks(origShape,origShape.GetObject(),refresh=False) #show neighbours of device
		    self.CenterToObject(origShape)
	    else:
		    self.Center()

	    if not subsystemview: # select current device
	            self.Select(origShape,refresh=False)
		    
	    inputLinks,outputLinks = [],[]
	
	# if prev_zoom != 100:
		# self.main.OnZoom(prev_zoom,False)
	
	if refresh:
		self.canvas.update() #Refresh() # in case we need to trigger paint event manually
	return (inputLinks,outputLinks)

    def OnPathSelected(self, my_device, pathNr,paintevent=True):
        """ Select a device and show a path this device participates in. The user chooses the path from
	a list in a pop-up window.
            
        This method should be called to show paths devices participate at. It first retrieves all
        the necessary information of the nodes and links in the path and after that it creates and
        positions the corresponding visual objects on the screen.

        @my_device - the current device to view paths for
        @pathNr - id number(integer) of the path to be selected in the pop-up path view window
	@paintevent - whether we should trigger paint event manually or not
        """

	path = my_device.GetSpecificPathInfo(pathNr) # get path information about the chosen path
	deviceInfo = my_device.GetObjectInfo(True)

        self.DoDeleteAll(refresh=False) # clear window

	#set up path visually
        pLen = len(path)-1
        pL = len(path)
        self.ResetAbsY()
        shapeBefore = None

	pathposition = 0.0
	index = 0
        for i in range(pL): #each node/device in path

	    if path[i]['reversed'] == True:
		device1 = path[i][DEV_SWITCH_TO]
		device2 = path[i][DEV_SWITCH_FROM]
	    else:
		device1 = path[i][DEV_SWITCH_FROM]
		device2 = path[i][DEV_SWITCH_TO]

		
	    tmp_device_1 = Device(deviceInfo[DEV_SYSTEM],device1,False)
	    shape_1, newNodes_1 = self.AddNode(tmp_device_1, index) #0, pathposition) #(i - pL/2)+1,refresh=False)
	    pathposition += 0.5; index += 1
	    print tmp_device_1.GetName()
	    
	    tmp_device_2 = Device(deviceInfo[DEV_SYSTEM],device2,False)
	    shape_2, newNodes_2 = self.AddNode(tmp_device_2, index) #0, pathposition) # i - pL/2,refresh=False)
	    pathposition += 0.5; index += 1
	    print tmp_device_2.GetName()
	    
               
	    tmp_link = Link(path[i][DEV_SYSTEM],path[i][DEV_LINK],False,False)
	    self.CreateConnectedLink(tmp_link)


        self.Select(self.FindByName(deviceInfo[DEV_NAME]),refresh=False)

	if paintevent:
		self.paintevent(None)
		
	self.canvas.update()

    def OnKeyEvent(self, event):
        """ Respond to a keypress event.

            We make the arrow keys move the selected object(s) by one pixel in
            the given direction.
        """
	
    	key = event.GetKeyCode()

        if key == WXK_UP:
            self._MoveObject(0, -1)
        elif key == WXK_DOWN:
            self._MoveObject(0, 1)
        elif key == WXK_LEFT:
            self._MoveObject(-1, 0)
        elif key == WXK_RIGHT:
            self._MoveObject(1, 0)
        else:
            event.Skip()

    def OnMouseEvent(self, event):
        """ Respond to the user clicking in our visual window.
            How we respond depends on the currently selected tool and
	    on the mouse event.

	    @event - contains information such as which mouse event and where the event
	    occured (coordinates)
        """
	
	if not self.main.GetCdb(): #not connected to db
		return False

        if not (event.Dragging() or event.LeftUp() or event.LeftDown() or event.Moving()):
	    return

	# LHCb system - our LCHb illustration image map
    	if (self.levelzoom == 0) and (event.Moving() or event.LeftUp() or event.LeftDown()): #valid mouse events
		#w_and_h = [width,height,x,y_offset_from_middle_of_window,MACRO_ID,subsystem name]
		w_and_h = [[55,25,50,-2,"","VELO"],[30,80,105,-1,"","RICH1"],[15,35,137,-1,"","TT"],[40,50,279,30,"","OT"],[40,13,279,-2,"","IT"],[40,50,279,-32,"","OT"],[60,160,319,-1,"","RICH2"],[12,140,379,-1,"1","MUON"],[6,75,391,-40,"","PRS"],[6,75,391,40,"","PUS"],[20,150,397,-1,"","ECAL"],[42,166,417,-1,"","HCAL"],[29,180,459,-1,"2","MUON"],[30,195,488,-1,"3","MUON"],[30,210,518,-1,"4","MUON"],[30,220,548,-1,"5","MUON"],[70,21,232,-155,"","ECS"],[70,20,302,-155,"","TFC"],[65,20,372,-155,"","DAQ"],[70,24,232,-131,"","L0DU"],[70,24,302,-131,"","L0MUON"],[65,24,372,-131,"","L0CALO"]]
		new_x,new_y = self.contentsX(), self.contentsY() #self.CalcUnscrolledPosition(0,0)
		my_w,my_h = self.GetSizeTuple() #width and height
		new_x = new_x + my_w/2
		new_y = new_y + my_h/2

		if self.mem_image != None: # if LHCb illustration already loaded; set up the image map
			image_w, image_h = self.mem_image.GetSizeTuple()
	
			startx = new_x-image_w/2
			starty = new_y

			mouse_x = event.GetX()
			mouse_y = event.GetY()
			mouse_x, mouse_y = self.contentsX() + mouse_x, self.contentsY() + mouse_y  #self.CalcUnscrolledPosition(mouse_x,mouse_y)

			i = 0
			oversystem = False
			for subsystem in w_and_h:
				x = startx + subsystem[2]
				y = starty - subsystem[1]/2 + subsystem[3]
				w = subsystem[0]
				h = subsystem[1]
				if mouse_x >= x and mouse_x <= x+w and mouse_y >= y and mouse_y <= y+h:
					oversystem = True
					self.SetCursor(wxStockCursor(wxCURSOR_HAND))
					if event.Moving():
						if self.prevsystemover != str(subsystem[5] + " " + subsystem[4]):
							self.prevsystemover = str(subsystem[5] + " " + subsystem[4])
							self.Refresh()
						dc = wxClientDC(self)
						dc.SetBrush(wxBrush(Qt.white, wxSOLID)) #to be changed to another colour
						self.PrepareDC(dc)
						dc.BeginDrawing()
						dc.DrawText(str(subsystem[5]) + " " + str(subsystem[4]),new_x-my_w/2,new_y-my_h/2)
						dc.EndDrawing()
					elif event.LeftUp():
						# if we click -> move to next leve in selected subsystem
						self.main.SetActiveSystem(subsystem[5],"visual") #NB! only valid systems
    						self.SetLevelSelections(str(subsystem[4]),subsystem[5],1,reset=True)
						self.SetLevelZoom(1) #to next level
					break
				i += 1
			if not oversystem or event.LeftUp():
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))

			return # Ignore mouse movement without click/drag.
	# level 1 subsystem -> only set up for muon so far. -> muon station
	# if you want to add a specific levelzoom for a subsystem, add it before the elif-statement below, at the same level
	elif self.levelzoom == 1 and self.main.GetActiveSystem() == "MUON":
		if event.LeftUp():
            		mousePt = self._GetEventCoordinates(event)

			obj = self._GetObjectAt(mousePt,True)
	            	if (obj != None):
				self.macroobjects = [] # we reset the global macro objects list
				#station = self.levelselections[self.main.GetActiveSystem()][0] #try error
    				self.SetLevelSelections(str(obj.GetMacroID()),self.main.GetActiveSystem(),2)
				self.SetLevelZoom(2) #change level
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))
		elif event.Moving():
			mousePt = self._GetEventCoordinates(event)

			obj = self._GetObjectAt(mousePt,True)
	            	if (obj != None):
        		        # The user is moving over a clickable object
				self.SetCursor(wxStockCursor(wxCURSOR_HAND))
			else:
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))

		return
	# level 2 subsystem -> muon quadrant
	elif self.levelzoom == 2 and self.main.GetActiveSystem() == "MUON":
		if event.LeftUp():
            		mousePt = self._GetEventCoordinates(event)

			obj = self._GetObjectAt(mousePt,True)
	            	if (obj != None):
				self.macroobjects = [] # we reset the global macro objects list
    				self.SetLevelSelections(str(obj.GetMacroID()),"MUON",3)
				self.SetLevelZoom(3) #change level
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))
		elif event.Moving():
			mousePt = self._GetEventCoordinates(event)

			obj = self._GetObjectAt(mousePt,True)
	            	if (obj != None):
				self.SetCursor(wxStockCursor(wxCURSOR_HAND))
			else:
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))

		return
	# level 3 subsystem -> double gap view in Muon
	elif self.levelzoom == 3 and self.main.GetActiveSystem() == "MUON":
		if event.LeftUp():
            		mousePt = self._GetEventCoordinates(event)

			obj = self._GetObjectAt(mousePt,True)
	            	if (obj != None):
				self.macroobjects = [] # we reset the global macro objects list
    				self.SetLevelSelections(str(obj.GetMacroID()),"MUON",5)
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))
		elif event.Moving():
			mousePt = self._GetEventCoordinates(event)

			obj = self._GetObjectAt(mousePt,True)
	            	if (obj != None):
				self.SetCursor(wxStockCursor(wxCURSOR_HAND))
			else:
				self.SetCursor(wxStockCursor(wxCURSOR_ARROW))

		return


	# The rest is handling mouse events in device level (level -1)
        feedbackType = feedback_RECT
        action       = self.SelectByRectangle
        actionParam  = param_RECT
        selecting    = True
        dashedLine   = True

        if event.LeftDown():
            mousePt = self._GetEventCoordinates(event)
            if selecting:
                obj, handle = self._GetObjectAndSelectionHandleAt(mousePt)

            if selecting and (obj != None) and (handle != handle_NONE) and ((self.curTool == tool_EDIT and obj.GetType() == obj_LINK) or (obj.GetType() == obj_NODE)): #only in creation mode

                # The user clicked on an objects selection handle.  Let the
                # user resize the clicked-on object.

                self.dragMode     = drag_RESIZE
                self.resizeObject = obj

                if obj.GetType() in [obj_LINE, obj_LINK]:
                    self.resizeFeedback = feedback_LINE
                    pos  = obj.GetPosition()
                    startPt = QPoint(pos.x + obj.GetStartPt().x,
                                      pos.y + obj.GetStartPt().y)
                    endPt   = QPoint(pos.x + obj.GetEndPt().x,
                                      pos.y + obj.GetEndPt().y)
                    if handle == handle_START_POINT:
                        self.resizeAnchor  = endPt
                        self.resizeFloater = startPt
			self.startpointsel = True
			self.main.ShowError("You have chosen the start point of the link.",ERR_INFORMATION)
                    else:
                        self.resizeAnchor  = startPt
                        self.resizeFloater = endPt
			self.startpointsel = False
			self.main.ShowError("You have chosen the end point of the link.",ERR_INFORMATION)
                else:
                    self.resizeFeedback = feedback_RECT
                    pos  = obj.GetPosition()
                    size = obj.GetSize()
                    topLeft  = QPoint(pos.x, pos.y)
                    topRight = QPoint(pos.x + size.width, pos.y)
                    botLeft  = QPoint(pos.x, pos.y + size.height)
                    botRight = QPoint(pos.x + size.width, pos.y + size.height)

                    if handle == handle_TOP_LEFT:
                        self.resizeAnchor  = botRight
                        self.resizeFloater = topLeft
                    elif handle == handle_TOP_RIGHT:
                        self.resizeAnchor  = botLeft
                        self.resizeFloater = topRight
                    elif handle == handle_BOTTOM_LEFT:
                        self.resizeAnchor  = topRight
                        self.resizeFloater = botLeft
                    elif handle == handle_BOTTOM_RIGHT:
                        self.resizeAnchor  = topLeft
                        self.resizeFloater = botRight

                self.curPt = mousePt
                self.resizeOffsetX = self.resizeFloater.x - mousePt.x
                self.resizeOffsetY = self.resizeFloater.y - mousePt.y
                endPt = QPoint(self.curPt.x + self.resizeOffsetX,
                                self.curPt.y + self.resizeOffsetY)
                self._DrawVisualFeedback(self.resizeAnchor, endPt,
                                         self.resizeFeedback, False)

            elif selecting and (self._GetObjectAt(mousePt) != None):

                # The user clicked on an object to select it.  If the user
                # drags, he/she will move the object.

		#distinguish between device and link, a link can be selected but not appear in the devicelist in selectpanel

                obj = self._GetObjectAt(mousePt)
                if obj not in self.selection:
                    # The user clicked on a new object
		    if event.ControlDown() == False:
	            	self.Select(obj,refresh=True)
		    else: #controlkey was down
			self.Select(obj,True,refresh=True) #add to selection	
		elif obj in self.selection and event.ControlDown() == True:
			self.Deselect(obj,refresh=True)

                self.dragMode = drag_MOVE
                self.moveOrigin = mousePt
                self.curPt      = mousePt
                self._DrawObjectOutline(0, 0)

            elif selecting and (self._GetObjectAt(mousePt) == None) and event.ControlDown() == True:
		#clicked in the visual window where no object is, when holding down Ctrl : missed object on click
                self.dragMode = drag_MOVE
                self.moveOrigin = mousePt
                self.curPt      = mousePt
                self._DrawObjectOutline(0, 0)
		self.main.ResetActiveObjects()
            else:
                # The user is dragging out a selection rect or new object.
                self.dragOrigin = mousePt
                self.curPt      = mousePt
                self.SetCursor(wxCROSS_CURSOR)
                self.CaptureMouse()
                self._DrawVisualFeedback(mousePt, mousePt, feedbackType,
                                         dashedLine)
                self.dragMode = drag_DRAG
		self.main.ResetActiveObjects()
            event.Skip()
            return

        if event.Dragging():
            if self.dragMode == drag_RESIZE:

                # We are resizing an object.

                mousePt = self._GetEventCoordinates(event)
                if (self.curPt.x != mousePt.x) or (self.curPt.y != mousePt.y): #we dragged away from last position
		    if self.curTool == tool_EDIT: #only in creation mode
			    #Know whether we "resized" to a node -> moved selection part to a node
			    dc = wxClientDC(self)
			    self.PrepareDC(dc)
        		    dc.BeginDrawing()

               		    dc.SetPen(wxPen(wxRED, 1, wxSOLID))

			    if self.resizeObject.GetType() == obj_LINK:
				obj = self._GetObjectAt(QPoint(mousePt.x,mousePt.y))
				if obj and obj.GetType() != obj_LINK:
					if obj != self.prevseldragobj and self.prevseldragobj != None: #deselect prev
						self.prevseldragobj.Draw(dc,draw_NORMAL,self.zoom >= 65,self.zoom >= 50)

					self.prevseldragobj = obj
					obj.Draw(dc,draw_NORMAL,self.zoom >= 65,self.zoom >= 50,False)
				else:
					if self.prevseldragobj != None: #deselect prev
						self.prevseldragobj.Draw(dc,draw_NORMAL,self.zoom >= 65,self.zoom >= 50)
						self.prevseldragobj = None

  			    dc.EndDrawing()

		    # We draw a visual feedback line or rectangle instead of the object itself
                    # Erase previous visual feedback.
                    endPt = QPoint(self.curPt.x + self.resizeOffsetX,
                                    self.curPt.y + self.resizeOffsetY)
                    self._DrawVisualFeedback(self.resizeAnchor, endPt,
                                             self.resizeFeedback, False)
                    self.curPt = mousePt
                    # Draw new visual feedback.
                    endPt = QPoint(self.curPt.x + self.resizeOffsetX,
                                    self.curPt.y + self.resizeOffsetY)
                    self._DrawVisualFeedback(self.resizeAnchor, endPt,
                                             self.resizeFeedback, False)


            elif self.dragMode == drag_MOVE:

                # We are moving a selected object.

                mousePt = self._GetEventCoordinates(event)
                if (self.curPt.x != mousePt.x) or (self.curPt.y != mousePt.y):
                    # Erase previous visual feedback.
                    self._DrawObjectOutline(self.curPt.x - self.moveOrigin.x,
                                            self.curPt.y - self.moveOrigin.y)
                    self.curPt = mousePt
                    # Draw new visual feedback.
                    self._DrawObjectOutline(self.curPt.x - self.moveOrigin.x,
                                            self.curPt.y - self.moveOrigin.y)

            elif self.dragMode == drag_DRAG:
                    # We are dragging out a new object or selection rect.

                    mousePt = self._GetEventCoordinates(event)
                    if (self.curPt.x != mousePt.x) or (self.curPt.y != mousePt.y):
                        # Erase previous visual feedback.
                        self._DrawVisualFeedback(self.dragOrigin, self.curPt,
                                             feedbackType, dashedLine)
                        self.curPt = mousePt
                        # Draw new visual feedback.
                        self._DrawVisualFeedback(self.dragOrigin, self.curPt,
                                                 feedbackType, dashedLine)
            event.Skip()
            return

        if event.LeftUp():
            if self.dragMode == drag_RESIZE: 

                mousePt = self._GetEventCoordinates(event)
                # Erase last visual feedback.
                endPt = QPoint(self.curPt.x + self.resizeOffsetX,
                                self.curPt.y + self.resizeOffsetY)
                self._DrawVisualFeedback(self.resizeAnchor, endPt,
                                         self.resizeFeedback, False)

                resizePt = QPoint(mousePt.x + self.resizeOffsetX,
                                   mousePt.y + self.resizeOffsetY)

                if (self.resizeFloater.x != resizePt.x) or \
                   (self.resizeFloater.y != resizePt.y):
                   self._ResizeObject(self.resizeObject,
                                      self.resizeAnchor,
                                      self.resizeFloater,
                                      resizePt)
                else:
                    pass
                    self.Refresh() # Clean up after empty resize.


                # We are resizing an object.
		if self.curTool == tool_EDIT: #only in creation mode
		    #Know whether we "resized" to a node -> moved selection part to a node
		    dc = wxClientDC(self)
		    self.PrepareDC(dc)
        	    dc.BeginDrawing()

		    if self.resizeObject.GetType() == obj_LINK:
			obj = self._GetObjectAt(QPoint(mousePt.x,mousePt.y))
			if obj and obj.GetType() != obj_LINK:
				#self.resizeObject
				obj.Draw(dc,draw_NORMAL,self.zoom >= 65,self.zoom >= 50,False)
				self.main.ModifyLinkClick(None,self.resizeObject.GetObject(),self.startpointsel,obj.GetObject())

			else:
				pass

		    self.prevseldragobj = None
		    dc.EndDrawing()
		    self.Refresh()

            elif self.dragMode == drag_MOVE:

                # We are moving a selected object.

                mousePt = self._GetEventCoordinates(event)
                # Erase last visual feedback.
                self._DrawObjectOutline(self.curPt.x - self.moveOrigin.x,
                                        self.curPt.y - self.moveOrigin.y)
                if (self.moveOrigin.x != mousePt.x) or \
                   (self.moveOrigin.y != mousePt.y):
                    self._MoveObject(mousePt.x - self.moveOrigin.x,
                                     mousePt.y - self.moveOrigin.y)
                else:
                    pass

            elif self.dragMode == drag_DRAG:
                # We are dragging out a new object or selection rect.

                mousePt = self._GetEventCoordinates(event)
                # Erase last visual feedback.
                self._DrawVisualFeedback(self.dragOrigin, self.curPt,
                                         feedbackType, dashedLine)
                self.ReleaseMouse()
                self.SetCursor(wxSTANDARD_CURSOR)
                # Perform the appropriate action for the current tool.
                if actionParam == param_RECT:
                    x1 = min(self.dragOrigin.x, self.curPt.x)
                    y1 = min(self.dragOrigin.y, self.curPt.y)
                    x2 = max(self.dragOrigin.x, self.curPt.x)
                    y2 = max(self.dragOrigin.y, self.curPt.y)

                    startX = x1
                    startY = y1
                    width  = x2 - x1
                    height = y2 - y1

                    if not selecting:
                        if ((x2-x1) < 8) or ((y2-y1) < 8): return # Too small.

                    action(x1, y1, x2-x1, y2-y1)
                elif actionParam == param_LINE:
                    action(self.dragOrigin.x, self.dragOrigin.y,
                           self.curPt.x, self.curPt.y)
            self.dragMode = drag_NONE # We have finished with this mouse event.
            event.Skip()


    def OnDoubleClickEvent(self, event=None,paintevent=True):
        """ Respond to a double-click in the visual window. Only used for device level (-1)

	@event - mouse event information
	@paintevent - whether we should manually trigger a paint event or not
        """
	
        mousePt = self._GetEventCoordinates(event)
        obj = self._GetObjectAt(mousePt)
        if obj == None: 
		return
	
        # Let the user edit the given object.
	if ((self.main.GetViewMode() != self.main.GetPrevView()) or self.main.GetViewMode() == ID_SUCCESSORS_MODE or self.main.GetViewMode() == ID_PATHS_MODE):
		# Change view; needed for UNDO/REDO to know whether we can paste objects in visual window or not
                self.main.SetPrevView(self.main.GetViewMode())
                self.main.WhenChangeView()


        if obj.GetType() == obj_NODE:
	    if self.main.GetViewMode() == ID_DYNAMIC_MODE:
		self.ShowNeighbourLinks(obj,obj.GetObject(),refresh=False)
	    elif self.main.GetViewMode() == ID_SUBSYSTEM_MODE:
			#self.main.ViewSubSystem(obj.GetObject().GetName(),refresh=False)
			#self.Select(obj,refresh=False)
			self.OnDeviceSelected(obj.GetObject(),False,True,refresh=False) 
			#set last parameter her to False if we want to expand/collapse with subsystem
			self.CenterToObject(obj)
	    elif self.main.GetViewMode() == ID_PATHS_MODE:
			self.main.GetSelectWindow().UpdatePath(obj.GetObject())
		
	    else: #Neighbor view
		self.main.GetSelectWindow().UpdateInfo(obj.GetObject())
		self.OnDeviceSelected(obj.GetObject(),refresh=False)

        elif obj.GetType() == obj_TEXT: # Not used
            pass
            
        else:
	    pass

	if paintevent:
		self.paintevent(None)

    def ViewOnlyLinksOfGivenType(self):
	""" To make the choice of link type to view have immediate effect in the
	    visual window. If the user chooses to view a given link type, all links
	    of other link types are removed for visual display. If the user chooses to
	    view all link types nothing happens before next time the user chooses something
	    to display (because we do not know what the user would like to view)

	    We do not need any parameters as the link type (name) is taken care of in a member
	    varible in CdbVis module.

	    !return - False if action not taken because then would all objects on the screen disappear, True if successful
	"""
	print "restricted: " + str(self.main.IsLinkTypeRestricted())

	tbd_links = []
	total_links = 0
	tbd_devs = []

	linktype = self.main.GetRestrictedLinkType()
	i = 0
	while i < len(visWindow.contents):
		my_itm = visWindow.contents[i]
		if my_itm.GetObject().__class__ == Link:
			if my_itm.GetObject().GetType() != linktype:
				tbd_links.append(my_itm)
				my_itm.show()
			else:
				my_itm.hide()
			total_links += 1
		i += 1

	if (len(tbd_links) == total_links):
		self.main.ShowError("None of the links that are shown are of the given link type, it would therefore have resulted in an empty visual window, and the choice was skipped.",ERR_INFORMATION,True)
		return False

	# now it is time to check the devices, if they do not have any connections left..we remove them
	i = 0
	# while i < len(visWindow.contents):
		# my_itm = visWindow.contents[i]
		# if my_itm.GetObject().__class__ == Device and my_itm.GetName() != self.main.GetActiveDevice():
			# typelist = map(Link.GetType,my_itm.GetLinks())
			# if :
				# self.DoDeleteObj(my_itm.GetName(),False,refresh=False)
				# continue
		# i += 1

    def ShowNeighbourLinks(self,origShape,deviceobj,refresh=True):
	""" Show or hide the neighbor objects of a selected object in the visual window; and
	    the links between them. If the neighbors are shown; the links to them will be collapsed
	    and the neighbors removed from visual window when calling this method, if they are hidden;
	    the links to them will be expanded and they will be shown.

	    @origShape - the drawing object that we want to view the neighbors for
	    @deviceobj - the data object for the drawing object we want to view neighbors for
	    @refresh - whether we should trigger a paint event manually or not.
	"""
	
	if GetZoomFactor() != 100:
		prev_zoom = GetZoomFactor()
		self.main.OnZoom(100,False)
	else:
		prev_zoom = 100

        
        def _CountNodes(linkList):
	    """ Return the number of linked nodes of the given linkList. This is a private
	    method inside ShowNeighborLinks(...) and hence not visible outside.

	    @linkList - a list of link objects for a device

	    !return - an integer telling the number of other devices this device is connected
	    to in the given link list for a device
            """

            nodes = {}
            for linkobj in linkList:
		link = linkobj.GetObjectInfo(True)
                node = link[DEV_SWITCH_FROM]
                nodes[node] = node		
                node = link[DEV_SWITCH_TO]
                nodes[node] = node
		
            # Return all nodes (if no node is linked, there is still the node from 
            # which we are searching!)
            return max(len(nodes), 1)
        
        
        cdb = self.main.GetCdb()

	#Get position of parent node
	ylayer_pos = origShape.GetLayer()
	node_pos = origShape.GetPosition()

	#Number of ingoing links
	inputLinks = deviceobj.GetLinks(False)	#Previous in path
	if inputLinks == False:
		self.ShowError(str(deviceobj.GetErrorMessage()),ERR_ERROR)
		if prev_zoom != 100:
			self.main.OnZoom(prev_zoom,False)
		return ([],[])

	#Number of outgoing links
	outputLinks = deviceobj.GetLinks(True)	#Next in path
	if outputLinks == False:
		self.ShowError(str(deviceobj.GetErrorMessage()),ERR_ERROR)
		if prev_zoom != 100:
			self.main.OnZoom(prev_zoom,False)
		return ([],[])
	
	#max ports
	#find whether maximum number of nodes are of in or out type, to correctly calculate the length in AddNode()
	if (deviceobj.GetPortsOut() > deviceobj.GetPortsIn()):
	    max_ports = deviceobj.GetPortsOut()
	else:
	    max_ports = deviceobj.GetPortsIn()



	### COLLAPSE/EXPAND CODE ###
	def countConnections(selfobj,portlist): 
		"""Count whether we have 0 or many (n > 0) connections for a node. We only count
		connections to other nodes than the parent; we need to do this in order to know
		whether the node is fully or partially expanded; since it is only the links
		to the parent node that are visible after a call to ShowNeighbourLinks(...)
		Private method for ShowNeighbourLinks(...).

		Rule: We cannot remove links from devices connected to a collapsing device; if
		they are not collapsed and connected to other devices as well.

		@selfobj - parent object; the device that we wanted to see the neighbors of
		@portlist - the port list for a neighbor node of the parent device (the one we want
		to view the neighbors of)

		!return - -1 if the node/device contain connections to other nodes as well, 0 if the
		node is only connected to the parent device here.
		"""
		
		for item in portlist:
			if item != None and (item.GetObject().GetNodeTo() != selfobj.GetName() and item.GetObject().GetNodeFrom() != selfobj.GetName()): #connections to parent we want to collapse doesnt count
				return -1

		return 0

	def removeConnection(obj,portlist,porttype):
		""" If we are collapsing links to neighbors; both the links and the neighbors will
		be removed from visual window. This method removes links that are to be collapsed;
		between the parent device (the device that was double-clicked) and a child device.

		@obj - the child device; that are about to be collapsed
		@portlist - list of links for the parent-device that is going to be collapsed
		@porttype - whether we are removing links going "IN" to the child device
		or "OUT" from the child device
		"""
		
		i = 0
		if porttype == "IN":
			for item in portlist:
				if item != None:
					if item.GetObject().GetNodeFrom() == obj.GetName(): # link going to parent -> remove
						tmp = portlist.pop(i)
						portlist.insert(i,None)
						if item in self.selection:
							self.selection.remove(item)
						if item in visWindow.contents:
							visWindow.contents.remove(item)
								
				i += 1

		elif porttype == "OUT":
			for item in portlist:
				if item != None:
					if item.GetObject().GetNodeTo() == obj.GetName():
						tmp = portlist.pop(i)
						portlist.insert(i,None)
						if item in self.selection:
							self.selection.remove(item)
						if item in visWindow.contents:
							visWindow.contents.remove(item)
	
				
				i += 1
	

	if origShape.GetExpanded() == 2: #totally expanded: we will need to collapse.
		tbd_content = []		
		tmp_collapsed = 0 #if we can not remove a node connection because of further linking to other devices,
				  #this one is set to 1, partially collapsed/expanded
				  
		for lnk in inputLinks: #ie. the parent node is the outputNode, we check from-nodes to the parent node
			visobj = self.FindByName(lnk.GetNodeFrom())
			visparent = self.FindByName(lnk.GetNodeTo())
			vislink = self.FindByName(lnk.GetName())
			if visobj != None: #object found
				if (visobj.GetExpanded() == 0) \
					or (visobj.GetExpanded() == 2 \
					and countConnections(visparent,visobj.GetPortList(port_IN)) == 0 \
					and countConnections(visparent,visobj.GetPortList(port_OUT)) == 0) \
					or (visobj.GetExpanded() == 1 \
					and countConnections(visparent,visobj.GetPortList(port_IN)) == 0 \
					and countConnections(visparent,visobj.GetPortList(port_OUT)) == 0):
					#we remove connections between parent device and child device if the expand/collapse
					#states matches:
					# a. child is totally collapsed
					# b. child is totally expanded and no other links than to parent device
					# c. child is partially expanded and no other links than to parent device

					removeConnection(visobj,origShape.GetPortList(port_IN),"IN")
					tbd_content.append(visobj)

				elif (visobj.GetExpanded() == 1) \
					and (countConnections(visparent,visobj.GetPortList(port_IN)) != 0 \
					or countConnections(visparent,visobj.GetPortList(port_OUT)) != 0):
					# if
					# parent still has connections to other devices
					# set to partially expanded/collapsed
					tmp_collapsed = 1
				elif (visobj.GetExpanded() == 2) \
					and (countConnections(visparent,visobj.GetPortList(port_IN)) != 0 or \
					countConnections(visparent,visobj.GetPortList(port_OUT)) != 0):
					#if
					#parent still has connections to other devices
					#set to partially expaned/collapsed
					tmp_collapsed = 1

		for lnk in outputLinks: # same as for iteration in inputLinks; but opposite
			visobj = self.FindByName(lnk.GetNodeTo())
			visparent = self.FindByName(lnk.GetNodeFrom())
			vislink = self.FindByName(lnk.GetName())
			if visobj != None: #object found
				if (visobj.GetExpanded() == 0) or \
					(visobj.GetExpanded() == 2 \
					and countConnections(visparent,visobj.GetPortList(port_IN)) == 0 \
					and countConnections(visparent,visobj.GetPortList(port_OUT)) == 0) \
					or (visobj.GetExpanded() == 1 \
					and countConnections(visparent,visobj.GetPortList(port_IN)) == 0 \
					and countConnections(visparent,visobj.GetPortList(port_OUT)) == 0):

					removeConnection(visobj,origShape.GetPortList(port_OUT),"OUT")
					tbd_content.append(visobj)

				elif (visobj.GetExpanded() == 1) \
					and (countConnections(visparent,visobj.GetPortList(port_IN)) != 0 \
					or countConnections(visparent,visobj.GetPortList(port_OUT)) != 0):
					tmp_collapsed = 1
				elif (visobj.GetExpanded() == 2) \
					and (countConnections(visparent,visobj.GetPortList(port_IN)) != 0 \
					or countConnections(visparent,visobj.GetPortList(port_OUT)) != 0):
					tmp_collapsed = 1

		# objects to be removed from visual window because they no longer have any connections (single/free objects)
		# in the visual window
		for delitm in tbd_content:
			if delitm in self.selection:
				self.selection.remove(delitm)
			if delitm in visWindow.contents:
				visWindow.contents.remove(delitm)
		
		#set new expand/collapse property of parent device
		origShape.SetExpanded(tmp_collapsed)
    		self.CenterToObject(origShape)
		if prev_zoom != 100:
			self.main.OnZoom(prev_zoom,False)
		return True #if the parent device was expanded; we successfully collapsed it; and return

	###END COLLAPSE CODE###

	ylayer_pos = origShape.GetLayer()
	node_pos = origShape.GetPosition()
	parent_x_pos_center = node_pos.x+(origShape.GetSize().GetWidth()/2)

	### START EXPAND CODE ###
        position = 1
	nr_out_added = 0
	devices_on_output = {}
        for i in range(len(outputLinks)): # output links, positioning
		tmp_link = outputLinks[i].GetObjectInfo(True)
		tmp_devobj = Device(self.main.GetActiveSystem(),tmp_link[DEV_SWITCH_TO],False)
		index,portnr = origShape.FindCorrespondingPortIndex(outputLinks[i].GetPortFrom(),outputLinks[i].GetPortTypeFrom(),port_OUT)
		port_pos = origShape.GetPortPosition(index,port_OUT)
		shape,newNodes = self.AddNode(tmp_devobj,port_pos.x,1,False,deviceobj.GetPortsOut(),parent_x_pos_center,port_pos.y,max_ports,refresh=False) #+1 from parentnode (y layer 1)

		# add all entries (links) for a device, to find average position and set that later
		if outputLinks[i].GetNodeTo() not in devices_on_output.keys(): # have to add new entry if not exist
			devices_on_output[outputLinks[i].GetNodeTo()] = []
			
		my_x = shape.GetPosition().x
		my_y = shape.GetPosition().y
		devices_on_output[outputLinks[i].GetNodeTo()].append([my_x,my_y])
		
		#device to -> connect it
                self.CreateConnectedLink(outputLinks[i])
		position += 1
		nr_out_added += newNodes

	# If there are several links between two devices, we find the average position for the node for all the links
	#print str(devices_on_output)
	for out_node in devices_on_output:
		sum_x = 0
		sum_y = 0
	    	for out_link in devices_on_output[out_node]:
			sum_x += out_link[0]
			sum_y += out_link[1]
			
		vis_node = self.FindByName(out_node)
		avg_x = int(sum_x / len(devices_on_output[out_node]))
		avg_y = int(sum_y / len(devices_on_output[out_node]))
		if vis_node != None:
			vis_node.SetPosition(QPoint(avg_x,avg_y))
            
        position = 1
	nr_in_added = 0
	devices_on_input = {}
        for i in range(len(inputLinks)): #input links, positioning
		tmp_link = inputLinks[i].GetObjectInfo(True)
		tmp_devobj = Device(self.main.GetActiveSystem(),tmp_link[DEV_SWITCH_FROM],False)
		index,portnr = origShape.FindCorrespondingPortIndex(inputLinks[i].GetPortTo(),inputLinks[i].GetPortTypeTo(),port_IN)
		port_pos = origShape.GetPortPosition(index,port_IN)
		shape,newNodes = self.AddNode(tmp_devobj,port_pos.x,-1,False,deviceobj.GetPortsIn(),parent_x_pos_center,port_pos.y,max_ports,refresh=False) #-1 from parentnode (y layer -1)
		
		if inputLinks[i].GetNodeFrom() not in devices_on_input.keys(): # have to add new entry if not exist
			devices_on_input[inputLinks[i].GetNodeFrom()] = []
			
		my_x = shape.GetPosition().x
		my_y = shape.GetPosition().y
		devices_on_input[inputLinks[i].GetNodeFrom()].append([my_x,my_y])
		
		#device from -> connect it
                self.CreateConnectedLink(inputLinks[i])
		position += 1
		nr_in_added += newNodes

	# If there are several links between two devices, we find the average position for the node for all the links
	for in_node in devices_on_input:
		sum_x = 0
		sum_y = 0
	    	for in_link in devices_on_input[in_node]:
			sum_x += in_link[0]
			sum_y += in_link[1]
			
		vis_node = self.FindByName(in_node)
		avg_x = int(sum_x / len(devices_on_input[in_node]))
		avg_y = int(sum_y / len(devices_on_input[in_node]))
		if vis_node != None:
			vis_node.SetPosition(QPoint(avg_x,avg_y))
	
            
	#we rearrange and count the total number of nodes in each layer after insertion for both input and output nodes
        absY_out = self.GetAbsY(ylayer_pos+1)
        self.SetAbsY(1, absY_out + nr_out_added)

        absY_in = self.GetAbsY(ylayer_pos-1)
        self.SetAbsY(1, absY_in + nr_in_added)

        self.Select(origShape,refresh=False)
	origShape.SetExpanded(2) #expanded
    	self.CenterToObject(origShape)
	
	if prev_zoom != 100:
		self.main.OnZoom(prev_zoom,False)

	if refresh:
		self.Refresh()

    def OnRightClick(self, event):
        """ Respond to the user right-clicking in the visual window.

            We select the clicked-on item, if necessary, and display a pop-up
            menu of available options which can be applied to the selected
            item(s).
        """

	if self.GetLevelZoom() != -1:
		return
	
        mousePt = self._GetEventCoordinates(event)
        obj = self._GetObjectAt(mousePt)

        if obj == None: # Nothing selected. Show a general visual menu.
		if not self.main.GetCdb(): # Do not show menu if not connected to ConfDB
			return False
		
		menu = wxMenu()
		menu.Append(menu_SELECTALL, "Select All")
		menu.Append(menu_DESELECTALL, "Deselect All")

		EVT_MENU(self, menu_SELECTALL, self.SelectAll)
		EVT_MENU(self, menu_DESELECTALL, self.DeselectAll)
		
		zoommenu = wxMenu()
		zoommenu.Append(menu_ZOOMIN, "Zoom In")
		zoommenu.Append(menu_ZOOMOUT, "Zoom Out")
		zoommenu.Append(menu_ZOOMCUSTOM, "Custom Zoom")

		menu.AppendMenu(-1,"Zoom",zoommenu,"")

		# We have to go through the CdbVis module because the zoom function is always called from that module
		EVT_MENU(self, menu_ZOOMIN, self.main.ZoomIn)
		EVT_MENU(self, menu_ZOOMOUT, self.main.ZoomOut)
		EVT_MENU(self, menu_ZOOMCUSTOM, self.main.ZoomCustom)

		#TODO: Up and Down one level?
		

		clickPt = QPoint(event.GetX(),event.GetY())
		self.PopupMenu(menu,clickPt)
		menu.Destroy()
	
	else: # obj != None

		# Select the clicked-on object.
		if obj not in self.selection:
		    self.Select(obj,refresh=True)
		
		
		if self.curTool == tool_EDIT:
			# Build our pop-up menu.

			menu = wxMenu()
			menu.Append(menu_CLONE, "Clone")
			menu.Append(menu_DUPLICATE, "Duplicate")
			menu.Append(menu_DELETE,    "Delete")
			menu.Append(menu_MODIFY,"Modify")

			EVT_MENU(self, menu_DUPLICATE,     self.DoDuplicateHelper)
			EVT_MENU(self, menu_DELETE,        self.DoDelete)
			EVT_MENU(self,menu_CLONE,self.DoCloneHelper)

			if obj.GetObject().__class__ == Device:
				EVT_MENU(self, menu_MODIFY, self.main.ModifyDeviceClick)
			else:
				EVT_MENU(self, menu_MODIFY, self.main.ModifyLinkClick)

			# Show the pop-up menu.

			clickPt = QPoint(event.GetX(), event.GetY())
			self.PopupMenu(menu, clickPt)
			menu.Destroy()

    def SetLevelZoom(self,factor):
	""" Change the level zoom to the given level.

	@factor - level to change to (-1 -> n, where n is the max level in the active subsystem
	"""
	self.levelzoom = factor
	#self.paintevent(None)

    def GetLevelZoom(self):
	""" Get the level zoom

	!return - level zoom (integer)
	"""
	return self.levelzoom

    def GetLevelInfo(self):
	""" Get the whole row of history of navigation within this active subsystem

	!return - a list with entries describing what the user chose in each level, 
	or False is returned if an error occured.
	"""
	try:
		levelinfo = self.levelselections[self.main.GetActiveSystem()]
		return levelinfo
	except KeyError,err:
		return False
	
    def GetLevelSelections(self):
	""" Get the history information about what the user chose in the previous level (1 up)

	!return - string describing the choice made at the previous level, or False if an error
	occurs
	"""
	try:
		levelinfo = self.levelselections[self.main.GetActiveSystem()][self.levelzoom-1]
		return levelinfo
	except IndexError,err:
		return False
	
    def GetLevelSelection(self,level):
	""" Get the history information about the current level.

	!return - string describing the contents of the current level.
	"""
	return self.levelselections[self.main.GetActiveSystem()][level]

    def SetLevelSelections(self,id,system,level,reset=False):
	""" Set history information about the choice the user made at the previous/current
	level.

	@id - obsolete, not used
	@system - the name of the active subsystem
	@level - the number/id of the level
	@reset - whether the history information about other levels should be reset
	before adding this new history information.
	"""
	try:
		if self.levelselections[system]:
			if reset:
				self.levelselections[system] = []
			while len(self.levelselections[system]) >= level:
				self.levelselections[system].pop(level-1)

			self.levelselections[system].append(id)
	except KeyError,err:
		self.levelselections[system] = []
		self.levelselections[system].append(id)

    def ResizeToFitContents(self):
	""" Helper function that checks the contents of the window and automatically make them fit inside the window,
	down to 25% zoom. If lower zoom is needed, the user will have to set it himself, because the devices doesnt show
	properly at lower zoom.

	The items in the visual window is only auto zoomed when new items are added, or the user explicitly change the
	enable autozoom to enabled on the View menu.

	This function works quite nice, but it does have some weird bugs:
	* sometimes it sets the zoom to 100 %, even though it should have been much smaller (seems that it breaks the
	iterative loop somehow, but because of lack of time I have not had possibillity to investigate this further)
	* The visible visual window also counts area behind widgets that is on top of the visual window (menu bar and tool
	bar and scroll bar and sash), therefore it often happens that devices that is close to one of the 4 edges of the
	window will not be seen as outside the visible portion of the window, and thus considered as no zoom is needed)
	"""

	# We may want to have less zoom after last change, therefore we go all the way out to 100%, and see
	# how much zoom-out that is needed again. We do this every time the content changes on the screen.
	if GetZoomFactor() < 100:
		self.main.OnZoom(100,False)	
		
	# if device level, we zoom to fit the contents in the window (but we do not zoom more than 25%)
	if self.levelzoom == -1:
		vw_size_w, vw_size_h = self.width(), self.height() #self.GetSize() #width and height of visible vis_win part
		vw_start_x, vw_start_y = self.contentsX(), self.contentsY()  #self.CalcUnscrolledPosition(0,0) # coordinates upper left corner
		
		# to float, because we will get decimals to work with later
		vw_start_x = float(vw_start_x)
		vw_start_y = float(vw_start_y)
		vw_size_w = float(vw_size_w)
		vw_size_h = float(vw_size_h)
		
		# end window coords.
		vw_end_x = vw_start_x + vw_size_w 
		vw_end_y = vw_start_y + vw_size_h

		# center vis.win. coords.
		center_x = vw_start_x + vw_size_w/2.0
		center_y = vw_start_y + vw_size_h/2.0
		greatest_distance = 1.0 # This is a distance factor for objects outside the visible window, if it is greater
		# than 1.0, we will have to zoom-out. The distance factor is calculated by finding the distance from the
		# center of the visual window to the edge of the visual portion of the window, and we divide the total
		# distance to the furthest object on this distance.
		zoom = 100.0 # default zoom, before we check if we need to zoom-out

		# maximum distance to the edge of the visible window, as well as the angle
		max_distance_rect = ((vw_size_w/2.0)*(vw_size_w/2.0) + (vw_size_h/2.0)*(vw_size_h/2.0))**(0.5) # hyptenuse
		max_angle_rect = (vw_size_h/2.0)/(vw_size_w/2.0) # angle of the diagonal in the vis.win, which is the
		# greatest distance from the center to the edges, in radians (before arctan)
		max_angle_rect_deg = degrees(atan(max_angle_rect)) # the angle in degrees, after arctan
		
		for vis_obj in visWindow.contents: # we find the outer positions of objects in all directions
			if vis_obj.GetType() == obj_NODE:
				obj_start_x, obj_start_y = vis_obj.GetPosition().x(), vis_obj.GetPosition().y()
				obj_size_w, obj_size_h = vis_obj.GetSize().width(), vis_obj.GetSize().height()

				obj_start_x = float(obj_start_x)
				obj_start_y = float(obj_start_y)
				obj_size_w = float(obj_size_w)
				obj_size_h = float(obj_size_h)
				
				obj_end_x = obj_start_x + obj_size_w
				obj_end_y = obj_start_y + obj_size_h
				
				# first we check whether the object is too much to the left of the vis.win.
				if obj_start_x < vw_start_x:
					x_dist = fabs(center_x - obj_start_x) # absolute distance to center x from startx pos. on obj

					# we take bottom y corner if it is below the center y, and top y corner if above
					if (obj_start_y < center_y):
						y_dist = fabs(center_y - obj_start_y)
					else:
						y_dist = fabs(center_y - obj_end_y)
						
					# calculate the distance to the point of the object that is most far away
					tot_dist = (x_dist*x_dist + y_dist*y_dist)**(0.5)

					# to avoid zero-division, if difference in x is 0
					if x_dist == 0.0:
						x_dist = 0.001

					angle = y_dist/x_dist # angle to our object, radians before arctan
					angle_deg = degrees(atan(angle)) # angle, degrees after arctan

					# if greater, it has a greater distance in another direction (y up or down)
					if angle_deg <= max_angle_rect_deg:
						distance_to_rect = ((tot_dist/x_dist) * vw_size_w/2.0) # calculate distance to
						# edge on the way to our object, by using similar triangles
						# compare to distance to the object itself, how much greater distance we need (1.xx)
						my_distance = tot_dist/distance_to_rect # how much more zoom we need to see it

						# if the zoom we need is smaller than the zoom calculated so far..we set it as new zoom
						if my_distance > greatest_distance:
							zoom = 100.0 / my_distance
							greatest_distance = my_distance

				if (obj_end_x > vw_end_x - 20):
					x_dist = fabs(center_x - obj_end_x)
					if (obj_end_y > center_y):
						y_dist = fabs(center_y - obj_end_y)
					else:
						y_dist = fabs(center_y - obj_start_y)
						
					tot_dist = (x_dist*x_dist + y_dist*y_dist)**(0.5)
					
					if x_dist == 0.0:
						x_dist = 0.001

					angle = y_dist/x_dist
					angle_deg = degrees(atan(angle))

					# if greater, it has a greater distance in another direction (y up or down)
					if angle_deg <= max_angle_rect_deg:
						distance_to_rect = ((tot_dist/x_dist) * vw_size_w/2.0)
						my_distance = (tot_dist+30.0)/distance_to_rect # how much more zoom we need to see it

						if my_distance > greatest_distance:
							zoom = 100.0 / my_distance
							greatest_distance = my_distance
							
				if obj_end_y > vw_end_y:
					if (obj_end_x > center_x):
						x_dist = fabs(center_x - obj_end_x)
					else:
						x_dist = fabs(center_x - obj_start_x)
						
					y_dist = fabs(center_y - obj_end_y)
						
					tot_dist = (x_dist*x_dist + y_dist*y_dist)**(0.5)
					
					if y_dist == 0.0:
						y_dist = 0.001

					angle = x_dist/y_dist
					angle_deg = degrees(atan(angle))

					# if greater, it has a greater distance in another direction (y up or down)
					if angle_deg <= max_angle_rect_deg:
						distance_to_rect = ((tot_dist/y_dist) * vw_size_h/2.0)
						my_distance = tot_dist/distance_to_rect # how much more zoom we need to see it

						if my_distance > greatest_distance:
							zoom = 100.0 / my_distance
							greatest_distance = my_distance

				if obj_start_y > vw_start_y:
					if (obj_start_x < center_x):
						x_dist = fabs(center_x - obj_start_x)
					else:
						x_dist = fabs(center_x - obj_end_x)
						
					y_dist = fabs(center_y - obj_start_y)
						
					tot_dist = (x_dist*x_dist + y_dist*y_dist)**(0.5)
					
					if y_dist == 0.0:
						y_dist = 0.001

					angle = x_dist/y_dist
					angle_deg = degrees(atan(angle))

					# if greater, it has a greater distance in another direction (y up or down)
					if angle_deg <= max_angle_rect_deg:
						distance_to_rect = ((tot_dist/y_dist) * vw_size_h/2.0)
						my_distance = tot_dist/distance_to_rect # how much more zoom we need to see it

						if my_distance > greatest_distance:
							zoom = 100.0 / my_distance
							greatest_distance = my_distance

		zoom = (GetZoomFactor() * (zoom/100.0)) # calculate zoom to fit contents

		# if below, 25, we set it to 25 coz smaller zoom is on users own risk :)
		if zoom < 25: 
			zoom = 25 #minimum
		else:
			zoom = int(zoom) # no decimals, cast to integer

		if zoom < GetZoomFactor():
			self.main.OnZoom(zoom,False) # Zoom!


    def paintevent(self, event):
	pass

    def DoSelectAll(self, event):
	""" Respond to the "Select All" menu command:
	select all objects in the visual window.
        """
        self.SelectAll() 

    def DoCloneHelper(self,addtoundo=True):
	""" Method called when clone is chosen for a selection of objects.

	@addtoundo - whether the clone information should be added to the UNDO list or not
	"""
	if len(self.selection) == 0:
		self.main.ShowError("Cannot clone a device and/or link because nothing is seleted.",ERR_ERROR)
		return False
	
	#nrofduplicates = wxGetNumberFromUser("How many clones do you want to make?","Nr. of clones","Nr. of clones",1,1,10000,self.main)
	nrofduplicates, valid = QInputDialog.getInteger("Nr. of clones","How many clones do you want to make?",1,1,10000,1,self)
	if not valid:
		return False
	res = self.DoClone(addtoundo,nrofduplicates)
	self.canvas.update()


    def DoClone(self,addtoundo=True,nrofclones=1):
	""" The real clone method, clones the objects selected in the number of clones
	chosen.

	@addtoundo - whether the clone information should be added to the UNDO list or not.
	@nrofclones - number of clones of each selected object to be created
	"""
	
	if len(self.selection) == 0:
		self.main.ShowError("Cannot clone a device and/or link because nothing is seleted.",ERR_ERROR)
		return False

	if addtoundo:
		undoindex = self.main.GetFirstNewUndoIndex()

		if len(self.selection) == 0: #None selected
			return False
		elif len(self.selection) == 1 and self.selection[0].GetType() == obj_NODE:
			descr = "clone device: " + str(self.selection[0].GetName()) + " " + str(nrofclones) + " times"
		elif len(self.selection) == 1 and self.selection[0].GetType() == obj_LINK:
			descr = "clone link: " + str(self.selection[0].GetName()) + " " + str(nrofclones) + " times"
		else:
			descr = "clone objects " + str(nrofclones) + " times"

	selecteditems = []
	for node in self.selection:
		selecteditems.append(node.GetObject().GetName())

	#Find free links; we want to find links to be cloned where 1 or 2 of the nodes it is attached to at the moment is not to be cloned, hence the cloned link will connect to available ports in the not-cloned devices
	to_links = {}
	from_links = {}
	free_links_available_to = {}	#nr of INGOING ports of given devices
	free_links_to_arr = [] 		#names of devices we know information about free to-ports
	free_links_available_from = {}	#nr of free OUTGOING ports of given devices
	free_links_from_arr = []	#names of devices we know information about free from-ports

	sorted_links = {} # sorted on: port type,port nbr
	port_reference = {}
	for itm in self.selection:
		#print "LinkName: " + str(itm.GetObject().GetName())
		if itm.GetType() == obj_LINK: #only links
			if itm.GetObject().GetNodeTo() not in selecteditems: #found free to link
				is_in_hash = False
				if itm.GetObject().GetNodeTo() in free_links_to_arr:
					inp = free_links_available_to[itm.GetObject().GetNodeTo()] #get nr of free in-links from given device
					is_in_hash = True
				else:
					dev_tmp = Device(itm.GetObject().GetSystem(),itm.GetObject().GetNodeTo(),False) #have to create a device to get port info
					totp,inp_arr,outp_arr = dev_tmp.GetFreePorts() #get information about how many free ports we have
					if totp == False and inp_arr == False and outp_arr == False:
						self.main.ShowError("Cannot clone device: " + str(dev_tmp.GetName()) + ": " + str(dev_tmp.GetErrorMessage()),ERR_ERROR)
						return False
					
					inp = len(inp_arr)
					outp = len(outp_arr)

				if inp < nrofclones:
					self.main.ShowError("Can not clone " + str(itm.GetObject().GetName()) + " because there are not enough free input ports on the device " + str(dev_tmp.GetName()) + ".",ERR_ERROR)
					return False

				if is_in_hash:
					free_links_available_to[itm.GetObject().GetNodeTo()] -= nrofclones #we subtract number of clones of given link, we may also have clones of other links connected to this device
				else:
					free_links_to_arr.append(itm.GetObject().GetNodeTo())
					free_links_available_to[itm.GetObject().GetNodeTo()] = inp-nrofclones
					
				to_links[itm.GetObject().GetName()] = dev_tmp

			if itm.GetObject().GetNodeFrom() not in selecteditems: #found free from link
				is_in_hash = False
				if itm.GetObject().GetNodeFrom() in free_links_from_arr:
					outp = free_links_available_from[itm.GetObject().GetNodeFrom()]
					is_in_hash = True
				else:
					dev_tmp = Device(itm.GetObject().GetSystem(),itm.GetObject().GetNodeFrom(),False)
					totp,inp_arr,outp_arr = dev_tmp.GetFreePorts()
					inp = len(inp_arr)
					outp = len(outp_arr)

				if outp < nrofclones:
					self.main.ShowError("Can not clone " + str(itm.GetObject().GetName()) + " because there are not enough free output ports on the device " + str(dev_tmp.GetName()) + ".",ERR_ERROR)
					return False

				if is_in_hash:
					free_links_available_from[itm.GetObject().GetNodeFrom()] -= nrofclones
				else:
					free_links_from_arr.append(itm.GetObject().GetNodeFrom())
					free_links_available_from[itm.GetObject().GetNodeFrom()] = outp-nrofclones
	
				from_links[itm.GetObject().GetName()] = dev_tmp #=SAVE NR OF FREE PORTS IN A MEMBER VARIABLE SO IT CAN EASILY BE ACCESSED

			porttype = itm.GetObject().GetPortTypeFrom()
			try:
				portnbr = int(itm.GetObject().GetPortFrom())
			except ValueError,err:
				portnbr = itm.GetObject().GetPortFrom()

			if porttype not in sorted_links.keys():
				sorted_links[porttype] = []

			sorted_links[porttype].append(portnbr)
			port_reference[porttype+str(portnbr)] = itm

	from_links_copy = []
	to_links_copy = []
	for porttype in sorted_links:
		sorted_links[porttype].sort() #sort the list of portnbrs for each port type!!! so that we can pick nbrs from the start
		for port in sorted_links[porttype]:
			linkname = port_reference[porttype+str(port)].GetObject().GetName()
			#print "linkname: " + str(linkname)
			if linkname in from_links:
				from_links_copy.append([linkname,from_links[linkname]])
			if linkname in to_links:
				to_links_copy.append([linkname,to_links[linkname]])
		
	pendinglinks = [] # links to be stored in dirty objects list
	i = 0
	while i < nrofclones:
		prev_sel = self.selection[:] #because things get deselected f.ex. using DoDeleteObj
		duplicateoriginlist = self.DoDuplicate(False,1,True,False)

		def sortmyports(portlist,orgporttype):
			''' A helper function to find remaining ports in sorted order (first port type then portnbr)
			for a given port type, and return the first free port in the sorted order.
			
			@portlist - the port list (IN or OUT) for a device
			@orgporttype - we need to clone a link and connect it to the same port type as the original
			link, therefore we need to know what port type the original link was connected to.
			'''

			ports = {}

			for port in portlist:
				tmp_port = port.split(":")
				porttype = tmp_port[1].strip()

				if porttype not in ports.keys():
					ports[porttype] = []

				try:
					portnbr = int(tmp_port[0].strip())
				except ValueError,err:
					portnbr = tmp_port[0].strip()

				ports[porttype].append(portnbr) #porttype -> portnbr[]

			for porttype in ports:
				ports[porttype].sort() #sort the list of portnbrs for each port type!!! so that we can pick nbrs from the start
			if orgporttype not in ports.keys():
				return "-1"
			else:
				return str(ports[orgporttype].pop(0)) #first free (lowest nr or alphabetical string)


		# IF we cant find a proper port to connect to, we HAVE to delete the given link, because we can not have free or partially free links
		# Both from visual window and remove from dirty list
		for tolink in to_links_copy:
			#totalp,inp,outp = to_links[tolink].GetSavedFreePorts()
			totalp,inp,outp = tolink[1].GetSavedFreePorts()
			if len(inp) <= 0:
				self.main.ShowError("Could not clone " + str(duplicateoriginlist[tolink[0]].GetName()) + " to " + str(tolink[1].GetName()) + " because there are not enough free ports.",ERR_ERROR)
				#return False?
			else:
				#tmp_link = self.main.GetDirtyObject(duplicateoriginlist[tolink],Link) #We work directly on the link in dirtylist, overwrite information given in dirty list with information about the cloned objects
				tmp_link = duplicateoriginlist[tolink[0]]
				if not tmp_link:
					return False
				
				if tmp_link not in pendinglinks:
					pendinglinks.append(tmp_link)

				if tmp_link.GetNodeTo() not in selecteditems: #if true, we will connect this link to a device that will not be cloned

					newportnbr = sortmyports(inp,tmp_link.GetPortTypeTo()) #!
					if newportnbr == "-1":
						self.main.ShowError("Could not clone " + str(duplicateoriginlist[tolink[0]].GetName()) + " to " + str(tolink[1].GetName()) + " because there are not any ports left with the same port type.",ERR_ERROR)
						continue #Here we should clean up instead? Or at least delete the object that failed.
					else:
						inp.remove(newportnbr + " : " + tmp_link.GetPortTypeTo()) #This can never fail, because we found this in the list
				else: #we will connect this link to a device we are cloning as well, which means connect to the SAME ports as with the original
					newportnbr = tmp_link.GetPortTo()

				tmp_link.SetNodeTo(tolink[1].GetName())
				tmp_link.SetPortTo(newportnbr) #to be changed, need have a smarter solution to pick "correct" port
				tmp_link.SetPortTypeTo(tmp_link.GetPortTypeTo())

				#newObj.SetPortIndexFrom(link.GetPortIndexFrom()) #! have to set this as well!!!

				self.DoDeleteObj(tmp_link.GetName(),False,refresh=False) #?
				self.CreateConnectedLink(tmp_link)
				
		for fromlink in from_links_copy:
			#totalp,inp,outp = from_links[fromlink].GetSavedFreePorts()
			totalp,inp,outp = fromlink[1].GetSavedFreePorts()
			if len(outp) <= 0:
				self.main.ShowError("Could not clone " + str(duplicateoriginlist[fromlink[0]].GetName()) + " from " + str(fromlink[1].GetName()) + " because there are not enough free ports.",ERR_ERROR)
				#return False?
			else:
				#tmp_link = self.main.GetDirtyObject(duplicateoriginlist[fromlink],Link)
				tmp_link = duplicateoriginlist[fromlink[0]]
				if not tmp_link:
					#tmp_link = Link(itm.GetObject().GetSystem(),duplicateoriginlist[fromlink],False,False)
					return False
				
				if tmp_link not in pendinglinks:
					pendinglinks.append(tmp_link)

				if tmp_link.GetNodeFrom() not in selecteditems: #connect link to a device that will not be cloned
					newportnbr = sortmyports(outp,tmp_link.GetPortTypeFrom()) #!
					if newportnbr == "-1":
						self.main.ShowError("Could not clone " + str(duplicateoriginlist[fromlink[0]].GetName()) + " from " + str(fromlink[1].GetName()) + " because there are not any ports left with the same port type.",ERR_ERROR)
						continue #Here we should clean up instead? Or at least delete the object that failed.
					else:
						outp.remove(newportnbr + " : " + tmp_link.GetPortTypeFrom()) #This can never fail, because we found this in the list
				else: #we will connect this link to a device we are cloning as well, which means connect to the SAME ports as with the original
					newportnbr = tmp_link.GetPortFrom()

				tmp_link.SetNodeFrom(fromlink[1].GetName())
				tmp_link.SetPortFrom(newportnbr)
				tmp_link.SetPortTypeFrom(tmp_link.GetPortTypeFrom())
				#delete the one in the window and replace with a new one
				self.DoDeleteObj(tmp_link.GetName(),False,refresh=False)
				self.CreateConnectedLink(tmp_link)
		
		# We have to wait until here to add links to the dirty objects list, because the same link may go through
		# both loops (for to_links and from_links), reset list as well for next iteration.
		for link in pendinglinks:
			link.Create()

		pendinglinks = []

		self.selection = prev_sel
		i += 1


	#UNDO information
	if addtoundo:
		#undo information
		#redo information is always directly stolen from undo information
		pass ############# self.main.AddToUndo(undoindex,descr)

	#Set up the free links to connect to the same device if possible (enough ports of the given type in/out

    def DoDuplicateHelper(self,addtoundo=True):
	""" Called when the user chooses to duplicate a selection of objects

	@addtoundo - whether the duplicate actions should be added to the undo list for later
	duplication or not.
	"""

	if len(self.selection) == 0:
		self.main.ShowError("Cannot duplicate a device and/or link because nothing is seleted.",ERR_ERROR)
		return False
	
	nrofduplicates = wxGetNumberFromUser("How many duplicates do you want to make?","Nr. of duplicates","Nr. of duplicates",1,1,10000,self.main)
	if nrofduplicates == -1:
		self.main.ShowError("Invalid number of duplicates.",ERR_ERROR)
		return False
	teststr = str(nrofduplicates)
	if teststr.find(".",0,len(teststr)) != -1: #float
		self.main.ShowError("Float number is not supported.",ERR_ERROR)
		return False
		
	res = self.DoDuplicate(addtoundo,int(nrofduplicates),False,False)
	self.paintevent(None)

    def DoDuplicate(self,addtoundo=True,nrofduplicates=1,fromclone=False,paintevent=True):
        """ Duplicates the selected objects in the number of duplicates selected by
	the user.

	@addtoundo - whether the duplicate actions should be added to the undo list for alter
	duplication or not.
	@nrofduplicates - the number of duplicates that will be created of each selected object.
	@fromclone - whether this method was called from the DoClone(...) method or not; 
	there are some differences in code
	@paintevent - whether we should trigger a paint event or not.
        """
	
	duplicateoriginlist = {}

	if len(self.selection) == 0:
		self.main.ShowError("Cannot duplicate a device and/or link because nothing is seleted.",ERR_ERROR)
		return False

	#To duplicate a link, both devices it is connected to has to be duplicated as well. Check.
	if not fromclone:
		devnames = []
		links = []
		for myitm in self.selection:
			if obj_NODE == myitm.GetType():
				devnames.append(myitm.GetName())
			elif obj_LINK == myitm.GetType():
				links.append(myitm.GetObject())

		for link in links:
			if not (link.GetNodeTo() in devnames and link.GetNodeFrom() in devnames):
				self.main.ShowError("Cannot duplicate the link " + str(link.GetName()) + " because it has to be connected to two devices after duplication.",ERR_ERROR)
				return False

	if addtoundo:
		undoindex = self.main.GetFirstNewUndoIndex()
	#ONLY when addtoundo = true


	i = 0
	while i < nrofduplicates:
		duplicatelist = {}
		tmplinks = []
		prev_portListIn = {}
		prev_portListOut = {}
		objs = [] #new objects
		#ports = [] #new ports (duplicates)

		for obj in self.selection:
			type = obj.GetType()
			if type == obj_NODE:
			    #pdb.set_trace()
			    newObj = Box(self.canvas,type)
			    newObj.SetData(obj.GetData()) #(1) Device + DeviceType
			    prev_portListIn[obj.GetName()] = obj.GetPortList(port_IN)[:] #copy
			    prev_portListOut[obj.GetName()] = obj.GetPortList(port_OUT)[:] #copy

			    tmp_obj = newObj.GetObject()

			    devid = self.main.GetNextDeviceID(obj.GetObject().GetSystem(),obj.GetObject().GetType()) #(2)
			    if devid == False:
				#should not happen..but you never know
				devid = ((str(self.main.GetActiveSystem()) + "_tmp_device_" + str(self.main.GetNewLinkNr())).upper())

			    tmp_obj.SetName(devid)

			    #Duplicate ports
			    old_ports = obj.GetObject().GetPorts()
			    for tmp_oldport in old_ports:
				tmp_newport = Port(tmp_oldport.GetSystem(),tmp_oldport.GetPortNbr(),tmp_oldport.GetPortType(),tmp_oldport.GetPortWay(),tmp_oldport.GetDevice(),True)
				tmp_newport.SetDict(tmp_oldport.GetObjectInfo(True))
				tmp_newport.SetDevice(tmp_obj.GetName())
				tmp_newport.SetPortID(-1) #So we do not mistaken this port to be the old one...

				#ports.append(tmp_newport)
				tmp_newport.Create()

			    #selectpanel, treeview updates
		            try:
                            	pass ##!!!!!!!!!!!    my_id = self.main.GetSelectWindow().GetItemID(tmp_obj.GetType())
                                ##!!!!!!!!!!!!!!!!!!!!    self.main.GetSelectWindow().AddItem(my_id,tmp_obj.GetName(),TN_DEVICE)
                            except KeyError,err:
                                #print "treeitemid not found"
                                pass

			    tmp_obj.SetID("")
			    tmp_obj.Create() #set savestatus to insert
			    newObj.SetText(tmp_obj.GetName())

			    duplicateoriginlist[obj.GetObject().GetName()] = devid
			    newObj.ClearPorts()
			    objs.append(newObj)
			    duplicatelist[obj.GetObject().GetName()] = tmp_obj.GetName() #to handle the links afterwards
			    ### SHOW THE DEVICE NOW
			    newObj.show()
			elif obj_LINK == type:
				tmplinks.append(obj) #add to linklist tmp to walk through when we finished this loop

		for link in tmplinks:
			newObj = myLine(self.canvas,type)
			newObj.SetData(link.GetData())

			newObj.SetPortIndexFrom(link.GetPortIndexFrom())
			newObj.SetPortIndexTo(link.GetPortIndexTo())

			tmp_obj = newObj.GetObject()
			tmp_obj.SetName(("tmp_link_" + str(self.main.GetNewLinkNr())).upper())
			if fromclone: #set first to the same as the previous object, fake of course, but so that it can be saved
				if link.GetObject().GetNodeTo() not in duplicatelist:
					tmp_obj.SetNodeTo(link.GetObject().GetNodeTo())
				else:
					tmp_obj.SetNodeTo(duplicatelist[link.GetObject().GetNodeTo()])
					
				if link.GetObject().GetNodeFrom() not in duplicatelist: #only happens with cloning
					tmp_obj.SetNodeFrom(link.GetObject().GetNodeFrom())
				else:
					tmp_obj.SetNodeFrom(duplicatelist[link.GetObject().GetNodeFrom()])

				tmp_obj.SetPortFrom(link.GetObject().GetPortFrom())
				tmp_obj.SetPortTypeFrom(link.GetObject().GetPortTypeFrom())
				tmp_obj.SetPortTo(link.GetObject().GetPortTo())
				tmp_obj.SetPortTypeTo(link.GetObject().GetPortTypeTo())
				newObj.SetFixed(False)
			#tmp_obj.SetID("")
			if not fromclone: #pending save to dirty objects list before all properties set for cloning
				tmp_obj.Create() #savestatus to insert
			newObj.SetText(tmp_obj.GetName())
			newObj.SetFixed(True)

			duplicateoriginlist[link.GetObject().GetName()] = tmp_obj #.GetName() #old name -> duplicate name
			
			if not fromclone:
				if link.GetObject().GetNodeTo() not in duplicatelist: #only happens with cloning, because connection between links and devices is handled in DoClone(...)
					tmp_obj.SetNodeTo("-1")
					tmp_obj.SetPortTo("-1")
					tmp_obj.SetPortTypeTo("")
					newObj.SetFixed(False)
				else:
					tmp_obj.SetNodeTo(duplicatelist[link.GetObject().GetNodeTo()])

				if link.GetObject().GetNodeFrom() not in duplicatelist: #only happens with cloning
					tmp_obj.SetNodeFrom("-1")
					tmp_obj.SetPortFrom("-1")
					tmp_obj.SetPortTypeFrom("")
					newObj.SetFixed(False)
				else:
					tmp_obj.SetNodeFrom(duplicatelist[link.GetObject().GetNodeFrom()])

			objs.append(newObj)
			
			duplicatelist[link.GetObject().GetName()] = tmp_obj.GetName() #to handle the links afterwards
			#### We show now the new object
			newObj.show()
		#update..so we can find by findbyname
		visWindow.contents = objs + visWindow.contents

		#update link connections for the nodes
		for itm in prev_portListIn:
			tmp_inlist = []
			for port in prev_portListIn[itm]:
				if port != None:
					if port.GetObject().GetName() not in duplicatelist: #if not given link was duplicated
						tmp_inlist.append(None)
					else:
						tmp_inlist.append(self.FindByName(duplicatelist[port.GetObject().GetName()])) #if duplicated

				else:
					tmp_inlist.append(None)

			self.FindByName(duplicatelist[itm]).SetPortList(port_IN,tmp_inlist[:])

		for itm in prev_portListOut:
			tmp_outlist = []
			for port in prev_portListOut[itm]:
				if port != None:
					if port.GetObject().GetName() not in duplicatelist:
						tmp_outlist.append(None)
					else:
						tmp_outlist.append(self.FindByName(duplicatelist[port.GetObject().GetName()]))
				else:
					tmp_outlist.append(None)
			self.FindByName(duplicatelist[itm]).SetPortList(port_OUT,tmp_outlist[:])


		tmp_sel = self.selection[:]
		self.selection = []
		
	        self.SelectMany(objs,refresh=False)
		tab = [-1,1]
		tabentry = i%2

    		self._MoveObject(-100,tab[tabentry]*i*10) #move all selected objects

		self.selection = tmp_sel

		i += 1 #nrofduplicates created	

	#UNDO information
	if addtoundo:
		#undo information
		#redo information is always directly stolen from undo information
		if len(self.selection) == 0:
			return False
		elif len(self.selection) == 1 and self.selection[0].GetType() == obj_NODE:
			descr = "duplicate device: " + str(tmp_obj.GetName()) + " " + str(nrofduplicates) + " times"
		elif len(self.selection) == 1 and self.selection[0].GetType() == obj_LINK:
			descr = "duplicate link: " + str(tmp_obj.GetName()) + " " + str(nrofduplicates) + " times"
		else:
			descr = "duplicate objects " + str(nrofduplicates) + " times"

		self.main.AddToUndo(undoindex,descr)
		#undoindex had to be retrieved before anything was added to the dirty list from this function
		
	if paintevent:
		self.paintevent(None)

	return duplicateoriginlist

    def DoDelete(self, event=None,addtoundo=True,refresh=True):
        """ Delete objects from the visual window, and add to the dirty objects
	list if set to do so.
	
	@addtoundo - whether we should add information about the action to the undo list; and 
	that the objects deleted should be deleted for real (True), if False it is only removed
	for visual appearance.
	@refresh - whether we should trigger the paint event or not.
        """

	# Ok to delete?
	ans = QMessageBox.question(self,"Delete ?","Are you sur you want to delete this Device.","Yes","No","",0,1)
	if ans == 1: # No
		return
	if addtoundo: #check that if one try to delete a device, that it is not stored in the db yet. The ports are always stuck to the devices so if it is okey to delete a device, it is also ok to delete a port (none of them is then saved in the db)
		for obj in self.selection:
			if obj.GetType() == obj_NODE:
				test_obj = Device(self.main.GetActiveSystem(),obj.GetObject().GetName(),True)
				success = test_obj.Update(True) #We force to check in the db, because if it manages
				# to return data, it is already stored in db; and hence not possible to delete
				if success != False and success != {}: #managed to retrieve data from the db, thus cannot delete device
					self.main.ShowError("Cannot delete the objects in the selection because at least one of the devices (" + str(obj.GetObject().GetName()) + "), is already stored in the database.",ERR_ERROR)
					return False

	# Create undo information
	if addtoundo:
		#undo information
		#redo information is always directly stolen from undo information
		if len(self.selection) == 0:
			self.main.ShowError("Cannot delete a device and/or link because nothing is seleted.",ERR_ERROR)
			return False
		elif len(self.selection) == 1 and self.selection[0].GetType() == obj_NODE:
			descr = "delete device: " + str(self.selection[0].GetName())
		elif len(self.selection) == 1 and self.selection[0].GetType() == obj_LINK:
			descr = "delete link: " + str(self.selection[0].GetName())
		else:
			descr = "delete objects"

		undoindex = self.main.GetFirstNewUndoIndex()


	addlast = []
	addfirst = []
	i = len(self.selection) - 1
	while i >= 0: #We add the devices to a new list, so that they are tried to be deleted after all links have been deleted. Easier to see if there are any links left connected to the devices one wants to have removed
		if self.selection[i].GetType() == obj_NODE:
			addlast.append(self.selection.pop(i))
		else: #obj_LINK
			addfirst.append(self.selection.pop(i))
		i -= 1

	#my_list = self.selection + addlast
	my_list = addfirst + addlast
	tmp_obj = None
 	for obj in my_list:

	    if addtoundo:
		    if obj.GetType() == obj_NODE:
			    isfree = obj.GetObject().IsAllPortsFree()
		    else:
			    isfree = True #for links, always true

		    if isfree == False:
				self.main.ShowError("An error occured while trying to fetch port information for " + str(obj.GetObject().GetName()) + ". Could not delete object.",ERR_ERROR)
				addlast.remove(obj)
				continue
		    elif isfree == -1:
				self.main.ShowError("Could not delete the device " + str(obj.GetObject().GetName()) + ", because it is connected to a non-deleted link. To delete this device, first delete the link, then the device.",ERR_ERROR)
				addlast.remove(obj)
				continue

	    if addtoundo: #to be moved to cdbvis delete method later
		    try:
			#remove from treelist
			my_id = self.main.GetSelectWindow().GetItemID(obj.GetName())
			self.main.GetSelectWindow().RemoveItem(my_id,obj.GetObject().GetType()) #link and device
			self.main.GetSelectWindow().SetItemToDeleted(obj.GetName())
		    except KeyError,err:
			print "treeitemid not found"

	    #we copy the create object to make a delete object
	    if addtoundo:
		if not self.main.FindDirtyObject(obj.GetName(),obj.GetObject().__class__):
			obj.GetObject().Delete() #set object to be deleted, and added to dirty list. We do not have to create a new object because this object is not
						 #yet in the dirtylist, and hence no references is overwritten
		else:
			#tmp_obj = copy.deepcopy(obj.GetObject())
			if obj.GetType() == obj_NODE:
				tmp_obj = Device(self.main.GetActiveSystem(),obj.GetObject().GetName(),False)
			elif obj.GetType() == obj_LINK:
				tmp_obj = Link(self.main.GetActiveSystem(),obj.GetObject().GetName(),False,False)

			tmp_obj.Delete() #set the new object to delete status, and add to dirty list, has to be done before removed from contents, because
					 #visual object is added to visual undo list
		
	    if obj.GetType() == obj_LINK: #remove references to this link in the nodes that this link is connected to
		tmp_nodefrom = obj.GetObject().GetNodeFrom()
		tmp_nodeto = obj.GetObject().GetNodeTo()

		tmp_visnodefrom = self.FindByName(tmp_nodefrom)
		tmp_visnodeto = self.FindByName(tmp_nodeto)

		if tmp_visnodefrom != None and tmp_visnodefrom not in addlast: #only remove link info from a device if the link is deleted and not the device
			tmp_portoutlist = tmp_visnodefrom.GetPortList(port_OUT)
			i = 0
			for port in tmp_portoutlist:
				if port == obj:
					tmp_portoutlist[i] = None
					break
				i += 1
		else:
			#print "There are no devices this link is connected to in the visual window any more"
			pass

		if tmp_visnodeto != None and tmp_visnodeto not in addlast:
			tmp_portinlist = tmp_visnodeto.GetPortList(port_IN)
			i = 0
			for port in tmp_portinlist:
				if port == obj:
					tmp_portinlist[i] = None
					break
				i += 1

		else:
			#print "There are no devices this link is connected to in the visual window any more"
			pass

		visWindow.contents.remove(obj) #this has to be done after Delete() since Delete() grabs the visobj reference from contents

	    elif obj.GetType() == obj_NODE:
		#if we are deleting the object for real
		if addtoundo: #a real delete
			my_ports = obj.GetObject().GetPorts()
			if my_ports == False:
				self.main.ShowError("An error occured while retrieving port data from the ConfDB: " + str(obj.GetObject().GetErrorMessage()),ERR_ERROR)
			else:
				for port in my_ports:
					tmp_port = Port(self.main.GetActiveSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),False)
					tmp_port.Delete()
				
		visWindow.contents.remove(obj)	

	    #del obj #also delete ports on devices, and set free links to non fixed
	    if obj.GetObject().__class__ == Device and self.main.GetActiveDevice() == obj.GetName():
		self.main.SetActiveDevice(None)
	    elif obj.GetObject().__class__ == Link and self.main.GetActiveLink() == obj.GetName():
		self.main.SetActiveLink(None)

	if (addfirst + addlast) != [] and addtoundo:
		self.main.AddToUndo(undoindex,descr)

        self.DeselectAll(refresh=refresh)

    def GetContents(self):
	""" Return a list of references to the visual objects in the visual window

	!return - a list of references to visual objects (DrawingObject)
	"""
	return visWindow.contents

    def GetSelection(self):
	""" Return a list of references to the visual objects in the visual window
	that are selected.

	!return - a list of references to visual objects (DrawingObject)
	"""
	return self.selection

    def DoDeleteObj(self, name,addtoundo=True,refresh=True):
        """ Delete an object given by name.
	    A object can only be deleted if it does not have any links attached to itself.

	    @name - name of the object to delete
	    @addtoundo - whether the delete information should be real and added to the 
	    dirty objects list.
	    @refresh - whether we should trigger a paint event or not.
        """
	
	self.DeselectAll(refresh=False)
	visobj = self.FindByName(name)

	if visobj != None:
		self.Select(visobj,refresh=False)
	else:
		return False
	self.DoDelete(None,addtoundo,refresh=refresh)

    def DoDeleteAll(self, event = None,onlyselected=False,refresh=False):
        """ Remove all visual objects in visual window from the screen. None
	of them will be deleted for real.

	@onlyselected - whether only the selected objects should be deleted (True),
	or every object in the visual window should be removed (False)
	@refresh - whether we shoul trigger the paint event or not
	"""
	
	
	# if not onlyselected:
	        # for obj in visWindow.contents:
		    # obj.hide()
        	    # del obj
	        # visWindow.contents = []
        	# self.DeselectAll(refresh=refresh)
	# else:
		# for obj in self.selection:
			# obj.hide()
			# del obj
		# self.selection = []
		# list = self.canvas.collisions(QRect(self.contentsX(),self.contentsY(),self.width(),self.height()))
		# for obj in list:
			# obj.hide()
			# del obj
		# del list
		# self.DeselectAll(refresh=refresh)		

		
	self.selection = []
	self.main.SetActiveDevice(None)
	self.main.SetActiveLink(None)
	self.pendingItem, self.activeItem, = None, None
	self.__moving = None
	# for item in self.canvas.allItems():
		# item.setCanvas(None)
		
		
	# for item in self.canvas.allItems():
	    # if isinstance(item,myLine):
		# item.setCanvas(None)
		# item.nodeFrom.setCanvas(None)
		# item.nodeTo.setCanvas(None)
		#del item.nodeFrom
		#del item.nodeTo
	    # elif isinstance(item,Box):
		# item.Canvastext.setCanvas(None)
		#del item.Canvastext
		# del item.portList  #PortList = {0: [<visWindow.myLine object at 0x09308C90>, None, None], 1: [None, None]}
		#del item.object
	for item in visWindow.contents: ##self.canvas.allItems():
	   if isinstance(item,Box) or isinstance(item,myLine):  ###### OBSELETE : dans le liste contents on n a que des lignes et des boxes.
		if item not in self.keptItems:
			item.hide()
			item._delete()
		
	for item in visWindow.contents: ##self.canvas.allItems():
		if item not in self.keptItems:
			item.setCanvas(None)
			if item in visWindow.contents: visWindow.contents.remove(item)
			del item
	list = []
	

	### Personal memory gestion
	# for TYPE in [Box, myLine]:
		# for i in range(len(Memo[TYPE])):
			# Memo[TYPE][i][0].setCanvas(None)
			# Memo[TYPE][i][1] = FREE
		
	

	print " ############  WARNING : " + str(len(self.canvas.allItems())) + "  Items still in the Canvas"
	visWindow.contents  = []
	
	##Check if there is kept item : (The kept items are delete the second time the DodeleteAll function is called )
	# if self.keptItems: # and not visWindow.contents:
		##Move item :
		# for item in self.keptItems:
			# if isinstance(item,Box):
				# kept = Box(self.canvas,obj_NODE, item)
				# kept.move(2500,2400)
				# kept.show()
		# self.keptItems = []
	self.canvas.update()

    def DoMoveForward(self, event):
        """ Respond to the "Move Forward" menu command, to move
	the selected object one step to the front; possibly
	hiding other objects.
        """
        if len(self.selection) != 1: return

        obj = self.selection[0]
        index = visWindow.contents.index(obj)
        if index == 0: return

        del visWindow.contents[index]
        visWindow.contents.insert(index-1, obj)

        self.Refresh()
        self._AdjustMenus()


    def DoMoveToFront(self, event):
        """ Respond to the "Move To The Front" menu command, to move
	the selected object all the way to the front; possibly
	hiding other objects; but not objects can hide this.
        """
        if len(self.selection) != 1: return

        obj = self.selection[0]
        visWindow.contents.remove(obj)
        visWindow.contents.insert(0, obj)

        self.Refresh()
        self._AdjustMenus()

    def DoMoveBackward(self, event):
        """ Respond to the "Move backward" menu command, to move
	the selected object one step to the back; possibly
	being hided by other objects.
        """
	
        if len(self.selection) != 1: return

        obj = self.selection[0]
        index = visWindow.contents.index(obj)
        if index == len(visWindow.contents) - 1: return

        del visWindow.contents[index]
        visWindow.contents.insert(index+1, obj)

        self.Refresh()
        self._AdjustMenus()


    def DoMoveToBack(self, event):
        """ Respond to the "Move To Back" menu command, to move
	the selected object all the way to the back; possibly
	being hided by other objects.
        """
        if len(self.selection) != 1: return

        obj = self.selection[0]
        visWindow.contents.remove(obj)
        visWindow.contents.append(obj)

        self.Refresh()
        self._AdjustMenus()


    # =============================
    # == Object Creation Methods ==
    # =============================

    def CreateNode(self, x, y, obj,refresh=True): #from info to obj
        """ Creates a new node object at the given position and size)

	@x - x position to create the object (upper left corner)
	@y - y position to create the object (upper left corner)
	@obj - the data object to be created as visual object
	@refresh - whether we should trigger the paint event or not
        """
	info = obj.GetObjectInfo(True)

	#set the size of the object to constants
	pin = info[DEV_PORTS_IN]
	pout = info[DEV_PORTS_OUT]
	maxp = 0
	if pin > pout:
		maxp = pin
	else:
		maxp = pout
	if maxp < 5:
		maxp = 5
	if maxp > 100:
		maxp = 100
		
	new_height = NODE_HEIGHT
	new_width = NODE_WIDTH * maxp
	if maxp >= 20:
		new_height += maxp/20/10

        obj_tmp = Box(self.canvas,obj_NODE, obj, QPoint(x, y),
                            QSize(new_width, new_height),
                            self.penColour,
                            Color(info[DEV_COLOUR]),
                            self.lineSize)
	##print "///////////////////////// juste apres la cretation dans CreateNode" + str(sys.getrefcount(obj_tmp))
	
	# obj_tmp = myAlloc(Box,self.canvas,obj_NODE, object = obj, position = QPoint(x, y),
                            # size = QSize(new_width, new_height),
                            # penColour = self.penColour,
                            # fillColour = info[DEV_COLOUR],
                            # lineSize = self.lineSize)
        visWindow.contents.insert(0, obj_tmp)
        obj_tmp.SetText(str(info[DEV_NAME]))
        obj_tmp.SetPorts(port_IN, info[DEV_PORTS_IN])
        obj_tmp.SetPorts(port_OUT, info[DEV_PORTS_OUT])

	if refresh:
	        self.canvas.update()#WALID_CHANGE self.Refresh()

        return obj_tmp


    def CreateLink(self, x1, y1, x2, y2, obj): #from info to obj
        """ Create a new link object at the given position and size.

	@x1 - x position of the start of the link
	@y1 - y position of the start of the link
	@x2 - x position of the end of the link
	@y2 - y position of the end of the link
	@obj - the data object that will be created to visual object
        """

        topLeftX  = min(x1, x2)
        topLeftY  = min(y1, y2)
        botRightX = max(x1, x2)
        botRightY = max(y1, y2)

	#no check whether the object was created or not...should be
        obj_tmp = myLine(self.canvas,obj_LINK, obj, QPoint(topLeftX, topLeftY),
                            QSize(botRightX-topLeftX, botRightY-topLeftY),
                            Qt.black,
                            self.fillColour,
                            self.lineSize,
                            QPoint(x1 - topLeftX, y1 - topLeftY),
                            QPoint(x2 - topLeftX, y2 - topLeftY))        
	# obj_tmp = myAlloc(myLine,self.canvas,obj_LINK, object = obj, position=QPoint(topLeftX, topLeftY),
                            # size=QSize(botRightX-topLeftX,
                                        # botRightY-topLeftY),
                            # penColour= Qt.black,
                            # fillColour=self.fillColour,
                            # lineSize=self.lineSize,
                            # startPt = QPoint(x1 - topLeftX, y1 - topLeftY),
                            # endPt   = QPoint(x2 - topLeftX, y2 - topLeftY))
        #obj_tmp.SetFixed(True) #set in createconnectedlink!
        obj_tmp.SetText(str(obj.GetObjectInfo(True)[DEV_LINK]))
        visWindow.contents.append(obj_tmp)
        return obj_tmp

    def CreateConnectedLink(self, object,linkpos=QPoint(0,0),linksize=QSize(0,0),paintevent=False):
        """ Create a new connection between two devices, where both the connection information
	for the devices connected and the link is set.

        The connections to nodes are set, too. They must be present as visual objects before,
        otherwise the link is NOT created and None is returned.
        The link is created at position (0,0)

	@object - the link data object for the connection that will be created
	@linkpos - positon of the link to be created (neglected)
	@size - size of the link being created (neglected)
	@paintevent - whether the function should trigger the paint event (True) or not (False
        """
	deviceInfo = object.GetObjectInfo(True) 
	
       	obj = self.FindByName(str(deviceInfo[DEV_LINK]))
        if not obj:
        	obj = self.CreateLink(linkpos.x(), linkpos.y(), linkpos.x() + linksize.width(), linkpos.y() + linksize.height(), object) #f.ex. info about dev- and port- from and to
	        obj.SetFixed(True) #set here when it is connected to two objects...
	else: #remove previous links for this device, in order to reconnect properly
		#we found an object, we have to take care of it in case we want to undo
		tmp_nodefrom = obj.GetObject().GetNodeFrom()
		tmp_nodeto = obj.GetObject().GetNodeTo()

		tmp_visnodefrom = self.FindByName(tmp_nodefrom)
		tmp_visnodeto = self.FindByName(tmp_nodeto)

		if tmp_visnodefrom != None:
			tmp_portoutlist = tmp_visnodefrom.GetPortList(port_OUT)
			i = 0
			for port in tmp_portoutlist:
				if port == obj:
					tmp_portoutlist[i] = None
					break
				i += 1
		else:
			#print "Device not found to connect to!!!"
			pass

		if tmp_visnodeto != None:
			tmp_portinlist = tmp_visnodeto.GetPortList(port_IN)
			i = 0
			for port in tmp_portinlist:
				if port == obj:
					tmp_portinlist[i] = None
					break
				i += 1

		else:
			#print "Device not found to connect to!!!"	
			pass

	connected = False
	connected2 = False
	node_from = self.FindByName(deviceInfo[DEV_SWITCH_FROM])
	
	if node_from:
		deviceindex,portnr = node_from.FindCorrespondingPortIndex(object.GetPortFrom(),object.GetPortTypeFrom(),port_OUT)
		# print node_from.GetName(), node_from.GetType()
		# print portnr
		# print "DATAOBJETC.GetPortFrom() : " + str(object.GetPortFrom())
		# print "%%%%%%%%%%%%%%%%%%%%%%"
		obj.SetPortIndexFrom(deviceindex)
		if portnr != -1:
			connected = obj.SetConnection(node_from,port_OUT,portnr)
        if not connected:
		self.missing_parentnodes.append([obj.GetName(),deviceInfo[DEV_SWITCH_FROM]])
	node_to = self.FindByName(deviceInfo[DEV_SWITCH_TO])
	if node_to:
		deviceindex,portnr = node_to.FindCorrespondingPortIndex(object.GetPortTo(),object.GetPortTypeTo(),port_IN)
		obj.SetPortIndexTo(deviceindex)
		if portnr != -1:
			connected2 = obj.SetConnection(node_to,port_IN,portnr)
        if not connected2:
		self.missing_parentnodes.append([obj.GetName(),deviceInfo[DEV_SWITCH_FROM]])

	obj.SetText(str(deviceInfo[DEV_LINK]))
	#pdb.set_trace()
	obj.to_delta = obj.nodeTo.GetPortPosition(obj.portindexto,port_IN).x() - obj.nodeTo.x()
	obj.from_delta = obj.nodeFrom.GetPortPosition(obj.portindexfrom,port_OUT).x() - obj.nodeFrom.x()
	obj.show()
	if paintevent:
		self.paintevent(None)
        return obj


    # =======================
    # == Selection Methods ==
    # =======================

    def SelectAll(self,refresh=True):
        """ Select every DrawingObject in the visual window.

	@refresh - whether the paint event should be triggered or not.
        """
	
        self.selection = []
        for obj in visWindow.contents:
            self.selection.append(obj)

	if refresh:
	        self.Refresh()

        self._AdjustMenus()
	self.main.GetSelectWindow().UpdateInfo(None)


    def DeselectAll(self,refresh=True):
        """ Deselect every DrawingObject in the visual window.

	@refresh - whether the paint event should be triggered or not
        """
	
        self.selection = []

	if refresh:
	        pass #WAL_PROB self.Refresh()

        self._AdjustMenus()
	self.main.GetSelectWindow().UpdateInfo(None)

    def Deselect(self,obj,refresh=True):
	""" Deselect a given object.

	@obj - the data object of the object to be deselected
	@refresh - whether the paint event should be triggered or not
	"""
	
	tmp_name = obj.GetObject().GetName()
	i = 0
	for item in self.selection:
		if item.GetObject().GetName() == tmp_name:
			self.selection.pop(i)
		i += 1

	if refresh:
		self.Refresh()

	self._AdjustMenus()

    def Select(self, obj,add=False,showinfo=True,refresh=True):
        """ Select the given DrawingObject in the visual window.

	@obj - the visual object of the object to be selected
	@add - whether the object should be added to an already existing
	selection of objects; or that the already existing selection of
	objects (if any) should be reset, and this object should be
	selected alone
	@showinfo - whether the info about this object should be shown in the
	information window (True) or not (False)
	@refresh - whether the paint event should be triggered or not
        """
	if add == False:
	        self.selection = [obj]
	else:
		self.selection.append(obj)


        self._AdjustMenus()
	if obj.GetType() in [obj_NODE]: #because 
	    #IS this correct, should the selected device be set to the active device, and its devicetype to the active devicetype?
	    self.main.SetActiveDeviceType(obj.GetObject().GetObjectInfo(True)[DEV_TYPE]) #should I set active device type here?
	    self.main.SetActiveDevice(obj.GetObject().GetName())
	    self.main.SetActiveLinkType(None)
	    self.main.SetActiveLink(None)
		
	    if showinfo:
		    if add == False:
			self.main.GetSelectWindow().UpdateInfo(obj.GetObject())
		    else:
			self.main.GetSelectWindow().UpdateInfo(None)
			
	elif obj.GetType() in [obj_LINK]:
		self.main.SetActiveLinkType(obj.GetObject().GetType())
		self.main.SetActiveLink(obj.GetObject().GetName())
	    	self.main.SetActiveDeviceType(None)
		self.main.SetActiveDevice(None)
		if showinfo:
			if add == False:
				self.main.GetSelectWindow().UpdateInfo(obj.GetObject())
			else:
				self.main.GetSelectWindow().UpdateInfo(None)

	if refresh:
	        self.Refresh()


    def SelectMany(self, objs,refresh=True):
        """ Select the a range of objects.

	@objs - a list of visual objects that should be selected
	@refresh - whether the paint event should be triggered or not
        """
        self.selection = objs
	
	if refresh:
	        self.Refresh()

        self._AdjustMenus()
	self.main.GetSelectWindow().UpdateInfo(None)


    def SelectByRectangle(self, x, y, width, height):
        """ Select every DrawingObject in the given rectangular region
	which was set up by the user (selection area).

	@x - x start position of selection area
	@y - y start position of selection area
	@width - width of selection area
	@height - height of selection area
        """
	
        self.selection = []
        for obj in visWindow.contents:
            if obj.ObjectWithinRect(x, y, width, height):
                self.selection.append(obj)
        self.Refresh()
        self._AdjustMenus()
        if len(self.selection) == 1:
            if obj.GetType() in [obj_NODE, obj_LINK]:
		self.main.GetSelectWindow().UpdateInfo(obj.GetObject())
        else: 
	    self.main.GetSelectWindow().UpdateInfo(None)
        
    def FindByName(self, name):
        """ Search after a DrawingObject (visual object) in the visual
	window by name, and return it if found

	@name - name of the visual object to be looking for
	
	!return - either the DrawingObject instance of the object if found,
	or None if not found.
        """
        for obj in visWindow.contents:
            if obj.GetName() == name:
                return obj
        return None

    # =====================
    # == Private Methods ==
    # =====================
    
    def _AdjustMenus(self):
        """ Adjust our menus to reflect the current state of the
            world (the objects and number of them selected)
        """
        selection = len(self.selection) > 0
        onlyOne   = len(self.selection) == 1
        isText    = onlyOne and (self.selection[0].GetType() == obj_TEXT)
        front     = onlyOne and (self.selection[0] == visWindow.contents[0])
        back      = onlyOne and (self.selection[0] == visWindow.contents[-1])

    def SetCurrentTool(self, newTool):
        """ Set the currently selected tool.

	@newTool - a constant defining the tool to use
        """
        if self.curTool == newTool: return # Nothing to do.
        self.curTool = newTool


    def _SetPenColour(self, colour,refresh=True):
        """ Set the default or selected objects pen colour.

	@colour - colour used for lines
	@refresh - whether we should trigger the paint event or not
        """
        if len(self.selection) > 0:
            for obj in self.selection:
                obj.SetPenColour(colour)

	    if refresh:
	            self.Refresh()
        else:
            self.penColour = colour
            self.optionIndicator.setPenColour(colour)


    def _SetFillColour(self, colour, refresh=True):
        """ Set the default or selected objects fill colour.

	@colour - colour used for fill
	@refresh - whether we should trigger the paint event or not
        """
        if len(self.selection) > 0:
            for obj in self.selection:
                obj.SetFillColour(colour)
	    if refresh:
	            self.Refresh()
        else:
            self.fillColour = colour
            self.optionIndicator.SetFillColour(colour)


    def _SetLineSize(self, size,refresh=True):
        """ Set the default or selected objects line size.
	
	@size - size of line
	@refresh - whether we should trigger the paint event or not
        """
        if len(self.selection) > 0:
            for obj in self.selection:
                obj.SetLineSize(size)
	    if refresh:
	            self.Refresh()
        else:
            self.lineSize = size
            self.optionIndicator.SetLineSize(size)


    def _ResizeObject(self, obj, anchorPt, oldPt, newPt,refresh=True):
        """ Resize the given object. The object should fit within the given 
	dimensions, though if the new point is less than the anchor point 
	the object will need to be moved as well as resized, to avoid giving 
	it a negative size.

	@obj - the visual object to be resized
        @anchorPt - is the unchanging corner of the object, while the
        opposite corner has been resized.
	@oldPt - are the current coordinates for the changed corner
	@newPt - are the new coordinates for the changed corner.
	@refresh - whether we should trigger the paint event or not
        """
        if obj.GetType() == obj_TEXT:
            # Not allowed to resize text objects -- they are sized to fit text.
            wxBell()
            return

        if obj.IsFixed():
            # object fixed, resizing not allowed
            return

        topLeft  = QPoint(min(anchorPt.x, newPt.x),
                           min(anchorPt.y, newPt.y))
        botRight = QPoint(max(anchorPt.x, newPt.x),
                           max(anchorPt.y, newPt.y))

        newWidth  = botRight.x - topLeft.x
        newHeight = botRight.y - topLeft.y

        if obj.GetType() in [obj_LINE, obj_LINK]:
            # Adjust the line so that its start and end points match the new
            # overall object size.

            startPt = obj.GetStartPt()
            endPt   = obj.GetEndPt()

            slopesDown = ((startPt.x < endPt.x) and (startPt.y < endPt.y)) or \
                         ((startPt.x > endPt.x) and (startPt.y > endPt.y))

            # Handle the user flipping the line.

            hFlip = ((anchorPt.x < oldPt.x) and (anchorPt.x > newPt.x)) or \
                    ((anchorPt.x > oldPt.x) and (anchorPt.x < newPt.x))
            vFlip = ((anchorPt.y < oldPt.y) and (anchorPt.y > newPt.y)) or \
                    ((anchorPt.y > oldPt.y) and (anchorPt.y < newPt.y))

            if (hFlip and not vFlip) or (vFlip and not hFlip):
                slopesDown = not slopesDown # Line flipped.

            if slopesDown:
                obj.SetStartPt(QPoint(0, 0))
                obj.SetEndPt(QPoint(newWidth, newHeight))
            else:
                obj.SetStartPt(QPoint(0, newHeight))
                obj.SetEndPt(QPoint(newWidth, 0))

        # Finally, adjust the bounds of the object to match the new dimensions.

        obj.SetPosition(topLeft)
        obj.SetSize(QSize(botRight.x - topLeft.x, botRight.y - topLeft.y))

	if refresh:
	        self.Refresh()


    def _MoveObject(self, offsetX, offsetY,refresh=True):
        """ Move the currently selected object(s) by the given offset.

	@offsetX - offset in the x direction
	@offsetY - offset in the y direction
	@refresh - whether we should trigger the paint event or not
        """

        for obj in self.selection:
            if not obj.IsFixed():
		obj.move(obj.x() + offsetX, obj.y() + offsetY)

	if refresh:
	        self.canvas.update() #self.Refresh()

    def MoveObjectToPos(self, obj, posX, posY,refresh=True):
        """ Move the given object to the given position

	@obj - the visual object to be moved
	@posX - the new X position
	@posY - the new Y position
	@refresh - whether we should trigger the paint event or not
        """
        pos = obj.GetPosition()
        pos=QPoint(posX,posY) #pos.x = posX; pos.y = posY
        obj.SetPosition(pos)

	if refresh:
	        self.Refresh()


    def _GetEventCoordinates(self, event):
        """ Return the coordinates associated with the given mouse event.

        The coordinates have to be adjusted to allow for the current scroll
        position.

        !return - QPoint(...) for the given mouse event.
        """
	
        originX, originY = self.GetViewStart()
        unitX, unitY = self.GetScrollPixelsPerUnit()
        return QPoint(event.GetX() + (originX * unitX),
                       event.GetY() + (originY * unitY))


    def _GetObjectAndSelectionHandleAt(self, pt):
        """ Return the object and selection handle at the given point.

            We draw selection handles (small rectangles) around the currently
            selected object(s).  If the given point is within one of the
            selection handle rectangles, we return the associated object and a
            code indicating which selection handle the point is in.  If the
            point is not within any selection handle at all, we return the tuple
            (None, handle_NONE).

	    @pt - the point we are looking for objects at (that contains this point)

	    !return - a tuple of the visual object and its handle if found,
	    (None,handle_NONE) if not.
        """
        for obj in self.selection:
            handle = obj.GetSelectionHandleContainingPoint(pt.x, pt.y)
            if handle != handle_NONE:
                return obj, handle

        return None, handle_NONE


    def _GetObjectAt(self, pt,macrodevice=False):
        """ Return the first object found which is at the given point.

	@pt - the point we look for visual objects at (point within the visual object)
	@macrodevice - whether we are looking for a macro device (virtual device) or not;
	need a check because not all macro devices are clickable.

	!return - the first found visual object (DrawingObject) that contains the point given.
        """
	if macrodevice:
		for obj in self.macroobjects:
			if obj.ObjectContainsPoint(pt.x,pt.y):
				if obj.IsClickable():
					return obj
				elif not obj.IsClickable():
					continue
		return None

	else:#nodes and links
	        for obj in visWindow.contents:
        	    if obj.ObjectContainsPoint(pt.x, pt.y):
                	return obj
	        return None


    def _DrawObjectOutline(self, offsetX, offsetY):
        """ Draw an outline of the currently selected object.

            The selected objects outline is drawn at the objects position
            plus the given offset.

            Note that the outline is drawn by *inverting* the windows
            contents, so calling _drawObjectOutline twice in succession will
            restore the windows contents back to what they were previously.

	    @offsetX - offset in x direction for the outline
	    @offsetY - offst in y direction for the outline
        """
	
        if len(self.selection) == 0: return
        
        dc = wxClientDC(self)
        self.PrepareDC(dc)
        dc.BeginDrawing()
        dc.SetPen(Qt.black_DASHED_PEN)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        dc.SetLogicalFunction(wxINVERT)
        
        # draw object outline for all objects in selection
        for obj in self.selection:
            position = obj.GetPosition()
            size     = obj.GetSize()
            dc.DrawRectangle(position.x + offsetX, position.y + offsetY,
                             size.width, size.height)
            
        dc.EndDrawing()


    def _DrawVisualFeedback(self, startPt, endPt, type, dashedLine):
        """ Draw visual feedback for a drawing operation.

            The visual feedback consists of a line, ellipse, or rectangle based
            around the two given points.  'type' should be one of the following
            predefined feedback type constants:

                feedback_RECT     ->  draw rectangular feedback.
                feedback_LINE     ->  draw line feedback.
                feedback_ELLIPSE  ->  draw elliptical feedback.

            if 'dashedLine' is True, the feedback is drawn as a dashed rather
            than a solid line.

            Note that the feedback is drawn by *inverting* the windows
            contents, so calling _drawVisualFeedback twice in succession will
            restore the windows contents back to what they were previously.

	    @startPt - start point QPoint(...) x and y for the rectangle feedback
	    @endPt - end point QPoint(...) x and y for the rectangle feedback
	    @type - type of feedback (depends on what kind of visual object we do
	    an operation on (feedback_RECT, feedback_LINE or feedback_ELLIPSE)
	    @dashedLine - whether the feedback should be drawn as a dashed line or not
        """
        dc = wxClientDC(self)
        self.PrepareDC(dc)
        dc.BeginDrawing()
        if dashedLine:
            dc.SetPen(Qt.black_DASHED_PEN)
        else:
            dc.SetPen(Qt.black_PEN)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        dc.SetLogicalFunction(wxINVERT)

        if type == feedback_RECT:
            dc.DrawRectangle(startPt.x, startPt.y,
                             endPt.x - startPt.x,
                             endPt.y - startPt.y)
        elif type == feedback_LINE:
            dc.DrawLine(startPt.x, startPt.y, endPt.x, endPt.y)
        elif type == feedback_ELLIPSE:
            dc.DrawEllipse(startPt.x, startPt.y,
                           endPt.x - startPt.x,
                           endPt.y - startPt.y)

        dc.EndDrawing()

#----------------------------------------------------------------------------
	
	
class DrawingObject:
    """ An object within the drawing panel.

        A pySketch document consists of a front-to-back ordered list of
        DrawingObjects.  Each DrawingObject has the following properties:

            'type'          What type of object this is (node, link or macro (virtual))
            'position'      The position of the object within the document.
            'size'          The size of the object within the document.
            'penColour'     The colour to use for drawing the objects outline.
            'fillColour'    Colour to use for drawing objects interior.
            'lineSize'      Line width (in pixels) to use for objects outline.
            'startPt'       The point, relative to the objects position, where
                            an obj_LINE objects line should start.
            'endPt'         The point, relative to the objects position, where
                            an obj_LINE objects line should end.
            'text'          The objects text (obj_TEXT objects only).
            'textFont'      The text objects font name.
            'textSize'      The text objects point size.
            'textBoldface'  If True, this text object will be drawn in
                            boldface.
            'textItalic'    If True, this text object will be drawn in italic.
            'textUnderline' If True, this text object will be drawn underlined.
            'linkLength'    Integer how long the link connectors of a node are
            'fixed'         If True, the object is connected with others and cannot be resized alone   
            'object'	    Data object (device or link) defined with the visual object
            'portList'      List of link objects connected to a node
	    'expanded'	    Boolean, whether the neighbours of given _node_ is expanded or not..For use in dynamic link view 				 mode.
	    'zoomfactor'    Integer, what zoom the visual object has right now.
	    'clickable'     Boolean, whether the visual object is click-sensitive (triggers visual feedback) when you click
	    		    on it or not.
	    'originalsize'  QSize, the original size of the object in 100% zoom.
	    'macroid'	    Integer, the unique macroID needed to identify macro/virtual objects
	    'textvalign'    Integer constant, where the text should be placed vertically on the object: text_CENTER,
	    		    text_RIGHT or text_LEFT
	    'texthaling'    Integer constant, where the text should be placed horizontally ont he object; text_CENTER,
	    		    text_TOP or text_BOTTOM.
            """

    def __init__(self, canvas, type, object = None, position=QPoint(0, 0), size=QSize(0, 0),
                 penColour=Qt.black, fillColour=Qt.white, lineSize=1,
                 text=None, startPt=QPoint(0, 0), endPt=QPoint(0,0),zoomFactor=100,clickable=True,originalsize=QSize(0,0),macroid=-1,textvalign=text_CENTER,texthalign=text_CENTER):
        """ Standard constructor.

            'type' is the type of object being created.  This should be one of
            the following constants: obj_NODE, obj_LINK, obj_MACRODEVICE

            The remaining parameters is explained above for the class.
        """
        QCanvasRectangle.__init__(self, canvas)
        self.i = 0 # temporary to remove !!  has no effect !!
        self.type              = type
        self.position          = position
        self.size              = size
        self.penColour         = penColour
        self.fillColour        = fillColour
        self.lineSize          = lineSize
        self.startPt           = startPt
        self.endPt             = endPt
        self.text              = text
        self.layer	       = 0 #default layer...
        self.textFont          = "Arial" # wxSystemSettings_GetSystemFont( wxSYS_DEFAULT_GUI_FONT).GetFaceName()
        self.textvalign = textvalign
        self.texthalign = texthalign
        self.textSize          = 10
        self.textBoldface      = False
        self.textItalic        = False
        self.textUnderline     = False
	self.canvas = canvas

        self.linkLength        = 7
        self.fixed             = False
	self.zoomFactor	       = zoomFactor
	if type == obj_LINK:
		self.portindexfrom = -1 #index of the port on the given device its connected to the output
		self.portindexto = -1 #index of the port on the given device it is connected to the input

	self.object		= object

	self.expanded		= 0 #always totally collapsed for links, 1 or 2 for nodes if expanded

	self.clickable = clickable

	self.originalsize = originalsize
	self.macroid = macroid
        
        # only used by obj_NODE: obj_LINK objects saved for each port
	if self.type == obj_NODE and self.object:
		deviceinfo_tmp = self.object.GetObjectInfo(True)
                self.portList  = {port_IN: [None for i in range(max(deviceinfo_tmp[DEV_PORTS_IN], 0))],
                                  port_OUT: [None for i in range(max(deviceinfo_tmp[DEV_PORTS_OUT], 0))]}
        else: self.portList    = {port_IN: [], port_OUT: []}
	
	

    # =============================
    # == Object Property Methods ==
    # =============================

    def GetData(self):
        """ Return a copy of the objects internal data.
            This is used to save this DrawingObject to disk.

	    !return a list of all member variables in this object.
        """
        return [self.type, self.x(), self.y(),
                self.width(), self.height(),
                self.penColour.red(),
                self.penColour.green(),
                self.penColour.blue(),
                self.fillColour.red(),
                self.fillColour.green(),
                self.fillColour.blue(),
                self.lineSize,
                self.startPt.x(), self.startPt.y(),
                self.endPt.x(), self.endPt.y(),
                self.text,
                self.textFont,
                self.textSize,
                self.textBoldface,
                self.textItalic,
                self.textUnderline,
                self.linkLength,
                self.fixed,
                self.object,
                self.portList,self.expanded,self.zoomFactor]


    def SetData(self, data):
        """ Set the objects internal data.

            @data is a copy of an objects member variables, as returned by
            getData() above.  This is used to restore a previously saved
            DrawingObject.
        """
	
        self.type              = data[0]

        self.position          = QPoint(data[1], data[2])
        self.size              = QSize(data[3], data[4])
        self.penColour         = QColor(data[5], data[6], data[7])
        self.fillColour        = QColor(data[8], data[9], data[10])
        self.lineSize          = data[11]
        self.startPt           = QPoint(data[12], data[13])
        self.endPt             = QPoint(data[14], data[15])
        self.text              = data[16]
        self.textFont          = data[17]
        self.textSize          = data[18]
        self.textBoldface      = data[19]
        self.textItalic        = data[20]
        self.textUnderline     = data[21]
        self.linkLength        = data[22]
        self.fixed             = data[23]
	
	#NB!!!!! Handle carefully...we copy the object reference
	if self.type == obj_NODE: # We have to create a new device object for this visual object, cannot
		#reference to the old one
		self.object = Device(data[24].GetSystem(),data[24].GetName(),False)
	elif self.type == obj_LINK:
		self.object = Link(data[24].GetSystem(),data[24].GetName(),False,False)

	if self.type == obj_NODE:
		self.portList = {}
		for item in data[25]:
			self.portList[item] = data[25][item]

		self.expanded = data[26]

	self.zoomFactor = data[27]

    def GetType(self):
        """ Return this DrawingObjects type (obj_NODE, obj_LINK or obj_MACRODEVICE)
        """
        return self.type

    def SetLayer(self,layer):
	self.layer = layer
    def GetLayer(self):
	return self.layer

    def SetPosition(self, position, adjust = True):
        """ Set the position (top-left corner) for this DrawingObject.

	@position - QPoint for top-left corner for this object.
	@adjust - whether the links to this object (if obj_NODE) should be adjusted
	to the new position.
        """
        self.position = QPoint(position.x(),position.y())
	if self.type == obj_NODE or self.type == obj_LINK:
	        if adjust: self._AdjustLinks()
    def GetPosition(self):
        """ Return this DrawingObjects position.

	!return QPoint(...) for the position of the top-left corner of this object.
        """
        return self.position

    def GetPortIndexFrom(self):
	""" For visual objects of type obj_LINK, the port index they go from an
	obj_NODE (port index is the internal port numbering; from 0 to n-1 for the 
	port way).
	"""
	return self.portindexfrom

    def GetPortIndexTo(self):
	""" For visual objects of type obj_LINK, the port index they go to an
	obj_NODE (port index is the internal port numbering; from 0 to n-1 for the 
	port way).
	"""
	return self.portindexto

    def SetPortIndexFrom(self,index):
	""" Set internal port index, converted from the port number and port type.
	"""
	self.portindexfrom = index
    def SetPortIndexTo(self,index):
	""" Set internal port index, converted from the port number and port type.
	"""
	self.portindexto = index

    def GetExpanded(self):
	""" The expanded status of this obj_NODE.
	"""
	return self.expanded

    def SetExpanded(self,expanded):
	""" 
		expanded - 0: totally collapsed
		expanded - 1: partly collapse/expanded
		expanded - 2: totally expanded
	"""
	self.expanded = expanded

    def SetZoomFactor(self,zf):
	self.zoomFactor = zf
    def GetZoomFactor(self):
	return self.zoomFactor

    def SetSize(self, size, adjust = True):
        """ Set the size for this DrawingObject.

	@size - QSize object
	@adjust - whether the links and a obj_NODE should be adjusted.
        """
        self.size = size
	if self.type == obj_NODE or self.type == obj_LINK:
	        if adjust: self._AdjustLinks()


    def GetSize(self):
        """ Return this DrawingObjects size.
        """
        return self.size


    def SetPenColour(self, colour):
        """ Set the pen colour used for this DrawingObject.
        """
        self.penColour = colour


    def GetPenColour(self):
        """ Return this DrawingObjects pen colour.
        """
        return self.penColour


    def SetFillColour(self, colour):
        """ Set the fill colour used for this DrawingObject.
        """
        self.fillColour = colour


    def GetFillColour(self):
        """ Return this DrawingObjects fill colour.
        """
        return self.fillColour


    def SetLineSize(self, lineSize):
        """ Set the linesize used for this DrawingObject.
        """
        self.lineSize = lineSize


    def GetLineSize(self):
        """ Return this DrawingObjects line size.
        """
        return self.lineSize


    def SetStartPt(self, startPt):
        """ Set the starting point for this line DrawingObject.
        """
        self.startPt = startPt


    def GetStartPt(self):
        """ Return the starting point for this line DrawingObject.
        """
        return self.startPt


    def SetEndPt(self, endPt):
        """ Set the ending point for this line DrawingObject.
        """
        self.endPt = endPt


    def GetEndPt(self):
        """ Return the ending point for this line DrawingObject.
        """
        return self.endPt


    def SetText(self, text):
        """ Set the text for this DrawingObject.
        """
        self.text = text


    def GetText(self):
        """ Return this DrawingObjects text.
        """
        return self.text


    def SetTextFont(self, font):
        """ Set the typeface for this text DrawingObject.
        """
        self.textFont = font


    def GetTextFont(self):
        """ Return this text DrawingObjects typeface.
        """
        return self.textFont


    def SetTextSize(self, size):
        """ Set the point size for this text DrawingObject.
        """
        self.textSize = size


    def GetTextSize(self):
        """ Return this text DrawingObjects text size.
        """
        return self.textSize


    def SetTextBoldface(self, boldface):
        """ Set the boldface flag for this text DrawingObject.
        """
        self.textBoldface = boldface


    def GetTextBoldface(self):
        """ Return this text DrawingObjects boldface flag.
        """
        return self.textBoldface


    def SetTextItalic(self, italic):
        """ Set the italic flag for this text DrawingObject.
        """
        self.textItalic = italic


    def GetTextItalic(self):
        """ Return this text DrawingObjects italic flag.
        """
        return self.textItalic


    def SetTextUnderline(self, underline):
        """ Set the underling flag for this text DrawingObject.
        """
        self.textUnderline = underline


    def GetTextUnderline(self):
        """ Return this text DrawingObjects underline flag.
        """
        return self.textUnderline

    def IsClickable(self):
	return self.clickable
    def GetOriginalSize(self):
	return self.originalsize #QSize object; (w,h)
    def GetMacroID(self):
	return self.macroid
    def SetMacroID(self,macroid):
	self.macroid = macroid #ID of the macro device in the given level


    def SetObject(self, object):
        """ Assign data object to this visual object; Device for obj_NODe
	and Link for obj_LINK.

	@object - the data object to be assigned to this object.
        """
	if self.type == obj_NODE:
		self.object = Device(object.GetSystem(),object.GetName(),False)
	elif self.type == obj_LINK:
		self.object = Link(object.GetSystem(),object.GetName(),False,False)
		
    def SetObjectRef(self,object):
	""" Assign a data object reference to this visual object; Device for obj_NODE
	and Link for obj_LINK
	"""
	self.object = object
	self.text = self.object.GetName()

    def GetObject(self):
        """ Return object with deviceInfo..reference
        """
        return self.object
        
    def GetName(self):
        """ Return name of visual object, which
	is retrieved from the data object it has
	assigned.
        """
	return self.object.GetName()
        
    def SetFixed(self, fixed):
        """ Set if object is fixed to another object
        """
        self.fixed = fixed

    def IsFixed(self):
        """ Return, wheter object is fixed
        """
        return self.fixed

    def SetPorts(self, portType, nr):
        """ Set the number of ports of port way (only obj_NODE)
	for this object, we have one list for each port way (in
	and out)

	@portType - the port way this list of ports is assigned to
	@nr - nr of ports to assigned to the given port way
        """
        self.portList[portType] = [None for i in range(nr)]

    def GetPorts(self, portType):
        """ Return the number of ports of given port way.

	@portType - port way to get port list for.

	!return - list of ports (port indexes) for given port way.
        """
        return len(self.portList[portType])

    def ClearPorts(self):
        """ Remove the references to links in the port lists
	for this visual object (if obj_NODE).
	"""
        self.SetPorts(port_IN, self.GetPorts(port_IN))
        self.SetPorts(port_OUT, self.GetPorts(port_OUT))

    def SetPortList(self, portType, list):
        """ Set the port list for the given port way.

	@portType - port way to set the port list for
	@list - list of references to links to set for this port way
	for this visual object.
        """
        self.portList[portType] = list

    def GetPortList(self, portType):
        """ Return objects to all links connected to ports at portType
        """
        return self.portList[portType]

    def GetLinks(self):
        """ Return all the references to links in the port lists
	for this visual object, if this is of type obj_NODE

	!return - a list of references to link visual objects referred
	to in the port list of this device
        """
        list = []
        if obj_NODE == self.GetType():
            for obj in self.GetPortList(port_IN) + self.GetPortList(port_OUT):
                if obj: list.append(obj)
        return list

    def FindCorrespondingPortIndex(self,portnbr="",porttype="",portdirection=port_IN):
	""" Find corresponding port index for a port number, port type and port direction
	    collection for a port. This is "calculated" by using specific algorithms for each
	    device type because of the big differences in how the devices are given port number
	    and port types as well as port direction.

	    @portnbr - the port number of the port
	    @porttype - the port type of the port
	    @portdirection - the port direction/way of the port

	    !return - a tuple of port_index and port_number; the port index is the internal
	    indexing of the port in this class, and the port number is the place where it is
	    added to in the port list.
	"""

	#standard indexing, as f.ex. FEEs use
	# IN/OUT PORTS
	port = -1
	i = 0
	for obj in self.GetPortList(portdirection):
		if obj == None: # look for free port
			port = i+1 #portnr, not the index
			break
		i += 1
	if port == -1: #all ports filled up
		print "No free ports found."
		return -1,-1 #both index in portlist and index on device set to -1

	try:
		if self.GetObject().GetType() == "M5R4":
			if porttype.lower() in ["a","b"]:
				if porttype.lower() == "a":
					offset = int(portnbr)/8 * 8
				else:
					offset = (int(portnbr)+8)/8 * 8
				index = int(portnbr) + offset
			elif porttype.lower() == "ab":
				offset = 48 #48 FEE ports
				index = int(portnbr) + offset
			else:
				print "The M5R4 device does not have correct syntax on all ports. Fix this according to the syntax given in the documentation!"
				return -1,-1
		else: 	#standard device with default numbering, like this:
			# __________________
			# | 0 1 2 3 4 5 6 7| #numbering of input ports starts top left from 0
			# |		   |
			# | 0 1 2 3 4 5 6 7| #numbering of output ports starts bottom left from 0
			# ------------------

			index = int(portnbr)

		#print "Returning: " + str(index) + " and " + str(port)
		return index,port
	except ValueError,err:
		print "An error occured while trying to find corresponding index to portnbr, porttype and portdirection. The portnbr for M5R4s must have an integer as portnbr. Correct this, and try to connect."
		return -1,-1
	

    # ========================================
    # == More spezialized get/set-functions ==
    # ========================================
    
    def GetPortPosition(self, index,portdir):
	""" Get the port position on a visual object (obj_NODE) as a QPoint(...) 
	for a given port index.

	@index - port index of the port in the given port list (port way list)
	@portdir - port direction/way for the port.
	
        !return - QPoint of port (obj_NODE only)
        """
        #print "GetPortPosition Called " + str(self.i) + " time"

        if self.GetType() == obj_NODE:
	    if self.GetObject().GetType() == "M5R4":
		#I already know the number and location of the ports
		# INOUTPORTS
		if index >= 0:
			if index < 48: #0-23 A,B FEE OUT
		    		if GetZoomFactor() < 50: #50% zoom
					xPos = self.position.x() + self.size.width()/2
					yPos = self.position.y() + self.size.height()
				else:
					xPos = self.position.x() + self.size.width()*(index+1)/(49)
					yPos = self.position.y() + self.size.height() + self.linkLength
			elif index < 52: #0-3 AB HV IN
		    		if GetZoomFactor() < 50: #50% zoom
					xPos = self.position.x() + self.size.width()
					yPos = self.position.y() + self.size.height()/2
				else:
					xPos = self.position.x() + self.size.width() + self.linkLength - 1
					yPos = self.position.y() + self.size.height() - self.size.height()*(index+1-48)/(5)
			elif index < 54: #4-5 AB GAS IN,OUT
		    		if GetZoomFactor() < 50: #50% zoom
					xPos = self.position.x()
					yPos = self.position.y() + self.size.height()/2
				else:
					xPos = self.position.x() - self.linkLength + 1
					yPos = self.position.y() + self.size.height()*(index+1-52)/(5)
			else:
				print "Portindex is not valid. The given portindex cannot be found for the given devicetype: M5R4"
				return QPoint(-1,-1)
	    else:
		    if GetZoomFactor() >= 50: #50% zoom
			#pdb.set_trace()
			xPos = self.x() + self.width()*(index+1)/(self.GetPorts(portdir)+1) #may fail, if index starts on 1 instead of 0
			if portdir == port_IN:
			    yPos = self.y() - self.linkLength
			else:
			    yPos = self.y() + self.height() + self.linkLength
		    else: #less than 50% zoom
			xPos = self.x() + self.width()/2
			if portdir == port_IN:
			    yPos = self.y()
			else:
			    yPos = self.y() + self.height()
			
            return QPoint(xPos, yPos)
        else:
		return (-1,-1)
                
    def SetSinglePort(self, portType, portNr, link, override = True):
        """ Connect the given link to the specified port of portType.
            

	    @portType - the port way of the port (portIN or portOUT)
	    @portNr - the port number of the port, 1 -> n, internal indexing
	    of the port numbers
	    @link - the visual object link that will be connected to a 
	    port on a visual object device
	    @override - whether we should set this link to the given port
	    even when it is taken by another link.
	    
            !return - True if port is set, else False
        """
        if link.__class__ == myLine and link.GetType() == obj_LINK:
            if portNr > self.GetPorts(portType):
                return False
            if self.portList[portType][portNr-1]: #portnr 1 is stored in the 0th position a.s.o.
                # port already set
                if override:
                    print "WARNING: Overwriting port already connected (port %i (type %i) of %s)" \
                           %(portNr, portType, self.GetName())
                else:
                    # do not set port, leave old connection
                    print  "ERROR: Tried to overwrite port already connected. \
                              (port %i (type %i) of %s). NO overwritign activated" \
                              %(portNr, portType, self.GetName())
                               
                    return False

            self.portList[portType][portNr-1] = link
            return True
           

    def SetConnection(self, partner, portType, portNr, setPartner = True):
        """ Connect two DrawingObjects (links and nodes) together.

	@partner - the link or node that this object should be connected to.
	@portType - the port way they should connect to on the device.
	@portNr - the port number (internal indexing) they should connect to on
	the device
	@setPartner - whether the connection should be set for the partner of this
	as well (this method called for the partner as well)
	
        !return - True if connection is set, else False
        """
        connected = False
        if self.GetType() == obj_NODE:
            if partner.__class__ == myLine and \
               partner.GetType() == obj_LINK and \
               portNr <= self.GetPorts(portType) and \
               portType in [port_IN, port_OUT]:
                    connected = self.SetSinglePort(portType, portNr, partner)
                    if setPartner: 
                        partner.SetConnection(self, portType, portNr, False)
            else: 
                print "Connection failed (node)"
                    
        elif self.GetType() == obj_LINK:
            if partner.__class__ == Box and \
               partner.GetType() == obj_NODE and \
               portNr <= partner.GetPorts(portType) and \
               portType in [port_IN, port_OUT]: #if we have 2 ports on a device, portnr 1 in and portnr 2 out *or opposite*. 1 outports, 1 inports.
                    if portType == port_IN:
                        self.nodeTo = partner
                        self.portTo = portNr
                        self._AdjustLinks(None, partner.GetPortPosition(self.GetPortIndexTo(),port_IN))
                    else: #portType == port_OUT
                        self.nodeFrom = partner
                        self.portFrom = portNr
                        #WAL_OBSELETE ?????  self._AdjustLinks(partner.GetPortPosition(self.GetPortIndexFrom(),port_OUT), None)
                    if setPartner:
                        connected = partner.SetConnection(self, portType, portNr, False)
            else:
                print "Connection failed (link)" 

        else:
            print "Connection failed: wrong type : %s"%str(self.GetType())
        return connected

            
            
    # ============================
    # == Object Drawing Methods ==
    # ============================

    def AddPoints(self, point1, point2):
        """ Add two QPoint objects together.

	@point1 - the first QPoint(...) object
	@point2 - the second QPoint(...) object
	"""
        return QPoint(point1.x() + point2.x(), point1.y() + point2.y())

    def _AdjustLinks(self, startPt = None, endPt = None):
	""" If links are moved or resized; the nodes they
	are connected to needs to be adjusted. If nodes are moved
	or resized; the links that connect tot hem need to be
	adjusted.

	@startPt - starting point QPoint(...) for the object this
	object needs to be adjusted to
	@endPt - end point QPoint(...) for the object this object
	need to be adjusted to
	"""
	
        if self.GetType() == obj_NODE:
		for i in range(len(self.portList[port_IN])):
			link = self.portList[port_IN][i]
			if link:
			    pos = link.GetPosition()
			    start = link.GetStartPt()
			    
			    link._AdjustLinks(QPoint(pos.x()+start.x(), pos.y()+start.y()), self.GetPortPosition(link.GetPortIndexTo(),port_IN))
	        for i in range(len(self.portList[port_OUT])):
			link = self.portList[port_OUT][i]
			if link:
			    pos = link.GetPosition()
			    end = link.GetEndPt()
			    link._AdjustLinks(self.GetPortPosition(link.GetPortIndexFrom(),port_OUT), QPoint(pos.x()+end.x(), pos.y()+end.y()))
	    
        elif self.GetType() == obj_LINK:
            if startPt == None: 
                startPt = self.AddPoints(self.GetPosition(), self.GetStartPt())
            if endPt == None: 
                endPt = self.AddPoints(self.GetPosition(), self.GetEndPt())
            topLeftX  = min(startPt.x(), endPt.x())
            topLeftY  = min(startPt.y(), endPt.y())
            botRightX = max(startPt.x(), endPt.x())
            botRightY = max(startPt.y(), endPt.y())
            
            self.SetPosition(QPoint(topLeftX, topLeftY), False)
            self.SetSize(QSize(botRightX-topLeftX, botRightY-topLeftY), False)
            self.SetStartPt(QPoint(startPt.x() - topLeftX, startPt.y() - topLeftY))
            self.SetEndPt(QPoint(endPt.x() - topLeftX, endPt.y() - topLeftY))
	
    def _delete(self):
        self.i = 0 
        ##del self.type             # = None
        del self.position         # = None
        del self.size             # = None
        del self.penColour        # = None
        del self.fillColour       # = None
        del self.lineSize         # = None
        del self.startPt          # = None
        del self.endPt            # = None
        del self.text             # = None
        del self.layer	      # = None
        del self.textFont         # = None
        del self.textvalign # = None
        del self.texthalign # = None
        del self.textSize         # = None
        del self.textBoldface     # = None
        del self.textItalic       # = None
        del self.textUnderline    # = None
	del self.canvas# = None
        #del self.linkLength       # = None
        del self.fixed            # = None
	del self.zoomFactor	      # = None
	#del self.portindexfrom# = None #index of the port on the given device its connected to the output
	#del self.portindexto# = None #index of the port on the given device it is connected to the input
	del self.object #	= None
	del self.expanded		#= None #always totally collapsed for links, 1 or 2 for nodes if expanded
	del self.clickable# = None
	del self.originalsize# = None
	del self.macroid# = None
	##del self.deviceinfo_tmp # = None
        del self.portList # = None

	
i = 0
class Box(QCanvasRectangle,DrawingObject):

    def __init__(self, canvas, type, object = None, position=QPoint(0,0), size=QSize(3, 3),
                 penColour=Qt.black, fillColour=Qt.white, lineSize=1,
                 text=None,zoomFactor=100,clickable=True,originalsize=QSize(0,0),macroid=-1,textvalign=text_CENTER,texthalign=text_CENTER):
	DrawingObject.__init__(self, canvas, type, object, position, size, penColour, fillColour, lineSize,text, QPoint(0,0), QPoint(0,0),zoomFactor,clickable,originalsize,macroid,textvalign,texthalign)
	QCanvasRectangle.__init__(self, canvas)
	##visWindow.contents.append(self)
	if self.object:
		self.Canvastext = QCanvasText(self.GetName(),canvas)
		self.Canvastext.setTextFlags(Qt.AlignCenter)
		self.Canvastext.setFont(QFont("Arial",10))
		self.setSize(size.width(),size.height())
		self.move(position.x(),position.y())

		fillColour = Color(self.fillColour)
		self.setPen( QPen(QColor(0,0,0)) )
		self.setBrush(QBrush(Color(self.fillColour)))
		if fillColour.red()+fillColour.blue()+fillColour.green() < 200:
			self.Canvastext.setColor(QColor(150,150,150))

    def SetData(self,data):
	DrawingObject.SetData(self,data)
	self.Canvastext = QCanvasText(self.GetName(),self.canvas)
	self.Canvastext.setTextFlags(Qt.AlignCenter)
	self.Canvastext.setFont(QFont("Arial",10,QFont.Bold))
	self.setSize(self.size.width(),self.size.height())
	fillColour = Color(self.fillColour)
	self.setPen( QPen(QColor(0,0,0)) )
	self.setBrush(QBrush(Color(self.fillColour)))
	if self.fillColour.red()+self.fillColour.blue()+self.fillColour.green() < 200:
		self.Canvastext.setColor(QColor(150,150,150))
	
    def setCanvas(self,c):
	try:
	   self.Canvastext.setCanvas(c)
	except:
		print "WARNING : CanvasText not found"
	QCanvasItem.setCanvas(self,c)

    def hide(self):
	QCanvasRectangle.hide(self)
	try:
	   self.Canvastext.hide()
	except:
	   pass
    def _delete(self):
	try:
		#DrawingObject._delete(self)
		self.Canvastext = None
		#del self.nodeFrom
		#del self.nodeTo # = None, None
		self.object = None
		del self.portList #= None
		self.canvas = None
	except:
		pass
	
    def show(self):
	QCanvasRectangle.show(self)
	self.Canvastext.show()
	self.Canvastext.setZ(self.z()+1)
	self.Canvastext.move(self.x()+self.width()/2,self.y()+self.height()/2)
	self.Canvastext.move(self.x()+self.width()/2,self.y()+self.height()/2)

    def moveBy(self,x,y):
	QCanvasRectangle.moveBy(self,x,y)
	self.Canvastext.moveBy(x,y)

    def move(self,x,y):
	QCanvasRectangle.move(self,abs(x)%5000,abs(y)%5000)

    def SetSize(self, size, adjust = True):
	DrawingObject.SetSize(self,size,adjust)
	self.setSize(size.width(),size.height())

    def SetPosition(self, position, adjust = True):
	DrawingObject.SetPosition(self, position, adjust)
	self.move(position.x(),position.y())

    def areaPoints(self):
	points = QPointArray(4)
	A = QPoint(self.x(),self.y())# + self.offset()
	B = QPoint(self.x()+self.width(),self.y()+self.height()) #self.endPoint()# + self.offset()
	margin = 0
	if self.isActive(): margin = 2
	points.setPoint(0,A.x()-margin,A.y()-self.linkLength)
	points.setPoint(1,B.x()+margin,A.y()-self.linkLength)
	points.setPoint(2,A.x()-margin,B.y()+self.linkLength)
	points.setPoint(3,B.x()+margin,B.y()+self.linkLength)
	return points

    def drawShape(self,painter):
	if not (self in visWindow.contents):
		self._delete()
		self.setCanvas(None)
		return
	# special way of drawin specific muon chambers  NOT TESTED YET
	if self.GetObject().GetType() in ["M5R4","M4R4"]: #chambertype M5R4 or M4R4 in the muon system
	  #I already know the number and location of the ports
	  # INOUTPORTS
	  i = 0
	  colour = dc.GetPen()
	  while i < 48: #0-23 A,B FEE OUT
		xPos = position.x + self.size.width*(i+1)/(49)
		if (i > 7 and i < 16) or (i > 23 and i < 32) or (i > 39 and i < 48): #RED - A, GREEN - B
			dc.SetPen(wxGREEN_PEN)
		else:
			dc.SetPen(wxRED_PEN)
		dc.DrawLine(xPos, position.y+self.size.height,xPos, position.y+self.size.height+self.linkLength)
		i += 1

	  dc.SetPen(colour)
	  i = 0
	  while i < 4: #0-3 AB HV IN
		yPos = position.y + self.size.height*(i+1)/(5)
		dc.drawLine(position.x+self.size.width,yPos, position.x+self.size.width+self.linkLength,yPos)
		i += 1
	  i = 0
	  while i < 4: #4-5 AB GAS IN,OUT
		yPos = position.y + self.size.height*(i+1)/(5)
		dc.drawLine(position.x, yPos, position.x-self.linkLength,yPos)
		i += 2
	else: #assume default port numbering is used 
	    ports = self.GetPorts(port_IN)
	    if 2*ports < GetZoomFactor()/100*self.width():
		xPos = self.x()
		for x in range(ports):
			xPos = self.x() +  self.width()*(x+1)/(ports+1)
			painter.drawLine(xPos, self.y()-self.linkLength, xPos, self.y())
	    else:
		painter.save()
		painter.setPen(Qt.NoPen)
		painter.setBrush(QBrush(QColor(200,200,200)))
		painter.drawRect(QRect(QPoint(self.x(),self.y()-self.linkLength),QPoint(self.x()+self.width()-1,self.y()-1)))
		painter.restore()
	    ports = self.GetPorts(port_OUT)
	    if 2*ports < GetZoomFactor()/100*self.width():
		xPos = self.x()
		for x in range(ports):
			xPos = self.x() +  self.width()*(x+1)/(ports+1)
			painter.drawLine(xPos, self.y()+self.height(), xPos, self.y()+self.height()+self.linkLength)
	    else:
		painter.save()
		painter.setPen(Qt.NoPen)
		painter.setBrush(QBrush(QColor(200,200,200)))
		painter.drawRect(QRect(QPoint(self.x(),self.y()+self.height()+self.linkLength),QPoint(self.x()+self.width()-1,self.y()+self.height()-1)))
		painter.restore()
	#painter.save()
	if self.isActive():
		painter.setPen(QPen(Qt.black,2,Qt.DashLine)) #QColor(200,50,0),2))
	if self.GetObject().GetName() in visWindow.main.DoNotShow:
		painter.setBrush(QBrush(painter.brush().color(),Qt.Dense7Pattern))

	QCanvasRectangle.drawShape(self,painter)
	#painter.restore()
	QCanvasRectangle.drawShape(self,painter)

    def dragEnterEvent(self, event):
	event.ignore()
    def dragMoveEvent(self, event):
	event.ignore()

class myLine(QCanvasLine,DrawingObject):
    def __init__(self,canvas,obj_link,object,position=QPoint(2500,2500),size=0,penColour=Qt.black, \
                fillColour=Qt.black,lineSize=1,startPt=QPoint(2500,2500),endPt=QPoint(2500,2500)):
	DrawingObject.__init__(self,canvas,obj_link,object,position,size,penColour,fillColour,lineSize,startPt,endPt)
	QCanvasLine.__init__(self,canvas)
	self.penColour = Color(self.GetObject().GetColor())

    def dragEnterEvent(self, event):
	event.ignore()
    def dragMoveEvent(self, event):
	event.ignore()
    def show(self):
	try:
		self.setPoints(self.nodeFrom.x()+self.from_delta,  self.nodeFrom.y()+self.nodeFrom.height()+self.linkLength,self.nodeTo.x()+self.to_delta ,self.nodeTo.y()-self.linkLength)
		QCanvasLine.show(self)
	except:
		print "There is an error in the Configuration database"

    def drawShape(self, painter):
	if not (self in visWindow.contents) or self.GetObject().GetName() in visWindow.main.DoNotShow:
		self._delete()
		self.setCanvas(None)
		return

	self.setPoints(self.nodeFrom.x()+self.from_delta,  self.nodeFrom.y()+self.nodeFrom.height()+self.linkLength,self.nodeTo.x()+self.to_delta ,self.nodeTo.y()-self.linkLength)
	if self.isActive():
		painter.setPen(QPen(self.penColour,2)) 
	else:
		painter.setPen(QPen(self.penColour,1))
	QCanvasLine.drawShape(self,painter)

    def boundingRect(self):
	#return QRect(self.startPoint(),QPoint(self.startPoint().x()+10,self.startPoint().y()+10))
	return QRect(min(self.startPoint().x(),self.endPoint().x()),min(self.startPoint().y(),self.endPoint().y()), abs(self.startPoint().x()-self.endPoint().x()), abs(self.startPoint().y()-self.endPoint().y()))

    def areaPoints(self):
	points = QPointArray(4)
	A = self.startPoint()
	B = self.endPoint()
	zoomfactor=GetZoomFactor()/100
	delta_x = abs(A.x()-B.x())*zoomfactor
	delta_y = abs(A.y()-B.y())*zoomfactor
	delta = 4*max(delta_x,delta_y)
	points.setPoint(0,A.x(),A.y()-delta)
	points.setPoint(1,B.x(),B.y()-delta)
	points.setPoint(2,B.x(),B.y()+delta)
	points.setPoint(3,A.x(),A.y()+delta)
	return points
    def _delete(self):
	#DrawingObject._delete(self)
	try:
		self.nodeFrom, self.nodeTo = None, None
		self.object = None	
		self.canvas = None	
	except:
		pass
    def setCanvas(self,c):
	QCanvasItem.setCanvas(self,c)