from qt import *
import string # string manipulation functions needed

from cdbVisCore import *
from objectclasses import *
import helper
from GUIcreatedevices import *
from CreatePorts import *

#####################################################################
# CreateDeviceWindow class					#####
#####################################################################

class CreateDeviceWindow(GUIcreateDevices):
	""" This class is responsible for showing the dialog for creating and modifying devices,
	    let the user create or modify devices, and to handle the ports for the devices.
	"""
	def __init__(self,parent,selectedsystem,creationmode=True,deviceobj=None,devicetypes=[],devtype="",location="",functionlist=[]):
		""" The Constructor.

		    Parameters:
		    @parent         - parent window to this window (MainWindow)
		    @selectedsystem - the system to create this device in (the user can also add others)
		    @creationmode   - if True, then a device will be created, 
		                      if False, a device will be modified (already exists)
		    @deviceobj      - if the user are about to modify a device, then the previous values of
		                      the device will have to be set, and we use this object for that
		    @devicetypes    - A list of strings of the device types that are available in the active
		                      subsystem; user can choose device type in the combo box.
		    @devtype        - If device type is already chosen for the new device (create), it is set here
		    @location       - When create, set location string.
		"""
		GUIcreateDevices.__init__(self,parent,"Create_Devices",0,0)
		self._creationmode = creationmode
		in_db = False # Whether devices is stored in ConfDB or not..yet
		self.main = parent
		close_window = False # If we get an unrecoverable error, we have to close the window to avoid the user to do any "damage"
		self._deletedports = []

                if creationmode == True:
                        title = "Create Device(s)"
			self._deviceports = []
			if devtype == None:
				devtype = ""
                else:
			self._deviceports = deviceobj.GetPorts() # Port objects for the device; if modifying
			
			if self._deviceports == False:
				self.main.ShowError("A database error occured while fetching information about the ports for the current device: " + str(deviceobj.GetErrorMessage()) + ". We are unable to continue.",ERR_ERROR,True)
				close_window = True
			
                        title = "Modify Device " + str(deviceobj.GetName())
			selectedsystem = deviceobj.GetSystem()
			self.renameobj = None 
			
			# We have to know the current values of these variables in order 
			# to know what modify status we have to set for the device
			self.oldname = deviceobj.GetName()
			self.oldnode = deviceobj.GetNode()
			self.oldsystem = deviceobj.GetSystem()
			
			self.deviceid = deviceobj.GetID()
			self.nodechanged = deviceobj.IsNodeChanged()
			self.systemchanged = deviceobj.IsSystemChanged()
			
			# Check if device exists in db, if not we can do more edits than restricted by db functions
			test_device = Device(deviceobj.GetSystem(),deviceobj.GetName(),True)
			res = test_device.Update(True)
			if res == {}:
				in_db = False
			elif res == False:
				in_db = False
				close_window = True
				self.main.ShowError("A database error occured while trying to fetch information about the current device: " + str(test_device.GetErrorMessage()) + ". We are unable to continue.",ERR_ERROR,True)
			else:
				in_db = True

		
		 
		if True:
			pass
			'''Create_DevicesLayout = QGridLayout(self,1,1,11,6,"Create_DevicesLayout")

		        self._serialtxtbox = QLineEdit(self,"__serialtxtbox")

		        Create_DevicesLayout.addMultiCellWidget(self._serialtxtbox,1,1,8,10)

		        self._hwtypetxtbox = QLineEdit(self,"__hwtypetxtbox")

		        Create_DevicesLayout.addMultiCellWidget(self._hwtypetxtbox,3,3,8,10)

		        self._responsibletxtbox = QLineEdit(self,"__responsibletxtbox")

		        Create_DevicesLayout.addMultiCellWidget(self._responsibletxtbox,5,5,8,10)

		        self.textLabel2_4_3 = QLabel(self,"textLabel2_4_3")

		        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3,0,0,8,9)

		        self.textLabel2_4_3_2 = QLabel(self,"textLabel2_4_3_2")

		        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2,2,2,8,9)

		        self.textLabel2_4_3_2_2_2 = QLabel(self,"textLabel2_4_3_2_2_2")

		        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2_2_2,6,6,8,9)

		        self.textLabel2_4_3_2_2 = QLabel(self,"textLabel2_4_3_2_2")

		        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2_2,4,4,8,9)

		        self.textLabel2_4_3_2_2_2_2 = QLabel(self,"textLabel2_4_3_2_2_2_2")

		        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2_2_2_2,9,9,8,9)

		        layout6 = QVBoxLayout(None,0,6,"layout6")

		        self.textLabel2_3_2 = QLabel(self,"textLabel2_3_2")
		        layout6.addWidget(self.textLabel2_3_2)

		        self._functiontxtbox = QComboBox(0,self,"__functiontxtbox")
		        layout6.addWidget(self._functiontxtbox)

		        Create_DevicesLayout.addMultiCellLayout(layout6,13,13,0,6)

		        layout11 = QVBoxLayout(None,0,6,"layout11")

		        self.textLabel2_2 = QLabel(self,"textLabel2_2")
		        layout11.addWidget(self.textLabel2_2)

		        self._sysnamescmbbox = QComboBox(0,self,"__sysnamescmbbox")
		        layout11.addWidget(self._sysnamescmbbox)

		        Create_DevicesLayout.addMultiCellLayout(layout11,0,1,2,4)

		        self._delbutton = QPushButton(self,"__delbutton")
		        self._delbutton.setMaximumSize(QSize(21,21))

		        Create_DevicesLayout.addWidget(self._delbutton,1,6)

		        self._addbutton = QPushButton(self,"__addbutton")
		        self._addbutton.setMaximumSize(QSize(21,21))

		        Create_DevicesLayout.addWidget(self._addbutton,1,5)

		        self._chk_prommode = QCheckBox(self,"__chk_prommode")

		        Create_DevicesLayout.addMultiCellWidget(self._chk_prommode,11,11,0,1)

		        layout5 = QVBoxLayout(None,0,6,"layout5")

		        self.textLabel2_4_2_2 = QLabel(self,"textLabel2_4_2_2")
		        layout5.addWidget(self.textLabel2_4_2_2)

		        self._startspinbox = QSpinBox(self,"__startspinbox")
		        layout5.addWidget(self._startspinbox)

		        Create_DevicesLayout.addMultiCellLayout(layout5,10,10,0,1)

		        layout4 = QVBoxLayout(None,0,6,"layout4")

		        self.textLabel2_4_2_2_2 = QLabel(self,"textLabel2_4_2_2_2")
		        layout4.addWidget(self.textLabel2_4_2_2_2)

		        self._endspinbox = QSpinBox(self,"__endspinbox")
		        layout4.addWidget(self._endspinbox)

		        Create_DevicesLayout.addMultiCellLayout(layout4,10,10,2,6)

		        self._paddingchkbox = QCheckBox(self,"__paddingchkbox")

		        Create_DevicesLayout.addWidget(self._paddingchkbox,8,0)

		        self._paddingspinbox = QSpinBox(self,"__paddingspinbox")
		        self._paddingspinbox.setEnabled(0)

		        Create_DevicesLayout.addMultiCellWidget(self._paddingspinbox,8,8,1,3)

		        self.textLabel2_4_2 = QLabel(self,"textLabel2_4_2")

		        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_2,8,8,4,5)
		        spacer1 = QSpacerItem(310,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
		        Create_DevicesLayout.addMultiCell(spacer1,14,14,0,8)

		        self._okbutton = QPushButton(self,"__okbutton")

		        Create_DevicesLayout.addWidget(self._okbutton,14,9)

		        self._cancelbutton = QPushButton(self,"__cancelbutton")

		        Create_DevicesLayout.addWidget(self._cancelbutton,14,10)

		        self._portbutton = QPushButton(self,"__portbutton")

		        Create_DevicesLayout.addMultiCellWidget(self._portbutton,13,13,8,10)

		        self._commentstxtbox = QTextEdit(self,"__commentstxtbox")

		        Create_DevicesLayout.addMultiCellWidget(self._commentstxtbox,10,12,8,10)
		        spacer4 = QSpacerItem(20,20,QSizePolicy.Fixed,QSizePolicy.Minimum)
		        Create_DevicesLayout.addItem(spacer4,8,7)

		        self._locationtxtbox = QLineEdit(self,"__locationtxtbox")

		        Create_DevicesLayout.addMultiCellWidget(self._locationtxtbox,7,7,8,10)

		        layout9 = QVBoxLayout(None,0,6,"layout9")

		        self.textLabel2 = QLabel(self,"textLabel2")
		        layout9.addWidget(self.textLabel2)

		        self._systemnametxtbox = QLineEdit(self,"__systemnametxtbox")
		        self._systemnametxtbox.setEnabled(0)
		        layout9.addWidget(self._systemnametxtbox)

		        Create_DevicesLayout.addMultiCellLayout(layout9,0,1,0,0)

		        layout7 = QVBoxLayout(None,0,6,"layout7")

		        self.textLabel2_4 = QLabel(self,"textLabel2_4")
		        layout7.addWidget(self.textLabel2_4)

		        self._prefixtxtbox = QLineEdit(self,"__prefixtxtbox")
		        layout7.addWidget(self._prefixtxtbox)

		        Create_DevicesLayout.addMultiCellLayout(layout7,6,7,0,6)

		        self._chk_node = QCheckBox(self,"__chk_node")

		        Create_DevicesLayout.addMultiCellWidget(self._chk_node,12,12,0,4)

		        layout8 = QVBoxLayout(None,0,6,"layout8")

		        self.textLabel2_3 = QLabel(self,"textLabel2_3")
		        layout8.addWidget(self.textLabel2_3)

		        self._devicetypetxtbox = QComboBox(0,self,"__devicetypetxtbox")
		        layout8.addWidget(self._devicetypetxtbox)

		        Create_DevicesLayout.addMultiCellLayout(layout8,4,5,0,6)

		        layout7_2_2 = QVBoxLayout(None,0,6,"layout7_2_2")

		        self.textLabel2_4_4_2 = QLabel(self,"textLabel2_4_4_2")
		        layout7_2_2.addWidget(self.textLabel2_4_4_2)

		        self._prefixtxtbox = QLineEdit(self,"__prefixtxtbox")
			self._prefixtxtbox.setEnabled(False)
			layout7_2_2.addWidget(self._prefixtxtbox)

		        Create_DevicesLayout.addMultiCellLayout(layout7_2_2,2,3,0,2)

		        layout7_2 = QVBoxLayout(None,0,6,"layout7_2")

		        self.textLabel2_4_4 = QLabel(self,"textLabel2_4_4")
		        layout7_2.addWidget(self.textLabel2_4_4)

		        self._deviceidtxtbox = QLineEdit(self,"__deviceidtxtbox")
			self._deviceidtxtbox.setEnabled(False)
		        layout7_2.addWidget(self._deviceidtxtbox)

		        Create_DevicesLayout.addMultiCellLayout(layout7_2,2,3,3,6)

		        self.languageChange()

		        self.resize(QSize(579,461).expandedTo(self.minimumSizeHint()))
		        self.clearWState(Qt.WState_Polished)
			'''
		
		self.system = selectedsystem
		self.deviceobj = deviceobj
		self.devs = []
		self.deviceobjs = []


		# Set up the subsystem text box (which subsystems the device will be created in)
		if self._creationmode:
			mysystems = selectedsystem + ","
		else:
			mysystems = deviceobj.GetSystem() + ","
		# Subsystem text box and combo box
		self._systemnametxtbox.setText(mysystems)
		tmpsystems = self.main.GetCdb().GetSubSystems()
		subsystems = []
		for tmp in tmpsystems:
			subsystems.append(tmp.GetName())
		for system in subsystems:
			self._sysnamescmbbox.insertItem(system)
		self.connect(self._addbutton,SIGNAL("released()"),self.OnAdd)
		self.connect(self._delbutton,SIGNAL("released()"),self.OnRemove)

		if not self._creationmode:
			self._deviceurl.setText(deviceobj.GetURL())
		
		# Device type name combo box	#added by Lana
		print "devicetypes=%s " % (devicetypes);
		
		for type in devicetypes:
			self._devicetypetxtbox.insertItem(type)

		if not self._creationmode:
			devtype = deviceobj.GetType()
			self._devicetypetxtbox.setCurrentText(devtype) #Select the active device type:
			self._devicetypetxtbox.setEnabled(False)
			if in_db:
				self._deviceidtxtbox.setText(str(self.deviceobj.GetID()))
			

		# Device name, shown if we modify a device
		if not self._creationmode:
			self._prefixtxtbox.setEnabled(True)  # Enabling "Device Name"
			self._prefixtxtbox.setText(self.deviceobj.GetName())
		#added by Lana
		# print "in lana section %s" %(functionlist)
		# function_name=""
		# for fct in functionlist:
			# self._functiontxtbox.insertItem(str(fct))
		#self._functiontxtbox.setCurrentText(function_name)


		# What properties we can change on the right hand side depends on whether the device is already created
		# and if so if it is stored in ConfDB.
		if not self._creationmode:
			self._chk_prommode.setChecked(self.deviceobj.GetPromMode())
			self._chk_node.setChecked(self.deviceobj.GetNode())
			self._serialtxtbox.setText(self.deviceobj.GetSerial())
			self._hwtypetxtbox.setText(self.deviceobj.GetHWType())
			self._responsibletxtbox.setText(self.deviceobj.GetResponsible())
			self._locationtxtbox.setText(self.deviceobj.GetLocation())
			self._commentstxtbox.setText(self.deviceobj.GetComments())
			#added by Lana
			## PROBLEME : no method   GetFunction defined !!!!!!!!!!!
			##self._functiontxtbox.insertItem(self.deviceobj.GetFunction()) 

			if in_db:
				self._serialtxtbox.setEnabled(False)
				self._hwtypetxtbox.setEnabled(False)
				self._responsibletxtbox.setEnabled(False)
				self._commentstxtbox.setEnabled(False)
				##self._functiontxtbox.setEnabled(False)
		else: # Default settings
			self._chk_prommode.setChecked(True)
			self._locationtxtbox.setText(location)

		# If we create a device, options for naming the device(s) is shown (prefix, padding, nr etc)
		if self._creationmode:
			self._prefixtxtbox.setEnabled(True)
			self._paddingchkbox.setEnabled(True)
			self._paddingspinbox.setEnabled(True)
			self._startspinbox.setEnabled(True)
			self._endspinbox.setEnabled(True)
			## WAL_PROB : self.connect(self._paddingchkbox,SIGNAL("toggled(bool)"),self.OnSelectPadding) #EVT_CHECKBOX(self._panel,PADDING_ID,self.OnSelectPadding)

			if devtype != "": # If device type is set..we guess right away
				self.OnDeviceTypeChanged(None)
			self._endspinbox.setValue(1) # nr of devices to be created
		else:
			self._prefixtxtbox.setEnabled(False)
			self._paddingchkbox.setEnabled(False)
			self._paddingspinbox.setEnabled(False)
			self._paddingspinbox.setEnabled(True)
			self._startspinbox.setEnabled(False)
			self._endspinbox.setEnabled(False)
		
		if self._creationmode:
			text = "Create/Modify ports..."
		else:
			text = "Modify ports..."
		self._portbutton.setText(text)
		self.connect(self._portbutton,SIGNAL("released()"),self.OnCreatePorts) # EVT_BUTTON(self,PORT_BTN,self.OnCreatePorts)

		# OK and Cancel buttons
		self.connect(self._okbutton,SIGNAL("released()"),self.OkButtonClicked)
		self.connect(self._cancelbutton,SIGNAL("released()"),self.reject)

	def SaveObjects(self):
		""" Here we create device object, rename object if needed, validate all properties
		and create the ports needed.
		
		!return - True if successful, False if an error occur
		"""
		try:

			if self._creationmode: # Create
				
				i = self._startspinbox.value()
				# from start number to end number (start number + total number of devices to be created)
				while i < (int(str(self._endspinbox.text())) + int(str(self._startspinbox.text()))):
					padding = int(str(self._paddingspinbox.text())) # Padding...?
					nrlength = len(str(i)) # Length of number without padding

					if self._paddingchkbox.isChecked():
						padstr = "".zfill(padding-nrlength) # Fill number with padding
					else:
						padstr = ""

					# Replace the %d with the number (with or without padding) in the device name string
					devname = str(self._prefixtxtbox.text()).replace("%d",str(padstr)+str(i))
					serial = str(self._serialtxtbox.text()).replace("%d",str(padstr)+str(i))
					deviceobj = Device(str(self._systemnametxtbox.text())[:-1],devname,True)
					deviceobj.SetName(devname) # We have already checked that the device name is unique
					sysname = str(self._systemnametxtbox.text())[:-1] #Removing the last comma
					deviceobj.SetSystem(sysname,False)
					deviceobj.SetNode(int(self._chk_node.isChecked()),False)
					deviceobj.SetType(str(self._devicetypetxtbox.currentText()))
					deviceobj.SetSerial(serial)
					deviceobj.SetHWType(str(self._hwtypetxtbox.text()))
					deviceobj.SetResponsible(str(self._responsibletxtbox.text()))
					deviceobj.SetLocation(str(self._locationtxtbox.text()))
					deviceobj.SetComments(str(self._commentstxtbox.text()))
					deviceobj.SetPromMode(int(self._chk_prommode.isChecked()))
					deviceobj.SetURL(str(self._deviceurl.text()))
					## deviceobj.SetFunction(str(self._functiontxtbox.currentText()))
					
					self.deviceobjs.append(deviceobj) # Add device obj to list of devices that we will create
					
					i += 1
			else:
					devname = str(self._prefixtxtbox.text())
					if devname != self.oldname:
						ans = QMessageBox.question(self,"Rename ?","You have renamed the device, was that your intention? Remember that you can not undo renaming, \nbut you can rename the object as many times as you want.","Yes","No","",0,1)
						if ans == 1: # No
        	                                        self._prefixtxtbox.setText(self.oldname)
                	                                return False
						devtypename = str(self._devicetypetxtbox.currentText())
						devtype = DeviceType(str(self._systemnametxtbox.text())[:-1],devtypename,False) #The [:-1] removes the last comma
						devs = devtype.GetDevices()
						if devs == False:
							self.main.ShowError("Failed to fetch data from ConfDB: " + str(devtype.GetErrorMessage()),ERR_ERROR,True)
							return False
						else:
							if devname in devs:
								self.main.ShowError("You cannot rename the device to this name because " + str(devname) + " is already in use by another device.",ERR_ERROR,True)
								self._prefixtxtbox.setText(self.oldname)
								return False

                                        	self.renameobj = Device(str(self._systemnametxtbox.text())[:-1],str(self._prefixtxtbox.text()),True)
	                                        self.renameobj.SetOldName(self.oldname)
						self.renameobj.SetType(str(self._devicetypetxtbox.currentText()))
						self.renameobj.SetID(self.deviceid)
                        	                self.renameobj.SetSaveStatus(RENAME)
	
					self.deviceobj = Device(str(self._systemnametxtbox.text())[:-1],devname,False)
					self.deviceobj.SetID(self.deviceid)
	
					self.deviceobj.SetName(str(self._prefixtxtbox.text()))
					self.deviceobj.SetType(str(self._devicetypetxtbox.currentText()))
					
					self.deviceobj.SetSerial(str(self._serialtxtbox.text()))
					self.deviceobj.SetHWType(str(self._hwtypetxtbox.text()))
					self.deviceobj.SetResponsible(str(self._responsibletxtbox.text()))
					self.deviceobj.SetLocation(str(self._locationtxtbox.text()))
					self.deviceobj.SetComments(str(self._commentstxtbox.text()))
					self.deviceobj.SetURL(str(self._deviceurl.text()))
					## WALID PROB : The methode SetLocationFunction Doesnot exist
					## self.deviceobj.SetLocationFunction(str(self._functiontxtbox.currentText()))

					if int(self._chk_node.isChecked()) != self.oldnode or self.nodechanged:
						changed = True
					else:
						changed = False
					self.deviceobj.SetNode(int(self._chk_node.isChecked()),changed)
	
					sysname = str(self._systemnametxtbox.text())[:-1] #Removing the last comma
					if sysname != self.oldsystem or self.systemchanged:
						changed = True
					else:
						changed = False

					self.deviceobj.SetSystem(sysname,changed)					
					self.deviceobj.SetPromMode(int(self._chk_prommode.isChecked()))

					self.deviceobjs.append(self.deviceobj) # Add device object to list (although it is only one)
					
                	return True # All well

                except ValidationError,err:
                        self.main.ShowError(str(err),ERR_ERROR,True)
                        return False
		except ValueError,err:
			self.main.ShowError("A String to Integer conversion failed: " + str(err),ERR_ERROR,True)
			return False

	def OnCreatePorts(self):
		""" We click on the Create/Modify button to create port(s) for the device(s) we create.
		They will be stored in a list in this object, and sent to the user when he/she
		asks for it.

		!return - True if successful, False if not.
		"""

		 # We create the same ports for all the devices 
		devicename = str(self._prefixtxtbox.text())
		devtype = DeviceType(str(self._systemnametxtbox.text())[:-1],str(self._devicetypetxtbox.currentText()),False)
		my_createportframe = createPortsWindow(self.main,devtype,devicename,self._deviceports,self)
		my_createportframe.show()
		ans =  my_createportframe.exec_loop()
		if ans==1:
			self._deviceports = my_createportframe.GetPorts()
			self._deletedports = my_createportframe.GetDeletedPorts()
		return
		'''# We need device type and device name for the device(s) before we assign ports to them.
		if self._devicetypetxtbox.GetValue == "":
			self.main.ShowError("You have to choose a device type before you assign ports.",ERR_ERROR,True)
			return False

		# If we create, the device name has to be set. Prefix with %d if more than one device to be created.
		if self._creationmode:
			if str(self._prefixtxtbox.text()) == "" or (str(self._prefixtxtbox.text()).find("%d",0,len(str(self._prefixtxtbox.text()))) == -1 and str(self._endspinbox.text()) > 1):
				self.main.ShowError("You have to set a proper device name, and include a %d if you create more than one device, before you assign ports.",ERR_ERROR,True)
				return False

			# We set up the device names in a string like this: DEV_NAME_001 -> DEV_NAME_010
			# for the devices that the ports will be created for.
			padding = int(str(self._paddingspinbox.text()))
			nrlength = len(str(str(self._startspinbox.text())))
			if self._paddingchkbox.isChecked():
				padstr = "".zfill(padding-nrlength)
			else:
				padstr = ""
	
				
			devname = str(self._prefixtxtbox.text()).replace("%d",str(padstr)+str(str(self._startspinbox.text())))
			if str(self._endspinbox.text()) > 1:
				nrlength = len(str(str(self._endspinbox.text())))
				if self._paddingchkbox.isChecked():
					padstr = "".zfill(padding-nrlength)
				else:
					padstr = ""
				devname += " -> " + str(self._prefixtxtbox.text()).replace("%d",str(padstr)+str(str(self._startspinbox.text())+str(self._endspinbox.text())-1))
		else: # Modify
			if str(self._prefixtxtbox.text()) == "":
				self.main.ShowError("You have to choose a device name before you assign ports",ERR_ERROR,True)
				return False
			devname = str(self._prefixtxtbox.text())

		# We also need to send the device type to the create ports window (to check that the number of created ports
		# and type is correct.
		devtypename = str(self._devicetypetxtbox.currentText())
		devtype = DeviceType(str(self._systemnametxtbox.text())[:-1],devtypename,False) #The [:-1] removes the last comma

		# Open Create Ports Window
		win = CreatePortsWindow(self.main,-1,self._creationmode,devname,devtype,self._deviceports)
		if win.ShowModal() == wxID_OK:
			self._deviceports,deletedports = win.GetPortObjs() # Set the port objects created to the self._deviceports list
			self._deletedports.extend(deletedports)'''
		

	def OnSelectPadding(self,checked):
		""" Enable/Disable the padding spin box depending on whether
		the paddingcheckbox is checked or not.
		"""
		
		self._paddingspinbox.setEnabled(not self._paddingspinbox.isEnabled())

	def OnDeviceTypeChanged(self,event):
		""" If the user change the device type, we will have to make a new
		guess for the syntax of the device name.
		"""

		devtypename = str(self._devicetypetxtbox.currentText())
		sysname = str(self._systemnametxtbox.text())[:-1] #Removing the last comma
		devtype = DeviceType(sysname,devtypename,False)
		devs = devtype.GetDevices() # 1. We get all the devices of the device type chosen
		if devs == False:
			self.main.ShowError("Failed to retrieve devices of the given device type: " + str(devtype.GetErrorMessage()),ERR_ERROR,True)
			self._prefixtxtbox.setEnabled(True)
			self._paddingchkbox.setEnabled(True)
			self._startspinbox.setEnabled(True)
			self._endspinbox.setEnabled(True)

			self._prefixtxtbox.setText("")
			self.devs = []
		elif devs == []: #no devs found, first time, let user set all
			self._prefixtxtbox.setEnabled(True)
			self._paddingchkbox.setEnabled(True)
			self._startspinbox.setEnabled(True)
			self._endspinbox.setEnabled(True)
			self._prefixtxtbox.setText("")
			self.devs = []
		else: #found devs
			# 2. We have to find the number of the last device created (highest number), we sort the devices
			# and assign the list of device names to a member variable so that it is accessible for
			# MakeEstimatedGuess method
			devs.sort()
			self.devs = devs
			self._prefixtxtbox.setEnabled(True)
			self._paddingchkbox.setEnabled(True)
			self._startspinbox.setEnabled(True)
			self._endspinbox.setEnabled(True)
			self._prefixtxtbox.setText(devs[0]) # 3. Set first device in the sorted list in the prefix txt box
			self.MakeEstimatedGuess(None) # 4. Continue to guess prefix
	
        def OnAdd(self):
		""" Add a subsystem (shown in the combo box) to the list of subsystems that the
		the device will be added to.
		"""
		
                val = str(self._sysnamescmbbox.currentText())
		if val != "":
	                if string.find(str(self._systemnametxtbox.text()),val,0) != -1:
        	                self.main.ShowError("The subsystem is already added to the subsystem list.",ERR_ERROR)
                	else:
                        	self._systemnametxtbox.setText(str(self._systemnametxtbox.text()) + val + ",")
                                                                                                                                                                       
        def OnRemove(self):
		""" Remove a subsystem (shown in the combo box) from the list of
		subsystems that the device will be added to.
		"""
		
                val = str(self._sysnamescmbbox.currentText())
		if val != "":
	                if string.find(str(self._systemnametxtbox.text()),val,0) == -1:
        	                self.main.ShowError("This subsystem is not a part of the subsystem list, and cannot be removed.",ERR_ERROR)
                	else:
                        	myvals = str(self._systemnametxtbox.text())
	                        myvals = myvals.replace(val + ",","")
        	                self._systemnametxtbox.setText(myvals)

	def MakeEstimatedGuess(self,event=None):
		""" When a device type has been chosen, this method is called from the 
		OnDeviceTypeChanged(...) method, to make an estimated guess
		of what the prefix/syntax for the next device that will be added.
		We also set up padding and startnr to what we think is the correct.
		"""

		# 1. We get the value set in the prefix text box by the OnDeviceTypeChanged(...) method
		# This value is the name of the first device created of the given device type.
		syntax = str(self._prefixtxtbox.text())
		if syntax == "":
			self.main.ShowError("We didn't find any devices of the given device type, the user will have to fill out all fields him/her self",ERR_ERROR)
			return
		if self.devs == []:
			self.main.ShowError("We didn't find any devices of the given device type, the user will have to fill out all fields him/her self",ERR_ERROR)
			return
		
		# 2. Ok, we have enough information, we continue to guess..
		i = len(syntax)-1
		nrofindexint = []
		prevwasint = False
		while i >= 0: # a. We start looking for the ID (integer) in the device name string at the last position
			try:
				nr = int(syntax[i]) # b. Check if we can convert the character in given pos to int..
				nrofindexint.insert(0,i) # c. We could convert to int, add to our list
				prevwasint = True # d. We found an integer all right
			except ValueError,err:
				if prevwasint: # e. If the prev character was an integer, but not this, we found a whole
					       # integer string in the device name, and can continue to work on it.
					       # We don't need to look any more since we found our int, break the loop
					break
			i -= 1

		# 3. Did we find an integer?
		if nrofindexint == []: # No..
			return

		# 4. Get nr substring
		start = nrofindexint[0] # a. check first position in our list
		end = nrofindexint[len(nrofindexint)-1] # b. check last position in our list
		if start == end: # c. if both first and last position was the same index, it is only 1 number
			substring = syntax[start] # get the number from the string
		else: # d. more than 1 number in the number
			substring = syntax[start:end+1] # get the number from the string

		# 5. Split our device name string in a string before the number and a string after the number
		if nrofindexint[0]-1 >= 0: # a. at least one character before the number?
			startstr = syntax[0:start]
		else:
			startstr = ""

		if len(syntax)-1 > end: # b. at least one character after the number?
			endstr = syntax[end+1:len(syntax)]
		else:
			endstr = ""

		# 6. Set prefix in prefix txt box, %d indicates where we found the number in the device name string
		prefix = startstr + "%d" + endstr
		self._prefixtxtbox.setText(prefix)

		# 7. Set correct padding value
		# Check if padding; 
		# "003" -> int("003") -> 3 -> str(3) -> "3" -> "003" != "3" -> PADDING
		# "3" -> int("3") -> 3 -> str(3) -> "3" -> "3" == "3" -> NO PADDING
		if len(str(int(substring))) != len(str(substring)): # a. Padding was used. (substring is the number string)
			nrofdigits = len(str(substring))
			self._paddingchkbox.setChecked(True)
			self._paddingspinbox.setValue(nrofdigits)
			self._paddingspinbox.setEnabled(True)

		'''# 8. Find start number of next device (need to find the highest number used for a device for this type)
		nrs = []
		for dev in self.devs: # self.devs is alphabetically sorted
			# a. We replace string before and after number with "" (empty string), then we can easily
			# extract the number, convert it to integer, and check if we have found the biggest number
			# 2DO: a more efficient way to find the highest number without having to check every device name...
			# (Perhaps: first check string length, then compare numbers)
			if startstr != "":
				dev = dev.replace(startstr,"")
			if endstr != "":
				dev = dev.replace(endstr,"")

			try:
				nrs.append(int(dev))
			except ValueError,err:
				self.main.ShowError("Some problem automatically finding the correct prefix, you will have to set the prefix yourself. \nStart number for new devices was not set.",ERR_ERROR,True)
				return
		# 9. Sort the numbers found and pick out the last one -> the biggest one -> add 1 to get the start number
		nrs.sort()
		self._startspinbox.setValue(nrs[len(nrs)-1]+1)'''

	def GenerateNames(self,event=None,showerror=False):
		""" This method is either called when the user alters the prefix,
		or when the user wants to save the changes made. The user will
		only be informed about errors when he/she wants clicks on the
		OK button to create a device.

		The name of the method is a bit confusing, as the method just checks
		that you have a %d in the string if needed or not.

		Parameters:
		@showerror - 	default (False), whether we should show an error message to the user or not
				if it occurs.
		"""
		
		syntax = str(self._prefixtxtbox.text())
		index = syntax.find("%d",0,len(syntax)) # We have maximum one %d in the prefix (do not need it if we
							# only create one device
		if index == -1 and showerror:
			if self._endspinbox.value() == 1: # Do not need %d if only 1 device
				return True
			else:
				self.main.ShowError("Couldn't find %d in the prefix, this HAS to be added because you are creating more than one device.",ERR_ERROR,True)
				self._startspinbox.setValue(0)
				return False
		elif index == -1:
			if str(self._endspinbox.text()) == 1: # Do not need %d if only 1 device
				return True
			else:
				self._startspinbox.setValue(0)
				return False
		else: # Did find %d
			if index != 0:
				startstr = syntax[0:index]
			else:
				startstr = ""
			if index+1 != len(syntax)-1:
				endstr = syntax[index+2:len(syntax)]
			else:
				endstr = ""

		return True

	def OkButtonClicked(self):
        	""" Easy validation before accepting user settings.
		We check that the required fields are filled out,
		that there are no device name conflicts,
		correct number of ports added to the device etc.

		!return - True if successful, False if not
                """
		if self._systemnametxtbox.text() == "":
			self.main.ShowError("No subsystems set!",ERR_ERROR,True)
			return False

		if self._devicetypetxtbox.currentText() == "":
			self.main.ShowError("No device type set!",ERR_ERROR,True)
			return False

		if not self._creationmode: # Modify
			if self._prefixtxtbox.text() == "":
				self.main.ShowError("No device name set!",ERR_ERROR,True)
				return False
		else: # Create
			if self._endspinbox.value()>1 and self._prefixtxtbox.text() == "":
				self.main.ShowError("No prefix set.",ERR_ERROR,True)
				return False

		if self._creationmode:
			success = self.GenerateNames(None,True) # Check that the prefix is correct
			if not success:
				return False

			# Check that we're creating a unique device and that we have same nr of ports created 
			# as given by the device type.
			# Actually...we should check if this device name is already in use, put then we
			# will have to connect to the database and run such a query for each device we create
			# or we will have to get a list of names of EVERY device in ConfDB (which can be 
			# enourmous). We decided to do a thing that will work in most cases: check that no
			# other devices of the same device type have the same name.
			
			devtype = DeviceType(str(self._systemnametxtbox.text())[:-1],str(self._devicetypetxtbox.currentText()),False)
			# Check if correct number of ports is created.
			if len(self._deviceports) > (devtype.GetPortsIn()+devtype.GetPortsOut()):
				self.main.ShowError("The number of ports that a device of the given device type has (" + str(devtype.GetPortsIn()+devtype.GetPortsOut()) + "), does not correspond to the number of ports that you have created for this device (" + str(len(self._deviceports)) + "). Click the create/modify ports button to remove ports.",ERR_ERROR,True)
				return False


			# Check that the number ( and padding ) does not exceed the number of maximum digits set
			# Also check that the device name is unique (for the given device type)
			devs = devtype.GetDevices()
			if devs == False:
				self.main.ShowError("Failed to fetch data from ConfDB: " + str(devtype.GetErrorMessage()),ERR_ERROR,True)
				return False
			i = int(str(self._startspinbox.text()))
			while i < (self._endspinbox.value() + self._startspinbox.value()):
				padding = int(str(self._paddingspinbox.text()))
				nrlength = len(str(i))
				if nrlength > padding and self._paddingchkbox.isChecked():
					self.main.ShowError("You have exceeded the maximum number of digits that you set as a maximum for the given device type.",ERR_ERROR,True)
					return False
				elif self._paddingchkbox.isChecked():
					padstr = "".zfill(padding-nrlength)
				else:
					padstr = ""

				devname = str(self._prefixtxtbox.text()).replace("%d",str(padstr)+str(i))
				if devname in devs:
					self.main.ShowError("The device " + str(devname) + " already exists. You have to change the range or delete the device with the same name in order to save a new one with that name.",ERR_ERROR,True)
					return False
				i += 1


		# Finally, try to create the objects needed.
		success = self.SaveObjects()
		if not success:
			return False

                QDialog.accept(self) # Close window, let the calling code have access to public methods

	def IsCreationMode(self):
		""" Whether we are creating or modifying a device.
		
		!return - True if creating, False if modifying
		"""
                return self._creationmode

	def GetDeviceObjs(self):
		""" Return the objects that were created by the user in this dialog

		!return    - 	Create: list of devices and ports 
				Modify: the device modified, rename object if renamed, 
			        	ports if any were altered.
		"""
		if self._creationmode:
			return self.deviceobjs,self._deviceports
		else:
			return self.deviceobjs[0],self.renameobj,self._deviceports,self._deletedports
