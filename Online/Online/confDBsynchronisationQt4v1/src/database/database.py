#from cdbutils import subsystems
from PyQt4.QtCore import QObject
from config import eDB_name, eDB_user, eDB_pass, dhcp_file, dhcp_flags
from config import cDB_name, cDB_user, cDB_pass
import cx_Oracle
from confDBpython import CONFDB
from utilities import getModuleType
from utilities import getDetector
from model.DHCP import DHCP
from model.system import System
from model.Hugin import Hugin
from model.Tell1 import Tell1
from model.UKL1 import UKL1
from model.spare import Spare
from string import upper, lower, split, replace
import wx
from re import compile, sub
from emptydatabase import removedev

class Subsystem:
    def __init__(self,sys,pat,rne,start,midfix=""):
        self.ippatt = pat
        self.system=sys
        self.NetClass=rne
        self.midfix=midfix
        self.start=start
    def __str__(self):
        return str(self.system)

CTRLsubnet="255.252.0.0"
RNsubnet="255.255.255.0"
systems=["","RICH2","HCAL","ECAL","L0MUON","TFC","L0CALO","TRG","PRS","MUON","MUON_A",
"MUON_C","TT","VELO","VELO_A","VELO_C","OT","OT_A","OT_C","IT","TDET","PUS"]

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

subsystems = {"ecal" : ecal, "tcal" : tcal, "muon_a" : muon_a, "muon_c" : muon_c ,"ot_a" : ot_a,
              "ot_c" : ot_c, "mutq" : mutq, "velo_a" : velo_a, "velo_c" : velo_c, "prs" : prs,
              "tt" : tt, "rich1" : rich1, "rich2" : rich2, "hcal" : hcal, "it" : it,
              "pus" : pus, "tdet" : tdet, "tfc" : tfc, "DUM" : dum, 
              "UNKNOWN" : ecal, "UNKNOWN_DETECTOR" : ecal}


"""discconnecting using confDBpython"""
def disconnect(db):
    db.DBDeconnexion()

"""connecting to the configuration database using confDBpython and returning an instance of confdb"""
def connect(name = cDB_name, user = cDB_user, password = cDB_pass):
    cfDB = CONFDB(name, user, password)
    print str(cfDB)
    cfDB.DBConnexion()
    return cfDB

"""class to access an oracle database"""
class DataBase:
    def __init__(self, name, user, password, logfile):
        print "DataBase.__init__() start"
        self.connection = cx_Oracle.connect(user, password, name)
        print "DataBase.__init__(): connection created"
        self.logfile = logfile
        print "DataBase.__init__() end"
    """returns the connection"""
    def getConnection(self):
        return self.connection
    """executing an insert or update query with following commit"""
    def executeQuery(self, query, commit = True):
        cursor = self.connection.cursor()
        cursor.execute(query)
        if commit:
            cursor.execute("commit")
        cursor.close()
    """sends a commit"""
    def commit(self, query):
        cursor = self.connection.cursor()
        cursor.execute("commit")
        cursor.close()
    """doing a select query which will not modify data in the database and returning the result"""
    def executeSelectQuery(self, query):
        cursor = self.connection.cursor()
        cursor.execute(query)
        return cursor.fetchall()
    def log(self, string_to_log):
        self.logfile.write(string_to_log+"\n")
    """reverses an ip address, resulting ip address may not be a valid ip address!"""
    def make_temp_ip(self, ip):
        m = []
        m = split(ip, ".")
        return m[2]+"."+m[1]+"."+m[0]+".253"

"""class to access the configuration oracle database"""
class ConfigurationDB(DataBase):
    def __init__(self, logfile, name = cDB_name, user = cDB_user, password = cDB_pass):
        DataBase.__init__(self, name, user, password, logfile)
    def getDevicesCount(self):
        query = "select count(*) count from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645"
        result = self.executeSelectQuery(query)
        if result is not None:
            return result[0][0]
        else:
            return 0
    """deletes all devices"""
    def removeAllDevices(self):
        query = "select devicename from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645"
        result = self.executeSelectQuery(query)
        db = connect()
        if len(result) > 1:
            for device in result:
                self.log("removing "+upper(device[0]))
                try:
                    removedev(db, upper(device[0]))
                except RuntimeError, inst:
                    self.log("could not remove "+upper(device[0])+" "+inst.__str__())
                i+=1
        disconnect(db)
        query = "delete from LHCb_microscopic_connectivity where author like 'DSONNICK'"
        self.executeQuery(query)
        query = "delete from LHCb_macroscopic_connectivity where author like 'DSONNICK'"
        self.executeQuery(query)
        query = "delete from LHCB_port_properties where author like 'DSONNICK'"
        self.executeQuery(query)
        query = "delete from LHCB_IPINFO where IPNAME NOT like 'pctest'"
        self.executeQuery(query)
    """gets all devices"""
    def getAllDevices(self):
        query = "select devicename, serialnb from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645 ORDER BY devicename ASC"
        result = self.executeSelectQuery(query)
        devices = []
        for r in result:
            devices.append(str(r[0])+" ("+str(r[1])+")")
        return devices
    """returns systems with devices which have different data in lhcbintegration and confdb"""
    def getChangedSystems(self, equipdb_system):
        new_devices_with_dhcp = System("New Devices with DHCP-data")
        new_devices_without_dhcp = System("New Devices without DHCP-data")
        changed_location_system = System("Devices with changed location")
        changed_dhcp_system = System("Devices with changed DHCP-data")
        devices_up_to_date = System("Devices with the same data in lhcbintegration and confdb")
        allDevices = equipdb_system.getAllDevices()
        #print "devices in equipdb: "+str(len(allDevices))
        for device in allDevices:
            device_up_to_date = True
            query =     "SELECT devices.serialnb, devices.location, ipinfo.ipaddress "+\
                        "FROM lhcb_lg_devices devices "+\
                        "LEFT JOIN lhcb_ipinfo ipinfo ON lower(devices.devicename) = ipinfo.ipname "+\
                        "WHERE lower(devices.devicename) = '"+lower(device.IPname)+"'"
            result = self.executeSelectQuery(query)
            if len(result) == 0:
                #device is new
                device_up_to_date = False
                if device.CIPadd is None or device.CMACadd is None:
                    #print "adding "+str(device)+"to new_devices_without_dhcp"
                    new_devices_without_dhcp.addDevice(device)
                else:
                    #print "adding "+str(device)+"to new_devices_with_dhcp"
                    new_devices_with_dhcp.addDevice(device)
            else:
                #device exists in both databases, check if data in confdb is different
                #from the data in lhcbintegration
                confdb_location = result[0][1]
                confdb_ipaddress = result[0][2]
                if device.loc != confdb_location:
                    #print "adding "+str(device)+"to changed_location_system"
                    changed_location_system.addDevice(device)
                    device_up_to_date = False
                if device.CIPadd != confdb_ipaddress:
                    #print "adding "+str(device)+"to changed_dhcp_system"
                    changed_dhcp_system.addDevice(device)
                    device_up_to_date = False
            if device_up_to_date == True:
                #print "adding "+str(device)+"to devices_up_to_date"
                devices_up_to_date.addDevice(device)
        #print len(new_devices_with_dhcp.getAllDevices())
        #print len(new_devices_without_dhcp.getAllDevices())
        #print len(changed_location_system.getAllDevices())
        #print len(changed_dhcp_system.getAllDevices())
        #print len(devices_up_to_date.getAllDevices())
        return [ new_devices_with_dhcp, new_devices_without_dhcp, changed_location_system, changed_dhcp_system, devices_up_to_date]
    """updates the location of a given device"""
    def updateLocationOfDevice(self, device, db):
        new_connections = []
        try:
            db.UpdateMultipleAttributesDevices(device.devicename, -1, -1, device.loc, "none", 1, 1)
            self.log("updating location of "+device.devicename+"("+device.sn+"): "+" new location= "+device.loc)
            """alte ports loeschen"""
            try:
                devid  = db.GetDeviceID_devicename(device.devicename)
            except RuntimeError:
                print "Exception retrieving deviceID"
                return
            if devid == -1:
                self.log("Device Name "+device.devicename+"not found")
                return
            try:
                ports = db.GetPortIDPerDevID(devid)
                for p in ports:
                    db.DeletePortRow(p)
                self.log("deleting old ports of "+device.devicename+"("+device.sn+")")
            except RuntimeError:
                self.log("error deleting old ports of "+device.devicename+"("+device.sn+")")
            """neue Ports einfuegen"""
            try:
                device.insports(db)
                self.log("inserting insports for "+device.devicename+", "+device.sn)
                new_connections.append(device)
            except RuntimeError, inst:
                self.log("error inserting insports for "+device.devicename+", "+device.sn+", "+inst.__str__())
        except RuntimeError, inst:
            self.log("error updating location of "+device.devicename,+"("+device.sn+")"+" new location: "+device.loc+" ERROR: "+inst.__str__())
    """updates the location of a given device"""
    def updateDHCPDataOfDevice(self, item, db):
        query="select ipaddress from lhcb_ipinfo where ipname = '"+lower(item.devicename)+"'"
        old_ip_a = self.executeSelectQuery(query)
        if len(old_ip_a) == 0 and item.CIPadd is not None:
            try:
                if item.CIPadd is None or item.CMACadd is None:
                    self.log("error inserting insports for "+item.devicename+", "+item.sn+", reason: no IP address or no MAC address")
                else:
                    self.log("inserting insports for "+item.devicename+", "+item.sn)
                    item.insports(db)
            except RuntimeError, inst:
                self.log("error inserting insports for "+item.devicename+", "+item.sn+", "+inst.__str__())
        else:
            """DHCP data already exists"""
            try:
                db.UpdateMultipleIPAddresses(item.CIPadd, old_ip_a[0][0], 1, 1)
                self.log("trying:"+item.devicename+" "+item.CIPadd+" -> "+old_ip_a[0][0])
            except:
                self.log("Did not work:"+item.devicename+" "+item.CIPadd+" -> "+old_ip_a[0][0]+" trying to swap with temp_ip")
                try:
                    """excpected error when bew ip address is used by another device"""
                    temp_ip = self.make_temp_ip(old_ip_a[0][0])
                    """1. free new ip (new ip becomes temp ip)"""
                    self.log(item.devicename+" "+item.CIPadd+" -> "+temp_ip)
                    db.UpdateMultipleIPAddresses(temp_ip, item.CIPadd, 1, 1)
                    """2. the update"""
                    self.log(item.devicename+" "+old_ip_a[0][0]+" -> "+item.CIPadd)
                    db.UpdateMultipleIPAddresses(item.CIPadd, old_ip_a[0][0], 1, 1)
                    """3. temp ip becomes old ip"""
                    self.log(item.devicename+" "+temp_ip+" -> "+old_ip_a[0][0])
                    db.UpdateMultipleIPAddresses(old_ip_a[0][0], temp_ip, 1, 1)
                except RuntimeError, inst:
                    self.log("error updating ipaddress of "+item.devicename+" "+inst.__str__())
    """update devices with changed locations"""
    def updateChangesOfLocation(self, db, controller, progressFrame = None):
        print "\nConfigurationDB.updateChangesOfLocation start..."
        new_connections = []
        i = len(controller.changed_location_system.getAllDevices())+len(controller.changed_dhcp_system.getAllDevices())
        changed_devices = []
        for device in controller.changed_location_system.getAllDevices():
            changed_devices.append(device)
        for device in changed_devices:
            try:
                db.UpdateMultipleAttributesDevices(device.devicename, -1, -1, device.loc, "none", 1, 1)
                self.log("updating location of "+device.devicename+"("+device.sn+"): "+" new location= "+device.loc)
                """alte ports loeschen"""
                try:
                    devid  = db.GetDeviceID_devicename(device.devicename)
                except RuntimeError:
                    print "Exception retrieving deviceID"
                    continue
                if devid == -1:
                    self.log("Device Name "+device.devicename+"not found")
                    continue
                try:
                    ports = db.GetPortIDPerDevID(devid)
                    for p in ports:
                        db.DeletePortRow(p)
                    self.log("deleting old ports of "+device.devicename+"("+device.sn+")")
                except RuntimeError:
                    self.log("error deleting old ports of "+device.devicename+"("+device.sn+")")
                """neue Ports einfuegen"""
                try:
                    device.insports(db)
                    self.log("inserting insports for "+device.devicename+", "+device.sn)
                    new_connections.append(device)
                except RuntimeError, inst:
                    self.log("error inserting insports for "+device.devicename+", "+device.sn+", "+inst.__str__())
            except RuntimeError, inst:
                self.log("error updating location of "+device.devicename,+"("+device.sn+")"+" new location: "+device.loc+" ERROR: "+inst.__str__())
            if progressFrame is not None:
                wx.CallAfter(progressFrame.update, i)
                i+=1
        self.createThrottleConnectivity(db, new_connections, controller)
        print "\nConfigurationDB.updateChangesOfLocation finished!"
    """updates the dhcp data of the devices in the given system"""
    def updateDHCPSystem(self, db, controller, progressFrame = None):
        #TODO: update DHCP data of devices
        print "ConfigurationDB.updateDHCPSystem start"
        changed_devices = []
        for device in controller.changed_dhcp_system.getAllDevices():
            changed_devices.append(device)
        i = 0
        for item in changed_devices:
            i+=1
            #get old ip address
            query="select ipaddress from lhcb_ipinfo where ipname = '"+lower(item.devicename)+"'"
            old_ip_a = self.executeSelectQuery(query)
            if len(old_ip_a) == 0 and item.CIPadd is not None:
                try:
                    if item.CIPadd is None or item.CMACadd is None:
                        self.log("error inserting insports for "+item.devicename+", "+item.sn+", reason: no IP address or no MAC address")
                    else:
                        self.log("inserting insports for "+item.devicename+", "+item.sn)
                        item.insports(db)
                except RuntimeError, inst:
                    self.log("error inserting insports for "+item.devicename+", "+item.sn+", "+inst.__str__())
            else:
                """DHCP data already exists"""
                try:
                    db.UpdateMultipleIPAddresses(item.CIPadd, old_ip_a[0][0], 1, 1)
                    self.log("trying:"+item.devicename+" "+item.CIPadd+" -> "+old_ip_a[0][0])
                except:
                    self.log("Did not work:"+item.devicename+" "+item.CIPadd+" -> "+old_ip_a[0][0]+" trying to swap with temp_ip")
                    try:
                        """excpected error when bew ip address is used by another device"""
                        temp_ip = self.make_temp_ip(old_ip)
                        """1. free new ip (new ip becomes temp ip)"""
                        self.log(item.devicename+" "+item.CIPadd+" -> "+temp_ip)
                        db.UpdateMultipleIPAddresses(temp_ip, item.CIPadd, 1, 1)
                        """2. the update"""
                        self.log(item.devicename+" "+old_ip_a[0][0]+" -> "+item.CIPadd)
                        db.UpdateMultipleIPAddresses(item.CIPadd, old_ip_a[0][0], 1, 1)
                        """3. temp ip becomes old ip"""
                        self.log(item.devicename+" "+temp_ip+" -> "+old_ip_a[0][0])
                        db.UpdateMultipleIPAddresses(old_ip_a[0][0], temp_ip, 1, 1)
                    except RuntimeError, inst:
                        self.log("error updating ipaddress of "+item.devicename+" "+inst.__str__())
            if progressFrame is not None:
                wx.CallAfter(progressFrame.update, i)
        self.createThrottleConnectivity(db, changed_devices, controller)
        print "ConfigurationDB.updateDHCPSystem end"
    """does a full upate"""
    def update(self, controller, progressFrame = None):
        db = connect()
        self.updateChangesOfLocation(db, controller, progressFrame)
        self.updateDHCPSystem(db, controller, progressFrame)
        disconnect(db)
    """inserts the new devices"""
    def insertNewDevices(self, controller, progressFrame = None):
        print "ConfigurationDB.insertNewDevices start"
        db = connect()
        new_devices = []
        for device in controller.new_devices_without_dhcp.getAllDevices():
            new_devices.append(device)
        for device in controller.new_devices_with_dhcp.getAllDevices():
            new_devices.append(device)
        i = 1
        for device in new_devices:
                try:
                    self.log("inserting "+device.devicename+", "+device.sn)
                    device.insert(db, 1, 1)
                except RuntimeError, inst:
                    self.log("error inserting "+device.devicename+", "+device.sn+", "+inst.__str__())
                    print "Error inserting ", device.devicename, "(",device.sn, ")"
                if progressFrame is not None:
                    wx.CallAfter(progressFrame.update, i)
                    i+=1
        print "\ninserting ports of devices..."
        for device in new_devices:
            if device.CIPadd is None or device.CMACadd is None:
                self.log("error inserting insports for "+device.devicename+", "+device.sn+", reason: no ipaddress found")
                continue
            try:
                device.insports(db)
            except RuntimeError, inst:
                self.log("error inserting insports for "+device.devicename+", "+device.sn+", "+inst.__str__())
                print "Error inserting insports for", device.devicename, "(",device.sn, ")"
            if progressFrame is not None:
                wx.CallAfter(progressFrame.update, i)
                i+=1
        self.createThrottleConnectivity(db, new_devices, controller)
        disconnect(db)
        print "ConfigurationDB.insertNewDevices end"
    """connecting masterhugins to tfcmunin01"""
    def connect_masterhugins(self):
        print "connecting masterhugins with TFCMUNIN..."
        db = connect()
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
                try:
                    self.log("source hugin "+hugins[i]+" destination munin port" +str(i+1))
                    db.InsertMultipleMacroLinks(hugins[i],"TFCMUNIN01","0",str(i+1),"THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,"Throttle Link",1,1)
                except RuntimeError, inst:
                        self.log("could not connect "+ hugins[i] +" with tfcmunin01, this could also mean that this connection is already been inserted into the database"+"\n"+inst.__str__())
            except RuntimeError, inst:
                print "Source or Destination Hugin does not exist\n"+inst.__str__()
                pass
        disconnect(db)
        print "connecting masterhugins with TFCMUNIN finished!"
    """creates the throttle connectivity"""
    def createThrottleConnectivity(self, db, devices_to_connect, equipDBSystem):
        for device in devices_to_connect:
            if device.typ != "hugin":
                hugin_device = equipDBSystem.getHuginForDevice(device)
                if hugin_device is not None:
                    dport = device.loc.split("f")[1]
                    """removing leading 0"""
                    if dport[0] == "0":
                        dport = dport[1]
                    try:
                        db.InsertMultipleMacroLinks(device.devicename, hugin_device.devicename,"4",dport,"THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,"Throttle Link",1,1)
                        self.log("connecting "+device.devicename+"("+device.loc+", Port 4) and "+hugin_device.devicename+"("+hugin_device.loc+", Port, "+dport+")")
                    except:
                        self.log("could not connect "+device.devicename+"("+device.loc+", Port 4) and "+hugin_device.devicename+"("+hugin_device.loc+", Port, "+dport+")")
                else:
                    self.log("could not find hugin  for "+device.devicename+" at location "+device.loc)
        """connecting XXHugin02 - XXHuginNN to XXHugin01"""
        masterhugins = []
        last_masterhugin = "none"
        huginpattern = compile(r".*hugin[abc]?[0-9][2-9]")
        hugin01pattern = compile(r".*hugin[abc]?01")
        for device in devices_to_connect:
            result = huginpattern.search(device.IPname)
            if result is not None:
                hugin = result.group(0)
                masterhugin = hugin[:-1]+"1"
                if not last_masterhugin == masterhugin:
                    masterhugins.append(masterhugin)
                last_masterhugin = masterhugin
                splt = hugin.split("0")
                dport=splt[1]
                try:
                    db.InsertMultipleMacroLinks(hugin,masterhugin,"0",dport,"THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,"Throttle Link",1,1)
                    self.log("connecting "+hugin+", Port 0 with "+masterhugin+", Port "+dport)
                except RuntimeError, inst:
                    self.log("error connecting "+hugin+", Port 0 with "+masterhugin+", Port "+dport+" "+inst.__str__())
    """swaps the data of two devices"""
    def swapDevices(self, device1, device2):
        print "ConfigurationDB.swapDevices("+device1.summary()+", "+device2.summary()+ ") start"
        self.log("try to swap "+device1.summary()+" and "+device2.summary()+ "in ConfigurationDB")
        swap_device_device_query = """
        DECLARE
            location1 lhcb_lg_devices.location%TYPE;
            location2 lhcb_lg_devices.location%TYPE;
            devicename1 lhcb_lg_devices.devicename%TYPE;
            devicename2 lhcb_lg_devices.devicename%TYPE;
            serialnb1 lhcb_lg_devices.serialnb%TYPE;
            serialnb2 lhcb_lg_devices.serialnb%TYPE;
        BEGIN
            SELECT devicename, location, serialnb INTO devicename1, location1, serialnb1 FROM lhcb_lg_devices WHERE serialnb = 'SERIALNUMBER1';
            SELECT devicename, location, serialnb INTO devicename2, location2, serialnb2 FROM lhcb_lg_devices WHERE serialnb = 'SERIALNUMBER2';
            UPDATE lhcb_lg_devices SET location = location2||'UNI' WHERE serialnb = serialnb1;
            UPDATE lhcb_lg_devices SET location = location1||'UNI' WHERE serialnb = serialnb2;
            UPDATE lhcb_lg_devices SET location = location2 WHERE serialnb = serialnb1;
            UPDATE lhcb_lg_devices SET location = location1 WHERE serialnb = serialnb2;
            COMMIT;
        END;"""
        swap_spare_spare_query = """
        DECLARE
            location1 lhcb_hw_devices.location%TYPE;
            location2 lhcb_hw_devices.location%TYPE;
            serialnb1 lhcb_hw_devices.serialnb%TYPE;
            serialnb2 lhcb_hw_devices.serialnb%TYPE;
        BEGIN
            SELECT location, serialnb INTO location1, serialnb1 FROM lhcb_hw_devices WHERE serialnb = 'SERIALNUMBER1';
            SELECT location, serialnb INTO location2, serialnb2 FROM lhcb_hw_devices WHERE serialnb = 'SERIALNUMBER2';
            UPDATE lhcb_hw_devices SET location = location2 WHERE serialnb = serialnb1;
            UPDATE lhcb_hw_devices SET location = location1 WHERE serialnb = serialnb2;
            COMMIT;
        END;"""
        swap_spare_device_query = """
        DECLARE
            devicename2 lhcb_lg_devices.devicename%TYPE;
            hwname1 lhcb_hw_devices.hwname%TYPE;
            serialnb_spare lhcb_lg_devices.serialnb%TYPE;
            serialnb_device lhcb_lg_devices.serialnb%TYPE;
            location_spare lhcb_hw_devices.location%TYPE;
            location_device lhcb_hw_devices.location%TYPE;
        BEGIN
            SELECT hwname, location, serialnb INTO hwname1, location_spare, serialnb_spare FROM lhcb_hw_devices WHERE serialnb = 'SERIALNUMBERSPARE';
            SELECT devicename, location, serialnb INTO devicename2, location_device, serialnb_device FROM lhcb_lg_devices WHERE serialnb = 'SERIALNUMBERDEVICE';
            UPDATE lhcb_hwport_properties SET serialnb = serialnb_spare WHERE serialnb = serialnb_device
            UPDATE lhcb_lg_devices SET location = location_spare WHERE serialnb = serialnb_device;
            UPDATE lhcb_lg_devices set serialnb = serialnb_spare where serialnb = serialnb_device
            UPDATE lhcb_hw_devices SET location = location_device WHERE serialnb = serialnb_spare;
            COMMIT;
        END;"""
        """case 1: swapping two devices"""
        if self.isDevice(device1) and self.isDevice(device2):
            print "Swapping two devices"
            swap_device_device_query = sub("SERIALNUMBER1", device1.sn, swap_device_device_query)
            swap_device_device_query = sub("SERIALNUMBER2", device2.sn, swap_device_device_query)
            try:
                self.executeQuery(swap_device_device_query)
                print "ConfigurationDB.swapDevices() end"
                return True
            except:
                self.log(swap_device_device_query+"\nFAILED")
                print "ConfigurationDB.swapDevices() end"
                return False
        """case 2: swapping device and spare"""
        if (self.isDevice(device1) and self.isSpare(device2)) or (self.isDevice(device2) and self.isSpare(device1)):
            print "Swapping a device and a spare"
            if self.isDevice(device1):
                sn_device = device1.sn
                sn_spare = device2.sn
            else:
                sn_device = device2.sn
                sn_spare = device1.sn
            swap_spare_device_query = sub("SERIALNUMBERSPARE", sn_spare, swap_spare_device_query)
            swap_spare_device_query = sub("SERIALNUMBERDEVICE", sn_device, swap_spare_device_query)
            try:
                self.executeQuery(swap_spare_device_query)
                print "ConfigurationDB.swapDevices() end"
                return True
            except:
                self.log(swap_spare_device_query+"\nFAILED")
                print "ConfigurationDB.swapDevices() end"
                return False
        """case 3: swapping two spares"""
        if self.isSpare(device1) and self.isSpare(device2):
            print "Swapping two spares"
            swap_spare_spare_query = sub("SERIALNUMBER1", device1.sn, swap_spare_spare_query)
            swap_spare_spare_query = sub("SERIALNUMBER2", device2.sn, swap_spare_spare_query)
            #print swap_spare_spare_query
            try:
                self.executeQuery(swap_spare_spare_query)
                print "ConfigurationDB.swapDevices() end"
                return True
            except:
                self.log(swap_spare_spare_query+"\nFAILED")
                print "ConfigurationDB.swapDevices() end"
                return False
        print "can only swap devices of type Hugin, UKL1, Tell1 and Spare"
        print "ConfigurationDB.swapDevices() end"
        return False
    """checks if a device is hugin, ukl1 or Tell"""
    def isDevice(self, device):
        return device.typ == "hugin" or device.typ == "ukl1" or device.typ == "tell"
    """checks if a device is a spare"""
    def isSpare(self, device):
        return device.typ == "spare"
    """returns a device with the given serial or None if not found"""
    def getDeviceBySerial(self, serialnb):
        print "ConfigurationDB.getDeviceBySerial("+str(serialnb)+") start"
        query = """SELECT lg.devicename, lg.location, hw.responsible, lg.serialnb, ipinfo.ipaddress, p.macaddress
                    FROM lhcb_hw_devices hw 
                    LEFT JOIN lhcb_lg_devices lg ON (lg.serialnb = hw.serialnb)
                    LEFT JOIN lhcb_ipinfo ipinfo ON lower(lg.devicename) = ipinfo.ipname
                    LEFT JOIN lhcb_hwport_properties p ON p.serialnb = hw.serialnb
                    WHERE hw.serialnb = '"""+str(serialnb)+"'"
        result = self.executeSelectQuery(query)
        if len(result) == 0:
            print "module with serilanb "+str(serialnb)+" not found!"
            return None
        module = result[0]
        if len(module) == 0:
            return None #device not found
        if module[0] is None:
            return None
        mac = module[5]
        ip = module[4]
        if mac is None:
            mac = ""
        if ip is None:
            ip = ""
        device = None
        modulenumber = module[0]
        modulenumber = modulenumber[-2:]
        if getModuleType(module[0]) == "Hugin":
            return Hugin(subsystems[ getDetector(module[0]) ], modulenumber, mac, ip, module[2], serialnb, module[1])
        elif getModuleType(module[0]) == "Ukl1":
            return UKL1(subsystems[ getDetector(module[0]) ], modulenumber, mac, ip, module[2], serialnb, module[1]) 
        elif getModuleType(module[0]) == "Tell1":
            return Tell1(subsystems[ getDetector(module[0]) ], modulenumber, mac, ip, module[2], serialnb, module[1]) 
        else:
            return None #unknwon type

"""class to access the equipment oracle database"""
class EquipmentDB(DataBase):
    def __init__(self, logfile,name = eDB_name, user = eDB_user, password = eDB_pass):
        print "EquipmentDB.__init__() start"
        DataBase.__init__(self, name, user, password, logfile)
        self.dhcp = DHCP(dhcp_file, dhcp_flags)
        self.board_columns = ("name", "height", "power_dissipation", "orientation", 
                              "crate_side", "position", "responsible", "power_consumption", 
                              "type", "label", "label_det", "item_id", "weight", "slots", 
                              "approved", "system", "subsystem", 
                              "position_x","position_y","position_z",
                              "users","last_change")
        print "EquipmentDB.__init__() end"
    """returns a list of all devices (hugin, tell1 or UKL1)"""
    def getAllDevices(self):
        query = "select b.name,b.label,b.responsible,b.position,b.item_id from board b where (label like 'D%f%') and type='electronics' and item_id like '4%' order by name"
        modules = self.executeSelectQuery(query)
        result = []
        dhcp = self.dhcp
        for module in modules:
            #print module
            modulenumber = module[0]
            modulenumber = modulenumber[-2:]
            if getModuleType(module[0]) == "Hugin":
                result.append(Hugin(subsystems[ getDetector(module[0]) ], modulenumber, dhcp.getMACByIPName(module[0]), dhcp.getIPByIPName(module[0]), module[2], module[4], module[1]) )
            elif getModuleType(module[0]) == "Ukl1":
                result.append(UKL1(subsystems[ getDetector(module[0]) ], modulenumber, dhcp.getMACByIPName(module[0]), dhcp.getIPByIPName(module[0]), module[2], module[4], module[1]) )
            elif getModuleType(module[0]) == "Tell1":
                result.append(Tell1(subsystems[ getDetector(module[0]) ], modulenumber, dhcp.getMACByIPName(module[0]), dhcp.getIPByIPName(module[0]), module[2], module[4], module[1]) )
            else:
                pass#print "Warning: ", module[0], module[1], module[2], module[3], module[4], " is not a known module type and will be ignored!"
        return result
    def getAllSpares(self):
        query = "select b.name,b.label,b.responsible,b.position,b.item_id from board b where (label like 'D%f%') and type='electronics' and item_id like '4%' and name like '%Spare%' or name like '%spare%' order by name"
        spares = self.executeSelectQuery(query)
        result = []
        for spare in spares:
            result.append(Spare(spare[0],"type",spare[4],spare[2],spare[1],"comment"))
        return result
    """returns all devides in a model of system - subsyste - devices"""
    def getSystem(self):
        system = System("Devices in lhcbintegration")
        devices = self.getAllDevices()
        for device in devices:
            system.addDevice(device) #will manage the subsystems automatically
        return system
    """returns a device with the given serial or None if not found"""
    def getDeviceBySerial(self, serial):
        query = "select b.name,b.label,b.responsible,b.position,b.item_id from board b where item_id = '"+str(serial)+"'"
        modules = self.executeSelectQuery(query)
        if len(modules) == 0:
            return None #device not found
        dhcp = self.dhcp
        device = None
        for module in modules:
            #print module
            modulenumber = module[0]
            modulenumber = modulenumber[-2:]
            if getModuleType(module[0]) == "Hugin":
                print "subsystem:"+str(subsystems[ getDetector(module[0]) ])
                return Hugin(subsystems[ getDetector(module[0]) ], modulenumber, dhcp.getMACByIPName(module[0]), dhcp.getIPByIPName(module[0]), module[2], module[4], module[1])
            elif getModuleType(module[0]) == "Ukl1":
                print "subsystem:"+str(subsystems[ getDetector(module[0]) ])
                return UKL1(subsystems[ getDetector(module[0]) ], modulenumber, dhcp.getMACByIPName(module[0]), dhcp.getIPByIPName(module[0]), module[2], module[4], module[1]) 
            elif getModuleType(module[0]) == "Tell1":
                print "subsystem:"+str(subsystems[ getDetector(module[0]) ])
                return Tell1(subsystems[ getDetector(module[0]) ], modulenumber, dhcp.getMACByIPName(module[0]), dhcp.getIPByIPName(module[0]), module[2], module[4], module[1]) 
            else:
                return None #unknwon type
    """swaps the data of two devices
        Return:
            True if swap succeed
            False if swap was not successful
    """
    def swapDevices(self, device1, device2):
        print "EquipmentDB.swapDevices("+str(device1.devicename)+", "+str(device2.devicename)+") start"
        self.log("swapping "+str(device1.devicename)+" and "+str(device2.devicename)+" in equipmentdb")
        if device1 is None or device2 is None:
            self.log("device1 or device2 is None, swap failed!")
            return False
        if device1.devicename == device2.devicename:
            self.log("cannot swap 2 identical devices, stopping swap!")
            return False
        swap_query = """
        DECLARE
            location1 board.label%TYPE;
            location2 board.label%TYPE;
            item_id1 board.item_id%TYPE;
            item_id2 board.item_id%TYPE;
        BEGIN
            SELECT label, item_id INTO location1, item_id1 FROM board WHERE item_id = 'SERIALNUMBER1';
            SELECT label, item_id INTO location2, item_id2 FROM board WHERE item_id = 'SERIALNUMBER2';
            UPDATE board SET label = location2||'UNIQUE' WHERE item_id = item_id1;
            UPDATE board SET label = location1||'UNIQUE' WHERE item_id = item_id2;
            UPDATE board SET label = location2 WHERE item_id = item_id1;
            UPDATE board SET label = location1 WHERE item_id = item_id2;
            COMMIT;
        END;"""
        swap_query = sub("SERIALNUMBER1", device1.sn, swap_query)
        swap_query= sub("SERIALNUMBER2", device2.sn, swap_query)
        try:
            self.executeQuery(swap_query)
            print "EquipmentDB.swapDevice() end"
            return True
        except:
            self.log(query+"\nFAILED")
            print "EquipmentDB.swapDevice() end"
            return False