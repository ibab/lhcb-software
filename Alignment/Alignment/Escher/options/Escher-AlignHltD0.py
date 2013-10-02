###############################################################################
# File for running alignment with D0 reconstructed from HLT sel reports
###############################################################################
# Syntax is:
#   gaudiiter.py Escher-AlignHltJpsi.py <someDataFiles>.py
###############################################################################

from Configurables import Escher

# Just instantiate the configurable...
theApp = Escher()
theApp.DataType   = "2012"
theApp.InputType  = "MDF"
theApp.PrintFreq = 1000
theApp.EvtMax = 20000
theApp.DatasetName = 'AlignHltD0'

# COND DB
theApp.DDDBtag = 'dddb-20130503-1'
theApp.CondDBtag = 'cond-20130710'

# specify the input to the alignment
from Configurables import TAlignment
from TAlignment.ParticleSelections import defaultHLTD0Selection
TAlignment().ParticleSelections = [ defaultHLTD0Selection() ]
from TAlignment.TrackSelections import NoPIDTracksFromHlt
TAlignment().TrackSelections = [ NoPIDTracksFromHlt() ]

# specify what we actually align for
from TAlignment.AlignmentScenarios import *
configure2012DataAlignment()
