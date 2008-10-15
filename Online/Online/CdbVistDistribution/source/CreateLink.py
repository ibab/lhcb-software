from GUIcreatelink import *
from CreatePorts import *
from objectclasses import *


class CreateLinkWindow(GUIcreateLink):
    def __init__(self, main, parent,deviceFrom=None, deviceTo=None):
        GUIcreateLink.__init__(self,parent,"Create Link",1,0)
	self.connect(self.swapbutton,SIGNAL("released()"),self.Swap)
	self.connect(self.addportfrombutton,SIGNAL("released()"),self.addPortFrom)
	self.connect(self.addporttobutton,SIGNAL("released()"),self.addPortTo)
	self.main = main
	self.linkid.setEnabled(False)
	self.linkid.setText('-1')
	# self.deviceto.insertItem(device1.GetName())
	# self.deviceto.insertItem(device2.GetName())
	# self.devicefrom.insertItem(device2.GetName())
	# self.devicefrom.insertItem(device1.GetName())
	if deviceFrom and deviceTo:
		if type(deviceFrom)==type("") and type(deviceTo)==type(""):
			deviceFrom = Device('',deviceFrom)
			deviceTo = Device('',deviceTo)
			if not deviceFrom.Update():
				self.main.showError("device From does not exist in the Database",ERR_ERROR)
				return
			if not deviceTo.Update():
				self.main.showError("device To does not exist in the Database",ERR_ERROR)
				return	
		# Now we are sure that deviceFrom and deviceTo are "objectclasses.Device"  objects
		self.deviceFrom = deviceFrom
		self.deviceTo = deviceTo
		
		freeports = self.deviceFrom.GetFreePorts()
		self.devFromINfreePorts = freeports[1]
		self.devFromOUTfreePorts = freeports[2]
		if not self.devFromINfreePorts: self.devFromINfreePorts=[]
		if not self.devFromOUTfreePorts: self.devFromOUTfreePorts=[]
		
		freeports = self.deviceTo.GetFreePorts()
		self.devToINfreePorts = freeports[1]
		self.devToOUTfreePorts = freeports[2]
		if not self.devToINfreePorts: self.devToINfreePorts=[]
		if not self.devToOUTfreePorts: self.devToOUTfreePorts=[]
		
		if self.devFromOUTfreePorts:
			for item in self.devFromOUTfreePorts:
			
				self.portfrom.insertItem(item)
		else:
			self.portfrom.insertItem("No Free Port found !")

		if self.devToINfreePorts:
			for item in self.devToINfreePorts:
				self.portto.insertItem(item)
		else:
			self.portto.insertItem("No Free Port found !")
		
		self.devicefrom.setText(self.deviceFrom.GetName())
		self.deviceto.setText(self.deviceTo.GetName())
		
	# Link Types
	system = self.main.GetActiveSystem()
	if system == None or system == "":
		self.ShowError("No subsystem is selected!",ERR_ERROR)
		return False
	my_subsystem = SubSystem(self.main.GetCfg(),system)
	my_linktypes = my_subsystem.GetLinkTypes()
	for item in my_linktypes:
		self.linktype.insertItem(item)

    def accept(self):
	new = Link('')
	new.SetNodeFrom(str(self.devicefrom.text()))
	new.SetNodeTo(str(self.deviceto.text()))

	portfrom = str(self.portfrom.currentText()).split(':')[0].strip()
	portto = str(self.portto.currentText()).split(':')[0].strip()
	try:
		int(portfrom)
		int(portto)
	except:
		self.main.ShowError("You should create a new Port",ERR_ERROR)
		return
	new.SetPortFrom(portfrom)
	new.SetPortTo(portto)
	new.SetType(str(self.linktype.currentText()))
	new.SetLinkInfo(str(self.comment.text()))
	new.SetBidirectional(int(self.bidirectionallink.isChecked()))
	new.SetLinkUsed(int(self.usethislink.isChecked()))
	new.SetSaveStatus(CREATE)
	new.Create()	
	QDialog.accept(self)
	
    def addPortFrom(self):
	portslist = self.deviceFrom.GetPorts()
	devtype = DeviceType('',self.deviceFrom.GetType())
	devtype.Update()
	
	my_createportframe = createPortsWindow(self.main,devtype,self.deviceFrom.GetName(),portslist,self)
	my_createportframe.show()
	ans =  my_createportframe.exec_loop()
	self.deviceports = my_createportframe.GetPorts()
	
	for ports in my_createportframe.GetNewFreePorts()[0]+my_createportframe.GetNewFreePorts()[1]:
		ports.Create()
	self.deletedports = my_createportframe.GetDeletedPorts()
	for ports in self.deletedports:
		ports.Delete()
	# TODO : ici bas on ne prend pas en compte les ports supprime !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	# mettre a jour les champs !
	
	# self.devFromINfreePorts = my_createportframe.GetNewFreePorts()[0] + self.devFromINfreePorts
	# self.devFromOUTfreePorts = my_createportframe.GetNewFreePorts()[1] + self.devFromOUTfreePorts	
	freeports = self.deviceFrom.GetFreePorts()
	self.devFromINfreePorts = freeports[1]
	self.devFromOUTfreePorts = freeports[2]

	self.portfrom.clear()
	for item in self.devFromOUTfreePorts:
		# name = item.GetName().split(':')
		# name = name[1]+':'+name[2]
		self.portfrom.insertItem(item)
	
    def addPortTo(self):
	portslist = self.deviceTo.GetPorts()
	devtype = DeviceType('',self.deviceTo.GetType())
	devtype.Update()
	
	my_createportframe = createPortsWindow(self.main,devtype,self.deviceTo.GetName(),portslist,self)
	my_createportframe.show()
	ans =  my_createportframe.exec_loop()
	
	for ports in my_createportframe.GetNewFreePorts()[0]+my_createportframe.GetNewFreePorts()[1]:
		ports.Create()
	self.deletedports = my_createportframe.GetDeletedPorts()
	for ports in self.deletedports:
		ports.Delete()	
	# mettre a jour les champs !
	# self.devToINfreePorts = my_createportframe.GetNewFreePorts()[0] + self.devToINfreePorts
	# self.devToOUTfreePorts = my_createportframe.GetNewFreePorts()[1] + self.devToOUTfreePorts
	freeports = self.deviceTo.GetFreePorts()
	self.devToINfreePorts = freeports[1]
	self.devToOUTfreePorts = freeports[2]

	self.portto.clear()
	for item in self.devToINfreePorts:
		# name = item.GetName().split(':')
		# name = name[1]+':'+name[2]
		self.portto.insertItem(item)
	
    def GetPorts(self):
	deletedPorts, createdPorts = [], []
	portslist = self.deviceFrom.GetPorts()
	for port in portslist:
		if port.GetSaveStatus()==CREATE:
			createdPorts.append(port)
		elif port.GetSaveStatus()==DELETE:
			deletedPorts.append(port)

	portslist = self.deviceTo.GetPorts()
	for port in portslist:
		if port.GetSaveStatus()==CREATE:
			createdPorts.append(port)
		elif port.GetSaveStatus()==DELETE:
			deletedPorts.append(port)
	return deletedPorts, createdPorts
	
    def Swap(self):
	tmp = str(self.deviceto.text())
	self.deviceto.setText(self.devicefrom.text())
	self.devicefrom.setText(tmp)

	tmp = self.deviceFrom
	self.deviceFrom = self.deviceTo
	self.deviceTo = tmp
	
	tmp = self.devFromINfreePorts
	self.devFromINfreePorts = self.devFromOUTfreePorts 
	self.devFromOUTfreePorts = tmp
	
	tmp = self.devToINfreePorts
	self.devToINfreePorts = self.devToOUTfreePorts 
	self.devToOUTfreePorts = tmp
	
	self.portfrom.clear()
	if self.devFromOUTfreePorts!=[]:
		for item in self.devFromOUTfreePorts:
			self.portfrom.insertItem(item)

	self.portto.clear()
	if self.devToINfreePorts!=[]:
		for item in self.devToINfreePorts:
			self.portto.insertItem(item)