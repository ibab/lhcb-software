## @file
#  Small script forcing a time-out in the access to the
from Gaudi.Configuration  import *
from Configurables import CondDB, CondDBAccessSvc, DDDBConf

DDDBConf()

#DDDB = CondDBAccessSvc("DDDB")
#DDDB.ConnectionTimeOut = 5

MessageSvc(OutputLevel=DEBUG)

import GaudiPython
app = GaudiPython.AppMgr()
app.initialize()
app.start()

import time
app.detSvc()["/dd/Conditions/Online/LHCb"] # access the DB
print "TEST ===> start"
reader = app.service('CondDBCnvSvc', GaudiPython.gbl.ICondDBReader)
reader.disconnect()
print "TEST ===> end"
