###############################################################################
# File for running alignment with J/psi reconstructed from HLT sel reports
###############################################################################
# Syntax is:
#   gaudiiter.py Escher-AlignHltJpsi.py <someDataFiles>.py
###############################################################################

from Configurables import Escher

# Just instantiate the configurable...
theApp = Escher()
theApp.DataType   = "2011"
theApp.InputType  = "MDF"
theApp.PrintFreq = 1000
theApp.EvtMax = -1
theApp.DatasetName = 'AlignHltJpsi'

# specify the input to the alignment
from Configurables import TAlignment
from TAlignment.ParticleSelections import *
TAlignment().ParticleSelections = [ defaultHLTJPsiSelection() ]

from TAlignment.TrackSelections import NoPIDTracksFromHlt
TAlignment().TrackSelections = [ NoPIDTracksFromHlt( RerunVeloTracking = True ) ]

# specify what we actually align for
from TAlignment.AlignmentScenarios import *
configurePromptAlignment()
