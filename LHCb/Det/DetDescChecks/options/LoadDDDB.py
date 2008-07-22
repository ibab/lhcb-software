from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from Configurables import LoadDDDB

# ---------- option to use Oracle CondDB instead of SQLDDDB
#LHCbApp().useOracleCondDB = True

importOptions( "$DDDBROOT/options/DDDB.py" )
importOptions( "$DDDBROOT/options/DC06.opts" )

ApplicationMgr().EvtSel     = "NONE"
ApplicationMgr().EvtMax     = 1

ApplicationMgr().TopAlg  = [ "GaudiSequencer" ]
GaudiSequencer().Members += [ "LoadDDDB" ]
GaudiSequencer().MeasureTime = True

importOptions( "$DDDBROOT/options/DDDB.py" )
importOptions( "$DDDBROOT/options/DC06.opts" )

# ---------- option to select only a subtree
#LoadDDDB().Node = "/dd/Geometry*"


LHCbApp().applyConf()
