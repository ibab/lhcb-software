from cdbutils import *
from config import eDB_name, eDB_user, eDB_pass, dhcp_file, dhcp_flags
from config import cDB_name, cDB_user, cDB_pass
import cx_Oracle
from utilities import getModuleType
from utilities import getDetector
from model.DHCP import DHCP
from spare import Spare
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
    def __init__(self, name, user, password):
        self.connection = cx_Oracle.connect(user, password, name)
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

"""class to access the configuration oracle database"""
class ConfigurationDB(DataBase):
    def __init__(self, name = cDB_name, user = cDB_user, password = cDB_pass):
        DataBase.__init__(self, name, user, password)
    """gets all devices"""
    def getAllDevices(self):
        query = "select devicename, serialnb from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645"
        result = self.executeSelectQuery(query)
        devices = []
        for r in result:
            devices.append(str(r[0])+" ("+str(r[1])+")")
        return devices

"""class to access the equipment oracle database"""
class EquipmentDB(DataBase):
    def __init__(self, name = eDB_name, user = eDB_user, password = eDB_pass):
        DataBase.__init__(self, name, user, password)
    """returns a list of all devices (hugin, tell1 or UKL1)"""
    def getAllDevices(self):
        query = "select b.name,b.label,b.responsible,b.position,b.item_id from board b where (label like 'D%f%') and type='electronics' and item_id like '4%' order by name"
        modules = self.executeSelectQuery(query)
        result = []
        dhcp = DHCP(dhcp_file, dhcp_flags)
        self.dhcp = dhcp
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