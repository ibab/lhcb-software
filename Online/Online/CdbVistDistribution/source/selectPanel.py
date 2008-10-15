import string
import time

# from wxPython.wx import *
# from wxPython.lib.mixins.listctrl import wxColumnSorterMixin
from qt import *

from cdbVisCore import *
from objectclasses import *
import visWindow
import pdb
        

##########################################################################
# SelectWindow class
##########################################################################

# GUI CONSTANTS
ID_COMMIT_BUTTON = 451
ID_LINKTYPECHOICE = 452
ID_LINKTYPERESTRICT = 453
ID_TREE_CTRL  = 454

ID_SPAREPARTSCHOICE = 455


listviewIcon0_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\xdd\x49\x44\x41\x54\x78\x9c\xad\x93\x31\x0a\xc2" \
    "\x30\x14\x40\x5f\x4b\xd1\x03\xa8\x20\x14\x14\x71" \
    "\x71\x70\x73\x72\xeb\xaa\x6e\x05\x4f\xa1\x77\xd0" \
    "\xde\x41\x4f\x21\xb8\x89\xab\x93\xe0\x50\x90\x42" \
    "\x47\x1d\x04\xa1\x20\xe2\xec\xa2\x4e\x5f\x6a\x9a" \
    "\x88\xa8\x6f\x4b\xf2\xdf\x23\x94\x06\x7e\xc4\x32" \
    "\x1d\x4c\xe6\x83\x7b\x7a\x3d\xf4\xa7\xda\xd9\xcc" \
    "\xa6\x88\x8d\x6a\x8b\x9a\x5b\x05\x20\xde\x6f\x39" \
    "\x24\x7b\x6d\xc8\xd2\x89\xed\x66\x17\x80\xe4\x12" \
    "\xbf\xc4\x75\x21\x4b\xe4\xb4\x28\xa8\x01\x35\x34" \
    "\xf4\xa7\x96\x2d\x9b\xb9\xe2\x86\x30\x19\x69\x85" \
    "\x34\xcb\xf5\xfc\x79\x0b\x00\x47\x1d\x90\x48\xab" \
    "\x1c\x64\x44\x1d\x99\x80\x1a\x8a\x42\xe3\x08\x00" \
    "\xf6\xdb\xd3\x0f\xf8\x5f\xe0\x78\xf8\xae\x65\x03" \
    "\xd4\xf3\x5e\x70\x3e\xd9\x44\xa1\xf3\x71\xa8\x9e" \
    "\xf7\x02\x48\xfd\x48\xcb\xc5\x6c\x0c\xb0\xbb\xae" \
    "\x46\x00\x85\xd2\x0d\xb7\x72\xcb\x7c\x44\x11\x3b" \
    "\xbd\xfe\xf8\x25\x60\x0a\x99\x44\xc1\xf8\x98\x24" \
    "\x24\xa8\xe2\xdf\x78\x00\x98\xd4\x59\xeb\xa4\x1a" \
    "\x1a\x26\x00\x00\x00\x00\x49\x45\x4e\x44\xae\x42" \
    "\x60\x82"
listviewIcon1_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\xe0\x49\x44\x41\x54\x78\x9c\xc5\x93\xbd\x0a\xc2" \
    "\x30\x14\x85\x4f\xb5\xbe\x85\xbb\x2e\x5d\xea\x24" \
    "\x14\x71\xab\x3f\x73\xf3\x16\x7d\x06\xeb\x33\xe4" \
    "\x2d\xe2\x5c\x6d\x37\x91\x80\x8b\x76\x71\xb0\xee" \
    "\x2e\x52\xc1\xad\x28\x15\xea\x62\x82\x85\x0a\x89" \
    "\x8b\x67\xba\x43\xbe\xef\x90\x0b\x17\xf8\x77\x0c" \
    "\x31\xf8\xbe\x5f\xea\x80\x94\x52\x03\x00\x4c\x01" \
    "\x77\x1d\x07\x7d\x42\x94\xe0\x2d\x63\xf0\x81\x92" \
    "\x52\x6a\x34\x74\x5a\xeb\x62\x8a\x21\xe5\x1c\x29" \
    "\xe7\xbf\x0b\x7a\x8e\x89\x11\x69\x29\x41\x11\x2b" \
    "\xb0\xe7\xcf\xaa\x20\xbf\x17\xb8\x5c\x1f\x4a\x82" \
    "\xfc\xde\x80\xd8\xbf\x14\x1c\x77\x06\x8e\xbb\xa6" \
    "\x92\xe0\x33\x52\xe0\xb4\x0f\x20\x9d\xb5\x12\xc4" \
    "\x4e\x43\xf0\xb3\x55\x15\xe0\x01\xe0\xa6\x58\xfb" \
    "\xf1\x53\x29\xe0\x99\x05\x9e\x59\x8a\x86\x1a\x81" \
    "\x6d\xdb\x70\x5d\x57\x09\x8a\xe3\x18\x49\x92\x00" \
    "\x78\xaf\x72\x15\x2e\x82\x65\xb4\x99\xe9\x34\x4f" \
    "\x46\x83\xf9\x78\xea\x05\xf2\x16\x56\xe1\x22\xd0" \
    "\x11\x8c\xa7\x9e\xd6\xfb\xaf\x79\x01\x74\x09\x41" \
    "\x81\x95\x44\x4b\x30\x00\x00\x00\x00\x49\x45\x4e" \
    "\x44\xae\x42\x60\x82"
listviewIcon2_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\xeb\x49\x44\x41\x54\x78\x9c\xc5\x92\xbf\x8a\x02" \
    "\x31\x10\xc6\xbf\x93\x7b\x83\x7b\x89\x45\x52\x2e" \
    "\x6c\xb5\x60\x2f\xf8\x00\xc2\x59\x7a\x82\xa5\x8a" \
    "\x36\x22\x56\xba\x78\x8d\x9d\x6e\x61\x7f\xcd\xbd" \
    "\xc0\x36\xfb\x00\xfb\x44\x99\x4c\x62\xb5\xc1\x6c" \
    "\x56\x36\x56\x0e\x04\x32\x7f\xbe\xdf\xcc\x90\x00" \
    "\xef\xb6\x8f\x47\x67\x7d\xba\x98\x10\x51\xb6\x9a" \
    "\x59\xdd\x67\x33\x79\x5c\xfe\x78\x02\x63\x0c\xb4" \
    "\xd6\xd0\x5a\x63\x7b\xbe\x39\x39\x0f\x00\x00\x9b" \
    "\xdf\x6b\x6b\xe7\xdd\x7c\xe2\xc5\x5a\x01\x87\xc5" \
    "\xd4\x76\xac\x0f\x33\x43\x4a\xd9\x0d\x60\x66\xe4" \
    "\x7f\xa5\xf5\xbf\x47\x09\x8a\xa2\x00\x00\xa4\x69" \
    "\xea\x01\x7a\xcd\x80\x52\xca\xf1\x89\xc8\x0a\x83" \
    "\x26\x20\x22\x8c\x87\xb1\x33\x36\x33\x23\x8e\x63" \
    "\x10\x51\x18\xa0\x2c\x4b\xaf\x10\x00\x84\x10\xdd" \
    "\x00\x29\x25\x92\x24\x01\x33\xdb\x29\xea\x7b\x73" \
    "\xbd\xa7\x80\xaa\xaa\x9c\x58\x14\x45\xf6\x35\x82" \
    "\x56\x10\x42\x38\x4f\xa8\x94\x82\x31\xed\x9f\xd4" \
    "\x03\xe4\xff\x45\x6b\xe1\x33\x73\x00\x83\xfe\xd7" \
    "\x3e\x44\x94\xbd\xd4\xa2\xc3\xee\xfb\xcf\x88\x6e" \
    "\x06\x74\xb3\x65\x00\x00\x00\x00\x49\x45\x4e\x44" \
    "\xae\x42\x60\x82"
listViewEmptyDevice_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\x97\x49\x44\x41\x54\x78\x9c\x63\x60\x18\x68\xc0" \
    "\x88\xcc\x79\x75\xa9\xf6\x3f\x21\x0d\x62\x7a\xcd" \
    "\x28\x7a\xe0\x9c\x57\x97\x6a\xff\xb3\x70\x2a\x30" \
    "\xb0\xf1\xe9\xe1\xd4\xfc\xeb\xd3\x25\x86\x3f\xdf" \
    "\x1f\xa0\x18\xc2\x82\xac\x00\xa6\x19\xa6\x10\xae" \
    "\x08\x6a\x30\x1b\x9f\x1e\x8a\x38\x86\x01\x30\xf0" \
    "\xe7\xfb\x03\x86\xd7\x8c\x3e\x8d\x30\xbe\xe8\xf7" \
    "\x2d\xf5\xb8\x5c\x86\xd5\x00\x06\x06\x06\x06\x6d" \
    "\x5d\xf3\x06\x18\xfb\xd5\xa5\xda\x7a\x5c\xea\xb0" \
    "\x1a\xc0\xc2\xa9\x80\x12\xa0\x2c\x9c\x0a\xb8\xf4" \
    "\x63\x37\x00\xe6\x5f\x62\x00\x13\x51\xaa\x86\xb7" \
    "\x01\x28\x81\xf8\xed\xe5\x26\xf2\x0d\x40\x4e\x38" \
    "\x84\x41\x33\xc9\x16\xe1\x04\x00\x9f\x05\x2b\x35" \
    "\x07\xd6\xdf\x91\x00\x00\x00\x00\x49\x45\x4e\x44" \
    "\xae\x42\x60\x82"
listViewDevice_data = \
    "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
    "\x49\x48\x44\x52\x00\x00\x00\x10\x00\x00\x00\x10" \
    "\x08\x06\x00\x00\x00\x1f\xf3\xff\x61\x00\x00\x00" \
    "\x96\x49\x44\x41\x54\x78\x9c\x63\x60\x18\x68\xc0" \
    "\x88\xcc\x39\xbd\x21\xe8\x3f\x21\x0d\xa6\x01\xeb" \
    "\x50\xf4\xc0\x39\xa7\x37\x04\xfd\xe7\x17\x90\x63" \
    "\x10\x16\x51\xc2\xa9\xf9\xed\x9b\x7b\x0c\x1f\x3f" \
    "\x3c\x42\x31\x84\x05\x59\x01\x4c\x33\x4c\x21\x0c" \
    "\xc0\x0c\x16\x16\x51\x42\x11\xc7\x30\x00\x06\x3e" \
    "\x7e\x78\xc4\xf0\x86\x35\xb2\x11\x2e\xf0\x61\x79" \
    "\x3d\x2e\x97\x61\x35\x80\x81\x81\x81\xc1\xd3\x3b" \
    "\xb4\x01\xc6\x3e\xbd\x21\xa8\x1e\x97\x3a\xac\x06" \
    "\xf0\x0b\xc8\xa1\x04\x28\xbf\x80\x1c\x2e\xfd\xd8" \
    "\x0d\x80\xf9\x97\x18\xc0\x44\x94\xaa\xe1\x6d\x00" \
    "\x4a\x20\xde\xbb\x73\x80\x7c\x03\x50\x12\x0e\x41" \
    "\xb0\x8e\x64\x8b\x70\x02\x00\x78\x5f\x2a\xe9\x56" \
    "\xf2\x43\xfc\x00\x00\x00\x00\x49\x45\x4e\x44\xae" \
    "\x42\x60\x82"


class selectWindow(QWidget,dbUpdate): # (wxPanel, dbUpdate, wxColumnSorterMixin):
    """ Class responsible for the tree view and information window and
    the path window that pops up for a device when set to path view.
    """

    def __init__(self,main,parent):
        """ Constructor

	    Parameters:
            @parent - the parent window (mainPanel)
            @id     - the window id set by parent
            @main   - main program object (mainWindow), 
	              for communication with instances of other classes
        """
	QWidget.__init__(self,parent)
        self.main = main

	# The path Window
        self.__path = None	# The currently selected path for a device
        self.selectPathFrame = None # The instance of the path window
        
        # Layout and controls of the select window
        self.sizer = QVBoxLayout(self) #QVBox(self)
	self.border = 5

	#Tree Ctrl -> set up the first node: Subsystems
# 	self.sizer.Add(wxStaticText(self,-1,"LHCb System Treeview:"),0,wxEXPAND | wxALL,self.border)
	#label=QLabel("LHCb System Treeview:",self)
	#label.setMinimumSize(QSize(250,20))
	self.sizer.addWidget(QLabel(self,"LHCb System Treeview:"))
	
	self.systemIcon = QPixmap()
	self.systemIcon.loadFromData(listviewIcon0_data,"PNG")
	self.deviceTypesIcon = QPixmap()
	self.deviceTypesIcon.loadFromData(listviewIcon1_data,"PNG")
	self.linkTypesIcon = QPixmap()
	self.linkTypesIcon.loadFromData(listviewIcon2_data,"PNG")
	self.listViewEmptyDevice = QPixmap()
	self.listViewEmptyDevice.loadFromData(listViewEmptyDevice_data,"PNG")
	self.listViewDevice= QPixmap()
	self.listViewDevice.loadFromData(listViewDevice_data,"PNG")

	
# 	self.treectrl = wxTreeCtrl(self,ID_TREE_CTRL, wxDefaultPosition,wxSize(250,300),style=wxTR_HAS_BUTTONS	| wxTR_HAS_VARIABLE_ROW_HEIGHT | wxTR_SINGLE | wxRAISED_BORDER)
# 	self.sizer.Add(self.treectrl,0,wxEXPAND | wxALL,self.border)
	self.treectrl = QListView(self)
	self.treectrl.setMinimumSize(QSize(250,300))
	self.sizer.addWidget(self.treectrl)
	
	self.__treeitemids = {} # dictionary containing <treenodename : treenodeid>, because if we want to refer to a 
				# tree node, we have to use the treenode id, therfore all treenodes in the tree view
				# are added to the dictionary like this as soon as they are added.
	rootdata = TN_SYSTEM_HEADER # tree node type for Subsystems; (to f.ex. assign icon to the type)
	
	self.treectrl.addColumn("Subsystems")
	self.treeroot = QListViewItem(self.treectrl,"Subsystems",str(rootdata)) # self.treectrl.AddRoot("Subsystems",-1,-1,wxTreeItemData(rootdata)) # Root: Subsystems, we get the
											   # tree node id in return
	self.treeroot.setPixmap(0,self.systemIcon)
	self.__treeitemids["Subsystems"] = self.treeroot
        self.treectrl.setEnabled(False)
        
        self.connect(self.treectrl,SIGNAL("mouseButtonClicked(int,QListViewItem*,const QPoint&,int)"),self.OnClick) # EVT_TREE_ITEM_ACTIVATED(self,ID_TREE_CTRL,self.OnChoice) #double click; if not expanded, expand
        self.connect(self.treectrl,SIGNAL("doubleClicked(QListViewItem*,const QPoint&,int)"),self.OnChoice) # EVT_TREE_ITEM_ACTIVATED(self,ID_TREE_CTRL,self.OnChoice) #double click; if not expanded, expand
        self.connect(self.treectrl,SIGNAL("expanded(QListViewItem*)"), self.OnExpanding) # EVT_TREE_ITEM_EXPANDING(self,ID_TREE_CTRL,self.OnExpanding) #prevent expand, and do it ourselves
        #self.connect(self.treectrl,SIGNAL("selectionChanged(QListViewItem*)"), self.slotOnSelect) # EVT_TREE_SEL_CHANGED(self,ID_TREE_CTRL,self.OnSelect) #one click, do not expand  
	      
	# Filter by spare parts/non-spare parts
	# Spare parts -> view only spare parts
	options = ['All','Spares only','In place only']
#         self.sparepartsCtrl = wxComboBox(self, ID_SPAREPARTSCHOICE, options[0], size = (180, 25), style = wxCB_READONLY | wxCB_SORT, choices = options)
	self.sizer.addWidget(QLabel(self, "Filter by spare parts"))
        self.sparepartsCtrl=QComboBox(False, self)
	self.sparepartsCtrl.setEnabled(True) # self.sparepartsCtrl.Enable(True)
	self.sparepartsCtrl.insertItem("All")
	self.sparepartsCtrl.insertItem("Spares only")
	self.sparepartsCtrl.insertItem("In place only")
	self.sparepartsCtrl.setGeometry(0,0,250,25)
	self.sizer.addWidget(self.sparepartsCtrl)

	
#         EVT_COMBOBOX(self, ID_SPAREPARTSCHOICE, self.OnRestrictedSparePartChoice)

        # Link Type -> view only links of given link type
	self.linktyperestrictchk = QCheckBox(self,"View only links of this linktype:")
	self.linktyperestrictchk.setText("View only links of this linktype:")
	self.linktyperestrictchk.setGeometry(0,0,250,25)
	self.linktyperestrictchk.setChecked(False)
	self.linktyperestrictchk.setEnabled(False)
	self.connect(self.linktyperestrictchk,SIGNAL("toggled(bool)"),self.OnCheckRestrict)
	self.sizer.addWidget(self.linktyperestrictchk)
	
	self.main.RestrictLinkType(False) #make linktype checkbox take effect
	
	#EVT_CHECKBOX(self,ID_LINKTYPERESTRICT,self.OnCheckRestrict)
	
        
       
#         EVT_COMBOBOX(self, ID_LINKTYPECHOICE, self.OnRestrictedLinkTypeChoice)
#         self.sizer.Add(self.linktypeCtrl, 0, 
#                        wxEXPAND | wxLEFT | wxTOP | wxRIGHT, self.border)
	
        self.linktypeCtrl = QComboBox(self, None) #self.linktypeCtrl = wxComboBox(self, ID_LINKTYPECHOICE, "", size = (180, 25), style = wxCB_READONLY | wxCB_SORT)
	self.linktypeCtrl.setEnabled(False)
	self.linktypeCtrl.insertItem("Default")
	self.linktypeCtrl.setGeometry(0,0,200,25)
	self.sizer.addWidget(self.linktypeCtrl)
	self.connect(self.linktypeCtrl,SIGNAL("activated(int)"),self.OnRestrictedLinkTypeChoice)
         # Information window
	
	self.sizer.addWidget(QLabel(self, "Information"))
	 #self.sizer.Add(wxStaticText(self, -1, \
# 	"Information"), 0, wxEXPAND | wxLEFT | wxTOP, self.border)
# 	self.infoCtrl = wxListCtrl(self, ID_DEVINFO, size = (180,150), 
# 	style = wxLC_REPORT | wxLC_SINGLE_SEL | \
# 	wxLC_HRULES | wxLC_VRULES | wxSIMPLE_BORDER) # Report view, like a list
	self.infoCtrl = QListView(self)
	self.infoCtrl.addColumn("Attribute")
	self.infoCtrl.addColumn("Value")
	self.infoCtrl.setAllColumnsShowFocus(True)
	self.sizer.addWidget(self.infoCtrl)
	
	self.connect(self.infoCtrl,SIGNAL("doubleClicked(QListViewItem*,const QPoint&,int)"),self.InfoCtrlDoubleClick)
# 	EVT_LIST_ITEM_SELECTED(self,ID_DEVINFO,self.OnDeviceInfoSelected)
# 	EVT_LIST_ITEM_ACTIVATED(self,ID_DEVINFO,self.OnDeviceInfoSelected)

	#WALID_OBSELETE? self.my_data = [] # Needed to sort the list ctrl
	self.itemDataMap = {} # This is also needed for sorting the list ctrl

	self.prevvalue = ""
	self.prevrestrict = False

    def myCollapseAndReset(self, item):
	for i in range(item.childCount()):
		tmp_ = item.firstChild()
		item.takeItem(tmp_)
		tmpText_=str(tmp_.text(0))
		if tmpText_ in self.__treeitemids.keys():
			del self.__treeitemids[tmpText_]
			
    def CollapseInactiveSystems(self, item):
	for i in range(item.childCount()):
		tmp_ = item.firstChild()
		item.takeItem(tmp_)
		tmpText_=str(tmp_.text(0))
		if tmpText_ in self.__treeitemids.keys():
			del self.__treeitemids[tmpText_]
			
    def InfoCtrlDoubleClick(self,item,point,col):
	if col==0:
		return
	if item and str(item.text(0))=="url" and str(item.text(1))!="":
		try:
			webbrowser.open_new(str(item.text(1)))
		except:
			self.main.ShowError("Cannot Open the file",ERR_ERROR,True)
	# elif col==1 and item:
		# item.setRenameEnabled(1,True)
		# item.startRename(1)
		# print "rename"

    def GetItemID(self,value):
	""" Since we need the tree node/item id to do something with
	a tree node, this function will return the tree node id in
	the tree view, if it exists.

	Parameters:
	@value - The name of the tree node as seen by the user.

	!Return - The tree node id as a TreeItemID Object, or a KeyError Exception if
		  an error occurs.
	"""
	
	try:
		if self.__treeitemids[value].__class__ != QListViewItem: #or (self.__treeitemids[value].__class__ == QListViewItem and not self.__treeitemids[value].IsOk())
				# We check whether we have added the tree node to the tree view, if it is
				# of the correct type (treeitemidptr) and that the value is valid
				# If any of these fail, a keyerror is arised.
			raise KeyError, "We did not find a valid TreeItemID for the current object."
		
		else: # We have a valid tree item id, return it
		   return self.__treeitemids[value]
	   
	except KeyError,err:
		raise KeyError,err # We just throw the KeyError further up..
	
    def RemoveItem(self,id,parent):
	""" Remove a tree node/item from the tree view. Usually because
	the object has been deleted by the user.

	Parameters:
	@id     - the tree node id of the item that we want to delete
	@parent - the tree node name/string of the parent to the item that
	we want to delete.

	!Return - Raise a KeyError exception if some error occured
	"""
	
	self.main.RefreshObjectBoxData() # We removed some object; need to update combo boxes etc.
	
	try:
		parentid = self.GetItemID(parent) # tree node id of the parent
		#if self.treectrl.IsExpanded(parentid): # if parent is expanded; we can delete the child
		self.treectrl.Delete(id)
		
	except KeyError,err:
		raise KeyError,"We could not delete the object from the tree view: " + str(err)

    def SetItemToDeleted(self,item):
	""" We set the tree item node entry in the dictionary to -1, which
	is not a valid treeitemID, because the tree item node has been deleted
	from the tree view.

	Parameters:
	@item - tree item name/string of the deleted object
	"""
	self.__treeitemids[item] = -1

    def CollapseAndExpand(self,id,doexpand=True,set_to_minus_one=True):
	""" Sometimes we need to collapse a tree node, and then expand it again
	automatically; f.ex. if new nodes have been added as children of the tree
	node in question, or children have been deleted.

	We therefore need a new expand to sort the tree node names alphabetically.

	Parameters:
	@id - tree node id of the tree node that should be collapsed and expanded.
	@doexpand - Whether the node should be expanded after the collapse or not.
	@set_to_minus_one - Whether the level in the visual window should be set to
	device level (-1) or not.
	"""
	
	try:
		self.treectrl.CollapseAndReset(id) #Collapse the node and remove all its child nodes
    		self.OnChoice(None,False,id,doexpand,set_to_minus_one)
	except KeyError,err:
		self.main.ShowError("A TreeView error occured: " + str(err),ERR_ERROR)
		
    def AddItem(self,item,type):
	""" Add an item (DeviceType, Device) to the tree.
	    type : the type of the item : TN_DEVICETYPE, TN_DEVICE
	"""
	for system in item.GetSystem().split(","):
		if type == TN_DEVICETYPE:
			itemData = str(TN_DEVICETYPE)+"|"+system
		elif type == TN_DEVICE:
			itemData = str(TN_DEVICE)+"|"+system
		elif type == TN_PORT:
			itemData = str(TN_PORT)+"|"+system
		else:
			return
		try:
			res = self.treectrl.findItem(itemData,1)
			parent = res.parent()
			newItem = QListViewItem(parent,item.GetName(),itemData)
		except:
			print "Item : "+item.GetName()+" was not added to the navigation Tree"
		# mySystem = self.treectrl.find(system,0)
		# if type == TN_DEVICETYPE:
			# newItem = QListView(self.treectrl.find("Device types",1),item.GetName(),
    '''def AddItem(self,id,itm,type):
	""" Add a tree node to the tree view. Usually because a new object
	has been created, or that an object has been renamed.

	Parameters:
	@id - tree node ID of the parent to the new node that will be added
	@itm -  the tree node name/string to be added
	@type - the tree node type that will be added (Device Type, Device, Link etc)
	"""
	
	## self.main.RefreshObjectBoxData() # If we added a new node; we will have to refresh combo boxes with new data

	try:
		if self.treectrl.IsExpanded(id): # We only add a new child node if the parent is expanded
			system = str(self.treectrl.GetPyData(id)).split("|")[1] # get the subsystem for the node
			itmid = self.treectrl.AppendItem(id,itm,data=wxTreeItemData(str(type) + "|" + str(system))) # add
			self.__treeitemids[itm] = itmid # tree item id to dictionary
    			self.treectrl.SortChildren(id) # sort the children alphabetically
	except KeyError,err:
		self.main.ShowError("A TreeView error occured: " + str(err),ERR_ERROR)
	'''
    def OnCheckRestrict(self,event):
	""" When a user clicks on the check box to restrict the view of the links
	to a given link type.
	"""

	if self.linktyperestrictchk.isChecked(): # Restrict
        	#self.linktypeCtrl.Enable(True)
		if self.linktypeCtrl.currentText() != "":
			self.main.RestrictLinkType(True) #make linktype checkbox take effect
			self.main.SetRestrictedLinkType(self.linktypeCtrl.currentText())
		else:
			self.main.RestrictLinkType(False) #make linktype checkbox take effect
			self.main.SetRestrictedLinkType("")
	else: # No restriction
        	#self.linktypeCtrl.Enable(False)
		self.main.RestrictLinkType(False) #make linktype checkbox take effect
		self.main.SetRestrictedLinkType("")
	
	res = self.main.GetVisWindow().ViewOnlyLinksOfGivenType()
	self.main.GetVisWindow().canvas.update()
	# if res == False:
		#self.linktyperestrictchk.setChecked(False)
		# self.main.SetRestrictedLinkType("")
		# self.main.RestrictLinkType(False) #make linktype checkbox take effect
		
    def OnRestrictedSparePartChoice(self,event=''):
        """ When user changes spare parts view """
	self.main.SetRestrictedSparePart(self.sparepartsCtrl.currentText()) #sets the desired view
	self.SystemChanged() #collapses tree view and re-opens it after updating contents

    def OnRestrictedLinkTypeChoice(self,event):
	""" When a user changes restriction of the link type in the combo box, to
	another link type.
	"""
        #self.linktypeCtrl.Enable(True)
	if not self.linktyperestrictchk.isChecked():
		return 
	self.main.RestrictLinkType(self.linktyperestrictchk.isChecked()) #make linktype checkbox take effect
	self.main.SetRestrictedLinkType(str(self.linktypeCtrl.currentText()))
	
	res = self.main.GetVisWindow().ViewOnlyLinksOfGivenType()
	self.main.GetVisWindow().canvas.update()
	## Je ne vois pas trop a quoi ca sert ce qu'il a ici en bas alors je le met pas pour le moment 
	# if res == False:
		# self.main.RestrictLinkType() #make linktype checkbox take effect
		# self.main.SetRestrictedLinkType(self.prevvalue)
		# self.linktypeCtrl.SetValue(self.prevvalue)
		
	# else:
		# self.prevrestrict = self.main.IsLinkTypeRestricted()
		# self.prevvalue = self.linktypeCtrl.GetValue()

		
    def OnDeviceInfoSelected(self,event):
	""" If the user selects (single-click) an info row in the
	list control, a tooltip text will appear. this tooltip text is available
	because it is not always possible to read the whole string/text in the
	information window, but the whole information text will be visible in this tooltip window.
	"""

	my_sel_item = event.GetIndex()
	my_col_0 = self.infoCtrl.GetItem(my_sel_item,0)
	my_col_1 = self.infoCtrl.GetItem(my_sel_item,1)
	self.infoCtrl.SetToolTip(wxToolTip(my_col_0.GetText() + " : " + my_col_1.GetText()))
	
    def SystemChanged(self):
	""" If a subsystem change is detected (the active subsystem is changed)
	this method will be called. Here we collapse all nodes of the previous chosen
	subsystem, in fact; everything down to the tree node root item will be collapsed.
	"""
	try:
		self.self.myCollapseAndReset(self.treeroot) #CollapseAndReset(self.treeroot) #collapse to root when new sub system is chosen outside this module, user will have to chose item him/herself
        	self.UpdateSystems() # We will try to get an update of subsystems available
		self.treeroot.sortChildItem(0,True) #self.treectrl.SortChildren(self.treeroot) # Sort the subsystems found
		self.treeroot.setOpen(True) # self.treectrl.Expand(self.treeroot) # And expand the root; so that the user sees all the new subsystems

		nr = self.treeroot.childCount() #self.treectrl.GetChildrenCount(self.treeroot,False) # After adding the children, we count them
	
		i = 0
		cookie = 6543
		while i < nr: # We iterate through all the subsystems too look for the active subsystem
		
			# We then get the tree node item (id) and cookie for each tree node; 
			# we have to add a cookie to each tree node, so that we can keep track of
			# the current retrieval of tree elements
			if i == 0:
				itm,cookie = self.treeroot.firstChild() #self.treectrl.GetFirstChild(self.treeroot)# ,cookie), needed in linux?
			else:
				itm,cookie = self.treeroot.nextSibling() #self.treectrl.GetNextChild(self.treeroot,cookie)

			# For the active sub system; we update its children (and expand it)
			if itm.text() == self.main.GetActiveSystem():
				itm.setSelected(True) # self.treectrl.SelectItem(itm)
				self.UpdateSystem(itm,False)
				itm.sortChildItem(0,True) # self.treectrl.SortChildren(itm)
				itm.setOpen(True) # self.treectrl.Expand(itm)
				break
			i += 1

	except KeyError,err:
		self.main.ShowError("A tree view error occured: " + str(err),ERR_ERROR)

    def OnDbUpdate(self, evtReason, cdb):
        """ This method is called from another class as a reaction to that
	the ConfDB status changed. 
	React on database state change event          

	Call UpdateDb to change things to the current state
	of the ConfDB
        """
        self.UpdateDb(evtReason, cdb)

    def UpdateDb(self, evtReason, cdb):
        """ Update controls on a database state change. This method
	is usually called by the MainWindow OnDbUpdate
    
            evtReason -- not used here, from interface definition
            cdb -- If not connected to database (cdb == None), delete all the 
                   contents of the controls.
                   If connected (was disconnected), update content of the controls.
        """
        self.setCursor(QCursor(Qt.WaitCursor)) # wxBeginBusyCursor()
	try:
		self.main.SetCdb(cdb)

        	if not cdb: # The user may have disconnected, or the database disconnected itself
		    self.main.ShowError("Connection to the ConfDB was lost!",ERR_INFORMATION)
		    self.main.ResetActiveObjects() # Reset active objects
		    self.infoCtrl.clear() #DeleteAllItems() # Reset information window
		    self.main.GetVisWindow().Clear() # Reset visual window
		    self.myCollapseAndReset(self.treeroot) # self.treectrl.CollapseAndReset(self.treeroot) # Reset tree view
		    # Also update miniature window to reset itself
	        else: # Connected
		    self.main.ShowError("Connection to the ConfDB is now set.",ERR_INFORMATION)
		    self.UpdateSystems() # Update the root to get the subsystems as its children
		    self.expandInCode = True
		    self.treeroot.setOpen(True) # self.treectrl.Expand(self.treeroot) # Expand the root
		    self.treectrl.setEnabled(True)	
	except KeyError,err:
		self.main.ShowError("A tree view error occured: " + str(err),ERR_ERROR)
		self.setCursor(QCursor(Qt.ArrowCursor))

        self.setCursor(QCursor(Qt.ArrowCursor)) # wxEndBusyCursor()

    def myListViewMouseEvent(self,button, item):
	if(item==None):
		return
	if (button==1):
		self.OnSelect(item) #one click, do not expand
	else:
		self.OnChoice(item) #right click; if not expanded, expand

    def OnClick(self,button,item,point,column):
	global timer
	global pendingItem
	pendingItem = item
	timer = QTimer(self);	
	self.connect(timer,SIGNAL("timeout()"),self.OnSelect);
	timer.start( 100, True ); #100 micro-seconds single-shot timer

    def OnSelect(self):
	""" A tree node was selected with a single-click.
	Treat it the same wat as a double-click, wo. expanding
	it.

	Parameters:
	@event - information about the node that was clicked
	"""
	global pendingItem
	item = pendingItem
	if item:
		type = str(item.text(1)) #self.treectrl.GetPyData(item)
		text = str(item.text(0)) #self.treectrl.GetItemText(item)
		if type != str(TN_SYSTEM_HEADER) and type != str(TN_SYSTEM):
			data = type.split("|")
			type = int(data[0])
			system = data[1]
		if type == TN_DEVICETYPE:
			self.UpdateDeviceType(item,system,False)
		elif type == TN_LINKTYPE:
			self.UpdateLinkType(item,system,False)
		elif type == TN_DEVICE:
			self.UpdateDevice(item,system,False)
		elif type == TN_PORT:
			self.UpdatePort(item,system,False)
		#self.OnChoice(event,False,None,False) # OnChoice - our main method for handling tree view clicks


    def OnExpanding(self,event):
	""" When a treeItem is expanded we close the other treeItem located at the same level in the tree.
	
	Parameters:
	@event - The node that is expanding
	"""
	try:
		item=event.parent().firstChild()
	except:
		return
	while item:
		if item != event:
			item.setOpen(False)
		item = item.nextSibling()


    def OnChoice(self,item,point=QPoint(0,0),column=0,onexpand=False,doexpand=True,set_to_minus_one=True):
	""" This method was called in one of the three situations:
	1. Single-click: doexpand=False, rest default
	2. Double-click: all defaults
	3. On expanding of a tree node, onexpand=True, rest default

	Parameters:
	@event - information about the tree node that is chosen
	@onexpand - whether the node chosen is about to expand or not (OnExpand event)
	@item     - the tree node item object that is chosen
	@doexpand - whether the chosen tree item should be expanded or not
	@set_to_minus_one - whether we should change to device level (-1) in the visual window
	or not.
	"""

	global timer
	global pendingItem
	timer.stop()
	pendingItem = 0
	
	if not item or item.isOpen() or (self.main.GetUserMode() == ID_CREATION_MODE and item.firstChild()):
		return
	type = str(item.text(1)) #self.treectrl.GetPyData(item)
	text = str(item.text(0)) #self.treectrl.GetItemText(item)	
	# We get tree item data and tree item text/name of the node we have chosen
		# data = type.split("|")
		# type = int(data[0])
		# system = data[1]
		#print item.text(0)
		#print item.text(1)
	self.main.ResetActiveObjects() # Reset all active objects -> only the currently selected object can be active
	# This is for undo/redo functionality; whether we have changed device within neighbor or path view mode
	# if we have, since last undo/redo, we will not try to add the new undo/redo objects to the window.
	if (self.main.GetViewMode() == ID_SUCCESSORS_MODE or self.main.GetViewMode() == ID_PATHS_MODE):
		self.main.SetPrevView(self.main.GetCurrentView())
		self.main.WhenChangeView()

	if type == str(TN_SYSTEM_HEADER): # If we clicked on "Subsystems"
		self.main.GetVisWindow().SetLevelZoom(0) # LHCb system level, show the illustration
		return
		# if onexpand: return True #Already expanding, continue expanding (clicked plus-sign)
	elif type == str(TN_SYSTEM):
		##self.main.setCursor(QCursor(Qt.WaitCursor))
		if self.main.GetActiveSystem() != text: # If we changed subsystem
			self.main.GetVisWindow().Clear() # Changed system, clear!!!
			self.main.SetActiveSystem(text,self)
			self.main.GetVisWindow().canvas.update()
			if text=="Spare devices" and not item.firstChild():
				tmp = waiter(None,"Loading data from ConfDB")
				self.UpdateSystem(item,True,True)
				tmp.accept()
		##self.main.setCursor(QCursor(Qt.ArrowCursor))
		return
		#self.treectrl.Collapse(self.treeroot)
		#TODO: issue: when we change to a new subsystem, the other open subsystem should
		#be collapsed, but in windows it seems to be difficult to automatically expand nodes
		#in code
		##self.UpdateSystem(item,True,doexpand) # Add children to chosen subsystem
		self.main.GetVisWindow().Clear() # Clear Visual Window because we changed system (...)
		#self.main.GetVisWindow().SetLevelZoom(0) # Change level zoom?
	else: # Any other tree node type
		if set_to_minus_one:
			self.main.GetVisWindow().SetLevelZoom(-1) # Device level
		
		data = type.split("|")
		type = int(data[0])
		system = data[1]

		if type == TN_LINKTYPE_HEADER:
			self.UpdateLinkTypes(item,system,doexpand)
		elif type == TN_DEVICETYPE_HEADER:
			self.UpdateDeviceTypes(item,system,doexpand)
		elif type == TN_DEVICETYPE:
			self.UpdateDeviceType(item,system,doexpand)
		elif type == TN_LINKTYPE:
			self.UpdateLinkType(item,system,doexpand)
		elif type == TN_DEVICE:
			self.UpdateDevice(item,system,doexpand)
			# if item.firstChild():	item.setPixmap(0,self.listViewDevice)   # Pour ajouter des iconnes au devices
			# else:	item.setPixmap(0,self.listViewEmptyDevice)
		elif type == TN_PORT:
			pass #self.UpdatePort(item,system,False)
		else:
			self.main.ShowError("An invalid treeitem was chosen: " + str(type),ERR_ERROR)
	return
	if not onexpand and doexpand:
		item.setOpen(True) #self.treectrl.Expand(item)

    ############################################################################
    # Update Functions
    ############################################################################

    def UpdateSystems(self,update=False,doexpand=True):
	""" Update subsystems available in ConfDB.

	Parameters:
	@update - whether we should update and add children to the child nodes of this node
	@doexpand - whether this node should be expanded
        """
	self.setCursor(QCursor(Qt.WaitCursor)) # wxBeginBusyCursor()
	self.infoCtrl.clear() # self.infoCtrl.DeleteAllItems() # Clear information window
	self.linktypeCtrl.clear()      # Clear combobox for link type
	if doexpand:
		#the following instruction just remove the treeroot from the treectrl but it does not delete the treeroot that's why we have reinitialise the treeroot with the instruction comming after.
		self.myCollapseAndReset(self.treeroot) #self.treectrl.CollapseAndReset(self.treeroot) # Collapse root
	if not self.main.GetCdb(): #no connection
		self.main.ShowError("Not connected to database!",ERR_INFORMATION)
		self.main.SetActiveSystem(None,"select") # No Active subsystem
	else:
		my_db = self.main.GetCdb() # new db
		if doexpand:
			systems = my_db.GetSubSystems()
			if systems and len(systems) > 0:
				self.main.SetAvailableSystems(systems)
				my_subsystems = []
				progress=QProgressDialog( "Loading data from ConfigDB...", "Abort", len(systems), self, "progress", True) #Progress dialog
				i = 0
				for system in systems:
					if str(system.GetName()) not in do_not_show_systems:
						i += 1
						progress.setProgress(i) #Progress dialog
						progress.setLabelText(str(system.GetName())) #Progress dialog
						self.main.app.processEvents()
						if progress.wasCanceled():
							break
						itmdata = TN_SYSTEM 
						my_currsystem = QListViewItem(self.treeroot,str(system.GetName()),str(itmdata)) # self.treectrl.AppendItem(self.treeroot,str(system.GetName()),-1,-1,wxTreeItemData(itmdata))
						if str(system.GetName()) != "Spare devices":
						    self.UpdateSystem(my_currsystem,True,True)
						my_subsystems.append(my_currsystem)
						self.__treeitemids[str(system.GetName())] = my_currsystem #WAL_OBSELETE ??
				progress.setProgress(len(systems)) #Progress dialog

						#self.treectrl.SortChildren(self.treeroot)
				self.treeroot.sortChildItems(0,True)  # We will sort one time at the end of the boucle that's will be better
				if update:
					for subsystem in my_subsystems:
						if subsystem.GetName()!="Spare devices":
							self.UpdateSystem(subsystem,True,True)
				self.treeroot.setEnabled(True)
				self.main.ShowError(str(len(systems)) + " available subsystems retrieved from ConfDB.",ERR_INFORMATION)

			else: #No systems available
				self.main.SetAvailableSystems([])
				self.main.ShowError("No Subsystems found in the ConfDB." + str(my_db.GetErrorMessage()),ERR_CRITICAL)

	self.setCursor(QCursor(Qt.ArrowCursor))
 
    def UpdateSystem(self,treeitem,update=False,doexpand=True):
        """ A subsystem is selected in the tree view. Update it.

	Parameters:
	@treeitem - The tree item id of the subsystem chosen
	@update - whether the child nodes of this node should be update or not
	@doexpand - whether this node should be expanded or not
	"""
	#self.main.app.setOverrideCursor(QCursor(Qt.WaitCursor))

	
	#Collapse other subsystem if open
	activesystem = self.main.GetActiveSystem()
	if activesystem != str(treeitem.text(0)): #self.treectrl.GetItemText(treeitem):
		try:
			treeid = self.GetItemID(activesystem)

			##self.myCollapseAndReset(treeid) #self.treectrl.CollapseAndReset(treeid)
		except KeyError,err:
			pass #system not found, ok, nothing to collapse
	else:	
		if doexpand:
			pass ##self.myCollapseAndReset(treeitem) #self.treectrl.CollapseAndReset(treeitem)
	self.infoCtrl.clear() #self.infoCtrl.DeleteAllItems()
	self.linktypeCtrl.clear()

	if not self.main.GetCdb(): #no connection
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select") #ok?
	    self.SystemChanged()
	else:
	    #enable export as text option on File -> menu
	    self.main.EnableDisableExportText(True)

	    system = str(treeitem.text(0)) #self.treectrl.GetItemText(treeitem)
	    self.main.SetActiveSystem(system,"select")
	    if system == "MUON":
		self.main.GetVisWindow().SetLevelZoom(0) #cause we have to f.ex. choose which MUON station (1-5) or add the stations as children of MUON in the three
	    else:
		self.main.GetVisWindow().SetLevelZoom(1) #have to define specific for each system

	    if doexpand:
                    if system!="Spare devices":
                        itmdata = str(TN_DEVICETYPE_HEADER) + "|" + str(system)
                        devtype_itm = QListViewItem(treeitem,"Device types",itmdata) #self.treectrl.AppendItem(treeitem,"Device types",-1,-1,wxTreeItemData(itmdata))
                        devtype_itm.setPixmap(0,self.deviceTypesIcon)
			self.__treeitemids["Device types"] = devtype_itm
                        itmdata = str(TN_LINKTYPE_HEADER) + "|" + str(system)
                        linktype_itm = QListViewItem(treeitem,"Link types",itmdata)#self.treectrl.AppendItem(treeitem,"Link types",-1,-1,wxTreeItemData(itmdata))
                        linktype_itm.setPixmap(0,self.linkTypesIcon)
			self.__treeitemids["Link types"] = linktype_itm
                    else:
                        itmdata = str(TN_DEVICETYPE_HEADER) + "|" + str(system)
                        devtype_itm = QListViewItem(treeitem,"Spare hw types",itmdata) #self.treectrl.AppendItem(treeitem,"Spare hw types",-1,-1,wxTreeItemData(itmdata))
                        self.__treeitemids["Spare hw types"] = devtype_itm

		    self.main.ShowError(str(treeitem.text(0)) + " subsystem selected.",ERR_INFORMATION)
		    #WALPROB self.main.RefreshObjectBoxData() #update device types and link type in toolbar
		    if update:
                        self.UpdateDeviceTypes(devtype_itm,system)
			
			devtype_itm.sortChildItems(0,True) #self.treectrl.SortChildren(devtype_itm)
                        if system!="Spare devices":
                            self.UpdateLinkTypes(linktype_itm,system)
                            linktype_itm.sortChildItems(0,True) #self.treectrl.SortChildren(linktype_itm)
			
        #self.main.app.setOverrideCursor(QCursor(Qt.ArrowCursor))
	


    def UpdateDeviceTypes(self,treeitem,system,doexpand=True):
        """ Chose DeviceTypes item in treeview, update it.

	parameters:
	@treeitem - treeitem id of the selected tree item
	@system   - in which system we are in (and the selected tree item)
	@doexpand - whether the DeviceTypes nodes should be expanded or not
        """

	self.setCursor(QCursor(Qt.WaitCursor)) 
	
	if doexpand:
		self.myCollapseAndReset(treeitem) #self.treectrl.CollapseAndReset(treeitem)


        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select") #ok?
	    self.SystemChanged()
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem available.",ERR_CRITICAL)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")

	    if doexpand:
			if self.main.GetActiveSystem()!='Spare devices':
				subsystem = SubSystem(self.main.GetCfg(),self.main.GetActiveSystem())	#subsystem object
				types = subsystem.GetDeviceTypes()		#getdevicestypes from currently selected/active system
				#set devicetable in main or is it already set?
			else:
				types=self.main.GetCdb().GetAllSpareHWTypes()
			if types == False:
				self.main.ShowError(subsystem.GetErrorMessage(),ERR_ERROR)
			else:
				if len(types) == 0:
					self.main.ShowError(subsystem.GetErrorMessage(),ERR_INFORMATION)
				else:
					itmdata = str(TN_DEVICETYPE) + "|" + str(system)
					for type in types:
						tmp_devtype = QListViewItem(treeitem,str(type),str(itmdata)) #self.treectrl.AppendItem(treeitem,type,-1,-1,wxTreeItemData(itmdata))
						self.__treeitemids[type] = tmp_devtype
					self.treeroot.sortChildItems(0,True) #self.treectrl.SortChildren(treeitem)

					#WALPROB self.main.RefreshObjectBoxData() #update link types and device types in toolbar
			

        self.setCursor(QCursor(Qt.ArrowCursor)) #wxEndBusyCursor() 

    def UpdateLinkTypes(self,treeitem,system,doexpand=True):
	""" Chose LinkTypes item in tree view, update it.

	parameters:
	@treeitem - tree item id of the selected tree item
	@system - subsystem of the selected tree item
	@doexpand - whether the tree item should be expanded or not
	"""

	self.setCursor(QCursor(Qt.WaitCursor))

	if doexpand:
		self.myCollapseAndReset(treeitem) # self.treectrl.CollapseAndReset(treeitem)

        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select")
	    self.SystemChanged()
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem is available.",ERR_CRITICAL)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")

	    if doexpand:
		    subsystem = SubSystem(self.main.GetCfg(),self.main.GetActiveSystem())	#subsystem object
		    linktypes_raw = subsystem.GetLinkTypes()		#getlinktypes from currently selected/active system
		    self.main.SetLinkTypeTable(subsystem.GetLinkTypeTable()) #or is it already set?
		    self.main.SetDeviceTable(subsystem.GetDeviceTable())
		    #Set for Devicetype later
		    if linktypes_raw == False:
			self.main.ShowError(subsystem.GetErrorMessage(),ERR_ERROR)
		    else:
			types = linktypes_raw
			self.linktypeCtrl.clear()

			if len(types) == 0:
				self.main.ShowError(subsystem.GetErrorMessage(),ERR_INFORMATION)
			else:
				map(self.linktypeCtrl.insertItem,types)
				self.linktypeCtrl.setEnabled(True)
				self.linktyperestrictchk.setEnabled(True)
				self.linktyperestrictchk.setChecked(False)

				itmdata = str(TN_LINKTYPE) + "|" + str(system)
				for type in types:
					tmp_linktype = QListViewItem(treeitem,type,str(itmdata)) # self.treectrl.AppendItem(treeitem,type,-1,-1,wxTreeItemData(itmdata))
					self.__treeitemids[type] = tmp_linktype
				self.treeroot.sortChildItems(0,True) #self.treectrl.SortChildren(treeitem)

				#WALPROB self.main.RefreshObjectBoxData() #update link types and device types in toolbar

        self.setCursor(QCursor(Qt.ArrowCursor)) #wxEndBusyCursor()

    def UpdateDeviceType(self,treeitem,system,doexpand=True):
    	""" Chose a Device Type item in tree view, update it.

	parameters:
	@treeitem - tree item id of the selected tree item
	@system - subsystem of the selected tree item
	@doexpand - whether the tree item should be expanded or not
        """

        self.setCursor(QCursor(Qt.WaitCursor))
        #print "UpdateDeviceType"
	if doexpand:
		self.myCollapseAndReset(treeitem) #self.treectrl.CollapseAndReset(treeitem)

        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select")
	    self.main.SetActiveDeviceType(None)
	    self.SystemChanged()
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem set to active.",ERR_INFORMATION)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")
	    self.main.SetActiveDeviceType(str(treeitem.text(0)))
	    devicetype = DeviceType(self.main.GetActiveSystem(),self.main.GetActiveDeviceType(),False)
            self.UpdateInfo(devicetype)  

	    if doexpand:
			if self.main.GetActiveSystem() !='Spare devices':
				devices_list = devicetype.GetDevices()
				if self.main.GetActiveSystem()=="VELO" :
					restrict = self.main.GetRestrictedSparePart()
					#print "je suis dans expand"
					if restrict == 'Spares only':
                                            devices_notinstalled=devicetype.GetDevicesByLocation("SPARE")
					    devices=[x for x in devices_list if x in devices_notinstalled]
					elif restrict == 'In place only':
                                            devices_notinstalled=devicetype.GetDevicesByLocation("SPARE")
                                            devices=[x for x in devices_list if x not in devices_notinstalled]
					else:
					    devices=devices_list
				else:
					devices=devices_list	
			else:
				devices = devicetype.GetSpareDevices()
		    
			if devices == False:
				self.main.ShowError(devicetype.GetErrorMessage(),ERR_ERROR)
			else:
                            print "devices: " + str(devices)
			if type(devices)==type([]) and len(devices) != 0:
				#devices = map(lambda x: x[0],devices)
				
			
			    	#print 'restricted: %s' % restrict
				
				itmdata = str(TN_DEVICE) + "|" + str(system)
				for device in devices:
                                    tmp_dev = QListViewItem(treeitem,device,str(itmdata)) # self.treectrl.AppendItem(treeitem,device,-1,-1,wxTreeItemData(itmdata))
				    self.__treeitemids[device] = tmp_dev
				treeitem.sortChildItems(0,True) # self.treectrl.SortChildren(treeitem)
				
			else:
				self.main.ShowError(devicetype.GetErrorMessage(),ERR_INFORMATION)

        self.setCursor(QCursor(Qt.ArrowCursor))

    def UpdateLinkType(self,treeitem,system,doexpand=True):
    	""" Chose a Link Type item in tree view, update it.

	parameters:
	@treeitem - tree item id of the selected tree item
	@system - subsystem of the selected tree item
	@doexpand - whether the tree item should be expanded or not
        """

        self.setCursor(QCursor(Qt.WaitCursor))

	if doexpand:
		self.myCollapseAndReset(treeitem)

        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select")
	    self.main.SetActiveLinkType(None)
	    self.SystemChanged()
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem set to active.",ERR_INFORMATION)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")
	    self.main.SetActiveLinkType( str(treeitem.text(0)) )
	    linktype = LinkType(self.main.GetActiveSystem(),self.main.GetActiveLinkType(),False)
	    self.UpdateInfo(linktype)

        self.setCursor(QCursor(Qt.ArrowCursor))

    def UpdateDevice(self,treeitem,system,doexpand=True):
    	""" Chose a Device item in tree view, update it.

	parameters:
	@treeitem - tree item id of the selected tree item
	@system - subsystem of the selected tree item
	@doexpand - whether the tree item should be expanded or not
        """

        self.setCursor(QCursor(Qt.WaitCursor))
	##tmp = waiter(None, "wa sber")
	if doexpand:
		self.myCollapseAndReset(treeitem)

        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select")
	    self.main.SetActiveDeviceType(None)
	    self.main.SetActiveDevice(None)
	    self.SystemChanged()
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem set to active.",ERR_INFORMATION)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")
	    self.main.SetActiveDevice(str(treeitem.text(0)))
	    if self.main.GetActiveSystem() !='Spare devices':
		device = Device(self.main.GetActiveSystem(),self.main.GetActiveDevice(),False)
	    else :
                device = Spare_Device(self.main.GetActiveDeviceType(),self.main.GetActiveDevice(),False)
                
	    self.UpdateInfo(device)
            if self.main.GetActiveSystem() !='Spare devices':
                if self.main.GetViewMode() == ID_SUCCESSORS_MODE:
		    self.main.GetVisWindow().OnDeviceSelected(device)	#sending the object and deviceinfo
		elif self.main.GetViewMode() == ID_PATHS_MODE:
                    self.UpdatePath(device)
                elif self.main.GetViewMode() == ID_DYNAMIC_MODE:
                    if self.main.GetVisWindow().GetContents() == []:
			self.main.GetVisWindow().OnDeviceSelected(device,False) #Add to center
                    else:
			x_offset,y_offset = self.main.GetVisWindow().CalcUnscrolledPosition(10,10)
			self.main.GetVisWindow().CreateNode(x_offset,y_offset,device)
			shape = self.main.GetVisWindow().FindByName(device.GetName())
			if shape:
				self.main.GetVisWindow().Select(shape)
                elif self.main.GetViewMode() == ID_SUBSYSTEM_MODE:
                    self.main.ViewSubSystem(device.GetName())

	    if doexpand:
                if self.main.GetActiveSystem() !='Spare devices':
		    ports = device.GetPorts()
		else:
                    ports = device.GetSparePorts()
		
		if ports == False:
		    self.main.ShowError(device.GetErrorMessage(),ERR_ERROR)
		else:
		    if len(ports) != 0:
			itmdata = str(TN_PORT) + "|" + str(system)
			for port in ports:
                            #print 'getname '+str(port.GetName())
			    tmp_port = QListViewItem(treeitem,port.GetName(), str(itmdata)) #self.treectrl.AppendItem(treeitem,port.GetName(),-1,-1,wxTreeItemData(itmdata))
			    self.__treeitemids[port] = tmp_port
			treeitem.sortChildItems(0,True)
				
		    else:
			    self.main.ShowError(device.GetErrorMessage(),ERR_INFORMATION)

        self.setCursor(QCursor(Qt.ArrowCursor))
	##tmp.accept()

    def UpdateLink(self,treeitem,system,doexpand=True): 
    	""" Chose a Link item in tree view, update it. 
	Not in use at the moment...may be in the future

	parameters:
	@treeitem - tree item id of the selected tree item
	@system - subsystem of the selected tree item
	@doexpand - whether the tree item should be expanded or not
        """

        wxBeginBusyCursor()

	if doexpand:
		self.treectrl.CollapseAndReset(treeitem)

        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveSystem(None,"select")
	    self.main.SetActiveLinkType(None)
	    self.main.SetActiveLink(None)
	    self.SystemChanged()
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem set to active.",ERR_INFORMATION)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")
	    self.main.SetActiveLink(self.treectrl.GetItemText(treeitem))
	    link = Link(self.main.GetActiveSystem(),self.main.GetActiveLink(),False,False)
	    self.UpdateInfo(link)

	    #add more options for selection etc for links if wanted later on
            visobj = self.main.GetVisWindow().FindByName(link.GetName())
	    if visobj != None:
		self.main.GetVisWindow().Select(visobj)

        wxEndBusyCursor()

    def UpdatePort(self,treeitem,system,doexpand=True):
    	""" Chose a Link item in tree view, update it. 
	Not in use at the moment...may be in the future

	parameters:
	@treeitem - tree item id of the selected tree item
	@system - subsystem of the selected tree item
	@doexpand - whether the tree item should be expanded or not
        """

        self.setCursor(QCursor(Qt.WaitCursor))

	if doexpand:
		self.myCollapseAndReset(treeitem) #?!

        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.main.SetActiveDevice(None)
	    self.main.SetActivePort(None)
	    self.SystemChanged() #active system may have changed...
	elif not self.main.GetActiveSystem():
	    self.main.ShowError("No subsystem set to active.",ERR_INFORMATION)
        else:
	    if self.main.GetActiveSystem() != system:
	    	self.main.SetActiveSystem(system,"select")

	    self.main.SetActivePort(str(treeitem.text(0)))

	    tmpport = str(treeitem.text(0)).split(":")
            #print 'spare port ' + str(tmpport[2].strip())
	    self.main.SetActiveDevice(tmpport[0].strip())
	    if tmpport[3].strip() == "IN":
		way = 1
	    else: #OUT
		way = 2
	    
	    if self.main.GetActiveSystem()!='Spare devices':
                port = Port(self.main.GetActiveSystem(),tmpport[1].strip(),tmpport[2].strip(),way,tmpport[0].strip(),False)
            else:
                port = Spare_Port(tmpport[1].strip(),tmpport[2].strip(),way,tmpport[0].strip(),False)

	    self.UpdateInfo(port)

        self.setCursor(QCursor(Qt.ArrowCursor)) 

    def UpdateInfo(self,obj=None):
        """ Send an data object of one of the types that are
	created in objectclasses.py (DeviceType,LinkType,Device,
	Link or Port at the moment), and display the information
	about the data object in the information window.

	Parameters:
	@obj - the data object which has to have a method called
	GetObjectInfo() which returns a dictionary of {attribute:value}
	pairs.

	!return - False if unsuccessful
        """
	self.my_data = []

	self.infoCtrl.clear()
	if obj == None: #If no object was sent (to clear the information window)
		return False	

        self.setCursor(QCursor(Qt.WaitCursor))
        if not self.main.GetCdb():
	    self.main.ShowError("Not connected to database.",ERR_INFORMATION)
	    self.SystemChanged()
        else:
	    infos = obj.GetObjectInfo() # Get the dictionary with information
	    #print "in update info" +str(obj.GetName())
	    if infos and len(infos) > 0:
		new_dict = {}
		keys = infos.keys()
		#rgbcolor_=infos["rgbcolor"]
		#infos["rgbcolor"]="(" + str(rgbcolor_.red()) + ", " + str(rgbcolor_.green()) + ", " + str(rgbcolor_.blue()) + ")"
		i = 0
		for key in keys:
		    if key not in DEV_GENERIC:	#do not show some fields like device type, systemname etc. 
			    			# given in the list DEV_GENERIC in cdbviscore
			elt_=QListViewItem(self.infoCtrl,key,str(infos[key]).replace('\n','   '))#self.infoCtrl.InsertStringItem(i, key)
			#self.infoCtrl.SetStringItem(i, 0, key)
			#self.infoCtrl.SetStringItem(i, 1, str(infos[key]))
                        #print "key" +str(key)
                        #print "key" +str(infos[key])
			#self.my_data.append(str(key))
			#WAL_OBSELETE new_dict[i] = (key,str(infos[key]))
			#WAL_OBSELETE self.infoCtrl.SetItemData(i, i) # to be able to sort the column
			#WAL_OBSELETE i += 1

		#WAL_OBSELETE self.itemDataMap = new_dict # needed to be able to sort the list control alphabetically (the attribute names)
		#WAL_OBSELETE self.infoCtrl.SetColumnWidth(0, wxLIST_AUTOSIZE)
	
		#WAL_OBSELETE my_width = self.GetSizeTuple()[0]-self.infoCtrl.GetColumnWidth(0)-10
		#WAL_OBSELETE if self.infoCtrl.GetCountPerPage() < self.infoCtrl.GetItemCount():
			#WAL_OBSELETE my_width -= 3
		#WAL_OBSELETE self.infoCtrl.SetColumnWidth(1,my_width)
		self.infoCtrl.setSorting(0,True) # self.SortListItems(col=0,ascending=1)
		
	    else:
		self.main.ShowError(obj.GetErrorMessage(),ERR_ERROR)

        self.setCursor(QCursor(Qt.ArrowCursor))
      
    def GetListCtrl(self):
	""" One of the requirements for using the sorting mechanism
	of the ColMixer thingy, this method is needed.
	"""
	return self.infoCtrl
     
    def GetCdb(self):
        """ Return handle on configuration database module

        !return - The method does not return the confifuration database itself, but an
        instance of a confdb class. Before connecting (and after 
        disconnecting) None return None.
        """
	return self.main.GetCdb()

#################################################################################################################
# PATH VIEW STUFF
##############################################################################################################
            
    def GetPath(self):
        """ Return the path index currently chosen in the 
	path window dialog.
	
	!return - path index (integer)
	"""
	
        return self.__path

    def UpdatePath(self, deviceobj=None):
        """ Update the path selection control.

            This method uses the PathSelect class to show the paths passing
            the selected device.

	parameters:
	@deviceobj - the device object that we will show the path window for,
	and get paths from.
        """
	
        if not self.selectPathFrame: #If path window not visible, create it
        	self.selectPathFrame = PathSelect(self.main.GetCdb(), self.main, self.main.GetVisWindow().OnPathSelected,device=deviceobj)

        res = self.selectPathFrame.SetPathDevice(deviceobj) # Set device object; in case it was changed
	if res != False:
	        self.selectPathFrame.show() # Show the path window
	else:
		self.selectPathFrame.show()
 
###########################################################################
# PathSelect class							###
###########################################################################

class PathSelect(QDialog):
    """ PathSelect class pops up as a little window inside CdbVis, so that 
            the user can choose a path passing a device to it.
    """

    def __init__(self,cdb, parent = None, callback = None, device=None):
        """ Constructor.

	        Parameters:
                @parent - the parent window (usually MainWindow)
                @cdb    - the confDB instance that should be used (reference)
                @pos    - position of the path window
                @style  - window style
                @callback - callable object method that should
                  	    be called if a path is selected. 
                            callback must have 3 parameters: (self, dictionary deviceInfo, integer pathNr)
        """
        QDialog.__init__(self,parent,"Paths",0,0)  # Args = parent, name, modal, flags

        self.setMinimumSize(QSize(200,0))
        self.setFocusPolicy(QDialog.TabFocus)
        self.setModal(0)

        PathsLayout = QGridLayout(self,1,1,2,6,"PathsLayout")

        self.listBox = QListBox(self,"listBox")
        self.listBox.setFrameShadow(QListBox.Sunken)
        self.listBox.setLineWidth(1)
        self.listBox.setMargin(0)
        self.listBox.setVScrollBarMode(QListBox.Auto)

        PathsLayout.addWidget(self.listBox,0,0)

        self.resize(QSize(200,376).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

	# End GUI stuffs
	self.__cdb = cdb
	self.__callback = callback
	self.__deviceInfo = None
	self.__path = None
	self.__device = device

	self.main = parent

	# Events
	self.connect(self.listBox,SIGNAL("selected(int)"),self.OnPathChoice)
	#self.connect(self,SIGNAL("lastWindowClosed()"),self.OnClose)




    def OnPathChoice(self, event):
            """ User chose a path from the path window.
	    """
	    
	    self.SetPath(event+1)


    def OnClose(self, event):
            """ Path window is closing.
	    """
            self.Destroy()

    def clear(self):
            """ Clear the list of paths in the control and reset path selection.
	    """
	    
            self.listBox.clear()
            self.__path = None

    def SetPath(self, pathNr, update = True):
            """ Choose a specific path in the list

                @pathNr - the number of the path to select (integer); 
                          NOT the index in the path list!
                #update (optional, default: True)
                    	if True the given callback function is called
			(detailed description given in the constructor)
            """

            if self.__path != pathNr:
                self.__path = pathNr
		self.listBox.clearSelection()
		self.listBox.setSelected(pathNr-1,True)
                if update and self.__callback:
                	self.__callback(self.__device, pathNr)

    def GetPath(self):
            """ Return the index of the path selected.
	    """
	    
            return self.__path
            
    def SetPathDevice(self,deviceobj):
            """ Set the device for which paths should be shown in path window.

                This method calls GetPathIDs method for the device object chosen

		Parameters:
                @deviceobj - the device data object which we will get the paths for
            """
	    
	    self.__device = deviceobj #set as current path view device
	    deviceInfo = self.__device.GetObjectInfo(True)
	    
            if (None == self.__deviceInfo)  or (self.__deviceInfo[DEV_NAME] != deviceInfo[DEV_NAME]): #name -> id
                self.__deviceInfo = deviceInfo
		
		self.main.ShowError("Please wait while paths are being processed...",ERR_INFORMATION)
		paths = self.__device.GetPaths()

		if paths == False:
			self.main.ShowError(self.__device.GetErrorMessage() + " ,device: " + str(self.__device.GetName()),ERR_ERROR)
			return False
		elif paths == []:
			self.main.ShowError("No paths found: " + str(self.__device.GetErrorMessage()),ERR_INFORMATION)
			return False
		
		self.main.ShowError("Paths were successfully retrieved for the given device.",ERR_INFORMATION)

                oldpath = self.GetPath()
                self.clear()

                for x in paths:
			#print "X: " + str(x)
			#print "X: " + str(x)
			#self.listBox.Append(str(x) + ": " + str(len(paths[x])))
			self.listBox.insertItem(str(x))

                #self.SetPath(oldpath, False)

'''
    class PathSelect(QDialog): #wxMiniFrame):
        """ PathSelect class pops up as a little window inside CdbVis, so that 
            the user can choose a path passing a device to it.
        """
    
        def __init__(self, parent, cdb, pos = wxPoint(720,410), 
                     style = wxDEFAULT_FRAME_STYLE, callback = None,device=None):
            """ Constructor.


	        Parameters:
                @parent - the parent window (usually MainWindow)
                @cdb    - the confDB instance that should be used (reference)
                @pos    - position of the path window
                @style  - window style
                @callback - callable object method that should
                  	    be called if a path is selected. 
                            callback must have 3 parameters: (self, dictionary deviceInfo, integer pathNr)
            """
            wxMiniFrame.__init__(self, parent, -1, "Paths", pos, 
                                 wxSize(250, 300), style)
            self.__cdb = cdb
            self.__callback = callback
            self.__deviceInfo = None
            self.__path = None
	    self.__device = device

	    self.main = parent
            
            # Layout and controls of the select window
            self.sizer = wxBoxSizer(wxVERTICAL)
            self.border = 5
       
            # System
            self.deviceText = wxStaticText(self, -1, "")
            self.sizer.Add(self.deviceText, 0, 
                           wxEXPAND | wxLEFT | wxRIGHT | wxTOP, self.border)
        
            self.listBox = wxListBox(self, ID_PATHCHOICE, 
                                     style = wxLB_NEEDED_SB) 
            self.listBox.Enable(True)
            EVT_LISTBOX(self, ID_PATHCHOICE, self.OnPathChoice)
            self.sizer.Add(self.listBox, 1, 
                           wxEXPAND | wxLEFT | wxTOP | wxRIGHT, self.border)

            button = wxButton(self, -1, "Close")
            EVT_BUTTON(self, button.GetId(), self.OnCloseButton)
            self.sizer.Add(button, 0, wxLEFT | wxBOTTOM | wxRIGHT, self.border)

            EVT_CLOSE(self, self.OnClose)

            self.SetSizer(self.sizer)


        def OnPathChoice(self, event):
            """ User chose a path from the path window.
	    """
	    
	    self.SetPath(int(event.GetSelection()+1))

        def OnCloseButton(self, event):
            """ User closed the path view window, using the close button.
	    """
	    
            self.Close(True)

        def OnClose(self, event):
            """ Path window is closing.
	    """
	    
            self.Destroy()

        def Clear(self):
            """ Clear the list of paths in the control and reset path selection.
	    """
	    
            self.listBox.Clear()
            self.__path = None

        def SetPath(self, pathNr, update = True):
            """ Choose a specific path in the list

                @pathNr - the number of the path to select (integer); 
                          NOT the index in the path list!
                #update (optional, default: True)
                    	if True the given callback function is called
			(detailed description given in the constructor)
            """

            if self.__path != pathNr:
                self.__path = pathNr
		self.listBox.Select(pathNr-1)
                if update and self.__callback:
                	self.__callback(self.__device, pathNr)

        def GetPath(self):
            """ Return the index of the path selected.
	    """
	    
            return self.__path
            
        def SetPathDevice(self,deviceobj):
            """ Set the device for which paths should be shown in path window.

                This method calls GetPathIDs method for the device object chosen

		Parameters:
                @deviceobj - the device data object which we will get the paths for
            """
	    
	    self.__device = deviceobj #set as current path view device
	    deviceInfo = self.__device.GetObjectInfo(True)

            if (None == self.__deviceInfo)  or (self.__deviceInfo[DEV_NAME] != deviceInfo[DEV_NAME]): #name -> id
                self.__deviceInfo = deviceInfo
		
		self.main.ShowError("Please wait while paths are being processed...",ERR_INFORMATION)
		paths = self.__device.GetPaths()

		if paths == False:
			self.main.ShowError(self.__device.GetErrorMessage() + " ,device: " + str(self.__device.GetName()),ERR_ERROR)
			return False
		elif paths == []:
			self.main.ShowError("No paths found: " + str(self.__device.GetErrorMessage()),ERR_INFORMATION)
			return False
		
		self.main.ShowError("Paths were successfully retrieved for the given device.",ERR_INFORMATION)

                oldpath = self.GetPath()
                self.Clear()

                for x in paths:
			#print "X: " + str(x)
			#print "X: " + str(x)
			#self.listBox.Append(str(x) + ": " + str(len(paths[x])))
			self.listBox.Append(str(x))

                #self.SetPath(oldpath, False)
'''
