from Configurables import LHCbApp, CondDB, UpdateManagerSvc,DDDBConf
from Gaudi.Configuration import *
import os

#Use Oliver's XML DDDB describing the FT detector
#DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/myDDDB-LHCb-Feb2012/lhcb.xml"
#-- the above is a running target, therefore use the following snaphot:
DDDBConf().DbRoot = "/afs/cern.ch/user/p/phopchev/public/FT/DDDBSlice_FT_v3/lhcb.xml"

CondDB().Tags['DDDB'] = 'HEAD'


lhcbApp = LHCbApp()
lhcbApp.Simulation = True

'''
lhcbApp.DataType = '2011'
lhcbApp.DDDBtag = "head-20110914"
lhcbApp.CondDBtag = "sim-20111020-vc-md100"
#dataDir = os.environ['PANORAMIXDATA']
#EventSelector().Input = ["DATAFILE='PFN:"+dataDir+"/2011_Bs2DsmuX.dst' TYP='POOL_ROOTTREE'"]
'''

lhcbApp.DataType = "2011"
lhcbApp.DDDBtag = "MC11-20111102"
lhcbApp.CondDBtag = "sim-20111111-vc-md100"
EventSelector().Input = ["DATAFILE='/castor/cern.ch/user/o/ogruenbe/Bs_mumu_v3.sim' TYP='POOL_ROOTTREE'"]

### Set fake event time to avoid useless ERROR messages from the EventClockSvc (no DAQ/RawEvent as it is MC)
from Configurables import EventClockSvc
EventClockSvc().EventTimeDecoder = 'FakeEventTime'
