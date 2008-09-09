# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam5TeV-VeloOpen-BfieldZero 
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam5TeV-VeloClose-BfieldZero.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-JobExample.py (ie. job specific: random seed,
#                                                  output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
from DetCond.Configuration import addCondDBLayer
simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
simCondVelo.DefaultTAG = "velo-open"
addCondDBLayer(simCondVelo)

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

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 111000*ns
