from database.database import *
from config import eDB_name, eDB_user, eDB_pass, dhcp_file, dhcp_flags
from config import cDB_name, cDB_user, cDB_pass

db = connect()

devicename = "VETELLA08"
try:
    devid  = db.GetDeviceID_devicename(devicename)
except RuntimeError, inst:
    print "Exception retrieving deviceID. Reason: "+str(inst)
if devid == -1:
    print "Device Name not found"
ports = []
try:
    ports = db.GetPortIDPerDevID(devid)
except RuntimeError, inst:
    print "error getting ports: "+inst.__str__()
for port in ports:
    print "found port "+str(port)+" for "+devicename
    try:
        print str(db.GetMacroConnectivityRow_node(devid, str(188669), 0, ""))
    except RuntimeError, inst:
        print str(inst)
    try:
        print str(db.GetMacroConnectivityRow_node(devid, str(188669), 1, "None"))
    except RuntimeError, inst:
        print str(inst)
disconnect(db)