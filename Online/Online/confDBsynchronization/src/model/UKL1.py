from database.confDBpython import *

CTRLsubnet="255.252.0.0"
RNsubnet="255.255.255.0"

class UKL1:
    def __init__(self, sys, module, CMac, CIP, resp,sern,loca):
        self.subsystem = sys
        self.system=sys.system
        self.NetClass=sys.NetClass
        self.modulenumber=module
        self.typ="ukl1"
        self.hwtyp="UKL1"
        self.IPname=sys.ippatt+self.typ+sys.midfix+str(self.modulenumber).zfill(2)
        self.devicename  = self.IPname.upper()
        self.CMACadd      = CMac
        self.CIPadd      = CIP
        self.RNIPadd = "192.169."+str(self.NetClass)+"."+str(self.modulenumber)
        if self.CIPadd is not None:
            mn = str(self.modulenumber)
            #print "mn1 "+mn
            if mn[0] == "0" or mn[0] == "L":
                mn = mn[1:]
                #print "mn2 "+mn
                mn = int(mn) + sys.start
                #print "mn3: "+str(mn)
                self.RNIPadd     = "192.169."+str(self.NetClass)+"."+str(mn)
                #print self.RNIPadd
        self.RNMacadd = []
        self.loc=loca;
        self.slot= self.loc.split("f")[1]
        self.responsible=resp
        self.sn=sern
        self.system="TFC,DAQ,"+self.system
        for j in range(4):
            self.RNMacadd.append("00:CC:BB:"+str(self.NetClass).zfill(2)+":"+
            str(self.modulenumber).zfill(2)+":"+str(j).zfill(2))

    def __cmp__(self, other):
        if self.devicename == other.devicename:
            return 0
        else:
            return -1

    def insert(self,db,first,last):
        db.InsertMultipleFunctionalDevices(self.system,self.devicename,
        "ROBoard",1,0,self.sn,self.hwtyp,self.responsible,self.loc,
        "First Try to enter Readout Boards","none",first,last)

    def insports(self,db):
        controlport = IntVec()
        controlport.append(100) #speed
        controlport.append(1)   #admin_status
        controlport.append(1)   #pxi booting
        intvector = IntVec()
        intvector.append(0) #speed         
        intvector.append(1) #admin_status  
        intvector.append(0) #pxi booting   
        gbEport = IntVec()
        gbEport.append(1000)   #speed         
        gbEport.append(1)      #admin_status  
        gbEport.append(0)      #pxi booting   
        db.InsertMultiplePorts(self.devicename,"0",1,"control","",self.IPname,self.CIPadd,CTRLsubnet,self.CMACadd,controlport,"",1,0)
        db.InsertMultiplePorts(self.devicename,"1",1,"tfc_data","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts(self.devicename,"2",1,"data","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts(self.devicename,"3",1,"data","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts(self.devicename,"4",1,"data","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts(self.devicename,"5",1,"data","","","","","",intvector,"",0,0)
        for j in range(4):
            db.InsertMultiplePorts(self.devicename,str(j),2,"DAQ_data","",self.IPname+"-d1",self.RNIPadd,RNsubnet,self.RNMacadd[j],gbEport,"",0,0)
        db.InsertMultiplePorts(self.devicename,"4",2,"THROTTLE_data_out","","","","","",intvector,"",0,1)

    def summary(self):
        return self.devicname+", "+self.sn