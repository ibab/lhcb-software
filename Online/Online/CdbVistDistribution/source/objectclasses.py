import platform # to detect the OS
import string
import os
import copy

#from wxPython.wx import wxColour # wxColour object for storing device type colour

from cdbVisCore import *

if OS_VERSION == LINUX:
	from libconfDBpython import *
elif OS_VERSION == WINDOWS:
	from confDBpython import *
else:
	from confDBpython import * #linux distro..or linux "compatible" with *.so's

from MyExceptions import *
import Validation
from helper import *
import pdb
import cx_Oracle

# Global variables
configfile = None # Reference to the configuration file for CdbVis
appmain = None # Reference to the running instance of MainWindow, created first time when connecting to ConfDB
cfDB = None # Reference to the ConfDB Library instance that is responsible for keeping the persistent connection to ConfDB
cfDB2 = None # to store the cursor of the connexion established using cx_Oracle module.  // Added by walid 
oracle = None
# Global functions
def GetConfig():
	return configfile #to get config file reference in other object classes
def GetAppWin():
	return appmain	#to get an instance of the MainWindow object, to be able to get the dirty list etc.
	
def Color(string):
	""" this fonction take as argument a string like "(r,g,b)" and return a QColor object"""
	if(isinstance(string,QColor)):
		return string
	string = string.strip('(').strip(')').split(',')
	return QColor(int(string[0]),int(string[1]),int(string[2]))
def ColorToTuple(color):
	return ( color.red(), color.green(), color.blue() )

'''def connectLink(deviceFrom,deviceTo,portFrom,portTo):
	""" connect deviceFrom to deviceTo using the ports numbers : portFrom and portTo.
	if the port does not exist in the confDB they will be created.
	"""
	global cfDB2
	
	# get the deviceFrom and deviceTo IDs.
	cfDB2.execute("SELECT DEVICENAME,DEVICEID FROM LHCB_LG_DEVICES WHERE DEVICENAME='"+deviceFrom+"' OR DEVICENAME='"deviceTo"' ")
	res = cfDB2.fetchall()
	if len(res)<2:
		GetAppWin().ShowError("One of the two devices does not existe in the Configuration Database!",ERR_ERROR)
		return False
	if res[0][0]=deviceFrom:
		deviceIdFrom = res[0][1]
		deviceIdTo = res[1][1]
	else:
		deviceIdFrom = res[1][1]
		deviceIdTo = res[0][1]
	
	#get ports id 
	cfDB2.execute("SELECT DEVICEID,PORTID FROM LHCB_PORT_PROPERTIES WHERE \
	( DEVICEID="+str(deviceIdFrom)+" AND PORT_NBR="+portFrom+" AND PORT_WAY=2) OR (DEVICEID="+str(deviceIdTo)+" AND PORT_NBR="+portTo+" AND PORT_WAY=1) ")
	res = cfDB2.fetchall()
	if len(res)>3:
		GetAppWin().ShowError("Incoherent entries in ConfDB !",ERR_ERROR)
	elif len(res)<2:
		GetAppWin().ShowError("Corresponding port doesnot exist !",ERR_ERROR)
		# TODO :   + validate the ports number by checking the Device_Type informations.
		#	+ Create the corresponding ports if they do not exist
	else:
		cfDB2.execute("INSERT INTO LHCB_MACROSCOPIC_CONNECTIVITY \
		(PORTIDFROM, PORTIDTO, LINKTYPE, BIDIRECTIONAL_LINK_USED, LINK_WEIGHT, LINK_USED, SYSTEM_NAME, LINK_INFO, CREATED, AUTHOR, TERMINAL_NAME, LAST_UPDATE, USER_UPDATE) \
		VALUES ("+deviceIdFrom+", "+deviceIdTo+", "+linkType+", )")
	
'''	
class DBInfo(dbUpdate):
	""" Abstract class with common varibles and functions for all db objects. All db objects inherit from this class.
	    This is actually what you in Java would call an interface. All classes that implement (inherit) this class MUST have
	    all variables and functions this class has.
	"""

	def __init__(self):
		self.__errormessage = "" # Variable to store the last set error message; so that it can be retrieved by calling code
		
	def SetErrorMessage(self,msg):
		""" Set the error message so that it can be looked up.
		@msg - the message string
		"""
		
		self.__errormessage = msg
		
	def GetErrorMessage(self):
		""" Get the error message that was set most recently.

		!return - the error message string
		"""
		
		return self.__errormessage

	def Create(self):
		""" Create a new instance of the object in the database. This actually only sets the save status of the object to CREATE, and adds the
		object to the dirty objects list so that it can be stored in ConfDB later on.
		"""
		pass

	def Modify(self):
		""" Modify a data object; sets actually the save status of the object to MODIFY; and adds an instance of it to the
		dirty objects list so that it can be stored in ConfDB later.
		"""
		pass

	def Delete(self):
		""" Delete a data object; sets actually the save status of the object to DELETE; and adds an instance of it to the
		dirty objects list so that it can be stored in ConfDB later.
		"""
		pass
	
	def Update(self):
		""" This function is to update the object with the most recent information that the user has set to it. It may be stored in ConfDB
		or retrieved from an object in the dirty objects list.
		"""
		pass

	def GetObjectInfo(self):
		""" This function retrieves information/properties from the object as it is stored in the ConfDB or in the dirty objects list, and
		sends a dictionary with all this information back to the caller. This function is almost equal to Update(...), but this function
		may contain the most recent information about the object itself, and hence not look in the dirty objects list or ConfDB for the most
		recent information.
		"""
		pass
	
	def Save(self):
		""" A call to this information actually tries to store changes in the ConfDB, what it does depends on the save and modify status set
		for this object. The change are not necessarily commited immediately.
		"""
		pass

	

class DBSystem:
	""" This is the overall general class, all other classes are a part of this class, or a part of a class that is a part of this class. 
	The classes in this class hierarchy do NOT inherit from each other, but a simply objects in the other classes.

	This class is like a class for all subsystems in the ConfDB, it is responsible to connect and disconnect to and from the ConfDB,
	retrieving subsystem info from the configuration file, setting global variables and retrieving info about available subsystems.
	"""

	def __init__(self,cfgFile,main):
		""" Constructor

		    Parameters:
		    @cfgFile - a reference to the program configuration file
		    @main - a reference to the program instance (MainWindow), for communication purposes
		"""
		
		self.__cfg = cfgFile
		
		global configfile
		configfile = cfgFile	#to make it visible to other object classes in this module

		self.__connected = False
		global appmain
		appmain = main # global reference to MainWindow; accessible from every instance of objects in this module
		self.__main = None
		self.__rootDir = None # Root directory of the configuration file, usually; db/confdb

		self.__subsystems = []	#list of subsystem objects (references), filled later when we connect to ConfDB
		self.__functions=[]
		self.__errormessage = ""

	def __setSettingsFile(self,settingsfile):
		""" Set configuration file.

		@settingsfile - reference to the configuration file
		"""
		self.__cfg = settingsfile

	def __getSettingsFile(self):
		""" Return the reference to the configuration file.

		!return - reference to the configuration file.
		"""
		return self.__cfg

	def SetErrorMessage(self,msg):
		self.__errormessage = msg
		
	def GetErrorMessage(self):
		return self.__errormessage

	def GetSubSystems(self):
		#TODO: Set up try-catch for configuration file reading
		
		if not self.__cfg:
			self.__errormessage = "Configuration file not yet loaded. This file must either exist as settings.cfg in the root folder of the running program, or loaded before you connect to the database by choosing File > Load Settings... from the menu."
			return False

		if not self.__connected:
			self.__errormessage = "The connection to the database was lost. If this problem continues, there is probably a problem with the database or database connection."
			return False

		validsystems = allvalidsystems #retrieved from global variable allvalidsystems in cdbVisCore module
		self.__subsystems = []
		mysystem = ""
		for mysystem in validsystems:
			obj = SubSystem(self.__cfg,mysystem) # Create SubSystem object for each subsystem found
			self.__subsystems.append(obj)    #append systems to the list, which exist in the configuration file


		return self.__subsystems

	def __isConnected(self):
		""" Return whether we are connected to the ConfDB or not.

		!return - True if we are connected, False otherwise
		"""
		
		return self.__connected


	def Connect(self,rootDir):
		""" Connect and set up a persistent connection to the ConfDB. Set the connection object to be available
		for every other object; as a global variable.

		@rootDir - The root directory of where we will start to search in the configuration file (set file cursor)
		"""

		try:
			if self.__connected:
				self.__errormessage = "Cannot connect to the ConfDB. Reason: Already connected!"
				return False

			self.__rootDir = rootDir
														
			# Read user, pwd and database name from the configuration file
			dsn = self.__cfg.get(self.__rootDir,"dsn")
			user = self.__cfg.get(self.__rootDir,"user")
			password = self.__cfg.get(self.__rootDir,"password")		
			# Connect to ConfDB and set up global ConfDB object
			global cfDB
			cfDB = CONFDB(dsn,user,password)
			DBSystem.cfDB = cfDB #Added by walid just for debugging !
			cfDB.DBConnexion()

		        self.__connected = True # Connected!
			
			##return True

		except RuntimeError,err:
		    self.__errormessage = "Failed to connect to ConfDB using confDBpython. Reason: \n" + str(err)
		    return False #on error
		
		# Connect using cx_Oracle module  // Added by walid because there are some functions missing in the confDBpython library.
		try:
			self.__rootDir = rootDir
			# Read user, pwd and database name from the configuration file
			dsn = self.__cfg.get(self.__rootDir,"dsn")
			user = self.__cfg.get(self.__rootDir,"user")
			password = self.__cfg.get(self.__rootDir,"password")		
			# Connect to ConfDB and set up global ConfDB object
			
			global oracle
			oracle = cx_Oracle.connect(user+'/'+password+'@'+dsn)
			global cfDB2
			cfDB2 = oracle.cursor()
			DBSystem.cfDB2 = cfDB2 #Added by walid just for debugging !
			return True

		except RuntimeError,err:
		    self.__errormessage = "Failed to connect to ConfDB using cx_Oracle module. Reason: \n" + str(err)
		    return False #on error

	def Disconnect(self):
		""" Disconnect from the ConfDB. Destroy the persistent connection.
		"""
		
		try:
			if not self.__connected:
			    self.__errormessage = "Error disconnecting from ConfDB. Reason: Not connected"
			    return False
																     
			cfDB.DBDeconnexion()
			self.__connected = False
			self.__rootDir = None
			self.__cfg = None
			self.__main = None
			
			global oracle
			oracle.commit()

			# Get error that cfDB is not declared yet if we set this. Strange.
			#global cfDB
			#cfDB = None
		
			return True

		except RuntimeError,err:
		    self.__errormessage = "Error disconnecting from ConfDB. Reason: " + str(err)
		    return False #on error

	#added by Lana

	def GetAllDeviceFunctions(self):
		""" Get all the available funcitons

		!return - a list of device functions available if successful, an empty list if
		none found, and False if an error occurs.
		"""

		#TODO: Also look in dirty objects list.
		try:
			
			my_functions= cfDB.GetAvailableFunctions()# Get list of functions from the confdb
			my_functions.append("none")
			return my_functions

		except ValidationError,err:
        	    	self.__errormessage = str(err)
        		return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "No devices found in the selected subsystem " + str(self.__name)
			return [] #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error  
 
	def GetAllSpareHWTypes(self):
		""" Get all the available funcitons

		!return - a list of device functions available if successful, an empty list if
		none found, and False if an error occurs.
		"""

		#TODO: Also look in dirty objects list.
		try:

			my_functions= cfDB.GetSpareHWTypeList()# Get list of functions from the confdb
			return my_functions

		except ValidationError,err:
        	    	self.__errormessage = str(err)
        		return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "No devices found in the selected subsystem " + str(self.__name)
			return [] #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error  

	def GetDevicesByLocation(self,location_string):
		""" General get a device at a given location.
		    TODO: should also be moved to SubSystem class.
		    NB! This function does not look for devices that are not yet stored in the ConfDB.

		    @location_string - the location string which looks for devices with the same location
		    string set.
		"""

		try:

		    res = cfDB.GetDeviceNamesPerLocation(location_string)
		    devices = res

		    i = 0 #reset
		    my_devices = []
		    while i < len(devices):
			my_devices.append(devices[i])
			i += 1

		    return my_devices

		except RuntimeError,err:
		        if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
					self.__errormessage = "No devices found at the given location."
					return [] #none found
			else:
				self.__errormessage = str(err)
		    	return False #on error

	

#end added

class SubSystem(DBInfo):
	""" Class for properties and methods for individual subsystems such as Muon and TFC. Valid subsystems are given in
	CdbVisCore module in the allvalidsystems global variable.
	"""

	def __init__(self,configFile,subsystemname=""):
		""" Constructor.

		Parameters:
		@configFile - instance of the configurations file that we have loaded
		@subsystemname - name of the subsystem this instance will contain information about and act on
		"""
		
		self.__name = subsystemname	# The subsystem name is unique and identifies the subsystem we work on/in
		self.__cfg = configFile
		self.__deviceTypes = []
		self.__devices = []
		self.__linkTypes = []

		#dictionaries of linkids/names linkid : linkname, and linkname: linkid
		self.__linkTypeTable = {}
		self.__deviceTable = {}
		
		self.__created = False
		self.__description = ""

		# Obsolete information retrieved from configuration file; where the subsystems were positioned in the visual window.
		# May be useful to use.
		self.__xPos = 0
		self.__yPos = 0
		self.__width = 0
		self.__height = 0
		self.__colour = None
		self.__rotate = 0

		self.__errormessage = ""

	### GET ###
	def GetLinkTypeTable(self):
		return self.__linkTypeTable
	def GetDeviceTable(self):
		return self.__deviceTable

	def GetName(self):
		return self.__name
	def GetDescription(self):
		return self.__description
	def GetXPos(self):
		return float(self.__xPos)
	def GetYPos(self):
		return float(self.__yPos)
	def GetWidth(self):
		return float(self.__width)
	def GetHeight(self):
		return float(self.__height)
	def GetColour(self):
		return self.__colour
	def GetRotate(self):
		return float(self.__rotate)

	def GetErrorMessage(self):
		return self.__errormessage

	### SET ###
	def SetName(self,name):
		self.__name = name
	def SetColour(self,colour):
		self.__colour = colour
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	
	def GetObjectInfo(self):
		""" Get all information needed about a subsystem. The information is stored in the configuration file.
		"""
		return []	
	
	def IsCreated(self):
		""" We can disable a subsystem (not visible to the user) by setting created=False in the
		configuration file. This function checks whether this variable is set to True or False.

		!return - True if the subsystem is created and available, False otherwise.
		"""

		# TODO: exception handling
	        path = "/db/devdb/data/systems/" + str(self.__name)
        	if self.__cfg.Exists(path):
		    self.__cfg.SetPath(path)
		    if self.__cfg.Read("created") == "True":
			return True
		    else:
			return False


	def GetAllDevices(self):
		""" Get all devices in the given subsystem.

		!return - a list of devices available in a subsystem if successful, an empty list if
		none found, and False if an error occurs.
		"""

		#TODO: Also look in dirty objects list.
		try:
			Validation.ValidateSystem("Active subsystem",self.__name)

			my_devices = []
			tmp = cfDB.MatchDeviceIDDeviceName(self.__name) # Get both IDs and Names from the ConfDB of devices in the given subsystem
			result = tmp
		
			i=0
		        self.__deviceTable = {}
			while i < len(result):
				tmp_device = result[i].split("|")
				my_devices.append(tmp_device[1])

				# Set up a mapping dictionary: device name - device id and vice versa
				# TODO: IS this necessary?
		    		self.__deviceTable[int(tmp_device[0])] = tmp_device[1]
			    	self.__deviceTable[tmp_device[1]] = int(tmp_device[0])
				i += 1

			return my_devices

		except ValidationError,err:
        	    	self.__errormessage = str(err)
        		return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "No devices found in the selected subsystem " + str(self.__name)
			return [] #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error                                                                                                                             
    	def GetDeviceTypes(self,onlyfromdb=False,onlyfromdl=False):
        	""" Return list of device types found in the given subsystem.

		Parameters:
		@onlyfromdb - If true we only look for device types in the ConfDB
		@onlyfromdl - If true we only look for device types in the Dirty Objects List

		Only one of the parameters can be true, both can be false.

		!return - list of device types (string names) if successful, an empty list if none found
		and False if an error occurs,
        	"""
                                                                                                                             
        	try:
            		Validation.ValidateSystem("Active System",self.__name)

			if onlyfromdb and onlyfromdl: #it is like saying, do not look anywhere
				return []

			my_devicetypes = []
			dirty_objs_devtypes = []
			tbd = [] # = to be deleted
			if not onlyfromdb: # If not only from db, we have a look in the dirty objects list if creation mode
				if GetAppWin().GetUserMode() == ID_CREATION_MODE:
					tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
					for obj in tmp_dirtyobjs:
						if string.find(obj.GetSystem(),self.__name,0) != -1: # if data object is available in this system
							if obj.__class__ == DeviceType: # IF device type
								if obj.GetSaveStatus() != DELETE: # if it is not set for deletion
									if obj.GetName() not in my_devicetypes: # and not already added to our list
										my_devicetypes.append(obj.GetName()) # Add it!
										dirty_objs_devtypes.append(obj.GetName())
								elif obj.GetSaveStatus() == DELETE: # if set for deletion
									if obj.GetName() in my_devicetypes: # if we already added an object of this device
										#type to the list; remove it.
										my_devicetypes.remove(obj.GetName())
										dirty_objs_devtypes.remove(obj.GetName())

									tbd.append(obj.GetName()) # Object set for deletion; later if we find an object in
									#ConfDB with the same name, do not add it since it is set for deletion in dirty
									#objects list

			result = cfDB.LoadDeviceTypeTable(self.__name) # Get device types in subsystem from ConfDB
			
			i = 0
			tmp_devtypes = []
			while i < len(result):
				tmp_devtypes.append(result[i].split("|")[1]) # device types given as ID|NAME
				i += 1

			if not onlyfromdb and not onlyfromdl and GetAppWin().GetUserMode() == ID_CREATION_MODE:
				for obj in tmp_devtypes:
					# Do not add to list if:
					# a. already added
					# b. set for deletion in dirty objects list
					# c. the device type has changed name and already added to the list
					if obj not in dirty_objs_devtypes and obj not in tbd and obj not in GetAppWin().GetOldNames():
						my_devicetypes.append(obj)
			elif not onlyfromdl:
				for obj in tmp_devtypes:
					# Only add to list if device type has not changed name
					if obj not in GetAppWin().GetOldNames():
						my_devicetypes.append(obj)

			self.__deviceTypes = my_devicetypes

			return self.__deviceTypes

		except ValidationError,err:
        	    	self.__errormessage = str(err)
        		return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1: # No device types found in ConfDB; but we may have found some in dirty obj list
			if my_devicetypes != []:
				self.__deviceTypes = my_devicetypes
				return self.__deviceTypes
			else:
				self.__errormessage = "No devices types found in the selected subsystem " + str(self.__name)
				return [] #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error

	def GetLinkTypes(self,onlyfromdb=False,onlyfromdl=False):
		""" Return list of link types found in the given subsystem. (Obsolete: actually we look for link types in the LHCb system
		because there are not that many different link types).
		
		Parameters:
		@onlyfromdb - If true we only look for link types in the ConfDB
		@onlyfromdl - If true we only look for link types in the Dirty Objects List

		Only one of the parameters can be true, both can be false.

		!return - list of link types (string names) if successful, an empty list if none found
		and False if an error occurs,		
		"""

		
		try:
			Validation.ValidateSystem("Active subsystem:",self.__name)

			if onlyfromdb and onlyfromdl: #it is like saying, do not look anywhere
				return []
		
			
			# Same as for GetDeviceTypes, look there for explanation.
			my_linktypes = []
			dirty_objs_linktypes = []
			tbd = [] #to be deleted
			if not onlyfromdb:
				if GetAppWin().GetUserMode() == ID_CREATION_MODE:
					tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
					for obj in tmp_dirtyobjs:
						if string.find(obj.GetSystem(),self.__name,0) != -1:
							if obj.__class__ == LinkType:
								if obj.GetSaveStatus() != DELETE:
									if obj.GetName() not in my_linktypes:
										my_linktypes.append(obj.GetName())
										dirty_objs_linktypes.append(obj.GetName())
								elif obj.GetSaveStatus() == DELETE:
									if obj.GetName() in my_linktypes:
										my_linktypes.remove(obj.GetName())
										dirty_objs_linktypes.remove(obj.GetName())

									tbd.append(obj.GetName())

			tmp_linktypes = []
			result = cfDB.LoadLinkTypeTable() #
			i = 0
			self.__linkTypeTable = {} #reset, so that we do not show deleted linktypes
			while i < len(result):
				tmp_link = result[i].split("|") #because we get name + nr #we will use names for the user, and use nrs to save

				# Set up link type name and nr mapping..necessary? TODO.
				self.__linkTypeTable[int(tmp_link[0])] = tmp_link[1]
				self.__linkTypeTable[tmp_link[1]] = int(tmp_link[0])
				tmp_linktypes.append(tmp_link[1])
				i += 1
			
			GetAppWin().SetLinkTypeTable(self.__linkTypeTable) #lookup in this table on main class
		
			# For explanation; look in GetDeviceTypes above
			if not onlyfromdb and not onlyfromdl and GetAppWin().GetUserMode() == ID_CREATION_MODE:
				for obj in tmp_linktypes:
					if obj not in dirty_objs_linktypes and obj not in tbd and obj not in GetAppWin().GetOldNames():
						my_linktypes.append(obj)
			elif not onlyfromdl:
				for obj in tmp_linktypes:
					if obj not in GetAppWin().GetOldNames():
						my_linktypes.append(obj)
	 
			self.__linkTypes = my_linktypes

			return self.__linkTypes

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1: # None found in ConfDB; but we may have something in the dirty objects list
			if my_linktypes != []:
				self.__linkTypes = my_linktypes
				return self.__linkTypes
			else:
				self.__errormessage = "No  link types found in the selected subsystem " + str(self.__name)
				return [] #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error

	def Modify(self):
		"""Modify an already created sub system
		"""

		return self.Create() # We will never modify systems through the GUI (can be done manually)

	def Create(self):
        	""" Kind of obsolete.

		Sets some settings in the configuration database for a newly created subsystem in CdbVis. But this feature
		is no longer accessible for the user. Should only be done manually through editing the configuration file
		in a text editor
	        """

		#TODO: exception handling for IO configuration file..if we decide to use this function
		
                try:
			Validation.ValidateSystem("Subsystem name",self.__name)
                                                                                                                             
			#We continue to write to config file (should also be errorchecking here)
	        	self.__cfg.SetPath("/db/devdb/data/systems/" + self.__name)
			self.__cfg.Write("created","True")

			my_colour = str(self.__colour.Red()) + "," + str(self.__colour.Green()) + "," + str(self.__colour.Blue())
			self.__cfg.Write("colour",my_colour)

	        	return True

	        except ValidationError,err:
        		self.__errormessage = str(err)
		        return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
	
class DeviceType(DBInfo):
	""" There are several device types, and each device is of only one device type.
	A device type contains information that is common for devices of the same type.
	It is not a physical object itself.
	"""

	def __init__(self,systemname,deviceType="",new=True):
		""" Constructor.

		Parameters:
		@systemname - name of the subsystem(s) the device type is available in
		@deviceType - device type name set by the user; which identifies a device type
		@new 	    - if True, this device type is newly created, not stored in ConfDB yet
		if False, this is a device type that is already created, and hence we can retrieve
		information for the object from ConfDB or dirty objects list. That is done by calling
		the Update(...) function in the constructor.
		"""
                self.__errormessage = ""
		self._deviceTypeInfo = {}	#all device type info that is common for all device types in this dictionary
		self._deviceTypeInfo[DEV_PORTS_IN] = 0
		self._deviceTypeInfo[DEV_PORTS_OUT] = 0
		self._deviceTypeInfo[DEV_TYPE_DESCRIPTION] = ""
		self._deviceTypeInfo[DEV_SYSTEM] = systemname
		self._deviceTypeInfo[DEV_TYPE] = deviceType
		self._deviceTypeInfo[DEV_TYPEID] = "" #id in ConfDB (auto number from sequence)

		self._deviceTypeInfo[DEV_COLOUR] = None # wxColour object
		self._deviceTypeInfo[OLD_NAME] = "" # If we renamed the object we have the previous name stored here

		self.__devices = []	#a list of devices of this device type (if retrieved)

		self._deviceTypeInfo[SAVE_STATUS] = NO_STATUS #NO_STATUS : not in dirty objectslist, CREATE - insert
							       #MODIFY - update, DELETE - delete, RENAME - rename

		self.__cfg = GetConfig() # Get the configuration file from global method (May be not needed)

		if new == False:
			self.Update() #Force an update, retrieve info from db or dl

	###### GET ######
	def GetOldName(self):
		return self._deviceTypeInfo[OLD_NAME]
	def GetDeviceTypeID(self):
		return self._deviceTypeInfo[DEV_TYPEID]
	def GetDeviceTable(self):
		return self.__deviceTable
	def GetSaveStatus(self):
		return self._deviceTypeInfo[SAVE_STATUS]
	def GetName(self):
		return self._deviceTypeInfo[DEV_TYPE]
	def GetPortsIn(self):
		return self._deviceTypeInfo[DEV_PORTS_IN]
	def GetPortsOut(self):
		return self._deviceTypeInfo[DEV_PORTS_OUT]
	def GetDescription(self):
		return self._deviceTypeInfo[DEV_TYPE_DESCRIPTION]
	def GetSystem(self):
		return self._deviceTypeInfo[DEV_SYSTEM]
	def GetColour(self):
		return self._deviceTypeInfo[DEV_COLOUR]
	def GetErrorMessage(self):
		return self.__errormessage

	###### SET ######
	def SetOldName(self,oldname):
		self._deviceTypeInfo[OLD_NAME] = oldname
	def SetDeviceTypeID(self,id):
		Validation.ValidateNumber("DeviceTypeID",id,0)
		self._deviceTypeInfo[DEV_TYPEID] = int(id)
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	def SetName(self,name):
		Validation.ValidateString("devicetype name",name,100,False)
		self._deviceTypeInfo[DEV_TYPE] = name
	def SetPortsIn(self,nbr):
		Validation.ValidateNumber("nbr ports in",nbr,0,10000)
		self._deviceTypeInfo[DEV_PORTS_IN] = nbr
	def SetPortsOut(self,nbr):
		Validation.ValidateNumber("nbr ports out",nbr,0,10000)
		self._deviceTypeInfo[DEV_PORTS_OUT] = nbr
	def SetDescription(self,descr):
		Validation.ValidateString("device type description",descr,500,True)
		self._deviceTypeInfo[DEV_TYPE_DESCRIPTION] = descr
	def SetColour(self,colour): 
		#will always be a valid value/wxcolour object as the user cannot set it directly
		self._deviceTypeInfo[DEV_COLOUR] = colour
	def SetSystem(self,name):
		Validation.ValidateSystem("system name",name)
		self._deviceTypeInfo[DEV_SYSTEM] = name
	def SetDict(self,newdict): 
		# Copy all the contents of the dictionary: just like a object copy except some variables.
		# This is implemented because the deepcopy of objects itself caused segmentation faults.
		#self._deviceTypeInfo = copy.deepcopy(newdict)
		for entry in newdict:
			self._deviceTypeInfo[entry] = newdict[entry]
	def SetSaveStatus(self,status):
		self._deviceTypeInfo[SAVE_STATUS] = status
	def SetDictInfo(self,dict):
		# Almost the same as SetDict(...) but this is more safe. As actually use of deepcopy
		# sometimes causes segmentation faults, even the one above.
		for item in dict:
			self._deviceTypeInfo[item] = dict[item]

    	def GetObjectInfo(self,doNotLookUp=True):
        	""" Get all info about a device type in a subsystem, when given the name of a device type in the constructor.
		Here we can set up to check different statuses (TODO), f.ex. if it is the most recent version of an object;
		then just return the contents; else look in dl and db.

		@doNotLookUp - whether we should look for more recent information in dirty objects list or ConfDB.

		!return - a dictionary with all the parameters set.
		"""

		if doNotLookUp == True:
			return self._deviceTypeInfo
		else:
			return self.Update()

	def Update(self,doNotCheckInDirtyList=False):
		""" Check first in dirty objects list for most recent version of this object; if in creation mode.
		If in navigation mode or alredy looked in dirty objects list and not found; check in ConfDB.

		@doNotCheckInDirtyList - if True, we skip the check in the dirty list, else we check. But only in
		creation mode

		!return - a dictionary with all the parameters set.
		"""

		if not doNotCheckInDirtyList:
			# Dirty Objects List
			if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Creation Mode
				# We look for dirty object of same name (unique within type) and type (device type)
				tmp_obj = GetAppWin().GetDirtyObject(self._deviceTypeInfo[DEV_TYPE],DeviceType)
				if tmp_obj != False: # Found an object of same type
					self._deviceTypeInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self._deviceTypeInfo # return what we found after we set it to this object
		
	        try:
		    # ConfDB
		    Validation.ValidateString("DeviceType name",self._deviceTypeInfo[DEV_TYPE],100)
 
        	    result = cfDB.GetDeviceTypeRow(self._deviceTypeInfo[DEV_TYPE]) # Get device type info from ConfDB

		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string to a list; each entry is a parameter name and value

		    i = 0
		    deviceTypeInfo = {DEV_OBJECT : obj_NODE}	#To identify this as a devicenode (as opposite to a linknode)
		    deviceTypeInfo[DEV_SYSTEM] = self._deviceTypeInfo[DEV_SYSTEM] # Not stored in ConfDB
		    for i in range(len(infolist)-1):
			tmp = infolist[i].split(":")	#split each property, name on left site, value on right side
			tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
			datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
			if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
			elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()

			deviceTypeInfo[tmp0] = tmp[1]
			i += 1

		    #convert rgbcolour to wxColour object
		    if deviceTypeInfo[DEV_COLOUR] in ["none","None","","(0,0,0)"]:
			    deviceTypeInfo[DEV_COLOUR] = GetAppWin().GetTempDeviceTypeColour(deviceTypeInfo[DEV_TYPE])
		    else:
			    rgbcolour = deviceTypeInfo[DEV_COLOUR].split(",")
			    deviceTypeInfo[DEV_COLOUR] = "( " + rgbcolour[0] + ", " + rgbcolour[1] + ", " + rgbcolour[2] + " )"  #wxColour(red=int(rgbcolour[0]),green=int(rgbcolour[1]),blue=int(rgbcolour[2]))
		
		
		    self._deviceTypeInfo = deviceTypeInfo

		    return self._deviceTypeInfo                                                                                                         
                                                                                                                             
	        except ValidationError,err:
        		self.__errormessage = str(err)
	        	return False
		except ValueError,err:
			self.__errormessage = "Could not convert data, found wrong datatype: " + str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
			if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
				self.__errormessage = "No information found in the database on the given devicetype: " + str(self._deviceTypeInfo[DEV_TYPE])
				return {}
			else:
			    	self.__errormessage = str(err)

		        return False #on error

	def GetMuonDevicesByLocation(self,quadrant,chamber_x,chamber_y,fee_x="",fee_y=""):
		""" _MUON Specific_, may be changed to fit for other systems later.
		TODO: Move this to SubSystem class
		
		Given a location of a _chamber or an FEE board_, return object at given
		location with all parameters set.

		Parameters:
		@quadrant  - quadrant number, 1-4, where the device is located
		@chamber_x - the x position of the chamber or FE board
		@chamber_y - the y position of the chamber or FE board
		@fee_x     - the fee X position of a fee board on a chamber
		@fee_y     - the fee Y position of a fee board on a chamber
		"""

		#Location syntax: QiMjRkCxCyFExFEy
		location_string = "Q" + str(quadrant) + self.GetName() + "C" + str(chamber_x) + "C" + str(chamber_y)
		if fee_x != "" and fee_y != "":
			location_string += "FE" + str(fee_x) + "FE" + str(fee_y)
		#location_string = location_string.lower() #perhaps, perhaps not?

		return self.GetDevicesByLocation(location_string) #All error handling handled in GetDevicesByLocation

	def GetDevicesByLocation(self,location_string):
		""" General get a device at a given location.
		    TODO: should also be moved to SubSystem class.
		    NB! This function does not look for devices that are not yet stored in the ConfDB.

		    @location_string - the location string which looks for devices with the same location
		    string set.
		"""

		try:

		    res = cfDB.GetDeviceNamesPerLocation(location_string)
		    devices = res

		    i = 0 #reset
		    my_devices = []
		    while i < len(devices):
			my_devices.append(devices[i])
			i += 1

		    return my_devices

		except RuntimeError,err:
		        if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
					self.__errormessage = "No devices found at the given location."
					return [] #none found
			else:
				self.__errormessage = str(err)
		    	return False #on error
	def GetDevices(self):
		""" Get all _device_names_ for a given device type; each device name is unique. Looks both in 
		dirty objects list and ConfDB.
		!Return - list of device names; both in db and in dirtylist
        """

		devices_in_dirtylist = []
		my_devices = []
		dirty_objs_devs = []
		tbd = [] #to be deleted
		#tbd_objs = -[]
		if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Only if creation mode
			tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
			for obj in tmp_dirtyobjs:
				# If type is: DEVICE
				if obj.__class__ == Device:
					# If not set to be deleted, and that the device type is correct (this type)
					if obj.GetSaveStatus() != DELETE and obj.GetType() == self._deviceTypeInfo[DEV_TYPE]:
						if obj.GetName() not in my_devices: # If not already added; ADD!
							my_devices.append(obj.GetName())
							dirty_objs_devs.append(obj.GetName())
					#if set to be deleted and correct type
					elif obj.GetSaveStatus() == DELETE and obj.GetType() == self._deviceTypeInfo[DEV_TYPE]:
						# if already added; remove
						if obj.GetName() in my_devices:
							my_devices.remove(obj.GetName())
							dirty_objs_devs.remove(obj.GetName())
						# set to the tbd list, in case we find same device in ConfDB, but set for
						# delete in dirty objects list
						tbd.append(obj.GetName())

		#First check if this device type has been renamed, cause the name in DB will be the prev one:
		if self.GetName() in GetAppWin().GetNewNames():
			old_name = GetAppWin().GetOldNamesOfRenamed()[self.GetName()]
		else:
			old_name = self._deviceTypeInfo[DEV_TYPE]
			
		try:
        	    Validation.ValidateSystem("System Name",self._deviceTypeInfo[DEV_SYSTEM])
				          
	            tmp = cfDB.GetDeviceNamesPerType(old_name)
			
		    devices = tmp														     
		    i = 0 #reset
		    if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Need to do some checks if creation mode
			while i < len(devices):
				dev_tmp = devices[i].split("|")
				# do add to list if:
				# a. not already added
				# b. not set for deletion
				# c. not already added under a new name (renamed)
				if (dev_tmp[0] not in dirty_objs_devs) and (dev_tmp[0] not in tbd) and dev_tmp[0] not in GetAppWin().GetOldNames():
					my_devices.append(str(dev_tmp[0]))
				i += 1


		    else: #navigation mode
			while i < len(devices):
					dev_tmp = devices[i].split("|")
					if  dev_tmp[0] not in GetAppWin().GetOldNames(): #TODO: this is wrong? do not need 
											#to do this in navigation mode?
						my_devices.append(str(dev_tmp[0]))
						i += 1

	            self.__devices = my_devices
				
		    return self.__devices

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
		        if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
				if len(my_devices) == 0:
					self.__errormessage = "No devices found for the given device type " + str(self._deviceTypeInfo[DEV_TYPE])
					return [] #none found
				else: #none found in db, but we found in dirty list
					self.__devices = my_devices
					return self.__devices
			else:
				self.__errormessage = str(err)
		    	return False #on error
	def GetSpareDevices(self):  #should be called only for spare devices
        	""" Get all _sparedevice_names_ for a given device type; each device name is unique. Looks both in 
		dirty objects list and ConfDB.
		
	        !Return - list of device names; both in db and in dirtylist
        	"""

		devices_in_dirtylist = []
		my_devices = []
		dirty_objs_devs = []
		tbd = [] #to be deleted
		#tbd_objs = -[]
		if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Only if creation mode
			tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
			for obj in tmp_dirtyobjs:
				# If type is: DEVICE
				if obj.__class__ == Device:
					# If not set to be deleted, and that the device type is correct (this type)
					if obj.GetSaveStatus() != DELETE and obj.GetType() == self._deviceTypeInfo[DEV_TYPE]:
						if obj.GetName() not in my_devices: # If not already added; ADD!
							my_devices.append(obj.GetName())
							dirty_objs_devs.append(obj.GetName())
					#if set to be deleted and correct type
					elif obj.GetSaveStatus() == DELETE and obj.GetType() == self._deviceTypeInfo[DEV_TYPE]:
						# if already added; remove
						if obj.GetName() in my_devices:
							my_devices.remove(obj.GetName())
							dirty_objs_devs.remove(obj.GetName())
						# set to the tbd list, in case we find same device in ConfDB, but set for
						# delete in dirty objects list
						tbd.append(obj.GetName())

		#First check if this device type has been renamed, cause the name in DB will be the prev one:
		if self.GetName() in GetAppWin().GetNewNames():
			old_name = GetAppWin().GetOldNamesOfRenamed()[self.GetName()]
		else:
			old_name = self._deviceTypeInfo[DEV_TYPE]
			
		try:
        	    Validation.ValidateSystem("System Name",self._deviceTypeInfo[DEV_SYSTEM])
				          
	            tmp = cfDB.GetSpareHWPerType(old_name)
			
		    devices = tmp														     
		    i = 0 #reset
		    if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Need to do some checks if creation mode
			while i < len(devices):
				dev_tmp = devices[i].split("|")
				# do add to list if:
				# a. not already added
				# b. not set for deletion
				# c. not already added under a new name (renamed)
				if (dev_tmp[0] not in dirty_objs_devs) and (dev_tmp[0] not in tbd) and dev_tmp[0] not in GetAppWin().GetOldNames():
					my_devices.append(str(dev_tmp[0]))
				i += 1


		    else: #navigation mode
			while i < len(devices):
					dev_tmp = devices[i].split("|")
					if  dev_tmp[0] not in GetAppWin().GetOldNames(): #TODO: this is wrong? do not need 
											#to do this in navigation mode?
						my_devices.append(str(dev_tmp[0]))
						i += 1

	            self.__devices = my_devices
				
		    return self.__devices

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
		        if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
				if len(my_devices) == 0:
					self.__errormessage = "No devices found for the given device type " + str(self._deviceTypeInfo[DEV_TYPE])
					return [] #none found
				else: #none found in db, but we found in dirty list
					self.__devices = my_devices
					return self.__devices
			else:
				self.__errormessage = str(err)
		    	return False #on error


	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""
		
		save_status = self._deviceTypeInfo[SAVE_STATUS]
		my_colour = str(self._deviceTypeInfo[DEV_COLOUR][0]) + "," + str(self._deviceTypeInfo[DEV_COLOUR][1]) + "," + str(self._deviceTypeInfo[DEV_COLOUR][2])
		if save_status == CREATE:
			if insertmany:
				return 'cfDB.InsertMultipleDeviceType("%s","%s",%i,%i,"%s","%s",%i,%i)' \
					%(str(self._deviceTypeInfo[DEV_SYSTEM]),str(self._deviceTypeInfo[DEV_TYPE]),self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],str(self._deviceTypeInfo[DEV_TYPE_DESCRIPTION]),str(my_colour),first,commit)
					
			else:
				print '1 : "%s"'%str(self._deviceTypeInfo[DEV_SYSTEM])
				print '2 : "%s"'%str(self._deviceTypeInfo[DEV_TYPE])
				print '3 : %i'%self._deviceTypeInfo[DEV_PORTS_IN]
				print '4 : %i'%self._deviceTypeInfo[DEV_PORTS_OUT]
				print '5 : "%s"'%str(self._deviceTypeInfo[DEV_TYPE_DESCRIPTION])
				print '6 : "%s"'%str(my_colour)
				print '7 : %i'%first
				print '8 : %i'%commit
				#print 'cfDB.InsertDeviceType("%s","%s",%i,%i,"%s","%s",%i)'%(str(self._deviceTypeInfo[DEV_SYSTEM]),str(self._deviceTypeInfo[DEV_TYPE]),self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],str(self._deviceTypeInfo[DEV_TYPE_DESCRIPTION]),str(my_colour),first,commit)
				return 'cfDB.InsertDeviceType("%s","%s",%i,%i,"%s","%s",%i)' \
					%(str(self._deviceTypeInfo[DEV_SYSTEM]),str(self._deviceTypeInfo[DEV_TYPE]),self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],str(self._deviceTypeInfo[DEV_TYPE_DESCRIPTION]),str(my_colour),first) #,commit)

		elif save_status == MODIFY:
			#modify_status = self._deviceTypeInfo[MODIFY_STATUS]
			return 'cfDB.UpdateMultipleDeviceTypeAttributes("%s","%s",%i,%i,"%s",%i,%i)' \
				%(str(self._deviceTypeInfo[DEV_TYPE]),str(self._deviceTypeInfo[DEV_TYPE_DESCRIPTION]),self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],str(my_colour),first,commit)
	
		elif save_status == DELETE:
                    return 'cfDB.DeleteFunctionalDeviceType("%s")'\
                           %s(str(self._deviceTypeInfo[DEV_TYPE]))
		
		elif save_status == RENAME:
				return 'cfDB.UpdateMultipleDeviceTypes("%s","%s",%i,%i)' \
				%(str(self._deviceTypeInfo[OLD_NAME]),str(self._deviceTypeInfo[DEV_TYPE]),first,commit)

		else: #invalid status
			return ""

	def Save(self,first=False,insertmany=False,commit=True):
		""" Save the changes made on this object in the ConfDB;
		deletion, modification/update or insertion of the object in the database.
		The methods Delete, Modify and Create only set/change the variables in this object, 
		this method makes things happen in the db (committed)

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""	

		try:
			if self._deviceTypeInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE,RENAME]:
				raise RuntimeError,"Wrong save status set, can not do any database action on: " + str(self.GetName())


			if self._deviceTypeInfo[SAVE_STATUS] == DELETE:
				Validation.ValidateString("Device Type name",self._deviceTypeInfo[DEV_TYPE],100)
				
			if self._deviceTypeInfo[SAVE_STATUS] == CREATE or self._deviceTypeInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateSystem("Saving devicetype, system name",self._deviceTypeInfo[DEV_SYSTEM])
				Validation.ValidateString("Device Type name",self._deviceTypeInfo[DEV_TYPE],100)
				Validation.ValidateNumber("Nbr ports in",self._deviceTypeInfo[DEV_PORTS_IN],0,10000)
				Validation.ValidateNumber("Nbr ports out",self._deviceTypeInfo[DEV_PORTS_OUT],0,10000)
				Validation.ValidateString("Device Type Description",self._deviceTypeInfo[DEV_TYPE_DESCRIPTION],500,True)
				my_colour = str(self._deviceTypeInfo[DEV_COLOUR][0]) + "," + str(self._deviceTypeInfo[DEV_COLOUR][1]) + "," + str(self._deviceTypeInfo[DEV_COLOUR][2])
			if self._deviceTypeInfo[SAVE_STATUS] == RENAME:
				Validation.ValidateString("Device Type name",self._deviceTypeInfo[DEV_TYPE],100)

			if self._deviceTypeInfo[SAVE_STATUS] == CREATE:
				if insertmany:
					success = cfDB.InsertMultipleDeviceType(self._deviceTypeInfo[DEV_SYSTEM],self._deviceTypeInfo[DEV_TYPE],self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],self._deviceTypeInfo[DEV_TYPE_DESCRIPTION],my_colour,first,commit)
				else:
					success = cfDB.InsertDeviceType(self._deviceTypeInfo[DEV_SYSTEM],self._deviceTypeInfo[DEV_TYPE],self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],self._deviceTypeInfo[DEV_TYPE_DESCRIPTION],my_colour,commit)

				# If an error occurs, an exception will be thrown.
				if commit: #has to be commited before added to db
					self.Update(True) #retrieve new data from database, also new devicetypeid

			elif self._deviceTypeInfo[SAVE_STATUS] == MODIFY:
				success = cfDB.UpdateMultipleDeviceTypeAttributes(self._deviceTypeInfo[DEV_TYPE],self._deviceTypeInfo[DEV_TYPE_DESCRIPTION],self._deviceTypeInfo[DEV_PORTS_IN],self._deviceTypeInfo[DEV_PORTS_OUT],my_colour,first,commit)
				if commit: #has to be commited before added to db
					self.Update(True) #retrieve new data from database, also new devicetypeid
				
			elif self._deviceTypeInfo[SAVE_STATUS] == DELETE:
				success=cfDB.DeleteFunctionalDeviceType(self._deviceTypeInfo[DEV_TYPE])
				
			elif self._deviceTypeInfo[SAVE_STATUS] == RENAME:
				success = cfDB.UpdateMultipleDeviceTypes(self._deviceTypeInfo[OLD_NAME],self._deviceTypeInfo[DEV_TYPE],first,commit)
				# No update needed, but TODO: update names in visual window on devices affected.

			return True

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err: #error on insertion
			self.__errormessage = str(err)
			return False

	def Delete(self):
		""" Delete this object. This object is put in the dirty list and set for deletion. 
		Sets the save status to deletion.
		"""
		
		self._deviceTypeInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self) #possible to send an instance of itself?

	def Modify(self):
		""" Modifies a device type with new values set by the user through the set functions. The modify status
		is set through SetModifyStatus(...) if needed.
		Sets the save status to modify, and adds the object to the dirty objects list.
		"""
		self._deviceTypeInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
		""" Create a device type. Set save status to creation, and adds the object to the
		dirty objects list.
		"""
		
		self._deviceTypeInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)
	
class Device(DBInfo):
	"""Class for storing information about a device. A device is a physical device in the LHCb
	detector that is usually connected to other devices with links.
	"""

	def __init__(self,systemname,deviceName="",new=True):
		""" Constructor.

		Parameters:
		@systemname - the subsystem(s) the device will be a part of (stored in)
		@devicename - the unique name of the device, given by the user
		@new        - whether the device is about to be created, or is already created.
		True if it is being created, False if not. If it is already created, we will
		fill the object with goods. (names and values)
		"""
		
		self.__errormessage = ""

		self._deviceInfo = {}	#all device info that is common for all devices
		self._deviceInfo[DEV_OBJECT] = obj_NODE
		self._deviceInfo[DEV_SYSTEM] = systemname
		self._deviceInfo[DEV_NAME] = deviceName
		self._deviceInfo[DEV_ID] = "" # unique ID set in ConfDB

		self._deviceInfo[DEV_PORTS_IN] = 0
		self._deviceInfo[DEV_PORTS_OUT] = 0
		self._deviceInfo[DEV_TYPE] = "" #name of the device type it is
		self._deviceInfo[DEV_NODE] = 0 # node or not; a node is a device at one of the ends of a path
		self._deviceInfo[DEV_PROMMODE] = 0 #promoscous mode..
		self._deviceInfo[DEV_SERIAL] = "" # serial number
		self._deviceInfo[DEV_HWTYPE] = "" # hardware type
		self._deviceInfo[DEV_RESPONSIBLE] = "" # responsible for the device
		self._deviceInfo[DEV_LOCATION] = "" # location string
		self._deviceInfo[DEV_COMMENTS] = "" # comments/description
		self._deviceInfo[DEV_COLOUR] = (255, 255, 255) #wxColour(255,255,255)
		self._deviceInfo[OLD_NAME] = "" # if device has been renamed, we store the old/previous name here
		self._deviceInfo[DEV_FUNCTION]="none" #added by Lana, function of the device
		# We need to keep track of some parameters; whether they have been changed when the user modified the object
		# or not because we have different modify functions for different parameters.
		self._deviceInfo[NODE_CHANGED] = 0 #set to 1 if DEV_NODE was changed; 
						    #if changed all paths have to be recalculated in ConfDB Library.
		self._deviceInfo[SYSTEM_CHANGED] = 0
		self._deviceInfo[MODIFY_STATUS] = NO_STATUS #0 - standard modify, 1 - change system list, NO_STATUS: not set

		self._deviceInfo[SAVE_STATUS] = NO_STATUS

		self.__deviceType = None #devicetype object
		self.__ports = [] # ports retrieved, of this device
		self.__links = [] # links retrieved, to/from this device
		self.__cfg = GetConfig() # configuration file reference

		self.__listofpaths = {} # paths this device is a part of

		if new == False:
			self.Update()
			self.UpdateDeviceType()

	### GET ###
	def GetModifyStatus(self):
		return self._deviceInfo[MODIFY_STATUS]
	def GetComments(self):
		return self._deviceInfo[DEV_COMMENTS]
	def GetLocation(self):
		return self._deviceInfo[DEV_LOCATION]
	def GetResponsible(self):
		return self._deviceInfo[DEV_RESPONSIBLE]
	def GetHWType(self):
		return self._deviceInfo[DEV_HWTYPE]
	def GetSerial(self):
		return self._deviceInfo[DEV_SERIAL]
	def GetOldName(self):
		return self._deviceInfo[OLD_NAME]
	def GetSaveStatus(self):
		return self._deviceInfo[SAVE_STATUS] #set INSERT, DELETE and UPDATE as constants
	def GetName(self):
		return self._deviceInfo[DEV_NAME]
	def GetID(self):
		return self._deviceInfo[DEV_ID]
	def GetType(self):
		return self._deviceInfo[DEV_TYPE]
	#added by Lana
	def GetDeviceFunction(self):
		return self._deviceInfo[DEV_FUNCTION]
	def GetNode(self):
		return self._deviceInfo[DEV_NODE]
	def GetPromMode(self):
		return self._deviceInfo[DEV_PROMMODE]
	def GetSystem(self):
		return self._deviceInfo[DEV_SYSTEM]
	def GetColour(self):
		return self._deviceInfo[DEV_COLOUR]
	def GetTheDict(self):
		return self._deviceInfo
	def GetErrorMessage(self):
		return self.__errormessage
	def IsSystemChanged(self):
		return self._deviceInfo[SYSTEM_CHANGED]
	def IsNodeChanged(self):
		return self._deviceInfo[NODE_CHANGED]

	# DevType properties
	def GetPortsIn(self):
		return self._deviceInfo[DEV_PORTS_IN]
	def GetPortsOut(self):
		return self._deviceInfo[DEV_PORTS_OUT]
	def GetTypeDescription(self):
		return self._deviceInfo[DEV_TYPE_DESCRIPTION]

	# Other
	def GetSavedFreePorts(self):
		return (self.totalp,self.inp,self.outp)

	def GetURL(self,update=False):
		if not update:
			return self._deviceInfo[DEV_URL]
		else:		
			cfDB2.execute("SELECT url FROM LHCB_DEVICES_URL WHERE DEVICENAME='"+self._deviceInfo[DEV_NAME]+"' ")
			res = cfDB2.fetchone()
			if res:
				return res[0]
			else:
				return None
	### SET ###
	def SetPortsOut(self,nbr):
		self._deviceInfo[DEV_PORTS_OUT]=nbr
		
	def SetPortsIn(self,nbr):
		self._deviceInfo[DEV_PORTS_IN]=nbr

	def GetPortsOut(self):
		return self._deviceInfo[DEV_PORTS_OUT]
	def SetURL(self,url):
		self._deviceInfo[DEV_URL]=url
	def SetModifyStatus(self,status):
		self._deviceInfo[MODIFY_STATUS] = status
	def SetComments(self,comments):
		Validation.ValidateString("User comments",comments,1000,True)
		self._deviceInfo[DEV_COMMENTS] = comments
	def SetLocation(self,location):
		Validation.ValidateString("Location",location,200,True)
		self._deviceInfo[DEV_LOCATION] = location
	def SetResponsible(self,responsible):
		Validation.ValidateString("Responsible",responsible,100,True)
		self._deviceInfo[DEV_RESPONSIBLE] = responsible
	def SetFunction(self,function):
		Validation.ValidateString("Function",function,100,True)
		self._deviceInfo[DEV_FUNCTION] = function
	def SetHWType(self,hwtype):
		Validation.ValidateString("Hardware type",hwtype,200,True)
		self._deviceInfo[DEV_HWTYPE] = hwtype
	def SetSerial(self,serial):
		Validation.ValidateString("Serial number",serial,500)
		self._deviceInfo[DEV_SERIAL] = serial
	def SetOldName(self,oldname):
		self._deviceInfo[OLD_NAME] = oldname
	def SetSaveStatus(self,state):
		self._deviceInfo[SAVE_STATUS] = state
	def SetName(self,name):
		Validation.ValidateString("Device name",name,100,False)
		self._deviceInfo[DEV_NAME] = name
	def SetID(self,id):
		self._deviceInfo[DEV_ID] = id #not set by user, given in db
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	def SetType(self,devtype,update=True):
		""" Sets the device type of this device.

		@devtype - the unique device type name
		@update - whether we also should update this devices dictionary with
		device type parameters (ports, colour)
		"""
		#TODO: check if there is exception handling in the calling code for this method
		try:
			Validation.ValidateString("Device type",devtype,100,False)
			self._deviceInfo[DEV_TYPE] = devtype

			if update:
				tmp_devtype = DeviceType(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_TYPE],False)

				self._deviceInfo[DEV_PORTS_IN] = tmp_devtype.GetPortsIn()
				self._deviceInfo[DEV_PORTS_OUT] = tmp_devtype.GetPortsOut()
				self._deviceInfo[DEV_TYPE_DESCRIPTION] = tmp_devtype.GetDescription()
				self._deviceInfo[DEV_COLOUR] = tmp_devtype.GetColour()

			return True
				
		except ValidationError,err:
			self.__errormessage = str(err)
			return False

	def SetNode(self,node,changed=True):
		""" We check whether the user changed the value of the node property.
		@node - the value of the node property
		@changed - if the node property was changed set to True, False otherwise
		"""
		Validation.ValidateNumber("Node?",node,0,1)
		if changed:
			self._deviceInfo[NODE_CHANGED] = 1
		self._deviceInfo[DEV_NODE] = node
	def SetPromMode(self,prommode):
		Validation.ValidateNumber("Promiscuous mode",prommode,0,1)
		self._deviceInfo[DEV_PROMMODE] = prommode
	def SetSystem(self,system,changed=True):
		Validation.ValidateSystem("System",system)
		self._deviceInfo[DEV_SYSTEM] = system
		if changed:
			self._deviceInfo[SYSTEM_CHANGED] = 1

	def SetDict(self,newdict):
		for entry in newdict:
			self._deviceInfo[entry] = newdict[entry]
		#self._deviceInfo = copy.deepcopy(newdict)

	def SetAllInfoDict(self,devinfo):
		""" Copy device info from one device to another (this one).
		"""
		my_name = self._deviceInfo[DEV_NAME]
		my_id = self._deviceInfo[DEV_ID]
		self._deviceInfo = {}
		for item in devinfo:
			self._deviceInfo[item] = devinfo[item]
		self._deviceInfo[DEV_NAME] = my_name
		if my_id != "":
			self._deviceInfo[DEV_ID] = my_id

	def UpdateDeviceType(self):
		""" If we need to update the information about the device type this device is, we use this method.
		The method fills the property dictionary with device type parameters and values.
		"""
		
		tmp_devtype = DeviceType(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_TYPE],False)

		self._deviceInfo[DEV_PORTS_IN] = tmp_devtype.GetPortsIn()
		self._deviceInfo[DEV_PORTS_OUT] = tmp_devtype.GetPortsOut()
		self._deviceInfo[DEV_TYPE_DESCRIPTION] = tmp_devtype.GetDescription()
		self._deviceInfo[DEV_COLOUR] = tmp_devtype.GetColour()

	def GetObjectInfo(self,doNotLookUp=True):
		""" Get all info about a device in a subsystem, given the unique name of a device.
		
		Here we do some check where to get the properies information from, f.ex. whether
		this device object is the most recent object in dirty list; if so we can just return
		this objects dictionary. TODO.

		@doNotLookUp - whether we should look for information about this object in dirty objects
		list and ConfDB (False) or not(True).
        	"""
		if doNotLookUp == True:
			return self._deviceInfo
		else:
			self.Update()
			self.UpdateDeviceType()
			
		return self._deviceInfo

	def Update(self,doNotCheckInDirtyList=False):
		""" Here we update the device with the most recent information about the device witht the
		given information; either in dirty objects list (if found and in creation mode), or from
		ConfDB.

		@doNotCheckInDirtyList - if True, we do not check in dirty objects list, but go straight
		forward to the ConfDB and look for information about this device there.
		"""
	
		# Dirty Objects List
		if not doNotCheckInDirtyList:
			##if GetAppWin().GetUserMode() == ID_CREATION_MODE:
			tmp_obj = GetAppWin().GetDirtyObject(self._deviceInfo[DEV_NAME],Device)
			if tmp_obj != False:
				self._deviceInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
				return self._deviceInfo
		# ConfDB
        	try:
		        Validation.ValidateString("Device name",self._deviceInfo[DEV_NAME],100)
	                result = cfDB.GetDeviceRow_devicename(self._deviceInfo[DEV_NAME])

			result = result[1:]	#skip first | character
			infolist = result.split("|")	#split string in list
			i = 0
			deviceInfo = {DEV_OBJECT : obj_NODE}	#To identify this as a devicenode (as opposite to a linknode)
			deviceInfo[DEV_SYSTEM] = self._deviceInfo[DEV_SYSTEM]	#should be retrieved from db? IS retrieved from db!!
			deviceInfo[DEV_COLOUR] = "(255, 255, 255)" #wxColour(red=255,green=255,blue=255) #changed to device type
			deviceInfo[NODE_CHANGED] = 0
			deviceInfo[SYSTEM_CHANGED] = 0
			deviceInfo[DEV_HWTYPE] = ""
			deviceInfo[DEV_RESPONSIBLE] = ""
			deviceInfo[DEV_FUNCTION] = ""
			deviceInfo[DEV_COMMENTS] = ""
			deviceInfo[MODIFY_STATUS] = NO_STATUS
			deviceInfo[DEV_URL] = ""
		
			for i in range(len(infolist)-1):
				tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
				tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
				datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
				if datatype == 'I': #integer
					tmp[1] = int(tmp[1])
				elif datatype == 'C': #string
					tmp[1] = tmp[1].strip()
				deviceInfo[tmp0] = tmp[1]
				i += 1
				
			# Get the URL of the web page associated to our device
			# Added By Walid, Here we will not use the ConfDBpython  and we will connect directely to the database
			DBSystem.cfDB2.execute("SELECT url FROM LHCB_DEVICES_URL WHERE DEVICENAME='"+self._deviceInfo[DEV_NAME]+"' ")
			res = cfDB2.fetchone()
			if res:
				deviceInfo[DEV_URL] = res[0]

			'''### Added by Walid	
			try:
				result = cfDB.GetHWDeviceRow_serialnb(deviceInfo[DEV_SERIAL]) # Get device type info from ConfDB
				if result:
				    result = result[1:]	#skip first '|' character
				    infolist = result.split("|")	#split string to a list; each entry is a parameter name and value

				    i = 0
				    for i in range(len(infolist)-1):
					tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
					tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
					datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
					if datatype == 'I': #integer
						tmp[1] = int(tmp[1])
					elif datatype == 'C': #string
						tmp[1] = tmp[1].strip()
					deviceInfo[tmp0] = tmp[1]
					i += 1
				
			except:
				print "Unable to update HW part"
			#########'''
				
			self._deviceInfo = deviceInfo
			

			# If device type has been renamed and not stored in ConfDB yet
			if self._deviceInfo[DEV_TYPE] in GetAppWin().GetOldNames():
				self._deviceInfo[DEV_TYPE] = GetAppWin().GetNewNamesOfRenamed()[self._deviceInfo[DEV_TYPE]]

			self.UpdateDeviceType() #colour, ports etc
			
                	return self._deviceInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "No information found in the database on the given device: " + str(self._deviceInfo[DEV_NAME])
			return {}
		    else:
		    	self.__errormessage = str(err)
		        return False #on error

	def IsAllPortsFree(self):
		""" Method to check whether the device has any connections at all to other devices.
		If this is true (all ports are free, no connections to other devices), the given device 
		can be deleted, else it can not. 

		!return - True if all ports are free, -1 if not all are free.
		False if an error occurs
		"""

		try:
			freeports,free_in,free_out = self.GetFreePorts()

			if freeports == False and free_in == False and free_out == False:
				raise RuntimeError,"An error occured when checking the ports for this device: " + str(self.GetErrorMessage())

			devtype_tmp = DeviceType(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_TYPE],False)
			nr_of_ports = devtype_tmp.GetPortsIn() + devtype_tmp.GetPortsOut()

			if freeports != nr_of_ports:
				return -1
			
			return True

	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    self.__errormessage = str(err)
		    return False #on error

	def GetFreePorts(self):
		""" Nr of ports on a device is nr of input + nr of output for the given device type it is.
		We find all the taken ports; subtract them from the list of all ports; and return the list
		of free ports (actually as a 3-tuple; number of free ports in total, and then 1 list for each
		direction; IN and OUT)

		!return - 3 tuple:
		integer    - number of free ports in total
		list *port name* - a list of port numbers that are free; TO the device
		list *port name* - a list of port numbers that are free; FROM the device

		False is returned if an error occurs
		"""

		try:
			devtype_tmp = DeviceType(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_TYPE],False)
			free_ports_in = devtype_tmp.GetPortsIn()
			free_ports_out = devtype_tmp.GetPortsOut()
			nr_of_ports = free_ports_in + free_ports_out


			#First we look for links connected to the current device object in the dirty objects list
			my_links = [] #link objects
			dirty_objs_links = [] #link names
			tbd = [] #to be deleted
			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
				for obj in tmp_dirtyobjs:
					if obj.__class__ == Link:
						if obj.GetSaveStatus() != DELETE:
							if obj.GetNodeFrom() == self._deviceInfo[DEV_NAME] or obj.GetNodeTo() == self._deviceInfo[DEV_NAME]:
								if obj.GetName() in dirty_objs_links:
									i = 0
									for lnk in my_links:
										if lnk.GetName() == obj.GetName(): #if name in list already, overwrite cause this is newer
											my_links.pop(i)
										i += 1
											
								my_links.append(obj)
								dirty_objs_links.append(obj.GetName())
						else: # if DELETE status
							tbd.append(obj.GetName())

				#if any links set for deletion, it does not count as a link any more, 
				#remove previous create/modify info for this link connection
				i = 0
				for obj in my_links:
					if obj.GetName() in tbd:
						my_links.pop(i)
					else:
						i += 1

			# If the ID is set..it can be found in ConfDB, then we look there; if not we skip looking in ConfDB
			if self._deviceInfo[DEV_ID] != "": 
				try:
					tmp1 = cfDB.GetLkFromDevID(self._deviceInfo[DEV_ID])
				except RuntimeError,err:
		    			if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
						tmp1 = [] #none found
	
				try:
					tmp2 = cfDB.GetLkToDevID(self._deviceInfo[DEV_ID])
				except RuntimeError,err:
		    			if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
						tmp2 = [] #none found
				
				#temporary list of linknames (IDs)
				#It is necessary to do it as below because the garbage collector will collect 
				#the tmp1 and tmp2 right after retrieval	
				links = []
				i = 0
				while i < len(tmp1):
					links.append(tmp1[i])
					i += 1
				i = 0
				while i < len(tmp2):
					links.append(tmp2[i])
					i += 1

				#add link objects to my_links from ConfDB, if not set for deletion or modified in dl
				i = 0
				if GetAppWin().GetUserMode() == ID_CREATION_MODE:
					while i < len(links):
						my_tmp_link = str(links[i]) #convert to string, because all linksnames (IDs)
									    #are converted to strings in dl
						if my_tmp_link not in dirty_objs_links and my_tmp_link not in tbd:
							link_tmp = Link(self._deviceInfo[DEV_SYSTEM],my_tmp_link,reverseLink=False,new=False)	#create link object
							my_links.append(link_tmp)
						i += 1
				else:
					while i < len(links):
						link_tmp = Link(self._deviceInfo[DEV_SYSTEM],str(links[i]),reverseLink=False,new=False)	#create link object
						my_links.append(link_tmp)
						i += 1


			#Add names of taken ports to a list
			takenports_in = []
			takenports_out = []
			for lnk in my_links:
				if lnk.GetNodeFrom() == self.GetName():
					tmp_port = lnk.GetNodeFrom() + ": " + lnk.GetPortFrom() + " : " + lnk.GetPortTypeFrom() + " : " + "OUT"
					takenports_out.append(tmp_port)
				elif lnk.GetNodeTo() == self.GetName():
					tmp_port = lnk.GetNodeTo() + ": " + lnk.GetPortTo() + " : " + lnk.GetPortTypeTo() + " : " + "IN"
					takenports_in.append(tmp_port)


			# Now we will have to find all port names available on this device, 
			# and then subtract the taken ones found above
			portlist = self.GetPorts() #finds all port objects belonging to the current device

			# If no ports found or wrong number; error, cause ports should be created on device creation
			if portlist == []:
				raise RuntimeError,"Found that no ports are created for this device, cannot continue."
			elif len(portlist) != nr_of_ports:
				#raise RuntimeError,"Found that an incorrect number of ports are created for this device, all the ports must always be created when a device is created."
				GetAppWin().ShowError("An incorrect number of ports are created for this device; either too many or too few considering the port number given for the device type, as a cause of this the result of the action may not be what you expect.",ERR_INFORMATION)

			#Find free ports, for both in and out
			freeports_in = []
			freeports_out = []
			for port in portlist:
				if port.GetPortWay() == 1: #1 IN, 2 OUT
					if port.GetName() not in takenports_in:
						freeports_in.append(port.GetPortNbr() + " : " + port.GetPortType())
				else: # == 2
					if port.GetName() not in takenports_out:
						freeports_out.append(port.GetPortNbr() + " : " + port.GetPortType())
			
			free_ports_total = len(freeports_in) + len(freeports_out) #number of free ports
			self.totalp = free_ports_total
			self.inp = freeports_in	#list of names of free input ports
			self.outp = freeports_out #list of names of free output ports

			return (free_ports_total,freeports_in,freeports_out)

	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False,False,False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False,False,False
		except RuntimeError,err:
		    self.__errormessage = str(err)
		    return False,False,False

	### GetLinks ###
	def GetLinks(self,nodefrom=True):
        	""" Get all links (objects) that start or end in the given device.

		Parameters:
                @nodefrom - if nodefrom is set to true, then all links that start (FROM) in the given device is returned
            	if nodefrom is set to false, then all links that end (TO) in the given device is returned
                
		!return - a list of link objects that either start or end at the current device.
        	"""
		links_in_dirtylist = []
		my_links = []
		dirty_objs_links = []
		tbd = [] #to be deleted
		tbd_objs = []
		if GetAppWin().GetUserMode() == ID_CREATION_MODE:
			tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
			for obj in tmp_dirtyobjs:
				if obj.__class__ == Link:
					if GetAppWin().IsLinkTypeRestricted():
						if obj.GetType() != GetAppWin().GetRestrictedLinkType():
							continue #skip this link, check next
					if obj.GetSaveStatus() != DELETE:
						if (nodefrom == True and obj.GetNodeFrom() == self._deviceInfo[DEV_NAME]) or (nodefrom == False and obj.GetNodeTo() == self._deviceInfo[DEV_NAME]):
							if obj.GetName() in dirty_objs_links:
								i = 0
								for lnk in my_links:
									if lnk.GetName() == obj.GetName():
										my_links.pop(i)
									i += 1
								
							my_links.append(obj)
							dirty_objs_links.append(obj.GetName())
						else:	
							links_in_dirtylist.append(obj.GetName())
					elif obj.GetSaveStatus() == DELETE: # We delete instances before the delete object
						# We do not delete instances after the delete of course
						# Remember that a modify of link connection (node or port from or to) 
						# is actually a DELETE+CREATE
						i = 0
						while i < len(my_links):
							if my_links[i].GetName() == obj.GetName():
								my_links.pop(i)
							else:
								i += 1
						tbd.append(obj.GetName())
				elif obj.__class__ == Device: #to handle deleted devices
					if obj.GetSaveStatus() == DELETE: #set for deletion
						tbd_objs.append(obj)

		try:
        	    Validation.ValidateSystem("DeviceClickGetLinkIDs",self._deviceInfo[DEV_SYSTEM])
		    
		    if not self._deviceInfo[DEV_ID]: # We need to set device ID to get links from ConfDB
			raise RuntimeError, str("NO_ROWS_SELECTED") # pretend as if we didnt get any rows from db

        	    if nodefrom:
	                tmp = cfDB.GetLkFromDevID(self._deviceInfo[DEV_ID])
        	    else:
                	tmp = cfDB.GetLkToDevID(self._deviceInfo[DEV_ID])
		    links = tmp                                                                                                     
	            if links[0] == -1 and links[1] == -1:
			raise DBError, "Error retrieving information about the links from the given device."
	            else:
			i = 0

			if GetAppWin().GetUserMode() == ID_CREATION_MODE: #Creation mode
				if nodefrom:	name = "Loading Output links..."
				else:	name = "loading Input links..."
				progress=QProgressDialog(name, "Abort", len(links), GetAppWin(), "progress", True) #Progress dialog
				while i < len(links):
					progress.setProgress(i)
					if progress.wasCanceled():	
						break
					my_tmplink = str(links[i]) #convert int linkid to string linkid!!
					if my_tmplink not in dirty_objs_links and my_tmplink not in tbd and my_tmplink not in links_in_dirtylist:
						link_tmp = Link(self._deviceInfo[DEV_SYSTEM],my_tmplink,reverseLink=False,new=False)	#create link object

						if link_tmp.GetNodeFrom() in tbd_objs:
							raise DBError,"The link " + str(link_tmp) + " is connected to the deleted device " + str(link_tmp.GetNodeFrom())
 						elif link_tmp.GetNodeTo() in tbd_objs:
							raise DBError,"The link " + str(link_tmp) + " is connected to the deleted device " + str(link_tmp.GetNodeTo())
						
						if GetAppWin().IsLinkTypeRestricted(): # Only get links of set link type
							if link_tmp.GetType() != GetAppWin().GetRestrictedLinkType():
								i += 1
								continue # check next link, because of wrong link type
							
						my_links.append(link_tmp)
					i += 1
				progress.setProgress(len(links))
			else: # Navigation mode
				while i < len(links):
					my_tmplink = str(links[i])
					link_tmp = Link(self._deviceInfo[DEV_SYSTEM],my_tmplink,reverseLink=False,new=False)
					if GetAppWin().IsLinkTypeRestricted(): # Only link of set link type
						if link_tmp.GetType() != GetAppWin().GetRestrictedLinkType():
							i += 1
							continue # check next link, because wrong link type
						
					my_links.append(link_tmp)
					i += 1
			#for tst in my_links:
			#print "Link found: " + str(tst.GetName())
			self.__links = my_links
        	        return self.__links
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			if my_links == []:
				self.__errormessage = "No links found for the selected device " + str(self._deviceInfo[DEV_NAME])
				return [] #none found
			else: #didnt find links in db, but we found in dirty list
				self.__links = my_links
				return self.__links
		    else:
			self.__errormessage = str(err)
		    return False #on error

	def GetPortIDs(self):
        	""" Get all portids of the current device. This is the IDs set automatically
		to ports in the ConfDB. Usually used as a helper function to GetPorts(...).

		!return - list of portIDs (ints) of ports on this device
		"""
                                                                                                                             
	        try:
	            Validation.ValidateNumber("DeviceID",self._deviceInfo[DEV_ID],0)
                                                                                                                             
        	    tmp = cfDB.GetPortIDPerDevID(self._deviceInfo[DEV_ID])
                                                                                                                             
		    ports = tmp
		    i = 0
		    portlist = []
		    while i < len(ports):
			portlist.append(ports[i])
			i += 1

		    return portlist
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
				self.__errormessage = "No ports found for the selected device " + str(self._deviceInfo[DEV_NAME])
				return [] #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error

	def GetPorts(self):
		""" Get all ports assigned to a device as port objects in a list.
		Look for ports in both dirty objects list and ConfDB.

		!return - list of port objects for the ports of a device.
		"""

		# 1. Get all ports assigned to this device in dirty objects list; if in creation mode
		# 2. Get all portIDs assigned to this device from ConfDB; filter out those that were already found in dirty
		# objects list
		# 3. Retrieve all info for each port; from ConfDB or dirty objects list.
		# 4. Return the port objects in a list.

		try:
			my_ports = [] #port objects
			dirty_objs_ports = [] #port names
			tbd = [] #names on ports to be deleted

			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				print "Check"
				tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
				for obj in tmp_dirtyobjs:
					if obj.__class__ == Port:
						if obj.GetDevice() == self.GetName(): #port of this device
							if obj.GetSaveStatus() != DELETE:
								if obj.GetName() in dirty_objs_ports:
									i = 0
									for port in my_ports:
										if port.GetName() == obj.GetName():
											# Newer version found; replace
											my_ports.pop(i)
										i += 1
												
								my_ports.append(obj)
								dirty_objs_ports.append(obj.GetName())
							else: #found port that is set to deleted, look for identical in dl
								print "infodel: " + str(obj.GetObjectInfo(True))
								print "Deleted obj: " + str(obj.GetName())
								if obj.GetName() in dirty_objs_ports:
									# del port found, del port if found in dirty objs
									i = 0
									while i < len(my_ports):
										if my_ports[i].GetName() == obj.GetName():
											my_ports.pop(i)
											break
										i += 1
										
								tbd.append(obj.GetName())

			# We do not have to check whether this device has been renamed or not because
			# we retrieve ports in the ConfDB by looking for ports assigned to the deviceID
			# which will never change.

			if self.GetID() == "" or self.GetID() == -1:
				self._deviceInfo[DEV_ID] = cfDB.GetDeviceID_devicename(self.GetName()) 
				#if this fails we do not have this device in ConfDB, and thus not any ports either.
			if self._deviceInfo[DEV_ID] == -1:
				if self._deviceInfo[DEV_NAME] in GetAppWin().GetNewNames():
					tmp_name = GetAppWin().GetOldNamesOfRenamed()[self._deviceInfo[DEV_NAME]]
					self._deviceInfo[DEV_ID] = cfDB.GetDeviceID_devicename(tmp_name)
					
			if self._deviceInfo[DEV_ID] != -1:
				portids = self.GetPortIDs()
				if portids == False:
					raise RuntimeError,str(self.GetErrorMessage())

				for port in portids:
					tmp_port = Port(self.GetSystem(),"","",PORT_IN,"",True) #will not update
					tmp_port.SetPortID(port)
					tmp_port.UpdateByPortID()

					if tmp_port.GetName() not in dirty_objs_ports and tmp_port.GetName() not in tbd: 
						#in case we have a newer version in the dirty objects list
						#or the port is set to be deleted in dl
						my_ports.append(tmp_port)
						
			self.__ports = my_ports
			return self.__ports

	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			if my_ports == []:
				self.__errormessage = "No ports found for the selected device " + str(self._deviceInfo[DEV_NAME])
				return [] #none found
			else: #didnt find ports in db, but we found in dirty list
				self.__ports = my_ports
				return self.__ports
		    else:
			self.__errormessage = str(err)
		    return False #on error #NB! Change when error is fixed in DB moduel !!!!!

	def GetPaths(self):
		""" Get all the paths this device is a part of. We set the paths found to a
		member variable dictionary, and what we return is actually a list with 
		descriptions (start and end nodes) of each path. Each index in this list
		corresponds to an entry in the member variable dictionary we have set.
		
		!return - a list with string description of each path
		"""
		
		try:
        		Validation.ValidateSystem("Systemname",self._deviceInfo[DEV_SYSTEM])
			progress=QProgressDialog( "Loading paths from the ConfDB...", "Abort", 2, GetAppWin(), "progress", True) #Progress dialog
			#Paths for a given device
			result = cfDB.GetAllPathsPerDevice(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_NAME],1,1)
			del progress
			# First we collect all info in a list of lists, integers instead of strings etc.
			# For information of what this ConfDB Library returns (syntax etc), look in the ConfDB Library
			# documentation.
		    	i = 0
			#pathof0s = []
			pathlisttmp = {}
			nrofpaths = 0		#number of paths, not included the first 0 part
			#nrof0s = 0		#number of common parts of a path for all the paths
			lastpathid = -1
		    	
			while i < len(result):
				#print "Path: " + str(result[i])
				pathtmp = result[i].split("|")
				pathtmp[0] = int(pathtmp[0])
				pathtmp[1] = int(pathtmp[1])
				pathtmp[2] = int(pathtmp[2])

				#if pathtmp[0] == 0:
				#	pathof0s.append(pathtmp[2])
				#	nrof0s += 1
				#else:
				if pathtmp[0] != lastpathid:
					pathlisttmp[pathtmp[0]] = []

				pathlisttmp[pathtmp[0]].append(pathtmp[2])

				if pathtmp[0] != lastpathid: # and pathtmp[0] != 0:
					nrofpaths += 1

				i += 1
				lastpathid = pathtmp[0]

			# how many alternative paths will we have to choose from for this device
			nrofpathsintotal = nrofpaths #* nrof0s 

			pathdict = {}
			j = 1
			#for path0 in pathof0s: #walk through the 0 paths
			for path in pathlisttmp:
				pathdict[j] = []
				#pathdict[j].append(path0)
				pathdict[j].append(pathlisttmp[path])
				pathdict[j] = flatten(pathdict[j]) #the flatten function in helper
				j += 1
			
			if j == 1:
				self.__listofpaths = pathlisttmp
			else:
				self.__listofpaths = pathdict
				

			# self.__listofpaths = {1: [-212,-232], 2: [232,-232], ... } # dict with pathid, then linkids in path

			# Find first and last node for each path
			# Connections given the linkid (every link between two devices has a linkid)
			# TODO: This should be done in GetSpecificPathInfo()
			result = cfDB.LoadConnectivityTable(self._deviceInfo[DEV_SYSTEM])
			j = 0
			linkstmp = {} #dictionary of lists
			while j < len(result):
				linktmp = result[j].split("|")
				linkstmp[int(linktmp[0])] = linktmp #search key indexed on linkid
				j += 1

			# the user will see strings in the pathlist with the first and last nodes in the path
			userpathlist = [] 
			
			for path in self.__listofpaths:
				mystring = ""
				tmparray = self.__listofpaths[path]
				
				# from-node
				if tmparray[0] < 0:
					mystring1 = linkstmp[-tmparray[0]][2]
				else:
					mystring1 = linkstmp[tmparray[0]][1]
					
				# to-node
				if tmparray[-1] < 0:
					mystring2 = linkstmp[-tmparray[-1]][1]
				else:
					mystring2 = linkstmp[tmparray[-1]][2]

				mystring1 = cfDB.GetDeviceName_deviceid(int(mystring1))
				if mystring1 == -1:
					raise DBError,"The Device name was not found in the database."
				mystring2 = cfDB.GetDeviceName_deviceid(int(mystring2))
				if mystring2 == -1:
					raise DBError,"The Device name was not found in the database."
				
				mystring = mystring1 + " --> " + mystring2 + " (" + str(len(tmparray)) + ")" 

				userpathlist.append(mystring)
			
			return userpathlist
			#return self.__listofpaths
			
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "No paths found for the selected device " + str(self._deviceInfo[DEV_NAME])
			return [] #none found
		    else:
			self.__errormessage = str(err)
		    print self.__errormessage
		    return False #on error
			
	def GetSpecificPathInfo(self,pathid):
		""" Return all path information necessary to display a path for a given path(ID).
		
		A path from the list of paths for a device was selected; the entry index of that list
		corresponds to entries in our member variable dictionary.

		Parameters:
		@pathid - the ID of the path chosen to get path information for. The path ID is the index
		of the entry in the list returned by the GetPaths(...) function.

		!return - an ordered list of member variable dictionaries for the devices in the given path.
		"""
		try:
			if self.__listofpaths == {}:
				self.GetPaths()

			# We need to get information about the connections in this system
			# TODO: We cannot load the whole connectivity table every time!!!
			# Alt. 1: Fill a list with the info, and use that on second retrieval
			# Alt. 2: Only find necessary info on some given links
			#result = cfDB.LoadConnectivityTable(self._deviceInfo[DEV_SYSTEM])
			
			#j = 0
			#links = {} #dictionary of lists of links (connectivity information)
			#while j < len(result):
			#linktmp = result[j].split("|")
			#links[int(linktmp[0])] = linktmp #search key on linkid
			#j += 1
			

			linkdevinfolist = []
			linksadded = []
			my_path = self.__listofpaths[pathid] #the pathid set in the parameter list
			print "Path: " + str(my_path)
			# We will then set up each link/device in this path, and return it.
			for link in my_path:
				my_reverseLink = False
				# if a linkID is negative; it means that the data flow is in the opposite direction
				# of what you get from the ConfDB; reversed link
				if link < 0: 
					link *= -1
					my_reverseLink = True
				
				link_tmp = Link(self._deviceInfo[DEV_SYSTEM],link,reverseLink=my_reverseLink,new=False)
				devinfotmp = link_tmp.GetObjectInfo(True)
				devinfotmp["reversed"] = my_reverseLink
				
				# we dont want to draw the same link two times; bidirectional links are retrieved two times
				# from the ConfDB
				if link not in linksadded: 
					#bidirectional link
					linkdevinfolist.append(devinfotmp) #objects
					linksadded.append(link)
					
			return linkdevinfolist
		
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    self.__errormessage = str(err)
		    return False #on error
	    
	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""
	
		save_status = self._deviceInfo[SAVE_STATUS]
	
		if save_status == CREATE:
			if insertmany:
				tofile = 'cfDB.InsertMultipleFunctionalDevices("%s","%s","%s",%i,%i,"%s","%s","%s","%s","%s","%s",%i,%i)' \
				%(str(self._deviceInfo[DEV_SYSTEM]),str(self._deviceInfo[DEV_NAME]),str(self._deviceInfo[DEV_TYPE]),int(self._deviceInfo[DEV_NODE]),int(self._deviceInfo[DEV_PROMMODE]),str(self._deviceInfo[DEV_SERIAL]),str(self._deviceInfo[DEV_HWTYPE]),str(self._deviceInfo[DEV_RESPONSIBLE]),str(self._deviceInfo[DEV_LOCATION]),str(self._deviceInfo[DEV_COMMENTS]),str(self._deviceInfo[DEV_FUNCTION]),first,commit)
			else:
				tofile = 'cfDB.InsertFunctionalDevice("%s","%s","%s",%i,%i,"%s","%s","%s","%s","%s","%s",%i)' \
				%(str(self._deviceInfo[DEV_SYSTEM]),str(self._deviceInfo[DEV_NAME]),str(self._deviceInfo[DEV_TYPE]),self._deviceInfo[DEV_NODE],self._deviceInfo[DEV_PROMMODE],str(self._deviceInfo[DEV_SERIAL]),str(self._deviceInfo[DEV_HWTYPE]),str(self._deviceInfo[DEV_RESPONSIBLE]),str(self._deviceInfo[DEV_LOCATION]),str(self._deviceInfo[DEV_COMMENTS]),str(self._deviceInfo[DEV_FUNCTION]),commit)
			
			if self._deviceInfo[DEV_URL]!="":
				tofile += '\ncfDB2.execute(\"INSERT INTO LHCB_DEVICES_URL (DEVICENAME, URL) VALUES (\''+self._deviceInfo[DEV_NAME]+'\',\''+self._deviceInfo[DEV_URL]+'\') \")\n'
			return tofile
			
		elif save_status == MODIFY:
			modify_status = self._deviceInfo[MODIFY_STATUS]
			
			if modify_status == 0: # Change Node
				try:
					if self._deviceInfo[NODE_CHANGED] == 1:
						node = self._deviceInfo[DEV_NODE]
					else:
						node = -1
				except KeyError,err:
					node = -1

				#devicename, node, promiscuous_mode, location, function_list, frist, commit
				tofile = 'cfDB.UpdateMultipleAttributesDevices("%s",%i,%i,"%s","%s",%i,%i)' \
				%(str(self._deviceInfo[DEV_NAME]),node,self._deviceInfo[DEV_PROMMODE],str(self._deviceInfo[DEV_LOCATION]),"none",first,commit)
				tofile += '\ncfDB2.execute(\"UPDATE LHCB_DEVICES_URL SET URL=\''+self._deviceInfo[DEV_URL]+'\' WHERE DEVICENAME=\''+self._deviceInfo[DEV_NAME]+'\' \")'
				return tofile
				
			elif modify_status == 1: # Change System
				return '\ncfDB.UpdateMultipleDeviceSystemList("%s","%s",%i,%i)' \
				%(str(self._deviceInfo[DEV_NAME]),str(self._deviceInfo[DEV_SYSTEM]),first,commit)

		elif save_status == DELETE:
			tofile = 'cfDB.DeleteFunctionalDevice("%i")'\
                        %(int(self._deviceInfo[DEV_ID]))
			tofile += '\ncfDB2.execute(\"DELETE FROM LHCB_DEVICES_URL WHERE DEVICENAME=\''+self._deviceInfo[DEV_NAME]+'\' \")'
			return tofile
			
		elif save_status == RENAME:
			tofile = 'cfDB.UpdateMultipleDevNamesDevices("%s","%s",%i,%i)' \
			%(str(self._deviceInfo[OLD_NAME]),str(self._deviceInfo[DEV_NAME]),first,commit)
			tofile += '\ncfDB2.execute(\"UPDATE LHCB_DEVICES_URL SET DEVICENAME=\''+self._deviceInfo[DEV_NAME]+'\' WHERE DEVICENAME=\''+self._deviceInfo[OLD_NAME]+'\' \")'
			return tofile
			
		else: #invalid status
			return ""


	def Save(self,first=False,insertmany=False,commit=True):
		""" Update changes to the ConfDB for this object.

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""
		try:
			if self._deviceInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE,RENAME]:
				raise ValidationError,"This object should not be in the dirty list or has incorrect savestatus setting"

			if self._deviceInfo[SAVE_STATUS] == RENAME:
				Validation.ValidateString("Device name",self._deviceInfo[DEV_NAME],100)

			if self._deviceInfo[SAVE_STATUS] == CREATE or self._deviceInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateSystem("Create Device",self._deviceInfo[DEV_SYSTEM])
				Validation.ValidateNumber("Node",self._deviceInfo[DEV_NODE],0,1)
				Validation.ValidateNumber("Promiscous mode",self._deviceInfo[DEV_PROMMODE],0,1)
				Validation.ValidateString("Device name",self._deviceInfo[DEV_NAME],100)
				# TODO: Check that the device type exists in the ConfDB?
				Validation.ValidateString("Device type",self._deviceInfo[DEV_TYPE],100)
				Validation.ValidateString("Serial number",self._deviceInfo[DEV_SERIAL],500)
				Validation.ValidateString("Hardware type",self._deviceInfo[DEV_HWTYPE],200,True)
				Validation.ValidateString("Responsible",self._deviceInfo[DEV_RESPONSIBLE],100,True)
				Validation.ValidateString("Location",self._deviceInfo[DEV_LOCATION],200,True)
				Validation.ValidateString("Comments",self._deviceInfo[DEV_COMMENTS],1000,True)
				Validation.ValidateString("Function",self._deviceInfo[DEV_FUNCTION],100,True)

			if self._deviceInfo[SAVE_STATUS] == CREATE:
				if insertmany:
					success = cfDB.InsertMultipleFunctionalDevices(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_NAME],self._deviceInfo[DEV_TYPE],self._deviceInfo[DEV_NODE],self._deviceInfo[DEV_PROMMODE],self._deviceInfo[DEV_SERIAL],self._deviceInfo[DEV_HWTYPE],self._deviceInfo[DEV_RESPONSIBLE],self._deviceInfo[DEV_LOCATION],self._deviceInfo[DEV_COMMENTS],self._deviceInfo[DEV_FUNCTION],first,commit)
				else:
					print (self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_NAME],self._deviceInfo[DEV_TYPE],self._deviceInfo[DEV_NODE],self._deviceInfo[DEV_PROMMODE],self._deviceInfo[DEV_SERIAL],self._deviceInfo[DEV_HWTYPE],self._deviceInfo[DEV_RESPONSIBLE],self._deviceInfo[DEV_LOCATION],self._deviceInfo[DEV_COMMENTS],self._deviceInfo[DEV_FUNCTION],commit)
					success = cfDB.InsertFunctionalDevice(self._deviceInfo[DEV_SYSTEM],self._deviceInfo[DEV_NAME],self._deviceInfo[DEV_TYPE],self._deviceInfo[DEV_NODE],self._deviceInfo[DEV_PROMMODE],self._deviceInfo[DEV_SERIAL],self._deviceInfo[DEV_HWTYPE],self._deviceInfo[DEV_RESPONSIBLE],self._deviceInfo[DEV_LOCATION],self._deviceInfo[DEV_COMMENTS],self._deviceInfo[DEV_FUNCTION],commit)
				
				# Add entry in the LHCB_DEVICES_URL:
				if success and self._deviceInfo[DEV_URL]!="":
					cfDB2.execute("INSERT INTO LHCB_DEVICES_URL (DEVICENAME, URL) VALUES ('"+self._deviceInfo[DEV_NAME]+"','"+self._deviceInfo[DEV_URL]+"') ")
				if commit:
					oracle.commit()
					self.Update(True)

			elif self._deviceInfo[SAVE_STATUS] == MODIFY:
				if self._deviceInfo[MODIFY_STATUS] == 0: # Change Node
					try:
						if self._deviceInfo[NODE_CHANGED] == 1:
							node = self._deviceInfo[DEV_NODE]
						else:
							node = -1
					except KeyError,err:
						node = -1
					print "DEBUG MSG : cfDB.UpdateMultipleAttributesDevices",(self._deviceInfo[DEV_NAME],node,self._deviceInfo[DEV_PROMMODE],self._deviceInfo[DEV_LOCATION],'none',first,commit)
					success = cfDB.UpdateMultipleAttributesDevices(self._deviceInfo[DEV_NAME],node,self._deviceInfo[DEV_PROMMODE],self._deviceInfo[DEV_LOCATION],'none',first,commit)
					dev_url = self.GetURL(True);
					# Added by walid
					if success:
						if dev_url and dev_url!=self._deviceInfo[DEV_URL]:
							cfDB2.execute("UPDATE LHCB_DEVICES_URL SET URL='"+self._deviceInfo[DEV_URL]+"' WHERE DEVICENAME='"+self._deviceInfo[DEV_NAME]+"' ")
						elif not dev_url and self._deviceInfo[DEV_URL]!="":
							cfDB2.execute("INSERT INTO LHCB_DEVICES_URL (DEVICENAME, URL) VALUES ('"+self._deviceInfo[DEV_NAME]+"','"+self._deviceInfo[DEV_URL]+"') ")
						
					if commit:
						oracle.commit()
						self._deviceInfo[NODE_CHANGED] = 0
						self.Update(True)
						
				elif self._deviceInfo[MODIFY_STATUS] == 1: # Change System

					success = cfDB.UpdateMultipleDeviceSystemList(self._deviceInfo[DEV_NAME],self._deviceInfo[DEV_SYSTEM],first,commit) #first and commit

			elif self._deviceInfo[SAVE_STATUS] == RENAME:
				success = cfDB.UpdateMultipleDevNamesDevices(self._deviceInfo[OLD_NAME],self._deviceInfo[DEV_NAME],first,commit)
				if commit:
					self.Update(True)
				# Added by walid
				if success:
					dev_url = self.GetURL(True);
					if dev_url:
						cfDB2.execute("UPDATE LHCB_DEVICES_URL SET DEVICENAME='"+self._deviceInfo[DEV_NAME]+"' WHERE DEVICENAME='"+self._deviceInfo[OLD_NAME]+"' ")
					
				
			elif self._deviceInfo[SAVE_STATUS] == DELETE:
			    portIDs = self.GetPortIDs()
			    if portIDs !=[]:
				for portID in portIDs:
					cfDB.DeletePortRow(portID)
			    success=cfDB.DeleteFunctionalDevice(self._deviceInfo[DEV_ID])
			    cfDB2.execute("DELETE FROM LHCB_DEVICES_URL WHERE DEVICENAME='"+self._deviceInfo[DEV_NAME]+"' ")
			    
			return True

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
			self.__errormessage = str(err)
			return False #on error

	def Delete(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		
		self._deviceInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self)

	def Modify(self,commit=1):
		"""Sets the save status of this object to modify, and
		adds it to the dirty objects list for later update in the ConfDB.
		The Modify status is set in the respective Set-functions for the
		parameters they change.
		"""
		self._deviceInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
        	""" Sets the save status of this object to create, and adds it
		to the dirty objects list for later update in the ConfDB.
        	"""
		
		self._deviceInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)

class Link(DBInfo):
	""" A link is a physical connection between two devices, and displayed visually in the
	visual window as lines.
	"""

	def __init__(self,systemname,linkID=-1,reverseLink=False,new=True):
		""" Constructor.

		Parameters:
		@systemname - (Obsolete) Name of the subsystem a link is in, but
		is now longer needed to be set because the subsystem it is in can be
		retrieved from the devices it connects.
		@linkID - If already stored in ConfDB the linkID is set, otherwise it is -1
		@reverseLink - only used when creating links in path view. If this is true
		the link is the opposite of what that is set (from = to and to = from)
		@new - whether the link is being created (True), or already created (False).
		"""

		self._deviceInfo = {}
		self._deviceInfo[DEV_OBJECT] = obj_LINK
		self._deviceInfo[DEV_SYSTEM] = systemname
		self._deviceInfo[DEV_LINK] = str(linkID) #always string, also linkIDs from the ConfDB

		self._deviceInfo[DEV_SWITCH_FROM] = -1
		self._deviceInfo[DEV_SWITCH_TO] = -1
		self._deviceInfo[DEV_PORT_FROM] = -1
		self._deviceInfo[DEV_PORT_TO] = -1

		self._deviceInfo[DEV_PORTTYPE_TO] = ""
		self._deviceInfo[DEV_PORTTYPE_FROM] = ""

		self._deviceInfo[DEV_LINKTYPE] = -1
		self._deviceInfo[DEV_BIDIRECTIONAL] = 0 #default, no
		self._deviceInfo[DEV_LINK_USED] = -1 #default yes when creating, can be set to 0 if not to be used
		self._deviceInfo[DEV_LINKINFO] = ""

		#variables we need in order to know whether the 3 variables above have been modified by the user
		self._deviceInfo[LINKTYPE_CHANGED] = 0
		self._deviceInfo[BIDIRECTIONAL_CHANGED] = 0
		self._deviceInfo[LINKUSED_CHANGED] = 0
		self._deviceInfo[LINKINFO_CHANGED] = 0

		# In case different link types should have different colours..not used
		self._deviceInfo[DEV_COLOUR] = "(0, 0, 0)" #wxColour(0,0,0)

		self._deviceInfo[SAVE_STATUS] = NO_STATUS

		self.__reverseLink = reverseLink #only used for GetPaths() in a device object

		self.__cfg = GetConfig()

		self.__errormessage = ""

		if new == False:
			self.Update()

	### GET ###
	def GetSystem(self):
		return self._deviceInfo[DEV_SYSTEM]
	def GetSaveStatus(self):
		return self._deviceInfo[SAVE_STATUS] #set INSERT, DELETE and UPDATE as constants
	def GetName(self):
		return str(self._deviceInfo[DEV_LINK])
	def GetNodeTo(self):
		return self._deviceInfo[DEV_SWITCH_TO]
	def GetNodeFrom(self):
		return self._deviceInfo[DEV_SWITCH_FROM]
	def GetPortTo(self):
		return self._deviceInfo[DEV_PORT_TO]
	def GetColor(self):
		return self._deviceInfo[DEV_COLOUR]
	def GetPortFrom(self):
		return self._deviceInfo[DEV_PORT_FROM]
	def GetPortTypeTo(self):
		return self._deviceInfo[DEV_PORTTYPE_TO]
	def GetPortTypeFrom(self):
		return self._deviceInfo[DEV_PORTTYPE_FROM]
	def GetReverseLink(self):
		return self.__reverseLink
	def GetType(self):
		return self._deviceInfo[DEV_LINKTYPE]
	def GetBidirectional(self):
		return self._deviceInfo[DEV_BIDIRECTIONAL]
	def GetLinkUsed(self):
		return self._deviceInfo[DEV_LINK_USED]
	def GetLinkInfo(self):
		return self._deviceInfo[DEV_LINKINFO]
	def GetErrorMessage(self):
		return self.__errormessage
	def SetErrorMessage(self,msg):
		self.__errormessage = msg

	###SET###
	def SetSaveStatus(self,state):
		self._deviceInfo[SAVE_STATUS] = state
	def SetName(self,name):
		# temporary unique link name before save to ConfDB
		# set automatically in code (no user interference)
		self._deviceInfo[DEV_LINK] = str(name) 
	def SetNodeTo(self,node):
		Validation.ValidateString("Node To",node,100,False)
		self._deviceInfo[DEV_SWITCH_TO] = node
	def SetNodeFrom(self,node):
		Validation.ValidateString("Node From",node,100,False)
		self._deviceInfo[DEV_SWITCH_FROM] = node
	def SetPortTo(self,port):
		Validation.ValidateString("Port to",port,50,False)
		self._deviceInfo[DEV_PORT_TO] = port
	def SetPortFrom(self,port):
		Validation.ValidateString("Port from",port,50,False)
		self._deviceInfo[DEV_PORT_FROM] = port
	def SetPortTypeTo(self,porttype):
		Validation.ValidateString("Porttype To",porttype,100,True)
		self._deviceInfo[DEV_PORTTYPE_TO] = porttype
	def SetPortTypeFrom(self,porttype):
		Validation.ValidateString("Porttype From",porttype,100,True)
		self._deviceInfo[DEV_PORTTYPE_FROM] = porttype
	def SetReverseLink(self,reverse):
		self.__reverseLink = reverse
	def SetSystem(self,system):
            	Validation.ValidateSystem("Systemname",system)
	    	self._deviceInfo[DEV_SYSTEM] = system #validate system

	def SetLinkInfo(self,linkinfo,changed=True):
		Validation.ValidateString("Link Info",linkinfo,1000,True)
		self._deviceInfo[DEV_LINKINFO] = linkinfo
		if changed:
			self._deviceInfo[LINKINFO_CHANGED] = 1
		else:
			self._deviceInfo[LINKINFO_CHANGED] = 0
	def IsTypeChanged(self):
		return self._deviceInfo[LINKTYPE_CHANGED] == 1
	def IsBidirChanged(self):
		return self._deviceInfo[BIDIRECTIONAL_CHANGED] == 1
	def IsLkusedChanged(self):
		return self._deviceInfo[LINKUSED_CHANGED] == 1
	def IsReversed(self):
		return self.__reverseLink
	def IsLkInfoChanged(self):
		return self._deviceInfo[LINKINFO_CHANGED] == 1
	
	def SetType(self,linktype,changed=True):
		""" Update linktype name for a link.
		"""
		Validation.ValidateString("Linktype",linktype,100,False)
		self._deviceInfo[DEV_LINKTYPE] = linktype #name for user, but change to id before save, name or id, no Validation
		if changed:
			self._deviceInfo[LINKTYPE_CHANGED] = 1
		else:
			self._deviceInfo[LINKTYPE_CHANGED] = 0
	def SetBidirectional(self,bidir,changed=True):
		Validation.ValidateNumber("Bidirectional",bidir,0,1)
		self._deviceInfo[DEV_BIDIRECTIONAL] = bidir
		if changed:
			self._deviceInfo[BIDIRECTIONAL_CHANGED] = 1
		else:
			self._deviceInfo[BIDIRECTIONAL_CHANGED] = 0
	def SetLinkUsed(self,lkused,changed=True):
		Validation.ValidateNumber("Link Used",lkused,0,1)
		self._deviceInfo[DEV_LINK_USED] = lkused
		if changed:
			self._deviceInfo[LINKUSED_CHANGED] = 1
		else:
			self._deviceInfo[LINKUSED_CHANGED] = 0
	def SetDict(self,newdict):
		""" Copy member variable (parameters) information
		from a link object to another (this)
		"""
		#self._deviceInfo = copy.deepcopy(newdict)
		for entry in newdict:
			self._deviceInfo[entry] = newdict[entry]

	def GetObjectInfo(self,doNotLookUp=True):
        	"""Get all info about a Point-to-Point connection (link).
                
		TODO: Here we can have checks whether we need to look for link information in
		dirty objects list or ConfDB if this is the most recent version of the link
		object.
	        
		@doNotLookUp - whether we should look for information about this object in dirty objects
		list and ConfDB (False) or not(True).
        	"""
		
		if doNotLookUp == True:
			return self._deviceInfo
		else:
			return self.Update()

	def Update(self,doNotCheckInDirtyList=False):
		""" Update the information/parameters for this link with the most recent information,
		and also return the member variable dictionary.

		Parameters:
		@doNotCheckInDirtyList - if set to True, we do not check in the dirty objects list for
		a version of the object.

		!return - the member variable dictionary of parameters for the link
		"""

		if not doNotCheckInDirtyList:
			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				tmp_obj = GetAppWin().GetDirtyObject(self._deviceInfo[DEV_LINK],Link)
				if tmp_obj != False:
					self._deviceInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self._deviceInfo

        	try:
        	    Validation.ValidateNumber("LinkID",int(self._deviceInfo[DEV_LINK]))
		    linkerror = True
	            result = cfDB.GetMacroConnectivityRow_lkid(int(self._deviceInfo[DEV_LINK]))
		    linkerror = False
		    	
		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string in list

		    i = 0
		    deviceInfo = {DEV_OBJECT : obj_LINK}	#To identify this as a linknode (as opposite to a devicenode)
		    deviceInfo[DEV_SYSTEM] = self._deviceInfo[DEV_SYSTEM]

		    deviceInfo[LINKTYPE_CHANGED] = 0
		    deviceInfo[BIDIRECTIONAL_CHANGED] = 0
		    deviceInfo[LINKUSED_CHANGED] = 0
		    deviceInfo[LINKINFO_CHANGED] = 0
		    deviceInfo[DEV_LINKINFO] = ""
	
		    for i in range(len(infolist)-1):
			tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
			tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
			datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
			if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
			elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()

			deviceInfo[tmp0] = tmp[1]
			i += 1

		    # If link is reversed; fix it
		    if self.__reverseLink == True:
			deviceInfo[DEV_SWITCH_FROM], deviceInfo[DEV_SWITCH_TO] = deviceInfo[DEV_SWITCH_TO], deviceInfo[DEV_SWITCH_FROM]
			deviceInfo[DEV_PORT_FROM], deviceInfo[DEV_PORT_TO] = deviceInfo[DEV_PORT_TO], deviceInfo[DEV_PORT_FROM]
			deviceInfo[DEV_PORTTYPE_FROM], deviceInfo[DEV_PORTTYPE_TO] = deviceInfo[DEV_PORTTYPE_TO], deviceInfo[DEV_PORTTYPE_FROM]

		    deviceInfo[DEV_LINK] = str(deviceInfo[DEV_LINK]) #convert the linktypeID of integer to string

		    try:
			deviceInfo[DEV_COLOUR] = LinkTypesColor[deviceInfo[DEV_LINKTYPE]]
		    except:
			print "WARNING : there no corresponding color found for the device type: "+deviceInfo[DEV_LINKTYPE]
			deviceInfo[DEV_COLOUR] = '(0,0,0)'
			
		    # Check whether link type of device type has been renamed, and take action thereafter
		    if deviceInfo[DEV_LINKTYPE] in GetAppWin().GetOldNames():
			deviceInfo[DEV_LINKTYPE] = GetAppWin().GetNewNamesOfRenamed()[deviceInfo[DEV_LINKTYPE]] #set to the new name of the devtype
		    if deviceInfo[DEV_SWITCH_FROM] in GetAppWin().GetOldNames():
			deviceInfo[DEV_SWITCH_FROM] = GetAppWin().GetNewNamesOfRenamed()[deviceInfo[DEV_SWITCH_FROM]] #set to the new name of the dv
		    if deviceInfo[DEV_SWITCH_TO] in GetAppWin().GetOldNames():
			deviceInfo[DEV_SWITCH_TO] = GetAppWin().GetNewNamesOfRenamed()[deviceInfo[DEV_SWITCH_TO]] #set to the new name of the dev	

		    if deviceInfo[DEV_PORTTYPE_FROM].lower() == "none":
			deviceInfo[DEV_PORTTYPE_FROM] = ""
		    if deviceInfo[DEV_PORTTYPE_TO].lower() == "none":
			deviceInfo[DEV_PORTTYPE_TO] = ""

		    self._deviceInfo = deviceInfo
		    
		    #self.UpdateLinkType() #needed to be done if we choose to use different colours for different linktypes
		
		    return self._deviceInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			if linkerror:
				self.__errormessage = "Didn't find the LinkID in the database: " + str(self._deviceInfo[DEV_LINK])
			else:
				self.__errormessage = "Didn't find one of the devices the link links to in the database: " + str(self._deviceInfo[DEV_LINK])
			return {} #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error
		except ValueError,err:
		    self.__errormessage = str("LinkID is not a valid ID.")
		    return False
	    
	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""

		save_status = self._deviceInfo[SAVE_STATUS]
		
		# Change name of port type to none, because the ConfDB Library function expects that instead
		# of ""
		if self._deviceInfo[DEV_PORTTYPE_FROM] == "":
			porttype_from = "none"
		else:
			porttype_from = self._deviceInfo[DEV_PORTTYPE_FROM]
			
		if self._deviceInfo[DEV_PORTTYPE_TO] == "":
			porttype_to = "none"
		else:
			porttype_to = self._deviceInfo[DEV_PORTTYPE_TO]

	
		if save_status == CREATE:
			if insertmany:
				return 'cfDB.InsertMultipleMacroLinks("%s","%s","%s","%s","%s","%s","%s",%i,"%s",%i,%i)' \
				%(str(self._deviceInfo[DEV_SWITCH_FROM]),str(self._deviceInfo[DEV_SWITCH_TO]),str(self._deviceInfo[DEV_PORT_FROM]),str(self._deviceInfo[DEV_PORT_TO]),str(porttype_from),str(porttype_to),str(self._deviceInfo[DEV_LINKTYPE]),self._deviceInfo[DEV_BIDIRECTIONAL],self._deviceInfo[DEV_LINKINFO],first,commit)
			else:
				return 'cfDB.InsertMacroLink("%s","%s","%s","%s","%s","%s","%s",%i,"%s",%i)' \
				%(str(self._deviceInfo[DEV_SWITCH_FROM]),str(self._deviceInfo[DEV_SWITCH_TO]),str(self._deviceInfo[DEV_PORT_FROM]),str(self._deviceInfo[DEV_PORT_TO]),str(porttype_from),str(porttype_to),str(self._deviceInfo[DEV_LINKTYPE]),self._deviceInfo[DEV_BIDIRECTIONAL],self._deviceInfo[DEV_LINKINFO],commit)

		elif save_status == MODIFY:
			modify_status = self._deviceTypeInfo[MODIFY_STATUS]
			tmpstring = ""
			
			if self._deviceInfo[BIDIRECTIONAL_CHANGED] == 1:
				tmpstring += 'cfDB.UpdateMultipleBidirectionalLinks("%s","%s","%s","%s","%s","%s",%i,%i,%i)\n' \
				%(str(self._deviceInfo[DEV_SWITCH_FROM]),"none",str(self._deviceInfo[DEV_PORT_FROM]),str(porttype_from),"none","none",self._deviceInfo[DEV_BIDIRECTIONAL],first,commit)
				
			if self._deviceInfo[LINKTYPE_CHANGED] == 1:
				tmpstring += 'cfDB.UpdateMultipleLkTypeLinks("%s","%s","%s","%s","%s","%s","%s",%i,%i)\n' \
				%(str(self._deviceInfo[DEV_SWITCH_FROM]),"none",str(self._deviceInfo[DEV_PORT_FROM]),"none",str(porttype_from),"none",str(self._deviceInfo[DEV_LINKTYPE]),first,commit)
			
			if self._deviceInfo[LINKUSED_CHANGED] == 1:
				tmpstring += 'cfDB.UpdateMultipleLkUsedLinks("%s","%s","%s","%s","%s","%s",%i,%i,%i)\n' \
				%(str(self._deviceInfo[DEV_SWITCH_FROM]),"none",str(self._deviceInfo[DEV_PORT_FROM]),"none",str(porttype_from),"none",self._deviceInfo[DEV_LINK_USED],first,commit) ###??
				
			if self._deviceInfo[LINKINFO_CHANGED] == 1:
				tmpstring += 'cfDB.UpdateMultipleLkInfoLinks("%s","%s","%s","%s","%s","%s","%s",%i,%i)\n' \
				%(str(self._deviceInfo[DEV_SWITCH_FROM]),"none",str(self._deviceInfo[DEV_PORT_FROM]),"none",str(porttype_from),"none",self._deviceInfo[DEV_LINKINFO],first,commit) ###??


			return tmpstring

		elif save_status == DELETE:
			return 'cfDB.DeleteLinkRow(%i,%i)' \
			%(int(self._deviceInfo[DEV_LINK]),1)
	
		else: #invalid status
			return ""


	def Save(self,first=False,insertmany=False,commit=True):
		""" Update changes to the ConfDB for this object.

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""
		try:
			if self._deviceInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE]:
				raise RuntimeError, "Save status incorrectly set. This object should not be stored in db."
			
			if self._deviceInfo[SAVE_STATUS] == CREATE or self._deviceInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateString("Device from",self._deviceInfo[DEV_SWITCH_FROM],100)
				Validation.ValidateString("Device to",self._deviceInfo[DEV_SWITCH_TO],100)
				Validation.ValidateString("Port from",self._deviceInfo[DEV_PORT_FROM],50,False)
				Validation.ValidateString("Port to",self._deviceInfo[DEV_PORT_TO],50,False)
				Validation.ValidateString("Porttype to",self._deviceInfo[DEV_PORTTYPE_TO],100,True)
				Validation.ValidateString("Porttype from",self._deviceInfo[DEV_PORTTYPE_FROM],100,True)
				Validation.ValidateString("Linktype",self._deviceInfo[DEV_LINKTYPE],100)
				Validation.ValidateNumber("Link bidirectional",self._deviceInfo[DEV_BIDIRECTIONAL],-1,1)
				Validation.ValidateString("Link ID",self._deviceInfo[DEV_LINK],100)
				Validation.ValidateString("Link Info",self._deviceInfo[DEV_LINKINFO],1000,True)

				# Change name of port type to none, because the ConfDB Library function expects that instead
				# of ""
				if self._deviceInfo[DEV_PORTTYPE_FROM] == "":
					porttype_from = "none"
				else:
					porttype_from = self._deviceInfo[DEV_PORTTYPE_FROM]
				if self._deviceInfo[DEV_PORTTYPE_TO] == "":
					porttype_to = "none"
				else:
					porttype_to = self._deviceInfo[DEV_PORTTYPE_TO]

			if self._deviceInfo[SAVE_STATUS] == MODIFY or self._deviceInfo[SAVE_STATUS] == DELETE:
				Validation.ValidateNumber("Link ID",int(self._deviceInfo[DEV_LINK]),0)

			if self._deviceInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateNumber("Link used",self._deviceInfo[DEV_LINK_USED],-1,1)
			
			print " END VALIDATION "
			print self._deviceInfo[SAVE_STATUS],CREATE
			if self._deviceInfo[SAVE_STATUS] == CREATE:
				if insertmany:
				    success = cfDB.InsertMultipleMacroLinks(self._deviceInfo[DEV_SWITCH_FROM],self._deviceInfo[DEV_SWITCH_TO],self._deviceInfo[DEV_PORT_FROM],self._deviceInfo[DEV_PORT_TO],porttype_from,porttype_to,self._deviceInfo[DEV_LINKTYPE],self._deviceInfo[DEV_BIDIRECTIONAL],self._deviceInfo[DEV_LINKINFO],first,commit)
				else:
				    print "argument de ma fonction : "
				    print (self._deviceInfo[DEV_SWITCH_FROM],self._deviceInfo[DEV_SWITCH_TO],self._deviceInfo[DEV_PORT_FROM],self._deviceInfo[DEV_PORT_TO],porttype_from,porttype_to,self._deviceInfo[DEV_LINKTYPE],self._deviceInfo[DEV_BIDIRECTIONAL], self._deviceInfo[DEV_LINKINFO],commit)
				    print "_-_____-_____-----_________-------________------"
				    success = cfDB.InsertMacroLink(self._deviceInfo[DEV_SWITCH_FROM],self._deviceInfo[DEV_SWITCH_TO],self._deviceInfo[DEV_PORT_FROM],self._deviceInfo[DEV_PORT_TO],porttype_from,porttype_to,self._deviceInfo[DEV_LINKTYPE],self._deviceInfo[DEV_BIDIRECTIONAL], self._deviceInfo[DEV_LINKINFO],commit)
				    print "Resultat : " +  str(success)
				if commit:
					#change from temp linkname to linkid in db
					found = self.SetLinkIDByDeviceAndPort(self._deviceInfo[DEV_SWITCH_FROM],self._deviceInfo[DEV_PORT_FROM],porttype_from,self._deviceInfo[DEV_SWITCH_TO],self._deviceInfo[DEV_PORT_TO],porttype_to)
					if not found:
						raise RuntimeError,str(self.__errormessage)
					else:
						self.Update(True)
						
			elif self._deviceInfo[SAVE_STATUS] == MODIFY:
				if self._deviceInfo[BIDIRECTIONAL_CHANGED] == 1:
					success = cfDB.UpdateMultipleBidirectionalLinks(self._deviceInfo[DEV_SWITCH_FROM],"none",self._deviceInfo[DEV_PORT_FROM],porttype_from,"none","none",self._deviceInfo[DEV_BIDIRECTIONAL],first,commit)
					if commit:
						self._deviceInfo[BIDIRECTIONAL_CHANGED] = 0
				if self._deviceInfo[LINKTYPE_CHANGED] == 1:
					success = cfDB.UpdateMultipleLkTypeLinks(self._deviceInfo[DEV_SWITCH_FROM],"none",self._deviceInfo[DEV_PORT_FROM],"none",porttype_from,"none",self._deviceInfo[DEV_LINKTYPE],first,commit)
					if commit:
						self._deviceInfo[LINKTYPE_CHANGED] = 0 #Need to UpdateLinktype if we set diff colour on the linktypes
				if self._deviceInfo[LINKUSED_CHANGED] == 1:
					success = cfDB.UpdateMultipleLkUsedLinks(self._deviceInfo[DEV_SWITCH_FROM],"none",self._deviceInfo[DEV_PORT_FROM],"none",porttype_from,"none",self._deviceInfo[DEV_LINK_USED],first,commit) ###??
					if commit:
						self._deviceInfo[LINKUSED_CHANGED] = 0 #requres visWindow update?
						
				if self._deviceInfo[LINKINFO_CHANGED] == 1:
					success = cfDB.UpdateMultipleLkInfoLinks(self._deviceInfo[DEV_SWITCH_FROM],"none",self._deviceInfo[DEV_PORT_FROM],"none",porttype_from,"none",self._deviceInfo[DEV_LINKINFO],first,commit) ###??
					if commit:
						self._deviceInfo[LINKINFO_CHANGED] = 0 #requres visWindow update?


				# TODO: No update after modify??

			elif self._deviceInfo[SAVE_STATUS] == DELETE:
				success = cfDB.DeleteLinkRow(int(self._deviceInfo[DEV_LINK]),1)
				#Of course no update is needed when it is deleted...

			return True

		except ValueError,err:
			self.__errormessage = "An integer conversion failed. Cannot continue on the given link: " + str(self.GetName())
			return False
		except ValidationError,err:
			print "validation errror"
			self.__errormessage = str(err)
			return False
		except DBError,err:
			print "DB error"
			self.__errormesage = str(err)
			return False
		except RuntimeError,err:
			print "runtime error"
			self.__errormessage = str(err)
			return False

	def UpdateLinkID(self):
		""" Before we store a link in the ConfDB, the linkID/name is a temporary name
		that is automatically assigned; this has to be changed to the unique ID set to
		the link in the ConfDB. This method updates the linkID to what it was set to
		in the ConfDB.

		!return - False if an error occurs, True otherwise
		"""
		
		if self._deviceInfo[DEV_PORTTYPE_FROM] == "":
			porttype_from = "none"
		else:
			porttype_from = self._deviceInfo[DEV_PORTTYPE_FROM]
		if self._deviceInfo[DEV_PORTTYPE_TO] == "":
			porttype_to = "none"
		else:
			porttype_to = self._deviceInfo[DEV_PORTTYPE_TO]

		found = self.SetLinkIDByDeviceAndPort(self._deviceInfo[DEV_SWITCH_FROM],self._deviceInfo[DEV_PORT_FROM],porttype_from,self._deviceInfo[DEV_SWITCH_TO],self._deviceInfo[DEV_PORT_TO],porttype_to)
		if not found:
			return False #errormessage already set in SetLinkIDByDeviceAndPort
		self.Update(True) # Get most recent information about the link from the ConfDB.
		return True

	def SetLinkIDByDeviceAndPort(self,dev_from,port_from,porttype_from,dev_to,port_to,porttype_to):
		""" This is the method that actually finds the new link ID to a link that was just
		stored in the ConfDB, by retrieving information about the link that is connected to the
		devices and ports this link is connected to.

		Parameters:
		@dev_from - the device name where the link is connected to the OUT port
		@port_from - the port number of the device that the link is connected to on the from-device (OUT port)
		@porttype_from - the port type of the device that the link is connected to on the from-device (OUT port)
		@dev_to - the device name where the link is connected to the IN port
		@port_to - the port number of the device that the link is connected to on the to-device (IN port)
		@porttype_to - the port type of the device that the link is connected to on the to-deivce (IN port)

		!return - True if successful, False otherwise.
		"""

		try:
			devid_from = cfDB.GetDeviceID_devicename(dev_from)
			devid_to   = cfDB.GetDeviceID_devicename(dev_to)	

			linkidtmp = []
			linkidtmp.append(cfDB.GetMacroConnectivityRow_node(devid_from,port_from,2,porttype_from))
			linkidtmp.append(cfDB.GetMacroConnectivityRow_node(devid_to,port_to,1,porttype_to))

			linkidabs = []

			j = 0
			while j < 2:
				print str(linkidtmp[j])
				result = linkidtmp[j][1:]	#skip first '|' character
				linklist = result.split("|")	#split string in list

				i = 0
				for i in range(len(linklist)-1):
					tmp = linklist[i].split(":")		#split each property, name and datatype on left site, value on right side
					tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
					datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String, I - integer
					
					if datatype == 'I' and tmp0 == "linkid": #Found what we are looking for, the linkid
						linkidabs.append(int(tmp[1]))
						break

					i += 1

				j += 1

			if linkidabs[0] == linkidabs[1]: #Fine, same linkid for the given connection, set it
				self._deviceInfo[DEV_LINK] = str(linkidabs[0]) #set as string
				return True
			else:
				raise RuntimeError,"An unexpected error occured, couldn't retrieve correct LinkID"

		except RuntimeError,err:
			self.__errormessage = str(err)
			return False

	def Delete(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		
		self._deviceInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self)

	def Modify(self):
		""" Sets the save status of this object to modify, and adds
		it to the dirty objects list for later update in the ConfDB.
		Modify status is set in respective Set-methods.
		"""
		self._deviceInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
		""" Sets the save status of this object to create, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		self.SetName(self.GetNodeFrom()+":"+str(self.GetPortFrom())+":"+self.GetNodeTo()+":"+str(self.GetPortTo()))
		self._deviceInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)

class LinkType(DBInfo):
	""" Create a link type, which is an abstract object. Common properties for physical links of
	a given type; f.ex. HV, gas, data etc.
	"""

	def __init__(self,systemName,linkTypeName="",new=True):
		""" Constructor.

		Parameters:
		@systemname - name of the subsystem this linktype is created in (Obsolete). Now
		all link types are available in all subsystems, because there are not that many of them.
		@linkTypeName - the unique link type name set by the user
		@new - whether the link type is being created (True) or that it has already been created (False)
		"""

		self._deviceInfo = {}
		self._deviceInfo[LINK_NAME] = linkTypeName
		self._deviceInfo[LINK_TYPE] = -1 #ID
		self._deviceInfo[DEV_SYSTEM] = systemName
		self._deviceInfo[DEV_COLOUR] = "(0, 0, 0)" #wxColour(0,0,0)
		self._deviceInfo[OLD_NAME] = "" #previous name if renamed

		self._deviceInfo[COMPOSITE_LINKS] = "" #set to "" if this is not a composite linktype
							#if it composite this variable contains the commaseparated linktypes
		self._deviceInfo[COMPLINKS_CHANGED] = 0

		self._deviceInfo[SAVE_STATUS] = NO_STATUS

		self.__cfg = GetConfig()

		if new == False:
			self.Update()

		self.__errormessage = ""

	### GET ###
	def GetOldName(self):
		return self._deviceInfo[OLD_NAME]
	def GetSaveStatus(self):
		return self._deviceInfo[SAVE_STATUS] #set INSERT, DELETE and UPDATE as constants
	def GetName(self):
		return self._deviceInfo[LINK_NAME]
	def GetSystem(self):
		return self._deviceInfo[DEV_SYSTEM]
	def GetID(self):
		return self._deviceInfo[LINK_TYPE]
	def GetCompositeLinks(self):
		return self._deviceInfo[COMPOSITE_LINKS]
	def GetErrorMessage(self):
		return self.__errormessage
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	def SetOldName(self,oldname):
		self._deviceInfo[OLD_NAME] = oldname
	def SetSaveStatus(self,status):
		self._deviceInfo[SAVE_STATUS] = status
	def SetName(self,name):
		Validation.ValidateString("link type name",name,100,False)
		self._deviceInfo[LINK_NAME] = name
	def SetSystem(self,name):
		Validation.ValidateSystem("system name",name)
	def SetID(self,id):
		Validation.ValidateNumber("link type id",id)
		self._deviceInfo[LINK_TYPE] = id
	def SetCompositeLinks(self,concanlist):
		self._deviceInfo[COMPLINKS_CHANGED] = 1
		self._deviceInfo[COMPOSITE_LINKS] = concanlist #only set if composite linktype, else it is ""
	def SetDict(self,newdict):
		#self._deviceInfo = copy.deepcopy(newdict)
		for entry in newdict:
			self._deviceInfo[entry] = newdict[entry]

	def GetObjectInfo(self,doNotLookUp=True):
		"""Get all info about a link type.
                
		TODO: Here we can have checks whether we need to look for link type information in
		dirty objects list or ConfDB if this is the most recent version of the link type
		object.
		
	        @doNotLookUp - whether we should look for information about this object in dirty objects
		list and ConfDB (False) or not(True).
        	"""
		if doNotLookUp == True:
			return self._deviceInfo
		else:
			return self.Update()

	def Update(self,doNotCheckInDirtyList=False):
		""" Update the information/parameters for this link with the most recent information,
		and also return the member variable dictionary.

		Parameters:
		@doNotCheckInDirtyList - if set to True, we do not check in the dirty objects list for
		a version of the object.

		!return - the member variable dictionary of parameters for the link type
		"""

		# Dirty Objects list
		if not doNotCheckInDirtyList:
			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				tmp_obj = GetAppWin().GetDirtyObject(self._deviceInfo[LINK_NAME],LinkType)
				if tmp_obj != False:
					self._deviceInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self._deviceInfo
		# ConfDB
	        try:
		    Validation.ValidateString("Linktype name",self._deviceInfo[LINK_NAME],100)
		    result = cfDB.GetLkTypeRow_lkname(self._deviceInfo[LINK_NAME])

		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string in list

		    i = 0
		    deviceInfo = {DEV_OBJECT : obj_LINK}	#To identify this as a link...maybe change to linktype?
		    deviceInfo[DEV_SYSTEM] = self._deviceInfo[DEV_SYSTEM]	#should be retrieved from db? YES..dangerous
		    deviceInfo[DEV_COLOUR] = "(0, 255, 0)" #wxColour(0,0,0) #default: black
		    for i in range(len(infolist)-1):
			tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
			tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
			datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
			#print "Datatypes: " + datatype
			if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
			elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()

			deviceInfo[tmp0] = tmp[1]
			i += 1

		    self._deviceInfo = deviceInfo			#same name for indexes in dictionary and name on cols in database
		    return self._deviceInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "Did not find the LinkType in the database: " + str(self._deviceInfo[LINK_NAME])
			return {} #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error
	    
	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""

		save_status = self._deviceInfo[SAVE_STATUS]
		
		if save_status == CREATE:
			if self._deviceInfo[COMPOSITE_LINKS] == "":
				if insertmany:
				    	return 'cfDB.InsertMultipleSimpleLinkTypes("%s",%i,%i)' \
				    	%(str(self._deviceInfo[LINK_NAME]),first,commit)
				else:
				    	return 'cfDB.InsertSimpleLinkType("%s",%i)' \
				    	%(str(self._deviceInfo[LINK_NAME]),commit)
			else:
				if insertmany:
					return 'cfDB.InsertMultipleCompositeLinkTypes("%s","%s",%i,%i)' \
					%(str(self._deviceInfo[LINK_NAME]),str(self._deviceInfo[COMPOSITE_LINKS]),first,commit)
				else:
					return 'cfDB.InsertCompositeLinkType("%s","%s",%i)' \
					%(str(self._deviceInfo[LINK_NAME]),str(self._deviceInfo[COMPOSITE_LINKS]),commit)
		
		elif save_status == MODIFY:
			if self._deviceInfo[COMPLINKS_CHANGED] == 1:
		    		if string.find(self._deviceInfo[COMPOSITE_LINKS],",",0) == -1:
					self._deviceInfo[COMPOSITE_LINKS] = "none" #no longer a composite linktype
					return 'cfDB.UpdateMultipleCompositeLinkTypes("%s","%s",%i,%i)' \
					%(str(self._deviceInfo[LINK_NAME]),str(self._deviceInfo[COMPOSITE_LINKS]),first,last)
	
		elif save_status == DELETE:
			return "ERROR: Requested to delete LinkType; not possible"

		elif save_status == RENAME:
			return 'cfDB.UpdateMultipleLinkTypeNames("%s","%s",%i,%i)' \
			%(str(self._deviceInfo[OLD_NAME]),str(self._deviceInfo[LINK_NAME]),first,commit)
	
		else: #invalid status
			return ""

	def Save(self,first=False,insertmany=False,commit=True):
		""" Update changes to the ConfDB for this object.

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""

		try:
			if self._deviceInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE,RENAME]:
				raise RuntimeError,"Wrong save status set, this linktype cannot be saved in database: " + str(self.GetName())

			if self._deviceInfo[SAVE_STATUS] == CREATE or self._deviceInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateSystem("Linktype",self._deviceInfo[DEV_SYSTEM])
				Validation.ValidateString("Linkname",self._deviceInfo[LINK_NAME],100)

			if self._deviceInfo[SAVE_STATUS] == MODIFY or self._deviceInfo[SAVE_STATUS] == DELETE:
				Validation.ValidateNumber("Linktype ID",int(self._deviceInfo[LINK_TYPE]))

			if self._deviceInfo[SAVE_STATUS] == CREATE:
				if self._deviceInfo[COMPOSITE_LINKS] == "":
					if insertmany:
					    success = cfDB.InsertMultipleSimpleLinkTypes(self._deviceInfo[LINK_NAME],first,commit)
					else:
					    success = cfDB.InsertSimpleLinkType(self._deviceInfo[LINK_NAME],commit)
				else:
					if insertmany:
						success = cfDB.InsertMultipleCompositeLinkTypes(self._deviceInfo[LINK_NAME],self._deviceInfo[COMPOSITE_LINKS],first,commit)
					else:
						success = cfDB.InsertCompositeLinkType(self._deviceInfo[LINK_NAME],self._deviceInfo[COMPOSITE_LINKS],commit)

				self.Update(True) #update with data from database

			elif self._deviceInfo[SAVE_STATUS] == MODIFY:
				if self._deviceInfo[COMPLINKS_CHANGED] == 1:
		    			if string.find(self._deviceInfo[COMPOSITE_LINKS],",",0) == -1:
						self._deviceInfo[COMPOSITE_LINKS] = "none" #no longer a composite linktype
					
					success = cfDB.UpdateMultipleCompositeLinkTypes(self._deviceInfo[LINK_NAME],self._deviceInfo[COMPOSITE_LINKS],first,last)
			
			elif self._deviceInfo[SAVE_STATUS] == DELETE:
				GetAppWin().ShowError("It is not possible to delete a linktype once it is stored in ConfDB!",ERR_ERROR)
			elif self._deviceInfo[SAVE_STATUS] == RENAME:
				success = cfDB.UpdateMultipleLinkTypeNames(self._deviceInfo[OLD_NAME],self._deviceInfo[LINK_NAME],first,commit)
				
			return True
		
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    self.__errormessage = str(err)
		    return False #on error
		except ValueError,err:
		    self.__errormessage = str("Linktype ID is not an integer.")
		    return False

	def Delete(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		
		self._deviceInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self)

	def Modify(self):
		""" Sets the save status of this object to modify, and adds
		it to the dirty objects list for later update in the ConfDB.
		Modify status is set in respective Set-methods.
		"""
		self._deviceInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""

		self._deviceInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)

class Port(DBInfo):
	""" A port is a physical part of a device; f.ex. a port on a switch. So this class contains
	properties (member variables) and methods for a port instance.
	"""

	def __init__(self,systemname,portnbr="",porttype="",portway=PORT_IN,devname="",new=True):
		""" Constructor.

		Parameters:
		@systemname - the name of the subsystem a port is in (actually the subsystem the device
		it is a part of is in). Obsolete and is no longer necessary.
		@portnbr - the port number, a string and usually an integer, not unique alone.
		@porttype -  the port type, a string, not unique alone.
		@portway - the port way (direction, in or out). It is unique together with port number 
		and port type it for a device.
		@devname - the device name, of which this port belongs to. Together with port number,
		port type and port way it is unique in a subsystem.
		"""

		self.__portInfo = {}
		self.__portInfo[DEV_SYSTEM] = systemname
		self.__portInfo[DEV_PORT] = -1
		self.__portInfo[DEV_PORTNBR] = portnbr
		self.__portInfo[DEV_PORTTYPE] = porttype
		self.__portInfo[DEV_PORTWAY] = portway #1 means this is input (IN), 2 means this is output (OUT)
		self.__portInfo[DEV_NAME] = devname #devicename, must be set to id of device when update from db

		self.__portInfo[PORT_BIA] = ""
		self.__portInfo[PORT_MAC] = ""
		self.__portInfo[PORT_SPEED] = -1
		self.__portInfo[PORT_PHY] = ""
		self.__portInfo[PORT_PXI] = -1
		self.__portInfo[PORT_IP] = ""
		self.__portInfo[PORT_ADM] = -1
		self.__portInfo[PORT_IPNAME] = ""
		self.__portInfo[PORT_SUBNET] = ""

		self.__portInfo[SAVE_STATUS] = NO_STATUS
		self.__portInfo[MODIFY_STATUS] = NO_STATUS
		self.__portInfo[OLD_IP] = ""

		self.__cfg = GetConfig()

		if new == False:
			self.Update()

		self.__errormessage = ""
		
	def GetName(self):
		""" Since a port does not have a unique name, but a collection of several properties
		together; we create a port name with the following syntax/format:
		DEVICE_NAME: PORT NUMBER : PORT TYPE : IN/OUT
		"""
	
		if self.__portInfo[DEV_PORTWAY] == 1:
			way = "IN"
		else: #2
			way = "OUT"
			
		tmp_name = self.__portInfo[DEV_NAME] + ": " + str(self.__portInfo[DEV_PORTNBR]) + " : " + self.__portInfo[DEV_PORTTYPE] + " : " + str(way)
		
		return tmp_name #DEVID:PORTNBR:PORTTYPE:PORTWAY
	
	def GetSystem(self):
		return self.__portInfo[DEV_SYSTEM]
	def SetSystem(self,system):
		Validation.ValidateSystem("System: Port",system)
		self.__portInfo[DEV_SYSTEM] = system
	def GetDevice(self):
		return self.__portInfo[DEV_NAME]
	def SetDevice(self,devname):
		self.__portInfo[DEV_NAME] = devname
	def GetPortID(self):	
		return self.__portInfo[DEV_PORT]
	def SetPortID(self,portid):
		print "Portid: " + str(portid)
		Validation.ValidateNumber("PortID",portid,-1)
		self.__portInfo[DEV_PORT] = portid
	def GetPortNbr(self):
		return self.__portInfo[DEV_PORTNBR]
	def SetPortNbr(self,portnbr):
		Validation.ValidateString("PortNbr",portnbr,50)
		self.__portInfo[DEV_PORTNBR] = portnbr
	def GetPortType(self):
		return self.__portInfo[DEV_PORTTYPE]
	def SetPortType(self,porttype):
		Validation.ValidateString("Porttype",porttype,100,True)
		self.__portInfo[DEV_PORTTYPE] = porttype
	def GetPortWay(self):
		return self.__portInfo[DEV_PORTWAY]#1 means this is input, 2 means this is output
	def SetPortWay(self,portway):
		Validation.ValidateNumber("Portway",portway,1,2)
		self.__portInfo[DEV_PORTWAY] = portway #1 means this is input, 2 means this is output
	def GetBia(self):
		return self.__portInfo[PORT_BIA]
	def SetBia(self,bia):
		Validation.ValidateString("Bia",bia,18,True)
		self.__portInfo[PORT_BIA] = bia
	def GetMAC(self):
		return self.__portInfo[PORT_MAC]
	def SetMAC(self,mac):
        	#Validation.ValidateString("MACAddress",macaddress,17,False,"r'^([0-9a-fA-F]{2,2}:){5,5}[0-9a-fA-F]{1,1}$'")
		Validation.ValidateString("MAC Adress",mac,18,True)
		self.__portInfo[PORT_MAC] = mac
	def GetPortSpeed(self):
		return self.__portInfo[PORT_SPEED]
	def SetPortSpeed(self,speed):
		Validation.ValidateNumber("Port Speed",speed,0)
		self.__portInfo[PORT_SPEED] = speed
	def GetPhy(self):
		return self.__portInfo[PORT_PHY]
	def SetPhy(self,phy):
		Validation.ValidateString("Phy",phy,3,True)
		self.__portInfo[PORT_PHY] = phy
	def GetPXIBooting(self):
		return self.__portInfo[PORT_PXI]
	def SetPXIBooting(self,pxi):
		Validation.ValidateNumber("PXI Booting",pxi,0,1)
		self.__portInfo[PORT_PXI] = pxi
	def GetIP(self):
		return self.__portInfo[PORT_IP]
	def SetIP(self,ip):
		Validation.ValidateString("IP",ip,15,True)
		self.__portInfo[PORT_IP] = ip
	def SetOldIP(self,ip):
		self.__portInfo[OLD_IP] = ip
	def GetOldIP(self):
		return self.__portInfo[OLD_IP]
	def GetAdmStatus(self):
		return self.__portInfo[PORT_ADM]
	def SetAdmStatus(self,admstatus):
		Validation.ValidateNumber("Administrative Status",admstatus,0,1)
		self.__portInfo[PORT_ADM] = admstatus
	def GetIPName(self):
		return self.__portInfo[PORT_IPNAME]
	def SetIPName(self,ipname):
		Validation.ValidateString("IP Name",ipname,100,True)
		self.__portInfo[PORT_IPNAME] = ipname
	def GetSubnet(self):
		return self.__portInfo[PORT_SUBNET]
	def SetSubnet(self,subnet):
		Validation.ValidateString("Sub net",subnet,15,True)
		self.__portInfo[PORT_SUBNET] = subnet

	def GetSaveStatus(self):
		return self.__portInfo[SAVE_STATUS]
	def SetSaveStatus(self,status):
		self.__portInfo[SAVE_STATUS] = status
	def GetModifyStatus(self):
		return self.__portInfo[MODIFY_STATUS]
	def SetModifyStatus(self,status):
		self.__portInfo[MODIFY_STATUS] = status #0 - normal, 1 - MACIP, 2 - IP

	def GetErrorMessage(self):
		return self.__errormessage
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	def SetDict(self,newdict):
		#self.__portInfo = copy.deepcopy(newdict)
		for entry in newdict:
			self.__portInfo[entry] = newdict[entry]

	def Delete(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		
		self.__portInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self)

	def Modify(self):
		""" Sets the save status of this object to modify, and adds
		it to the dirty objects list for later update in the ConfDB.
		The modify status is set in the respective Set-methods.
		"""
		self.__portInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""

		self.__portInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)

	def GetObjectInfo(self,doNotLookUp=True):
		"""Get all info about a port.
                
		TODO: Here we can have checks whether we need to look for port information in
		dirty objects list or ConfDB if this is the most recent version of the port
		object.
		
	        @doNotLookUp - whether we should look for information about this object in dirty objects
		list and ConfDB (False) or not(True).
        	"""
		
		if doNotLookUp == True:
			return self.__portInfo
		else:
			return self.Update()

	def Update(self,doNotCheckInDirtyList=False):
		""" Update the information/parameters for this link with the most recent information,
		and also return the member variable dictionary.

		Parameters:
		@doNotCheckInDirtyList - if set to True, we do not check in the dirty objects list for
		a version of the object.

		!return - the member variable dictionary of parameters for the link type
		"""

		# Dirty Objects List
		if not doNotCheckInDirtyList:
			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				tmp_obj = GetAppWin().GetDirtyObject(self.GetName(),Port)
				if tmp_obj != False:
					self.__portInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self.__portInfo
				
		# ConfDB
	        try:
		    Validation.ValidateString("Device Name",self.__portInfo[DEV_NAME])
		    Validation.ValidateString("PortNbr",self.__portInfo[DEV_PORTNBR],50)
		    Validation.ValidateString("Porttype",self.__portInfo[DEV_PORTTYPE],100,True)
		    Validation.ValidateNumber("Portway",self.__portInfo[DEV_PORTWAY],1,2)
		    
		    if self.__portInfo[DEV_PORTTYPE].lower() == "":
			self.__portInfo[DEV_PORTTYPE] = "none"

		    devid = cfDB.GetDeviceID_devicename(self.__portInfo[DEV_NAME]) #devname -> devid
		    result = cfDB.GetPortRow_devid(devid,self.__portInfo[DEV_PORTNBR],self.__portInfo[DEV_PORTWAY],self.__portInfo[DEV_PORTTYPE])
                    #print "port " +str(result)
		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string in list

		    i = 0
		    portInfo = {}
		    portInfo[DEV_SYSTEM] = self.__portInfo[DEV_SYSTEM]	#should be retrieved from db? YES..dangerous
		    portInfo[PORT_IPNAME] = ""
		    portInfo[PORT_SUBNET] = ""
		    portInfo[SAVE_STATUS] = -1
		    for i in range(len(infolist)-1):
			tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
			tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
			datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
			#print "Datatypes: " + datatype
			#print "tmp " +str(tmp[1])
			if len(tmp)==2 :
                            if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
                            elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()
			    portInfo[tmp0] = tmp[1]
			else:
                            tmp.remove(tmp[0])
                            collect=":".join(tmp)
                            if datatype == 'I': #integer
				collect = int(collect)
                            elif datatype == 'C': #string
				collect= collect.strip()
                            portInfo[tmp0] = collect
			
			i += 1

		    if portInfo[PORT_PHY].lower() == "none":
			portInfo[PORT_PHY] = ""
		    if portInfo[DEV_PORTTYPE].lower() == "none":
			portInfo[DEV_PORTTYPE] = ""

		    #We have a port with ipname and subnet
		    if portInfo[PORT_IP] != "" or portInfo[PORT_IP].strip().lower() != "none": 
			ipinfo = self.UpdateIPInfo(portInfo[PORT_IP].strip())
			if ipinfo != False: #if ipinfo==false; error, but ignore
				for col in ipinfo:
					portInfo[col] = ipinfo[col]

		    self.__portInfo = portInfo		#same name for indexes in dictionary and name on cols in database

		    #change name of devname in case it has been renamed since last save
		    if self.__portInfo[DEV_NAME] in GetAppWin().GetOldNames():
			self.__portInfo[DEV_NAME] = GetAppWin().GetNewNamesOfRenamed()[self.__portInfo[DEV_NAME]] #set to the new name of the dev

		    return self.__portInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "Did not find the Port in the database: " + str(self.GetName())
			return {} #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error

	def UpdateByPortID(self):
		""" If we have the portID for a port object; we can update the information for this
		object with info from the ConfDB usign the portID.

		!return - the port member variable dictionary
		"""

	        try:
		    Validation.ValidateNumber("PortID",self.__portInfo[DEV_PORT],0)
	
		    result = cfDB.GetPortRow_pid(self.__portInfo[DEV_PORT])

		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string in list

		    i = 0
		    portInfo = {}
		    portInfo[DEV_SYSTEM] = GetAppWin().GetActiveSystem() #should be retrieved from db? YES..dangerous
		    portInfo[PORT_IPNAME] = "" #set to empty string because not retrieved from db
		    portInfo[PORT_SUBNET] = ""
		    portInfo[SAVE_STATUS] = -1
		    for i in range(len(infolist)-1):
			tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
			tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
			datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
			if len(tmp)==2 :
                            if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
                            elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()
			    portInfo[tmp0] = tmp[1]
			else:
                            tmp.remove(tmp[0])
                            collect=":".join(tmp)
                            if datatype == 'I': #integer
				collect = int(collect)
                            elif datatype == 'C': #string
				collect= collect.strip()
                            portInfo[tmp0] = collect
			i += 1

		    if portInfo[PORT_PHY].lower() == "none":
			portInfo[PORT_PHY] = ""
		    if portInfo[DEV_PORTTYPE].lower() == "none":
			portInfo[DEV_PORTTYPE] = ""
		
		    # Port with IP name and subnet
		    if portInfo[PORT_IP] != "" or portInfo[PORT_IP].strip().lower() != "none":
			ipinfo = self.UpdateIPInfo(portInfo[PORT_IP].strip())
			if ipinfo != False: #if false, error, but ignore
				for col in ipinfo:
					portInfo[col] = ipinfo[col]

		    self.__portInfo = portInfo		#same name for indexes in dictionary and name on cols in database

		    #change name of devname in case it has been renamed since last save
		    if self.__portInfo[DEV_NAME] in GetAppWin().GetOldNames():
			self.__portInfo[DEV_NAME] = GetAppWin().GetNewNamesOfRenamed()[self.__portInfo[DEV_NAME]] #set to the new name of the dev
		    #print "info: " + str(self.__portInfo)

		    return self.__portInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "Did not find the Port in the database: " + str(self.GetName())
			return {} #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error

	def UpdateIPInfo(self,ip):
		""" Update IP address and related network information for this port. This
		is not included in the other Update(...) method because most devices and
		ports does not have this network information.

		Parameters:
		@ip - the IP address (unique) of the port to be updated with info from ConfDB.
		"""
		
		try:
			result = cfDB.GetIPInfoRow(ip)
			result = result[1:]
			infolist = result.split("|")
			i = 0
			portInfo = {}
			for i in range(len(infolist)-1):
				tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
				tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
				datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
				#print "Datatypes: " + datatype
				if datatype == 'I': #integer
					tmp[1] = int(tmp[1])
				elif datatype == 'C': #string
					tmp[1] = tmp[1].strip()
	
				portInfo[tmp0] = tmp[1]
				i += 1

			return portInfo

	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "Did not find the Port in the database: " + str(self.GetName())
			return {} #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error
	    
	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""

		save_status = self.__portInfo[SAVE_STATUS]
		
		if save_status == CREATE:
			tmp_string = "intvector = IntVec()\n\
intvector.append(%i)\n\
intvector.append(%i)\n\
intvector.append(%i)\n" \
			%(self.__portInfo[PORT_SPEED],self.__portInfo[PORT_ADM],self.__portInfo[PORT_PXI])
				
			if insertmany:
				return tmp_string + 'cfDB.InsertMultiplePorts("%s","%s",%i,"%s","%s","%s","%s","%s","%s",%s,"%s",%i,%i)' \
				%(str(self.__portInfo[DEV_NAME]),str(self.__portInfo[DEV_PORTNBR]),self.__portInfo[DEV_PORTWAY],str(self.__portInfo[DEV_PORTTYPE]),str(self.__portInfo[PORT_BIA]),str(self.__portInfo[PORT_IPNAME]),str(self.__portInfo[PORT_IP]),str(self.__portInfo[PORT_SUBNET]),str(self.__portInfo[PORT_MAC]),"intvector",str(self.__portInfo[PORT_PHY]),first,commit)
			else:
				return tmp_string + 'cfDB.InsertMultiplePorts("%s","%s",%i,"%s","%s","%s","%s","%s","%s",%s,"%s",%i)' \
				%(str(self.__portInfo[DEV_NAME]),str(self.__portInfo[DEV_PORTNBR]),self.__portInfo[DEV_PORTWAY],str(self.__portInfo[DEV_PORTTYPE]),str(self.__portInfo[PORT_BIA]),str(self.__portInfo[PORT_IPNAME]),str(self.__portInfo[PORT_IP]),str(self.__portInfo[PORT_SUBNET]),str(self.__portInfo[PORT_MAC]),"intvector",str(self.__portInfo[PORT_PHY]),commit)

		elif save_status == MODIFY:
			if self.__portInfo[MODIFY_STATUS] == 0: #0 - normal, 1 - MACIP, 2 - IP
				#Also IP and MAC update available
				return 'cfDB.UpdateMultiplePorts("%s","%s",%i,"%s",%i,"%s",%i,%i,%i)' \
				%(str(self.__portInfo[DEV_NAME]),str(self.__portInfo[DEV_PORTNBR]),self.__portInfo[DEV_PORTWAY],str(self.__portInfo[DEV_PORTTYPE]),self.__portInfo[PORT_SPEED],str(self.__portInfo[PORT_PHY]),self.__portInfo[PORT_PXI],first,commit)

			elif self.__portInfo[MODIFY_STATUS] == 1: #MACIP
				return 'cfDB.UpdateMultipleAttributeMacIPs("%s","%s","%s",%i,%i)' \
				%(str(self.__portInfo[PORT_IP]),str(self.__portInfo[PORT_SUBNET]),str(self.__portInfo[PORT_IPNAME]),first,commit)
			elif self.__portInfo[MODIFY_STATUS] == 2: #IP
				return 'cfDB.UpdateMultipleIPAddresses("%s","%s",%i,%i)' \
				%(str(self.__portInfo[PORT_IP]),str(self.__portInfo[OLD_IP]),first,commit)

		elif save_status == DELETE:
			return 'cfDB.DeletePortRow(%i)' \
			%(self.__portInfo[DEV_PORT])

		else: #invalid status
			return ""
			
	def Save(self,first=False,insertmany=False,commit=True):
		""" Update changes to the ConfDB for this object.

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""
		try:
			
			if self.__portInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE]:
				raise RuntimeError,"Wrong save status set, this port cannot be saved in database: " + str(self.GetName())

			if self.__portInfo[SAVE_STATUS] == CREATE:
				Validation.ValidateString("Bia",self.__portInfo[PORT_BIA],18,True)
        			#Validation.ValidateString("MACAddress",macaddress,17,False,"r'^([0-9a-fA-F]{2,2}:){5,5}[0-9a-fA-F]{1,1}$'")
				Validation.ValidateString("MAC Adress",self.__portInfo[PORT_MAC],18,True)
				Validation.ValidateString("IP",self.__portInfo[PORT_IP],15,True)
				Validation.ValidateNumber("Administrative Status",self.__portInfo[PORT_ADM],0,1)
				Validation.ValidateString("IP Name",self.__portInfo[PORT_IPNAME],100,True)
				Validation.ValidateString("Sub net",self.__portInfo[PORT_SUBNET],15,True)
			
			if self.__portInfo[SAVE_STATUS] == CREATE or self.__portInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateString("Device Name",self.__portInfo[DEV_NAME],200)
				Validation.ValidateString("PortNbr",self.__portInfo[DEV_PORTNBR],50)
				Validation.ValidateString("Porttype",self.__portInfo[DEV_PORTTYPE],100,True)
				Validation.ValidateNumber("Portway",self.__portInfo[DEV_PORTWAY],1,2)
				
				Validation.ValidateNumber("Port Speed",self.__portInfo[PORT_SPEED],0)
				Validation.ValidateString("Phy",self.__portInfo[PORT_PHY],3,True)
				Validation.ValidateNumber("PXI Booting",self.__portInfo[PORT_PXI],0,1)
				
			if self.__portInfo[SAVE_STATUS] == CREATE:
				# Needed to do it this way to create a real int vector that is supported
				# in C/C++. This IntVec() is imported from the confDBpython library.
				intvector = IntVec()
				intvector.append(self.__portInfo[PORT_SPEED])
				intvector.append(self.__portInfo[PORT_ADM])
				intvector.append(self.__portInfo[PORT_PXI])
				
				if insertmany:
					success = cfDB.InsertMultiplePorts(self.__portInfo[DEV_NAME],self.__portInfo[DEV_PORTNBR],self.__portInfo[DEV_PORTWAY],self.__portInfo[DEV_PORTTYPE],self.__portInfo[PORT_BIA],self.__portInfo[PORT_IPNAME],self.__portInfo[PORT_IP],self.__portInfo[PORT_SUBNET],self.__portInfo[PORT_MAC],intvector,self.__portInfo[PORT_PHY],first,commit)
				else:
					
					print (self.__portInfo[DEV_NAME],self.__portInfo[DEV_PORTNBR],self.__portInfo[DEV_PORTTYPE],self.__portInfo[DEV_PORTWAY],self.__portInfo[PORT_BIA],self.__portInfo[PORT_IPNAME],self.__portInfo[PORT_IP],self.__portInfo[PORT_SUBNET],self.__portInfo[PORT_MAC],intvector,self.__portInfo[PORT_PHY],commit)		
					success = cfDB.InsertPort(self.__portInfo[DEV_NAME],self.__portInfo[DEV_PORTNBR],self.__portInfo[DEV_PORTTYPE],self.__portInfo[DEV_PORTWAY],self.__portInfo[PORT_BIA],self.__portInfo[PORT_IPNAME],self.__portInfo[PORT_IP],self.__portInfo[PORT_SUBNET],self.__portInfo[PORT_MAC],intvector,self.__portInfo[PORT_PHY],commit)
					
				if commit:
					self.Update(True) #update with data from database (port ID set)

			elif self.__portInfo[SAVE_STATUS] == MODIFY: #modify
				if self.__portInfo[MODIFY_STATUS] == 0: #0 - normal, 1 - MACIP, 2 - IP
					#Also IP and MAC update available
					success = cfDB.UpdateMultiplePorts(self.__portInfo[DEV_NAME],self.__portInfo[DEV_PORTNBR],self.__portInfo[DEV_PORTWAY],self.__portInfo[DEV_PORTTYPE],self.__portInfo[PORT_SPEED],self.__portInfo[PORT_PHY],self.__portInfo[PORT_PXI],first,commit)

				elif self.__portInfo[MODIFY_STATUS] == 1: #MACIP
					success = cfDB.UpdateMultipleAttributeMacIPs(self.__portInfo[PORT_IP],self.__portInfo[PORT_SUBNET],self.__portInfo[PORT_IPNAME],first,commit)
				elif self.__portInfo[MODIFY_STATUS] == 2: #IP
					success = cfDB.UpdateMultipleIPAddresses(self.__portInfo[PORT_IP],self.__portInfo[OLD_IP],first,commit)

				if commit:
					self.Update(True)

			elif self.__portInfo[SAVE_STATUS] == DELETE: #delete
				if DEV_PORT in self.__portInfo.keys() and self.__portInfo[DEV_PORT] != -1:
					success = cfDB.DeletePortRow(self.__portInfo[DEV_PORT])
				else:
					GetAppWin().ShowError("The Port cannot be deleted because it doesnt have a portID set yet.",ERR_ERROR)

			return True
		
	        except ValidationError,err:
		    print "validation error "
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    print "DB error"
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    print "run Time Error"
		    self.__errormessage = str(err)
		    return False #on error
		except ValueError,err:
		    self.__errormessage = str("Device Name is not a string.")
		    return False

	#added by Lana
class Spare_Device(DBInfo):
	"""Class for storing information about a spare device. A spare device is a physical device with no connections
	"""

	def __init__(self,hwtype,serialnb, hwname="",new=True):
		""" Constructor.

		Parameters:
		@hwname - the  name of the spare device, given by the user. like SPARE_HYBRUD_01 optional
		@hwtype - should be prefixed by the system if it is only meant for a specific subsystem
		@serialnb- serialnb 
		@new        - whether the device is about to be created, or is already created.
		True if it is being created, False if not. If it is already created, we will
		fill the object with goods. (names and values)
		"""
		
		self.__errormessage = ""

		self.__sparedeviceInfo = {}	#all device info that is common for all devices
		self.__sparedeviceInfo[DEV_OBJECT] = obj_NODE
		self.__sparedeviceInfo[SPARE_DEV_NAME] = hwname

		self.__sparedeviceInfo[DEV_SERIAL] = serialnb # serial number
		self.__sparedeviceInfo[DEV_HWTYPE] = "" # hardware type
		self.__sparedeviceInfo[DEV_RESPONSIBLE] = "" # responsible for the device
		self.__sparedeviceInfo[DEV_LOCATION] = "" # location string
		self.__sparedeviceInfo[DEV_COMMENTS] = "" # comments/description
		self.__sparedeviceInfo[SPARE_DEV_COLOUR] = "(255, 255, 255)" #wxColour(255,255,255)
		self.__sparedeviceInfo[SPARE_OLD_SERIAL] = "" # if spare device has been renamed new serial nb), we store the old/previous name here
		# We need to keep track of some parameters; whether they have been changed when the user modified the object
		# or not because we have different modify functions for different parameters.
		self.__sparedeviceInfo[MODIFY_STATUS] = NO_STATUS #0 - standard modify, 1 - change system list, NO_STATUS: not set

		self.__sparedeviceInfo[SAVE_STATUS] = NO_STATUS

		self.__spareports = [] # ports retrieved, of this device
		self.__cfg = GetConfig() # configuration file reference


		if new == False:
			self.Update()

	### GET ###
	def GetModifyStatus(self):
		return self.__sparedeviceInfo[MODIFY_STATUS]
	def GetComments(self):
		return self.__sparedeviceInfo[DEV_COMMENTS]
	def GetLocation(self):
		return self.__sparedeviceInfo[DEV_LOCATION]
	def GetResponsible(self):
		return self.__sparedeviceInfo[DEV_RESPONSIBLE]
	def GetHWType(self):
		return self.__sparedeviceInfo[DEV_HWTYPE]
	def GetSerial(self):
		return self.__sparedeviceInfo[DEV_SERIAL]
	def GetOldName(self):
		return self.__sparedeviceInfo[SERIAL_OLD_SERIAL]
	def GetSaveStatus(self):
		return self.__sparedeviceInfo[SAVE_STATUS] #set INSERT, DELETE and UPDATE as constants
	def GetHWName(self):
		return self.__sparedeviceInfo[SPARE_DEV_NAME]
	def GetColour(self):
		return self.__sparedeviceInfo[SPARE_DEV_COLOUR]
	def GetTheDict(self):
		return self.__sparedeviceInfo
	def GetErrorMessage(self):
		return self.__errormessage
	def GetName(self):
		return self.__sparedeviceInfo[DEV_SERIAL]
	
	### SET ###
	def SetModifyStatus(self,status):
		self.__sparedeviceInfo[MODIFY_STATUS] = status
	def SetComments(self,comments):
		Validation.ValidateString("User comments",comments,1000,True)
		self.__sparedeviceInfo[DEV_COMMENTS] = comments
	def SetLocation(self,location):
		Validation.ValidateString("Location",location,200,True)
		self.__sparedeviceInfo[DEV_LOCATION] = location
	def SetResponsible(self,responsible):
		Validation.ValidateString("Responsible",responsible,100,True)
		self.__sparedeviceInfo[DEV_RESPONSIBLE] = responsible
	def SetHWType(self,hwtype):
		Validation.ValidateString("Hardware type",hwtype,200,True)
		self.__sparedeviceInfo[DEV_HWTYPE] = hwtype
	def SetHWType(self,hwname):
		Validation.ValidateString("Hardware name",hwname,200,True)
		self.__sparedeviceInfo[SPARE_DEV_NAME] = hwname
	def SetSerial(self,serial):
		Validation.ValidateString("Serial number",serial,500)
		self.__sparedeviceInfo[DEV_SERIAL] = serialnb
	def SetOldName(self,oldname):
		self.__sparedeviceInfo[SPARE_OLD_SERIAL] = oldserial
	def SetSaveStatus(self,state):
		self.__sparedeviceInfo[SAVE_STATUS] = state
	def SetErrorMessage(self,msg):
		self.__errormessage = msg

	def SetDict(self,newdict):
		for entry in newdict:
			self.__sparedeviceInfo[entry] = newdict[entry]
		#self.__sparedeviceInfo = copy.deepcopy(newdict)

	def SetAllInfoDict(self,devinfo):
		""" Copy device info from one device to another (this one).
		"""
		my_serial = self.__sparedeviceInfo[DEV_SERIAL]
		self.__sparedeviceInfo = {}
		for item in devinfo:
			self.__sparedeviceInfo[item] = devinfo[item]
		self.__sparedeviceInfo[DEV_SERIAL] = my_serial
	

	def GetObjectInfo(self,doNotLookUp=True):
		""" Get all info about a device in a subsystem, given the unique name of a device.
		
		Here we do some check where to get the properies information from, f.ex. whether
		this device object is the most recent object in dirty list; if so we can just return
		this objects dictionary. TODO.

		@doNotLookUp - whether we should look for information about this object in dirty objects
		list and ConfDB (False) or not(True).
        	"""
		if doNotLookUp == True:
			return self.__sparedeviceInfo
		else:
			self.Update()
			
		return self.__sparedeviceInfo

	def Update(self,doNotCheckInDirtyList=False):
		""" Here we update the device with the most recent information about the device witht the
		given information; either in dirty objects list (if found and in creation mode), or from
		ConfDB.

		@doNotCheckInDirtyList - if True, we do not check in dirty objects list, but go straight
		forward to the ConfDB and look for information about this device there.
		"""
	
		# Dirty Objects List
		if not doNotCheckInDirtyList:
			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				tmp_obj = GetAppWin().GetDirtyObject(self.__sparedeviceInfo[SPARE_DEV_NAME],Spare_Device)
				if tmp_obj != False:
					self.__sparedeviceInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self.__sparedeviceInfo

		# ConfDB
        	try:
		        Validation.ValidateString("Spare hw serial nb",self.__sparedeviceInfo[DEV_SERIAL],100)
	                result = cfDB.GetHWDeviceRow_serialnb (self.__sparedeviceInfo[DEV_SERIAL])
                        #print 'get spare '+str(result)
			result = result[1:]	#skip first | character
			infolist = result.split("|")	#split string in list
			i = 0
			sparedeviceInfo = {DEV_OBJECT : obj_NODE}	#To identify this as a devicenode (as opposite to a linknode)
			sparedeviceInfo[SPARE_DEV_COLOUR] = wxColour(red=255,green=255,blue=255) #changed to device type
			sparedeviceInfo[DEV_HWTYPE] = ""
			sparedeviceInfo[DEV_RESPONSIBLE] = ""
			sparedeviceInfo[DEV_COMMENTS] = ""
			sparedeviceInfo[MODIFY_STATUS] = NO_STATUS
		
			for i in range(len(infolist)-1):
				tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
				tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
				datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
				if datatype == 'I': #integer
					tmp[1] = int(tmp[1])
				elif datatype == 'C': #string
					tmp[1] = tmp[1].strip()
				sparedeviceInfo[tmp0] = tmp[1]
				i += 1

			self.__sparedeviceInfo = sparedeviceInfo

		
			
                	return self.__sparedeviceInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "No information found in the database on the given device: " + str(self.__sparedeviceInfo[DEV_SERIAL])
			return {}
		    else:
		    	self.__errormessage = str(err)
		        return False #on error



	


	def GetSparePorts(self):
		""" Get all ports assigned to a device as port objects in a list.
		Look for ports in both dirty objects list and ConfDB.

		!return - list of port objects for the ports of a device.
		"""

		# 1. Get all ports assigned to this device in dirty objects list; if in creation mode
		# 2. Get all portIDs assigned to this device from ConfDB; filter out those that were already found in dirty
		# objects list
		# 3. Retrieve all info for each port; from ConfDB or dirty objects list.
		# 4. Return the port objects in a list.

		try:
                        portids=[]
                        dirty_objs_ports=[]
                        tbd=[]
                        my_ports=[]
                        i=0
			Validation.ValidateString("Serial number",self.__sparedeviceInfo[DEV_SERIAL],500)
			portids = cfDB.GetPortInfoPerSpare(self.__sparedeviceInfo[DEV_SERIAL])
			#print 'portids ' +str(portids[0])
			if portids == False:
				raise RuntimeError,str(self.GetErrorMessage())
			tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
			for obj in tmp_dirtyobjs:
					if obj.__class__ == Spare_Port:
						if obj.GetSpareDevice() == self.GetName(): #port of this device
							if obj.GetSaveStatus() != DELETE:
								if obj.GetName() in dirty_objs_ports:
									i = 0
									for port in my_ports:
										if port.GetName() == obj.GetName():
											# Newer version found; replace
											my_ports.pop(i)
										i += 1
												
								my_ports.append(obj)
								dirty_objs_ports.append(obj.GetName())
							else: #found port that is set to deleted, look for identical in dl
								print "infodel: " + str(obj.GetObjectInfo(True))
								print "Deleted obj: " + str(obj.GetName())
								if obj.GetName() in dirty_objs_ports:
									# del port found, del port if found in dirty objs
									i = 0
									while i < len(my_ports):
										if my_ports[i].GetName() == obj.GetName():
											my_ports.pop(i)
											break
										i += 1
										
								tbd.append(obj.GetName())
			while i < len(portids):
				port_tmp = portids[i].split("|")
				tmp_port = Spare_Port(port_tmp[0],port_tmp[1],port_tmp[2],self.__sparedeviceInfo[DEV_SERIAL],True)
				#print "port_tmp "+ str(port_tmp[2])
				tmp_port.SetMAC(port_tmp[3])
				tmp_port.SetBia(port_tmp[4])
				#print "value = "+ tmp_port.GetBia()
				i += 1
				if tmp_port.GetSpareDevice() not in dirty_objs_ports and tmp_port.GetName() not in tbd: 
					#in case we have a newer version in the dirty objects list
					#or the port is set to be deleted in dl
					my_ports.append(tmp_port)


			    
						
						
			self.__spareports = my_ports
			return self.__spareports

	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			if portids == []:
				self.__errormessage = "No ports found for the selected device " + str(self.__sparedeviceInfo[DEV_SERIAL])
				return [] #none found
			else: #didnt find ports in db, but we found in dirty list
				self.__ports = portids
				return self.__ports
		    else:
			self.__errormessage = str(err)
		    return False #on error #NB! Change when error is fixed in DB moduel !!!!!


	    
	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""

		save_status = self.__sparedeviceInfo[SAVE_STATUS]
	
		if save_status == CREATE:
			if insertmany:
				return 'cfDB.InsertMultipleSpareDevices("%s","%s","%s","%s","%s","%s",%i,%i)' \
				%(str(self.__sparedeviceInfo[SPARE_DEV_NAME]),str(self.__sparedeviceInfo[DEV_HWTYPE]),str(self.__sparedeviceInfo[DEV_SERIAL]),str(self.__sparedeviceInfo[DEV_RESPONSIBLE]),str(self.__sparedeviceInfo[DEV_LOCATION]),str(self.__sparedeviceInfo[DEV_COMMENTS]),first,commit)
			else:
				return 'cfDB.InsertSpareDevice("%s","%s","%s","%s","%s","%s",%i)' \
				%(str(self.__sparedeviceInfo[SPARE_DEV_NAME]),str(self.__sparedeviceInfo[DEV_HWTYPE]),str(self.__sparedeviceInfo[DEV_SERIAL]),str(self.__sparedeviceInfo[DEV_RESPONSIBLE]),str(self.__sparedeviceInfo[DEV_LOCATION]),str(self.__sparedeviceInfo[DEV_COMMENTS]),commit)
		
		
		elif save_status == DELETE:
			return 'cfDB.DeleteHWDevice("%s")'\
			%(str(self.sparedeviceInfo[DEV_SERIAL]))

		elif save_status == RENAME:
			return 'cfDB.UpdateHWSerialNB ("%s","%s")' \
			%(str(self.__sparedeviceInfo[SPARE_OLD_SERIAL]),str(self.__sparedeviceInfo[DEV_SERIAL]))
	
		else: #invalid status
			return ""


	def Save(self,first=False,insertmany=False,commit=True):
		""" Update changes to the ConfDB for this object.

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""
		
		try:
			if self.__sparedeviceInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE,RENAME]:
				raise ValidationError,"This object should not be in the dirty list or has incorrect savestatus setting"

			if self.__sparedeviceInfo[SAVE_STATUS] == DELETE:
				GetAppWin().ShowError("A device cannot be deleted, only internally before stored in the database.",ERR_ERROR)
			if self.__sparedeviceInfo[SAVE_STATUS] == RENAME:
				Validation.ValidateString("Device name",self.__sparedeviceInfo[DEV_SERIAL],100)

			if self.__sparedeviceInfo[SAVE_STATUS] == CREATE or self.__sparedeviceInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateString("HW name",self.__sparedeviceInfo[SPARE_DEV_NAME],100)
				Validation.ValidateString("Serial number",self.__sparedeviceInfo[DEV_SERIAL],500)
				Validation.ValidateString("Hardware type",self.__sparedeviceInfo[DEV_HWTYPE],200,True)
				Validation.ValidateString("Responsible",self.__sparedeviceInfo[DEV_RESPONSIBLE],100,True)
				Validation.ValidateString("Location",self.__sparedeviceInfo[DEV_LOCATION],200,True)
				Validation.ValidateString("Comments",self.__sparedeviceInfo[DEV_COMMENTS],1000,True)

			if self.__sparedeviceInfo[SAVE_STATUS] == CREATE:
				if insertmany:
					success = cfDB.InsertMultipleSpareDevices(self.__sparedeviceInfo[SPARE_DEV_NAME],self.__sparedeviceInfo[DEV_HWTYPE],self.__sparedeviceInfo[DEV_SERIAL],self.__sparedeviceInfo[DEV_RESPONSIBLE],self.__sparedeviceInfo[DEV_LOCATION],self.__sparedeviceInfo[DEV_COMMENTS],first,commit)
				else:
					success = cfDB.InsertSpareDevice(self.__sparedeviceInfo[SPARE_DEV_NAME],self.__sparedeviceInfo[DEV_HWTYPE],self.__sparedeviceInfo[DEV_SERIAL],self.__sparedeviceInfo[DEV_RESPONSIBLE],self.__sparedeviceInfo[DEV_LOCATION],self.__sparedeviceInfo[DEV_COMMENTS],commit)
				if commit:
					self.Update(True)

				
			elif self._deviceInfo[SAVE_STATUS] == RENAME:
				success = cfDB.UpdateHWSerialNB(self.__sparedeviceInfo[SPARE_OLD_SERIAL],self.__sparedeviceInfo[DEV_SERIAL])
				if commit:
					self.Update(True)
				
			elif self._deviceInfo[SAVE_STATUS] == DELETE:
				success = cfDB.DeleteHWDevice(self.__sparedeviceInfo[DEV_SERIAL])
				if commit:
					self.Update(True)

			return True

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
			self.__errormessage = str(err)
			return False #on error

	def Delete(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		
		self.__sparedeviceInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self)

	def Modify(self,commit=1):
		"""Sets the save status of this object to modify, and
		adds it to the dirty objects list for later update in the ConfDB.
		The Modify status is set in the respective Set-functions for the
		parameters they change.
		"""
		self.__sparedeviceInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
        	""" Sets the save status of this object to create, and adds it
		to the dirty objects list for later update in the ConfDB.
        	"""
		
		self.__sparedeviceInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)
	
	
	



class Spare_Port(DBInfo):
	""" A port is a physical part of a device; f.ex. a port on a switch. So this class contains
	properties (member variables) and methods for a port of a spare device instance.
	"""

	def __init__(self,portnbr="",porttype="",portway=PORT_IN,serialnb="",new=True):
		""" Constructor.

		Parameters:
		@portnbr - the port number, a string and usually an integer, not unique alone.
		@porttype -  the port type, a string, not unique alone.
		@portway - the port way (direction, in or out). It is unique together with port number 
		and port type it for a device.
		@serialnb - serailn of the spare device, of which this port belongs to. Together with port number,
		port type and port way it is unique .
		"""

		self.__spareportInfo = {}
		self.__spareportInfo[DEV_PORTNBR] = portnbr
		self.__spareportInfo[DEV_PORTTYPE] = porttype
		self.__spareportInfo[DEV_PORTWAY] = portway #1 means this is input (IN), 2 means this is output (OUT)
		self.__spareportInfo[DEV_SERIAL] = serialnb #serialnb, must be set to id of device when update from db

		self.__spareportInfo[PORT_BIA] = ""
		self.__spareportInfo[PORT_MAC] = ""
	
		self.__spareportInfo[SAVE_STATUS] = NO_STATUS
		self.__spareportInfo[MODIFY_STATUS] = NO_STATUS

		self.__cfg = GetConfig()

		if new == False:
			self.Update()

		self.__errormessage = ""
		
	def GetName(self):
		""" Since a port does not have a unique name, but a collection of several properties
		together; we create a port name with the following syntax/format:
		SERIALNB: PORT NUMBER : PORT TYPE : IN/OUT
		"""
	
		if self.__spareportInfo[DEV_PORTWAY]==1 or self.__spareportInfo[DEV_PORTWAY]=='1' :
			way = "IN"
		else: #2
			way = "OUT"
		#print 'port way ' +str(self.__spareportInfo[DEV_PORTWAY])
		#print 'port way 2' + way
		tmp_name = self.__spareportInfo[DEV_SERIAL] + ": " + self.__spareportInfo[DEV_PORTNBR] + " : " + self.__spareportInfo[DEV_PORTTYPE] + " : " + str(way)
		
		return tmp_name #SERIAL:PORTNBR:PORTTYPE:PORTWAY
	
	def GetSpareDevice(self):
		return self.__spareportInfo[DEV_SERIAL]
	def SetSpareDevice(self,serial):
		self.__spareportInfo[DEV_SERIAL] = serial
	def GetPortNbr(self):
		return self.__spareportInfo[DEV_PORTNBR]
	def SetPortNbr(self,portnbr):
		Validation.ValidateString("PortNbr",portnbr,50)
		self.__spareportInfo[DEV_PORTNBR] = portnbr
	def GetPortType(self):
		return self.__spareportInfo[DEV_PORTTYPE]
	def SetPortType(self,porttype):
		Validation.ValidateString("Porttype",porttype,100,True)
		self.__spareportInfo[DEV_PORTTYPE] = porttype
	def GetPortWay(self):
		return self.__spareportInfo[DEV_PORTWAY]#1 means this is input, 2 means this is output
	def SetPortWay(self,portway):
		Validation.ValidateNumber("Portway",portway,1,2)
		self.__spareportInfo[DEV_PORTWAY] = portway #1 means this is input, 2 means this is output
	def GetBia(self):
		return self.__spareportInfo[PORT_BIA]
	def SetBia(self,bia):
		Validation.ValidateString("Bia",bia,18,True)
		self.__spareportInfo[PORT_BIA] = bia
	def GetMAC(self):
		return self.__spareportInfo[PORT_MAC]
	def SetMAC(self,mac):
        	#Validation.ValidateString("MACAddress",macaddress,17,False,"r'^([0-9a-fA-F]{2,2}:){5,5}[0-9a-fA-F]{1,1}$'")
		Validation.ValidateString("MAC Adress",mac,18,True)
		self.__spareportInfo[PORT_MAC] = mac
	def GetSaveStatus(self):
		return self.__spareportInfo[SAVE_STATUS]
	def SetSaveStatus(self,status):
		self.__spareportInfo[SAVE_STATUS] = status
	def GetModifyStatus(self):
		return self.__spareportInfo[MODIFY_STATUS]
	def SetModifyStatus(self,status):
		self.__spareportInfo[MODIFY_STATUS] = status #0 - normal, 1 - MACIP, 2 - IP

	def GetErrorMessage(self):
		return self.__errormessage
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	def SetDict(self,newdict):
		#self.__portInfo = copy.deepcopy(newdict)
		for entry in newdict:
			self.__spareportInfo[entry] = newdict[entry]

	def Delete(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""
		
		self.__spareportInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self)

	def Modify(self):
		""" Sets the save status of this object to modify, and adds
		it to the dirty objects list for later update in the ConfDB.
		The modify status is set in the respective Set-methods.
		"""
		self.__spareportInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
		""" Sets the save status of this object to delete, and adds
		it to the dirty objects list for later update in the ConfDB.
		"""

		self.__spareportInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)

	def GetObjectInfo(self,doNotLookUp=True):
		"""Get all info about a port.
                
		TODO: Here we can have checks whether we need to look for port information in
		dirty objects list or ConfDB if this is the most recent version of the port
		object.
		
	        @doNotLookUp - whether we should look for information about this object in dirty objects
		list and ConfDB (False) or not(True).
        	"""
		
		if doNotLookUp == True:
			return self.__spareportInfo
		else:
			return self.Update()

	def Update(self,doNotCheckInDirtyList=False):
		""" Update the information/parameters for this link with the most recent information,
		and also return the member variable dictionary.

		Parameters:
		@doNotCheckInDirtyList - if set to True, we do not check in the dirty objects list for
		a version of the object.

		!return - the member variable dictionary of parameters for the link type
		"""

		# Dirty Objects List
		if not doNotCheckInDirtyList:
			if GetAppWin().GetUserMode() == ID_CREATION_MODE:
				tmp_obj = GetAppWin().GetDirtyObject(self.GetName(),Spare_Port)
				if tmp_obj != False:
					self.__spareportInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self.__spareportInfo
				
		# ConfDB
	        try:
		    Validation.ValidateString("Serial nb",self.__spareportInfo[DEV_SERIAL])
		    Validation.ValidateString("PortNbr",self.__spareportInfo[DEV_PORTNBR],50)
		    Validation.ValidateString("Porttype",self.__spareportInfo[DEV_PORTTYPE],100,True)
		    Validation.ValidateNumber("Portway",self.__spareportInfo[DEV_PORTWAY],1,2)
		    
		    if self.__spareportInfo[DEV_PORTTYPE].lower() == "":
			self.__spareportInfo[DEV_PORTTYPE] = "none"
                    #print 'self.__spareportInfo[DEV_SERIAL]' + str(self.__spareportInfo[DEV_PORTNBR]) +'test'
                    #print 'self.__spareportInfo[DEV_SERIAL]' + str(self.__spareportInfo[DEV_PORTTYPE])+ 'test'
                    #print 'self.__spareportInfo[DEV_SERIAL]' + str(self.__spareportInfo[DEV_PORTWAY])+ 'test'
                    #print 'self.__spareportInfo[DEV_SERIAL]' + str(self.__spareportInfo[DEV_SERIAL])+ 'test'
                    
		    result = cfDB.GetSparePortRow_snb(str(self.__spareportInfo[DEV_SERIAL]),str(self.__spareportInfo[DEV_PORTNBR]),self.__spareportInfo[DEV_PORTWAY],str(self.__spareportInfo[DEV_PORTTYPE]))
                    #print 'getspare ' + str(result)
		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string in list

		    i = 0
		    portInfo = {}
		    portInfo[SAVE_STATUS] = -1
		    for i in range(len(infolist)-1):
                        tmp = infolist[i].split(":")		#split each property, name on left site, value on right side
                        tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
                        datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
                        
                        if len(tmp)==2 :
                            if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
                            elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()
			    portInfo[tmp0] = tmp[1]
			else:
                            tmp.remove(tmp[0])
                            collect=":".join(tmp)
                            if datatype == 'I': #integer
				collect = int(collect)
                            elif datatype == 'C': #string
				collect= collect.strip()
                            portInfo[tmp0] = collect
                        i += 1
                        
                            
                            
		    if portInfo[DEV_PORTTYPE].lower() == "none":
			portInfo[DEV_PORTTYPE] = ""

		   
		    self.__spareportInfo = portInfo		#same name for indexes in dictionary and name on cols in database

		   
		    return self.__spareportInfo
                                                                                                                             
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
			self.__errormessage = "Did not find the Port in the database: " + str(self.GetName())
			return {} #none found
		    else:
			self.__errormessage = str(err)
		    return False #on error


	    
	def PrintSaveFunction(self,first=False,insertmany=False,commit=True):
		""" A function that returns the string to the function that will be called in the
		ConfDBLibrary for the given save and modify status of this data object, as well as
		use of single or multiple insert.

		Parameters:
		Same as for Save(...)

		!return - returns a String with the function name as well as all set parameters
		"""

		save_status = self.__spareportInfo[SAVE_STATUS]
		
		if save_status == CREATE:
					
			if insertmany:
				return tmp_string + 'cfDB.InsertMultipleSparePorts("%s","%s",%i,"%s","%s","%s",%i,%i)' \
				%(str(self.__spareportInfo[DEV_SERIAL]),str(self.__spareportInfo[DEV_PORTNBR]),self.__spareportInfo[DEV_PORTWAY],str(self.__spareportInfo[DEV_PORTTYPE]),str(self.__spareportInfo[PORT_BIA]),str(self.__spareportInfo[PORT_MAC]),first,commit)
			else:
				return tmp_string + 'cfDB.InsertSparePort("%s","%s",%i,"%s","%s","%s",%i)' \
				%(str(self.__spareportInfo[DEV_SERIAL]),str(self.__spareportInfo[DEV_PORTNBR]),self.__spareportInfo[DEV_PORTWAY],str(self.__spareportInfo[DEV_PORTTYPE]),str(self.__spareportInfo[PORT_BIA]),str(self.__spareportInfo[PORT_MAC]),commit)

		else: #invalid status
			return ""

	def Save(self,first=False,insertmany=False,commit=True):
		""" Update changes to the ConfDB for this object.

		Parameters:
		@first - if first is True; it is the first kind of this db update; prepare and initialize cache,
		if first is False, add it to the already existing cache. cache is only used if insertmany = True.
		@insertmany - If insertmany is set to false, the single insert function will be used, 
		which is optimized for single inserts. If insertmany is set to true, the multiple 
		insert function will be used, which is optimized for multiple inserts (use of cache)
		@commit - if commit is set to false, no data will be commited, put are put on hold in the cache, if 
		commit is set to true, all data that are put on hold in the cache will be commited.
		"""

		try:
			if self.__spareportInfo[SAVE_STATUS] not in [CREATE,MODIFY,DELETE]:
				raise RuntimeError,"Wrong save status set, this port cannot be saved in database: " + str(self.GetName())

			if self.__spareportInfo[SAVE_STATUS] == CREATE:
				Validation.ValidateString("Bia",self.__spareportInfo[PORT_BIA],18,True)
        			#Validation.ValidateString("MACAddress",macaddress,17,False,"r'^([0-9a-fA-F]{2,2}:){5,5}[0-9a-fA-F]{1,1}$'")
				Validation.ValidateString("MAC Adress",self.__spareportInfo[PORT_MAC],18,True)
			
			if self.__spareportInfo[SAVE_STATUS] == CREATE or self.__spareportInfo[SAVE_STATUS] == MODIFY:
				Validation.ValidateString("Serial nb",self.__spareportInfo[DEV_SERIAL],200)
				Validation.ValidateString("PortNbr",self.__spareportInfo[DEV_PORTNBR],50)
				Validation.ValidateString("Porttype",self.__spareportInfo[DEV_PORTTYPE],100,True)
				Validation.ValidateNumber("Portway",self.__spareportInfo[DEV_PORTWAY],1,2)

			if self.__spareportInfo[SAVE_STATUS] == CREATE:
				# Needed to do it this way to create a real int vector that is supported
				# in C/C++. This IntVec() is imported from the confDBpython library.
				print "before inserting spare ports"
				if insertmany:
					success = cfDB.InsertMultipleSparePorts(self.__spareportInfo[DEV_SERIAL],self.__spareportInfo[DEV_PORTNBR],self.__spareportInfo[DEV_PORTWAY],self.__spareportInfo[DEV_PORTTYPE],self.__spareportInfo[PORT_BIA],self.__spareportInfo[PORT_MAC],first,commit)
				else:
					success = cfDB.InsertSparePort(self.__spareportInfo[DEV_SERIAL],self.__spareportInfo[DEV_PORTNBR],self.__spareportInfo[DEV_PORTTYPE],self.__spareportInfo[DEV_PORTWAY],self.__spareportInfo[PORT_BIA],self.__spareportInfo[PORT_MAC],commit)

				if commit:
					self.Update(True) #update with data from database (port ID set)

			return True
		
	        except ValidationError,err:
        	    self.__errormessage = str(err)
	            return False
		except DBError,err:
		    self.__errormessage = str(err)
		    return False
		except RuntimeError,err:
		    self.__errormessage = str(err)
		    return False #on error
		except ValueError,err:
		    self.__errormessage = str("Device Name is not a string.")
		    return False

	
		

class SpareHWType(DBInfo):
	""" There are several device types, and each device is of only one device type.
	A device type contains information that is common for devices of the same type.
	It is not a physical object itself.
	"""

	def __init__(self,hwType="",new=True):
		""" Constructor.

		Parameters:
		@systemname - name of the subsystem(s) the device type is available in
		@deviceType - device type name set by the user; which identifies a device type
		@new 	    - if True, this device type is newly created, not stored in ConfDB yet
		if False, this is a device type that is already created, and hence we can retrieve
		information for the object from ConfDB or dirty objects list. That is done by calling
		the Update(...) function in the constructor.
		"""
                self.__errormessage = ""
		self.__SpareHWTypeInfo = {}	#all device type info that is common for all device types in this dictionary
		self.__SpareHWTypeInfo[DEV_PORTS_IN] = 1
		self.__SpareHWTypeInfo[DEV_PORTS_OUT] = 1
		self.__SpareHWTypeInfo[DEV_TYPE_DESCRIPTION] = "it is a hw spare type"
		self.__SpareHWTypeInfo[DEV_SYSTEM] = "All"
		self.__SpareHWTypeInfo[DEV_HWTYPE] = hwType
		
		self._deviceTypeInfo[DEV_COLOUR] = None # wxColour object

		self.__devices = []	#a list of devices of this device type (if retrieved)

							       #MODIFY - update, DELETE - delete, RENAME - rename

		self.__cfg = GetConfig() # Get the configuration file from global method (May be not needed)

		if new == False:
			self.Update() #Force an update, retrieve info from db or dl

	###### GET ######
	   
	def GetName(self):
	    return self.__SpareHWTypeInfo[DEV_HWTYPE]
	def GetPortsIn(self):
		return self.__SpareHWTypeInfo[DEV_PORTS_IN]
	def GetPortsOut(self):
		return self.__SpareHWTypeInfo[DEV_PORTS_OUT]
	def GetDescription(self):
		return self.__SpareHWTypeInfo[DEV_TYPE_DESCRIPTION]
	def GetSystem(self):
		return self.__SpareHWTypeInfo[DEV_SYSTEM]
	def GetColour(self):
		return self.__SpareHWTypeInfo[DEV_COLOUR]
	def GetErrorMessage(self):
		return self.__errormessage

	###### SET ######
	def SetErrorMessage(self,msg):
		self.__errormessage = msg
	def SetName(self,name):
		Validation.ValidateString("spare hwtype name",name,100,False)
		self.__SpareHWTypeInfo[DEV_HWTYPE] = name
	def SetPortsIn(self,nbr):
		Validation.ValidateNumber("nbr ports in",nbr,0,10000)
		self.__SpareHWTypeInfo[DEV_PORTS_IN] = nbr
	def SetPortsOut(self,nbr):
		Validation.ValidateNumber("nbr ports out",nbr,0,10000)
		self.__SpareHWTypeInfo[DEV_PORTS_OUT] = nbr
	def SetDescription(self,descr):
		Validation.ValidateString("device type description",descr,500,True)
		self.__SpareHWTypeInfo[DEV_TYPE_DESCRIPTION] = descr
	def SetColour(self,colour): 
		#will always be a valid value/wxcolour object as the user cannot set it directly
		self.__SpareHWTypeInfo[DEV_COLOUR] = colour
	def SetSystem(self,name):
                if string.lower(name)!='all':
                    Validation.ValidateSystem("system name",name)
		self.__SpareHWTypeInfo[DEV_SYSTEM] = name
	def SetDict(self,newdict): 
		# Copy all the contents of the dictionary: just like a object copy except some variables.
		# This is implemented because the deepcopy of objects itself caused segmentation faults.
		#self._deviceTypeInfo = copy.deepcopy(newdict)
		for entry in newdict:
			self.__SpareHWTypeInfo[entry] = newdict[entry]
	def SetSaveStatus(self,status):
		self.__SpareHWTypeInfo[SAVE_STATUS] = status
	def SetDictInfo(self,dict):
		# Almost the same as SetDict(...) but this is more safe. As actually use of deepcopy
		# sometimes causes segmentation faults, even the one above.
		for item in dict:
			self.__SpareHWTypeInfo[item] = dict[item]

    	def GetObjectInfo(self,doNotLookUp=True):
        	""" Get all info about a device type in a subsystem, when given the name of a device type in the constructor.
		Here we can set up to check different statuses (TODO), f.ex. if it is the most recent version of an object;
		then just return the contents; else look in dl and db.

		@doNotLookUp - whether we should look for more recent information in dirty objects list or ConfDB.

		!return - a dictionary with all the parameters set.
		"""

		if doNotLookUp == True:
			return self.__SpareHWTypeInfo
		else:
			return self.Update()

	def Update(self,doNotCheckInDirtyList=False):
		""" Check first in dirty objects list for most recent version of this object; if in creation mode.
		If in navigation mode or alredy looked in dirty objects list and not found; check in ConfDB.

		@doNotCheckInDirtyList - if True, we skip the check in the dirty list, else we check. But only in
		creation mode

		!return - a dictionary with all the parameters set.
		"""

		if not doNotCheckInDirtyList:
			# Dirty Objects List
			if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Creation Mode
				# We look for dirty object of same name (unique within type) and type (device type)
				tmp_obj = GetAppWin().GetDirtyObject(self.__SpareHWTypeInfo[DEV_HWTYPE],SpareHWType)
				if tmp_obj != False: # Found an object of same type
					self.__SpareHWTypeInfo = copy.deepcopy(tmp_obj.GetObjectInfo(True))
					return self.__SpareHWTypeInfo # return what we found after we set it to this object
		
	        try:
		    # ConfDB
		    Validation.ValidateString("DeviceType name",self._deviceTypeInfo[DEV_TYPE],100)
 
        	    result = '|hwtype (C):'+self.__SpareHWTypeInfo[DEV_TYPE]+'|description (C):it is a hw spare type|nbrofinput (I):1|nbrofoutput (I):1|rgb_color (C):none|' # Get device type info from ConfDB

		    result = result[1:]	#skip first '|' character
		    infolist = result.split("|")	#split string to a list; each entry is a parameter name and value

		    i = 0
		    deviceTypeInfo = {DEV_OBJECT : obj_NODE}	#To identify this as a devicenode (as opposite to a linknode)
		    deviceTypeInfo[DEV_SYSTEM] = self.__SpareHWTypeInfo[DEV_SYSTEM] # Not stored in ConfDB
		    for i in range(len(infolist)-1):
			tmp = infolist[i].split(":")	#split each property, name on left site, value on right side
			tmp0 = tmp[0].split("(")[0].strip().lower()	#split away the paranthesis telling datatype, and trim the string (strip)
			datatype = tmp[0].split("(")[1].split(")")[0].strip()	#get datatype, C - String,I - integer etc.
			if datatype == 'I': #integer
				tmp[1] = int(tmp[1])
			elif datatype == 'C': #string
				tmp[1] = tmp[1].strip()

			deviceTypeInfo[tmp0] = tmp[1]
			i += 1

		    #convert rgbcolour to wxColour object
		    if deviceTypeInfo[DEV_COLOUR] in ["none","None","","(0,0,0)"]:
			    deviceTypeInfo[DEV_COLOUR] = GetAppWin().GetTempDeviceTypeColour(deviceTypeInfo[DEV_HWTYPE])
		    else:
			    rgbcolour = deviceTypeInfo[DEV_COLOUR].split(",")
			    deviceTypeInfo[DEV_COLOUR] = QColour(int(rgbcolour[0]),int(rgbcolour[1]),int(rgbcolour[2]))

		    self._deviceTypeInfo = deviceTypeInfo

		    return self._deviceTypeInfo                                                                                                         
                                                                                                                             
	        except ValidationError,err:
        		self.__errormessage = str(err)
	        	return False
		except ValueError,err:
			self.__errormessage = "Could not convert data, found wrong datatype: " + str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
			if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
				self.__errormessage = "No information found in the database on the given devicetype: " + str(self.__SpareHWTypeInfo[DEV_HWTYPE])
				return {}
			else:
			    	self.__errormessage = str(err)

		        return False #on error

	def GetHWSparesByLocation(self,location_string):
		""" General get a device at a given location.
		    TODO: should also be moved to SubSystem class.
		    NB! This function does not look for devices that are not yet stored in the ConfDB.

		    @location_string - the location string which looks for devices with the same location
		    string set.
		"""

		try:

		    res = cfDB.GetSpareHWPerLocation(location_string)
		    devices = res

		    i = 0 #reset
		    my_devices = []
		    while i < len(devices):
			my_spares.append(devices[i])
			i += 1

		    return my_spares

		except RuntimeError,err:
		        if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
					self.__errormessage = "No devices found at the given location."
					return [] #none found
			else:
				self.__errormessage = str(err)
		    	return False #on error

	def GetSpareDevices(self):  #should be called only for spare devices
            	""" Get all _sparedevice_names_ for a given device type; each device name is unique. Looks both in 
		dirty objects list and ConfDB.
		
	        !Return - list of device names; both in db and in dirtylist
        	"""

		devices_in_dirtylist = []
		my_devices = []
		dirty_objs_devs = []
		tbd = [] #to be deleted
		#tbd_objs = -[]
		if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Only if creation mode
			tmp_dirtyobjs = GetAppWin().GetDirtyObjects()
			for obj in tmp_dirtyobjs:
				# If type is: DEVICE
				if obj.__class__ == Device:
					# If not set to be deleted, and that the device type is correct (this type)
					if obj.GetSaveStatus() != DELETE and obj.GetType() == self.__SpareHWTypeInfo[DEV_TYPE]:
						if obj.GetName() not in my_devices: # If not already added; ADD!
							my_devices.append(obj.GetName())
							dirty_objs_devs.append(obj.GetName())
					#if set to be deleted and correct type
					elif obj.GetSaveStatus() == DELETE and obj.GetType() == self.__SpareHWTypeInfo[DEV_TYPE]:
						# if already added; remove
						if obj.GetName() in my_devices:
							my_devices.remove(obj.GetName())
							dirty_objs_devs.remove(obj.GetName())
						# set to the tbd list, in case we find same device in ConfDB, but set for
						# delete in dirty objects list
						tbd.append(obj.GetName())

		#First check if this device type has been renamed, cause the name in DB will be the prev one:
		if self.GetName() in GetAppWin().GetNewNames():
			old_name = GetAppWin().GetOldNamesOfRenamed()[self.GetName()]
		else:
			old_name = self.__SpareHWTypeInfo[DEV_TYPE]
			
		try:
        			          
	            tmp = cfDB.GetSpareHWPerType(old_name)
			
		    devices = tmp														     
		    i = 0 #reset
		    if GetAppWin().GetUserMode() == ID_CREATION_MODE: # Need to do some checks if creation mode
			while i < len(devices):
				dev_tmp = devices[i].split("|")
				# do add to list if:
				# a. not already added
				# b. not set for deletion
				# c. not already added under a new name (renamed)
				if (dev_tmp[0] not in dirty_objs_devs) and (dev_tmp[0] not in tbd) and dev_tmp[0] not in GetAppWin().GetOldNames():
					my_devices.append(str(dev_tmp[0]))
				i += 1


		    else: #navigation mode
			while i < len(devices):
					dev_tmp = devices[i].split("|")
					if  dev_tmp[0] not in GetAppWin().GetOldNames(): #TODO: this is wrong? do not need 
											#to do this in navigation mode?
						my_devices.append(str(dev_tmp[0]))
						i += 1

	            self.__devices = my_devices
				
		    return self.__devices

		except ValidationError,err:
			self.__errormessage = str(err)
			return False
		except DBError,err:
			self.__errormessage = str(err)
			return False
		except RuntimeError,err:
		        if string.find(str(err),"NO_ROWS_SELECTED",0) != -1:
				if len(my_devices) == 0:
					self.__errormessage = "No devices found for the given device type " + str(self.__SpareHWTypeInfo[DEV_TYPE])
					return [] #none found
				else: #none found in db, but we found in dirty list
					self.__devices = my_devices
					return self.__devices
			else:
				self.__errormessage = str(err)
		    	return False #on error


	
	

	def Delete(self):
		""" Delete this object. This object is put in the dirty list and set for deletion. 
		Sets the save status to deletion.
		"""
		
		self._deviceTypeInfo[SAVE_STATUS] = DELETE
		GetAppWin().AddToDirtyObjects(self) #possible to send an instance of itself?

	def Modify(self):
		""" Modifies a device type with new values set by the user through the set functions. The modify status
		is set through SetModifyStatus(...) if needed.
		Sets the save status to modify, and adds the object to the dirty objects list.
		"""
		self._deviceTypeInfo[SAVE_STATUS] = MODIFY
		GetAppWin().AddToDirtyObjects(self)

	def Create(self):
		""" Create a device type. Set save status to creation, and adds the object to the
		dirty objects list.
		"""
		
		self._deviceTypeInfo[SAVE_STATUS] = CREATE
		GetAppWin().AddToDirtyObjects(self)
	
#end added by lana
