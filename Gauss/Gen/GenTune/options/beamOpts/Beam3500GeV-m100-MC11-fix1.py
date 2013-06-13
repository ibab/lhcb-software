# File for setting Beam conditions for MC11a corresponding to most of the data
# taken in 2011, but forcing always a single interaction per
# bunch crossing
# Beam 3.5 TeV, beta* = 3m , emittance(normalized) ~ 2 micron
#
# Requires Gauss v40r0 or higher.
#
# Syntax is: 
#  gaudirun.py $APPCONFIGOPTS/Gauss/Beam3500GeV-m100-MC11-fix1.py
#              $DECFILESROOT/options/30000000.opts (i.e. event type)
#              $LBGENROOT/options/GEN.py (i.e. production engine)
#              MC11a-Tags.py (i.e. database tags to be used)
#              gaudi_extra_options_NN_II.py (ie. job specific: random seed,
#                               output file names, see Gauss-Job.py as example)
#
from Configurables import Gauss, Generation
from Configurables import EventClockSvc, FakeEventTime
from GaudiKernel import SystemOfUnits

#To reduce the number of files the user in encouraged 
# to change the magnet polarity variable value as
# it applies

#magnetPolarity = 'up'	# the magnet is ON and with polarity 'Up'
magnetPolarity = 'down'	# the magnet is ON and with polarity 'Down'
#magnetPolarity = 'off'	# the magnet is OFF

#--Set the L/nbb, total cross section and revolution frequency and configure
#--the pileup tool
Gauss().Luminosity        = 0.123*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
# CrossingRate = 11.245*SystemOfUnits.kilohertz used internally
Gauss().TotalCrossSection = 91.1*SystemOfUnits.millibarn

# Set a fix number of interaction to 1
gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#--Set the energy of the beam,
Gauss().BeamMomentum      = 3.5*SystemOfUnits.TeV
#--beta* and emittance (e_norm ~ 2 microns)
Gauss().BeamEmittance     = 0.0022*SystemOfUnits.mm
Gauss().BeamBetaStar      = 3.0*SystemOfUnits.m

if magnetPolarity == 'down':
#--Set the luminous region for colliding beams and beam gas and configure
#--the corresponding vertex smearing tools, the choice of the tools is done
#--by the event type
    Gauss().InteractionPosition = [ 0.0*SystemOfUnits.mm ,
                                    0.0*SystemOfUnits.mm ,
                                    0.9*SystemOfUnits.mm ]
#--the half effective crossing angle (in LHCb coordinate system),
    Gauss().BeamHCrossingAngle = -0.520*SystemOfUnits.mrad
    Gauss().BeamLineAngles    = [-0.075*SystemOfUnits.mrad, 0.035*SystemOfUnits.mrad]
elif magnetPolarity == 'up':
    Gauss().InteractionPosition = [ 0.0*SystemOfUnits.mm ,
                                    0.0*SystemOfUnits.mm ,
                                    6.2*SystemOfUnits.mm ]
    Gauss().BeamHCrossingAngle = 0.020*SystemOfUnits.mrad
    Gauss().BeamLineAngles    = [-0.085*SystemOfUnits.mrad, 0.035*SystemOfUnits.mrad]
else:
    raise ValueError('Unknown or invalid magnet polarity.')


#
#--Starting time, all events will have the same
#--Can be used for beam conditions: YEBM (year,energy,bunch-spacing,field)
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 1301.0*SystemOfUnits.ms
ec.EventTimeDecoder.TimeStep  = 0.0

