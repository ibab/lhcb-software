# File for setting Beam conditions for February 2013 pA and Ap
# with magnet down.
# Beam xx TeV, beta* = x m , emittance(normalized) ~ x micron
#
# Requires Gauss v45r2 or higher.
#
from Configurables import Gauss
from Configurables import EventClockSvc, FakeEventTime
from GaudiKernel import SystemOfUnits

# give a lumi value to sore in the event (will not be used)
Gauss().Luminosity        = 0.122*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
Gauss().TotalCrossSection = 93.2*SystemOfUnits.millibarn

# Set a fix number of interaction to 1
from Configurables import Generation
gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#--Set the luminous region for colliding beams and beam gas and configure
#--the corresponding vertex smearing tools, the choice of the tools is done
#--by the event type
Gauss().BunchRMS = 82.03*SystemOfUnits.mm

Gauss().InteractionPosition = [ 0.0*SystemOfUnits.mm ,
                                0.0*SystemOfUnits.mm ,
                                20.0*SystemOfUnits.mm ]

#--Set the energy of the beam,
#--the half effective crossing angle (in LHCb coordinate system),
#--beta* and emittance (e_norm ~ 2 microns)
Gauss().BeamMomentum      = 5.023*SystemOfUnits.TeV
Gauss().BeamHCrossingAngle = -0.236*SystemOfUnits.mrad
Gauss().BeamVCrossingAngle = 0.0
Gauss().BeamEmittance     = 0.0022*SystemOfUnits.mm
Gauss().BeamBetaStar      = 3.0*SystemOfUnits.m
Gauss().BeamLineAngles    = [0.0, 0.0]

# Introduce a vertical crossing angle
def setVCrossingAngle():
    from Configurables import GenInit
    genInit = GenInit( "GaussGen" )
    genInit.VerticalCrossingAngle   = 0.100*SystemOfUnits.mrad

from Gaudi.Configuration import *
appendPostConfigAction( setVCrossingAngle )

#
#--Starting time, all events will have the same
#--Can be used for beam conditions: YEBM (year,energy,bunch-spacing,field)
ec = EventClockSvc()
ec.addTool(FakeEventTime(), name="EventTimeDecoder")
ec.EventTimeDecoder.StartTime = 2401.0*SystemOfUnits.ms
ec.EventTimeDecoder.TimeStep  = 0.0

