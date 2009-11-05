## /** @file
##  *
##  *  File containing options to simulate geometry upstream of VELO and
##  *  produce hits in Scintillator planes just at the wall.
##  *
##  *  Works ONLY with MC09 geometry:
##  *  DDDB="MC09-20090602",SIMCOND="sim-20090402-vc-md100"
##  *  
##  *  @author G.Corti
##  *  @date 2008-05-06
##  */

from Gaudi.Configuration import *

from Configurables import GetTrackerHitsAlg

## Switch on geometry upstream
importOptions('$GAUSSOPTS/BeforeVeloGeometry.py')

## Retrieve hits from Scintillator planes
getScintillatorHits.CollectionName = "VPlaneSDet/Hits"
getScintillatorHits.MCHitsLocation = "/Event/MC/Scintillators/Hits"
getScintillatorHits.Detectors = [
  "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/VPlane1", 
  "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/VPlane2" 
]
getScintillatorHits.ExtendedInfo = True
GaudiSequencer("DetectorsHits").Members += [ getScintillatorHits ]

## and save them on sim file
OutputStream("GaussTape").ItemList += [ "/Event/MC/Scintillators/Hits#1" ]
