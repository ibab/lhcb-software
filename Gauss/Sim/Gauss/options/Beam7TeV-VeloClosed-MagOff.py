# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam7TeV-VeloClosed-MagOff 
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam7TeV-VeloClosed-MagOff.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to generate Magnetic Field OFF, will be as tag "magnet-off"
from Configurables import UpdateManagerSvc
UpdateManagerSvc().ConditionsOverride += [
 "Conditions/HardwareProperties/LHCb/Magnet/ScaleUp := double_v Coeffs = 0 0;",
 "Conditions/HardwareProperties/LHCb/Magnet/ScaleDown := double_v Coeffs = 0 0;"
]
setCrossingAngle(0.0)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 1000*SystemOfUnits.ns
