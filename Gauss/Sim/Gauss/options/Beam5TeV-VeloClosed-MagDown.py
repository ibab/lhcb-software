# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam5TeV-VeloClosed-MagDown
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC09.py
#              $GAUSSOPTS/Beam5TeV-VeloClosed-MagDown.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to generate Velo closed, Magnet down
LHCbApp().CondDBtag = "sim-20090402-vc-md100"

#--Tell to use 5 TeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, and effective crossing angle as for polarity
#--up but this should be different!
importOptions("$GAUSSOPTS/BeamCond-5TeV.py")
setCrossingAngle(0.121*SystemOfUnits.mrad)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 200000*SystemOfUnits.ns
