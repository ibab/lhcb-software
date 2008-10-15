import string	# Needed for string manipulation


from cdbVisCore import *    # Constants
from objectclasses import * # Data objects
import pdb


###################################################################
# CreateDeviceTypeWindow class					###
###################################################################

# CONSTANTS, WIDGET IDs
INPUTSPINBOX = 101
OUTPUTSPINBOX = 102
DEVICETYPENAME = 103
COLORBUTTON = 104
TXTINPUTNBR = 105
TXTOUTPUTNBR = 106
DEVTYPE_DESCRIPTION = 107
SYSNAME_CMBBOX = 108
DEVTYPEID = 109


class CreateDeviceTypeWindow(QDialog):
	""" This dialog window and class lets the user create or modify a device type. It is set
	    to be modal.
	"""

	def __init__(self,parent,name,selectedsystem,creationmode=True,devicetypeobj=None,devicetypes=[]):
		""" The constructor.
		
		    Parameters:
		    @parent - parent window to this window (usually MainWindow)
		    @id - id of this window, set by its parent
		    @selectedsystem - the subsystem to create this device type in; 
		    		      the user can also add others in the dialog
		    @creationmode - if True; a device type is being created
		    		    if False; a device type is being modified (already created)
		    @devicetypeobj - if we are modifying a device type (creationmode=False), 
		    		     then we have to send the device type object that we want to
				     modify; to show the settings/attributes to the user so he/she
				     can modify them
		    @devicetypes - both when we are creating and modifying a device type we send a 
		    		   list of names of currently available devicetypes; so that we can 
				   check that the name we don't get any name conflicts when creating
				   or renaming a device type
		"""
	        QDialog.__init__(self,parent,name,0,0)
		Form1Layout = QGridLayout(self,1,1,11,6,"Form1Layout")
	        spacingButtons = QSpacerItem(110,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
	        Form1Layout.addMultiCell(spacingButtons,9,9,0,1)

	        self.buttonOk = QPushButton(self,"buttonOk")
	        self.buttonOk.setAutoDefault(1)
	        self.buttonOk.setDefault(1)

		'''########if not self.__creationmode:
			devtypeidlayout = wxBoxSizer(wxVERTICAL)
			devtypeidlayout.Add(self.__devicetypeidcaption)
			devtypeidlayout.Add(self.__devtypeidtxtbox)###############'''
			
	        Form1Layout.addMultiCellWidget(self.buttonOk,9,9,2,4)

	        self.buttonCancel = QPushButton(self,"buttonCancel")
	        self.buttonCancel.setAutoDefault(1)
		
		Form1Layout.addMultiCellWidget(self.buttonCancel,9,9,5,6)

		self.description = QTextEdit(self,"Description")

		Form1Layout.addMultiCellWidget(self.description,8,8,0,6)

		self.__nbrinputspinbutton = QSpinBox(self,"Inputnumber")

		Form1Layout.addWidget(self.__nbrinputspinbutton,6,1)
		spacinginputOutput = QSpacerItem(51,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
		Form1Layout.addMultiCell(spacinginputOutput,6,6,2,3)

		self.outputLabel = QLabel(self,"outputLabel")

		Form1Layout.addMultiCellWidget(self.outputLabel,6,6,4,5)

		self.__nbroutputspinbutton = QSpinBox(self,"Outputnumber")

		Form1Layout.addWidget(self.__nbroutputspinbutton,6,6)

		self.addsybsystemLabel = QLabel(self,"addsybsystemLabel")

		Form1Layout.addMultiCellWidget(self.addsybsystemLabel,4,4,0,1)

		self.inputLabel = QLabel(self,"inputLabel")

		Form1Layout.addWidget(self.inputLabel,6,0)

		self.descriptionLabel = QLabel(self,"descriptionLabel")

		Form1Layout.addMultiCellWidget(self.descriptionLabel,7,7,0,1)
		spacingNameColor = QSpacerItem(31,20,QSizePolicy.Minimum,QSizePolicy.Minimum)
		Form1Layout.addItem(spacingNameColor,1,3)

		self.__systemnamecaption = QLabel(self,"__systemnamecaption")

		Form1Layout.addMultiCellWidget(self.__systemnamecaption,2,2,0,1)		
		
		self.__devicetypetxtbox = QLineEdit(self,"devicetypenameEdit")

		Form1Layout.addMultiCellWidget(self.__devicetypetxtbox,1,1,0,2)

		self.__systemnametxtbox = QLineEdit(self,"__systemnametxtbox")
		Form1Layout.addMultiCellWidget(self.__systemnametxtbox,3,3,0,6)				

		self.__sysnamescmbbox = QListBox(self,"__sysnamescmbbox")

		Form1Layout.addMultiCellWidget(self.__sysnamescmbbox,5,5,0,6)

		self.__colorbutton = QPushButton(self,"__colorbutton")

		Form1Layout.addMultiCellWidget(self.__colorbutton,1,1,4,6)

		self.__devicetypenameLabel = QLabel(self,"DeviceNameLabel")

		Form1Layout.addMultiCellWidget(self.__devicetypenameLabel,0,0,0,1)

		self.colorLabel = QLabel(self,"colorLabel")

		Form1Layout.addMultiCellWidget(self.colorLabel,0,0,4,5)
		self.languageChange()
		
		self.connect(self.buttonCancel,SIGNAL("released()"),self.reject)
		self.connect(self.buttonOk,SIGNAL("released()"),self.OkButtonClicked)
		self.connect(self.__colorbutton,SIGNAL("released()"),self.getColor)
		self.connect(self.__sysnamescmbbox,SIGNAL("doubleClicked(QListBoxItem*)"),self.AddSubSystem)
		
	#### END GUI PART
		self.__devicetypes = devicetypes
		self.__creationmode = creationmode
		self.main = parent # MainWindow is _always_ the parent.
                                                                                                                             
                if creationmode == True:
                        title = "Create A DeviceType" # Title bar title
			selectedsystem = selectedsystem
                else:
                        title = "Modify A DeviceType"
			self.devicetypeobj = devicetypeobj # The device type object to modify
			selectedsystem = self.devicetypeobj.GetSystem() # System(s) this device type is stored in
			self.renameobj = None # A rename object in case we rename the device type
			self.devicetypeid = self.devicetypeobj.GetDeviceTypeID() # ID the device type is given in ConfDB (or -1 if not)
		self.__systemnametxtbox.setText(selectedsystem)
		self.setCaption(title)
		
		tmpsystems = self.main.GetCdb().GetSubSystems() # All available subsystems in ConfDB, as objects
		subsystems = [] # Store the names of the available subsystems here
		for tmp in tmpsystems:
			subsystems.append(tmp.GetName())

		if self.__creationmode: # Possibility to change/add subsystems if the device type is created
			self.__sysnamescmbbox.clear()
			# Add subsystemname to list of subsystems where this devicetype will be stored in
			for subsystem in subsystems:	self.__sysnamescmbbox.insertItem(subsystem)
		else:
			self.__systemnametxtbox.setEnabled(False) # Can not change if device type is being modified
		
		if not self.__creationmode: # Device type name and ID if being modified
			selected = self.devicetypeobj.GetName()
			selid = self.devicetypeobj.GetDeviceTypeID() #not visible if not stored in db yet
		else:
			selected = ""
			style = 0

		if not self.__creationmode:  ### 1 un champs en plus quand on modifie ?????
			self.__devicetypeidcaption = wxStaticText(self.__panel,-1,"Device Type ID:")
			self.__devtypeidtxtbox = wxTextCtrl(self.__panel,DEVTYPEID,str(selid),wxDefaultPosition,wxSize(50,-1),style=wxTE_READONLY)
			self.__devtypeidtxtbox.Disable()

		#2 spin wheels, select number of input and output to this device type
		if creationmode:
			val_in = 0
			val_out = 0
		else:
			val_in = int(devicetypeobj.GetPortsIn())
			val_out = int(devicetypeobj.GetPortsOut())

		if creationmode == True:
			self.__colorselected = (255,0,0) #default: red
                        self.__colorbutton.setPaletteBackgroundColor(Qt.red)
                else:
			tmpcolor = devicetypeobj.GetColour()
                        self.__colorbutton.setPaletteBackgroundColor(tmpcolor)
			self.__colorselected = ColorToTuple(rmpcolor)
		#description text area
		if creationmode == False:
			self.description.setText(devicetypeobj.GetDescription())

	
	def getColor(self):
			newColor =  QColorDialog.getColor(Qt.red,self)
			if newColor.isValid():
				self.__colorselected = ColorToTuple(newColor)
				self.__colorbutton.setPaletteBackgroundColor(newColor)

	def languageChange(self):
		self.buttonOk.setText("&OK")
		self.buttonOk.setAccel(QString.null)
		self.buttonCancel.setText("&Cancel")
		self.buttonCancel.setAccel(QString.null)
		self.outputLabel.setText("Output ports:")
		self.addsybsystemLabel.setText("Add to subsystem:")
		self.inputLabel.setText("Input ports:")
		self.descriptionLabel.setText("Description :")
		self.__systemnamecaption.setText("Create in:")
		self.__colorbutton.setText("Color")
		self.__devicetypenameLabel.setText("Device Type Name:")
		self.colorLabel.setText("Choose color:")

	def SaveObject(self):
		""" If standard validation was successful, this method is called 
		    when the user clicks on the Ok button. Here we create the 
		    device type object (and a rename object if renamed), and
		    do some more excessive validation on all values set for
		    this object.

		    !return -  False if unsuccessful, True if successful
		"""

		trackValueError = 0 # Needed so that we can track down where the ValueError occured (3 possibilities)

		if self.__creationmode:
			create = True
		else:
			create = False

		# We set up a try-catch to catch validation or other (value) errors that may occur.
		try:
			if self.__creationmode: # Create
				if str(self.__devicetypetxtbox.text()) in self.__devicetypes:
					raise ValidationError,"The devicetype cannot be created as the name you have given is already in use."
			else: # Modify
				if (self.__devicetypetxtbox.text()) != self.devicetypeobj.GetName(): # Renamed
					ans = wxMessageBox("You have renamed the device type, was that your intention? Remember that you can not undo renaming, \nbut you can rename the object as many times as you want later on.","Rename?",wxYES_NO | wxICON_QUESTION)
					if ans == wxNO:
						self.__devicetypetxtbox.SetValue(self.devicetypeobj.GetName())
						return False # User didn't want to rename; return and let user make up his mind
					if self.__devicetypetxtbox.GetValue() in self.__devicetypes:
						raise ValidationError,"The device type cannot be created as the name you have given is already in use."
					# Modify, and the user renamed the device; we create a rename object to put in the rename objects list
					self.renameobj = DeviceType(self.__systemnametxtbox.GetValue(),self.__devicetypetxtbox.GetValue(),True)
					self.renameobj.SetOldName(self.devicetypeobj.GetName()) # The old name before rename
					self.renameobj.SetSaveStatus(RENAME) #RENAME status

			# Create or modify, we'll have to set all attributes/properties anyway in a "new" object
			self.devicetypeobj = DeviceType(str(self.__systemnametxtbox.text()),str(self.__devicetypetxtbox.text()),create)

			self.devicetypeobj.SetName(str(self.__devicetypetxtbox.text()))
			self.devicetypeobj.SetSystem(str(self.__systemnametxtbox.text()))
			self.devicetypeobj.SetPortsIn(int(str(self.__nbrinputspinbutton.text()))) # NB, Integer conversion
			trackValueError = 1
			self.devicetypeobj.SetPortsOut(int(str(self.__nbroutputspinbutton.text()))) # NB, Integer conversion
			trackValueError = 2
			self.devicetypeobj.SetDescription(str(self.description.text()))
			self.devicetypeobj.SetColour(self.__colorselected)
			
			# Modify: Set the device type id (if already stored in db)
			if not self.__creationmode:
				if str(self.devicetypeid) != "" and str(self.devicetypeid) != "-1":
					self.devicetypeobj.SetDeviceTypeID(int(self.devicetypeid)) # Convert to integer if it was string
			# Everything successfully finished; we may continue.
                	return True

                except ValidationError,err:
                        self.main.ShowError(str(err),ERR_ERROR,True)
                        return False
		
                except ValueError,err:
			if trackValueError == 0:
	                        self.main.ShowError("The value given for number of ports in is not an integer.",ERR_ERROR,True)
			elif trackValueError == 1:
	                        self.main.ShowError("The value given for number of ports out is not an integer.",ERR_ERROR,True)
			else:
	                	self.main.ShowError("The value given for the device type ID is not an integer.",ERR_ERROR,True)
                        return False

	def OkButtonClicked(self):
        	""" As a reaction to the event that the user pressed the Ok button.
		    We only do some simple validation here; f. ex. that the required fields
		    are set.
		    
		    !return - False if unsuccessful (will not close the window)
                """

                if str(self.__devicetypetxtbox.text()) == "":
                        self.main.ShowError("No device type name chosen.",ERR_ERROR,True) #force message
                        return False
		
                success = self.SaveObject() # More validation and Create device type object (and rename object)
                if not success:
                        return False
                else:
			self.accept()

	def AddSubSystem(self,item):
		""" Add a subsystem to the systemname txtbox of subsystems that can be chosen 
		    that this device type should be added to.
		"""
		
		if string.find(str(self.__systemnametxtbox.text()),str(item.text()),0) != -1:
			self.main.ShowError("Subsystem already added!",ERR_ERROR,False) # If we try to add a subsystem that is already in the subsystem txtbox
		else:
			self.__systemnametxtbox.setText(str(self.__systemnametxtbox.text()) + "," + str(item.text()))

	def IsCreationMode(self):
		""" Whether we are creating or modifying a device type.
		    !return - True if we are creating, False if we are modifying
		"""
                return self.__creationmode
                                                                                                                                                                       
        '''def ColorButtonClicked(self,event):
                """ User clicks on the colour button and chooses a color for the current device type.
                """
		
                colordialog = wxColourDialog(self)                                                                                                                                                       
                if colordialog.ShowModal() == wxID_OK:
                        colorobj = colordialog.GetColourData()
                        color = colorobj.GetColour()
                        self.__colorbutton.SetBackgroundColour(color)
                        self.__colorselected = wxColour(red=color.Red(),green=color.Green(),blue=color.Blue()) # We create a wxColour(R,G,B) object to store the color.
	'''		
	def GetDeviceTypeObj(self):
		""" Called from outside this class (by the caller of this window)
		    to get the created device type or the modified device type
		    (and renamed device type) to be able to take action on them.

		    !return - If creating device type: the device type object
		              If modifying device type: the device type object,
			      and a rename object if renamed (the rename object
			      is None if not renamed)
		"""
		
		if self.__creationmode:
			return self.devicetypeobj
		else:
			return self.devicetypeobj, self.renameobj

