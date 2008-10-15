import csv, sys, pdb
from objectclasses import *
filename = "Velo_Connectivity_clean.csv"

class mainApp:
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
	        print row
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
						raise "error : this is not a DEVICE PORT"
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
		print "########## DEVICE : ",device_list
		print "########## PORT : ",port_list
		print "########## LINK : ",link_list
	
	
test = mainApp()
test.ImportCSV(filename)
 