from database.database import connect, disconnect, ConfigurationDB, EquipmentDB
from database.spare import SpareDB
from time import strftime
from string import replace
from string import upper, lower, split
from PyQt4.QtCore import QObject
from os.path import exists
import sys, os
import config
from string import replace
"""
    Controller base class from which other controller inherit methods to 
    connect to the database
    it initialises variables and creates the logfile which will be used by all other
    class in this application.
    It also handles all database connections and provides access to the databases.
    Warning: the get-methods are not thread-safe, that means that only one thread
    should actually operate on one db-connection.
"""
class Controller(object):
    def __init__(self, createLogFile = False):
        if createLogFile:
            self.logfile = self.createSessionLog()
        self.initVars()
    """creates the session-log-file"""
    def createSessionLog(self):
        #check if log-directory exists
        self.log_directory = config.log_directory.replace("\\", "/",)
        if not exists(self.log_directory):
           print("directory '"+str(self.log_directory)+"' does not exist.\nPlease create this directory or use the default one")
           self.log_directory = './'
        datestamp = strftime("%m.%d.%Y - %H:%M:%S")
        datestamp = replace(datestamp, ":", "-")
        logfilename = self.log_directory+datestamp + " session.log"
        lf = open(logfilename, "w")
        print "logfile "+str(logfilename)+" created"
        return lf
    """initialises variables"""
    def initVars(self):
        print "Controller.initVars() start"
        self.__cn = None
        self.__confDB = None
        self.__equipDB = None
        self.__spareDB = None
        print "Controller.initVars() end"
    """connects to confdb using the confDBpython library"""
    def connectConfDBPython(self):
        print "Controller.connectConfDBPython() start"
        self.__cn = None
        result = False
        try:
            self.__cn = connect()
            result =  True
        except RuntimeError, inst:
            result = False
            sys.exit("error occurred during connecting to confdb via confDBpython.\nReason: "+str(inst))
        print "confDBpython-connection:\t"+str(self.__cn)
        print "Controller.connectConfDBPython() end"
        return result
    """disconnects from confdb using the confDBpython library"""
    def disconnectConfDBPython(self):
        print "Controller.disconnectConfDBPython() start"
        if self.__cn is not None:
            try:
                disconnect(self.__cn)
            except RuntimeError, inst:
                print str(inst)
        else:
            print "No connection to close."
        self.__cn = None
        print "Controller.disconnectConfDBPython() end"
    """returns a connection to confdb using tcx_Oracle"""
    def getConfDBCxOracleConnection(self):
        print "Controller.getConfDBCxOracleConnection() start"
        if self.__confDB is None:
            try:
                self.__confDB = ConfigurationDB(self.logfile)
            except RuntimeError, inst:
                sys.exit("error occurred during connecting to confdb via cxOracle.\nReason: "+str(inst))
        print "Controller.getConfDBCxOracleConnection() end"
        return self.__confDB
    """returns a connection to equipdb/lhcbintegration using tcx_Oracle"""
    def getEquipDBCxOracleConnection(self):
        print "Controller.getEquipDBCxOracleConnection() start"
        if self.__equipDB is None:
            try:
                self.__equipDB = EquipmentDB(self.logfile)
            except RuntimeError, inst:
                sys.exit("error occurred during connecting to confdb via cxOracle.\nReason: "+str(inst))
        print "Controller.getEquipDBCxOracleConnection() end"
        return self.__equipDB
    """returns an object to handle all spare-related db operations using cx_Oracle"""
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
    """returns the connection created by the confDBpython library"""
    def getCn(self):
        if self.__cn is None:
            if self.connectConfDBPython():
                return self.__cn
        return self.__cn
