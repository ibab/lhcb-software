
from Gaudi.Configuration import *

RunNr = "111185_1"
NTupleSvc().Output = [ "FILE1 DATAFILE='data_" + RunNr + ".root' TYP='ROOT' OPT='NEW'" ]
HistogramPersistencySvc().OutputFile = "data_" + RunNr + "_hists.root"

EventSelector().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/111185/111185_0000000003.raw' SVC='LHCb::MDFSelector'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/111185/111185_0000000006.raw' SVC='LHCb::MDFSelector'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/111185/111185_0000000007.raw' SVC='LHCb::MDFSelector'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/111185/111185_0000000008.raw' SVC='LHCb::MDFSelector'",
  "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/111185/111185_0000000018.raw' SVC='LHCb::MDFSelector'"
]

#from Gaudi.Configuration import *
#from Configurables import CondDB, LHCbApp
#CondDB().LocalTags['LHCBCOND'] = [ 'align-20120410' ]

#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("align_db",
#  ConnectionString = "sqlite_file:alignDB.early2012.2days.jpsi.db/LHCBCOND")
#)

#from Configurables import ( CondDBAccessSvc, CondDB )
#CondDB().addLayer(CondDBAccessSvc("t0_db",
#  ConnectionString = "sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/2012-04/ModuleT0s_2012_04_111185_v2.db/LHCBCOND")
#)

importOptions("data_i.py")
