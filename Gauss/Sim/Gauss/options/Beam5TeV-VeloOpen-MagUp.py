# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam5TeV-VeloOpen-MagUp
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam5TeV-VeloOpen-MagUp.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
from DetCond.Configuration import addCondDBLayer
simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
simCondVelo.DefaultTAG = "velo-open"
addCondDBLayer(simCondVelo)

#--Tell SIMCOND to generate Magnetic Field of positive polarity (or mag up)
from Configurables import UpdateManagerSvc
UpdateManagerSvc().ConditionsOverride += [
 "Conditions/Online/LHCb/Magnet/Measured :=  int Polarity = 1;",
]

#--Tell to use 5 TeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region
importOptions("$GAUSSOPTS/BeamCond-5TeV.opts")

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 212000*SystemOfUnits.ns
