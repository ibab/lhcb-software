# A minimal configuration for running HLT timing measurements.
# Based on the work of Tim Head.
#
# Author: Kevin Dungs
# Date:   2014-01-19

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
    InputCopyStream,
    PatPixelTracking
)
from FstTools.Configuration import FstConf
from GaudiConf import IOHelper
from GaudiKernel.ProcessJobOptions import importOptions
from Gaudi.Configuration import *

# load options
importOptions('$APPCONFIGOPTS/Brunel/MC-WithTruth.py')
importOptions('$FSTTOOLSROOT/options/Sft.py')

# set input, no with spillover
Brunel().InputType = 'DST'
#input_path = '/group/online/data_hlt'
input_path = '/group/online/data_hlt/minbias_25ns_spillover'
input_files = glob(path.join(input_path, '*.xdst'))
IOHelper().inputFiles(input_files)

# configure trigger emulation
FstConf().VeloType = 'VP'
FstConf().TStationType = 'FT+VeloUT'
#FstConf().TStationType = 'FT'
FstConf().ForwardMinPt = 500  # MeV, pT cut in FT
FstConf().TrackFit = ''

# use the newly introduced DoNothing property to turn off algorithms
algos = [
    FstSelectForwardTracksPartOne('SelectFwd1'),
    FstSelectForwardTracksPartTwo('SelectFwd2'),
    #FstSelectGEC(),
    FstSelectVeloTracks()
]
for algo in algos:
    algo.DoNothing = True

# set multiplicity cuts
FstSelectGEC().MultiplicityCutECAL = 10000
FstSelectGEC().MultiplicityCutHCAL = 10000

# set the mcut via env var for a study
from os import getenv
mcut = getenv('MCUT')
if mcut:
    FstSelectGEC().MultiplicityCutHCAL = int(mcut) * 100 - 1

# detailed output for tracking algos
# if done run over 2 events
#tr_algos = [
#    ...('FstPixel'),
#    ...('PrVeloUT'),
#    ...('FstForward')
#]
#for tr_algo in tr_algos:
#    tr_algo.OutputLevel = 2

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
Brunel().EvtMax = 3000
Brunel().DataType = 'Upgrade'
Brunel().Simulation = True
Brunel().CondDBtag = 'sim-20130830-vc-md100'
Brunel().DDDBtag = "dddb-20131025"
# newer DB tags, here for future reference
#Brunel().CondDBtag = 'sim-20131108-vc-md100'
#Brunel().DDDBtag = "dddb-20131108"
Brunel().MCLinksSequence = ['Unpack', 'Tr']
Brunel().MCCheckSequence = ['Pat']


def setup_truth_matching():
    from Configurables import (
        GaudiSequencer,
        PrChecker,
        PrTrackAssociator,
    )
    GaudiSequencer('CaloBanksHandler').Members = []
    GaudiSequencer('DecodeTriggerSeq').Members = []
    GaudiSequencer('MCLinksTrSeq').Members = [
        "UnpackMCParticle", "UnpackMCVertex",
        'PrLHCbID2MCParticle',
        'PrTrackAssociator'
    ]
    PrTrackAssociator().RootOfContainers = '/Event/Fst/Track'
    
    GaudiSequencer("CheckPatSeq").Members = ['PrChecker']
    PrChecker().VeloTracks = '/Event/Fst/Track/Velo'
    PrChecker().UpTracks = '/Event/Fst/Track/VeloUTFst'
    PrChecker().ForwardTracks = '/Event/Fst/Track/Forward'
    PrChecker().Eta25Cut = True
   
appendPostConfigAction(setup_truth_matching)
