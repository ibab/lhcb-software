import string

from wxPython.wx import *
from wxPython.grid import *

from cdbVisCore import *

##################################################
# ErrorMessageGrid Class			##
##################################################

class ErrorMessageGrid(wxGrid):
	''' A custom data grid class inherited from wxGrid to show 
	all information and error messages throughout the current session.
	'''
	
        def __init__(self,parent):
		''' Constructor.

		    Parameters;
		    @parent - the parent of this class (usually LogWindow)
		'''
		
                wxGrid.__init__(self,parent,-1,size=wxSize(540,200))
		
                self.CreateGrid(0,2)
                self.SetColLabelSize(20)
                self.SetRowLabelSize(0)
                self.SetColLabelValue(0,"Message Type")
                self.SetColLabelValue(1,"Message")
                self.EnableEditing(False) #Sets the whole data grid to read only


##################################################
# LogWindow Class				##
##################################################

# GUI CONSTANTS
MESSAGE_ID = 123
ERRORTYPES_ID = 124

class LogWindow(wxDialog):
	""" The whole window displaying the error message data grid and a possibillity
	to double-click on the items in this grid to view the whole message in a text
	box control.

	The error and information messages are loaded in from the log file of the current session.
	"""                                                                                                                                                   
        def __init__(self,parent,id,file):
		''' Constructor.

		    Parameters:
		    @parent - the parent of this window (usually MainWindow)
		    @id     - the id of this window (set by its parent)
		    @file   - the log session file for the current session (string)
		'''
		
		wxDialog.__init__(self,parent,id,"CdbVis Session log",wxDefaultPosition,wxSize(560,460))

		self.main = parent
		self.file = file

		# Panel
                self.__panel = wxPanel(self,-1,wxDLG_PNT(self,wxPoint(5,5)))

		# Message text box
		self.__messagecaption = wxStaticText(self.__panel,-1,"Message:")
	        self.__messagetxtbox = wxTextCtrl(self.__panel,MESSAGE_ID,"",wxPoint(-1,-1),size=wxSize(400,100),style=wxTE_READONLY | wxTE_MULTILINE)

		# Choose which messages to show (filter out)
		radiochoices = ["All","Info","Error","Critical"]
                self.__errortypes = wxRadioBox(self.__panel,ERRORTYPES_ID,"View errors of type:",wxDefaultPosition,wxDefaultSize,radiochoices,1,wxRA_SPECIFY_ROWS)
		EVT_RADIOBOX(self.__panel,ERRORTYPES_ID,self.OnErrorTypeChoice)

		# The error message grid
		self.__datagrid = ErrorMessageGrid(self.__panel)
		self.__infocaption = wxStaticText(self.__panel,-1,"Double-click on a message row to display the message in the text control above.")

		self.__okbutton = wxButton(self.__panel,wxID_OK,"&Ok")

		# Load the messages from the log file at startup of this window
		self.LoadLogFileData(self.__errortypes.GetSelection())
		
		# can double click on items in the error message grid
		EVT_GRID_CELL_LEFT_DCLICK(self.__datagrid,self.OnRowChoice)


		#Layout
		self.__mylayout = wxBoxSizer(wxVERTICAL)
		self.__mylayout.Add(self.__messagecaption)
		self.__mylayout.Add(self.__messagetxtbox)
		self.__mylayout.AddSpacer((-1,10))
		self.__mylayout.Add(self.__errortypes)
		self.__mylayout.AddSpacer((-1,10))
		self.__mylayout.Add(self.__datagrid)
		self.__mylayout.AddSpacer((-1,5))
		self.__mylayout.Add(self.__infocaption)

		buttonlayout = wxBoxSizer(wxHORIZONTAL)
		buttonlayout.AddSpacer((400,-1))
		buttonlayout.Add(self.__okbutton)
		
		self.__mylayout.AddSpacer((-1,5))
		self.__mylayout.Add(buttonlayout)

		self.SetSizer(self.__mylayout)
                self.SetAutoLayout(1)
                self.__mylayout.Fit(self.__panel)


	def LoadLogFileData(self,choice):
		""" Load the messages from the log session file
		and display them in the error message grid.

		Parameters:
		@choice - Which of the radioboxes that are checked ( what type(s) of messages to show)
		"""
		
		# Reset the message data grid
		nrofrows = self.__datagrid.GetNumberRows()
		while nrofrows > 0:
			self.__datagrid.DeleteRows(0,1)
			nrofrows -= 1
		
		# Open log session file
		try:
			file = open(self.file,"r")
		except IOError,err:
			self.main.ShowError("Log file not found!",ERR_ERROR,True)
			return False

		# Read from the log session file, but it is empty
		if file.read(1) == "":
			self.main.ShowError("Log file was empty!",ERR_ERROR,True)
			file.close()
			# self.LoadLogFileData(self.__errortypes.GetSelection()) #since we added a error message, show it :)
			# Can lead to neverending loop...therefore skipped
			return False
		file.close()

		# Read from the log session file, and it is not empty
		file = open(self.file,"r")
		for line in file:
			message_line = line.split("|") #split into 4 array indexes: index 1 contains now the errortype, index 3 the message
		
			msgtype = message_line[1].strip()

			# Add the messages to the message grid
			if (choice == 0) or (choice == 1 and msgtype == "Information") or (choice == 2 and msgtype == "Error") or (choice == 3 and msgtype == "Critical"):
				self.__datagrid.InsertRows(0,1) # 1 row at position 0
				self.__datagrid.SetCellValue(0,0,str(message_line[1]))
				self.__datagrid.SetCellValue(0,1,str(message_line[3]))

		file.close()
		self.__datagrid.AutoSizeColumns()
	
	
	def OnRowChoice(self,event):
		""" If one clicks on a message row, we will display the message
		row that was click in the text box..so that the user can read the
		whole message.
		"""
		
		row = event.GetRow()
	        self.__messagetxtbox.SetValue(self.__datagrid.GetCellValue(row,1))

	def OnErrorTypeChoice(self,event):
		""" If we change the selection of what kinds of error types to
		view and filter out; reload all data.
		"""
		
		errortype = self.__errortypes.GetSelection()
		self.LoadLogFileData(errortype)
