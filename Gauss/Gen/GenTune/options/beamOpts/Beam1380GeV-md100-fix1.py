# File for setting Beam conditions for 2011 simulation
#
# Beam 1.38 TeV
#
# Syntax is: 
#  gaudirun.py $APPCONFIGOPTS/Gauss/Beam1380GeV-md100-fix1.py
#              $APPCONFIGOPTS/Conditions/XXXXXXX.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBGENROOT/options/GEN.py (i.e. production engine)
#              gaudi_extra_options_NN_II.py (ie. job specific: random seed,
#                               output file names, see Gauss-Job.py as example)
#
from Gauss.Configuration import * 

#--Set the L/nbb, total cross section and revolution frequency and configure
#--the pileup tool
Gauss().Luminosity        = 0.143*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
Gauss().TotalCrossSection = 78.4*SystemOfUnits.millibarn

#--Set the energy of the beam,
#--the half effective crossing angle (in LHCb coordinate system),
#--beta* and emittance
Gauss().BeamMomentum      = 1.38*SystemOfUnits.TeV
Gauss().BeamHCrossingAngle = -0.69*SystemOfUnits.mrad
Gauss().BeamEmittance     = 2.53*(10**(-9))*SystemOfUnits.rad*SystemOfUnits.m
Gauss().BeamBetaStar      = 10.0*SystemOfUnits.m

#--Starting time, to identify beam conditions, all events will have the same
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 301.0*SystemOfUnits.ms
ec.EventTimeDecoder.TimeStep  = 0.0

#-- Fix one interaction per bunch crossing!
from Configurables import Generation
Generation("Generation").PileUpTool = "FixedNInteractions"
