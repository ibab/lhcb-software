class Device(object):
    equipdb_devices = []
    confdb_devices = []
    new_devices_with_dhcp = []
    new_devices_no_dhcp = []
    changed_devices = []
    up_to_date_devices = []
    new_devices = []
    def __init__(self):
        pass
    def reset():
        Device.equipdb_devices = []
        Device.confdb_devices = []
        Device.new_devices_with_dhcp = []
        Device.new_devices_no_dhpc = []
        Device.changed_devices = []
        Device.up_to_date_devices = []
        Device.new_devices = []
    def printInfo():
        print "Device.equipdb_devices: " + str(len(Device.equipdb_devices))
        print "Device.confdb_devices: " + str(len(Device.confdb_devices))
        print "new_devices_with_dhcp: " + str(len(Device.new_devices_with_dhcp))
        print "Device.new_devices_no_dhpc: " + str(len(Device.new_devices_no_dhpc))
        print "Device.changed_devices: " + str(len(Device.changed_devices))
        print "Device.up_to_date_devices: " + str(len(Device.up_to_date_devices))
    
    reset = staticmethod(reset)
    printInfo = staticmethod(printInfo)