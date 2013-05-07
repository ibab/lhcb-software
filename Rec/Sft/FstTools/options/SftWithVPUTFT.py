# HLT1 emulation for upgraded LHCb with VP UT and FT
import sys

from GaudiKernel.ProcessJobOptions import importOptions
from Configurables import DDDBConf, CondDB
from Configurables import Brunel, InputCopyStream
from Configurables import GaudiSequencer, RecMoniConf, TrackSys, PrDebugTrackingLosses, FastVeloTracking
from Configurables import PrForwardTracking, PrForwardTool, PrForwardFromPointTool, ToolSvc, PrFTHitManager
from Configurables import EventSelector
from GaudiConf import IOHelper
from FstTools.Configuration import FstConf
from Gaudi.Configuration import *

importOptions("$APPCONFIGOPTS/Brunel/MC-WithTruth.py")
importOptions('$FSTTOOLSROOT/options/Sft.py')


# Input files, assumed to be digi files
Brunel().InputType = 'DIGI'

import os
setting = os.getenv("TIM")
if setting is None:
    setting = "2"
    
Brunel().DatasetName = setting    
if setting == "1":
    sample = {"mu": '3.8', "cooling": 'poco', "channel": 'Kstmumu'}
elif setting == "2":
    sample = {"mu": '3.8', "cooling": 'micro', "channel": 'Kstmumu'}
elif setting == "3":
    sample = {"mu": '3.8', "cooling": 'poco', "channel": 'minbias'}
elif setting == "4":
    sample = {"mu": '3.8', "cooling": 'micro', "channel": 'minbias'}
elif setting == "5":
    sample = {"mu": '7.6', "cooling": 'poco', "channel": 'Kstmumu'}
elif setting == "6":
    sample = {"mu": '7.6', "cooling": 'micro', "channel": 'Kstmumu'}
elif setting == "7":
    sample = {"mu": '7.6', "cooling": 'poco', "channel": 'minbias'}
elif setting == "8":
    sample = {"mu": '7.6', "cooling": 'micro', "channel": 'minbias'}

# Some privately generated samples, you still need to fill the sample
# dictionary with sensible things for things to work
#EventSelector().Input = ["/afs/cern.ch/work/p/pjalocha/public/VP_Sim/VP_PocoFoam_UT_FT_nu6.8.digi"]
#EventSelector().Input = ["/afs/cern.ch/work/p/pjalocha/public/VP_Sim/VP_MicroChannel_UT_FT_nu6.8.digi"]

#EventSelector().Input = ["/tmp/8k-dsts/VPUTFT-Kstmumu-poco-3.8.dst"]
# XXX How is this XML catalog + options file thing meant to work?
importOptions("$FSTTOOLSROOT/options/VPUTFT-%(channel)s-%(cooling)s-%(mu)s.py"%(sample))

# Output DST
output_fname = "/tmp/thead/VPUTFT-%(channel)s-%(cooling)s-%(mu)s.dst"%(sample)
#output_fname = "VPUTFT-%(channel)s-%(cooling)s-%(mu)s.dst"%(sample)
InputCopyStream("DstWriter2").Output = "DATAFILE='PFN:%s'"%(output_fname)


# Configuration of the trigger emulation
FstConf().VeloType = "VP"
FstConf().TStationType = "FT"
# XXX Need to figure this cut out
FstConf().TStationHits = 10

Brunel().EvtMax = 10000#*1000#3000

CondDB().Upgrade = True
if sample['cooling'] == "poco":
    CondDB().AllLocalTagsByDataType = ["VP_Compact_Pocofoam+UT", "FT"]
elif sample['cooling'] == "micro":
    CondDB().AllLocalTagsByDataType = ["VP_Compact_MicroChannel+UT", "FT"]
    
Brunel().DataType = "Upgrade"
Brunel().Simulation = True
Brunel().CondDBtag = "simcond-20121001-vc-md100"
Brunel().DDDBtag = "dddb-20130408"

Brunel().MCLinksSequence = ["Unpack", "Tr"]
Brunel().MCCheckSequence = ["Pat"]
def setup_truth_matching():
   from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker
   from Configurables import UnpackMCParticle, UnpackMCVertex
   from Configurables import PrDebugTrackingLosses
   GaudiSequencer("CaloBanksHandler").Members = []
   GaudiSequencer("DecodeTriggerSeq").Members = []
   GaudiSequencer("MCLinksTrSeq").Members = ["UnpackMCParticle", "UnpackMCVertex"]
   GaudiSequencer("MCLinksTrSeq").Members += ["PrLHCbID2MCParticle", "PrTrackAssociator"]
   PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"
   GaudiSequencer("CheckPatSeq").Members = ["PrChecker", "PrDebugTrackingLosses"]
   PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
   PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"
appendPostConfigAction(setup_truth_matching)
