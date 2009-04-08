# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam5TeV-VeloOpen-MagOff 
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC09.py
#              $GAUSSOPTS/Beam5TeV-VeloOpen-MagOff.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND tag to generate Open VELO
#                           and Field off
LHCbApp().CondDBtag = "sim-20090402-vo-moff"

#--Tell to use 5 TeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region
importOptions("$GAUSSOPTS/BeamCond-5TeV.py")

# Set zero crossing angle assuming no external angle, hence this conditions
# are always with only up to 156 equidistant bunches filling schema
setCrossingAngle(0.0)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 211000*SystemOfUnits.ns
