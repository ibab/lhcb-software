from model.device import Device
from time import strftime
from string import replace
import database.database
from string import upper, lower, split
from time import time
import database.confDBpython 
from database.cdbutils import  Hugin
from database.cdbutils import UKL1
from database.cdbutils import Tell1
from database.utilities import getModuleType
from re import compile
import wx
"""
    This class collects the information about the devices from the quipdb and the
    confdb and compares them
"""
class Devices(object):
    def __init__(self, controller):
        self.controller = controller
        self.dhcp = controller.equipDB.dhcp
        self.equipDB = controller.equipDB
        self.confDB = controller.confDB
        self.logfile = self.createSessionLog()
    def log(self, string_to_log):
        self.logfile.write(string_to_log+"\n")

    """creates the session log file"""
    def createSessionLog(self):
        datestamp = strftime("%m.%d.%Y - %H:%M:%S")
        datestamp = replace(datestamp, ":", "-")
        lf = open("log/"+datestamp + " session.log", "w")
        return lf
    def update(self):
        Device.reset()
        Device.equipdb_devices = self.equipDB.getAllDevices()
        for device in Device.equipdb_devices:
            query =     "SELECT devices.serialnb, devices.location, ipinfo.ipaddress "+\
                        "FROM lhcb_lg_devices devices "+\
                        "LEFT JOIN lhcb_ipinfo ipinfo ON lower(devices.devicename) = ipinfo.ipname "+\
                        "WHERE lower(devices.devicename) = '"+lower(device.IPname)+"'"
            result = self.confDB.executeSelectQuery(query)
            if len(result) == 0:
                if device.CMACadd is not None and device.CMACadd is not None:
                    """new device with dhcp-data"""
                    Device.new_devices_with_dhcp.append(device)
                    Device.new_devices.append(device)
                else:
                    """new device without dhcp-data"""
                    Device.new_devices_no_dhcp.append(device)
                    Device.new_devices.append(device)
            else:
                """device found, check if data has changed"""
                for data in result:
                    if data[0] != device.sn or data[1] != device.loc or data[2] != device.CIPadd:
                        """device data has changed"""
                        Device.changed_devices.append(device)
                    else:
                        """device is up to date"""
                        Device.up_to_date_devices.append(device)
        Device.printInfo()
    """updates existing devices in conf db"""
    def updateConfDB(self):
        """get all existing devices from confDB"""
        query = "select serialnb, device_status, responsible, user_comments, hwname, hwtype, location  from lhcb_hw_devices"
        result = self.confDB.executeSelectQuery(query)
        for row in result:
            confdb_data = {"serialnb" : row[0], "status" : row[1],
                           "responsible" : row[2], "user_comments" : row[3],
                           "hwname" : row[4], "hwtype" : row[5],
                           "location" : row[6]}
            check_query = "select name, responsible, label, item_id  from board"
            check_result = self.equipDB.executeSelectQuery(check_query)
            equipdb_data = {"hwname" : check_result[0], "responsible" : check_result[1],
                            "label" : check_result[2], "serialnb" : check_result[3]}
            module_type = getModuleType(confdb_data["hwname"])
            device_confdb = None
            device_equipdb = None
            if module_type == "Tell1":
                modulenumber = confdb_data["hwname"]
                modulenumber = confdb_data["hwname"][-2:]
                device_confdb = Tell1(subsystems[ getDetector(confdb_data["hwname"]) ], modulenumber, dhcp.getMACByIPName(confdb_data["hwname"]), dhcp.getIPByIPName(confdb_data["hwname"]), confdb_data["responsible"], confdb_data["serialnb"], confdb_data["location"])
                device_equipdb = Tell1(subsystems[ getDetector(confdb_data["hwname"]) ], modulenumber, dhcp.getMACByIPName(confdb_data["hwname"]), dhcp.getIPByIPName(confdb_data["hwname"]), confdb_data["responsible"], confdb_data["serialnb"], euqipdb_data["label"])
            elif module_type == "Ukl1":
                modulenumber = confdb_data["hwname"]
                modulenumber = confdb_data["hwname"][-2:]
                device_confdb = UKL1(subsystems[ getDetector(confdb_data["hwname"]) ], modulenumber, dhcp.getMACByIPName(confdb_data["hwname"]), dhcp.getIPByIPName(confdb_data["hwname"]), confdb_data["responsible"], confdb_data["serialnb"], confdb_data["location"])
                device_equipdb = UKL1(subsystems[ getDetector(confdb_data["hwname"]) ], modulenumber, dhcp.getMACByIPName(confdb_data["hwname"]), dhcp.getIPByIPName(confdb_data["hwname"]), confdb_data["responsible"], confdb_data["serialnb"], euqipdb_data["label"])
            elif module_type == "Hugin":
                modulenumber = confdb_data["hwname"]
                modulenumber = confdb_data["hwname"][-2:]
                device_confdb = Hugin1(subsystems[ getDetector(confdb_data["hwname"]) ], modulenumber, dhcp.getMACByIPName(confdb_data["hwname"]), dhcp.getIPByIPName(confdb_data["hwname"]), confdb_data["responsible"], confdb_data["serialnb"], confdb_data["location"])
                device_equipdb = Hugin1(subsystems[ getDetector(confdb_data["hwname"]) ], modulenumber, dhcp.getMACByIPName(confdb_data["hwname"]), dhcp.getIPByIPName(confdb_data["hwname"]), confdb_data["responsible"], confdb_data["serialnb"], euqipdb_data["label"])
            else:
                #Spare or unknown
                print "Unknown type of device: "+confdb_data["hwname"]
            if device_confdb is not None and device_equipdb is not None:
                #data up to date ?
                if device_confdb == device_equipdb:
                    print str(confdb_data["hwname"])+"is up to date"
                else:
                    #do update
                    update_query = "update set where "
                    self.confDB.executeQuery(update_query)
        pass

    """update devices where the association of serialnb and ipname has changed"""
    def updateChangesOfSerial2Ipname(self, progressFrame = None):
        print "\nUpdating associations of serialnb and ipname..."
        last = "NO"
        i = len(Device.changed_devices) 
        for device in Device.changed_devices:
            update_query1 = "UPDATE lhcb_lg_devices SET devicename = '" + upper(device.devicename) + "_OutOfDate"+str(int(time()))+"' WHERE devicename = '" + upper(device.devicename) + "'"
            update_query2 = "UPDATE lhcb_lg_devices SET devicename = '" + upper(device.devicename) + "' WHERE serialnb = '" + device.sn + "'"
            print update_query1
            print update_query2
            self.log("doing "+update_query1)
            self.log("doing "+update_query2)
            self.confdb.executeQuery(update_query1)
            self.confdb.executeQuery(update_query2)
            if progressFrame is not None:
                wx.CallAfter(progressFrame.update, i)
                i+=1
        print "\nUpdating associations of serialnb and ipname finished!"
    """creates the throttle_data connectivity
        if devices_to_connect = None then the new_devices will be used
    """
    def createThrottleConnectivity(self, devices_to_connect = None):
        if devices_to_connect is None:
            devices = Device.new_devices
        else:
            devices = devices_to_connect
        for device in devices:
            pass
    """connecting devices"""
    def create_connectivity(self, new_connections = None):
        if new_connections is None:
            connect_devices = Device.new_devices
        else:
            connect_devices = new_connections
        print "creating connectivity..."
        db = database.database.connect()
        """connecting ukl1 and tell1 to hugin"""
        for device in connect_devices:
            if device.typ == "tell" or device.typ == "ukl1":
                """find hugin in slot 21"""
                masterhugin_found = 0
                for hugin_device in Device.new_devices:
                    """check if type is hugin"""
                    if hugin_device.typ == "hugin":
                        """check position"""
                        if hugin_device.loc == device.loc[:-2]+"21":
                            """connect it, log success"""
                            masterhugin_found = 1
                            dport = device.loc.split("f")[1]
                            """removing leading 0"""
                            if dport[0] == "0":
                                dport = dport[1]
                            try:
                                db.InsertMultipleMacroLinks(device.devicename, hugin_device.devicename,"4",dport,"THROTTLE_data_out","THROTTLE_data_in","THROTTLE_data",0,"Throttle Link",1,1)
                                self.log("connecting "+device.devicename+"("+device.loc+", Port 4) and "+hugin_device.devicename+"("+hugin_device.loc+", Port, "+dport+")")
                            except:
                                self.log("could not connect "+device.devicename+"("+device.loc+", Port 4) and "+hugin_device.devicename+"("+hugin_device.loc+", Port, "+dport+")")
                if masterhugin_found == 0:
                    """log failure"""
                    self.log("could not find hugin in slot 21 for "+device.devicename+" at location "+device.loc)
        """connecting XXHugin02 - XXHuginNN to XXHugin01"""
        masterhugins = []
        last_masterhugin = "none"
        huginpattern = compile(r".*hugin[abc]?[0-9][2-9]")
        hugin01pattern = compile(r".*hugin[abc]?01")
        for device in connect_devices:
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
        database.database.disconnect(db)
        print "creating connectivity finished!"