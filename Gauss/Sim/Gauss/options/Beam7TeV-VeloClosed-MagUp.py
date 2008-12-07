# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam7TeV-VeloClosed-MagUp 
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py 
#              $GAUSSOPTS/Beam7TeV-VeloClosed-MagUp.py
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

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 2000*SystemOfUnits.ns
