from cdbutils import *
from config import eDB_name, eDB_user, eDB_pass, dhcp_file, dhcp_flags
from config import cDB_name, cDB_user, cDB_pass
import cx_Oracle
from utilities import getModuleType
from utilities import getDetector
from model.DHCP import DHCP
from spare import Spare
from model.system import System
from model.Hugin import Hugin
from model.Tell1 import Tell1
from model.UKL1 import UKL1
from string import upper, lower, split, replace
import wx
from re import compile
"""discconnecting using confDBpython"""
def disconnect(db):
    db.DBDeconnexion()

"""connecting to the configuration database using confDBpython and returning an instance of confdb"""
def connect(name = cDB_name, user = cDB_user, password = cDB_pass):
    cfDB = CONFDB(name, user, password)
    cfDB.DBConnexion()
    return cfDB

"""class to access an oracle database"""
class DataBase:
    def __init__(self, name, user, password, logfile):
        self.connection = cx_Oracle.connect(user, password, name)
        self.logfile = logfile
    """returns the connection"""
    def getConnection(self):
        return self.connection
    """executing an insert or update query with following commit"""
    def executeQuery(self, query):
        cursor = self.connection.cursor()
        cursor.execute(query)
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
    def make_temp_ip(ip):
        m = []
        m = split(ip, ".")
        return m[2]+"."+m[1]+"."+m[0]+".253"

"""class to access the configuration oracle database"""
class ConfigurationDB(DataBase):
    def __init__(self, logfile, name = cDB_name, user = cDB_user, password = cDB_pass):
        DataBase.__init__(self, name, user, password, logfile)
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
        for device in equipdb_system.getAllDevices():
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
                    new_devices_without_dhcp.addDevice(device)
                else:
                    new_devices_with_dhcp.addDevice(device)
            else:
                #device exists in both databases, check if data in confdb is different
                #from the data in lhcbintegration
                confdb_location = result[0][1]
                confdb_ipaddress = result[0][2]
                if device.loc != confdb_location:
                    changed_location_system.addDevice(device)
                    device_up_to_date = False
                if device.CIPadd != confdb_ipaddress:
                    changed_dhcp_system.addDevice(device)
                    device_up_to_date = False
            if device_up_to_date == True:
                devices_up_to_date.addDevice(device)
        return [ new_devices_with_dhcp, new_devices_without_dhcp, changed_location_system, changed_dhcp_system, devices_up_to_date]
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
    def createThrottleConnectivity(self, db, devices_to_connect, controller):
        for device in devices_to_connect:
            if device.typ != "hugin":
                hugin_device = controller.equipdb_system.getHuginForDevice(device)
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


"""class to access the equipment oracle database"""
class EquipmentDB(DataBase):
    def __init__(self, logfile,name = eDB_name, user = eDB_user, password = eDB_pass):
        DataBase.__init__(self, name, user, password, logfile)
        self.dhcp = DHCP(dhcp_file, dhcp_flags)
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
        query = "select b.name,b.label,b.responsible,b.position,b.item_id from board b where (label like 'D%f%') and type='electronics' and item_id like '4%' and name like '%Spare%' order by name"
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