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
#theApp.UseFileStager = True
theApp.PrintFreq = 1000
theApp.EvtMax = -1
theApp.DatasetName = 'AlignHltD0'

# specify the input to the alignment
from Configurables import TAlignment
from TAlignment.ParticleSelections import defaultHLTD0Selection
TAlignment().ParticleSelections = [ defaultHLTD0Selection() ]
TAlignment().TrackSelections = []

# specify what we actually align for
from TAlignment.AlignmentScenarios import *
configurePromptAlignment()
