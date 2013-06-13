# File for setting Beam conditions for MC10 simulation
#
# Beam 3.5 TeV
#
# Syntax is: 
#  gaudirun.py $APPCONFIGOPTS/Gauss/Beam3500GeV-mu100-MC10-fix1.py
#              $APPCONFIGOPTS/Conditions/XXXXXXX.py
#              $DECFILESROOT/options/30000000.opts (ie. event type)
#              $LBGENROOT/options/GEN.py (i.e. production engine)
#              gaudi_extra_options_NN_II.py (ie. job specific: random seed,
#                               output file names, see Gauss-Job.py as example)
#
from Gauss.Configuration import * 
from Configurables import Generation, FixedNInteractions

#--Set the L/nbb, total cross section and revolution frequency and configure
#--the pileup tool fixing one interaction per bunch crossing
Gauss().Luminosity        = 0.676*(10**28)/(SystemOfUnits.cm2*SystemOfUnits.s)
Gauss().TotalCrossSection = 91.1*SystemOfUnits.millibarn

gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#--Set the luminous region for colliding beams and beam gas and configure
#--the corresponding vertex smearing tools, the choice of the tools is done
#--by the event type
Gauss().InteractionPosition = [ 0.0*SystemOfUnits.mm ,
                                0.0*SystemOfUnits.mm ,
                                5.0*SystemOfUnits.mm ]

#--Set the energy of the beam,
#--the half effective crossing angle (in LHCb coordinate system),
#--beta* and emittance
Gauss().BeamMomentum      = 3.5*SystemOfUnits.TeV
Gauss().BeamCrossingAngle = 0.270*SystemOfUnits.mrad
Gauss().BeamEmittance     = 0.67*(10**(-9))*SystemOfUnits.rad*SystemOfUnits.m
Gauss().BeamBetaStar      = 2.0*SystemOfUnits.m
Gauss().BeamLineAngles    = [-0.06*SystemOfUnits.mrad, 0.025*SystemOfUnits.mrad]


#--Starting time, to identify beam conditions, all events will have the same
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 311.0*SystemOfUnits.ms
ec.EventTimeDecoder.TimeStep  = 0.0

