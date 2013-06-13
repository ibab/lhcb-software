# File for setting Beam conditions for MC11a corresponding to most of the data
# taken in 2011 for magnet polarity down, but forcing always a single interaction per
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

#--Set the L/nbb, total cross section and revolution frequency and configure
#--the pileup tool
Gauss().Luminosity        = 0.123*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
Gauss().TotalCrossSection = 91.1*SystemOfUnits.millibarn

gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#--Set the luminous region for colliding beams and beam gas and configure
#--the corresponding vertex smearing tools, the choice of the tools is done
#--by the event type
    Gauss().InteractionPosition = [ 0.0*SystemOfUnits.mm ,
                                    0.0*SystemOfUnits.mm ,
                                    0.9*SystemOfUnits.mm ]

# Set a fix number of interaction to 1
#--Set the energy of the beam,
#--the half effective crossing angle (in LHCb coordinate system),
#--beta* and emittance (e_norm ~ 2 microns)
Gauss().BeamMomentum       = 3.5*SystemOfUnits.TeV
Gauss().BeamHCrossingAngle = -0.520*SystemOfUnits.mrad
Gauss().BeamLineAngles     = [-0.075*SystemOfUnits.mrad, 0.035*SystemOfUnits.mrad]
Gauss().BeamEmittance      = 0.0022*SystemOfUnits.mm
Gauss().BeamBetaStar       = 3.0*SystemOfUnits.m

#--Starting time, all events will have the same
#--Can be used for beam conditions: YEBM (year,energy,bunch-spacing,field)
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 1301.0*SystemOfUnits.ms
ec.EventTimeDecoder.TimeStep  = 0.0

