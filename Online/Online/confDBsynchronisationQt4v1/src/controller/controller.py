from database.database import connect, disconnect, ConfigurationDB, EquipmentDB
from time import strftime
from string import replace
from string import upper, lower, split
from PyQt4.QtCore import QObject

"""
    Controller class from which other controller inherit methods to 
    connect to the database"""
class Controller(object):
    def __init__(self):
        self.logfile = self.createSessionLog()
        self.initVars()
    def createSessionLog(self):
        datestamp = strftime("%m.%d.%Y - %H:%M:%S")
        datestamp = replace(datestamp, ":", "-")
        lf = open("log/"+datestamp + " session.log", "w")
        return lf
    def initVars(self):
        print "Controller.initVars() start"
        self.cn = None
        self.confDB = None
        self.equipDB = None
        print "Controller.initVars() end"
    def connectConfDBPython(self):
        print "Controller.connectConfDBPython() start"
        self.cn = None
        result = False
        try:
            self.cn = connect()
            result =  True
        except:
            result = False
        print "confDBpython-connection:\t"+str(self.cn)
        print "Controller.connectConfDBPython() end"
        return result
    def disconnectConfDBPython(self):
        print "Controller.disconnectConfDBPython() start"
        if self.cn is not None:
            try:
                disconnect(self.cn)
            except Exception, inst:
                print str(inst)
        else:
            print "No connection to close."
        self.cn = None
        print "Controller.disconnectConfDBPython() end"
    def getConfDBCxOracleConnection(self):
        print "Controller.getConfDBCxOracleConnection() start"
        if self.confDB is None:
            try:
                self.confDB = ConfigurationDB(self.logfile)
            except:
                print "Error connecting to ConfigurationDB"
                self.confDB = None
        print "Controller.getConfDBCxOracleConnection() end"
        return self.confDB
    def getEquipDBCxOracleConnection(self):
        print "Controller.getEquipDBCxOracleConnection() start"
        if self.equipDB is None:
            try:
                self.equipDB = EquipmentDB(self.logfile)
            except:
                print "Error connecting to EquipmentDB"
                self.equipDB = None
        print "Controller.getEquipDBCxOracleConnection() end"
        return self.equipDB
    def getCn(self):
        return self.cn
