from PyQt4.QtCore import QThread
from PyQt4 import QtCore
from time import strftime
from string import replace
class CreateSummaryLogWorker(QThread):
    def __init__(self, confDB, equipDB):
        QThread.__init__(self)
        self.confDB = confDB
        self.equipDB = equipDB
    def run(self):
        print "CreateSummaryLogWorker.run() start"
        equipDBSystem = self.equipDB.getSystem()
        changedSystems = self.confDB.getChangedSystems(equipDBSystem)
        line = "-----------------------------------------------------------------------"
        datestamp = strftime("%m.%d.%Y - %H:%M:%S")
        datestamp = replace(datestamp, ":", "-")
        lf = open("log/"+datestamp + " summary.log", "w")
        lf.write("Summary log, created "+datestamp)
        headings = ["\n\n\nNew devices with DHCP data\n",
                    "\n\n\nNew devices without DHCP data",
                    "\n\n\nNew devices where the location has changed",
                    "\n\n\nNew devices where the dhcp data has changed",
                    "\n\n\nNew devices which are up-to-date",
                    ]
        i = 0
        for system in changedSystems:
            lf.write(headings[i])
            i += 1
            for device in system.getAllDevices():
                lf.write("\n"+str(device))
        lf.close()
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "CreateSummaryLogWorker.run() end"