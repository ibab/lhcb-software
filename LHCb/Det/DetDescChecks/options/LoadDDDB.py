# Options to load CondDB with default tag as defined in LHCbApp() configurable
# To use another tag, provide options like e.g. $DDDBROOT/options/LHCb-2008.py
from Gaudi.Configuration import *
from DDDB.Configuration import *

from Configurables import LoadDDDB 

DDDBConf() # Configure the detector description
#DDDBConf().DataType = "DC06"

# ---------- option to use Oracle CondDB instead of SQLDDDB
#from Configurables import CondDB
#CondDB(UseOracle = True)

ApplicationMgr().EvtSel = "NONE"
ApplicationMgr().EvtMax = 1

ApplicationMgr().TopAlg  = [ GaudiSequencer(Members = [ LoadDDDB() ],
                                            MeasureTime = True) ]

# ---------- option to select only a subtree
#LoadDDDB().Node = "/dd/Geometry*"
