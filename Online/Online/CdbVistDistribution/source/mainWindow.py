""" cdbVis (Configuration DataBase Visualization Tool).

    This program is designed to help human users of the LHCb Configuration 
    Database. It supports them understanding the contents saved in this 
    database by having a visual view on the components, their connections and 
    the parameters and information saved with them.

    This program should give the user a feel of how the system and subsystems of the LHCb
    system is built up and assembled. The user should be able to do all alterings on the system
    in this GUI (Graphical User Interface). The GUI should be very intuitive and easy to use
    for people who are both familiar and unfamiliar with the system.
    
    The first version(s) of this program was written by Felix Schmidt-Eisenlohr during summer 2004
    as work project at the CERN Summer Student Programme 2004. Because the program was far from
    finished, Technical Student Thomas Johansen continued working on this program from August 2005.
"""

from GUImainWindow import *
import ConfigParser
from CreateDeviceType import *
from CreateDevice import *
# from CreateLinkType import *
from CreateLink import *

from sys import *
import csv




# Global configfile reference
configfile = None

##########################################################################
# MainWindow class
##########################################################################

	
class mainWindow(GUImainWindow, dbUpdate):
    """ This is the Main Window of the CdbVis program. 
    """

    def __init__(self, app, parent = None,name = None,fl = 0):
        """ Initialize main window frame.
            @parent -- parent window (usually CdbVis)
        """
	# Appel du constructeur du parent
	# todo :  Set to fixed size: 800x600 at startup, recommended view is full screen.
	GUImainWindow.__init__(self,parent = None,name = None,fl = 0)
	self.connect(self.fileExitAction,SIGNAL("activated()"),self.close)
	
    	self.__cfg = None
	self.app = app
	#status bar
	# la creation de la status bar devra se faire dans la classe : GUImainWindow
	
	# mainPanel : Covers the whole mainWindow except menu bar, tool bar and status bar
 	
	# myLayout = QGridLayout(self.centralWidget(),1,1,5,6," My Layout")
	# self.panel = mainPanel(self,self.centralWidget(),"panelFrame")
	# myLayout.addWidget(self.panel,0,0)
	##########myLayout.setStretchFactor(self.visWindow,1)
	# self.resize(QSize(800,600).expandedTo(self.minimumSizeHint()))
	
	logfilename = "CdbVis.log"
	self.__logsessionfile = os.path.normpath(logfilename)
	
	try:
		file = open(self.__logsessionfile,"r")
		file = open(self.__logsessionfile,"w")
	except IOError,err:
		file = open(self.__logsessionfile,"w") #file does not exist, ignore recover, but create file
	
	file.write("")
	file.close()

	# Should be possible to set by the user whether he/she wants to see error messages in message boxes or in the
	# status bar
	self.__showmsgbox = False
	self.__previouserror = [] # we store the previous error, so that the user can easily access it and view it in a
	# message box if he/she clicks on the message icon in the status bar.

    #methode
	self.ShowError("Configuration Database Visualizer Version " + VERSION, ERR_INFORMATION,False)
	self.pbarvalue = 0
	#zoom toolbar
	self.zoomvalues = [10,15,20,25,33,50,75,100,150,200,400]
	self.__zoomvalue = 100 #default startup value
	self.__zoomindex = 0
	x = 0
	for itm in self.zoomvalues:
		if itm == 100:
			self.__zoomindex = x
		x += 1

	
	#DBSystem object; which contains the connect/disconnect confdb methods
	self.__cdb = None

	# At the moment, not in use. 
	self.__availablesystems = []
	self.__availabledevicetypes = []
	self.__availabledevices = []
	self.__availableports = []
	self.__availablelinktypes = []
	self.__availablelinks = []

	# mapping between name and ids, not used much
	self.__linktypetable = {}
	self.__devicetable = {}

	#what systems, devicetypes,devices etc are active; the current one.
	# Used a lot.
	self.__activesystem = None
	self.__activedevicetype = None
	self.__activedevice = None
	self.__activeport = None
	self.__activelinktype = None
	self.__activelink = None

	# The name of the only link type that should be shown in visual window, or "" if all should be visible.
	self.__restrictedlinktype = ""
	self.__restrictedLinks = False
        self.__restrictedsparepart = ""
	self.__list_sparedevice=[]
	self.__devicenrlist = {} #??
	self.__lastlinknr = 0 #last temporary link name (id) given to a link, so that the next one will be set to
	# self.__lastlinknr + 1

	#other lists
	""" The dirty objects list is a list of all kinds of objects that have been created or modified since the last time the user stored anything in the database. Whenever information is going to be read from objects, we have to see in this list if the object is placed here for creation/modification. Then we will get the objectinfo from this list, remove it from the list, and append it again when it is changed (or not if it is deleted) """
	self.__dirtyobjects = []
	""" Dirty objects, for renaming linktypes, devicetypes or devices. Cannot be a part of dirtyobjects because 
	it is not connected to UNDO and REDO functionality.
	"""
	self.__undolist = [] #information needed for undo
	self.__redolist = [] #Once you undo, the undo object is appended to the redo list. information needed for redo
	self.__redoobjectlist = [] #undone objects, opposite to dirtyobjects list. Can be added back to dirtylist and savelist if REDOed

	self.__visualundolist = [] #On delete, store object here if want to UNDO it later
	self.__visualredolist = [] #When UNDOing a create 

	
	# Rename lists for renamed objects
	self.__renameobjects = [] # Objects of all kinds with the new name and old name set
	self.__oldnames = [] # Names of all kinds of objects that has been renamed (the name before renaming)
	self.__oldnamesrenamed = {} # Names of all kinds of objects that has been renamed; mapping oldname : newname
	self.__newnames = [] # names of all kinds of objects that has been renamed (the name after renaming)
	self.__newnamesrenamed = {} # names of all kinds of objects that has been renamed; mapping newname : oldname

	self.__currentview = 0 #keep track on when views are changed, and whether the
			       #undone/redone objects should be added in visual window
	self.__prevview = ID_SUCCESSORS_MODE #neighbor view

	# other containers etc.
	self.__selectcreatedevtype = "" # device type selected in device combo box, the device to be created will get this
					# deice type

	# set up user mode
	self.__userMode = ID_NAVIGATION_MODE #default
	self.fileStore_object_in_DBAction.setEnabled(False)
	#self.__userMode = ID_CREATION_MODE
	self.__viewmode = ID_SUCCESSORS_MODE #ID_PATHS_MODE  #default, neighbor view
	'''	self.GetStatusBar().SetStatusText("Navigation Mode", 5)
	self.GetStatusBar().SetStatusText("Neighbour View", 6)
	'''
	self.__processubsystems = True #??

	#device types that does not have a colour set
	self.tempdevicetypecolour = {}

	#set up to use tool_SELECT on default
	self.__userModeTool = tool_SELECT # normal arrow in the visual window

	#methode
	self.systemlevels = {"MUON":4,"VELO":2,"VELO_A":1,"VELO_C":1,"MUON_A":3,"MUON_C":3,"RICH1":2,"TT":2,"OT":3,"IT":2,"RICH2":2,"PSSPD":1,"ECAL":1,"HCAL":1,"TFC":1,"DAQ":1,"L0MUON":1,"L0DU":1,"L0CALO":1,"Spare devices":1,"PUS":1,"ST":2,"OT_A":1,"OT_C":1,"RICH":3,"L0TRIGGER":2,"ECS":1,"PRS":1,"":0,None:0 }

	self.layouttype = INV_PARABOLIC
	self.DoNotShow = []  #added by walid : containt the devices to remove


    #############################################
    # Methods					#
    #############################################
    # def DoCloneHelper(self,...):
	# self.pamel.visWindow.DoCloneHelper
    
    
    def GetLayoutType(self):
	""" Returns the current layout type that is used.
	!return - Integer constant for the layout type"""
	return self.layouttype
    
    def SetLayoutType(self,layouttype):
	""" Sets the current layout type that is used for node layout.
	@layouttype - An integer constant defining the layouttype to be used (0->4)"""
	self.layouttype = layouttype

    def GetTempDeviceTypeColour(self,devicetype):
	""" Get a random colour for the given device type, if the device type is not already set. If
	it is set, then return the temporary device type colour for that device type.
	@devicetype - the name of the device type
	!return - temporary colour set for the given device type
	"""
	   
	if devicetype in self.tempdevicetypecolour.keys():
		return self.tempdevicetypecolour[devicetype]
		    
	import random
	random.seed()
	while (True):
		random.random()
		r = random.randint(0,255)
		g = random.randint(0,255)
		b = random.randint(0,255)

		if (r < 20 and g < 20 and b < 20):
			continue
		else:
			break

	self.tempdevicetypecolour[devicetype] = "( " + str(r) + ", " + str(g) + ", " + str(b) + " )" #QColor(r,g,b)
	
	return self.tempdevicetypecolour[devicetype]
	
    def EnableDisableLevelDown(self):
	""" Enable or disable the 'go down one level' toolbar button. It should only
	be enabled if it is just above the device level; so that a click on that button
	should lead you to the device level
	"""
	
	if self.systemlevels[self.GetActiveSystem()] == self.GetVisWindow().GetLevelZoom(): # Last level?
		self.GetToolBar().EnableTool(LEVEL_DOWN_BTN,True) # If yes: Enable
	else:
		self.GetToolBar().EnableTool(LEVEL_DOWN_BTN,False) # If no: Disable

    def EnableDisableZoomAndScroll(self):
	""" Zoom and scroll should only be enabled when we are in zoom level -1 (device level), otherwise it should
	be disabled.
	"""
	if self.GetVisWindow().GetLevelZoom() == -1: #ok, enable
		self.GetToolBar().EnableTool(ZOOMIN_BUTTON,True)
		self.GetToolBar().EnableTool(ZOOMOUT_BUTTON,True)
		self.GetToolBar().EnableTool(CUSTOMZOOM_BUTTON,True)
		self.GetMenuBar().Enable(ZOOMMENU,True)
		self.GetVisWindow().EnableScrolling(True,True) #doesnt work
	else: #not ok, disable
		self.GetToolBar().EnableTool(ZOOMIN_BUTTON,False)
		self.GetToolBar().EnableTool(ZOOMOUT_BUTTON,False)
		self.GetToolBar().EnableTool(CUSTOMZOOM_BUTTON,False)
		self.GetMenuBar().Enable(ZOOMMENU,False)
		if self.GetZoomValue() != 100:
	    		self.OnZoom(100,False)
    		self.SetZoomValue(100)
		self.GetVisWindow().EnableScrolling(False,False) #doesnt work


    def OnDownOneLevel(self,event):
	""" When the user clicks on the 'go down one level' button. So far it only works for the
	Muon subsystem, and should perhaps be limited to this subsystem.
	!return - False on error
	"""
	
	if self.systemlevels[self.GetActiveSystem()] == self.GetVisWindow().GetLevelZoom(): # last level?
		if self.GetActiveSystem() == "MUON":
			self.GetVisWindow().SetLevelZoom(-1) # go to device level
			# Get name of the device shown at the last level to show it in device level
			devname = self.GetVisWindow().GetLevelSelection(self.systemlevels[self.GetActiveSystem()])

			if devname in [False,[],""]: # If not set...fail!
				self.ShowError("Failed to show a general device view of the given chamber, because of an database error.",ERR_ERROR)
				return False

			# Create device
			device = Device(self.GetActiveSystem(),devname,False)
			self.SetActiveDevice(device.GetName())
			self.GetVisWindow().Clear()
			self.GetVisWindow().ResetMacroObjects()
			
			# And show it in the current view
			if self.GetViewMode() == ID_SUCCESSORS_MODE:
				self.GetVisWindow().OnDeviceSelected(device)
			elif self.GetViewMode() == ID_PATHS_MODE:
				self.UpdatePath(device)
			elif self.GetViewMode() == ID_DYNAMIC_MODE:
				self.GetVisWindow().OnDeviceSelected(device,False)
			elif self.GetViewMode() == ID_SUBSYSTEM_MODE:
				if self.NeedToDrawSubsystem():
					self.ViewSubSystem(device.GetName())
				else:
					self.GetVisWindow().OnDeviceSelected(device,False)
		else:
			self.GetVisWindow().SetLevelZoom(-1) # default for other systems...
			#self.ShowError("Only the Muon subsystem has support for this functionality.",ERR_ERROR)
			#return False


    def OnUpOneLevel(self,event):
	""" When the user clicks on the 'go up one level' toolbar button. This is accessible
	from most levels; except in device level if you have navigated to the device in the treeview.
	Because then it will not always know (at least in the Muon subsystem) what to show in the
	levels above.

	!return - False on error.
	"""
	
	level = self.GetVisWindow().GetLevelZoom() # Get levelzoom

	# greater than level 0 (lhcb system level)
	if level >= 0:
		if level == 0: # if lhcb system level, first clear the window
			self.GetVisWindow().Clear()

		self.GetVisWindow().ResetMacroObjects()
		self.GetVisWindow().SetLevelZoom(level-1) # f.ex. level 0 (lhcb system) take you to level -1 (device level)
	elif level == -1: #default level, device level -> go one up if we have history information for that.
		levelinfo = self.GetVisWindow().GetLevelInfo()
		if levelinfo == False:
			return False
		try:
			if self.GetActiveSystem() == "MUON" and not self.GetVisWindow().IsChamberFoundAtLocation():
				if len(levelinfo) >= 3:
					self.GetVisWindow().ResetMacroObjects()
					self.GetVisWindow().SetLevelZoom(2)
				else:
					return False
			else:
				if len(levelinfo) > self.systemlevels[self.GetActiveSystem()]:
					self.GetVisWindow().ResetMacroObjects()
					self.GetVisWindow().SetLevelZoom(self.systemlevels[self.GetActiveSystem()])
				else:
					return False
		except KeyError,err:
			self.GetVisWindow().ResetMacroObjects()
			self.GetVisWindow().SetLevelZoom(0)
			
	else:
		self.ShowError("Level " + str(level) + " is not a known level.",ERR_ERROR)
		return False

    def GetNewDeviceNr(self,devicetype):
	""" (Obsolete). Was used to generate temporary device names. Now we generate
	temporary device names (if needed) using the same as for generating temporary
	link names.
	@devicetype - name of the device type to create a temporary devicename for
	"""
	
	try:
		tmp = self.__devicenrlist[devicetype]	#devicetype is here _the_ string name
		self.__devicenrlist[devicetype] += 1
	except KeyError,err:
		#First device of this devicetype, assume we start at 0
		self.__devicenrlist[devicetype] = 0

	return self.__devicenrlist[devicetype]

    def GetNewLinkNr(self):
	""" Creates a new unique session id for a link nr. We start at 0, and increases it by 1
	every time we create a new temporary link name for the current session. The link names
	is put together with the prefixed string 'temporary_link_'.

	!return - returns a new unique link name in this session.
	"""
	
	self.__lastlinknr += 1 #inc first,then return new ID
	return self.__lastlinknr

    def SetPreviousLinkNr(self):
	""" In case a link creation is UNDOne, we free up the link names
	it used as well by decreasing by one for each link UNDOne.
	"""
	
	self.__lastlinknr -= 1

    def GetNextDeviceID(self,system,devicetype):
	""" This method tries to guess the syntax of devices of a given device type
	available in one or several subsystems, so that new devices that are created with
	duplicate or clone methods automatically can get the names they should have, or at
	least with the correct syntax.

	TODO: This method is quite slow if you run it for each device, and you duplicate/clone
	maybe 100 devices. Therefore this method should be changed to generate deviceIDs for
	a number of devices; the number of devices of a given type that will be created. So that
	this method will only run once for each device type duplication/cloning.

	For a more close explanation of how the function works, have a look in the user documentation
	or code documentation for the CreateDevice(...) class.

	@system - the subsystem(s) the device type is available in
	@devicetype - the device type of the devices to be duplicated.

	!return - the guessed deviceID for the next device of the given device type, if found, or
	a temporary device name such as 'device_%d'. Returns False on error.
	"""
	
	devtype = DeviceType(system,devicetype,False)
	devs = devtype.GetDevices() #get all devices of given devtype
	if devs == False:
		#an error occured
		return False

	devs.sort()
	prefixvalue = devs[0] #lowest id

	syntax = prefixvalue
	if syntax == "":
		return False #cannot happen
	if devs == []:
		return False #cannot happen
																			       
	i = len(syntax)-1
	nrofindexint = []
	prevwasint = False
	while i >= 0:
		try:
			nr = int(syntax[i])
			nrofindexint.insert(0,i)
			prevwasint = True
		except ValueError,err:
			#print str(syntax[i]) + " is not an int"
			if prevwasint: #we found the whole integer string
				break
		i -= 1
																			       
	#Get nr substring
	start = nrofindexint[0]
	end = nrofindexint[len(nrofindexint)-1]
	if start == end:
		substring = syntax[start]
	else:
		substring = syntax[start:end+1]
																			       
	#Get start and end substring; before and after nr
	if nrofindexint[0]-1 >= 0:
		startstr = syntax[0:nrofindexint[0]]
	else:
		startstr = ""
																			       
	if len(syntax)-1 > nrofindexint[len(nrofindexint)-1]:
		endstr = syntax[nrofindexint[len(nrofindexint)-1]+1:len(syntax)]
	else:
		endstr = ""
																			       
	#Set prefix
	prefix = startstr + "%d" + endstr #we have found the prefix, we want to change %d with the new index
	#self.__prefixtxtbox.SetValue(prefix)
																			       
	#NB! Test padding
	padding = False
	paddingnr = 0
	if len(str(int(substring))) != len(str(substring)): #we have padding
		nrofdigits = len(str(substring))
		padding = True
		paddingnr = nrofdigits
																			       
	nrs = []
	#print "Start: " + str(startstr) + " End: " + str(endstr)
	for dev in devs:
		if startstr != "":
			dev = dev.replace(startstr,"")
		if endstr != "":
			dev = dev.replace(endstr,"")
																			       
		try:
			nrs.append(int(dev))
		except ValueError,err:
			self.ShowError("No support for several %ds in the same name string yet. You will have to set the other %d yourself!",ERR_ERROR)
			return False
																			       
	nrs.sort()
	next = nrs[len(nrs)-1]+1

	nrlength = len(str(next)) #length of our number
	if nrlength > paddingnr and padding:
		self.ShowError("You have exceeded the maximum number of digits that you set as a maximum for the given device type.",ERR_ERROR)
		return False
	elif padding:
		padstr = "".zfill(paddingnr-nrlength)
	else:
		padstr = ""
		#print "No padding"

	devname = startstr + str(padstr) + str(next) + endstr
	if devname in devs:
        	self.ShowError("The device " + str(devname) + " already exists. You have to change the range or delete the device with the same name in order to save a new one with that name.",ERR_ERROR)
                return False

	return devname

    def OnDevTypeForCreationSelected(self,event):
	""" Set the device type chosen in the combo box to be the one
	chosen if user choose to drag and drop a device in the visual window.
	"""
	
	self.__seldevtypeforcreation = event.GetString()
	
    def OnLinkTypeForCreationSelected(self,event):
	""" Set the link type chosen in the combo box to be the one
	chosen if the user clicks on the Connect button to connect two
	devices together in the visual window.
	"""
	
	self.__sellinktypeforcreation = event.GetString()

    def OnDragInit(self,event):

	""" When the user clicks down the left mouse button on the device button on the tool bar
	the drag and drop has started; and is initialized here. We add data to the drag-and-drop
	action (device type chosen).
	"""

	if event.GetId() == DEVTYPE_BUTTON: 
		# string syntax: SUBSYSTEM|DEVICETYPE|Device
		# The device indicates that it is a device drag and drop action that is happening;
		# others can easily be added later.
		my_drag_value = self.GetActiveSystem() + "|" + self.__devtypecombobox.GetValue() + "|Device"
		tdo = wxPyTextDataObject(my_drag_value)
		tds = wxDropSource(self.__devtypebutton)

		tds.SetData(tdo)
		tds.DoDragDrop(True)

    def OnConnectDevices(self,event):
	""" The user clicks on the connect button in the tool bar to connect two devices together.
	We check that two devices are selected in the visual window, if yes we pop up the Create link
	window.

	!return - False on error.
	"""
	
	selected_devs = self.GetVisWindow().GetSelection()
	if len(selected_devs) != 2: # two visual objects in selection?
		self.ShowError("You have to choose _two_ devices to connect together.",ERR_ERROR)
		return False

	for dev in selected_devs:
		if dev.GetType() != obj_NODE: # two DEVICES in selection?
			self.ShowError("You have to choose two _devices_ to connect together.",ERR_ERROR)
			return False

	self.SetActiveLinkType(self.__linktypecombobox.GetValue())
	# show window with parameters set for devices selected.
    	self.CreateLinkClick(None,obj1=selected_devs[0].GetObject(),obj2=selected_devs[1].GetObject())
	
    def SetCurrentTool(self,tool):
	""" Change the tool used in the visual window. The tool defines whether we are
	viewing or altering things in the visual window. The cursor is used for navigation mode.

	@tool - a constant (tool_SELECT or tool_EDIT) defining whether what we will do with the visual objects
	"""
	
	self.__userModeTool = tool
	self.GetVisWindow().SetCurrentTool(tool)

    def GetCurrentTool(self):
	""" Get currently used tool in the visual window.

	!return - tool (constant) that is currently used in the visual window.
	"""
	
	return self.__userModeTool

    def EnableAutoZoom(self,event=False,enable=False):
	""" Menubar check item to turn on and off auto zoom. """

	if event == False: #Not caused by event, toggling of check has to be handled manually
		#if self.GetMenuBar().IsChecked(ID_ENABLEAUTOZOOM):
		if enable == False:
			self.GetVisWindow().EnableAutoZoom(False)
		    	self.GetMenuBar().Check(ID_ENABLEAUTOZOOM,False)
		else:
			self.GetVisWindow().EnableAutoZoom(True)
		    	self.GetMenuBar().Check(ID_ENABLEAUTOZOOM,True)
			self.GetVisWindow().OnPaintEvent(None)
	else: #user clicked on menu item
		if self.GetMenuBar().IsChecked(ID_ENABLEAUTOZOOM):
			self.GetVisWindow().EnableAutoZoom(True)
			self.GetVisWindow().OnPaintEvent(None)
		else:
			self.GetVisWindow().EnableAutoZoom(False)


    def ZoomIn(self,event=None):
	""" Zoom in in the visual window one factor (the factors are set in the 
	self.zoomvalues list).
	"""
	self.panel.visWindow.ZoomIn()
	
    	# self.EnableAutoZoom(False,False)
	# if self.__zoomindex >= len(self.zoomvalues)-1:
		# wxBell() #cannot zoom in more
		# self.ShowError("To zoom in more than 400%, choose custom zoom.",ERR_INFORMATION,False)
		# self.__zoomindex = len(self.zoomvalues)-1
	# else:
		# self.__zoomindex += 1

	# self.OnZoom(self.zoomvalues[self.__zoomindex],True) # Make the zoom happen.
	

    def ZoomOut(self,event=None):
	""" Zoom out in the visual window one factor (the factors are set in the
	self.zoomvalues list)
	"""
	self.panel.visWindow.ZoomOut()

	#self.GetVisWindow().EnableAutoZoom(False)
    	# self.EnableAutoZoom(False,False)
	# if self.__zoomindex <= 0:
		# wxBell() #cannot zoom out more
		# self.ShowError("To zoom out less than 10%, choose custom zoom.",ERR_INFORMATION,False)
		# self.__zoomindex = 0
	# else:
		# self.__zoomindex -= 1

	# self.OnZoom(self.zoomvalues[self.__zoomindex],True) # Make the zoom happen
	
    def ZoomCustom(self,event=None):
	""" Zoom in or out in the visual window using a custom zoom given as a number in 
	a input text box.
	"""
	
	#self.GetVisWindow().EnableAutoZoom(False)
    	self.EnableAutoZoom(False,False)
	nr = wxGetNumberFromUser("Type in a custom zoom value in percent, 100% is original/default size:","Zoom in %:","Custom Zoom",100,1,1000,self)
	if nr < 0:
		self.ShowError("You typed an invalid number.",ERR_INFORMATION,False)
	elif nr == 0:
		self.ShowError("You cannot zoom to 0%. Then it is not visible!",ERR_INFORMATION,False)
	else:
		self.OnZoom(nr,True) # Make the zoom happen
	
		self.__zoomindex = -1
		i = 0
		for itm in self.zoomvalues:
			if itm <= nr:
				self.__zoomindex = i
			else:
				break
			i += 1

    def OnZoom(self,zoom,paintevent=True):
	""" Calls the zoom function in the visual window to change to the zoom given.

	@zoom - the zoom factor in percent
	@paintevent - whether a paintevent has to be triggered to redraw the contents of the visual window.

	!return - True on success, False otherwise.
	"""
	
	#STATUS_CALL self.GetStatusBar().SetStatusText(str(zoom) + "%",4)

	if zoom == self.__zoomvalue:
		self.ShowError("The zoom value you chose is already the current one!",ERR_INFORMATION,False)
		return False
	else:
		self.__zoomvalue = zoom
		self.GetVisWindow().Zoom(self.__zoomvalue,paintevent)
		return True

    def SetZoomValue(self,value):
	""" Set the currently zoom value (as it is in the visual window) to the 
	member variable in this class (visual window instance sets the value once it has
	successfully changed the zoom of the visual window contents)

	@value - the zoom factor in percent.
	"""
	
	self.__zoomvalue = value
	i = 0
	for itm in self.zoomvalues:
		if itm == value:
			self.__zoomindex = i
		i += 1

	self.GetStatusBar().SetStatusText(str(self.__zoomvalue) + "%",4) # Show zoom value in the status bar

    def GetZoomValue(self):
	return self.__zoomvalue
	
    def GetViewMode(self):
	""" Get the current view: neighbour view, path view, dynamic link view or subsystem view.

	!return -  the constant for the current view
	"""
	
	return self.__viewmode

    def SetCreationMode(self,event):
	if event:
		self.__userMode = ID_CREATION_MODE
		self.myStatusbar.modeIcon.setPixmap(self.myStatusbar.imageCreation)
		self.myStatusbar.mode.setText("Creation Mode")
		self.fileStore_object_in_DBAction.setEnabled(True)
	else:
		self.__userMode = ID_NAVIGATION_MODE
		self.myStatusbar.modeIcon.setPixmap(self.myStatusbar.imageNavigation)
		self.myStatusbar.mode.setText("Navigation Mode")
		self.fileStore_object_in_DBAction.setEnabled(False)

    def OnPathView(self,event):
	if event:
		self.__viewmode = ID_PATHS_MODE
	else:
		self.__viewmode = ID_SUCCESSORS_MODE  # default

    def OnCreateLinkMode(self,event):
	if event:
		self.__viewmode = ID_PATHS_MODE
	else:
		self.__viewmode = ID_SUCCESSORS_MODE  # default
		
    def OnChangeViewT(self,event):
	""" If the view was changed by the user from the toolbar; this is called
	in order to change the check mark in the menu bar for the currently chosen view.
	"""

	viewmode = ""

	if self.GetToolBar().GetToolState(NEIGHBOUR_BUTTON):
		self.__viewmode = ID_SUCCESSORS_MODE
		self.GetMenuBar().Check(ID_VIEW_SUCCESSORS,True)
		viewmode = "Neighbour View"
	if self.GetToolBar().GetToolState(PATH_BUTTON):
		self.__viewmode = ID_PATHS_MODE
		self.GetMenuBar().Check(ID_VIEW_PATHS,True)
		viewmode = "Path View"
	if self.GetToolBar().GetToolState(DYNAMICLINK_BUTTON):
		self.__viewmode = ID_DYNAMIC_MODE
		self.GetMenuBar().Check(ID_VIEW_DYNAMIC,True)
		viewmode = "Dynamic Link View"
	if self.GetToolBar().GetToolState(SUBSYSTEM_BUTTON):
		self.__viewmode = ID_SUBSYSTEM_MODE
		self.GetMenuBar().Check(ID_VIEW_SUBSYSTEM,True)
		viewmode = "Subsystem View"
		self.__processubsystems = True
	
	self.GetStatusBar().SetStatusText(viewmode, 6)



    def OnChangeViewM(self,event):
	""" If the view was changed by the user from the menu bar; this is called
	in order to change the depressed button on the tool bar for the currently chosen view.
	"""

	viewmode = ""

	if self.GetMenuBar().IsChecked(ID_VIEW_SUCCESSORS):
		self.__viewmode = ID_SUCCESSORS_MODE
		self.GetToolBar().ToggleTool(NEIGHBOUR_BUTTON,True)
		self.GetToolBar().ToggleTool(PATH_BUTTON,False)
		self.GetToolBar().ToggleTool(DYNAMICLINK_BUTTON,False)
		self.GetToolBar().ToggleTool(SUBSYSTEM_BUTTON,False)
		viewmode = "Neighbour View"
	elif self.GetMenuBar().IsChecked(ID_VIEW_PATHS):
		self.__viewmode = ID_PATHS_MODE
		self.GetToolBar().ToggleTool(PATH_BUTTON,True)
		self.GetToolBar().ToggleTool(DYNAMICLINK_BUTTON,False)
		self.GetToolBar().ToggleTool(SUBSYSTEM_BUTTON,False)
		self.GetToolBar().ToggleTool(NEIGHBOUR_BUTTON,False)
		viewmode = "Path View"
	elif self.GetMenuBar().IsChecked(ID_VIEW_DYNAMIC):
		self.__viewmode = ID_DYNAMIC_MODE
		self.GetToolBar().ToggleTool(DYNAMICLINK_BUTTON,True)
		self.GetToolBar().ToggleTool(SUBSYSTEM_BUTTON,False)
		self.GetToolBar().ToggleTool(PATH_BUTTON,False)
		self.GetToolBar().ToggleTool(NEIGHBOUR_BUTTON,False)
		viewmode = "Dynamic Link View"
	elif self.GetMenuBar().IsChecked(ID_VIEW_SUBSYSTEM):
		self.__viewmode = ID_SUBSYSTEM_MODE
		self.GetToolBar().ToggleTool(SUBSYSTEM_BUTTON,True)
		self.GetToolBar().ToggleTool(DYNAMICLINK_BUTTON,False)
		self.GetToolBar().ToggleTool(PATH_BUTTON,False)
		self.GetToolBar().ToggleTool(NEIGHBOUR_BUTTON,False)
		viewmode = "Subsystem View"
		self.__processubsystems = True
	
	#no other options available

	self.GetStatusBar().SetStatusText(viewmode, 6)

    def GetUserMode(self):
	""" Get the constant for the currently chosen user mode set in the CdbVisCore module.

	!return - the constant for the currently chosen user mode (ID_NAVIGATION_MODE or ID_CREATION_MODE)
	"""
	
	return self.__userMode #ID_NAVIGATION_MODE or ID_CREATION_MODE

    def SetUserMode(self,event):
	""" Change the user mode; and only make the functionality that is accessible in the new user mode to be
	accessible, the rest should be disabled and inaccessible.

	!return - False if unsuccessful; and user mode not changed.
	"""
	
	if self.GetToolBar().GetToolState(NAVIGATE_TOGGLE_BUTTON):
    		if not self.IsSaveListEmpty():
			ans = QMessageBox.question(self,"Save Changes?","You have to save changes before you go back to Navigation Mode. Save Changes?","Yes","No","",0,1)
			#ans = wxMessageBox("You have to save changes before you go back to Navigation Mode. Save Changes?","Save Changes?", wxYES_NO | wxCANCEL | wxICON_QUESTION)
			if ans == 0: # Yes
				#save changes, if successful; goto navigation mode; if not 
				res = self.OnSave(None,False)
				if res == False:
					self.ShowError("Could not store all changes in confDB. If you still want to change to navigation mode, you will have to change without storing the changes made.",ERR_ERROR)
					##self.GetToolBar().ToggleTool(NAVIGATE_TOGGLE_BUTTON, False)
					##self.GetToolBar().ToggleTool(CREATE_TOGGLE_BUTTON, True)
					return False
				
			elif ans == 1: # No
				#do not save changes, discard, and continue
				##self.ProgressData(len(self.__undolist),False,0)
				while len(self.__undolist) != 0:
					self.DoUndo(None,False)
					##self.ProgressData(0,False,1)
				##self.ProgressData(0,True,0)
			else: # cancel pressed
				#user changed his mind, do not save and do not change mode...return
				#or should navigation mode button be disabled until save
				##self.GetToolBar().ToggleTool(NAVIGATE_TOGGLE_BUTTON, False)
				##self.GetToolBar().ToggleTool(CREATE_TOGGLE_BUTTON, True)
				return False

		self.ResetDirtyObjects() #no undo possible

		# Set the edit menu with undo and redo to be empty
		'''menu = wxMenu()
		menu.Append(ID_UNDO,"&Undo\tCtrl-Z","")
		menu.Append(ID_REDO,"&Redo\tCtrl-R","")
		self.GetMenuBar().Replace(1,menu,"&Edit")

		self.GetMenuBar().Enable(ID_UNDO,False)
		self.GetMenuBar().Enable(ID_REDO,False)

		# disable/enable functions etc. for the current user mode
		self.GetMenuBar().Enable(ID_STORE_OBJS,False)
		self.GetMenuBar().Enable(ID_MASSINSERT,False)
		self.GetMenuBar().Enable(ID_GENERATESQL,False)
'''
		self.__userMode = ID_NAVIGATION_MODE
	        ##self.GetStatusBar().SetStatusText("Navigation Mode", 5)
		self.SetCurrentTool(tool_SELECT)

		'''self.GetToolBar().EnableTool(NAVIGATE_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(CREATE_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(COPY_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(PASTE_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(DELETE_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(SAVE_TOGGLE_BUTTON,False) #possible to change between modes and save at any time?

		self.GetToolBar().EnableTool(LEVEL_DOWN_BTN,True)
		self.GetToolBar().EnableTool(LEVEL_UP_BTN,True)

		self.GetToolBar().EnableTool(ZOOMIN_BUTTON,True)
		self.GetToolBar().EnableTool(ZOOMOUT_BUTTON,True)
		self.GetToolBar().EnableTool(CUSTOMZOOM_BUTTON,True)
		
		self.__devtypecombobox.Enable(False)
		self.__devtypebutton.Enable(False)

		self.__linktypecombobox.Enable(False)
		self.__linktypebutton.Enable(False)

		self.GetMenuBar().EnableTop(3,False) #create
		self.GetMenuBar().EnableTop(4,False) #modify
		self.GetMenuBar().EnableTop(5,False) #delete
		
		# visual menu
		self.GetMenuBar().Enable(ZOOMMENU,True)
		self.GetMenuBar().Enable(SELECTIONMENU,True)
		self.GetMenuBar().Enable(OBJECTMENU,False)
		'''
	else: #	CREATE_TOGGLE_BUTTON

		self.__userMode = ID_CREATION_MODE
	        ##self.GetStatusBar().SetStatusText("Creation Mode", 5)
		##self.SetCurrentTool(tool_EDIT)
		
    		self.RecoverObjectFromFile() #recover objects, if any

		# enable/disable functions etc. for the current user mode
		'''self.GetToolBar().EnableTool(NAVIGATE_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(CREATE_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(COPY_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(PASTE_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(DELETE_TOGGLE_BUTTON,True)
		self.GetToolBar().EnableTool(SAVE_TOGGLE_BUTTON,True) #possible to change between modes and save at any time?

		self.GetToolBar().EnableTool(ZOOMIN_BUTTON,True)
		self.GetToolBar().EnableTool(ZOOMOUT_BUTTON,True)
		self.GetToolBar().EnableTool(CUSTOMZOOM_BUTTON,True)

		self.GetToolBar().EnableTool(LEVEL_DOWN_BTN,True)
		self.GetToolBar().EnableTool(LEVEL_UP_BTN,True)

		self.__devtypecombobox.Enable(True)
		self.__devtypebutton.Enable(True)

		self.__linktypecombobox.Enable(True)
		self.__linktypebutton.Enable(True)

		self.GetMenuBar().EnableTop(3,True) #create
		self.GetMenuBar().EnableTop(4,True) #modify
		self.GetMenuBar().EnableTop(5,True) #delete
		
		# visual menu
		self.GetMenuBar().Enable(ZOOMMENU,True)
		self.GetMenuBar().Enable(SELECTIONMENU,True)
		self.GetMenuBar().Enable(OBJECTMENU,True)

		# TODO: Only enable if there is something to store in DB. (Dirty List != [])
		self.GetMenuBar().Enable(ID_STORE_OBJS,True)
		self.GetMenuBar().Enable(ID_MASSINSERT,True)
		self.GetMenuBar().Enable(ID_GENERATESQL,True)
		'''
    def EnableDisableExportText(self,enable=False):
	""" Enables/Disables the export to text option in the File menu
	This should only be active when we are viewing a sub-system
	"""

	#WALPROB self.GetMenuBar().Enable(ID_EXPORT_TEXT,enable)


    def SetCreateDeviceType(self,devtype):
    	""" Set the device type to the given device type to be
	chosen if user choose to drag and drop a device in the visual window.

	@devtype - the device type name; to be used next time a device is created
	"""

	self.__selectcreatedevtype = devtype
	
    def GetCreateDeviceType(self):
	""" Get the device type a device will be created of. Device type chosen from combo box
	on tool bar.

	!return - the name of the device type currently chosen in the combo box in the tool bar.
	"""
	
	return self.__selectcreatedevtype

    def WhenChangeView(self):
	""" If the view changes we keep track of this in a variable; cause when a new device is selected to be viewed
	in the visual window, a undo/redo which causes visual objects to be created should not be added to the visual window,
	if the view is now of different devices than it was when the undo/redo happened the first time.
	"""
	
	self.__currentview += 1
	
    def GetCurrentView(self):
	""" Get current view; the integer telling whether we have changed the device to view in neighbor or path view.
	"""
	
	return self.__currentview

    def GetPrevView(self):
	""" When the current view changes, the previous view is set here. So that we can compare current and previous view,
	and thereafter make a decision whether we should add the undone/redone objects to the visual window or not.
	"""
	
	return self.__prevview

    def SetPrevView(self,prev):
	""" Set the previous view (the view that was the current view before it changed).
	"""
	
	self.__prevview = prev
	
    def DoRedo(self,event):
	""" REDOES the previously UNDONE action. Does not take any in parameters, but looks in different lists for input.
	"""

	self.ShowError("Please wait while REDOing " + str(self.__redolist[len(self.__redolist)-1][1]),ERR_INFORMATION,False)

	save_tmp = [] #a list where we save earlier instances of objects in the dirty list if we add a newer one to the dirtylist

	# Get Redo action from redo list
	tmp_redo = self.__redolist.pop()

	# And add it to the UNDO list, so that the redone action can be undone later..if wanted
	self.__undolist.append([len(self.__dirtyobjects),tmp_redo[1],tmp_redo[2]])

	# Add the objects in the redo object list to the dirty objects list
	tmp_listredo = [] # We add all redo object to this list, and iterate it afterwards
	i = tmp_redo[0]
	while i < len(self.__redoobjectlist):
		tmp_redoobj = self.__redoobjectlist.pop(i)
		fromdirtyobjs = self.FindDirtyObject(tmp_redoobj.GetName(),tmp_redoobj.__class__)
		if fromdirtyobjs:
			save_tmp.append(fromdirtyobjs)
		self.AddToDirtyObjects(tmp_redoobj,fromredoundo=True)
		tmp_listredo.append(tmp_redoobj)

	    	self.RecoverObjectToFile(tmp_redoobj,state=REDO) # Save that we REDID an object to the recover file

	self.ProgressData(len(tmp_listredo),False,0)

	for itm in tmp_listredo: # Iterate through REDID objects, make changes happen in visual window if needed (and treeview)
		
		#TreeView Changes
		if itm.__class__ == Device:
			parent = itm.GetType()
			type = TN_DEVICE
		elif itm.__class__ == Link:
			parent = itm.GetType()
			type = TN_LINK
		elif itm.__class__ == DeviceType:
			parent = "Device types"
			type = TN_DEVICETYPE
		elif itm.__class__ == LinkType:
			parent = "Link types"
			type = TN_LINKTYPE
		elif itm.__class__ == Port:
			parent = itm.GetDevice()
			type = TN_PORT
		elif itm.__class__==Spare_Port:
                        parent=itm.GetSpareDevice()
                        type = TN_PORT
                elif itm.__class__==Spare_Device:
                        parent=itm.GetHWType()
                        type=TN_DEVICE

		# Redo delete from treeview -> remove node
		if itm.GetSaveStatus() == DELETE: #Redo delete
			try:
        	        	my_id = self.GetSelectWindow().GetItemID(itm.GetName())
				self.GetSelectWindow().RemoveItem(my_id,parent)
				self.GetSelectWindow().SetItemToDeleted(itm.GetName())
        	        except KeyError,err:
				pass
		# Redo create from treeview -> add node
		elif itm.GetSaveStatus() == CREATE: #Redo create
			try:
				my_id = self.GetSelectWindow().GetItemID(parent)
				self.GetSelectWindow().AddItem(itm,type)
			except KeyError,err:
				pass
		else: #Redo modify (its NOT possible to redo rename), so nothing to do
			pass


		# Redo create or modify in visual window; add new visual object
		if itm.__class__ in [Device,Link] and itm.GetSaveStatus() != DELETE: #redo create (or modify) node...then links included

			if itm.GetSaveStatus() == CREATE:
				tmp_obj = self.GetVisWindow().FindByName(itm.GetName())
	
				if itm.__class__ == Link:
					if tmp_obj:
						self.GetVisWindow().DoDeleteObj(itm.GetName(),False,refresh=False)
						self.__visualundolist.append(tmp_obj)
					else:
						found = False
						for objitm in save_tmp:
							if itm.GetName() == objitm.GetName() and itm.__class__ == objitm.__class__:
								self.__visualundolist.append(objitm)
								found = True
								break
						if not found:
								tmp_link = Link(itm.GetSystem(),itm.GetName(),False,False)
								self.__visualundolist.append(tmp_link)
				else: #Device
					if tmp_obj:
						self.GetVisWindow().Deselect(tmp_obj,refresh=False)
						newobj = Device(itm.GetSystem(),itm.GetName(),False)
						tmp_obj.SetObjectRef(newobj)
						self.GetVisWindow().Select(tmp_obj,refresh=False)
						self.__visualundolist.append(tmp_obj) #with wrong reference, but will be fixed
					else:
						tmp_dev = Device(itm.GetSystem(),itm.GetName(),False)
						self.__visualundolist.append(tmp_dev)
						
					continue #finished with modified device, next itm in undo

			else: # redo modify in visual window
				try:
					if itm.__class__ == DrawingObject:
						type = itm.GetObject().GetType()
					else:
						type = itm.GetType()
					id = self.GetSelectWindow().GetItemID(type)
				except KeyError,err:
					pass


			# redo create or modify
			vis_obj = None

			i = len(self.__visualredolist)-1
			while i >= 0:
				if self.__visualredolist[i].__class__ == DrawingObject:
					if self.__visualredolist[i].GetName() == itm.GetName() and self.__visualredolist[i].GetObject().__class__ == itm.__class__:
						vis_obj = self.__visualredolist.pop(i)
						break
				elif self.__visualredolist[i].__class__ in [Device,Link]:
					if self.__visualredolist[i].GetName() == itm.GetName() and self.__visualredolist[i].__class__ == itm.__class__:
						vis_obj = self.__visualredolist.pop(i)
						break

				else:
					print "Unkown class type"
				
				i -= 1

			if vis_obj and not (itm.__class__ == Device and itm.GetSaveStatus() == CREATE):
				if vis_obj.__class__ == DrawingObject:
					if vis_obj.GetObject().__class__ == Device: #found device object, can easily recover
						print "Add to contents"
						self.GetVisWindow().AddToContents(vis_obj,False)
					elif vis_obj.GetObject().__class__ == Link: #found link object
						vis_link = self.GetVisWindow().CreateConnectedLink(vis_obj.GetObject(),vis_obj.GetPosition(),vis_obj.GetSize())
				elif vis_obj.__class__ in [Device,Link]:
					if vis_obj.__class__ == Device: #found device object, can easily recover
						print "Create node"
						self.GetVisWindow().CreateNode(0,0,vis_obj,False)
					elif vis_obj.__class__ == Link: #found link object
						self.GetVisWindow().CreateConnectedLink(vis_obj)
						#print "Added non visual object"
				else:
					print "we dont know how to handle this class type"

			else:
				print "visual object not found in visual redo list because of some strange reason"
				
		# redo delete in visual window
		elif itm.__class__ in [Device,Link] and itm.GetSaveStatus() == DELETE: #redo delete of node or link
			tmp_obj = self.GetVisWindow().FindByName(itm.GetName())

			if tmp_obj:
				self.GetVisWindow().DeselectAll(refresh=False)
				self.GetVisWindow().Select(tmp_obj,refresh=False)
				self.GetVisWindow().DoDelete(None,False,refresh=False)

				self.__visualundolist.append(tmp_obj) #For UNDOing delete
			else: #not found in visual contents, create dataobject:
				found = False
				for objitm in save_tmp:
					if itm.GetName() == objitm.GetName() and itm.__class__ == objitm.__class__:
						self.__visualundolist.append(objitm)
						found = True
						break
				if not found:
					if itm.__class__ == Device:
						tmp_dev = Device(itm.GetSystem(),itm.GetName(),False)
						self.__visualundolist.append(tmp_dev)
					elif itm.__class__ == Link:
						tmp_link = Link(itm.GetSystem(),itm.GetName(),False,False)
						self.__visualundolist.append(tmp_link)

		self.ProgressData(0,False,1)

	#Fix Edit menu -> new text for redo and undo
	if self.__redolist == []:
		description = ""
	else:
		description = self.__redolist[len(self.__redolist)-1][1]

	#REDO descr:
	descrundo = self.__undolist[len(self.__undolist)-1][1]

	menu = wxMenu()
	menu.Append(ID_UNDO,"&Undo " + str(descrundo) + "\tCtrl-Z","The last action you did will be undone, and added to the Redo list")
	menu.Append(ID_REDO,"&Redo " + str(description) + "\tCtrl-R","The last action you UNDOed will be undone")
	self.GetMenuBar().Replace(1,menu,"&Edit")

	if self.__redolist == []:
		menu.Enable(ID_REDO,False)

	menu.Enable(ID_UNDO,True)

	self.ProgressData(0,True,0)

	self.GetVisWindow().Refresh()
	self.ShowError("The REDO action was successfully finished.",ERR_INFORMATION,False)

	
    def GetFirstNewUndoIndex(self):
	""" Get the index of the next undo will start from 
	"""
	
	return len(self.__dirtyobjects)

    def AddToUndo(self,fromindex,description):
	""" Add a new UNDO action/operation to the UNDO list so that the user can UNDO the operation later.
	Can be called from any place where an action in the program. Maybe we will have an other list specific 
	for the visual contents and what is done there to be possibly undone as well, then we will need an extra 
	parameter constant saying what list. 
	
	@fromindex - the index in the dirty objects list where the UNDO operation will remove objects from
	@description - a string description of the action; so that the user knows what he/she UNDOes.
	"""
	
	my_view = self.__currentview
	self.__undolist.append([fromindex,description,my_view]) #fromindex means _FROM_THIS_ONE_IS_INCLUDED_

	'''#Edit menu
	menu = wxMenu()
	menu.Append(ID_UNDO,"&Undo " + str(description) + "\tCtrl-Z","The last action you did will be undone, and added to the Redo list")
	if not self.GetMenuBar().IsEnabled(ID_REDO):
		menu.Append(ID_REDO,"&Redo\tCtrl-R","The last action you UNDOed will be undone")
		menu.Enable(ID_REDO,False)
	else:
		tmp_menuitm = self.GetMenuBar().GetMenu(1).FindItemById(ID_REDO)
		tmp_text = tmp_menuitm.GetLabel()
		menu.Append(ID_REDO,"&Redo\tCtrl-R","The last action you UNDOed will be undone")
		menu.Enable(ID_REDO,False)
		self.ResetRedoObjects() # New action added to UNDO; that means that we have to reset the REDO possibility

	self.GetMenuBar().Replace(1,menu,"&Edit")
	menu.Enable(ID_UNDO,True)'''

    def ResetRedoObjects(self):
	    	""" Empties all redo lists for objects that is no longer needed because REDO action is reset.
	    	"""
	    
		self.__redolist = []

		#delete visual object first because they contain references to data objects
		for visobj in self.__visualredolist: # delete visual redo objects
			del visobj
			visobj = None

		for obj in self.__redoobjectlist: # delete data redo objects
			del obj
			obj = None

		
		self.__redoobjectlist = []
		self.__visualredolist = []
	

    def DoUndo(self,event=None,showprogress=True):
	""" UNDOES the previously done action. Gets information about the undo from the undo lists.

	@showprogress - whether the progressbar should show the progress of the UNDO or not.
	"""
	
	self.ShowError("Please wait while UNDOing " + str(self.__undolist[len(self.__undolist)-1][1]),ERR_INFORMATION,False)

	# Get undo information and remove objects from dirty objects list for that undo operation.
	tmp_undo = self.__undolist.pop()
    	tmp_removed = self.RemoveDirtyObjects(tmp_undo[0])

	#add action to redo list, and the objects in the redo object list, so that it can later be redone
	self.__redolist.append([len(self.__redoobjectlist),tmp_undo[1],tmp_undo[2]])
	self.__redoobjectlist.extend(tmp_removed)

	if showprogress:
		self.ProgressData(len(tmp_removed),False,0) # initialize progress bar
	
	for itm in tmp_removed: # go through each UNDONE item and make necessary action in the tree view and visual window (add or remove objects)
	    	self.RecoverObjectToFile(itm,state=UNDO) # add undo activity information to recover file

		#TreeView Changes
		if itm.__class__ == Device:
			parent = itm.GetType()
			type = TN_DEVICE
		elif itm.__class__ == Link:
			parent = itm.GetType()
			type = TN_LINK
		elif itm.__class__ == DeviceType:
			parent = "Device types"
			type = TN_DEVICETYPE
		elif itm.__class__ == LinkType:
			parent = "Link types"
			type = TN_LINKTYPE
		elif itm.__class__ == Port:
			parent = itm.GetDevice()
			type = TN_PORT
                elif itm.__class__ == Spare_Device:
			parent = itm.GetHWType()
			type = TN_DEVICE
		elif itm.__class__ == Spare_Port:
			parent = itm.GetSpareDevice()
			type = TN_PORT
		if itm.GetSaveStatus() == CREATE: #undo create in tree view
			try:
        	        	my_id = self.GetSelectWindow().GetItemID(itm.GetName())
				self.GetSelectWindow().RemoveItem(my_id,parent)
				self.GetSelectWindow().SetItemToDeleted(itm.GetName())
        	        except KeyError,err:
				pass
		elif itm.GetSaveStatus() == DELETE: #undo delete in tree view
			try:
				my_id = self.GetSelectWindow().GetItemID(parent)
				self.GetSelectWindow().AddItem(itm,type)
			except KeyError,err:
				pass
		else: #undo modify, we do NOT undo rename, so no action necessary
			pass

		# undo stuff in visual window
		if itm.__class__ in [Device,Link] and itm.GetSaveStatus() == CREATE: #undo create device and link
			tmp_obj = self.GetVisWindow().FindByName(itm.GetName())

			if tmp_obj:
				self.GetVisWindow().DeselectAll(refresh=False)
				self.GetVisWindow().Select(tmp_obj,False,False,refresh=False)
				self.GetVisWindow().DoDelete(None,False,refresh=False)

				self.__visualredolist.append(tmp_obj) #For REDOing create
			else: #not found in visual contents, create dataobject
				self.__visualredolist.append(itm)

		elif itm.__class__ in [Device,Link] and itm.GetSaveStatus() != CREATE: #undo modify or delete

			if itm.GetSaveStatus() == MODIFY:
				tmp_obj = self.GetVisWindow().FindByName(itm.GetName())

				if itm.__class__ == Link:
			
					if tmp_obj:
						self.GetVisWindow().DoDeleteObj(itm.GetName(),False,refresh=False)
						self.__visualredolist.append(tmp_obj)
					else:
						self.__visualredolist.append(itm)
				else: #Device

					if tmp_obj:
						self.GetVisWindow().Deselect(tmp_obj,refresh=False)
						newobj = Device(itm.GetSystem(),itm.GetName(),False)
						tmp_obj.SetObjectRef(newobj)
						self.GetVisWindow().Select(tmp_obj,refresh=False)
						self.__visualredolist.append(tmp_obj) #with wrong reference, but will be fixed
					else:
						#tmp_dev = Device(itm.GetSystem(),itm.GetName(),False)
						self.__visualredolist.append(itm)

			#for both delete and modify
			vis_obj = None

			i = len(self.__visualundolist)-1
			while i >= 0:
				if self.__visualundolist[i].__class__ == DrawingObject:
					if self.__visualundolist[i].GetName() == itm.GetName() and self.__visualundolist[i].GetObject().__class__ == itm.__class__:
						vis_obj = self.__visualundolist.pop(i)
						break
				elif self.__visualundolist[i].__class__ in [Device,Link]:
					if self.__visualundolist[i].GetName() == itm.GetName() and self.__visualundolist[i].__class__ == itm.__class__:
						vis_obj = self.__visualundolist.pop(i)
						break
					
				i -= 1

			if vis_obj and not (itm.__class__ == Device and itm.GetSaveStatus() == MODIFY):
				if vis_obj.__class__ == DrawingObject:
					#dynamic view, subsystem view or view not changed since UNDO
					if vis_obj.GetObject().__class__ == Device: #found device object, can easily recover
						self.GetVisWindow().AddToContents(vis_obj,False)
						self.GetVisWindow().Deselect(vis_obj,refresh=False)
						newobj = Device(itm.GetSystem(),itm.GetName(),False)
						vis_obj.SetObjectRef(newobj)
						self.GetVisWindow().Select(vis_obj,refresh=False)

						#may need to connect to links as well
					elif vis_obj.GetObject().__class__ == Link: #found link object
						#self.GetVisWindow().AddToContents(vis_obj,False) #need to connect to devices as well
						vis_link = self.GetVisWindow().CreateConnectedLink(vis_obj.GetObject(),vis_obj.GetPosition(),vis_obj.GetSize())
				elif vis_obj.__class__ in [Device,Link]:
					if vis_obj.__class__ == Device:
						self.GetVisWindow().CreateNode(0,0,vis_obj,False)
					elif vis_obj.__class__ == Link:
						self.GetVisWindow().CreateConnectedLink(vis_obj)

				else: #another unknown classtype
					print "Found a classtype we do not know how to handle!"


		if showprogress:
			self.ProgressData(0,False,1)

	#Edit menu
	if self.__undolist == []:
		description = ""
	else:
		description = self.__undolist[len(self.__undolist)-1][1]

	#REDO descr:
	descrredo = self.__redolist[len(self.__redolist)-1][1]

	menu = wxMenu()
	menu.Append(ID_UNDO,"&Undo " + str(description) + "\tCtrl-Z","The last action you did will be undone, and added to the Redo list")
	menu.Append(ID_REDO,"&Redo " + str(descrredo) + "\tCtrl-R","The last action you UNDOed will be undone")
	self.GetMenuBar().Replace(1,menu,"&Edit")

	if self.__undolist == []:
		menu.Enable(ID_UNDO,False)

	menu.Enable(ID_REDO,True)

	if showprogress:
		self.ProgressData(0,True,0)

	self.GetVisWindow().Refresh()
	self.ShowError("The UNDO operation was successfully finished.",ERR_INFORMATION,False)

    def IsSaveListEmpty(self):
	""" We check if there is anything to save in the dirty objects list. We actually check
	that if there are any objects in the dirty objects list that cancel each other 
	(first create, then delete = no change in ConfDB).

	!return - True if there is nothing to save, False otherwise.
	"""
	
	return (self.CleanUpDirtyObjects(False) == [])

    def GetDirtyObjects(self):
	""" Get the dirty objects list.
	"""
	return self.__dirtyobjects

    def GetRenamedObjects(self):
	""" Get the rename objects list.
	"""
	return self.__renameobjects

    def GetOldNames(self):
	""" Get the list of names of objects that have been renamed. This returns the names of the 
	objects before they were renamed.
	"""
	return self.__oldnames

    def GetNewNames(self):
	""" Get the list of names of objects that have been renamed. This returns the names of the
	objects after they were renamed.
	"""
	return self.__newnames

    def GetOldNamesOfRenamed(self):
        """ Get a dictionary of mapping between old and new names of renamed objects:
	old names : new names
	"""
	return self.__oldnamesrenamed

    def GetNewNamesOfRenamed(self):
	""" Get a dictionary of mappings between new and old names of renamed objects:
	new names : old names    
	"""
	return self.__newnamesrenamed

    def ResetDirtyObjects(self):
	""" Reset all lists that has something to do with dirty objects: UNDO,REDO,rename etc.
	and the dirty objects list itself of course. Done on program termination and
	if the user switches from creation mode to navigation mode f.ex.

	TODO: should there also be an iteration through the objects and explicitly
	deleting each one of them, or is this not necessary? Check if the memory usage
	of the program changes.
	"""

	self.__dirtyobjects = []
	self.__undolist = [] #information needed for undo
	self.__redolist = [] #Once you undo, the undo object is appended to the redo list. information needed for redo
	self.__redoobjectlist = [] #undone objects, opposite to dirtyobjects list. Can be added back to dirtylist and savelist if REDOed

	self.__visualundolist = [] #On delete, store object here if want to UNDO it later
	self.__visualredolist = [] #When UNDOing a create 
	self.__renameobjects = []
	self.__oldnamesrenamed = {}
	self.__newnamesofrenamed = {}
	self.__newnames = []
	self.__oldnames = []

    def AddToDirtyObjects(self,object,fromredoundo=False):
	""" Adds a data object to the dirty objects list. This can be done when
	the user changes something (create,modify,delete) or objects are UNDONE
	and REDONE.

	@object - the object that is being added to the dirty objects list (because
	it will be applied to the ConfDB later
	@fromredoundo - If the object is added as a result/action of a REDO or UNDO (True),
	otherwise set to False

	!return - False on Error, true if success
	"""
	if self.GetUserMode() != ID_CREATION_MODE:
		self.SetCreationMode(True)
	
	if not fromredoundo:
	    	self.RecoverObjectToFile(object,state=DESIGN) # Add to recover file as normal if this function is not called from redo or undo

	
	i = len(self.__dirtyobjects) - 1
	while i >= 0:
		# if we find the "same" object in the dirty list and the save statuses for the objects are inconsistent...return False
		if self.__dirtyobjects[i].GetName() == object.GetName() and self.__dirtyobjects[i].__class__ == object.__class__:
			if (isinstance(self.__dirtyobjects[i],Link) and self.__dirtyobjects[i].GetNodeTo()==object.GetNodeTo() and self.__dirtyobjects[i].GetNodeFrom()== object.GetNodeFrom() and self.__dirtyobjects[i].GetPortTo()== object.GetPortTo() and self.__dirtyobjects[i].GetPortFrom()== object.GetPortFrom()):
				linkExist = True
			else:
				linkExist = False
				
			if isinstance(self.__dirtyobjects[i],Device) or isinstance(self.__dirtyobjects[i],Port) or linkExist:
				if self.__dirtyobjects[i].GetSaveStatus() == DELETE and object.GetSaveStatus() != CREATE:
					self.ShowError("You are trying to modify or delete an object that has already been deleted: " + str(object.GetName()),ERR_ERROR)
					return False #if deleted, we cannot delete again or modify : set error message
				elif self.__dirtyobjects[i].GetSaveStatus() == CREATE and object.GetSaveStatus() == CREATE:
					self.ShowError("You are trying to create an object with the same name as an already existing object: " + str(object.GetName()),ERR_ERROR)

					return False #we cannot recreate same object : set error message
				elif self.__dirtyobjects[i].GetSaveStatus() == MODIFY and object.GetSaveStatus() == CREATE:
					self.ShowError("You are trying to create an object with the same name as an already existing object: " + str(object.GetName()),ERR_ERROR)
					return False #cannot recreate already created object
			
			break #found a valid object in the list, and the given object for insertion is valid, break while loop and append
		i -= 1



	if object.GetSaveStatus() == DELETE: #deleting object, save visual object in a list for later UNDO possibility
		visobj = self.GetVisWindow().FindByName(object.GetName())
		if visobj != None:
			self.__visualundolist.append(visobj)
		else:
			pass

	self.__dirtyobjects.append(object)
	return True

    def RemoveFromDirtyObjects(self,objectname,objecttype):
	""" Remove the last instance of an object from the dirtylist given the name (ID) of the object 
	and the objectype, and return it. Return False if not found
	The objecttype can be of the following types: Device, DeviceType, SubSystem, Link, LinkType, Port.
		
	@objectname - name of the object to be removed
	@objecttype - class type of the object to be removed (Device, DeviceType, etc)

	!return - the removed object instance if found, False if not found
	"""
	
	i = len(self.__dirtyobjects) - 1
	while i >= 0:
		if self.__dirtyobjects[i].GetName() == objectname and self.__dirtyobjects[i].__class__ == objecttype:
			my_item = str(self.__dirtyobjects[i].__class__) + ": " + str(self.__dirtyobjects[i].GetSaveStatus())
			return self.__dirtyobjects.pop(i)
		i -= 1

	return False #not found

    def RemoveDirtyObjects(self,indexfrom):
	""" Used for UNDOing operations. One operation like duplicate items can include n number of DBcalls, starting from the index given. 
	
	@indexfrom - start index to remove objects from the dirty objects list from (all objects from and after this index will be removed)

	!return - a list of the objects that were removed
	"""

	tmp_removed = []
	i = indexfrom #check that indexfrom is not >= len(list)
	while i < len(self.__dirtyobjects):
		tmp_removed.append(self.__dirtyobjects.pop(i))

	return tmp_removed

    def GetDirtyObject(self,objectname,objecttype):
	""" Get an object from the dirty list by name and type. We only look for objects that exist, and are not set for deletion.
	If object is found, and the last instance is not set for deletion, return the last instance, otherwise False.
	
	@objectname - name of the object to remove
	@objecttype - type of the object to remove (Device,DeviceType etc)

	!return - The instance of the last object with objectname and objectype given as parameters if it is not set for deletion,
	otherwise return False
	"""
	
	i = len(self.__dirtyobjects) - 1
	while i >= 0:
		if self.__dirtyobjects[i].GetName() == objectname and self.__dirtyobjects[i].__class__ == objecttype:
			if self.__dirtyobjects[i].GetSaveStatus() == DELETE:
				return False
			else:
				return self.__dirtyobjects[i]
		i -= 1

	return False

    def FindDirtyObject(self,objectname,objecttype):
	""" Get an object from the dirty list...returns the object if found, or False if not found 

	@objectname - name of the object to find
	@objecttype - type of the object to find

	!return - the last instance of the object if found, otherwise False
	"""
	
	i = len(self.__dirtyobjects) - 1
	while i >= 0:
		if self.__dirtyobjects[i].GetName() == objectname and self.__dirtyobjects[i].__class__ == objecttype:
				return self.__dirtyobjects[i]
		i -= 1

	return False

    def RecoverObjectFromFile(self,filename="recover.dat"):
	""" Recover previously stored objects in the recover file to the dirty objects list; because
	the objects stored in recover file is stored there because they are not applied to the
	ConfDB.
	
	First we check to see if the file is not empty. If it is not, it is most likely that the application crashed 
	or the user closed the application before he/she had saved the data, ask therefore to recover non-saved objects 
	since last time the app was run. Ask first to recover, the if you should reset the file. If user wants to recover,
	all objects are recovered to dirtylist, of he wants to reset, file is reset. 
	
	@filename - name of the recover file where we should look for recovered/not stored objects.

	!return - True if successful, False otherwise.
	"""
	devicetype_found = False
	linktype_found = False
	
	try:
		try:
			file = open(filename,"r")
		except IOError,err:   
			file = open(filename,"w") #file does not exist, ignore recover, but create file
			file.close()
			self.ShowError("Error opening file recover.dat for reading " + str(err),ERR_ERROR)
			return False 	

		if file.read(1) != "":
			file.close()
			ans = wxMessageBox("It seems that you did not save your changes in the database last time you used the program. The changes you did can be recovered, do you want to recover?","Recover?", wxYES_NO | wxICON_QUESTION)
			if ans == wxNO:
				ans = wxMessageBox("Ok. Are you sure you want to reset/remove the recovered objects?","Reset recovered objects?", wxYES_NO | wxICON_QUESTION)
				if ans == wxYES:
					self.ResetRecoverFile()
				else: #wxNO
					self.RecoverObjectFromFile() #restart questioning
			else: #wxYES
				#recover objects
				file = open(filename,"r")
				for line in file:
					obj_arr = line.split("|")
					tmp_dict = {}
					for itm in obj_arr:
						tmp_val = itm.split("^")
						if tmp_val[1] != "\n":
							if tmp_val[2] == "s": #s(tring)
								tmp_dict[tmp_val[0]] = str(tmp_val[1])
							elif tmp_val[2] == "i": #i(nteger)
								tmp_dict[tmp_val[0]] = int(tmp_val[1])
							elif tmp_val[2] == "w": #wxColour
								cols = tmp_val[1].split("(")
								cols = cols[1].split(")")
								cols = cols[0].split(",")
								tmp_dict[tmp_val[0]] = wxColour(int(cols[0]),int(cols[1]),int(cols[2]))
							else:
								print "A value could not be interpreted correctly, and was not extracted: " + str(tmp_val[0])

					classtype = tmp_dict["Class"]
					statetype = tmp_dict["State"]
					del tmp_dict["State"] #This is only helpers for writing/reading to/from file, not needed any more
					del tmp_dict["Class"]

					if classtype == "objectclasses.Device":
						tmp_obj = Device(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_NAME],True)
					elif classtype == "objectclasses.Link":
						tmp_obj = Link(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_LINK],False,True)

						#Update last temporary link nr/ID, so we know where to go from on next
						if tmp_dict[DEV_LINK].find("TMP_LINK_",0,len(tmp_dict[DEV_LINK])) != -1:
							tmp_string = tmp_dict[DEV_LINK].replace("TMP_LINK_","")
							try:
								tmp_string = int(tmp_string)
								if self.__lastlinknr < tmp_string:
									self.__lastlinknr = tmp_string
							except ValueError,err:
								pass #an invalid number was given or invalid string for the
								# tmp link name

					elif classtype == "objectclasses.DeviceType":
						tmp_obj = DeviceType(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_TYPE],True)
						devicetype_found = True
					elif classtype == "objectclasses.Port":
						tmp_obj = Port(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_PORTNBR],tmp_dict[DEV_PORTTYPE],tmp_dict[DEV_PORTWAY],tmp_dict[DEV_NAME],True)
					elif classtype == "objectclasses.LinkType":
						tmp_obj = LinkType(tmp_dict[DEV_SYSTEM],tmp_dict[LINK_NAME],True)
						linktype_found = True
					else:
						print "Found a class type saved in Recover file that is not supported: " + str(classtype)
						continue #next

					tmp_obj.SetDict(tmp_dict)

					if tmp_obj.GetSaveStatus() == RENAME: #rename object, update prev objects to the current name
						self.UpdateRenamedObject(tmp_obj,recovertofile=False) #ignore db errors
					else:
						if statetype == DESIGN:
							self.AddToDirtyObjects(tmp_obj,True)
						elif statetype == UNDO:
							self.RemoveFromDirtyObjects(tmp_obj.GetName(),tmp_obj.__class__)
						elif statetype == REDO:
							self.AddToDirtyObjects(tmp_obj,True)

				file.close()

				if devicetype_found or linktype_found:
					self.RefreshObjectBoxData()
		return True

	except IOError,err:
		self.ShowError("An unexpected IOError occured while reading from recover.dat: " + str(err),ERR_ERROR)
		return False
				
    def ResetRecoverFile(self,filename="recover.dat"):
	""" Reset (clear) the recover file.

	@filename -  the name of the recover file to reset.
	
	!return - True if successful, otherwise False
	"""
	
	try:
		file = open(filename,"w")
		file.write("") #reset
		file.close()
		return True
		
	except IOError,err:
		self.ShowError("An unexpected IOError occured while trying to reset the file " + str(filename),ERR_ERROR)
		return False

    def RecoverObjectToFile(self,object,state=DESIGN,filename="recover.dat",write="a"):
	""" Write a data object to a recover file for later be able to recover it to the
	dirty objects list if it is not stored in the ConfDB before program termination.
	
	When objects are added to the dirty list or UNDOne/REDOne, they will be stored in a data file, with all the info 
	on the objects. This file will be erased as soon as the user have done a successful save to the database. If
	unsuccessful it is not cleared, and if the program crashes etc, this file will be automatically loaded on startup
	if user wants to, then all information will be added to the dirty objects list, and the user can continue where 
	he/she stopped the time before. When all the data is reloaded on startup, it is not cleared, it is only cleared 
	when a successful save is done.

	All kinds of actions are written; what the user do of actions, and also redo and undo actions by the user. This
	way the recover file looks like a history log of what the user have done of manipulative actions in CdbVis in the
	current (previous) session, and everything can easily be recovered to the previous state of the program.

	@object - the data object that should be added to the recover file
	@state - the type of action done on the object: DESIGN, UNDO or REDO
	@filename - name of the recover file to save it to
	@write - w or a, w will start to write to the file at the beginning overwriting the previous contents,
	a will write to the file at the end; append to the file.

	!return - True if successful, False otherwise
	"""
	
	try:	
		file = open(filename,write) #open() is an alias to file(), a - append, w - write (overwrite)
		properties = object.GetObjectInfo(True)
		file.write("Class" + "^" + str(object.__class__) + "^s")
		file.write("|" + "State" + "^" + str(state) + "^i|")

		#AND we must save datatype; i for integers and booleans, s for strings (the rest), w for wxColour obj
		for property in properties:
			if (properties[property]).__class__ == int(3).__class__: #integers and boolean
				file.write(str(property) + "^" + str(properties[property]) + "^i|")
			elif (properties[property]).__class__ == str("string").__class__: #string or similar
				file.write(str(property) + "^" + str(properties[property]) + "^s|")
			elif (properties[property]).__class__ == tuple: #Colour
				file.write(str(property) + "^" + str(properties[property]) + "^w|")
			else:
				print "Class type not supported: " + str(properties[property].__class__) + " for storing in the recover file"
				
		file.write("^\n")
		file.close()

		return True

	except IOError,err:
		self.ShowError("An unexpected error occured while writing to recovery file",ERR_ERROR)
		return False

    def CleanUpDirtyObjects(self,actOnDirtyList=True):
		""" Remove objects in the dirty objects list that cancel each other out; f.ex. first a create and then a
		delete on the same object will cancel both of them out, and hence not necessary to write in the ConfDB.
		May act directly on the dirty objects list before dirty objects are stored in ConfDB, or that the user
		checks whether there is something to save or not in the dirty objects list.
	    
		This auxiliary function is deleting create-modify-delete objects from the dirty list, since we do not 
		want to store those in the database. ie. objects that are both created (and modified) before they are
		deleted in this session. This may cause some naming problems, but hopefully not, if we can have a list
		of names on deleted objects, that are free to use for new objects. 

		We also but several CREATE and MODIFY instances of the same object together to 1 object for storing in
		ConfDB.

		Rename objects are not affected at all.

		@actOnDirtyList - Whether objects that cancel each other out are directly removed from the dirty objects
		list or only done in a temporary list copy of the dirty objects list.

		!return - If we act on the real dirty objects list: True if dirty objects list became empty after 
			cancelling out objects, False if not.
			If we act on a copy of the real dirty objects list: The "repaired" dirty objects list copy where
			objects that appeal each other are compressed if the list is not empty, else the empty list is 
			returned.
		"""
		
		dict_setformodify = {} #name : index, for objects set for modify
		dict_delobjs = {} #delete objects found, see if we find modify and create of these object, if so, remove them

		lst_delobjs = []
		if not actOnDirtyList:
			dirtyobjs = self.__dirtyobjects[:]
		else:
			dirtyobjs = self.__dirtyobjects


		i = len(dirtyobjs)-1
		while i >= 0:
			if dirtyobjs[i].GetSaveStatus() == DELETE: #id 2
				dict_delobjs[dirtyobjs[i].GetName()] = dirtyobjs[i]
				lst_delobjs.append(dirtyobjs[i].GetName())
			elif dirtyobjs[i].GetSaveStatus() == MODIFY and dirtyobjs[i].GetName() in lst_delobjs:
				if dirtyobjs[i].__class__ != Port:
					try:
						dict_setformodify[dirtyobjs[i].GetName()].append(dirtyobjs[i])
					except KeyError,err:
						dict_setformodify[dirtyobjs[i].GetName()] = []
						dict_setformodify[dirtyobjs[i].GetName()].append(dirtyobjs[i])
				else:
					try:
						dict_setformodify[dirtyobjs[i].GetName()].append(dirtyobjs[i])
					except KeyError,err:
						dict_setformodify[dirtyobjs[i].GetName()] = []
						dict_setformodify[dirtyobjs[i].GetName()].append(dirtyobjs[i])
					
					
			elif dirtyobjs[i].GetSaveStatus() == CREATE and dirtyobjs[i].GetName() in lst_delobjs:	
				lst_delobjs.remove(dirtyobjs[i].GetName())
				dirtyobjs.remove(dict_delobjs[dirtyobjs[i].GetName()])

				try:
					mod_objs = dict_setformodify[dirtyobjs[i].GetName()]
					for mod_obj in mod_objs:
						dirtyobjs.remove(mod_obj)

				except KeyError,err:
					print "Modify object not found for " + str(dirtyobjs[i].GetName()) + ", delete create object instance then."

				dirtyobjs.remove(dirtyobjs[i])

			i -= 1

		if dirtyobjs == []:
			if not actOnDirtyList: #return the empty list
				return []
			return True #if savelist was "empty", nothing to do against database.
		else:
			if not actOnDirtyList: #return the copy
				return dirtyobjs
			return False

    def MassInsert(self):
	""" Load a file (XML or TEXT) with data to be added to the ConfDB as objects of DeviceType, LinkType, 
	Device, Link and Port, and add them to the dirty objects list, pending for storing in the ConfDB. The
	user will him/herself store the objects with the OnSave function, or use the ExportConfDBFunctions function
	to export the objects to a file.

	!return - False on Error
	"""
	# Constants that can be used in the data file, conversion to integer
	conversions = {'NO_STATUS':-1,'CREATE':0,'MODIFY':1,'DELETE':2,'RENAME':3,'False':0,'True':1}
	
	self.__directoryname = "" # Application (or current) directory
	opendlg = QFileDialog.getOpenFileName(self.__directoryname,\
                    "Data file (*.dat);;Text files (*.txt);;XML files (*.xml)",\
                    self,\
                    "Find a data file to load",\
                    "Choose a file" );
	opendlg = str(opendlg)
	if opendlg=='':
		self.ShowError("Mass Insert : No file selected", ERR_ERROR,False)
		return False
	print "file selected : "+opendlg
	filename =  opendlg[len(opendlg)-opendlg[::-1].index("/"):]
	directory = opendlg.strip(filename)
	fileext = filename.split(".")[1] # Cannot get error because only files with extensions are filtered to be shown

        filepath = os.path.join(directory,filename) # the filepath and file name	
	
	if fileext == "xml": #predefined xml format, see user documentation (also for txt and dat)
		from xml.dom import Node
		import xml.dom.minidom # import parse
		try:

			xmldocument = xml.dom.minidom.parse(filepath)
			# The first child to this document is the root/top node, which covers the whole file
			# This has to be here as defined, and will cause parser error if removed
			root = xmldocument.childNodes[0] #first element; outer element: only 1

			for node in root.childNodes: # We should have the data types (Device, Link, Port...) as children here
				# The parameter-value pairs are attributes of the element, or a text element
				if node.nodeType == Node.ELEMENT_NODE:
					tmp_dict = {}
					tmp_dict["Datatype"] = node.nodeName
					
					print 'Element name: %s' % node.nodeName
					for child in node.childNodes: # property nodes
						if child.nodeType == Node.ELEMENT_NODE:
							print 'Element name: %s' % child.nodeName
							attributeName = child.nodeName
							for textnode in child.childNodes: # text nodes
								if textnode.nodeType == Node.TEXT_NODE:
									if str(textnode.data).strip() == "":
										# line break
										pass
									elif textnode.nodeName == "#text":
										print '\tTextValue: %s' % (textnode.data)
										tmp_val = str(textnode.data).split(";")
										if tmp_val[1] == "s": #s(tring)
											tmp_dict[attributeName] = str(tmp_val[0])
										elif tmp_val[1] == "i": #i(nteger)
											# if we used constant instead of int
											# find corresponding int in dict.
											if tmp_val[0] in conversions:
												tmp_dict[attributeName] = conversions[tmp_val[0]]
											else:
												tmp_dict[attributeName] = int(tmp_val[0])
										elif tmp_val[1] == "w": #wxColour
											cols = tmp_val[0].split("(")
											cols = cols[1].split(")")
											cols = cols[0].split(",")
											tmp_dict[attributeName] = wxColour(int(cols[0]),int(cols[1]),int(cols[2]))
										else:
											print "A value could not be interpreted correctly, and was not extracted: " + str(tmp_val[0])

					classtype = tmp_dict["Datatype"]
					del tmp_dict["Datatype"]

					if classtype == "Device":
						tmp_obj = Device(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_NAME],True)
					elif classtype == "Link":
						tmp_obj = Link(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_LINK],False,True)
						tmp_dict[DEV_LINK] = "TMP_LINK_" + str(self.GetNewLinkNr())
					elif classtype == "Spare_Device":
						tmp_obj = Spare_Device(tmp_dict[DEV_HWTYPE],tmp_dict[DEV_SERIAL],tmp_dict[SPARE_DEV_NAME],True)
					elif classtype == "DeviceType":
						tmp_obj = DeviceType(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_TYPE],True)
					elif classtype == "Port":
						tmp_obj = Port(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_PORTNBR],tmp_dict[DEV_PORTTYPE],tmp_dict[DEV_PORTWAY],tmp_dict[DEV_NAME],True)
					elif classtype == "Spare_Port":
						tmp_obj = Spare_Port(tmp_dict[DEV_PORTNBR],tmp_dict[DEV_PORTTYPE],tmp_dict[DEV_PORTWAY],tmp_dict[DEV_SERIAL],True)
					elif classtype == "LinkType":
						tmp_obj = LinkType(tmp_dict[DEV_SYSTEM],tmp_dict[LINK_NAME],True)
					else:
						print "Found a Datatype that is not supported: " + str(classtype)
						continue #next

					tmp_obj.SetDict(tmp_dict)

					if tmp_obj.GetSaveStatus() == RENAME: #rename object, update those in dirty list
						self.UpdateRenamedObject(tmp_obj,recovertofile=True)
					else: # normal add to dirty objects list
						self.AddToDirtyObjects(tmp_obj,False)

		except xml.parsers.expat.ExpatError,err:
			self.ShowError("Error while parsing XML file: " + str(err),ERR_ERROR)
			return False
		except IndexError,err:
			self.ShowError("An unexpected IndexError occured while reading, probably due to a syntax error; remember that every parameter and value must be set like this: <parametername>parametervalue:datatype</parametername>",ERR_ERROR)
			return False
		except ValueError,err:
			self.ShowError("There was a data conversion error while reading from file, probably due to wrong data type given for a value.",ERR_ERROR)
			return False


	elif fileext in ["txt","dat"]: # the same format, just different file extension. Similar to recovery file format.
		try:
			try:
				file = open(filepath,"r")
			except IOError,err:
				self.ShowError("Error opening file " + str(filepath) + " for reading: " + str(err),ERR_ERROR)
				return False 	
			
			if file.read(1) != "":
				file.seek(0) # go back to start of file
				for line in file: # one object per line
					obj_arr = line.split("|") # attributename - value pairs split by |
					tmp_dict = {}
					for itm in obj_arr:
						if itm != "" and itm[0] not in ["","\r","\n"]: #empty row
							tmp_val = itm.split(";")
							if tmp_val[1] not in ["\n","\r"]:
								if tmp_val[2] == "s": #s(tring)
									tmp_dict[tmp_val[0]] = str(tmp_val[1])
								elif tmp_val[2] == "i": #i(nteger)
									if tmp_val[1] in conversions:
										tmp_dict[tmp_val[0]] = conversions[tmp_val[1]]
									else:
										tmp_dict[tmp_val[0]] = int(tmp_val[1])
								elif tmp_val[2] == "w": #wxColour
									cols = tmp_val[1].split("(")
									cols = cols[1].split(")")
									cols = cols[0].split(",")
									tmp_dict[tmp_val[0]] = (int(cols[0]),int(cols[1]),int(cols[2]))
								else:
									print "A value could not be interpreted correctly, and was not extracted: " + str(tmp_val[0])

					classtype = tmp_dict["Datatype"]
					del tmp_dict["Datatype"]
					if classtype == "Device":
						tmp_obj = Device(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_NAME],True)
					elif classtype == "Link":
						tmp_obj = Link(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_LINK],False,True)
						tmp_dict[DEV_LINK] = "TMP_LINK_" + str(self.GetNewLinkNr())
					elif classtype == "Spare_Device":
						tmp_obj = Spare_Device(tmp_dict[DEV_HWTYPE],tmp_dict[DEV_SERIAL],tmp_dict[SPARE_DEV_NAME],True)
					elif classtype == "DeviceType":
						tmp_obj = DeviceType(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_TYPE],True)
					elif classtype == "Port":
						tmp_obj = Port(tmp_dict[DEV_SYSTEM],tmp_dict[DEV_PORTNBR],tmp_dict[DEV_PORTTYPE],tmp_dict[DEV_PORTWAY],tmp_dict[DEV_NAME],True)
					elif classtype == "Spare_Port":
						tmp_obj = Spare_Port(tmp_dict[DEV_PORTNBR],tmp_dict[DEV_PORTTYPE],tmp_dict[DEV_PORTWAY],tmp_dict[DEV_SERIAL],True)
					elif classtype == "LinkType":
						tmp_obj = LinkType(tmp_dict[DEV_SYSTEM],tmp_dict[LINK_NAME],True)
					else:
						print "Found a Datatype that is not supported: " + str(classtype)
						continue #next

					tmp_obj.SetDict(tmp_dict)

					if tmp_obj.GetSaveStatus() == RENAME: #rename object, update those in dirty list
						self.UpdateRenamedObject(tmp_obj,recovertofile=True)
					else: # normal add to dirty objects list
						self.AddToDirtyObjects(tmp_obj,False)

				file.close()
				self.ShowError(" Data in "+opendlg+" was successfuly loaded !",ERR_SUCCESS)
		except IOError,err:
			self.ShowError("An unexpected IOError occured while reading from " + str(filepath) + ": " + str(err),ERR_ERROR)
			return False
		except IndexError,err:
			self.ShowError("An unexpected IndexError occured while reading, probably due to a syntax error; remember that every parameter and value must be set like this: |parametername:parametervalue:parametertype|",ERR_ERROR)
			return False
		except ValueError,err:
			self.ShowError("There was a data conversion error while reading from file, probably due to wrong data type given for a value.",ERR_ERROR)
			return False

	else:
		self.ShowError("The file extension of the file you loaded is not supported",ERR_ERROR)
		return False


    def ImportCSV(self,filename):
	def __addTo(elt,object_list):
		if object_list==[] or (object_list and elt.GetName() not in [item.GetName() for item in object_list]):
			object_list.append(elt)
			return elt
		else:
			for item in object_list:
				if item.GetName() == elt.GetName():
					return item
	
	def __CreateLink(FromPort,ToPort):
		if not FromPort or not ToPort:
			pdb.set_trace()
			raise 'No Port Given to the __CreateLink function'
			return False
		tmp_link = Link('')
		tmp_link.SetNodeTo(ToPort.GetDevice())
		tmp_link.SetNodeFrom(FromPort.GetDevice())
		tmp_link.SetPortTo(str(ToPort.GetPortNbr()))
		tmp_link.SetPortFrom(str(FromPort.GetPortNbr()))
		tmp_link.SetType(LINK_TYPE)
		tmp_link.SetName(tmp_link.GetNodeFrom()+":"+str(tmp_link.GetPortFrom())+":"+tmp_link.GetNodeTo()+":"+str(tmp_link.GetPortTo()))
		return tmp_link
	def LookIn(list,current,steps):
		length = len(list)
		index = current
		count = 0
		if steps>0:
			while index < length-1 and count < steps:
				if list[index]!=None:
					count += 1
				index += 1
			if list[index] == None:
				return None
			else:
				return list[index]
		if steps<0:
			while index >0 and count < abs(steps):
				if list[index]!=None:
					count += 1
				index -= 1
			if list[index] == None:
				return None
			else:
				return list[index]
				
		
        filename = "Velo_Connectivity_clean.csv"
	reader = csv.reader(open(filename, "rb"))
	DEVICE, DEVPORT, PORT, LINK = 0, 1, 2, 3
	LINK_TYPE = 'hv'
	device_list = []
	port_list = []
	link_list = []
	types = [None, None, None, None, DEVICE, PORT, LINK, DEVPORT, DEVPORT, None, LINK, DEVPORT, DEVPORT, LINK, DEVICE]
	lastWay = 2
	lastPort = None

	try:
	    # pdb.set_trace()
	    for row in reader:
	        #print row
		index = 0
		length = len(row)
		while index < length:
			cell = row[index]
			tmp_port = None
			if types[index]==DEVICE:
				# create device if does not exist and add it the the  object_list
				tmp_device = __addTo(Device("",cell),device_list)
				# Create a Link if  next cell is not a Port and previous cell is a LINK or a Port or a DEVPORT. 
				if index+1==length or (index+1<length and LookIn(types,index,1)!=PORT):  # if there is no port after our device we should create a port (with number = 1 and way= not lastWay) and a link.
					# creating the port
					lastWay = 3-lastWay
					tmp_port = Port('','1','',str(lastWay),cell)  # Port Nr = 1;   port way = Opposite of the last Port Way. (use of the function x -> 3-x) 
					tmp_port = __addTo(tmp_port, port_list)
					# ceating the link
					if index-1>0 and lastPort:
					    if LookIn(types,index,-1) == DEVICE: # if there is a different device before we  create a link
						tmp_link = __CreateLink(lastPort,tmp_port)
						tmp_link = __addTo(tmp_link,link_list)
					    elif LookIn(types,index,-1)== PORT:
						pass # TO DO 
					    elif LookIn(types,index,-1)== DEVPORT:
						pass # TO DO 	
				
			elif types[index]==DEVPORT: # DEVPORT =   device_name[in/out][ _ ][integer]
				# 1 - extract data from the current column
				portWay = -1
				try:
					portNr = int(cell[-1:]) # if the last caractere is a number so we have to continue to find how many caracter is the port number made of.
					i = 2
					while(i<len(cell)):
						try:
							portNr = int(cell[-i:])
							i += 1
						except:
							i -= 1
							break;
				except:
					portNr = 1  # default : if no port number is indiquited
					if cell[-2:]=="in":
						portWay = 1
						cell = cell[:-2]
					elif cell[-3:] == "out":
						portWay = 2
						cell = cell[:-3]
					else:
						print "error : this is not a DEVICE PORT"
						return
				
				if portWay == -1: # in the case have found a valid Port Number
					cell = cell[:(len(cell)-i)]
					if cell[-1:] in ['-','_']:
						cell = cell[:-1] # we delete the underscore.
					if cell[-2:]=="in":
						portWay = 1
						cell = cell[:-2]
					elif cell[-3:] == "out":
						portWay = 2
						cell = cell[:-3]
					else:
						print "Warinig: No port Way found for a DEVPORT"
						portWay = 3 - lastWay
				# 2 - Now we have the devicename (cell), the port number (portNr) and the port Way (potWay).
				lastWay = portWay
				tmp_device = __addTo(Device('',cell),device_list) # adding the port to the list if not already done
				# update the ports number for our device
				if portWay == 1:	tmp_device.SetPortsIn(max(tmp_device.GetPortsIn(),portNr))
				else:	tmp_device.SetPortsOut(max(tmp_device.GetPortsOut(),portNr))
				
				# Create Port
				tmp_port = __addTo(Port('',portNr,"",portWay,cell), port_list)
				
				#Create The link:
				if index-1 >= 0 and lastPort:
				   if LookIn(types,index,-1)== DEVICE: #types[index-1]==DEVICE:
					tmp_link = __CreateLink(lastPort,tmp_port)
					tmp_link = __addTo(tmp_link,link_list)
				   if LookIn(types,index,-1)==DEVPORT: #types[index-1]==DEVPORT:
					tmp_link = __CreateLink(lastPort,tmp_port)
					tmp_link = __addTo(tmp_link,link_list)
				   if LookIn(types,index,-1)== PORT:
					tmp_link = __CreateLink(lastPort,tmp_port)
					tmp_link = __addTo(tmp_link,link_list)
				   if LookIn(types,index,-1)== LINK:
					tmp_link = __CreateLink(lastPort,tmp_port)
					tmp_link.SetLinkInfo(row[index-1])
					tmp_link = __addTo(tmp_link,link_list)
				
			elif types[index]==PORT: #here we should create the Port then the Link if possible
				if LookIn(types,index,-1)!=DEVICE:  #types[index-1]!=DEVICE:
					print "error: No device correspond to a Port"
					return
				lastWay = 3 - lastWay
				tmp_port = Port('',cell,"",lastWay,row[index-1])
				tmp_port = __addTo(tmp_port, port_list)
				
				# Create the link
				if index-2>=0 and lastPort:
					if LookIn(types,index,-2)==LINK:
						tmp_link = __CreateLink(lastPort,tmp_port)
						tmp_link.SetLinkInfo(row[index-2])
						tmp_link = __addTo(tmp_link,link_list)
					elif LookIn(types,index,-2)!=None:
						tmp_link = __CreateLink(lastPort,tmp_port)
						tmp_link = __addTo(tmp_link,link_list)					
			elif types[index]==LINK:
				pass

			if tmp_port:
				lastPort = tmp_port
			index += 1
		
	# except csv.Error, e:
	    # sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))
	finally:
		print "DEVICEs successfuly imported : ",len(device_list)
		print "PORT successfuly imported : ",len(port_list)
		print "LINK successfuly imported : ",len(link_list)
		for item in device_list+port_list+link_list:
			item.Create()

		print "items added to the Dirty List"
		pdb.set_trace()
		
		DirtyItems = QListViewItem(self.GetSelectWindow().treeroot,"Imported Items")
		for device in device_list:
			elt = QListViewItem(DirtyItems,device.GetName(),str(TN_DEVICE) + "|" + "VELO")

    def GenerateSQL(self,event=None):
	""" Generates a file with the function calls needed to ConfDBLibrary for all the changes made by the user since
	last commit to  ConfDB. If the user has commited changes to the ConfDB and the user calls this function, nothing
	will be generated because there are no objects to generate from, therefore: this function will always _have to_ be
	called _before_ the user store changes in the ConfDB.

	!return - False if unsuccessful
	"""

	dirtylistcopy = self.CleanUpDirtyObjects(False) # we clean and compress the dirty objects list

	# Initializing progress bar and busy cursor
	##self.ProgressData(len(self.__dirtyobjects),False,0)
	self.setCursor(QCursor(Qt.WaitCursor))
	self.ShowError("Please wait while preprocessing and validating objects before save.",ERR_WAIT,False)

	dirtylistcopy = self.FixDataObjectOrder(dirtylistcopy)

	##self.ProgressData(0,True,0)
	self.ShowError("Preprocessing and validating finished successfully. Please wait while generating ConfDBLibrary functions file." ,ERR_WAIT, False)

	firstlist = [] #add Device, DeviceType,Link and LinkType *and Port* after first time use, + GetSaveStatus()
	backup_objects = []
	first = False
	many = True
	last = False #last item of the given type and function (insert, update, delete)
	i = 0
	prev_item = -1
	somefailed = False
    	self.ResetRecoverFile(filename="recover_tmp.dat") #reset "backup" file
	objects_not_stored = []
	
	##self.ProgressData(len(dirtylistcopy),False,0)
	self.ShowError("Please wait while generating ConfDBLibrary functions code, progress is shown in the progressbar.",ERR_WAIT,False)


	for ren in self.__renameobjects:
		dirtylistcopy.insert(0,ren)

	if dirtylistcopy == []: #nothing to store
		self.ShowError("There are no objects to generate ConfDBLibrary functions of.",ERR_ERROR)
		self.setCursor(QCursor(Qt.ArrowCursor))
		return False

	try:
		directory = ""
		while True:
			savedlg = QFileDialog.getSaveFileName(directory,\
			"Python file (*.py)",\
			self,\
			"Save ConfDBLibrary functions file as...",\
			"Save" );
			#savedlg = wxFileDialog(self,"Save ConfDBLibrary functions file as...",directory,"","*.py",wxSAVE)
			savedlg = str(savedlg)
			if savedlg=='':  #clicked cancel
				self.ShowError("Mass Insert : No file selected", ERR_ERROR,False)
				self.setCursor(QCursor(Qt.ArrowCursor))
				return False
			if savedlg[-3::]!='.py':
				savedlg = savedlg + ".py"
			filename =  savedlg[len(savedlg)-savedlg[::-1].index("/"):]
			directory = savedlg.strip(filename)
			if OS_VERSION == WINDOWS:
				if directory.split(":")[1] == "": #we have chosen the root of a drive
					directory += os.sep
			filepath = os.path.join(directory,filename)
			path_already_exists = os.path.exists(filepath)
			if path_already_exists:
				ans = QMessageBox.question(self,"Overwrite File?","The file already exists, do you want to replace the file?","Yes","No","",0,1)

				if ans == 1: # NO : If user changed his/her mind, or didnt intend to replace
					continue #new while loop
				else:
					break #ok, replace file
			break #out of loop, to write to file

		file = open(filepath,"w")

		startcode = ""
		startcode +=   "#################################################\n\
#            AUTOGENERATED PYTHON CODE		#\n\
#                   BY CDBVIS			#\n\
#################################################\n\n"
		startcode += "Edit the following : \ndatabasename=\"Your_DB_Name\"\nusername=\"Username\"\npassword=\"Password\"\n\n"

		if OS_VERSION == LINUX:
			startcode += "from libconfDBpython import *\n"
		elif OS_VERSION == WINDOWS:
			startcode += "from confDBpython import *\n"
		else:
			startcode += "from libconfDBpython import *\n" #linux distro..or linux "compatible" with *.so's

		startcode += "import cx_Oracle\n\n"	
		startcode += 'cfDB = CONFDB(databasename,username,password)\n' # here you can optionally set the username, password and database name to the ConfDB, so that the sqlfunctions file is always ready to be commited to the ConfDB.
		startcode += "cfDB.DBConnexion()\n\n"
		startcode += "oracle = cx_Oracle.connect(username+'/'+password+'@'+databasename)\ncfDB2 = oracle.cursor()\n\n"

		file.write(startcode)
	
		old_and_new_linknames = {} # old name : new object; for update of links in visual window
		while i < len(dirtylistcopy):
			item = dirtylistcopy[i] #index 0

			many = True
			if (item.__class__ in [Port,Device]) and item.GetSaveStatus() == MODIFY:
				my_item = str(item.__class__) + ":" + str(item.GetSaveStatus()) + ":" + str(item.GetModifyStatus())
			else:
				my_item = str(item.__class__) + ":" + str(item.GetSaveStatus())

			#pop it, do not fetch it since we have it in the item variable
			if (i+1) == len(dirtylistcopy): #do not have a next one; this is the last of its type/kind
				last = True
			else:
				if (dirtylistcopy[i+1].__class__ in [Port,Device]) and dirtylistcopy[i+1].GetSaveStatus() == MODIFY:
					next_item = str(dirtylistcopy[i+1].__class__) + ":" + str(dirtylistcopy[i+1].GetSaveStatus()) + ":" + str(dirtylistcopy[i+1].GetModifyStatus())
					
				else:
					next_item = str(dirtylistcopy[i+1].__class__) + ":" + str(dirtylistcopy[i+1].GetSaveStatus())
				if my_item == next_item:
					last = False
				else:
					last = True

			# We use cache to temporary store objects of same kind (same save status (and modify status)), and
			# store them in the ConfDB on the last object of its kind. If the next object in the list is another
			# kind, we store them to the ConfDB, and next iteration we will have to initialize a new cache for the
			# next kind.
			if prev_item != my_item:
				firstlist = []
				if last: #only one of this kind in this sequence. First and last, use single insert/update/delete.
					many = False
			
			# first object of its kind?
			if firstlist == []:
				first = True
			else:
				first = False
			firstlist.append(item.GetName())

			# Debug information
			print "Item: " + str(item.GetName()) + " First: " + str(first) + " many: " + str(many) + " last: " + str(last) + " savestatus: " + str(item.GetSaveStatus())
			my_savestatus = item.GetSaveStatus()

			# Call the save method for the object to be saved/applied to ConfDB
			function_to_file = item.PrintSaveFunction(first,many,last)
			if not last:
				appendix = "\n" #collection of same type, only 1 line break
			else:
				appendix = "\n\n" #next type, 2 line breaks

			#Save to file
			file.write(function_to_file + appendix)

			prev_item = my_item
			dirtylistcopy.pop(i)
			##self.ProgressData(0,False,1) # Update progress bar, one step

		##self.ProgressData(0,True,0) # Stop progress bar, and reset it.
		file.write("\ncfDB.DBDeconnexion()\noracle.commit()\n")
		file.close()

		self.ShowError("ConfDBLibrary functions file successfully generated!",ERR_SUCCESS)
		self.setCursor(QCursor(Qt.ArrowCursor))
		return True
		
	except IOError,err:
		self.ShowError("An unexpected error occured while writing to " + str(filepath) + ".",ERR_CRITICAL)
		self.setCursor(QCursor(Qt.ArrowCursor))
		return False

    def FixDataObjectOrder(self,objectlist=None):
	""" This function is a helper function to OnSave(...) and GenerateSQL(...) to sort the data objects in
	the dirty list (or a copy) after each other in a correct order based on data object type, save status 
	(and modify status). Returns the new and ordered list.

	Parameters:
	@objectlist - if the dirty objects list is not directly used, we use the list that is sent in this parameter. If
	no list is sent (None object), then we use the dirty objects list.

	!return - an save-ordered list of all the objects in the dirty objects list.
	"""
	
	if objectlist == None: # If we do not set another list, use the default dirty objects list (only used in OnSave)
		objectlist = self.__dirtyobjects
	    
	devtypes_sort = {}
	linktypes_sort = {}
	devs_sort = {}
	links_sort = {}
	ports_sort = {}
	#now we want to go through the dirtylist and set several objects of the same type together, since it is
	#chronological we can iterate from the start and add more and more, we create an objectlist of each 
	#objecttype, before we finally set them together in the dirtyobjectlist, in the correct order: 
	#devicetype, linktype, device, link, port, so that the consistency in the ConfDB is not violated, and
	#neither foreign keys and other constraints.
	
	for item in objectlist: #!
		if item.__class__ == Device:
			if item.GetSaveStatus() == MODIFY:
				if item.GetName() in devs_sort.keys(): #looking for prev create objects
					prevstatus = devs_sort[item.GetName()].GetSaveStatus()
					devs_sort[item.GetName()].SetDict(item.GetObjectInfo(True))
					devs_sort[item.GetName()].SetSaveStatus(prevstatus)
				else:
					newname = "#1:"+str(item.GetModifyStatus())+":"+item.GetName()
					if newname in devs_sort.keys():
						prevstatus = devs_sort[newname].GetSaveStatus()
						devs_sort[newname].SetDict(item.GetObjectInfo(True))
						devs_sort[newname].SetSaveStatus(prevstatus) #always modify
					else:
						devs_sort[newname] = item
			elif item.GetSaveStatus() == CREATE:
				devs_sort[item.GetName()] = item
			elif item.GetSaveStatus() == DELETE:
				devs_sort["#0:"+item.GetName()] = item
		if item.__class__ == Spare_Device:
			if item.GetSaveStatus() == CREATE:
				devs_sort[item.GetName()] = item
			elif item.GetSaveStatus() == DELETE:
				devs_sort["#0:"+item.GetName()] = item
		if item.__class__ == Spare_Port:
			if item.GetSaveStatus() == CREATE:
				devs_sort[item.GetName()] = item
			elif item.GetSaveStatus() == DELETE:
				devs_sort["#0:"+item.GetName()] = item
		elif item.__class__ == Link:
			if item.GetSaveStatus() == MODIFY:
				newname = "#1:"+item.GetName()
				if item.GetName() in links_sort.keys():
					prevstatus = links_sort[item.GetName()].GetSaveStatus()
					links_sort[item.GetName()].SetDict(item.GetObjectInfo(True))
					links_sort[item.GetName()].SetSaveStatus(prevstatus)
				else:
					if newname in links_sort.keys():
						prevstatus = links_sort[newname].GetSaveStatus()
						links_sort[newname].SetDict(item.GetObjectInfo(True))
						links_sort[newname].SetSaveStatus(prevstatus)
					else:
						links_sort[newname] = item
			elif item.GetSaveStatus() == CREATE:
				links_sort[item.GetName()] = item
			elif item.GetSaveStatus() == DELETE:
				links_sort["#0:"+item.GetName()] = item

		elif item.__class__ == DeviceType:
			if item.GetSaveStatus() == MODIFY:
				newname = "#1:"+item.GetName()
				if item.GetName() in devtypes_sort.keys():
					prevstatus = devtypes_sort[item.GetName()].GetSaveStatus()
					devtypes_sort[item.GetName()].SetDict(item.GetObjectInfo(True))
					devtypes_sort[item.GetName()].SetSaveStatus(prevstatus)
				else:
					if newname in devtypes_sort.keys():
						prevstatus = devtypes_sort[newname].GetSaveStatus()
						devtypes_sort[newname].SetDict(item.GetObjectInfo(True))
						devtypes_sort[newname].SetSaveStatus(prevstatus)
					else:
						devtypes_sort[newname] = item
			elif item.GetSaveStatus() == CREATE:
				devtypes_sort[item.GetName()] = item
			elif item.GetSaveStatus() == DELETE:
				devtypes_sort["#0:"+item.GetName()] = item
		elif item.__class__ == LinkType:
			if item.GetSaveStatus() == MODIFY:
				newname = "#1:"+item.GetName()
				if item.GetName() in linktypes_sort.keys():
					prevstatus = linktypes_sort[item.GetName()].GetSaveStatus()
					linktypes_sort[item.GetName()].SetDict(item.GetObjectInfo(True))
					linktypes_sort[item.GetName()].SetSaveStatus(prevstatus)
				else:
					if newname in linktypes_sort.keys():
						prevstatus = linktypes_sort[newname].GetSaveStatus()
						linktypes_sort[newname].SetDict(item.GetObjectInfo(True))
						linktypes_sort[newname].SetSaveStatus(prevstatus)
					else:
						linktypes_sort[newname] = item
			elif item.GetSaveStatus() == CREATE:
				linktypes_sort[item.GetName()] = item
			elif item.GetSaveStatus() == DELETE:
				linktypes_sort["#0:"+item.GetName()] = item
		elif item.__class__ == Port:
			if item.GetName() in ports_sort.keys(): #ok, we have a create object from before, can add any modify object here
				prevstatus = ports_sort[item.GetName()].GetSaveStatus()
				ports_sort[item.GetName()].SetDict(item.GetObjectInfo(True))
				ports_sort[item.GetName()].SetSaveStatus(prevstatus)
			else: #ok, no create objects, but perhaps an equal modify object?
				if item.GetSaveStatus() == MODIFY:
					newname = "#1:"+str(item.GetModifyStatus())+":"+item.GetName()
					if newname in ports_sort.keys():
						prevstatus = ports_sort[newname].GetSaveStatus()
						ports_sort[newname].SetDict(item.GetObjectInfo(True))
						ports_sort[newname].SetSaveStatus(prevstatus) #not necessary
					else:
						ports_sort[newname] = item
				elif item.GetSaveStatus() == CREATE:
					ports_sort[item.GetName()] = item
				elif item.GetSaveStatus() == DELETE:
					ports_sort["#0:"+item.GetName()] = item
		##self.ProgressData(0,False,1)


	#Now we sort the data objects by the save status, first delete, then modify and create last.
	#For an object there is only 1 action that will be stored in the ConfDB, because we put together
	#several create and modify, we cancel out create,modify,delete etc.
	mysorteddevtypes = devtypes_sort.keys()
	mysorteddevtypes.sort()
	mysortedlinktypes = linktypes_sort.keys()
	mysortedlinktypes.sort()
	mysorteddevs = devs_sort.keys()
	mysorteddevs.sort()
	mysortedlinks = links_sort.keys()
	mysortedlinks.sort()
	mysortedports = ports_sort.keys()
	mysortedports.sort()

	#self.__dirtyobjects = []
	objectlist = []
	for itm in mysorteddevtypes:
		objectlist.append(devtypes_sort[itm]) #!
	for itm in mysortedlinktypes:
		objectlist.append(linktypes_sort[itm]) #!
	for itm in mysorteddevs:
		objectlist.append(devs_sort[itm]) #!
	for itm in mysortedlinks:
		objectlist.append(links_sort[itm]) #!
	for itm in mysortedports:
		objectlist.append(ports_sort[itm]) #!

	return objectlist

    def OnSave(self,event=None,prompt=True):
	""" Save/Apply objects in dirty objects list to the ConfDB, remove one and one after successful save.
	After every object is saved, reset dirtylist, undolist and redolist.

	More details on each step in this method is explained below in the code.
	"""
	
	if prompt:
		ans = QMessageBox.question(self,"Commit Changes?","Are you sure that you want to store the changes made in CdbVis to the ConfDB?","Yes","No","",0,1)
		if ans == 1: # NO : If user changed his/her mind, or didnt intend to
			return False

	# Initializing progress bar and busy cursor
	##self.ProgressData(len(self.__dirtyobjects),False,0)
	##wxBeginBusyCursor()
	self.ShowError("Please wait while preprocessing and validating objects before save.",ERR_WAIT, False)

	# Check if there is something to save, and to clean up objects cancelling out each other.
	self.CleanUpDirtyObjects(True)

	self.__dirtyobjects = self.FixDataObjectOrder() #Fix the order of the data objects
	
	##self.ProgressData(0,True,0)
	self.ShowError("Preprocessing and validating finished successfully. Please wait while saving objects in the database." ,ERR_SUCCESS, False)
	##wxEndBusyCursor()

	# We will need som info to handle the commit/cache functions. We will need to count nr of inserts, 
	#updates and deletes for each objecttype so that we set commit on the last one. If commit fails, 
	#none of the objects is saved, therefore they should be put in a backuplist just in case...
	firstlist = [] #add Device, DeviceType,Link and LinkType *and Port* after first time use, + GetSaveStatus()
	backup_objects = []
	first = False
	many = True
	last = False #last item of the given type and function (insert, update, delete)
	i = 0
	prev_item = -1
	somefailed = False
    	self.ResetRecoverFile(filename="recover_tmp.dat") #reset "backup" file
	objects_not_stored = []
	
	##self.ProgressData(len(self.__dirtyobjects)+len(self.__renameobjects),False,0)
	##wxBeginBusyCursor()
	self.ShowError("Please wait while saving objects in the database",ERR_WAIT, False)

	for ren in self.__renameobjects:
		self.__dirtyobjects.insert(0,ren)

	old_and_new_linknames = {} # old name : new object; for update of links in visual window
	while i < len(self.__dirtyobjects):
		item = self.__dirtyobjects[i] #index 0

		many = True
		if (item.__class__ in [Port,Device]) and item.GetSaveStatus() == MODIFY:
			my_item = str(item.__class__) + ":" + str(item.GetSaveStatus()) + ":" + str(item.GetModifyStatus())
		else:
			my_item = str(item.__class__) + ":" + str(item.GetSaveStatus())

		#pop it, do not fetch it since we have it in the item variable
		if (i+1) == len(self.__dirtyobjects): #do not have a next one; this is the last of its type/kind
			last = True
		else:
			if (self.__dirtyobjects[i+1].__class__ in [Port,Device]) and self.__dirtyobjects[i+1].GetSaveStatus() == MODIFY:
				next_item = str(self.__dirtyobjects[i+1].__class__) + ":" + str(self.__dirtyobjects[i+1].GetSaveStatus()) + ":" + str(self.__dirtyobjects[i+1].GetModifyStatus())
				
			else:
				next_item = str(self.__dirtyobjects[i+1].__class__) + ":" + str(self.__dirtyobjects[i+1].GetSaveStatus())
			if my_item == next_item:
				last = False
			else:
				last = True

		# We use cache to temporary store objects of same kind (same save status (and modify status)), and
		# store them in the ConfDB on the last object of its kind. If the next object in the list is another
		# kind, we store them to the ConfDB, and next iteration we will have to initialize a new cache for the
		# next kind.
		if prev_item != my_item:
			firstlist = []
			if last: #only one of this kind in this sequence. First and last, use single insert/update/delete.
				many = False
		
		# first object of its kind?
		if firstlist == []:
			first = True
		else:
			first = False
		firstlist.append(item.GetName())

		# Debug information
		print "Item: " + str(item.GetName()) + " First: " + str(first) + " many: " + str(many) + " last: " + str(last) + " savestatus: " + str(item.GetSaveStatus())
		my_savestatus = item.GetSaveStatus()

		# Call the save method for the object to be saved/applied to ConfDB
		success = item.Save(first,many,last)

		if success == False: # Save failed
			somefailed = True
			for obj in backup_objects: # Backup all objects of same kind, that was in cache and not commited yet
				objects_not_stored.append(obj)
    				self.RecoverObjectToFile(obj,DESIGN,filename="recover_tmp.dat",write="a") # temp recover file
				
			self.RecoverObjectToFile(item,DESIGN,filename="recover_tmp.dat",write="a") # also the failing object
			objects_not_stored.append(item)

			self.ShowError("An unexpected error occured while trying to save the " + str(item.__class__) + ": " + str(item.GetName()) + ": " + str(item.GetErrorMessage()),ERR_ERROR)
			# We do not return False, we will try to finish.
		elif success == True and last == True: #Success and commited
			backup_objects.append(item) # we also add the current item to the list for update
			if item.__class__ == Link and my_savestatus != DELETE:
				# Update Linkname to the LinkID from the temporary link name we set on it, of course not when
				# deleted.
				for itm in backup_objects:
					tmp_oldname = itm.GetName()
					print "Old name: " + str(tmp_oldname)
					success = itm.UpdateLinkID()
					print "New name: " + str(itm.GetName())
					if not success:
						self.ShowError("An unexpected error occured while converting temporary link names to linkids in the database: " + str(itm.GetErrorMessage()),ERR_ERROR)
					else: #if success
						old_and_new_linknames[tmp_oldname] = itm

				# Update all names in visual window for links; if they are there
				for old_linkname in old_and_new_linknames:
						vis_linkobj = self.GetVisWindow().FindByName(old_linkname)
						if vis_linkobj != None:
							if old_linkname != old_and_new_linknames[old_linkname].GetName(): # if changed
								vis_linkobj.SetText(old_and_new_linknames[old_linkname].GetName())#new
							vis_linkobj.SetObjectRef(old_and_new_linknames[old_linkname])
							
				old_and_new_linknames = {} # old name : new object; for update of links in visual window; RESET.
						
			elif item.__class__ != Link and my_savestatus != DELETE: # update with ConfDB info
				for itm in backup_objects:
					itm.Update(True) #retrieve data from database.
					# Not update objects in visual window; should be update already
			backup_objects = [] #reset list

		elif success == True and last != True: # not commit yet, backup objects temp in a list
			backup_objects.append(item)

		prev_item = my_item
		self.__dirtyobjects.pop(i)
		##self.ProgressData(0,False,1) # Update progress bar, one step

	##self.ProgressData(0,True,0) # Stop progress bar, and reset it.

	#Ok, reset files
	self.ResetRecoverFile() #Reset file yes, and make ready to put in those objects that could not be saved.
	self.ResetDirtyObjects() #includes undo list and redo list

	if somefailed: #Recovering failed objects.
		self.__dirtyobjects = objects_not_stored
		for obj in self.__dirtyobjects:
			self.RecoverObjectToFile(obj,DESIGN)
    		self.ResetRecoverFile(filename="recover_tmp.dat") #reset backup recover file, but this file must also be taken into consideration after if the program fails in the middle of saving, or right before fixing the proper recover.dat...
		
		self.ShowError("Not all objects stored in ConfDB. Double-click on the message icon to see the logged errors.",ERR_ERROR)
	else:
		self.ShowError("All objects successfully stored in database!",ERR_SUCCESS,True)
		self.SetCreationMode(False)  # Return to the creation Mode
	

    def OnDelete(self,event):
	""" Delete one or several objects in the visual window. This method only calls some big
	methods in the visual window module that actually is responsible for errors etc. that
	may happen.
	"""
	self.GetVisWindow().DoDelete(None,True,refresh=False)
	self.GetVisWindow().Refresh()

    def OnDuplicate(self,event):
	""" Duplicate one or several objects in the visual window. This method only calls some big
	methods in the visual window moudle that actually is responsible for errors etc. that may
	happen.
	"""
	self.GetVisWindow().DoDuplicateHelper(event,True)

    def OnClone(self,event):
	""" Clone one or several objects in the visual window. This method only calls some big
	methods in the visual window moudle that actually is responsible for errors etc. that may
	happen.
	"""
	self.GetVisWindow().DoCloneHelper(event,True)

    def ProgressData(self,range=0,finished=False,step=0):
	""" Initialize, Update or Reset the progress bar in the status bar for an action.

	@range - set when progress bar is initialized, how many steps the progress bar should show
	@finished - if this is set to True, it is the reset of the progress bar, if False, ignore
	@step - how big step for the last update; usually 1, like once every iteration.
	"""
	
	if range != 0:
		self.progressbar.SetRange(range)
		self.pbarvalue = 0
		wxYield() # Needed to make the CPU give some time to refresh GUI.
	elif finished == True:
		self.pbarvalue = 0
		wxYield()
	else: #update
		self.pbarvalue += step
		wxYield()

    def OnTimer(self,event):
	""" We will trigger a timer event quite frequently so that we can update our 
	progressbar value and graphics (because wxYield() doesnt give any CPU time
	in Windows.
	"""

	self.progressbar.SetValue(self.pbarvalue)

    def IdleHandler(self,event):
	""" Called when wxYield() is called, short idle time for the CPU to update the GUI;
	here we update the progress bar state.
	"""
	
	self.progressbar.SetValue(self.pbarvalue)

    def NeedToClearWindow(self):
	""" ??? Obsolete ???
	"""
	return self.__clearfordynamic

    def ViewSubSystem(self,refresh=True):
	""" The new name is ViewNHopsOfDevices. The user clicks on a device node in the tree view in this view mode,
	a input box will pop up where the user has to type in how many hops out from this device he/she wants to see.
	Then the private draw function of this function will draw the devices recursively.

	@refresh - whether we should manually trigger a paint event (if the program does not do it itself)
	"""

	#First we get the currently active device, if any, if none is active, we quite and warns the user
	active_device = self.GetActiveDevice()
	if active_device in [None,""]:
		self.ShowError("You have not chosen any device to start from. A device has to be selected!", ERR_INFORMATION,False)
		return False

	#Then we ask the user about how many levels/links/hops he/she wants to view
	nr_of_hops = wxGetNumberFromUser("How many hops/links do you want to view outside this device?","Nr. of hops","Nr. of hops",1,0,1000,self)
	if nr_of_hops == -1:
		self.ShowError("Invalid number of hops.",ERR_ERROR)
		return False
	teststr = str(nr_of_hops)
	if teststr.find(".",0,len(teststr)) != -1: #float
		self.ShowError("Float number is not supported.",ERR_ERROR)
		return False
	
	#Inform user that it is processing
	# (We cannot indicate this with a progress bar becaus then we would have to know how many devices there are to
	# process, we could find out but then it would take much longer time to process...as we had to calculate this
	# before we could really start)
	wxBeginBusyCursor()
	self.ShowError("Please wait while processing content",ERR_INFORMATION, False)


	# We create the two lists we will need, a list of devices, to be filled up as we iterate
	# and a list of links for each device we process, as well as a list of already added devices, so that we dont
	# add them again...
	list_of_links = []
	list_of_added_devices = []

	# And clear the visual window
	self.GetVisWindow().DoDeleteAll(refresh=True)

	# internal recursive function for finding all devices within maxlevels for the selected active device
	def findDevicesRec(list_of_links,level=2,maxlevel=0,inlinks=True,list_of_added_devices=[],list_of_expanded_devices=[],maxhop=1):
		""" This private function finds and draws a device and its neighbors in the visual window recursively
		    until there are no more links to neighbors or the maximum hop distance given by the user has been
		    reached.

		    @list_of_links - a list of link objects from the previous device added, to expand its neighbors
		    if we have not reached the maximum level
		    @level - the level or hop distance we are at, we start at hop 2 since the first device and its 
		    neighbors are added before this function is called
		    @maxlevel - the maximum level/hop distance given by the user
		    @inlinks - whether the list_of_links list is of links going in or out of the previous device added
		    @list_of_added_devices - list of devices that have been drawn and possibly expanded in the visual window
		    @list_of_expanded_devices - list of devices that have been expanded in the visual window
		    @maxhop - the highest level so far we have reached in this recursion...to controll progress bar

		    !return @list_of_added_devices, @list_of_expanded_devices, @maxhop, to get the data for recursive search
		    on the other branch

		    TODO: add list_of_links, list_of_added_devices and list_of_expanded_devices to global list for less
		    memory consumption
		"""

		#break statement 1 -> if we've passed the max level of hops given
		if (level > maxlevel):
			return list_of_added_devices,list_of_expanded_devices,maxhop

		#break statement 2 -> if there are no links to iterate through
		if (list_of_links == []):
			return list_of_added_devices,list_of_expanded_devices,maxhop
		
		#Just to show the progress to the user, so that he knows that it is processing
		#One step for each new level hop we reach
		if (level > maxhop):
			self.ProgressData(0,False,1) #1 step
			maxhop = level

		new_links_in = []
		new_links_out = []
		for link in list_of_links:
			if inlinks:
				tmp_devicename = link.GetNodeFrom()
			else:
				tmp_devicename = link.GetNodeTo()

			# if the next level is the last level, we only check if the device has already been added
			# cause it will not have needed to have been expanded, because its to many hops away, at least
			# on the current path
			# If the next level is not the last level, we check whether it has been expanded or not
			if ((level+1 > maxlevel and tmp_devicename not in list_of_added_devices) or \
			(level+1 <= maxlevel and tmp_devicename not in list_of_expanded_devices)):
				tmp_device = Device(self.GetActiveSystem(),tmp_devicename,False)
				inL,outL = self.GetVisWindow().OnDeviceSelected(tmp_device,False,True,0,0,False)

				# Next level is not the last level, the device will be expanded, so we add links for it
				if level+1 <= maxlevel:
					list_of_expanded_devices.append(tmp_devicename)
					new_links_in.extend(inL)
					new_links_out.extend(outL)

				# always add the device to say that is has been added
				list_of_added_devices.append(tmp_devicename)

		# Then we take the left branch (input links) recursively
		list_of_added_devices,list_of_expanded_devices,maxhop = findDevicesRec(new_links_in,level+1,maxlevel,True,list_of_added_devices,list_of_expanded_devices,maxhop)
		# and finally the right branch (output links) recursively
		return findDevicesRec(new_links_out,level+1,maxlevel,False,list_of_added_devices,list_of_expanded_devices,maxhop)
	#And now we are ready to start on the real thing, the processing
    	self.ProgressData(nr_of_hops*2,False,0)
	tmp_device = Device(self.GetActiveSystem(),active_device,False)
	maxhop = 1
	list_of_expanded_devices = []
	list_of_added_devices.append(active_device)
	list_of_expanded_devices.append(active_device)
	if nr_of_hops == 0: #only the device itself is drawn
		self.GetVisWindow().AddNode(tmp_device,0,0) #hop 0
	else:
		# add the node selected itself, and if available, its neighbors
		inL,outL = self.GetVisWindow().OnDeviceSelected(tmp_device,False,True,0,0,False) #hop 0 (and 1)
		self.ProgressData(0,False,1) #first step
		# inL and outL now consists of links to and from this device, from and to devices it was connected to
		# so that we can draw them the same way
		if inL != []:
			list_of_added_devices,list_of_expanded_devices,maxhop1 = findDevicesRec(inL,2,nr_of_hops,True,list_of_added_devices,list_of_expanded_devices,maxhop)
		if outL != []:
			list_of_added_devices,list_of_expanded_devices,maxhop = findDevicesRec(outL,2,nr_of_hops,False,list_of_added_devices,list_of_expanded_devices,maxhop)

	self.ProgressData(0,True,0) #finished,step

	self.GetVisWindow().DeselectAll(refresh=False)

	visobj = self.GetVisWindow().FindByName(active_device)
	if visobj != None:
		self.GetVisWindow().Select(visobj,refresh=False)
		self.GetVisWindow().CenterToObject(visobj)
	else:
		self.GetVisWindow().Center() #so that the user see what that is drawn

	if refresh:
		self.GetVisWindow().Refresh()

	self.ShowError("The chosen selection of devices was successfully created." ,ERR_INFORMATION,False)
	wxEndBusyCursor()

    def GetCdb(self):
        """ Return handle on Configuration Database object; the DBSystem object/class
            If not connected return None.

	    !return - the reference to the ConfDB object.
        """
	
        return self.__cdb

    def SetCdb(self,cdb):
	""" Set the ConfDB object.

	@cdb - the ConfDB object reference.
	"""
	
	self.__cdb = cdb

    def ResetActiveObjects(self):
	""" Reset all active objects of all kinds.
	"""
	
	self.__activedevicetype = None
	self.__activedevice = None
	self.__activeport = None
	self.__activelinktype = None
	self.__activelink = None

    def SetActiveSystem(self,system,source):
	""" Change or set the active subsystem, so that this will be affected in all windows: tree view and visual window.

	@system - the new active subsystem
	@source - in which window we changed the subsystem: "visual" or "select"
	"""
	
	if self.__activesystem != system:
		self.__activesystem = system

		visual = self.panel.GetVisWindow()
		select = self.panel.GetSelectWindow()

		if source == "visual":
		    select.SystemChanged()	
		else: # Called from this window
		    # also update visual window somehow
		    pass

		#WALPROB self.GetStatusBar().SetStatusText(str(self.__activesystem),3)
		#WALPROB self.SetCursor(wxStockCursor(wxCURSOR_ARROW))
		
    """ Set or Get active objects, or set or get available objects of all kinds defined in objectclasses.py. """
    def GetActiveSystem(self):
	return self.__activesystem
    def SetAvailableSystems(self,systems):
	self.__availablesystems = systems
    def GetAvailableSystems(self):
	return self.__availablesystems
    def GetActiveDeviceType(self):
	return self.__activedevicetype
    def SetActiveDeviceType(self,devtype):
	self.__activedevicetype = devtype
    def SetAvailableDeviceTypes(self,devtypes):
	self.__availabledevicetypes = devtypes
    def GetAvailableDeviceTypes(self):
	return self.__availabledevicetypes
    def AddAvailableDeviceType(self,devtype):
	self.__availabledevicetypes.append(devtype)
    def RemoveAvailableDeviceType(self,devtype):
	self.__availabledevicetypes.remove(devtype)
    def GetActiveDevice(self):
	return self.__activedevice
    def SetActiveDevice(self,device):
	self.__activedevice = device
    def SetAvailableDevices(self,devices):
	self.__availabledevices = devices
    def GetAvailableDevices(self):
	return self.__availabledevices
    def AddAvailableDevice(self,device):
	self.__availabledevices.append(device)
    def RemoveAvailableDevice(self,device):
	self.__availabledevices.remove(device)
    def GetActivePort(self):
	return self.__activeport
    def SetActivePort(self,port):
	self.__activeport = port
    def SetAvailablePorts(self,ports):
	self.__availableports = ports
    def GetAvailablePorts(self):
	return self.__availableports
    def AddAvailablePort(self,port):
	self.__availableports.append(port)
    def RemoveAvailablePort(self,port):
    	self.__availableports.remove(port)
    def GetActiveLinkType(self):
	return self.__activelinktype
    def SetActiveLinkType(self,linktype):
	self.__activelinktype = linktype
    def SetAvailableLinkTypes(self,linktypes):
	self.__availablelinktypes = linktypes
    def GetAvailableLinkTypes(self):
	return self.__availablelinktypes
    def AddAvailableLinkType(self,linktype):
	self.__availablelinktypes.append(linktype)
    def RemoveAvailableLinkType(self,linktype):
	self.__availablelinktypes.remove(linktype)

    """ Available link types and devices in current subsystem set and get here. """
    def GetLinkTypeTable(self):
	return self.__linktypetable
    def SetLinkTypeTable(self,table):
	self.__linktypetable = table
    def SetRestrictedSparePart(self,sparepart):
	self.__restrictedsparepart = sparepart
	print self.__restrictedsparepart
    def GetRestrictedSparePart(self):
	return self.__restrictedsparepart
  
    def GetDeviceTable(self):
	return self.__devicetable
    def SetDeviceTable(self,table):
	self.__devicetable = table

    """ Get info on restricted link type: links of link type that is the only one to be shown. """
    def RestrictLinkType(self,restrict=True):
	""" If this is set to TRUE, only links of the active linktype is shown. Else links of all linktypes is shown.
	
	@restrict - Whether only links of the active link type should be shown, or all kinds of links (active link type set
	to "")
	"""
	
	self.__restrictedLinks = restrict
	
    def IsLinkTypeRestricted(self):
	return self.__restrictedLinks
    def GetRestrictedLinkType(self):
	return self.__restrictedlinktype
    def SetRestrictedLinkType(self,linktype):
	self.__restrictedlinktype = linktype

    def GetActiveLink(self):
	return self.__activelink
    def SetActiveLink(self,link):
	self.__activelink = link
    def SetAvailableLinks(self,links):
	self.__availablelinks = links
    def GetAvailableLinks(self):
	return self.__availablelinks
    def AddAvailableLink(self,link):
	self.__availablelinks.append(link)
    def RemoveAvailableLink(self,link):
	self.__availablelinks.remove(link)


    def test(self,event):
	""" React on File/Connect menu action to connect to the ConfDB, set up the
	"""
      	if self.GetCdb(): # already connected
            return
        
      	wxBeginBusyCursor()
	if not self.__cfg:
	    path = os.path.join(os.getcwd(),"settings.cfg") #default file name

    # Menu-EventHandler
    def OnConnect(self):
	""" React on File/Connect menu action to connect to the ConfDB, set up the
	configuration file and initialize the program after ConfDB connection.

	!return - False on error, True on success.
	"""
      	# 2DO: draw a dialog to select the parameters (username,pwd,db), animation during connect
      	cfgRootPath = "/db/devdb"
	
      	if self.GetCdb(): # already connected
            return
        
	self.setCursor(QCursor(Qt.WaitCursor))
	
	if not self.__cfg:
		path = os.path.join(os.getcwd(),"settings.cfg") #default file name
		if os.path.exists(path):
			self.__cfg = ConfigParser.ConfigParser() #if another config file has not been loaded by the user
			self.__cfg.read(path)
		else:
			QMessageBox.warning(self,"Error","No configuration file is specified, \nchoose File > Load settings... to open the config file")
			del self.__cdb
			self.unsetCursor()
			return False


	#Our new dbsystem object: to handle dbconnection and retrieval of subsystems from settingsfile
	self.__cdb = DBSystem(self.__cfg,self)

        if not self.__cdb.Connect(cfgRootPath): # Try to connect to ConfDB
		self.aConnect.setOn(False) # self.GetToolBar().ToggleTool(CONN_TOGGLE_BUTTON,False)
		self.aDisconn.setOn(True) # self.GetToolBar().ToggleTool(DISCONN_TOGGLE_BUTTON,True)
            	self.setCursor(QCursor(Qt.ArrowCursor)) # wxEndBusyCursor()
	      	self.ShowError("Connection error: " + self.__cdb.GetErrorMessage(),ERR_ERROR,True)
		self.__cdb = None
            	return False
	
	# Update tool bar
	self.aConnect.setEnabled(False) # self.GetMenuBar().Enable(ID_CONNECT, False)
	self.aDisconn.setEnabled(True)  # self.GetMenuBar().Enable(ID_DISCONNECT, True)

	self.aConnect.setOn(True)  # self.GetToolBar().ToggleTool(CONN_TOGGLE_BUTTON,True)
	self.aDisconn.setOn(False) # self.GetToolBar().ToggleTool(DISCONN_TOGGLE_BUTTON,False)
	self.setCursor(QCursor(Qt.ArrowCursor))
	self.ShowError("Connected...",ERR_INFORMATION,False)
	
	self.DbUpdate(DB_CONNECTED)
	## !!!!!!!!  self.SetUserMode(None)
	
    def OnDisconnect(self):
        """ React on File/Disconnect menu action to disconnect form the ConfDB, and update the program
	to after ConfDB disconnection.
        """
        if not self.GetCdb(): # Not connected
		return
        
	if self.__cdb.Disconnect() == True: # Try to disconnect
	    # self.GetMenuBar().Enable(ID_CONNECT, True)
            # self.GetMenuBar().Enable(ID_DISCONNECT, False) 
	    self.ShowError("Disconnected...",ERR_INFORMATION,False)
	    self.__cdb = None
	    self.DbUpdate(DB_DISCONNECTED)
	    # self.GetToolBar().EnableTool(DISCONN_TOGGLE_BUTTON,False)
	    # self.GetToolBar().EnableTool(CONN_TOGGLE_BUTTON,True)
	    # self.GetToolBar().ToggleTool(CONN_TOGGLE_BUTTON,False)
	    # self.GetToolBar().ToggleTool(DISCONN_TOGGLE_BUTTON,True)
	    self.aDisconn.setEnabled(False)
	    self.aConnect.setEnabled(True)
	else:
	    self.ShowError("Disconnection error: " + self.__cdb.GetErrorMessage(),ERR_ERROR,True)
    
    def OnOpen(self,event):
	""" Open a configuration file. We use the settings in the config file to:
	    find user,password and database name for connecting to the database
	    and find out what systems that are created
	"""
	
	self.__directoryname = ""
	opendlg = wxFileDialog(self,"Choose a Configuration File",self.__directoryname,"","*.cfg",wxOPEN)
	
	if opendlg.ShowModal() == wxID_OK: #The OK button was clicked when closing the dialog
		filename = opendlg.GetFilename()
		directory = opendlg.GetDirectory()
        	self.__cfg = wxConfig("cdbVis","CERN",os.path.join(directory,filename))

	opendlg.Destroy()
    
	
    def closeEvent(self,event):
        """ React on File/Exit menu action. Prompt for saving changes, if not everything is applied
	to the ConfDB, possible to veto the closure.
        """
	empty_dirtylist = self.CleanUpDirtyObjects(False) # Check if there is any obj in dirty objects list toapply to ConfDB
	if empty_dirtylist != []: #only save if there is something to save
		ans = QMessageBox.question(self,"Save Changes?","You have chosen to close the application, but there are objects you have\ncreated/modified/deleted that are not saved yet. Save Changes?","Yes","No","Cancel",0,1)
		if ans == 0: #Yes
			#do save, close # error may arise
			res = self.OnSave(None,False)
			if res == False:
				return False # Do not exit because of save error
		elif ans == 2: #Cancel
			#do not close app.
			return
		elif ans == 1: #No
			#do not save, close
			pass

        if self.GetCdb(): # Disconnect gracefully from ConfDB if connected
            self.OnDisconnect()
	    self.__cdb = None
	
	event.accept() # Close the application


    def OnAbout(self, event):
        """ React on Help/About menu action and show a dialog with Program version,
	date and other information
        """
	
        dlg = wxMessageDialog(self, "CdbVis\n"
                                    "Configuration Database Visualizer\n"
                                    "Version " + VERSION + "\n"
                                    "Date " + DATE + "\n"
                                    "Felix Schmidt-Eisenlohr, CERN, 2004\nThomas Johansen, CERN, 2005-2006",
                                    "About CdbVis", wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


    def ViewSystemsClick(self,event):
	""" Show or hide the miniature window. If it is shown, this method will hide it,
	if it is hidden, this method will show it.
	"""

	if event == False: #Not caused by event, toggling of check has to be handled manually
		if self.GetMenuBar().IsChecked(ID_VIEW_SUBSYSTEMS):
	    		self.panel.CloseAreaWindow(True)
	    		self.GetMenuBar().Check(ID_VIEW_SUBSYSTEMS,False)
		else:
	    		self.panel.CloseAreaWindow(False)
	    		self.GetMenuBar().Check(ID_VIEW_SUBSYSTEMS,True)
	else:
		if self.GetMenuBar().IsChecked(ID_VIEW_SUBSYSTEMS):
	    		self.panel.CloseAreaWindow(False)
		else:
	    		self.panel.CloseAreaWindow(True)


    def ViewObjectBox(self,event,show=False):
	""" Obsolete, used to have an object box for creating objects (devices etc)
	"""

	if show == True and event==None: #creation mode selected
		self.__objectbox_inst.Show(True)
		self.GetMenuBar().Check(ID_VIEW_OBJECTBOX,True)
		return
	elif show == False and event==None:	
		self.__objectbox_inst.Show(False)
		self.GetMenuBar().Check(ID_VIEW_OBJECTBOX,False)
		return
	else:

		if event == False: #triggered bu user or object toolbox window
			if self.GetMenuBar().IsChecked(ID_VIEW_OBJECTBOX): #hide
				self.__objectbox_inst.Show(False)
	    			self.GetMenuBar().Check(ID_VIEW_OBJECTBOX,False)
			else:
				self.__objectbox_inst.Show(True)
		    		self.GetMenuBar().Check(ID_VIEW_OBJECTBOX,True)
		else:
			if self.GetMenuBar().IsChecked(ID_VIEW_OBJECTBOX):
				self.__objectbox_inst.Show(True)
			else: #hide
				self.__objectbox_inst.Show(False)

    def OnSize(self,event):
	""" React on the resize of the Main Window: set the error/information message to
	be shown in the status bar (have to resize the size of the text control)
	"""
	
	self.ShowError(self.__previouserror[1],self.__previouserror[0],False,log=False)
	event.Skip()

    def ShowLog(self,event=None):
	""" Show the log window where the user can view every error and informative message
	sent to the user in the	current session.
	"""
	
	logwindow = LogWindow(self,-1,self.__logsessionfile)
	logwindow.ShowModal()


    def ShowError(self,descr,errtype,forcemsgbox=True,log=True):
	""" Show an error or informative message to the user, either in the status bar or in
	a message box.
	
	@descr - the message to the user
	@errtype - the message type - ERR_INFORMATION, ERR_ERROR or ERR_CRITICAL
	@forcemsgbox - whether we should force to show a message box with the message to the user or not.
	@log - whether the message should be logged to the log session file or not.
	"""
	
	if log:
		try:
			file = open(self.__logsessionfile,"r")
			file = open(self.__logsessionfile,"a")
		except IOError,err:
			file = open(self.__logsessionfile,"w") #file does not exist, ignore recover, but create file
			
		file.close()

		errnames = ["Information","Critical","Error","Busy","SUCCESSFUL"]
		file = open(self.__logsessionfile,"a") #open() is an alias to file(), a - append
		mod_descr = descr[:]
		mod_descr = mod_descr.replace("\n"," ") # We replace line breaks in the original message with spaces
		mod_descr = mod_descr.replace("\r"," ")
		file.write("Error type : " + str(errnames[errtype]) + " | Message : " + str(mod_descr) + "\n")
		file.close()

	ID_INFOTEXT = 987

	# If we show the message in a message box, we have a tiny algorithm to split up the text so that the
	# message boxes are not too big
	# TODO: make the messages be split where there are spaces in the message, and not in the middle of words.
	if (errtype != ERR_INFORMATION and forcemsgbox):
		new_descr = ""
		i = 1
		while True:
			if len(descr) <= (80*i)-1:
				new_descr += descr[80*(i-1):len(descr)]
				break
			new_descr += descr[80*(i-1):80*i] + "\n"
			i += 1

		descr = new_descr

	# Set the text control width of the text control in the status bar: same size as the column
	
	if errtype == ERR_INFORMATION: # informative message
		self.myStatusbar.info.setText("Information : "+descr) #self.statusBar().message("Information : "+descr)
	elif errtype == ERR_CRITICAL: # critical message -> if we can not continue normally.
		self.myStatusbar.icon.setPixmap(self.myStatusbar.imageCross)
		self.myStatusbar.info.setText("Critical : "+descr)
		if forcemsgbox:
			QMessageBox.critical(self,"CDBVis: CRITICAL ERROR",descr,"Ok" )

	elif errtype == ERR_ERROR:
		self.myStatusbar.icon.setPixmap(self.myStatusbar.imageWarning)
		self.myStatusbar.info.setText("Warning : "+descr)
		if forcemsgbox:
			QMessageBox.warning(self,"CDBVis: ERROR",descr,"Ok" )
	elif errtype == ERR_WAIT:
		self.myStatusbar.icon.setPixmap(self.myStatusbar.imageClock)
		self.myStatusbar.info.setText("Busy : "+descr)
	elif errtype == ERR_SUCCESS:
		self.myStatusbar.icon.setPixmap(self.myStatusbar.imageCheck)
		self.myStatusbar.info.setText(descr)
		if forcemsgbox:
			QMessageBox.information(self,"CDBVis: Success",descr,"Ok" )


	
	'''
	if errtype == ERR_INFORMATION: # informative message
		if forcemsgbox:
			ans = QMessageBox.Information(self,"Information!") # wxMessageBox(descr,"Information!", wxOK | wxICON_INFORMATION)
		else:
			##pic = wxArtProvider_GetBitmap(wxART_INFORMATION,wxART_TOOLBAR,(16,16))
			##picbmp = wxStaticBitmap(self.GetStatusBar(),-1,pic)
			##errtext = wxTextCtrl(self.GetStatusBar(),ID_INFOTEXT,descr,size=wxSize(txtctrl_width,20),style=wxTE_READONLY)
			##errtext.SetCursor(wxStockCursor(wxCURSOR_ARROW))
			##errtext.SetForegroundColour(wxColour(0,0,255))
			##errtext.SetBackgroundColour(wxLIGHT_GREY)
	elif errtype == ERR_CRITICAL: # critical message -> if we can not continue normally.
		wxBell()
		if self.__showmsgbox or forcemsgbox:
			ans = wxMessageBox(descr,"Critical error!", wxOK | wxICON_EXCLAMATION)
		else:
			pic = wxArtProvider_GetBitmap(wxART_ERROR,wxART_TOOLBAR,(16,16))
			picbmp = wxStaticBitmap(self.GetStatusBar(),-1,pic)
			errtext = wxTextCtrl(self.GetStatusBar(),ID_INFOTEXT,descr,size=wxSize(txtctrl_width,20),style=wxTE_READONLY)
			errtext.SetCursor(wxStockCursor(wxCURSOR_ARROW))
			errtext.SetForegroundColour(wxColour(255,0,0))
			errtext.SetBackgroundColour(wxColour(0,0,0))
	elif errtype == ERR_ERROR: # normal error message
		wxBell()
		if self.__showmsgbox or forcemsgbox:
			ans = wxMessageBox(descr,"Error!", wxOK | wxICON_ERROR)
		else:
			pic = wxArtProvider_GetBitmap(wxART_ERROR,wxART_TOOLBAR,(16,16))
			picbmp = wxStaticBitmap(self.GetStatusBar(),-1,pic)
			errtext = wxTextCtrl(self.GetStatusBar(),ID_INFOTEXT,descr,size=wxSize(txtctrl_width,20),style=wxTE_READONLY)
			errtext.SetCursor(wxStockCursor(wxCURSOR_ARROW))
			errtext.SetForegroundColour(wxColour(255,0,0))
			errtext.SetBackgroundColour(wxLIGHT_GREY)
	
	if (errtype == ERR_INFORMATION or not self.__showmsgbox) and not forcemsgbox: # show message in status bar
		##self.GetStatusBar().AddWidget(picbmp,pos=0)
		##self.GetStatusBar().AddWidget(errtext,pos=1,horizontalalignment=ESB_ALIGN_LEFT)
	
		self.__previouserror = []
		self.__previouserror.append(errtype)
		self.__previouserror.append(descr)

		#EVT_LEFT_DCLICK(picbmp,self.ShowLog)
		#EVT_LEFT_DCLICK(errtext,self.ForceMsgBox)
	'''
		
    def ForceMsgBox(self,event):
	""" Show the last shown error message in a message box.
	"""
	
	self.ShowError(self.__previouserror[1],self.__previouserror[0],True)

    def RefreshObjectBoxData(self):
	""" Refresh combo boxes in the main window with the most recent information (f.ex.
	if device types or link types have been added or removed, this should be shown immediately).
	"""
	my_subsystem = SubSystem(self.GetCfg(),self.GetActiveSystem())
	my_devicetypes = my_subsystem.GetDeviceTypes()
	self.__devtypecombobox.Clear()
	if my_devicetypes == False:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_ERROR)
	elif my_devicetypes == []:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_INFORMATION,False)
	else:
		map(self.__devtypecombobox.Append,my_devicetypes)

	my_linktypes = my_subsystem.GetLinkTypes()
	self.__linktypetable = my_subsystem.GetLinkTypeTable()

	self.__linktypecombobox.Clear()
	self.GetSelectWindow().linktypeCtrl.Clear()
	if my_linktypes == False:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_ERROR)
	elif my_linktypes == []:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_INFORMATION,False)
	else:
		map(self.__linktypecombobox.Append,my_linktypes)
		map(self.GetSelectWindow().linktypeCtrl.Append, my_linktypes)
		self.GetSelectWindow().linktypeCtrl.Enable(True)
		self.GetSelectWindow().linktyperestrictchk.Enable(True)
		self.GetSelectWindow().linktyperestrictchk.SetValue(False)

    def CreateSystemClick(self,event):
	""" At the moment obsolete. Maybe we should use this to set subsystem settings that are specific for
	a subsystem, and store it in the configuration file. F.ex. image map ++.
	"""

	systemchoices = allvalidsystems[:] #retrieved from global variable allvalidsystems in cdbVisCore module. Force COPY
	systemchoices_tmp = self.GetAvailableSystems() #get already created subsystems
	
	for obj in systemchoices_tmp:
		if obj.GetName() in systemchoices: #not yet created
			systemchoices.remove(obj.GetName())
	
	createsystemframe = CreateSystemWindow(self,-1,systemchoices)
	
	#show create sub system window, and retrieve values
	if createsystemframe.ShowModal() == wxID_OK: #retrieve values from the window
		systemname = createsystemframe.GetSystemName().upper()
		selectedcolor = createsystemframe.GetColorSelected()
		if self.__cdb != None:
			newsubsystem = SubSystem(self.__cfg,systemname)
			newsubsystem.SetColour(selectedcolor)
			#here we should throw up a dialog box telling the user to wait while the scehma(s) is being created...
			wxBeginBusyCursor()
			success = newsubsystem.Create()
			wxEndBusyCursor()
			if success == True:
			     self.DbUpdate(DB_CONNECTED)
			     dlg = wxMessageDialog(self,str(systemname) + " was successfully created.!")
			     dlg.ShowModal()
			     dlg.Destroy()     
			else:
			    dlg = wxMessageDialog(self,newsubsystem.GetErrorMessage(),"Error!",wxOK | wxICON_ERROR)
			    dlg.ShowModal()
			    dlg.Destroy()
					
		else:#if not connected to db
			dlg = wxMessageDialog(self,"Lost connection to database, sub system _not_ created!","Error!",wxOK | wxICON_ERROR)
		        dlg.ShowModal()
		        dlg.Destroy()
	else: #cancel clicked
		pass

	createsystemframe.Destroy()

    def ModifySystemClick(self,event):
	""" Shows the modify window for systems, and initializes all fields with the already set values
	    Perhaps used later if we need to set system settings (Obsolete)
	"""

	activesystem = self.GetActiveSystem()
	if activesystem == None or activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False
	modsubsystem = SubSystem(self.__cfg,activesystem)
	modsubsystem.GetObjectInfo()
	modifysystemframe = CreateSystemWindow(self,-1,[activesystem],False,modsubsystem.GetColour()) #initial values on modify

	if modifysystemframe.ShowModal() == wxID_OK:
		#systemname is the same
		if modsubsystem.IsCreated() == False:
			dlg = wxMessageDialog(self,str(activesystem) + " is not yet created.!")
			dlg.ShowModal()
			dlg.Destroy()
		else:
			if self.__cdb != None: #we have a db connection
				selectedcolor = modifysystemframe.GetColorSelected()

				modsubsystem.SetColour(selectedcolor)
				wxBeginBusyCursor()
				success = modsubsystem.Modify()
				wxEndBusyCursor()

				if success == True:
				     #self.GetSystemWindow().SystemChanged() #need to change, colour might have been changed
				     dlg = wxMessageDialog(self,str(activesystem) + " was successfully modified.!", "Success!",wxOK | wxICON_INFORMATION)
				     dlg.ShowModal()
				     dlg.Destroy()     
				else:
				    dlg = wxMessageDialog(self,modsubsystem.GetErrorMessage(),"Error!",wxOK | wxICON_ERROR)
				    dlg.ShowModal()
				    dlg.Destroy()
			else:#if not connected to db
				dlg = wxMessageDialog(self,"Lost connection to database, sub system _not_ modified!","Error!",wxOK | wxICON_ERROR)
			        dlg.ShowModal()
			        dlg.Destroy()
	else: #cancel clicked
		pass
				
		
	modifysystemframe.Destroy()				
	


    def CreateDeviceTypeClick(self):
	""" Show create device type window, create a new device type, and add it to the dirty objects
	list.
	"""

	# We have to have an active subsystem to store the device type in (can be stored in several)
	activesystem = self.GetActiveSystem()
	if activesystem == None or activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	my_subsystem = SubSystem(self.__cfg,activesystem)
	my_devicetypes = my_subsystem.GetDeviceTypes() # Get all the device types of that subsystem; so that we do not
	
	# create device types with the same name that conflict with each other.
	# TODO: check device type name against device types in the subsystem the user creates them in when the user
	# clicks on OK button in create device type window, because the subsystems may be different from the active
	# subsystem
	my_createdevicetypeframe = CreateDeviceTypeWindow(self,"",activesystem,True,None,my_devicetypes)
	my_createdevicetypeframe.show()
	ans =  my_createdevicetypeframe.exec_loop()
             
        #show create device window, and retrieve values set by the user
        if ans == 1: #retrieve values from the window
		my_devicetype = my_createdevicetypeframe.GetDeviceTypeObj() # get the device type object created
		### self.AddToUndo(self.GetFirstNewUndoIndex(),"create devicetype: " + str(my_devicetype.GetName())) # undo info
		my_devicetype.Create() # add to dirty objects list
		
		try: # add to treeview
                	self.GetSelectWindow().AddItem(my_devicetype,TN_DEVICETYPE)
			##my_id = self.GetSelectWindow().GetItemID("Device types")
			##self.GetSelectWindow().AddItem(my_id,my_devicetype.GetName(),TN_DEVICETYPE)
                except KeyError,err:
			print "petite exception"
			pass
        else: #cancel clicked
                pass


    def UpdateRenamedObject(self,obj,recovertofile=True):
	""" Update this objects renaming to reflect in all previous objects of this type and all objects with 
	references to this object in dirty objects list. Also save the renaming update to recover file (easier than
	iterating through every object in recover file, and restore them with new name/settings if needed).

	@obj - the object that is renamed and to be added to rename objects
	@recovertofile - whether it should be stored in recover file or not
	"""

	# For information about how this renaming works - look in the programmers documentation
	i = 0
	if obj.__class__ == DeviceType:
		while i < len(self.__dirtyobjects):
			if self.__dirtyobjects[i].__class__ == DeviceType and self.__dirtyobjects[i].GetName() == obj.GetOldName(): #rename
				self.__dirtyobjects[i].SetName(obj.GetName())
			elif self.__dirtyobjects[i].__class__ == Device and self.__dirtyobjects[i].GetType() == obj.GetOldName():
				self.__dirtyobjects[i].SetType(obj.GetName(),update=False)
			i += 1
		tmpobj = DeviceType(self.GetActiveSystem(),obj.GetOldName(),True)	
			
	elif obj.__class__ == LinkType:
		while i < len(self.__dirtyobjects):
			if self.__dirtyobjects[i].__class__ == LinkType and self.__dirtyobjects[i].GetName() == obj.GetOldName(): #rename
				self.__dirtyobjects[i].SetName(obj.GetName())
			elif self.__dirtyobjects[i].__class__ == Link and self.__dirtyobjects[i].GetType() == obj.GetOldName():
				self.__dirtyobjects[i].SetType(obj.GetName())
			i += 1
		tmpobj = LinkType(self.GetActiveSystem(),obj.GetOldName(),True)	
	elif obj.__class__ == Device:
		while i < len(self.__dirtyobjects):
			if self.__dirtyobjects[i].__class__ == Device and self.__dirtyobjects[i].GetName() == obj.GetOldName(): #rename
				self.__dirtyobjects[i].SetName(obj.GetName())
			elif self.__dirtyobjects[i].__class__ == Link:
				if self.__dirtyobjects[i].GetNodeTo() == obj.GetOldName():
					self.__dirtyobjects[i].SetNodeTo(obj.GetName())
				elif self.__dirtyobjects[i].GetNodeFrom() == obj.GetOldName():
					self.__dirtyobjects[i].SetNodeFrom(obj.GetName())
			elif self.__dirtyobjects[i].__class__ == Port:
				if self.__dirtyobjects[i].GetDevice() == obj.GetOldName():
					self.__dirtyobjects[i].SetDevice(obj.GetName())
			i += 1


	#look for previous rename objects,in case we have renamed the same object earlier, this will be removed
	j = 0
	found = False
	while j < len(self.__renameobjects):
		if self.__renameobjects[j].__class__ == obj.__class__ and self.__renameobjects[j].GetName() == obj.GetOldName():
			if obj.__class__ == Device:
				tmpobj = Device(self.GetActiveSystem(),obj.GetName(),True)
				tmpobj.SetType(self.__renameobjects[j].GetType(),False)
			elif obj.__class__ == DeviceType:
				tmpobj = DeviceType(self.GetActiveSystem(),obj.GetName(),True)
			elif obj.__class__ == LinkType:
				tmpobj = LinkType(self.GetActiveSystem(),obj.GetName(),True)
			
			found = True
			prev_oldname = self.__renameobjects[j].GetOldName()
			tmpobj.SetID(self.__renameobjects[j].GetID())
			tmpobj.SetOldName(prev_oldname)
			tmpobj.SetSaveStatus(3)

			#set up a list of oldnames to newnames
			self.__newnamesrenamed[prev_oldname] = obj.GetName() #instance of b found in list were a -> b, now b is changed to c, so a -> c
			self.__newnames.append(obj.GetName()) #c is the new name of a renamed obj
			self.__oldnamesrenamed[obj.GetName()] = prev_oldname #c had the name a before renaming

			self.__renameobjects.pop(j)
			self.__renameobjects.insert(0,tmpobj)

			break
		j += 1

	if not found:
		if obj.__class__ == Device:
			tmpobj = Device(self.GetActiveSystem(),obj.GetOldName(),True)
		elif obj.__class__ == DeviceType:
			tmpobj = DeviceType(self.GetActiveSystem(),obj.GetOldName(),True)
		elif obj.__class__ == LinkType:
			tmpobj = LinkType(self.GetActiveSystem(),obj.GetOldName(),True)
	
		#check if it is stored in db, if so, insert rename object on first position in the dirty list
		res = tmpobj.Update(True)
		if res == {}: #not in db
			pass
		elif res == False:
			self.ShowError("An error occured when trying to retrieve data from the database: " + str(tmpobj.GetErrorMessage()),ERR_ERROR)
			#cannot return false here, since we cannot undo the prev. renaming..or?
		else: #dictionary, in DB, not renamed before
			self.__renameobjects.insert(0,obj)
			self.__oldnames.insert(0,obj.GetOldName()) #a has been renamed
			self.__newnamesrenamed[obj.GetOldName()] = obj.GetName() #a has been renamed to b
			self.__newnames.append(obj.GetName()) #b is the new name of the renamed
			self.__oldnamesrenamed[obj.GetName()] = obj.GetOldName() #b had the name a before renaming


	#update objects in visual window, if needed:
	if obj.__class__ == DeviceType:
		for visobj in self.GetVisWindow().GetContents():
			if visobj == Device:
				if visobj.GetObject().GetType() == obj.GetOldName():
					visobj.GetObject().SetType(obj.GetName(),True)
	elif obj.__class__ == LinkType:
		for visobj in self.GetVisWindow().GetContents():
			if visobj == Link:
				if visobj.GetObject().GetType() == obj.GetOldName():
					visobj.GetObject().SetType(obj.GetName(),False)
	elif obj.__class__ == Device:
		for visobj in self.GetVisWindow().GetContents(): #not update ports cause they are not in visual window
			if visobj == Link:
				if visobj.GetObject().GetNodeFrom() == obj.GetOldName():
					visobj.GetObject().SetNodeFrom(obj.GetName())
				elif visobj.GetObject().GetNodeTo() == obj.GetOldName():
					visobj.GetObject().SetNodeTo(obj.GetName())
		
	if recovertofile:
	    	res = self.RecoverObjectToFile(obj,state=DESIGN) #save rename object in recovery file
		# TODO: actually check that this happens...and that the rename object is handled properly and right
		# on recover from file
		if not res:
			return False
		
	return True
	# TODO: update objects in the visual window to be renamed as well.

    def ModifyDeviceTypeClick(self,event):
	""" Open the Modify device type window, let the user do changes to
	the device type, and store it in the dirty objects list, and rename
	list if renamed.
	"""

	# active subsystem
	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	# active device type
	my_activedevicetype = self.GetActiveDeviceType()
	if my_activedevicetype == None or my_activedevicetype == "":
		self.ShowError("No devicetype is selected.",ERR_ERROR)
		return False

	# TODO: check that the device types that are loaded, are always loaded for the
	# subsystem that the device type will be stored in. Same in create device type.
	my_subsystem = SubSystem(self.__cfg,my_activesystem)
	my_devicetypes = my_subsystem.GetDeviceTypes()

	my_devicetype = DeviceType(my_activesystem,my_activedevicetype,False)

	my_createdevicetypeframe = CreateDeviceTypeWindow(self,"Create Device Type",my_activesystem,False,my_devicetype,my_devicetypes)
                                                                                                                                                                       
	if my_createdevicetypeframe.ShowModal() == wxID_OK:
		my_devicetype,renameobj = my_createdevicetypeframe.GetDeviceTypeObj()
		res = True
		if renameobj != None:
			res = self.UpdateRenamedObject(renameobj) 
			#if error occured in the middle of the renaming..we will have to continue
			#TODO: If error occurs, we should return.
			if res:
				try:
					#remove from tree view, and add to tree view (new name)
					my_id = self.GetSelectWindow().GetItemID(renameobj.GetOldName())
					self.GetSelectWindow().RemoveItem(my_id,"Device types") #link and device
					self.GetSelectWindow().SetItemToDeleted(renameobj.GetOldName())

					my_id = self.GetSelectWindow().GetItemID("Device types")
					self.GetSelectWindow().AddItem(renameobj,TN_DEVICETYPE)

					self.SetActiveDeviceType(renameobj.GetName())	 
				except KeyError,err:
					#print "treeitemid not found"
					pass
		if res:
			# undo
			self.AddToUndo(self.GetFirstNewUndoIndex(),"modify devicetype: " + str(my_devicetype.GetName()))
			my_devicetype.Modify() # to dirty objects list
			self.GetSelectWindow().UpdateInfo(my_devicetype)
	else: #cancel clicked
		pass

	my_createdevicetypeframe.Destroy()

    def DoModify(self):
	# define wich componant to Modify :
	if self.GetVisWindow().selection and self.GetActiveDevice():
		self.ModifyDeviceClick(None)
	
	elif self.GetActiveLink():
		toModify = self.GetActiveLink()
		print "modify LINK"
	else:
		print "No device selected ?"
		return
		
    def DoDelete(self):
	# define wich componant to delete :
	if self.GetVisWindow().selection:
		toDelete = self.GetVisWindow().selection
	elif self.GetActiveLink():
		toDelete = [self.GetActiveLink()]
	else:
		return
		
	toDeleteNames = []
	for item in toDelete:
		toDeleteNames.append(item.GetObject().GetName())

	ans = QMessageBox.question(self,"Delete ?","Are you sur you want to delete: %s"%', '.join(toDeleteNames),"Yes","No","",0,1)
	if ans == 1: # No
		return
	self.SetCreationMode(True)
	for item in toDelete:
		if isinstance(item,Box):
			# check if the device is connected to an other devie
			if item.GetObject().GetLinks(False)!=[] or item.GetObject().GetLinks(True)!=[]:
				self.ShowError("Cannot Delete : "+item.GetObject().GetName()+" because it is still connected to an other device",ERR_ERROR,True)
				return
		elif isinstance(item,myLine):
			pass
		else:
			self.ShowError("Cannot Delete : "+item.GetObject().GetName()+" ",ERR_ERROR,True)
			return
			
		item.GetObject().Delete()
		self.DoNotShow.append(item.GetObject().GetName())
		# Delete the item fro the tree:
		## itm = self.GetSelectWindow().treectrl.findItem(item.GetObject().GetName(),0)
		## self.GetSelectWindow().treectrl.takeItem(itm)
		self.ShowError("You will have to commit the changes to the Database, other wise you will continue seeing the deleted items",ERR_INFORMATION,True)
		
	self.GetVisWindow().canvas.update()
	
    def CreateDeviceClick(self,event=None,location=""):
	""" Show the create device window, let the user create a device (and ports), and store
	the device(s) and port(s) in the dirty objects list.

	@location - if the location string of the device to be created is already defined, to be set in the location
	text control in create device window.
	"""

	# active subsystem
	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	my_subsystem = SubSystem(self.__cfg,my_activesystem)
	my_devtypes = my_subsystem.GetDeviceTypes()
	my_functions= "" ### WALID_RPOB self.__cdb.GetAllDeviceFunctions();
	# Get all device types in that subsystem
	# TODO: user can change subsystem in createwindow, therefore we should be able to load
	# device types of that subsystem as well, or else it is inconsistent.
	if my_devtypes == False:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_ERROR)
		return False

	# set device type to either the active one, or the one selected in the combo box
	my_devtype = self.GetActiveDeviceType()
	
	createdeviceframe = CreateDeviceWindow(self,my_activesystem,True,None,my_devtypes,my_devtype,location,my_functions)
	createdeviceframe.show()
	ans =  createdeviceframe.exec_loop()
	
        if ans == 1: #retrieve values from the window
                my_devobjs,my_devports = createdeviceframe.GetDeviceObjs()
		if len(my_devobjs) == 1:
			self.AddToUndo(self.GetFirstNewUndoIndex(),"create device and ports: " + str(my_devobjs[0].GetName()))
		else:
			self.AddToUndo(self.GetFirstNewUndoIndex(),"create " + str(len(my_devobjs)) + " devices with ports")

		j = 0
		
		for my_devobj in my_devobjs:
			my_devobj.Create() # to dirty objects list.
			# add to tree view
			try:
                		# my_id = self.GetSelectWindow().GetItemID(my_devobj.GetType())
				self.GetSelectWindow().AddItem(my_devobj,TN_DEVICE)
	                except KeyError,err:
				pass
 
			# if event == None:
				# pass #called from drop visual device
			# else: # where to place the created devices in the visual window (if many)
				# x_offset,y_offset = self.GetVisWindow().CalcUnscrolledPosition(10,10+j)
				# self.GetVisWindow().CreateNode(x_offset,y_offset,my_devobj,True)
				# visobj = self.GetVisWindow().FindByName(my_devobj.GetName())
				# if visobj:
					# self.GetVisWindow().Select(visobj,refresh=False)

			#Create ports for every device
			for port in my_devports:
				tmp_port = Port(self.GetActiveSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),my_devobj.GetName(),True)
				tmp_port.SetDict(port.GetObjectInfo(True))
				tmp_port.SetDevice(my_devobj.GetName())
				tmp_port.Create() # We are creating new device so we can only create port (no modify or delet possible)
				self.GetSelectWindow().AddItem(tmp_port,TN_PORT) # add the Port to the tree:

			j += 5

		self.SetCreationMode(True)
		self.GetVisWindow().canvas.update()

		if event == None and location == "":
			return my_devobjs #drop objs

	else: #cancel clicked
		return False

	

    def ModifyDeviceClick(self,event=None):
	""" Show the modify device window to the user, let the user do changes
	for the device (and ports), and then add the changes to the dirty objects
	list.
	"""
	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False
	
	my_activedevice = self.GetActiveDevice()

	if my_activedevice == None or my_activedevice == "":
		self.ShowError("No device is selected.",ERR_ERROR)
		return False
	
	if type(my_activedevice)==type("") :
		devName = my_activedevice
	else:
		devName = my_activedevice.GetObject().GetName()
	device_tmp = Device(my_activesystem,devName,False)

	my_subsystem = SubSystem(self.__cfg,my_activesystem)
	my_devtypes = my_subsystem.GetDeviceTypes()
	# TODO: devicetypes to be selected from selected subsystem(s), as in create device

	if my_devtypes == False:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_ERROR)
		return False

	createdeviceframe = CreateDeviceWindow(self,my_activesystem,False,device_tmp,my_devtypes)
	createdeviceframe.show()
	ans =  createdeviceframe.exec_loop()                                                                                                                                                
        #show modify device window, and retrieve values
        if ans == 1: #retrieve values from the window
                self.SetCreationMode(True)
		device_tmp,renameobj,my_devports,deletedports = createdeviceframe.GetDeviceObjs()
		prev_name = device_tmp.GetName()
		if renameobj != None:
			prev_name = renameobj.GetOldName()

		obj = self.GetVisWindow().FindByName(prev_name)

		if renameobj != None:
			prev_name = renameobj.GetOldName()
			res = self.UpdateRenamedObject(renameobj) #what if this fails?
			# if it fails, we should just return, and inform the user.
			try:
				#remove from tree view, and add to tree view (rename)
				# my_id = self.GetSelectWindow().GetItemID(renameobj.GetOldName())
				# self.GetSelectWindow().RemoveItem(my_id,renameobj.GetType()) #link and device
				# self.GetSelectWindow().SetItemToDeleted(renameobj.GetOldName())
				## Walid : I replaced the commented line above by :
				itm = self.GetSelectWindow().treectrl.findItem(renameobj.GetOldName(),0)
				itm.setText(0,renameobj.GetName())


				# my_id = self.GetSelectWindow().GetItemID(renameobj.GetType())
				# self.GetSelectWindow().AddItem(renameobj,TN_DEVICE)
				self.SetActiveDevice(renameobj.GetName())
			except KeyError,err:
				pass
		undoindex = self.GetFirstNewUndoIndex()
		if device_tmp.IsSystemChanged():	
			newtmpdev = Device(device_tmp.GetSystem(),device_tmp.GetName(),True)
			newtmpdev.SetDict(device_tmp.GetObjectInfo(True))
			newtmpdev.SetModifyStatus(1) #system update
			newtmpdev.Modify()
			
		device_tmp.SetModifyStatus(0) # Default.
		device_tmp.Modify() #added to dirtylist
		self.AddToUndo(undoindex,"modify device (and ports): " + str(device_tmp.GetName()))
		self.GetSelectWindow().UpdateInfo(device_tmp) #Update info
		obj.GetObject().SetDict(device_tmp.GetObjectInfo())
		# If any ports where deleted by the user, we add them to dirty objects list here
		for deleted_port in deletedports: #portnbr,porttype,portway,portid
			tmp_port = Port(self.GetActiveSystem()) #,deleted_port[0],deleted_port[1],int(deleted_port[2]),device_tmp.GetName(),True)
			tmp_port.SetDict(deleted_port.GetObjectInfo(True))
			tmp_port.SetPortID(int(deleted_port.GetPortID())) #deleted_port[3]))
			tmp_port.Delete()				

		# add changed ports to dirty objects list
		for port in my_devports:
			tmp_port = Port(port.GetSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),True)
			tmp_port.SetDict(port.GetObjectInfo(True))
			if port.GetSaveStatus() == CREATE:
				tmp_port.Create()
			elif port.GetSaveStatus() == MODIFY:
				tmp_port.Modify()
			'''if port.GetSaveStatus() == 10: #port changed
				port.SetDevice(device_tmp.GetName())

				modifystatus = port.GetModifyStatus()
				if modifystatus.find("3",0,len(modifystatus)) != -1: #this modify status means that it has been created (modify points to that a device is modifyed, and here we have a port that is being created.
					tmp_port = Port(port.GetSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),True)
					tmp_port.SetDict(port.GetObjectInfo(True))
					tmp_port.Create()
				else:

					if modifystatus.find("0",0,len(modifystatus)) != -1:
						tmp_port = Port(port.GetSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),True)
						tmp_port.SetDict(port.GetObjectInfo(True))
						tmp_port.SetModifyStatus(0)
						tmp_port.Modify() #Ports cannot be deleted here, must be done manually!!
					if modifystatus.find("1",0,len(modifystatus)) != -1:
						tmp_port = Port(port.GetSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),True)
						tmp_port.SetDict(port.GetObjectInfo(True))
						tmp_port.SetModifyStatus(1)
						tmp_port.Modify() #Ports cannot be deleted here, must be done manually!!
					if modifystatus.find("2",0,len(modifystatus)) != -1:
						tmp_port = Port(port.GetSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),True)
						tmp_port.SetDict(port.GetObjectInfo(True))
						tmp_port.SetModifyStatus(2)
						tmp_port.Modify() #Ports cannot be deleted here, must be done manually!!

			if not obj:
				obj = Device(device_tmp.GetSystem(),device_tmp.GetName(),False)
			self.__visualundolist.append(obj)

			if obj.__class__ == DrawingObject:
				self.GetVisWindow().Deselect(obj,refresh=False)
				obj.SetObjectRef(device_tmp)
				self.GetVisWindow().Select(obj,refresh=False)
			'''
	else: #cancel clicked
		pass

	self.GetVisWindow().canvas.update()

    def CreateLinkTypeClick(self,event):
	""" Show the create link type window, let the user create a simple
	or composite link type, and add changes to dirty objects list when user is finished.
	"""

	activesystem = self.GetActiveSystem()
	if activesystem == None or activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	my_subsystem = SubSystem(self.__cfg,activesystem)
	my_linktypes = my_subsystem.GetLinkTypes()

	createlinktypeframe = CreateLinkTypeWindow(self,-1,activesystem,True,None,my_linktypes)
	
        #show create linktype window, and retrieve values
        if createlinktypeframe.ShowModal() == wxID_OK: #retrieve values from the window
        	my_linktypeobj = createlinktypeframe.GetLinkTypeObj()	
		self.AddToUndo(self.GetFirstNewUndoIndex(),"create linktype: " + str(my_linktypeobj.GetName()))
		my_linktypeobj.Create()	
		try:
                	my_id = self.GetSelectWindow().GetItemID("Link types")
			self.GetSelectWindow().AddItem(my_linktypeobj,TN_LINKTYPE)
                except KeyError,err:
                	#print "treeitemid not found"
			pass
 
        else: #cancel clicked
                pass

	createlinktypeframe.Destroy()

    def ModifyLinkTypeClick(self,event):
	""" Show the modify link type window, let the user modify the link type,
	and add the changes to the dirty objects list if the user pressed the Ok button
	"""

	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False
	my_activelinktype = self.GetActiveLinkType()

	if my_activelinktype == None:
		self.ShowError("No linktype is selected.",ERR_ERROR)
		return False

	my_subsystem = SubSystem(self.__cfg,my_activesystem)
	my_linktypes = my_subsystem.GetLinkTypes()

	for obj in my_linktypes:
		print str(obj)

	my_linktype = LinkType(my_activesystem,my_activelinktype,False)
	my_createlinktypeframe = CreateLinkTypeWindow(self,-1,my_activesystem,False,my_linktype,my_linktypes)
                                                                                                                                                                       
        if my_createlinktypeframe.ShowModal() == wxID_OK: #retrieve values from the window
        	my_linktype,renameobj = my_createlinktypeframe.GetLinkTypeObj()	
		res = True
		if renameobj != None:
			res = self.UpdateRenamedObject(renameobj)
			if res: #remove from treelist, and replace
				try:
					#remove from treelist
					my_id = self.GetSelectWindow().GetItemID(renameobj.GetOldName())
					self.GetSelectWindow().RemoveItem(my_id,"Link types")
					self.GetSelectWindow().SetItemToDeleted(renameobj.GetOldName())

					my_id = self.GetSelectWindow().GetItemID("Link types")
					self.GetSelectWindow().AddItem(renameobj,TN_LINKTYPE)

					self.SetActiveLinkType(renameobj.GetName())	 
				except KeyError,err:
					pass

		if res:
			self.AddToUndo(self.GetFirstNewUndoIndex(),"modify linktype: " + str(my_linktype.GetName()))
			my_linktype.Modify()
			self.GetSelectWindow().UpdateInfo(my_linktype)
       	else: #cancel clicked
               	pass

	my_createlinktypeframe.Destroy()
    def CreateLinkClick(self,device1,device2):
	self.SetCreationMode(True)
	my_createlinkframe = CreateLinkWindow(self,self,device1,device2)
	my_createlinkframe.show()
	ans =  my_createlinkframe.exec_loop()
	
	
	'''# Creating / deleting the ports modified
	deletedports, createdports =  my_createlinkframe.GetPorts()
	pdb.set_trace()
	# If any ports where deleted by the user, we add them to dirty objects list here
	for deleted_port in deletedports: #portnbr,porttype,portway,portid
		tmp_port = Port(self.GetActiveSystem()) #,deleted_port[0],deleted_port[1],int(deleted_port[2]),device_tmp.GetName(),True)
		tmp_port.SetDict(deleted_port.GetObjectInfo(True))
		tmp_port.SetPortID(int(deleted_port.GetPortID())) #deleted_port[3]))
		tmp_port.Delete()				

	# add changed ports to dirty objects list
	for port in createdports:
		tmp_port = Port(port.GetSystem(),port.GetPortNbr(),port.GetPortType(),port.GetPortWay(),port.GetDevice(),True)
		tmp_port.SetDict(port.GetObjectInfo(True))
		if port.GetSaveStatus() == CREATE:
			tmp_port.Create()'''
	
    def CreateLinkClick_OLD(self,event=None,obj1=None,obj2=None):
	""" Show the create link/connection window, let the user set link properties,
	and connection properties (between which to devices, and ports), and if the user
	pressed the Ok button, add the changes to the dirty objects list.

	@obj1 - if set, the device object for the FROM/START end of the link
	@obj2 - if set, the device object for the TO/END end of the link
	"""
	
	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	my_subsystem = SubSystem(self.__cfg,my_activesystem)
	my_linktypes = my_subsystem.GetLinkTypes()
	my_devtypes = my_subsystem.GetDeviceTypes()

	if my_linktypes == False or my_devtypes == False:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_ERROR)
		return False

	createlinkframe = CreateLinkWindow(self,-1,my_activesystem,True,None,my_linktypes,my_devtypes,True,obj1,obj2)
                                                                                                                                                                       
        #show create device windiw, and retrieve values
        if createlinkframe.ShowModal() == wxID_OK: #retrieve values from the window
                linkobj = createlinkframe.GetLinkObj()
		self.GetVisWindow().CreateConnectedLink(linkobj,wxPoint(0,0),wxSize(0,0),True)
    		self.AddToUndo(self.GetFirstNewUndoIndex(),"create link: " + str(linkobj.GetName()))	
		linkobj.Create() #added to dirtylist
		try:
                	my_id = self.GetSelectWindow().GetItemID(linkobj.GetType())
			self.GetSelectWindow().AddItem(linkobj,TN_LINK)
                except KeyError,err:
			pass
 
	else: #cancel clicked
    		self.SetPreviousLinkNr() #we didnt use the given linknr, set to previous so that we can reuse this number
		pass

	createlinkframe.Destroy()

    def ModifyLinkClick(self,event=None,obj=None,setfromnode=True,newnode=None):
	""" Open the modify link/connection window, let the user change properties
	for the link and connection, and add changes to dirty objects list if the
	user presses the Ok button.

	@obj - the object to modify (link), if not set, the active link is chosen
	@setfromnode - whether the node that is at the start of the link is given in @newnode, or the node at the end
	@newnode - either the device at the start of the link (@setfromnode=True), 
	or at the end of the link (@setfromnode=False)
	"""

	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	if event != None:
		my_activelink = self.GetActiveLink()
		if my_activelink == None or my_activelink == "":
			self.ShowError("No link selected!",ERR_ERROR)
			return False

		link_tmp = Link(my_activesystem,my_activelink,False,False)
	else:
		link_tmp = obj

	my_subsystem = SubSystem(self.__cfg,my_activesystem)
	my_linktypes = my_subsystem.GetLinkTypes()
	my_devtypes = my_subsystem.GetDeviceTypes()

	if my_linktypes == False or my_devtypes == False:
		self.ShowError(my_subsystem.GetErrorMessage(),ERR_ERROR)
		return False

	if obj:
		if setfromnode:
			if obj.GetNodeTo()  == newnode.GetName():
				self.ShowError("You cannot connect a device to itself!",ERR_ERROR)
				return False
		else:
			if obj.GetNodeFrom() == newnode.GetName():
				self.ShowError("You cannot connect a device to itself!",ERR_ERROR)
				return False

	createlinkframe = CreateLinkWindow(self,-1,my_activesystem,False,link_tmp,my_linktypes,my_devtypes,setfromnode,newnode)
                                                                                                                                                                       
        #show modify link window, and retrieve values
        if createlinkframe.ShowModal() == wxID_OK: #retrieve values from the window
		# It is not possible to modify a link in the ConfDB, therefore we do a
		# delete and then a create instead.
                link_tmp = createlinkframe.GetLinkObj()

		create_link = Link(link_tmp.GetSystem(),link_tmp.GetName(),False,True)
		create_link.SetDict(link_tmp.GetObjectInfo(True))

		obj = self.GetVisWindow().FindByName(create_link.GetName())
		if obj:
			self.GetVisWindow().DoDeleteObj(create_link.GetName(),False,refresh=False)
		else:
			obj = Link(create_link.GetSystem(),create_link.GetName(),False,True)
			obj.SetDict(create_link.GetObjectInfo(True))

		self.__visualundolist.append(obj)

		self.GetVisWindow().CreateConnectedLink(create_link)
		self.GetVisWindow().Refresh()

		undoindex = self.GetFirstNewUndoIndex()
		link_tmp.Delete() #added to dirtylist
		create_link.Create()
		# As you see, we never modify links, we always delete them and recreate them, do not think they will
		# be modified that often, so no performance loss
    		self.AddToUndo(undoindex,"modify link: " + str(create_link.GetName()))
	else: #cancel clicked
		pass

	createlinkframe.Destroy()

    def DeleteDeviceTypeClick(self,event):
	""" A window pops up where the user can choose to delete a device
	type in the currently active subsystem. It is only device types that are not yet stored in the ConfDB
	that is shown, because it is not possible to delete device types stored in ConfDB.
	"""
	
	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	tmp_system = SubSystem(self.__cfg,my_activesystem)
	my_devtypes = tmp_system.GetDeviceTypes(onlyfromdl=True)

	if my_devtypes == []: #no devtypes found
		self.ShowError("No devicetypes found in: " + str(my_activesystem),ERR_ERROR)
		return False
	elif my_devtypes == False:
		self.ShowError("Could not find any devicetypes because an error occured: " + tmp_system.GetErrorMessage(),ERR_ERROR)
		return False

	res = wxGetSingleChoice("Select a devicetype to delete from the single-selection listbox.","Delete Devicetype",my_devtypes,self)
	if res != "": #Not cancel
		deltype = DeviceType(my_activesystem,res,False)
		devs = deltype.GetDevices()
		if devs == []: #no devices found, ok
			undoindex = self.GetFirstNewUndoIndex()
			deltype.Delete() #added to dirtylist
    			self.AddToUndo(undoindex,"delete devicetype: " + str(res))
		        try:
                        	#remove from treelist
                                my_id = self.GetSelectWindow().GetItemID(res)
                                self.GetSelectWindow().RemoveItem(my_id,"Device types") #link and device
                                self.GetSelectWindow().SetItemToDeleted(deltype.GetName())
                        except KeyError,err:
                        	#print "treeitemid not found"
				pass

			self.SetActiveDeviceType(None)

		elif devs == False:
			self.ShowError("Could not delete devicetype because an error occured: " + deltype.GetErrorMessage(),ERR_ERROR)
		else: #found devices
			self.ShowError("Could not delete devicetype because there are still devices in the database of the given devicetype.",ERR_ERROR)

    def DeleteDeviceLinkClick(self,event):
	""" Calls the delete method in the visual window, that can delete visual objects. Links can
	be deleted at any time, but devices can only be deleted if they are not stored in ConfDB yet.
	TODO: check that the delete method in the visual window is working properly.
	"""
	
	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	self.GetVisWindow().DoDelete(None,True,refresh=False) #both device and link
	self.GetVisWindow().Refresh()

    def DeleteLinkTypeClick(self,event):
    	""" A window pops up where the user can choose to delete a link
	type in the currently active subsystem. It is only link types that are not yet stored in the ConfDB
	that is shown, because it is not possible to delete link types stored in ConfDB.
	"""

	my_activesystem = self.GetActiveSystem()
	if my_activesystem == None or my_activesystem == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False

	tmp_system = SubSystem(self.__cfg,my_activesystem)
	my_linktypes = tmp_system.GetLinkTypes(onlyfromdl=True)
	if my_linktypes == []: #no linktypes found
		self.ShowError("No linktypes found in: " + str(my_activesystem),ERR_ERROR)
		return False
	elif my_linktypes == False:
		self.ShowError("Could not find any linktypes because an error occured: " + tmp_system.GetErrorMessage(),ERR_ERROR)
		return False

	res = wxGetSingleChoice("Select a linktype to delete from the single-selection listbox.","Delete Linktype",my_linktypes,self)
	if res != "": #Not cancel
		linktype = LinkType(my_activesystem,res,False)
		#devs = deltype.GetLinks()
		links = True #temp cause we don\t have the abovegiven function yet..ask Lana

		if links == []: #no devices found, ok
			undoindex = self.GetFirstNewUndoIndex()
			linktype.Delete() #added to dirtylist
    			self.AddToUndo(undoindex,"delete linktype: " + str(res))	
		        try:
                        	#remove from treelist
                                my_id = self.GetSelectWindow().GetItemID(res)
                                self.GetSelectWindow().RemoveItem(my_id,"Link types") #link and device
                                self.GetSelectWindow().SetItemToDeleted(linktype.GetName())
                        except KeyError,err:
                        	#print "treeitemid not found"
				pass

			self.SetActiveLinkType(None)

		elif links == False:
			self.ShowError("Could not delete linktype because an error occured: " + linktype.GetErrorMessage(),ERR_ERROR)
		else: #found devices
			self.ShowError("Could not delete linktype because there are still links in the database of the given linktype.",ERR_ERROR)

    def OnTest(self, event):
        """ Readct on Test/Test menu action
            Call a function to test their functionality.
	    Just for debugging.
        """
	print "Dirty list: " + str(self.__dirtyobjects)
	#print "Is there anything to store?: " + str(self.IsSaveListEmpty()) + " 0: Yes, 1: No"

	print "UNDO list: " + str(self.__undolist)
	print "Visual UNDO Objects: " + str(self.__visualundolist)

	print "REDO objects: " + str(self.__redoobjectlist)
	print "REDO list: " + str(self.__redolist)
	print "Visual REDO Objects: " + str(self.__visualredolist)
	#print "Renamed objs, old db names: " + str(self.__oldnamesrenamed)

	for itm in self.__dirtyobjects:
		print "Status: " + str(itm.GetObjectInfo(True))

	#print "Visual Contents: " + str(self.GetVisWindow().GetContents())
	#print "Selected Contents: " + str(self.GetVisWindow().GetSelection())


    def GetCfg(self):
        """ Return reference (handle) to the wxConfig file object.

	!return - reference to the wxConfig file object.        
        """
        return self.__cfg

    def GetAreaWindow(self):
	""" Get the handle/reference to the miniature window

	!return - the reference to the miniature window
	"""
	return self.panel.GetAreaWindow()
    
    def GetVisWindow(self):
        """ Return handle/reference to the visuali window
	
	!return - the reference to the visual window
        """
	
        return self.panel.GetVisWindow()
    
    def GetSelectWindow(self):
        """ Return handle/reference to the selection window

	!return - the reference to the selection window
        """
	
        return self.panel.GetSelectWindow()

    def DbUpdate(self, evtReason):
        """Inform other windows that the state of the database changed
        Inherited from dbUpdate interface class

	@evtReason - constant telling the current state of the ConfDB (DB_CONNECTED,DB_DISCONNECTED)
        """
	
        self.OnDbUpdate(evtReason, self.GetCdb())
        #walprob : j'ai pas encore defini tous les bouttons pour les disabler
	self.panel.OnDbUpdate(evtReason, self.GetCdb())

    def OnDbUpdate(self, evtReason, cdb):
        """ React on database state change event
        Inherited from dbUpdate interface class

	@evtReason - constant telling the current state of the ConfDB (DB_CONNECTED or DB_DISCONNECTED)
	@cdb - the ConfDB instance
        """
	
	if evtReason == DB_DISCONNECTED:
		#CREATE
		pass
		'''self.GetMenuBar().Enable(ID_CREATE_DEVICETYPE,False)
		self.GetMenuBar().Enable(ID_CREATE_DEVICE,False)
		self.GetMenuBar().Enable(ID_CREATE_LINKTYPE,False)
		self.GetMenuBar().Enable(ID_CREATE_LINK,False)
		#MODIFY
		self.GetMenuBar().Enable(ID_MODIFY_DEVICETYPE,False)
		self.GetMenuBar().Enable(ID_MODIFY_DEVICE,False)
		self.GetMenuBar().Enable(ID_MODIFY_LINKTYPE,False)
		self.GetMenuBar().Enable(ID_MODIFY_LINK,False)

		#DELETE
		self.GetMenuBar().Enable(ID_DELETE_DEVICETYPE,False)
		self.GetMenuBar().Enable(ID_DELETE_DEVICE,False)
		self.GetMenuBar().Enable(ID_DELETE_LINKTYPE,False)

		#TOOLBAR
		self.GetToolBar().EnableTool(NAVIGATE_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(CREATE_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(COPY_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(PASTE_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(DELETE_TOGGLE_BUTTON,False)
		self.GetToolBar().EnableTool(SAVE_TOGGLE_BUTTON,False)

		self.GetToolBar().EnableTool(ZOOMIN_BUTTON,False)
		self.GetToolBar().EnableTool(ZOOMOUT_BUTTON,False)
		self.GetToolBar().EnableTool(CUSTOMZOOM_BUTTON,False)

		self.GetToolBar().FindControl(DEVTYPE_COMBOBOX).Enable(False)
		self.GetToolBar().FindControl(LINKTYPE_COMBOBOX).Enable(False)
		self.GetToolBar().FindControl(DEVTYPE_BUTTON).Enable(False)
		self.GetToolBar().FindControl(LINKTYPE_BUTTON).Enable(False)

		self.GetToolBar().EnableTool(NEIGHBOUR_BUTTON,False)
		self.GetToolBar().EnableTool(PATH_BUTTON,False)
		self.GetToolBar().EnableTool(DYNAMICLINK_BUTTON,False)
		self.GetToolBar().EnableTool(SUBSYSTEM_BUTTON,False)

		self.GetToolBar().EnableTool(LEVEL_DOWN_BTN,False)
		self.GetToolBar().EnableTool(LEVEL_UP_BTN,False)
		
		self.GetMenuBar().EnableTop(1,False) #edit
		self.GetMenuBar().EnableTop(2,False) #view
		self.GetMenuBar().EnableTop(6,False) #visual
		self.GetMenuBar().EnableTop(7,False) #test'''
		
	else: # Connected
		#CREATE
		'''self.GetMenuBar().Enable(ID_CREATE_DEVICETYPE,True)
		self.GetMenuBar().Enable(ID_CREATE_DEVICE,True)
		self.GetMenuBar().Enable(ID_CREATE_LINKTYPE,True)
		self.GetMenuBar().Enable(ID_CREATE_LINK,True)
		#MODIFY
		self.GetMenuBar().Enable(ID_MODIFY_DEVICETYPE,True)
		self.GetMenuBar().Enable(ID_MODIFY_DEVICE,True)
		self.GetMenuBar().Enable(ID_MODIFY_LINKTYPE,True)
		self.GetMenuBar().Enable(ID_MODIFY_LINK,True)
		#DELETE
		self.GetMenuBar().Enable(ID_DELETE_DEVICETYPE,True)
		self.GetMenuBar().Enable(ID_DELETE_DEVICE,True)
		self.GetMenuBar().Enable(ID_DELETE_LINKTYPE,True)
		
		self.GetToolBar().EnableTool(NEIGHBOUR_BUTTON,True)
		self.GetToolBar().EnableTool(PATH_BUTTON,True)
		self.GetToolBar().EnableTool(DYNAMICLINK_BUTTON,True)
		self.GetToolBar().EnableTool(SUBSYSTEM_BUTTON,True)
		
		self.GetMenuBar().EnableTop(2,True) #Test
		self.GetMenuBar().EnableTop(6,True) #visual
		self.GetMenuBar().EnableTop(7,True) #Test
                '''


