import database
from string import upper
#from model.devices import Devices
from config import cDB_name, cDB_user, cDB_pass

def removedev(db,nam):
    print "deleting "+nam+" in db "+str(db)+"..."
    devid = -1
    try:
        devid  = db.GetDeviceID_devicename(nam)
        #print "devid "+str(devid)
    except RuntimeError, inst:
        print "Exception retrieving deviceID. Reason: "+str(inst)
        return
    if devid == -1:
        print "Device Name not found"
        return
    ports = []
    try:
        ports = db.GetPortIDPerDevID(devid)
    except RuntimeError, inst:
        print "error getting ports: "+inst.__str__()
    for p in ports:
        try:
            db.DeletePortRow(p)
        except RuntimeError, inst:
            print "ports for "+nam+" were not deleted! Reason: "+inst.__str__()
    try:
        db.DeleteFunctionalDevice(devid)
    except RuntimeError, inst:
            print "Error while deleteing "+str(devidi)+"! Reason: "+inst.__str__()

def removeAllDevices(controller, progressFrame = None):
    confDB = controller.confDB
    #equipDB = database.database.EquipmentDB()
    #devices = Devices(confDB, equipDB)
    #devices.update()
    query = "select devicename from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645"
    result = confDB.executeSelectQuery(query)
    db = database.connect()
    #removedev(db, "ECTELLL1")
    #removedev(db, "OTTELLCOS")
    #removedev(db, "TFCHUGIN01")
    i = 1
    if len(result) > 1:
        for device in result:
            if progressFrame is not None:
                progressFrame.update(i)
                confDB.log("removing "+upper(device[0]))
            try:
                removedev(db, upper(device[0]))
            except RuntimeError, inst:
                confDB.log("could not remove "+upper(device[0])+" "+inst.__str__())
            i+=1
    database.disconnect(db)
    query = "delete from LHCb_microscopic_connectivity where author like 'DSONNICK'"
    confDB.executeQuery(query)
    query = "delete from LHCb_macroscopic_connectivity where author like 'DSONNICK'"
    confDB.executeQuery(query)
    query = "delete from LHCB_port_properties where author like 'DSONNICK'"
    confDB.executeQuery(query)
    query = "delete from LHCB_IPINFO where IPNAME NOT like 'pctest'"
    confDB.executeQuery(query)

if __name__ == "__main__":
    removeAllDevices()