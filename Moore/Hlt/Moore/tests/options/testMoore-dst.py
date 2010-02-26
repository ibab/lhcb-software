#!/usr/bin/env gaudirun.py

# this option file is for a qmtest
# uses the standard option file "options/Moore.py" 
# but makes sure:
#          EventSelector().PrintFreq = 100
#          Moore().EvtMax = 2000
#
# Author: Stephan Nies    

import os
from Gaudi.Configuration import importOptions
from Moore.Configuration import Moore

moore_root = os.path.expandvars("$MOOREROOT")
importOptions( moore_root + "/options/Moore.py" )

# now reconfigure as needed
from Configurables import EventSelector
EventSelector().PrintFreq = 100
Moore().EvtMax = 2000 

# correct data file
Moore().inputFiles = [ 'castor:/castor/cern.ch/user/d/dijkstra/Stripped-L0-MC09/Sim03Reco02-Mbias_%d.mdf'%f for f in range(1,5) ]  
#                  Moore().inputFiles = [ '/data/bfys/lhcb/MinBias-L0xHlt1strip/Sim03Reco02-Mbias_%d.mdf'%(i) for i in [ 1, 2 ]  ]

# finally run Moore
Moore().applyConf()
print Moore()
