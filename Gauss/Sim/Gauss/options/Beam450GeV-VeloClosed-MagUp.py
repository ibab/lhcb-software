# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloClosed-MagUp
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC09.py
#              $GAUSSOPTS/Beam450GeV-VeloClosed-MagUp.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to generate Magnetic Field up (positive polarity)
LHCbApp().CondDBtag = "sim-20090402-vc-mu100"

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, the options already set zero crossing angle
#--as for BfieldOff
importOptions("$GAUSSOPTS/PilotRun.opts")
setCrossingAngle(2.1*SystemOfUnits.mrad)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 102000*SystemOfUnits.ns
