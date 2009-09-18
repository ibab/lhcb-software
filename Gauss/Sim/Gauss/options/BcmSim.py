## // $Id: BcmSim.py,v 1.1 2009-09-18 17:30:01 gcorti Exp $

## /** @file
##  *
##  *  File containing options to generate events with hits in the BCM.
##  *  While BCM after TT are in the default simulation, those upstream
##  *  of the VELO are not, as well as the rest of the geometry, so they
##  *  need to be switched on. 
##  *
##  *  @author M.Lieng (adapted by G.Corti)
##  *  @date 2008-05-06
##  */

from Gaudi.Configuration import *
from Configurables import GiGaInputStream
from Configurables import GetTrackerHitsAlg

## Switch on geometry upstream
importOptions('$GAUSSOPTS/BeforeVeloGeometry.py')
importOptions('$GAUSSOPTS/UpstreamGeometry.py')
    
## Bcm after TT is already in standard geometry

## Switch on geometry after muon station: move to it
##importOptions('$GAUSSOPTS/DownstreamRegion.py')
GiGaInputStream('Geo').StreamItems += [
    "/dd/Structure/LHCb/DownstreamRegion/AfterMuon" ]


# Configure how to retrieve hits from Geant4 and add it to sequence
getBcmHits = GetTrackerHitsAlg("GetBcmHits")
getBcmHits.CollectionName = "BcmSDet/Hits"
getBcmHits.MCHitsLocation = "/Event/MC/Bcm/Hits"
getBcmHits.Detectors      = [
    "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/BcmUp",
    "/dd/Structure/LHCb/MagnetRegion/BcmDown" ]

GaudiSequencer("DetectorsHits").Members += [ getBcmHits ]

# save hits on sim file
gaussTape = OutputStream("GaussTape")
gaussTape.ItemList += [ "/Event/MC/Bcm/Hits#1" ]
gaussTape.OptItemList += [ "/Event/Next/MC/Bcm/Hits#1",
                           "/Event/Prev/MC/Bcm/Hits#1",
                           "/Event/PrevPrev/MC/Bcm/Hits#1" ]

# Monitor
importOptions("$BCMMONISIMROOT/options/BcmMoniSim.opts")
# If in detail uncomment the following lines
#from Configurables import BcmHitChecker
#BcmHitChecker("BcmHitCheckerUp").MonitorInDetail   = True
#BcmHitChecker("BcmHitCheckerDown").MonitorInDetail = True
#ApplicationMgr().ExtSvc += [ "NTupleSvc" ]
#NTupleSvc().Output = ["FILE1 DATAFILE='GaussMonitor.root' TYP='ROOT' OPT='NEW'"]

