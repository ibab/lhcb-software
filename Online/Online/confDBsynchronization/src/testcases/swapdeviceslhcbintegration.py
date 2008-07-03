from cx_Oracle import IntegrityError
from string import upper

class SwapDevicesLHCBIntegration(object):
    def __init__(self, equipDB):
        self.equipDB = equipDB
    def swapDevices(self):
        insert_testdevice1_query = "INSERT INTO board (name, responsible, type, label, item_id) VALUES ('testsparedsonnick1', 'dsonnick', 'electronics', 'D9B02F99f01', '9ODLAUTL000001')"
        insert_testdevice2_query = "INSERT INTO board (name, responsible, type, label, item_id) VALUES ('testsparedsonnick2', 'dsonnick', 'electronics', 'D9B02F99f02', '9ODLAUTL000002')"
        try:
            self.equipDB.executeQuery(insert_testdevice1_query)
        except IntegrityError:
            print "testdevice1 already exists"
        try:
            self.equipDB.executeQuery(insert_testdevice2_query)
        except IntegrityError:
            print "testdevice2 already exists"
        all_spares = self.equipDB.getAllSpares()
        testdevice1 = None
        testdevice2 = None
        for spare in all_spares:
            print spare.hwname
            if spare.hwname == "testsparedsonnick1":
                testdevice1 = spare
            if spare.hwname == "testsparedsonnick2":
                testdevice2 = spare
        if testdevice1 is not None and testdevice2 is not None:
            print "try to swap "+testdevice1.hwname+" and "+testdevice2.hwname
            return self.equipDB.swapDevices(testdevice1, testdevice2)
        else:
            return False
    def doAllTestCases(self):
        if self.swapDevices():
            print "TESTCASE: SwapDevicesLHCBIntegration.swapDevices was SUCCESSFUL"
        else:
            print "TESTCASE: SwapDevicesLHCBIntegration.swapDevices FAILED"
        self.clean()
    def clean(self):
        delete_testdevice1_query = "DELETE FROM board WHERE name = 'testsparedsonnick1' AND item_id = '9ODLAUTL000001'"
        delete_testdevice2_query = "DELETE FROM board WHERE name = 'testsparedsonnick2' AND item_id = '9ODLAUTL000002'"
        try:
            self.equipDB.executeQuery(delete_testdevice1_query)
        except IntegrityError:
            print "was not able to do "+str(delete_testdevice1_query)
        try:
            self.equipDB.executeQuery(delete_testdevice2_query)
        except IntegrityError:
            print "was not able to do "+str(delete_testdevice2_query)