from GUIcreatePorts import *
from objectclasses import *

# TABLE Keys
DEVICE_NAME 	= 0
PORT_NR 	= 1
PORT_TYPE	= 2
PORT_WAY	= 3
PORT_SPEED	= 4
PXI		= 5
ADM_STATUS	= 6
PHY		= 7
MAC_ADRESS	= 8
IP_ADRESS	= 9
SUBNET		= 10
IP_NAME		= 11
BIA		= 12
PORTID		= 13
MODIFIED 	= 14
MODIFIED_PARAMS = 15
OLD_IP		= 16
SAVE_STATUS	= 17

class createPortsWindow(GUIcreatePort):
    def __init__(self, main, devicetypeobj, devicename, portlist, parent = None):
        GUIcreatePort.__init__(self,parent,"Create Ports",0,0)
	self.connect(self.addportbutton,SIGNAL("released()"),self.OnAddPort)
	self.connect(self.removebutton,SIGNAL("released()"),self.OnDeleteRow)
	self.connect(self.okbutton,SIGNAL("released()"),self.accept)
	self.connect(self.cancelbutton,SIGNAL("released()"),self.reject)
	
	self.main = main
	self.table.hideColumn(SAVE_STATUS) # We keep this column in the table because we will need it to store port status but we do not show it to the user
	self.maxPortsIn = devicetypeobj.GetPortsIn()
	self.maxPortsOut = devicetypeobj.GetPortsOut()
	
	self.devicename.setText(devicename)
	self.devicename.setEnabled(False)
	self.portid.setText('-1')
	self.portid.setEnabled(False)
	self.phy.insertItem('')
	self.phy.insertItem('SX')
	self.phy.insertItem('T')
	self.phy.insertItem('SL')

	self.portlist = portlist
	self.deletedports = []
	self.NewFreePorts = [[],[]]  # [ IN, OUT]
	# Fill the table with the ports informations
	self.table.setReadOnly(True)
	for i in range(len(self.portlist)):
		port = self.portlist[i]
		self.insertInTable(i,port)
	
	
    def OnAddPort(self):
	""" Add Port Button clicked :
	We Create the Port object and add its information to the table below"""
	
	# Validation :
 	
	#Port Way
	if self.portwayOUT.isChecked():		portway = 2
	elif self.portwayIN.isChecked():		portway = 1
	else:
		self.main.ShowError("You should choose a port way",ERR_ERROR,True)
		return
	#Port Number
	if (self.portnumber.value() > self.maxPortsIn-1 and portway==1) or (self.portnumber.value() > self.maxPortsOut-1 and portway==2):
		self.main.ShowError("Invalid port number : \nport number should not exceed %d for Inputs and %d for Outputs. \n Port's number begins at zero."%(self.maxPortsIn-1,self.maxPortsOut-1),ERR_ERROR,True)
		return

	tmp_port = Port('',str(self.portnumber.value()),str(self.porttype.text()), portway, str(self.devicename.text()) )
	tmp_port.SetBia(str(self.bia.text()))
	tmp_port.SetMAC(str(self.macadress.text()))
	tmp_port.SetPortSpeed(self.portspeed.value())
	tmp_port.SetPhy(str(self.phy.currentText()))
	tmp_port.SetPXIBooting(int(self.pxibooting.isChecked()))
	tmp_port.SetIP(str(self.ipadress.text()))
	tmp_port.SetAdmStatus(int(self.admstatus.isChecked()))
	tmp_port.SetIPName(str(self.ipname.text()))
	tmp_port.SetSubnet(str(self.subnetadress.text()))
	
	if self.find(tmp_port)>0:
		self.main.ShowError("Port already exist ",ERR_ERROR,True)
		return
		
	tmp_port.SetSaveStatus(CREATE) #tmp_port.Create()
	self.insertInTable(0,tmp_port)
	self.portlist.insert(0,tmp_port)
	if portway==1:
		self.NewFreePorts[0].insert(0,tmp_port)
	else:
		self.NewFreePorts[1].insert(0,tmp_port)
	
    def GetNewFreePorts(self):
	return self.NewFreePorts

    def GetPorts(self):
	return self.portlist

    def GetDeletedPorts(self):
	return self.deletedports

    def OnDeleteRow(self):
	
	toDeleteIndex = self.table.currentRow()
	self.table.selectRow(toDeleteIndex)
	ans = QMessageBox.question(self,"Remove?","Are you sur you want to remove this port ?","Yes","No","",0,1)
	if ans == 1: # No
		return
	
	portToDelete = (int(str(self.table.text(toDeleteIndex,PORT_NR))), int(str(self.table.text(toDeleteIndex,PORT_WAY))))
	index = self.find(portToDelete)
	if index < 0:
		print "ERROR : port not found"
		return
	if int(str(self.table.text(index,SAVE_STATUS))) == CREATE:
		self.portlist.remove(self.portlist[index])
	else:
		#self.portlist[index].Delete()
		self.deletedports.append(self.portlist[index])
		self.portlist.remove(self.portlist[index])
		
	self.table.removeRow(toDeleteIndex)


    def find(self,port):
	""" Look for "port" in self.portlist and return its index in self.portlist if exist or -1.
	"""
	if self.portlist == []:
		return -1
	if isinstance(port,Port):
		port = (int(port.GetPortNbr()), port.GetPortWay())
	match = ( (int(self.portlist[0].GetPortNbr()), self.portlist[0].GetPortWay()) == port )
	i = 0
	while ( not match and i<len(self.portlist)-1 ):
		i += 1
		match = ( (int(self.portlist[i].GetPortNbr()), self.portlist[i].GetPortWay()) == port )
	if match:
		return i
	else:
		return -1
	
    def insertInTable(self,row,port):
	self.table.insertRows(row)
	self.table.setText( row, DEVICE_NAME,port.GetDevice());
	self.table.setText( row, PORT_NR,port.GetPortNbr());
	self.table.setText( row, PORT_TYPE,port.GetPortType());
	self.table.setText( row, PORT_WAY, str(port.GetPortWay()) );
	self.table.setText( row, PORT_SPEED,str(port.GetPortSpeed()));
	self.table.setText( row, PXI, str(port.GetPXIBooting()) );
	self.table.setText( row, ADM_STATUS, str(port.GetAdmStatus()) );
	self.table.setText( row, PHY,port.GetPhy());
	self.table.setText( row, MAC_ADRESS,port.GetMAC());
	self.table.setText( row, IP_ADRESS,port.GetIP());
	self.table.setText( row, SUBNET,port.GetSubnet());
	self.table.setText( row, IP_NAME,port.GetIPName().replace('\n',' '));
	self.table.setText( row, BIA,port.GetBia());
	self.table.setText( row, PORTID, str(port.GetPortID()) );
	print port.GetSaveStatus()
	self.table.setText( row, SAVE_STATUS, str(port.GetSaveStatus()) );
	## Should be done only if we modify a port :  self.table.setText( row, MODIFIED,str(port.GetModifyStatus()));
	##self.table.setText( row, MODIFIED_PARAMS,port.Get());
	## Should be done only if we modify a port : self.table.setText( row, OLD_IP,port.GetOldIP());