class ConnectController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.confDB = self.parentController.getConfDBCxOracleConnection()
        system = self.parentController.getEquipDBCxOracleConnection().getSystem()
        allDevices = system.getAllDevices()
        self.confDB.createThrottleConnectivity(self.parentController.getCn(), allDevices,  system)