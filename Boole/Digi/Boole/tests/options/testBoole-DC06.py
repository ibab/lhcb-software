##############################################################################
# File for running a Boole test with default DC06 geometry
##############################################################################

from Boole.Configuration import *

Boole().EvtMax       = 10
Boole().writeL0ETC   = True   # Write also ETC of L0 selected events
Boole().monitors    += ["SC"] # Add StatusCode check

Boole().applyConf()

# Do not compare times....
TimingAuditor().OutputLevel = WARNING 

# Data files
importOptions( "$BOOLEOPTS/DC06-Files.py" ) 
