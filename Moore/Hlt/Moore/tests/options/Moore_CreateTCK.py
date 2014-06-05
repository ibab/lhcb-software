from Gaudi.Configuration import *
from Configurables import Moore

## Create a local TCK
Moore().TCKData = '$MOOREROOT/tests/options/TCKData/'
Moore().generateConfig = True
Moore().configLabel = 'Test TCK'

## Extra settings required to make things work
from Configurables import L0MuonAlg
L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"
