from database.spare import Spare
from cx_Oracle import IntegrityError

class SwapDevicesConfDB(object):
    def __init__(self, confDB, equipDB, spareDB):
        self.confDB = confDB
        self.equipDB = equipDB
        self.spareDB = spareDB
    def swapDeviceAndDevice(self):
        print "TESTCASE: SwapDevicesConfDB.swapDeviceAndDevice start"
        equipDB_system = self.equipDB.getSystem()
        hugin1 = None
        hugin2 = None
        for device in equipDB_system.getAllDevices():
            if device.typ == "hugin":
                if hugin1 is None:
                    hugin1 = device
                    print "hugin1 = "+str(hugin1.devicename)
                    continue
                else:
                    hugin2 = device
                    print "hugin2 = "+str(hugin2.devicename)
                    break #hugins found, now lets test
        print "hugins found, swapping hugins"
        if hugin1 is not None and hugin2 is not None:
            return self.confDB.swapDevices(hugin1, hugin2)
        else:
            return False
    def swapSpareAndSpare(self):
        insert_testspare1_query = "insert into lhcb_hw_devices (serialnb,device_status,responsible,user_comments,hwname,location) VALUES ('9ODLAUTL000001', '2','test','test_comment','testspare001', 'D9B01F99f01')"
        insert_testspare2_query = "insert into lhcb_hw_devices (serialnb,device_status,responsible,user_comments,hwname,location) VALUES ('9ODLAUTL000002', '2','test','test_comment','testspare002', 'D9B01F99f02')"
        try:
            self.confDB.executeQuery(insert_testspare1_query)
        except IntegrityError:
            print "testspare1 already exists"
        try:
            self.confDB.executeQuery(insert_testspare2_query)
        except IntegrityError:
            print "testspare2 already exists"
        spare1 = self.spareDB.getSpareConfDBByName("testspare001")
        spare2 = self.spareDB.getSpareConfDBByName("testspare002")
        if spare1 is not None and spare2 is not None:
            return self.confDB.swapDevices(spare1, spare2)
        else:
            return False
    def swapSpareAndDevice(self):
        print "TESTCASE: SwapDevicesConfDB.swapSpareAndDevice start"
        equipDB_system = self.equipDB.getSystem()
        insert_testspare1_query = "insert into lhcb_hw_devices (serialnb,device_status,responsible,user_comments,hwname,location) VALUES ('9ODLAUTL000001', '2','test','test_comment','testspare001', 'D9B01F99f01')"
        try:
            self.confDB.executeQuery(insert_testspare1_query)
        except IntegrityError:
            print "testspare1 already exists"
        spare1 = self.spareDB.getSpareConfDBByName("testspare001")
        tell1 = None
        for device in equipDB_system.getAllDevices():
            if device.typ == "tell":
                tell1 = device
                break
        if spare1 is not None and tell1 is not None:
            return self.confDB.swapDevices(spare1, tell1) #and self.confDB.swapDevices(tell1, spare1)
        else:
            return False
    def doAllTestCases(self):
        if self.swapDeviceAndDevice():
            print "TESTCASE: SwapDevicesConfDB.swapDeviceAndDevice was SUCCESSFUL"
        else:
            print "TESTCASE: SwapDevicesConfDB.swapDeviceAndDevice FAILED"
        if self.swapSpareAndSpare():
            print "TESTCASE: SwapDevicesConfDB.swapSpareAndSpare was SUCCESSFUL"
        else:
            print "TESTCASE: SwapDevicesConfDB.swapSpareAndSpare FAILED"
        if self.swapSpareAndDevice():
            print "TESTCASE: SwapDevicesConfDB.swapSpareAndDevice was SUCCESSFUL"
        else:
            print "TESTCASE: SwapDevicesConfDB.swapSpareAndDevice FAILED"
        self.clean()
    def clean(self):
        insert_testspare1_query = "DELETE FROM lhcb_hw_devices WHERE serialnb = '9ODLAUTL000001' AND hwname = 'testspare001'"
        insert_testspare2_query = "DELETE FROM lhcb_hw_devices WHERE serialnb = '9ODLAUTL000002' AND hwname = 'testspare002'"
        try:
            self.confDB.executeQuery(insert_testspare1_query)
        except:
            print "was not able to do "+str(insert_testspare1_query)
        try:
            self.confDB.executeQuery(insert_testspare2_query)
        except:
            print "was not able to do "+str(insert_testspare2_query)