# File for setting Beam conditions corresponding to most of the data
# taken in 2011 with magnet down but with nominal beam line and only one fix
# interaction
#
# Requires Gauss v45r1 or higher.
#
# Syntax is: 
#  gaudirun.py $APPCONFIGOPTS/Gauss/Beam3500GeV-md100-2011-nominalBeamLinefix1.py
#              $DECFILESROOT/options/30000000.opts (i.e. event type)
#              $LBGENROOT/options/GEN.py (i.e. production engine)
#              Sim08-2011-Tags.py (i.e. database tags to be used)
#              gaudi_extra_options_NN_II.py (ie. job specific: random seed,
#                               output file names, see Gauss-Job.py as example)
#
from Configurables import Gauss
from GaudiKernel import SystemOfUnits

#--Set the L/nbb, total cross section and revolution frequency and configure
#  the pileup tool, a CrossingRate of 11.245 kilohertz is used internally
Gauss().Luminosity        = 0.247*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
Gauss().TotalCrossSection = 91.1*SystemOfUnits.millibarn

#--Fix the number of interaction to always 1
from Configurables import Generation
gaussGen = Generation("Generation")
gaussGen.PileUpTool = "FixedNInteractions"

#--Set the average position of the IP
Gauss().InteractionPosition = [  0.0*SystemOfUnits.mm ,
                                 0.0*SystemOfUnits.mm ,
                                 0.5*SystemOfUnits.mm ]

#--Set the bunch RMS, this will be used for calculating the sigmaZ of the
#  Interaction Region. SigmaX and SigmaY are calculated from the beta* and
#  emittance
Gauss().BunchRMS = 82.03*SystemOfUnits.mm

#--Set the energy of the beam,
Gauss().BeamMomentum      = 3.5*SystemOfUnits.TeV

#--the half effective crossing angle (in LHCb coordinate system), horizontal
#  and vertical. And tilts of the beam line
Gauss().BeamHCrossingAngle = -0.520*SystemOfUnits.mrad
Gauss().BeamVCrossingAngle = 0.0
Gauss().BeamLineAngles     = [ -0.075*SystemOfUnits.mrad,
                                0.035*SystemOfUnits.mrad ]

#--beta* and emittance (beta* is nomimally 3m and e_norm 2.5um,
#                       adjusted to match sigmaX and sigmaY)
Gauss().BeamEmittance     = 0.0037*SystemOfUnits.mm
Gauss().BeamBetaStar      = 3.1*SystemOfUnits.m


