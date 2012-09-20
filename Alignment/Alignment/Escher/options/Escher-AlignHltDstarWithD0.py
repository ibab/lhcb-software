###############################################################################
# File for running alignment with D* reconstructed from HLT sel reports
###############################################################################
# Syntax is:
#   gaudiiter.py Escher-AlignHltJpsi.py <someDataFiles>.py
###############################################################################

from Configurables import Escher

# Just instantiate the configurable...
theApp = Escher()
theApp.DataType   = "2012"
theApp.InputType  = "MDF"
theApp.PrintFreq = 10000
theApp.EvtMax = 200000
theApp.DatasetName = 'AlignHltDstarWithD0'

# COND DB
theApp.DDDBtag = 'dddb-20120831'
theApp.CondDBtag = 'cond-20120831'

# specify the input to the alignment
from Configurables import TAlignment
from TAlignment.ParticleSelections import defaultHLTDstarSelection
TAlignment().ParticleSelections = [ defaultHLTDstarSelection() ]
from TAlignment.TrackSelections import NoPIDTracksFromHlt
TAlignment().TrackSelections = [ NoPIDTracksFromHlt() ]

# specify what we actually align for
from TAlignment.AlignmentScenarios import *
configure2012DataAlignment()
