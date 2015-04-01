###############################################################################
# File for running alignment with J/psi reconstructed from HLT sel reports
###############################################################################
# Syntax is:
#   gaudiiter.py Escher-AlignMUON.py <someDataFiles>.py
###############################################################################
      
from Configurables import Escher

# Just instantiate the configurable...
theApp = Escher()
theApp.DataType   = "2012"
theApp.InputType  = "MDF"
theApp.PrintFreq = 10
theApp.EvtMax = 100
theApp.DatasetName = 'AlignHlt1JpsiMUON'

# COND DB
theApp.DDDBtag = 'dddb-20130111'
theApp.CondDBtag = 'cond-20130114'

# trigger requirements
theApp.HltFilterCode = "HLT_PASS_RE( 'Hlt2ExpressJPsiDecision' )"  

# specify the input to the alignment
from Configurables import TAlignment
#from TAlignment.ParticleSelections import muonTracksFORmuonAlignment # Stefania
#TAlignment().ParticleSelections = [ muonTracksFORmuonAlignment() ] # Stefania

from TAlignment.TrackSelections import BestMuonTracks
TAlignment().TrackSelections = [ BestMuonTracks("OfflineMuonAlignment") ]

# specify what we actually align for
from TAlignment.AlignmentScenarios import *
configureMuonAlignment()  # Stefania
