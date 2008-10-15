import string

from wxPython.wx import *

from cdbVisCore import *
from objectclasses import *
import helper

###################################################
# CreateLinkTypeWindow class			###
###################################################

# GUI CONSTANTS
LINKTYPEID = 102
LINKTYPENAME = 103
COLORBUTTON = 104
LINKTYPE_COMBO = 105
ID_ADD = 106
ID_REMOVE = 107
CHECK_COMP = 108
COMP_LINKTYPES = 109

class CreateLinkTypeWindow(wxDialog):
	""" The user creates or modifies a link type;
	of either simple link type or composite link type.
	A Composite link type is a link type that consists
	of several other simple link types.
	"""

	def __init__(self,parent,id,selectedsystem,creationmode=True,linktypeobj=None,linktypes=[]):
		""" Constructor.

		    Parameters:
		    @parent          - parent window to this window (usually MainWindow)
		    @id              - id of this window (set by its parent)
		    @selectedsystem  - the subsystem to create this link type in
		    @creationmode    - if True, then the link type is being created,
		                       if False the link type is being modified (already exists)
		    @linktypeobj     - if we're in modifying a link type, then we have to send a
		    		       link type object with all the variables set to be able to 
				       show the attributes to the user so he/she can modify them
		    @linktypes	     - a list of names of link types available in the ConfDB; to
		    		       check that we don't get name conflicts and also for adding
				       simple link types to a composite link type.
		"""

		self.__creationmode = creationmode
		self.main = parent
		self.__linktypes = linktypes
                                                                                                                                                                       
                if creationmode == True:
                        title = "Create A Link Type"
                else:
                        title = "Modify A Link Type"
			selectedsystem = linktypeobj.GetSystem()
			self.linktypeobj = linktypeobj
			self.renameobj = None # We need a rename object in case we rename the link type
			
			try:
				self.__linktypes.remove(linktypeobj.GetName()) # We remove this link type name from the list
										# of link types; we can not add this link
										# type as a part of a composite link type
										# for itself
			except ValueError,err:
				pass

		height = 200

		wxDialog.__init__(self,parent,id,title,wxDefaultPosition,wxSize(260,height))

		#Panel
		self.__panel = wxPanel(self,-1,wxDLG_PNT(self,wxPoint(5,5)))


		if not self.__creationmode: # Modify
			id = linktypeobj.GetID()
			self.__linktypeidcaption = wxStaticText(self.__panel,-1,"Link Type ID:")
			self.__linktypeidtxtbox = wxTextCtrl(self.__panel,LINKTYPEID,str(id),wxDefaultPosition,wxSize(50,-1))
			self.__linktypeidtxtbox.Enable(False)
			name = linktypeobj.GetName()
			style = wxCB_READONLY
		else: # Create
			name = ""
			my_links = []
			style = 0

		self.__linktypecaption = wxStaticText(self.__panel,-1,"Link Type Name(20):")
		self.__linktypetxtbox = wxTextCtrl(self.__panel,LINKTYPENAME,name,wxDefaultPosition,wxSize(120,-1))

		# Checkbox - composite? If yes, enable combo box
		self.__compositelinktype = wxCheckBox(self.__panel,CHECK_COMP,"Composite linktype?")
		EVT_CHECKBOX(self.__panel,CHECK_COMP,self.CheckboxClicked)

		self.__linktypescaption = wxStaticText(self.__panel,-1,"Created linktypes:")
		self.__linktypescmbbox = wxComboBox(self.__panel,LINKTYPE_COMBO,"",choices=self.__linktypes,size=wxSize(120,-1))
		self.__addbutton = wxButton(self.__panel,ID_ADD,"&Add",size=wxSize(60,-1))
		EVT_BUTTON(self.__panel,ID_ADD,self.OnAdd)
		self.__rembutton = wxButton(self.__panel,ID_REMOVE,"&Remove",size=wxSize(60,-1))
		EVT_BUTTON(self.__panel,ID_REMOVE,self.OnRemove)
		self.__complinktypes = wxTextCtrl(self.__panel,COMP_LINKTYPES,"",wxDefaultPosition,wxSize(220,50),style=wxTE_MULTILINE | wxTE_READONLY)

		self.CheckboxClicked(None) #set composite link type to disabled as default

		#OK and Cancel buttons
		self.__okbutton = wxButton(self.__panel,wxID_OK,"&Ok")
		self.__cancelbutton = wxButton(self.__panel,wxID_CANCEL,"&Cancel")

		EVT_BUTTON(self,wxID_OK,self.OkButtonClicked)

		#Layout
		self.__layout = wxBoxSizer(wxVERTICAL)
		buttonlayout = wxBoxSizer(wxHORIZONTAL)
		buttonlayout.Add(self.__okbutton)
		buttonlayout.Add(self.__cancelbutton)

		namelayout = wxBoxSizer(wxVERTICAL)
		namelayout.Add(self.__linktypecaption)
		namelayout.Add(self.__linktypetxtbox)
		linklayout = wxBoxSizer(wxHORIZONTAL)
		linklayout.Add(namelayout)
		if not creationmode:
			idlayout = wxBoxSizer(wxVERTICAL)
			idlayout.Add(self.__linktypeidcaption)
			idlayout.Add(self.__linktypeidtxtbox)
			linklayout.AddSpacer((10,-1))
			linklayout.Add(idlayout)
		self.__layout.Add(linklayout)

		linktypeslayout = wxBoxSizer(wxHORIZONTAL)
		linktypeslayout.Add(self.__linktypescmbbox)
		linktypeslayout.Add(self.__addbutton)
		linktypeslayout.Add(self.__rembutton)

		self.__layout.Add(self.__compositelinktype)
		self.__layout.AddSpacer((-1,5))
		self.__layout.Add(self.__linktypescaption)
		self.__layout.Add(linktypeslayout)
		self.__layout.Add(self.__complinktypes)

		self.__layout.AddSpacer((-1,5))
		self.__layout.Add(buttonlayout)

		self.SetSizer(self.__layout)
		self.SetAutoLayout(1)
		self.__layout.Fit(self.__panel)

	def CheckboxClicked(self,event=None):
		""" Enable/Disable the controls for composite link type.
		"""
		
		if self.__compositelinktype.GetValue() == False:
			self.__linktypescaption.Enable(False)
			self.__linktypescmbbox.Enable(False)
			self.__addbutton.Enable(False)
			self.__rembutton.Enable(False)
			self.__complinktypes.Enable(False)
		else:
			self.__linktypescaption.Enable(True)
			self.__linktypescmbbox.Enable(True)
			self.__addbutton.Enable(True)
			self.__rembutton.Enable(True)

	def OnAdd(self,event):
		""" Add a link type (selected in the combo box)
		to the collection of link types for the current 
		composite link type.
		"""
		
		val = self.__linktypescmbbox.GetValue()
		if string.find(self.__complinktypes.GetValue(),val,0) != -1:
			self.main.ShowError("The linktype is already added to the composite linktype.",ERR_ERROR)
		else:
			self.__complinktypes.SetValue(self.__complinktypes.GetValue() + val + ",")

	def OnRemove(self,event):
		""" Remove a link type (selected in the combo box)
		from the collection of link types for the current
		composite link type
		"""
		
		val = self.__linktypescmbbox.GetValue()
		if string.find(self.__complinktypes.GetValue(),val,0) == -1:
			self.main.ShowError("This linktype is not a part of the composite linktype, and cannot be removed.",ERR_ERROR)
		else:
			myvals = self.__complinktypes.GetValue()
			myvals = myvals.replace(val + ",","")
			self.__complinktypes.SetValue(myvals)

	def SaveObject(self):
		""" Excessive validation and create/modify the
		link type object.

		!return - True if successful, False if not.
		"""
		
                if self.__creationmode:
                        create = True
                else:
                        create = False

                try:
			if self.__creationmode: # Create
				if self.__linktypetxtbox.GetValue() in self.__linktypes:
					raise ValidationError, "Cannot create linktype as the name you chose is already in use."

			else: # Modify
				if self.__linktypetxtbox.GetValue() != self.linktypeobj.GetName():
					ans = wxMessageBox("You have renamed the link type, was that your intention? Remember that you can not undo renaming, \nbut you can rename the object as many times as you want.","Rename?",wxYES_NO | wxICON_QUESTION)
                                        if ans == wxNO:
                                                self.__linktypetxtbox.SetValue(self.linktypeobj.GetName())
                                                return False
					
                                        if self.__linktypetxtbox.GetValue() in self.__linktypes:
                                                raise ValidationError,"The linktype cannot be created as the name you have given is already in use."
					# Create the rename object since we renamed the link type
                                        self.renameobj = LinkType(self.main.GetActiveSystem(),self.__linktypetxtbox.GetValue(),True)
                                        self.renameobj.SetOldName(self.linktypeobj.GetName())
                                        self.renameobj.SetSaveStatus(RENAME)

			# Create the standard link type object (create or modify)
                        self.lktypeobj = LinkType(self.main.GetActiveSystem(),self.__linktypetxtbox.GetValue(),create)
                        self.lktypeobj.SetName(self.__linktypetxtbox.GetValue())
			
			# Create a composite link type (also if we change a simple link type to a composite link type)
			if self.__compositelinktype.GetValue() == True:
				if self.__complinktypes.GetValue() == "":
					raise ValidationError,"You have set this type as composite linktype, but you haven't added any linktypes to this linktype."
				else:
					self.lktypeobj.SetCompositeLinks(self.__complinktypes.GetValue())
				

			if not self.__creationmode and self.__linktypeidtxtbox != "":
					self.lktypeobj.SetID(int(self.__linktypeidtxtbox.GetValue())) # Set the ID, if already stored in ConfDB
					
	                return True                                                                                                                                                               
                except ValidationError,err:
                        self.main.ShowError(str(err),ERR_ERROR,True)
                        return False
                except ValueError,err:
                        self.main.ShowError("The link ID is not a legal integer value.",ERR_ERROR,True)
                        return False

	def OkButtonClicked(self,event):
        	""" Simple validation before trying to create the
		link type object.

		!return - False if unsuccessful, no return if successful; window is closed
                """
		
                if self.__linktypetxtbox.GetValue() == "":
			self.main.ShowError("The link type has to be given a name.",ERR_ERROR)
			return False

		success = self.SaveObject()
		if not success:
			return False

                event.Skip()

	def IsCreationMode(self):
		""" Whether we create or modify a link type
		
		!return - True if we create a link type, False if we modify
		"""
		
		return self.__creationmode

	def GetLinkTypeObj(self):
		""" Return link type object (and rename object).

		!return - The created link type object if we Create,
		The modified link type object if we modify (and a rename
		object if we renamed)
		"""
		
		if self.__creationmode:
			return self.lktypeobj
		else:
			return self.lktypeobj,self.renameobj
