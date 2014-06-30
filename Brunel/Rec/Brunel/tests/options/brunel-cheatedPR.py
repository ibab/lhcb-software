from Gaudi.Configuration import *
from Brunel.Configuration import *
from PRConfig import TestFileDB
TestFileDB.test_file_db["Brunel-SIM08-MinBias-Beam6500GeV-md100-nu4.8.xdst"].run()

from Configurables import ApplicationMgr, NTupleSvc, GaudiSequencer
from Configurables import ( IdealTracksCreator, TrackStateInitAlg,
                            TrackBestTrackCreator, PrTrackAssociator,
                            Velo__VeloTrackMonitorNT, VeloClusterPosition,
                            TrackEventFitter)
from TrackFitter.ConfiguredFitters import ConfiguredFit
importOptions("$ASSOCIATORSROOT/options/MCParticleToMCHit.py")

# set the event fitter to use ideal tracks (no outlier rejections)
TrackEventFitter('FitVelo').Fitter.MaxNumberOutliers = 0
TrackEventFitter('TrackRefitter').Fitter.MaxNumberOutliers = 0
TrackEventFitter().TracksInContainer = 'MC/Track/Ideal'

# set the Velo and tracking monitor sequences to reconstruct ideal tracks
GaudiSequencer("MoniVELOSeq").Members = [ 
    TrackStateInitAlg("InitIdealFit",
                      TrackLocation = "MC/Track/Ideal") , 
    ConfiguredFit("IdealFit","MC/Track/Ideal") , 
                   Velo__VeloTrackMonitorNT()  ]
GaudiSequencer("RecoTrSeq").Members += [ 
    IdealTracksCreator('IdealTracksCreator', TrueStatesAtMCHits = True, 
                       TrackTypes = [3]) ]
TrackBestTrackCreator().TracksInContainers = ['MC/Track/Ideal']


# set the Velo tuple monitoring to the ideal track container as input
Velo__VeloTrackMonitorNT().RunWithMC = False
Velo__VeloTrackMonitorNT().TracksInContainer = 'MC/Track/Ideal'
PrTrackAssociator().RootOfContainers = 'MC/Track/Ideal'
VeloClusterPosition().CalculateExactProjAngle = True

# setup a job to dump a VELO tuple with ideal PR tracks, no dst required
from Configurables import Brunel, DigiConf, L0Conf
Brunel().DataType   = "2011"
Brunel().EvtMax     = 10
Brunel().InputType  = 'XDST'
Brunel().PrintFreq  = 100
Brunel().Simulation = True
Brunel().WithMC = True
Brunel().OutputType = "NONE"
Brunel().WriteFSR = False
Brunel().MCLinksSequence = [ "Tr" ]
Brunel().MCCheckSequence = [ "Pat" , "Pr" ]
DigiConf().EnableUnpack = True
L0Conf().EnsureKnownTCK=False

# set tuple output as root
ApplicationMgr().ExtSvc +=  [ NTupleSvc() ]
NTupleSvc().Output = [ "FILE1 DATAFILE='brunel-mc11a-cheatedPR.root' TYP='ROOT' OPT='NEW'" ]

from Configurables import RootHistCnv__PersSvc
RootHistCnv__PersSvc( 'RootHistCnv' ).ForceAlphaIds = True
