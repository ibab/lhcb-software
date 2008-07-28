class Spare:
    def __init__(self, hwname, hwtype, serialnb, responsible, location, comments, mac = None, ip = None):
        self.CMACadd = mac
        self.CIPadd = ip
        self.hwname = hwname
        self.devicename = self.hwname
        self.IPname = self.devicename
        self.hwtype = hwtype
        self.typ = "spare"
        self.serialnb = serialnb
        self.sn = self.serialnb
        self.responsible = responsible
        self.location = location
        self.slot= self.location.split("f")[1]
        self.loca = self.location
        self.comments = comments
    def __cmp__(self, other):
        if (self.hwname == other.hwname and
            self.hwtype == other.hwtype and
            self.serialnb == other.serialnb and
            self.responsible == other.responsible and
            self.location == other.location and
            self.comments == other.comments):
            return 0
        else:
            return -1
    def __str__(self):
        return  str(self.hwname)+" "+str(self.hwtype)+" "+\
                str(self.serialnb)+" "+\
                str(self.responsible)+" "+\
                str(self.location)+" "+\
                str(self.comments)
    def insert(self, db):
        print "spare.insert start"
        db.InsertMultipleSpareDevices(str(self.hwname), str(self.hwtype), str(self.serialnb), str(self.responsible), str(self.location), str(self.comments), 1, 1)
        print "spare.insert end"
    def update(self, confdb):
        print "spare.update start"
        query = "UPDATE lhcb_hw_devices SET hwname = '"+str(self.hwname)+"', hwtype = '"+str(self.hwtype)+"', responsible='"+str(self.responsible)+"', location='"+str(self.location)+"', user_comments='"+str(self.comments)+"' where serialnb='"+str(self.serialnb)+"'"
        confdb.executeQuery(query)
        print "spare.update end" 
    def summary(self):
        return self.__str__()
