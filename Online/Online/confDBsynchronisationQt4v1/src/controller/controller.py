from database.database import connect, disconnect, ConfigurationDB, EquipmentDB
from database.spare import SpareDB
from time import strftime
from string import replace
from string import upper, lower, split
from PyQt4.QtCore import QObject

"""
    Controller class from which other controller inherit methods to 
    connect to the database"""
class Controller(object):
    def __init__(self, createLogFile = False):
        if createLogFile:
            self.logfile = self.createSessionLog()
        self.initVars()
    def createSessionLog(self):
        datestamp = strftime("%m.%d.%Y - %H:%M:%S")
        datestamp = replace(datestamp, ":", "-")
        lf = open("log/"+datestamp + " session.log", "w")
        return lf
    def initVars(self):
        print "Controller.initVars() start"
        self.__cn = None
        self.__confDB = None
        self.__equipDB = None
        self.__spareDB = None
        print "Controller.initVars() end"
    def connectConfDBPython(self):
        print "Controller.connectConfDBPython() start"
        self.__cn = None
        result = False
        try:
            self.__cn = connect()
            result =  True
        except:
            result = False
        print "confDBpython-connection:\t"+str(self.__cn)
        print "Controller.connectConfDBPython() end"
        return result
    def disconnectConfDBPython(self):
        print "Controller.disconnectConfDBPython() start"
        if self.__cn is not None:
            try:
                disconnect(self.__cn)
            except Exception, inst:
                print str(inst)
        else:
            print "No connection to close."
        self.__cn = None
        print "Controller.disconnectConfDBPython() end"
    def getConfDBCxOracleConnection(self):
        print "Controller.getConfDBCxOracleConnection() start"
        if self.__confDB is None:
            try:
                self.__confDB = ConfigurationDB(self.logfile)
            except:
                print "Error connecting to ConfigurationDB"
                self.__confDB = None
        print "Controller.getConfDBCxOracleConnection() end"
        return self.__confDB
    def getEquipDBCxOracleConnection(self):
        print "Controller.getEquipDBCxOracleConnection() start"
        if self.__equipDB is None:
            try:
                self.__equipDB = EquipmentDB(self.logfile)
            except:
                print "Error connecting to EquipmentDB"
                self.__equipDB = None
        print "Controller.getEquipDBCxOracleConnection() end"
        return self.__equipDB
    def getSpareDB(self):
        print "Controller.getSpareDB() start"
        if self.__spareDB is None:
            try:
                self.__spareDB = SpareDB(self.getConfDBCxOracleConnection(), self.getEquipDBCxOracleConnection(), self.logfile)
            except:
                print "Error creating SpareDB"
                self.__spareDB = None
        print "Controller.getSpareDB() end"
        return self.__spareDB
    def getCn(self):
        if self.__cn is None:
            if self.connectConfDBPython():
                return self.__cn
        return self.__cn
