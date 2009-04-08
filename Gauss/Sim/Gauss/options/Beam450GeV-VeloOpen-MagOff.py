# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloOpen-MagOff
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC009.py
#              $GAUSSOPTS/Beam450GeV-VeloClose-MagOff.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
#                           and Field off
LHCbApp().CondDBtag = "sim-20090402-vo-moff"

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region
importOptions("$GAUSSOPTS/PilotRun.opts")

# Set zero crossing angle assuming no external angle, hence this conditions
# are always with only up to 156 bunches filling schema
setCrossingAngle(0.0)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 111000*SystemOfUnits.ns
