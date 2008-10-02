from Gaudi.Configuration import * 
# Defines default conditions for 2008 LHCb detector

from Configurables import CondDBAccessSvc
import os

# 2008 database tags
tag = "head-20081002"
CondDBAccessSvc( "DDDB",     DefaultTAG = tag )
CondDBAccessSvc( "LHCBCOND", DefaultTAG = tag )
CondDBAccessSvc( "SIMCOND",  DefaultTAG = tag )
