###############################################################################
# File for running alignment using the default reconstruction sequence.
###############################################################################
# Syntax is:
#   gaudiiter.py -e NUMEVENTS Escher-AlignCollisions.py <someDataFiles>.py
#
###############################################################################

from Configurables import Escher
theApp = Escher()
theApp.DataType   = "2011"
theApp.InputType  = "MDF"
theApp.PrintFreq = 1
theApp.EvtMax = 100
theApp.DatasetName = 'AlignCollisions'

# specify the input to the alignment
from Configurables import TAlignment
from TAlignment.TrackSelections import *
TAlignment().TrackSelections = [ GoodLongTracks(),
                                 VeloOverlapTracks(),
                                 VeloBackwardTracks(),
                                 ITBoxOverlapTracks() ]

# add the default PV selection
from TAlignment.VertexSelections import configuredPVSelection
TAlignment().PVSelection = configuredPVSelection()

# specify what we actually align for
from TAlignment.AlignmentScenarios import *
configurePromptAlignment()
#configureVeloSensorAlignment()
