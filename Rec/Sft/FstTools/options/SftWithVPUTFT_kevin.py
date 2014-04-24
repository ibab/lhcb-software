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
    PrPixelTracking
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

# MinBias events
#input_path = '/group/online/data_hlt/minbias_25ns_spillover_nu7.6'
#input_path = '/group/online/data_hlt/minbias_25ns_spillover_nu11.4'
#input_path = '/calib/hlt/data_hlt/minbias_25ns_spillover_nu7.6'
#input_path = "/scratch/thead/hlt/data_hlt/minbias_25ns_spillover_nu7.6"
input_path = "/scratch/thead/hlt/data_hlt/minbias_25ns_spillover_nu7.6_NoSpillRich/"
input_files = glob(path.join(input_path, '0*.xdst'))
Brunel().DatasetName = "minbias-hlt-7.6"

# Bs->phiphi events
#input_path = '/group/online/data_hlt/bsphiphi_25ns_spillover_nu7.6'
input_path = '/calib/hlt/data_hlt/bsphiphi_25ns_spillover_nu7.6'
#input_path = '/calib/hlt/data_hlt/bsphiphi_25ns_spillover_nu11.4'
#input_path = '/scratch/thead/hlt/data_hlt/bsphiphi_25ns_spillover_nu11.4'
#input_path = "/scratch/thead/hlt/data_hlt/bsphiphi_25ns_spillover_nu3.8"
#input_files = glob(path.join(input_path, '0*.xdst'))
#Brunel().DatasetName = "bsphiphi-offline-7.6"

IOHelper().inputFiles(input_files)

# configure trigger emulation
FstConf().VeloType = 'VP'
FstConf().TStationType = 'FT+VeloUT'
FstConf().ForwardMinPt = 500  # MeV, pT cut in FT
# Offline like configuration
#FstConf().TStationType = 'FT'
#FstConf().ForwardMinPt = 50  # MeV, pT cut in FT

#
# empty string or None means do not run Rich or Fit
# For the fit to work we need the lite clusters from
# the Velo. Currently all that is being reworked.
# So we need to run the cluster decoding by hand
FstConf().TrackFit = 'HltFit'

# default is no Rich reco, turn on by uncommenting
FstConf().RICHType = 'HLT2015'
FstConf().MuonID = False
#FstConf().OutputFile = "/scratch/thead/Conor.xdst"

# Kurt's improvements
PrPixelTracking("FstPixel").RunOnRawBanks = True
PrPixelTracking("FstPixel").Trigger = True

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
FstSelectGEC().MultiplicityCutECAL = 1
FstSelectGEC().MultiplicityCutHCAL = 1199#*10000

# set the mcut via env var for a study
from os import getenv
mcut = getenv('MCUT')
if mcut:
    FstSelectGEC().MultiplicityCutHCAL = int(mcut) * 100 - 1

# Use this together with adding the algo to
# the Fst sequence to write out events after
# applying the GEC
#from Configurables import InputCopyStream
#output_fname = "/group/online/data_hlt/minbias_25ns_spillover/MinBias_spillover_GEC%i.xdst"%(1000)
#InputCopyStream("DstWriter3").Output = "DATAFILE='PFN:%s'"%(output_fname)
#InputCopyStream("DstWriter3").OutputLevel = 2

# set up CondDB
CondDB().Upgrade = True
CondDB().AllLocalTagsByDataType = [
    'VP_UVP+RICH_2019+UT_UUT',
    'FT_StereoAngle5',
    'Muon_NoM1',
    'Calo_NoSPDPRS'
]

# set up Brunel
Brunel().PrintFreq = 1000/100
Brunel().EvtMax = 1000 #100#500 #1000*3
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
        TrackIPResolutionChecker,
        PrPixelTracking,
        DumpTracks,
    )
    from Configurables import Rich__Rec__MC__PIDQC as PIDQC
    from Configurables import Rich__Rec__MC__RecoQC as RichRecoQC
    from Configurables import RichTools
    
    GaudiSequencer('CaloBanksHandler').Members = []
    GaudiSequencer('DecodeTriggerSeq').Members = []
    GaudiSequencer('MCLinksTrSeq').Members = [
        "UnpackMCParticle", "UnpackMCVertex",
        'PrLHCbID2MCParticle',
        'PrTrackAssociator',
    ]
    PrTrackAssociator().RootOfContainers = '/Event/Fst/Track'
    
    GaudiSequencer("CheckPatSeq").Members = ['PrChecker',
                                             TrackIPResolutionChecker(),
                                             #DumpTracks("DumpFwdTracks"),
                                             ]

    DumpTracks("DumpFwdTracks").TracksLocation = "/Event/Fst/Track/ForwardFst"
    DumpTracks("DumpFwdTracks").OutputLevel = 1

    if FstConf().getProp("RICHType"):
        GaudiSequencer("CheckPatSeq").Members += [PIDQC(),
                                                  RichRecoQC(),
                                                  ]
        pidqc = PIDQC()
        pidqc.addTool(RichTools().trackSelector(nickname="TrackSelector",
                                                private=True),
                      name="TrackSelector")
        pidqc.TrackSelector.TrackAlgs = ["Forward"]
        pidqc.InputPIDs = "/Event/Fst/Rich/PIDs"
        pidqc.MCTruth = True
        pidqc.HistoProduce = True
        
        recoqc = RichRecoQC()
        recoqc.addTool(RichTools().trackSelector(nickname="TrackSelector",
                                                 private=True),
                       name="TrackSelector")
        recoqc.TrackSelector.TrackAlgs = ["Forward"]
        recoqc.Context = "HLT"
        recoqc.HistoProduce = True
    
    if FstConf().getProp("OutputFile"):
        # run here so that MC linking info
        # is ready, configured in the FstSequencer though
        GaudiSequencer("CheckPatSeq").Members += [
            InputCopyStream("FstDstWriter"),
            ]
        
    PrChecker().VeloTracks = '/Event/Fst/Track/Velo'
    PrChecker().UpTracks = '/Event/Fst/Track/VeloUTFst'
    PrChecker().ForwardTracks = '/Event/Fst/Track/Forward'
    PrChecker().Eta25Cut = True
    PrChecker().WriteVeloHistos = 2
    PrChecker().WriteForwardHistos = 2
    PrChecker().WriteUpHistos  = 2
    PrChecker().TriggerNumbers = True
    PrChecker().UseElectrons = False

    # For this you should check out the head of
    # Tr/TrackCheckers
    TrackIPResolutionChecker().TrackContainer = '/Event/Fst/Track/Velo'
    TrackIPResolutionChecker().PVContainer = '/Event/Fst/Vertices/PV'

    # Decide which Velo stations to use, last 14, last 16, or !last16
    # Ask Tim about what this does
    # a copy of the code is in ~thead/public/forTom/TurnOffVelo
    active14 = "1111111111111111111111111111000000000000000000000000"
    active16 = "1111111111111111111111111111111100000000000000000000"
    other16 =  "0000000000000000000000000000000011111111111111111111"
    #PrPixelTracking("FstPixel").ActiveModules = active16
    
    # Uncomment to run over events written out
    # after the GEC cut, for some reason
    # reading in DSTs we wrote out ourselves is broken
    # Use this as well if producing DSTs for Conor
    GaudiSequencer("InitReprocSeq").Members = []
   
appendPostConfigAction(setup_truth_matching)


#if __name__ == "__main__":
#    import GaudiPython
#    app_mgr = GaudiPython.AppMgr()
#    app_mgr.run(1)
