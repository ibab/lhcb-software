from database.confDBpython import *

CTRLsubnet="255.252.0.0"
RNsubnet="255.255.255.0"

class TFCMUNIN01(object):
    def __init__(self, subsystem):
        self.subsystem = subsystem
        self.system = "TFC"
        self.IPname = "tfcmunin01"
        self.devicename = self.IPname.upper()
        self.CMACadd = "00:30:59:01:C9:32"
        self.CIPadd = "10.130.37.79"
        self.typ = "munin"
        self.loc = "D3XXXXf1"
        self.slot= self.loc.split("f")[1]
        self.responsible = "RICHARD JACOBSSON"
        self.sn = "4ODLAUTTFCMUNIN01"
        self.hwtyp = "MUNIN"
    def insert(self, db, first = 1, last = 1):
        db.InsertMultipleFunctionalDevices("TFC",self.devicename,self.hwtyp,1,0,self.sn,"MUNIN",self.responsible,self.loc,self.devicename,"none",first,last)
    def insports(self, db):
        controlport = IntVec()
        controlport.append(100) #speed
        controlport.append(1)   #admin_status
        controlport.append(1)   #pxi booting
        intvector = IntVec()
        intvector.append(0) #speed         
        intvector.append(1) #admin_status  
        intvector.append(0) #pxi booting   
        db.InsertMultiplePorts(self.devicename,"0",1,"control","",self.IPname,self.CIPadd,CTRLsubnet,self.CMACadd,controlport,"",1,0)
        for j in range(16):
            db.InsertMultiplePorts(self.devicename,str(j+1),1,"THROTTLE_data_in","","","","","",intvector,"",0,0)
        for j in range(15):
            db.InsertMultiplePorts(self.devicename,str(j+1),2,"THROTTLE_data_out","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts(self.devicename,"16",2,"THROTTLE_data_out","","","","","",intvector,"",0,1)
    def summary(self):
        return self.devicename+", "+self.sn
    def __str__(self):
        output = self.summary()
        return output
    def __cmp__(self, other):
        return self.devicename == other.devicename