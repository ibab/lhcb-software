from PyQt4.QtCore import QObject
class DevicesInfo(QObject):
    def __init__(self):
        QObject.__init__(self)
        self.new_devices_without_dhcp_data = 0
        self.new_devices_with_dhcp_data = 0
        self.changed_devices = 0
        self.devices_up_to_date = 0
        self.compute()
    def getNewDevicesWithoutDHCPData(self):
        return self.new_devices_without_dhcp_data
    def setNewDevicesWithoutDHCPData(self, value):
        print "setting self.new_devices_without_dhcp_data to"+str(value)
        self.new_devices_without_dhcp_data = value
        self.compute()
    def getNewDevicesWithDHCPData(self):
        return self.new_devices_with_dhcp_data
    def setNewDevicesWithDHCPData(self, value):
        print "setting self.new_devices_with_dhcp_data to"+str(value)
        self.new_devices_with_dhcp_data = value
        self.compute()
    def getNewDevicesAtAll(self):
        return self.new_devices_at_all
    """should not be called, is beeing computed automatically"""
    def setNewDevicesAtAll(self, value):
        self.new_devices_at_all = value
    def getChangedDevices(self):
        return self.changed_devices
    def setChangedDevices(self, value):
        self.changed_devices = value
        self.compute()
    def getDevicesUpToDate(self):
        return self.devices_up_to_date
    def setDevicesUpToDate(self, value):
        self.devices_up_to_date = value
        self.compute()
    def getDevicesAtAll(self):
        return self.devices_at_all
    """should not be called, is beeing computed automatically"""
    def setDevicesAtAll(self, value):
        self.devices_at_all = value
    """computes the overall sums"""
    def compute(self):
        self.new_devices_at_all = self.new_devices_with_dhcp_data + self.new_devices_without_dhcp_data
        self.devices_at_all = self.new_devices_at_all + self.changed_devices + self.devices_up_to_date
    NewDevicesWithoutDHCPData = property(getNewDevicesWithoutDHCPData, setNewDevicesWithoutDHCPData)
    NewDevicesWithDHCPData = property(getNewDevicesWithDHCPData, setNewDevicesWithDHCPData)
    NewDevicesAtAll = property(getNewDevicesAtAll, setNewDevicesAtAll)
    ChangedDevices = property(getChangedDevices, setChangedDevices)
    DevicesUpToDate = property(getDevicesUpToDate, setDevicesUpToDate)
    DevicesAtAll = property(getDevicesAtAll, setDevicesAtAll)