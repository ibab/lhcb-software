# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloClosed-MagDown
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam450GeV-VeloClosed-MagDown.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, the options already set zero crossing angle
#--as for BfieldOff
importOptions("$GAUSSOPTS/PilotRun.opts")

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 100000*SystemOfUnits.ns
