from string import upper
import csv
from sys import exit

class DHCP:
    """
    Given an Iterator (e.g. CSV iteratable), the DHCP-class will find
    the appropriate date (mac adresse and ip adress)
    
    You can say which index (column) of the data the iterator returns is which data
    """ 
    def __init__(self, file, flags, name_index = 0, location_index = 1, ip_index = 2, mac_index = 3, serial_index = 6):
        self.file = file
        self.flags = flags
        self.name_index = name_index
        self.location_index = location_index
        self.ip_index = ip_index
        self.mac_index = mac_index
        self.serial_index = serial_index
        self.csv_file_name = self.file.replace("txt", "csv")
        self.transformFile()
    def getMACByIPName(self, name):
        for row in csv.reader(open(self.csv_file_name, self.flags)):
            if(row != None and len(row) >= 4):
                if(upper(row[self.name_index]) == upper(name)):
                    return row[self.mac_index]
        return None
    def getIPByIPName(self, name):
        for row in csv.reader(open(self.csv_file_name, self.flags)):
            if(row != None and len(row) >= 4):
                if(upper(row[self.name_index]) == upper(name)):
                    return row[self.ip_index]
        return None
    def getMACBySerial(self, serial):
        for row in csv.reader(open(self.csv_file_name, self.flags)):
            if(row != None and len(row) >= 4):
                if(upper(row[self.serial_index]) == upper(serial)):
                    return row[self.mac_index]
    def getIPBySerial(self, serial):
        for row in csv.reader(open(self.csv_file_name, self.flags)):
            if(row != None and len(row) >= 4):
                if(upper(row[self.serial_index]) == upper(serial)):
                    return row[self.ip_index]
    def transformFile(self):
        try:
            file = open(self.file, self.flags)
        except IOError:
            print "file ", self.file, " not found or readable"
            exit(2)
        csv_file = open(self.csv_file_name, "w")
        for line in file:
            new_line = line[1:-3] # cuts leading and ending |
            new_line = new_line.replace("|", ",")
            csv_file.write(new_line)
            csv_file.write("\n")
        file.close()
        csv_file.close()