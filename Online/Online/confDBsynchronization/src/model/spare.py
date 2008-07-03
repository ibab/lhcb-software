class Spare:
    def __init__(self, hwname, hwtype, serialnb, responsible, location, comments):
        self.hwname = hwname
        self.devicename = self.hwname
        self.hwtype = hwtype
        self.typ = "spare"
        self.serialnb = serialnb
        self.sn = self.serialnb
        self.responsible = responsible
        self.location = location
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
        db.InsertMultipleSpareDevices(self.hwname, self.hwtype, self.serialnb, self.responsible, self.location, self.comments, 1, 1)
        print "spare.insert end"
    def update(self, confdb):
        print "spare.update start"
        query = "UPDATE lhcb_hw_devices SET hwname = '"+self.hwname+"', hwtype = '"+self.hwtype+"', responsible='"+self.responsible+"', location='"+self.location+"', user_comments='"+self.comments+"' where serialnb='"+self.serialnb+"'"
        confdb.executeQuery(query)
        print "spare.update end" 
