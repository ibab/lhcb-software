#!/usr/bin/env gaudirun.py

import os
from Gaudi.Configuration import importOptions
from Moore.Configuration import Moore
from GaudiConf.Configuration import LHCbApp

moore_root = os.path.expandvars("$MOOREROOT")
importOptions( moore_root + "/options/Moore.py" )

# now reconfigure as needed
from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().EvtMax = 2000 
Moore().Simulation = False
Moore().DataType = '2008'

# correct data file
Moore().inputFiles= [ "castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw" ]
#Moore().inputFiles= [ '/data/bfys/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw' ]

Moore().DDDBtag   = "head-20090330"
Moore().CondDBtag = "head-20090402"

# finally run Moore
Moore().applyConf()
print Moore()
