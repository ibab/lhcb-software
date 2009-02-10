# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloOpen-MagOff
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam450GeV-VeloClose-MagOff.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
from Configurables import CondDB
## from DetCond.Configuration import addCondDBLayer
## simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
## simCondVelo.DefaultTAG = "velo-open"
## addCondDBLayer(simCondVelo)
CondDB().LocalTags["SIMCOND"] = ["velo-open"]

#--Tell SIMCOND to generate Magnetic Field OFF, will be as tag "magnet-off"
from Configurables import UpdateManagerSvc
UpdateManagerSvc().ConditionsOverride += [
 "Conditions/HardwareProperties/LHCb/Magnet/ScaleUp := double_v Coeffs = 0 0;",
 "Conditions/HardwareProperties/LHCb/Magnet/ScaleDown := double_v Coeffs = 0 0;"
]

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, the options already set zero crossing angle
#--as for BfieldOff
importOptions("$GAUSSOPTS/PilotRun.opts")
setCrossingAngle(0.0)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 111000*SystemOfUnits.ns
