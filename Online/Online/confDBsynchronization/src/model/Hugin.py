from database.confDBpython import *

CTRLsubnet="255.252.0.0"
RNsubnet="255.255.255.0"

class Hugin:
    def __init__(self, sys, module, CMac, CIP, resp,sern,loca):
        self.subsystem = sys
        self.system=sys.system
        self.typ = "hugin"
        self.modulenumber=module
        self.IPname=sys.ippatt+"hugin"+sys.midfix+str(self.modulenumber).zfill(2)
        self.devicename  = self.IPname.upper()
        self.CMACadd      = CMac
        self.CIPadd      = CIP
        self.loc=loca;
        self.slot= self.loc.split("f")[1]
        self.responsible=resp
        self.sn=sern
        self.hwtyp="HUGIN"
        self.system="TFC,"+self.system
        self.RNIPadd="hugins have no RNIP add"

    def __cmp__(self, other):
        return self.devicename == other.devicename

    def insert(self,db,first,last):
        db.InsertMultipleFunctionalDevices(self.system,self.devicename,
        "HUGIN",1,0,self.sn,self.hwtyp,self.responsible,self.loc,
        "First Try to enter Hugins","none",first,last)
    def insports(self,db):
        controlport = IntVec()
        controlport.append(100) #speed
        controlport.append(1)   #admin_status
        controlport.append(1)   #pxi booting
        intvector = IntVec()
        intvector.append(0) #speed         
        intvector.append(1) #admin_status  
        intvector.append(0) #pxi booting   
        db.InsertMultiplePorts(self.devicename,"0",1,"control",str(""),self.IPname,self.CIPadd,CTRLsubnet,self.CMACadd,controlport,str(""),1,0)
        for j in range(20):
            db.InsertMultiplePorts(self.devicename,str(j+1),1,"THROTTLE_data_in","","","","","",intvector,"",0,0)
        for j in range(2):
            db.InsertMultiplePorts(self.devicename,str(j),2,"THROTTLE_data_out","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts(self.devicename,"2",2,"THROTTLE_data_out","","","","","",intvector,"",0,1)
        devid = db.GetDeviceID_devicename(self.devicename)
        thoutport1_id = db.GetPortID_portinfo(devid, "0", "THROTTLE_data_out", 2)
        thoutport2_id = db.GetPortID_portinfo(devid, "1", "THROTTLE_data_out", 2)
        thoutport3_id = db.GetPortID_portinfo(devid, "2", "THROTTLE_data_out", 2)
        thinport_ids = []
        for j in range(20):
            thinport_ids.append(db.GetPortID_portinfo(devid, str(j+1), "THROTTLE_data_in", 1))
        db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[0], thoutport1_id,"THROTTLE_data",0,1,0)
        db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[0], thoutport2_id,"THROTTLE_data",0,0,0)
        db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[0], thoutport3_id,"THROTTLE_data",0,0,0)
        for j in range (1,19):
            db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[j], thoutport1_id,"THROTTLE_data",0,0,0)
            db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[j], thoutport2_id,"THROTTLE_data",0,0,0)
            db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[j], thoutport3_id,"THROTTLE_data",0,0,0)
        db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[19], thoutport1_id,"THROTTLE_data",0,0,0)
        db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[19], thoutport2_id,"THROTTLE_data",0,0,0)
        db.InsertMultipleMicroLinks("motherboard","motherboard",thinport_ids[19], thoutport3_id,"THROTTLE_data",0,0,1)

    def summary(self):
        return self.devicname+", "+self.sn