##############################################################################
# File for running Brunel on 2008 cosmic data with default 2008 geometry
# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
##############################################################################

from Brunel.Configuration import *

Brunel().EvtMax       = 20
Brunel().PrintFreq    = 10

Brunel().outputType   = "NONE"

# Set the special data options
Brunel().specialData  = ["fieldOff","cosmics"]

#--------------------------------------------------------------------------------
# Tweak the tracking options for cosmics
#--------------------------------------------------------------------------------
Brunel().expertTracking += [ "noDrifttimes" ]
TrackSys().trackPatRecAlgorithms = [ "PatSeed" ]

# Customise PatSeeding cuts
#from Configurables import ( PatSeeding, PatSeedingTool )
#patSeedAlg  = PatSeeding("PatSeeding")
#patSeedTool = PatSeedingTool("PatSeedingTool")
#patSeedTool.MaxITOccupancy = 0.03
#patSeedTool.MaxOTOccupancy = 0.05
#patSeedAlg.addTool(patSeedTool)

#--------------------------------------------------------------------------------
# RICH options for cosmics
# NB : Ultimately (before release) will (largely) move to RichRecSys
#--------------------------------------------------------------------------------
# Disable pixel cleaning when creating RichRecPixels
RichPixelCreatorConfig().pixelCleaning = "None"

RichTools().photonPredictorType = "SelectAll"
RichTools().photonCreator().MaxAllowedCherenkovTheta = [ 99999, 99999, 99999 ]
RichTools().photonCreator().MinAllowedCherenkovTheta = [ 0,     0,     0     ]
RichTools().photonCreator().NSigma                   = [ 99999, 99999, 99999 ]
RichTools().photonCreator().MinPhotonProbability     = [ 0,     0,     0     ]

# Attempt to add calo momentum info to field off tracks for the RICH
RichRecSysConf().useCaloMomentumTracks = True

#MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"

##############################################################################
Brunel().applyConf()
##############################################################################
# I/O datasets are defined in a separate file, see examples 2008-Cosmic-Data.py
##############################################################################

