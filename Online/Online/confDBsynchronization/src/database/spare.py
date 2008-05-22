import database
from emptydatabase import removedev
from utilities import getModuleType, getDetector
from cdbutils import subsystems, Tell1, UKL1, Hugin

class Spare:
    def __init__(self, hwname, hwtype, serialnb, responsible, location, comments):
        self.hwname = hwname
        self.hwtype = hwtype
        self.serialnb = serialnb
        self.responsible = responsible
        self.location = location
        self.comments = comments
    def __cmp__(self, other):
        if (self.hwname == other.hwname and
            self.hwtype == other.hwtype and
            self.serialnb == other.serialnb and
            self.responsible == other.responsible and
            self.location == other.location and
            self.comments == other.comments):
            return 0
        else:
            return -1
    def __str__(self):
        return  str(self.hwname)+" "+str(self.hwtype)+" "+\
                str(self.serialnb)+" "+\
                str(self.responsible)+" "+\
                str(self.location)+" "+\
                str(self.comments)
    def insert(self, db):
        print "spare.insert start"
        db.InsertMultipleSpareDevices(self.hwname, self.hwtype, self.serialnb, self.responsible, self.location, self.comments, 1, 1)
        print "spare.insert end"
    def update(self, confdb):
        print "spare.update start"
        query = "UPDATE lhcb_hw_devices SET hwname = '"+self.hwname+"', hwtype = '"+self.hwtype+"', responsible='"+self.responsible+"', location='"+self.location+"', user_comments='"+self.comments+"' where serialnb='"+self.serialnb+"'"
        confdb.executeQuery(query)
        print "spare.update end" 

class SpareDB:
    def __init__(self, confdb, equipdb):
        self.confDB = confdb
        self.equipDB = equipdb
    """retruns a spare from coonfdb or None"""
    def getSpareConfDBByName(self, name):
        query = "SELECT hwname, hwtype, serialnb, responsible, location, user_comments FROM lhcb_hw_devices WHERE hwname='"+name+"'"
        result = self.confDB.executeSelectQuery(query)
        if len(result) == 0:
            return None
        return Spare(result[0][0], result[0][1], result[0][2], result[0][3], result[0][4], result[0][5])
    """retruns a spare from coonfdb or None"""
    def getSpareConfDBBySerial(self, serial):
        query = "SELECT hwname, hwtype, serialnb, responsible, location, user_comments FROM lhcb_hw_devices WHERE serialnb='"+serial+"'"
        result = self.confDB.executeSelectQuery(query)
        if len(result) == 0:
            return None
        return Spare(result[0][0], result[0][1], result[0][2], result[0][3], result[0][4], result[0][5])
    """returns a list with all spares that are in equipdb, but not in confdb"""
    def getNewSpares(self):
        equipdb_spares = self.equipDB.getAllSpares()
        print "spares in equipdb: "+str(len(equipdb_spares))
        new_spares = []
        if len(equipdb_spares) == 0:
            return []
        for spare in equipdb_spares:
            query = "SELECT hwname, hwtype, serialnb, responsible, location, location, user_comments from lhcb_hw_devices WHERE serialnb like '"+spare.serialnb+"'"
            confdb_spare_result = self.confDB.executeSelectQuery(query)
            print confdb_spare_result
            if len(confdb_spare_result) == 0: # not found means new
                new_spares.append(spare)
        return new_spares
    """returns a list of spares which are in both databases but with different attributes"""
    def getChangedSpares(self):
        equipdb_spares = self.equipDB.getAllSpares()
        new_spares = []
        if len(equipdb_spares) == 0:
            return []
        for spare in equipdb_spares:
            query = "SELECT hwname, hwtype, serialnb, responsible, location, user_comments from lhcb_hw_devices WHERE serialnb = '"+spare.serialnb+"' and device_status = 2"
            confdb_spare_result = self.confDB.executeSelectQuery(query)
            if len(confdb_spare_result) > 0:
                confdb_spare = Spare(confdb_spare_result[0][0], confdb_spare_result[0][1], confdb_spare_result[0][2], confdb_spare_result[0][3], confdb_spare_result[0][4], confdb_spare_result[0][5])
                if spare.__cmp__(confdb_spare) != 0:
                    new_spares.append(spare)
                    print "Spare in lhcbintegration: "+str(spare)
                    print "Spare in confdb:"+str(confdb_spare)
        return new_spares
    """returns a list of Spares existing in confDB"""
    def getSparesInConfDB(self):
        spares = []
        query = "SELECT hwname, hwtype, serialnb, responsible, location, user_comments from lhcb_hw_devices WHERE device_status =2 and serialnb like '4%'"
        confdb_spare_result = self.confDB.executeSelectQuery(query)
        for spare_result in confdb_spare_result:
            spares.append(Spare(spare_result[0], spare_result[1], spare_result[2], spare_result[3], spare_result[4], spare_result[5]))
        return spares
    """returns a list of Spares existing in confDB as String like: sparename (spare_serialnb)"""
    def getSparesInConfDBAsString(self):
        spares = []
        query = "SELECT hwname, serialnb from lhcb_hw_devices WHERE device_status =2 and serialnb like '4%'"
        confdb_spare_result = self.confDB.executeSelectQuery(query)
        for spare_result in confdb_spare_result:
            spares.append(str(spare_result[0])+" ("+str(spare_result[1])+")")
        return spares
    """inserts all spares which are in equipdb but not yet in confdb"""
    def InsertNewSpares(self):
        print "SpareDB.InsertNewSpares start"
        db = database.connect()
        new_spares = self.getNewSpares()
        for spare in new_spares:
            spare.insert(db)
        database.disconnect(db)
        print "SpareDB.InsertNewSpares end"
    """updates existing spares"""
    def UpdateSpares(self):
        print "SpareDB.UpdateSpares start"
        new_spares = self.getChangedSpares()
        for spare in new_spares:
            spare.update(self.confDB)
        print "SpareDB.UpdateSpares end"
    """deletes a spare"""
    def DeleteSpare(self, spare, db = None):
        print "SpareDB.DeleteSpare start"
        disconnect = False
        if spare is None:
            return None
        if db is None:
            db = database.connect()
            disconnect = True # shall disonnect after finishing work
        try:
            db.DeleteHWDevice(spare.serialnb)
        except:
            print "error deleting :"+str(spare)
        if disconnect:
            database.disconnect(db)
        print "SpareDB.DeleteSpare end"
    """deletes all spares from confdb"""
    def DeleteSparesFromConfDB(self):
        print "SpareDB.DeleteSparesFromConfDB start"
        query = "select hwname, hwtype, serialnb, responsible, location, user_comments from lhcb_hw_devices where serialnb like '4%' and device_status != 1"
        result = self.confDB.executeSelectQuery(query)
        if len(result) == 0:
            print "SpareDB.DeleteSparesFromConfDB end"
            return True# nothing to delete was found
        for r in result:
            self.DeleteSpare(Spare(r[0], r[1], r[2], r[3], r[4], r[5]))
        print "SpareDB.DeleteSparesFromConfDB end"
        return True
        """deletes a list of spare"""
    def DeleteSpares(self, spares):
        print "SpareDB.DeleteSpare start"
        if spares is None:
            return None
        db = database.connect()
        for spare in spares:
            try:
                db.DeleteHWDevice(spare.serialnb)
            except:
                print "error deleting :"+str(spare)
        database.disconnect(db)
        print "SpareDB.DeleteSpare end"
    """replaces a device with a spare
       hwname = ipname of the device to be replaced, spare is a Spare that replaces the device"""
    def replaceDevice(self, hwname, spare, confdb):
        print "SpareDB.replaceDevice start"
        db = database.connect()
        #get data of the old device
        query = "SELECT serialnb FROM lhcb_lg_devices WHERE devicename like '"+hwname+"'"
        result = confdb.executeSelectQuery(query)
        try:
            serialnb = result[0][0]
            print "serila of "+hwname+" + "+serialnb
        except:
            print str(devicename)+" does not exist in lhcb_lg_devices"
            return
        #get ipadress etc from the old device
        query = "SELECT ipaddress FROM lhcb_ipinfo WHERE ipname like '"+hwname+"'"
        result = confdb.executeSelectQuery(query)
        ipaddress = ""
        if len(result) > 0:
            ipaddress = result[0][0]
        device_query = "SELECT hwtype, serialnb, responsible, location, user_comments FROM LHCB_HW_DEVICES WHERE serialnb = '"+serialnb+"'"
        device_result = confdb.executeSelectQuery(device_query)
        if len(device_result) == 0:
            print str(spare)+" was not found in lhcb_hw_devices"
            print device_query
            return
        query = "SELECT location from lhcb_lg_devices where serialnb = '"+serialnb+"'"
        result = confdb.executeSelectQuery(query)
        location = result[0][0]
        print device_query
        newSpare = Spare(hwname, str(device_result[0][0]), str(device_result[0][1]), str(device_result[0][2]), str(location), str(device_result[0][4]))
        print newSpare
        #delete spare
        self.DeleteSpare(spare, db)
        #delete device
        removedev(db, hwname)
        #insert device as spare
        newSpare.insert(db)
        #insert spare as device
        device = None
        dhcp = self.equipDB.dhcp
        if getModuleType(hwname) == "Tell1":
            modulenumber = hwname[-2:]
            print hwname
            print modulenumber
            print getDetector(hwname)
            print subsystems[getDetector(hwname)]
            device = Tell1(subsystems[getDetector(hwname)] ,modulenumber,dhcp.getMACByIPName(spare.hwname), dhcp.getIPByIPName(spare.hwname), spare.responsible, spare.serialnb, spare.location)
        elif getModuleType(hwname) == "Ukl11":
            device = UKL1(subsystems[getDetector(hwname)] ,modulenumber,dhcp.getMACByIPName(spare.hwname), dhcp.getIPByIPName(spare.hwname), spare.responsible, spare.serialnb, spare.location)
        elif getModuleType(hwname) == "Hugin":
            device = Hugin(subsystems[getDetector(hwname)] ,modulenumber,dhcp.getMACByIPName(spare.hwname), dhcp.getIPByIPName(spare.hwname), spare.responsible, spare.serialnb, spare.location)
        else:
            print "Error: Unknown Moduletyp for " + hwname
        device.insert(db, 1, 1)
        if device.CIPadd is None or device.CMACadd is None or dhcp is None:
            print "Error inserting insports for "+hwname+" Reason: no ipaddress"
        else:
            device.insports(db)
        database.disconnect(db)
        print "SpareDB.replaceDevice end"
    """returns a list with all spares from configurationdb"""
    def getAllSparesConfDB(self):
        spares = []
        query = "select hwname, serialnb from lhcb_hw_devices where device_status = 2 and hwname like '%Spare%' ORDER BY hwname ASC"
        result = self.confDB.executeSelectQuery(query)
        for r in result:
            spares.append(str(r[0])+" ("+str(r[1])+")")
        return spares