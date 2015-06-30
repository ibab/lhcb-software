## @file
#  Small script forcing a time-out in the access to the
from Gaudi.Configuration  import *
from Configurables import CondDB, CondDBAccessSvc, DDDBConf

DDDBConf(DataType="2008")

#DDDB = CondDBAccessSvc("DDDB")
#DDDB.ConnectionTimeOut = 5

partitions = ['DDDB', 'ONLINE_2008', 'LHCBCOND', 'DQFLAGS']
msg = MessageSvc(OutputLevel=WARNING)
msg.setDebug.extend(partitions)
msg.setVerbose.extend([p + '.TimeOutChecker' for p in partitions])

import GaudiPython
app = GaudiPython.AppMgr()
app.initialize()
app.start()

import time
app.detSvc()["/dd/Conditions/Online/LHCb"] # access the DB
print "TEST ===> start"
reader = app.service('CondDBCnvSvc', GaudiPython.gbl.ICondDBReader)
reader.disconnect()
print "TEST ===> reconnect"
app.detSvc()["/dd/Conditions/Online/LHCb/Tick"] # access the DB
print "TEST ===> end"
