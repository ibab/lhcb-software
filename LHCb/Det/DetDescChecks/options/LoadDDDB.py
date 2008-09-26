# Options to load CondDB with default tag as defined in LHCbApp() configurable
# To use another tag, provide options like e.g. $DDDBROOT/options/LHCb-2008.py
from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from Configurables import LoadDDDB

# ---------- option to use Oracle CondDB instead of SQLDDDB
#LHCbApp().useOracleCondDB = True

ApplicationMgr().EvtSel     = "NONE"
LHCbApp().EvtMax            = 1

ApplicationMgr().TopAlg  = [ "GaudiSequencer" ]
GaudiSequencer().Members += [ "LoadDDDB" ]
GaudiSequencer().MeasureTime = True

# ---------- option to select only a subtree
#LoadDDDB().Node = "/dd/Geometry*"

LHCbApp().applyConf()
