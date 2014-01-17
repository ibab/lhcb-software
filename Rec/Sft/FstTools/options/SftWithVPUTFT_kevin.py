# A minimal configuration for running HLT timing measurements.
#
# Author: Kevin Dungs
# Date:   2014-01-10

from glob import glob
from os import path

from Configurables import (
    Brunel,
    CondDB,
    FstSelectForwardTracksPartOne,
    FstSelectForwardTracksPartTwo,
    FstSelectGEC,
    FstSelectVeloTracks,
    GaudiSequencer,
    InputCopyStream
)
from FstTools.Configuration import FstConf
from GaudiConf import IOHelper
from GaudiKernel.ProcessJobOptions import importOptions
from Gaudi.Configuration import *

# load options
importOptions('$APPCONFIGOPTS/Brunel/MC-WithTruth.py')
importOptions('$FSTTOOLSROOT/options/Sft.py')

# set input
Brunel().InputType = 'DST'
input_path = '/group/online/data_hlt'
input_files = glob(path.join(input_path, '*.xdst'))
IOHelper().inputFiles(input_files)

# configure trigger emulation
FstConf().VeloType = 'VP'
FstConf().TStationType = 'FT+VeloUT'
FstConf().ForwardMinPt = 500  # MeV, pT cut in FT
# use the newly introduced DoNothing property to turn off algorithms
algos = [
    FstSelectForwardTracksPartOne('SelectFwd1'),
    FstSelectForwardTracksPartTwo('SelectFwd2'),
    FstSelectGEC(),
    FstSelectVeloTracks()
]
for algo in algos:
    algo.DoNothing = True

# set up CondDB
CondDB().Upgrade = True
CondDB().AllLocalTagsByDataType = [
    'VP_UVP+RICH_2019+UT_UUT',
    'FT_StereoAngle5',
    'Muon_NoM1',
    'Calo_NoSPDPRS'
]

# set up Brunel
Brunel().PrintFreq = 1000
Brunel().EvtMax = 10000
Brunel().DataType = 'Upgrade'
Brunel().Simulation = True
Brunel().CondDBtag = 'sim-20130830-vc-md100'
Brunel().DDDBtag = "dddb-20131025"
Brunel().MCLinksSequence = ['Unpack', 'Tr']
Brunel().MCCheckSequence = ['Pat']


def setup_truth_matching():
   from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker
   from Configurables import UnpackMCParticle, UnpackMCVertex
   from Configurables import PrDebugTrackingLosses
   from Configurables import PatPixelTracking
   GaudiSequencer("CaloBanksHandler").Members = []
   GaudiSequencer("DecodeTriggerSeq").Members = []
   #GaudiSequencer("MCLinksTrSeq").Members = ["UnpackMCParticle", "UnpackMCVertex"]
   #GaudiSequencer("MCLinksTrSeq").Members += ["PrLHCbID2MCParticle", "PrTrackAssociator"]
   GaudiSequencer("MCLinksTrSeq").Members = ["PrLHCbID2MCParticle", "PrTrackAssociator"]
   PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"
   writer = InputCopyStream('DstWriter2')
   GaudiSequencer("CheckPatSeq").Members = ["PrChecker"]#, writer]
   PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
   PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"

   
appendPostConfigAction(setup_truth_matching)
