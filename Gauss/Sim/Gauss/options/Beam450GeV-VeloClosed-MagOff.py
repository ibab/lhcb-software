# File for setting SIMCOND settings from python promt for a given
# configuration: Beam energy, Velo Status, Magnetic Field Status 
#
# Beam450GeV-VeloClosed-MagOff
#
# Syntax is: 
#  gaudirun.py $GAUSSOPTS/Gauss-MC09.py
#              $GAUSSOPTS/Beam450GeV-VeloClosed-MagOff.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              $GAUSSOPTS/Gauss-Job.py (ie. job specific: random seed,
#                                                         output file names...)
#
from Gauss.Configuration import *

#--Tell SIMCOND to generate Magnetic Field OFF
LHCbApp().CondDBtag = "sim-20090402-vc-moff"

#--Tell to use 450 GeV beams for collisions and beam gas, with corresponding
#--beam size and luminous region, the options  set zero crossing angle as
#--Bfield On, so it has to be set to zero here
importOptions("$GAUSSOPTS/PilotRun.opts")

# Set zero crossing angle assuming no external angle, hence this conditions
# are always with only up to 156 bunches filling schema
setCrossingAngle(0.0)

#--Starting time
ec = EventClockSvc()
ec.EventTimeDecoder.StartTime = 101000*SystemOfUnits.ns

