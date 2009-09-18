##  File to include BLS geometry, activate hits collecting and monitoring
##
##  @author V.Talanov
##  @date 2009-08-13
##
from Gauss.Configuration import *

from Gaudi.Configuration import *
from Configurables import GiGaInputStream

#-- Use the appropriate local tag as the BLS are not in an MC09 global tag
#
from Configurables import CondDB
CondDB().LocalTags = { "DDDB":["bls-20090826"] }

#-- Switch on the LHCb upstream geometry
#
importOptions('$GAUSSOPTS/BeforeVeloGeometry.py')
    
#-- Activate hits collecting from Beam Loss Scintillators
#
GetTrackerHitsAlg( "GetBlsHits" ).CollectionName = "BlsSDet/Hits"
GetTrackerHitsAlg( "GetBlsHits" ).MCHitsLocation = "/Event/MC/Bls/Hits"
GetTrackerHitsAlg( "GetBlsHits" ).Detectors  = [ "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/Bls1" ]
GetTrackerHitsAlg( "GetBlsHits" ).Detectors += [ "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/Bls2" ]
GaudiSequencer( "DetectorsHits" ).Members += [ "GetTrackerHitsAlg/GetBlsHits" ]

#-- Add hits to tape (GaussTape comes from Gauss-Job.py...)
#
myGaussTape = OutputStream( "GaussTape" )
myGaussTape.ItemList += [ "/Event/MC/Bls/Hits#1" ]

# BlsSim.py
