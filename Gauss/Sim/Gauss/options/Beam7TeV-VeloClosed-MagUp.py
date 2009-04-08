# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam7TeV-VeloClosed-MagUp 
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC09.py 
#              $GAUSSOPTS/Beam7TeV-VeloClosed-MagUp.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to generate Magnetic Field up (positive polarity)
LHCbApp().CondDBtag = "sim-20090402-vc-mu100"

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 2000*SystemOfUnits.ns
