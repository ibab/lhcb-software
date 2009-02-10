# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloClosed-MagUp
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam450GeV-VeloClosed-MagUp.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to generate Magnetic Field of positive polarity (or mag up)
from Configurables import UpdateManagerSvc
UpdateManagerSvc().ConditionsOverride += [
 "Conditions/Online/LHCb/Magnet/Measured :=  int Polarity = 1;",
]

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, the options already set zero crossing angle
#--as for BfieldOff
importOptions("$GAUSSOPTS/PilotRun.opts")
setCrossingAngle(2.1*SystemOfUnits.mrad)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 102000*SystemOfUnits.ns
