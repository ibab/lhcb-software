## @file
#  Small script forcing a time-out in the access to the 
from Gaudi.Configuration  import *
from Configurables import CondDB, CondDBAccessSvc, DDDBConf

DDDBConf()

DDDB = CondDBAccessSvc("DDDB")
DDDB.ConnectionTimeOut = 5

#MessageSvc(OutputLevel = ERROR)

import GaudiPython
app = GaudiPython.AppMgr()
app.initialize()
app.start()

import time
app.detSvc()["/dd"] # access the DB
print "TEST ===> start"
time.sleep(6) # wait enough
print "TEST ===> end"
