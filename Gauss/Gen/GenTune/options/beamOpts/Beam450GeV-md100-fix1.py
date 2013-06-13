# File for setting Beam conditions for MC09 for beam of 450 GeV and magnet off? down polarity
#
# Beam450GeV
#
# Syntax is: 
#  gaudirun.py $APPCONFIGOPTS/Gauss/Beam450GeV-md100-fix1.py
#              $APPCONFIGOPTS/Conditions/20091211-vc15mm-md100.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBPYTHIAROOT/options/Pythia.opts (i.e. production engine)
#              gaudi_extra_options_NN_II.py (ie. job specific: random seed,
#                               output file names, see Gauss-Job.py as example)
#
from Configurables import Gauss, Generation
from Configurables import EventClockSvc, FakeEventTime
from GaudiKernel import SystemOfUnits

#--Set the L/nbb, total cross section and revolution frequency and configure
#--the pileup tool
Gauss().Luminosity        = 0.172*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
Gauss().TotalCrossSection = 65.3*SystemOfUnits.millibarn

# Set a fix number of interaction to 1
gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#--Set the energy of the beam,
#--the half effective crossing angle (in LHCb coordinate system),
#--beta* and emittance
Gauss().BeamMomentum      = 450.0*SystemOfUnits.GeV
Gauss().BeamHCrossingAngle = -2.1*SystemOfUnits.mrad
Gauss().BeamEmittance     = 7.835*(10**(-9))*SystemOfUnits.rad*SystemOfUnits.m
Gauss().BeamBetaStar      = 10.0*SystemOfUnits.m
Gauss().BeamLineAngles    = [-0.075*SystemOfUnits.mrad, 0.035*SystemOfUnits.mrad]

#--Starting time, to identify beam conditions, all events will have the same
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 111.0*SystemOfUnits.ms
ec.EventTimeDecoder.TimeStep  =   0.0

