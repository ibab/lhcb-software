# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam7TeV-VeloOpen-MagDown
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC09.py
#              $GAUSSOPTS/Beam7TeV-VeloOpen-MagDown.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to use tag for Open VELO
#--                        and Magnetic Field down (negative polarity)
LHCbApp().CondDBtag = "sim-20090402-vo-md100"

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 10000*SystemOfUnits.ns
