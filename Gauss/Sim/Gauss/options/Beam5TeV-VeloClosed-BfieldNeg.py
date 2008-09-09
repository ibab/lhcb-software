# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam5TeV-VeloClosed-BfieldNeg
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-2008.py
#              $GAUSSOPTS/Beam5TeV-VeloClosed-BfieldNeg.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $GAUSSOPTS/Gauss-JobExample.py (ie. job specific: random seed,
#                                                  output file names...)
#
from Gauss.Configuration import *

#--Tell to use 5 TeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, the options already set zero crossing angle
#--as for BfieldOff
importOptions("$GAUSSOPTS/BeamCond-5TeV.opts")

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 200000*ns
