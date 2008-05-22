import sys
global crats
crats=[]
global dbase
#from cdbutils import *
from confDBpython import *
from utilities import *
#from crates import *
#from modules import *
from database import *
#import cdbutils
CTRLsubnet="255.252.0.0"
RNsubnet="255.255.255.0"
systems=["","RICH2","HCAL","ECAL","L0MUON","TFC","L0CALO","TRG","PRS","MUON","MUON_A",
"MUON_C","TT","VELO","VELO_A","VELO_C","OT","OT_A","OT_C","IT","TDET","PUS"]

def removedev(db,nam):
    print "deleting "+nam+"..."
    try:
        devid  = db.GetDeviceID_devicename(nam)
    except RuntimeError:
        print "Exception retrieving deviceID"
        return
    if devid == -1:
        print "Device Name not found"
        return
    try:
        ports = db.GetPortIDPerDevID(devid)
        ports
        for p in ports:
#            print "deleting port "+str(p)+"..."
            db.DeletePortRow(p)
    except RuntimeError:
        pass
    db.DeleteFunctionalDevice(devid)

def insert_ROBoard_type(db):
    db.InsertMultipleDeviceTypes("DAQ","ROBoard",6,5,"Generic Readout Board","200,200,200",1,1)

def insert_HUGIN_type(db):
    db.InsertMultipleDeviceTypes("TFC","HUGIN",21,16,"Throttle OR","200,200,200",1,1)

def insert_MUNIN_type(db):
    db.InsertMultipleDeviceTypes("TFC","MUNIN",17,16,"Throttle Switch","200,200,200",1,1)

def insert_MUNIN(db):
    try:
        db.InsertMultipleFunctionalDevices("TFC","TFCMUNIN01","MUNIN",1,0,"Serial Number","MUNIN","RICHARD JACOBSSON","D3XXXX","First Try to enter Munin","none",1,1)
        controlport = IntVec()
        controlport.append(100) #speed
        controlport.append(1)   #admin_status
        controlport.append(1)   #pxi booting
        intvector = IntVec()
        intvector.append(0) #speed         
        intvector.append(1) #admin_status  
        intvector.append(0) #pxi booting   
        db.InsertMultiplePorts("TFCMUNIN01","0",1,"control","","tfcmunin01","10.130.37.79",CTRLsubnet,"00:30:59:01:C9:32",controlport,"",1,0)
        for j in range(16):
            db.InsertMultiplePorts("TFCMUNIN01",str(j+1),1,"THROTTLE_data_in","","","","","",intvector,"",0,0)
        for j in range(15):
            db.InsertMultiplePorts("TFCMUNIN01",str(j+1),2,"THROTTLE_data_out","","","","","",intvector,"",0,0)
        db.InsertMultiplePorts("TFCMUNIN01","16",2,"THROTTLE_data_out","","","","","",intvector,"",0,1)
    except RuntimeError:
        pass


class Subsystem:
    def __init__(self,sys,pat,rne,start,midfix=""):
        self.ippatt = pat
        self.system=sys
        self.NetClass=rne
        self.midfix=midfix
        self.start=start


class UKL1:
    def __init__(self, sys, module, CMac, CIP, resp,sern,loca):
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

class Tell1:
    def __init__(self, sys, module, CMac, CIP, resp,sern,loca,typ=""):
        self.system=sys.system
        self.NetClass=sys.NetClass
        self.modulenumber=module
        self.typ="tell"
        self.hwtyp="TELL1"
        if typ!="":
            self.typ=typ
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
        if resp is None:
            self.responsible="unknown"
        else:
            self.responsible=resp
        self.sn=sern
        self.system="TFC,DAQ,"+self.system
        for j in range(4):
            self.RNMacadd.append("00:CC:BB:"+str(self.NetClass).zfill(2)+":"+
            str(self.modulenumber).zfill(2)+":"+str(j).zfill(2))

    def __cmp__(self, other):
        return self.devicename == other.devicename

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


class Hugin:
    def __init__(self, sys, module, CMac, CIP, resp,sern,loca):
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


def insert_Boards(db, t):
    last  = 1
    first = 1
    l=len(t)
    if l == 1:
        print "inserting "+t[0].devicename
        t[0].insert(db,first,last)
    elif l == 2:
        first = 1
        last = 0
        print "inserting "+t[0].devicename
        t[0].insert(db,first,last)
        first=0
        last=1
        print "inserting "+t[1].devicename
        t[1].insert(db,first,last)
    else:
        last = 0
        print "inserting "+t[0].devicename
        t[0].insert(db,first,last)
        first=0
        for i in range(1,l-1):
            print "inserting "+t[i].devicename
            t[i].insert(db,first,last)
        last=1
        print "inserting "+t[l-1].devicename
        t[l-1].insert(db,first,last)
    for i in range(l):
        print "inserting ports for "+t[i].devicename
        t[i].insports(db)



def remove(db,t):
    for x in t:
        print "deleting "+x.devicename+"..."
        try:
            devid  = db.GetDeviceID_devicename(x.devicename)
        except RuntimeError:
            devid=-1
            pass
        try:
            ports = db.GetPortIDPerDevID(devid)
            ports
            for p in ports:
                print "deleting port "+str(p)+"..."
                db.DeletePortRow(p)
        except RuntimeError:
            pass
        if (devid <> -1):
            db.DeleteFunctionalDevice(devid)

def connect_roboard_hugin(db,t):
    hugi = -1
    park=[]
    for x in t:
        if isinstance(x,Hugin):
            splt = x.loc.split("f")
            if splt[1] in ["21"]:
                hugi = t.index(x)
                hugo    = x
            park.append(x)
    for x in park:
        t.remove(x)
    if hugi <> -1:
        first =1;
        last=0;
        l=len(t)
        print "Length of t: ",l
        if l > 0:
            x = t[0];
            if l == 1 :
                last = 1
            if isinstance(x, Tell1) or isinstance(x,UKL1):
                print "linking",x.devicename,hugo.devicename,"4",str(int(x.slot)),\
                "THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data","0",\
                "Throttle Link",first,last
                db.InsertMultipleMacroLinks(x.devicename,hugo.devicename,"4",str(int(x.slot)),
                "THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,
                "Throttle Link",first,last)
                first=0
            for i in range(1,l-1):
                x   = t[i]
                if isinstance(x, Tell1) or isinstance(x,UKL1):
                    print "linking",x.devicename,hugo.devicename,"4",str(int(x.slot)),\
                    "THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data","0",\
                    "Throttle Link",first,last
                    db.InsertMultipleMacroLinks(x.devicename,hugo.devicename,"4",str(int(x.slot)),
                    "THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,
                    "Throttle Link",first,last)
                    if first == 1:
                        first=0
            if l > 1:
                x = t[l-1]
                last=1
                if isinstance(x, Tell1) or isinstance(x,UKL1):
                    print "linking",x.devicename,hugo.devicename,"4",str(int(x.slot)),\
                    "THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data","0",\
                    "Throttle Link",first,last
                    db.InsertMultipleMacroLinks(x.devicename,hugo.devicename,"4",str(int(x.slot)),
                    "THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,
                    "Throttle Link",first,last)
        for x in park:
            t.append(x)

def add_all_crates(db,crats):
    for x in crats:
        if len(x) > 0:
            insert_Boards(db,x)
            connect_roboard_hugin(db,x)

def connect_hugins_munin(db):
    hugins = []
    hugins.append("tfchugin01".upper())
    hugins.append("puhugin01".upper())
    hugins.append("vehugina01".upper())
    hugins.append("vehuginc01".upper())
    hugins.append("r1hugin01".upper())
    hugins.append("tthugin01".upper())
    hugins.append("ithugin01".upper())
    hugins.append("othugina01".upper())
    hugins.append("othuginc01".upper())
    hugins.append("r2hugin01".upper())
    hugins.append("pshugin01".upper())
    hugins.append("echugin01".upper())
    hugins.append("hchugin01".upper())
    hugins.append("muhugina01".upper())
    hugins.append("muhuginc01".upper())
    hugins.append("dumhugin01".upper())
    for i in range(len(hugins)):
        try:
            sdevid  = db.GetDeviceID_devicename(hugins[i])
            print "source hugin "+hugins[i]+" destination munin port" +str(i)
            db.InsertMultipleMacroLinks(hugins[i],"TFCMUNIN01","0",str(i+1),"THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,"Throttle Link",1,1)
        except RuntimeError:
            print "Source or Destination Hugin does not exist"
            pass
        
    

def connect_hugins(db):
    connect_hugin_hugin(db,"ECHUGIN02",1,0)
    connect_hugin_hugin(db,"ECHUGIN03",0,0)
    connect_hugin_hugin(db,"ITHUGIN02",0,0)
    connect_hugin_hugin(db,"ITHUGIN03",0,0)
    connect_hugin_hugin(db,"ITHUGIN04",0,0)
    connect_hugin_hugin(db,"HUGINA02",0,0)
    connect_hugin_hugin(db,"HUGINA03",0,0)
    connect_hugin_hugin(db,"HUGINC02",0,0)
    connect_hugin_hugin(db,"HUGINC03",0,0)
    connect_hugin_hugin(db,"TFCHUGIN02",0,0)
    connect_hugin_hugin(db,"TTHUGIN02",0,0)
    connect_hugin_hugin(db,"TTHUGIN03",0,0)
    connect_hugin_hugin(db,"TTHUGIN04",0,0)
    connect_hugin_hugin(db,"OTHUGINC02",0,0)
    connect_hugin_hugin(db,"OTHUGINC03",0,0)
    connect_hugin_hugin(db,"OTHUGINA02",0,0)
    connect_hugin_hugin(db,"OTHUGINA03",0,0)
    connect_hugin_hugin(db,"VEHUGINA02",0,0)
    connect_hugin_hugin(db,"VEHUGINA03",0,0)
    connect_hugin_hugin(db,"VEHUGINA04",0,0)
    connect_hugin_hugin(db,"VEHUGINC02",0,0)
    connect_hugin_hugin(db,"VEHUGINC03",0,0)
    connect_hugin_hugin(db,"VEHUGINC04",0,1)

def connect_hugin_hugin(db,shugin,f,l):
    splt = shugin.split("0")
    dport=splt[1]
    dhugin = splt[0]+"01"
    try:
        sdevid  = db.GetDeviceID_devicename(shugin)
        ddevid  = db.GetDeviceID_devicename(dhugin)
        print "source hugin "+shugin+" destination hugin "+dhugin+" "+dport
        db.InsertMultipleMacroLinks(shugin,dhugin,"0",dport,"THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,"Throttle Link",f,l)
    except RuntimeError:
        print "Source or Destination Hugin does not exist"
        pass
def fill():
    crates.fill_crates(crats)
    crates.fill_modules()

def remove_all_crates(db,crats):
    for x in crats:
        if len(x) > 0:
            remove(db,x)

def de_populate(db):
    crates.fill_crates(crats)
    crates.fill_modules()
    remove_all_crates(db,crats)
    removedev(db,"TFCMUNIN01")

def populate(db):
    crates.fill_crates(crats)
    crates.fill_modules()
    add_all_crates(db,crats)
    connect_hugins(db)
    insert_MUNIN(db)
    connect_hugins_munin(db)

#systems=["",
#"RICH2","HCAL","ECAL",
#"L0MUON","TFC","L0CALO",
#"TRG","PRS","MUON",
#"MUON_A","MUON_C","TT",
#"VELO","VELO_A","VELO_C",
#"OT","OT_A","OT_C",
#"IT","TDET","PUS"]

rich2 = Subsystem("RICH2","r2",1, 0)
hcal = Subsystem("HCAL","hc",2, 0)
ecal=Subsystem("ECAL","ec",3, 0)
mutq = Subsystem("L0MUON","tmu",4, 0, "q")
tfc = Subsystem("TFC", "tfc", 5, 0)
tcal=Subsystem("L0CALO","tca",6, 0)
trg=Subsystem("TRG", "trg", 7, 0)
prs = Subsystem("PRS","ps",8, 0)
muon_a=Subsystem("MUON_A","mu",9, 0, "a")
muon_c=Subsystem("MUON_C","mu",9,0,"c")
tt  = Subsystem("TT","tt",10,0)
velo_a = Subsystem("VELO_A","ve",11,0,"a")
velo_c = Subsystem("VELO_C","ve",11,50,"c")
ot_a = Subsystem("OT_A","ot",12,0,"a")
it = Subsystem("IT","it",13,0)
rich1 = Subsystem("RICH1","r1",14,0)
ot_c = Subsystem("OT_C","ot",16,0,"c")
dum = Subsystem("DUM", "dum", 17,0)
tdet = Subsystem("TDET","dum",20,0)
pus = Subsystem("PUS","pu",21,0)


global subsystems


subsystems = {"ecal" : ecal, "tcal" : tcal, "muon_a" : muon_a, "muon_c" : muon_c ,"ot_a" : ot_a,
              "ot_c" : ot_c, "mutq" : mutq, "velo_a" : velo_a, "velo_c" : velo_c, "prs" : prs,
              "tt" : tt, "rich1" : rich1, "rich2" : rich2, "hcal" : hcal, "it" : it,
              "pus" : pus, "tdet" : tdet, "tfc" : tfc, "DUM" : dum, 
              "UNKNOWN" : ecal, "UNKNOWN_DETECTOR" : ecal}

#crat=D3B02F29
import crates


