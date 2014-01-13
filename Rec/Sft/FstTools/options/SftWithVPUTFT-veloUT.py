# HLT1 emulation for upgraded LHCb with VP UT and FT
import sys

from GaudiKernel.ProcessJobOptions import importOptions
from Configurables import DDDBConf, CondDB
from Configurables import LHCbApp
from Configurables import Brunel, InputCopyStream
from Configurables import GaudiSequencer, RecMoniConf, TrackSys, PrDebugTrackingLosses, FastVeloTracking
from Configurables import PrForwardTracking, PrForwardTool, PrForwardFromPointTool, ToolSvc, PrFTHitManager
from Configurables import EventSelector
from GaudiConf import IOHelper
from FstTools.Configuration import FstConf
from Gaudi.Configuration import *

import os
import inspect
# Directory of this file, needed for importOptions()
mypath = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))


importOptions("$APPCONFIGOPTS/Brunel/MC-WithTruth.py")
importOptions(os.path.join(mypath, 'Sft.py'))


Brunel().DatasetName = "veloUT-testing"
#EventSelector().Input = ["/afs/cern.ch/work/p/pjalocha/public/VP_Sim/VP_MicroChannel_UT_FT_nu6.8.digi"]
importOptions("MinBias.py") # taken directly from Espen


# Output DST
#output_fname = "/tmp/thead/VPUTFTmomentum-estimate-%(channel)s-%(cooling)s-%(mu)s.dst"%(sample)
#InputCopyStream("DstWriter2").Output = "DATAFILE='PFN:%s'"%(output_fname)


# Configuration of the trigger emulation
FstConf().VeloType = "VP"
FstConf().TStationType = "FT+VeloUT" #"FT"
# XXX Need to figure this cut out
FstConf().TStationHits = 10

# these cuts are being varied for Vava's timing studies
# IP and Pt
FstConf().ForwardMinPt = 500
FstConf().MinPt = FstConf().ForwardMinPt / 0.73
FstConf().MinP = 3000
FstConf().MinIP = 0.#float(os.getenv("TIM_MINIP"))

Brunel().EvtMax = 1000#10000#*1000#3000

Brunel().DataType   = "Upgrade"
Brunel().WithMC     = True
Brunel().Simulation = True
Brunel().InputType  = "XDST"
Brunel().OutputType = "None"
Brunel().Histograms = "None"

CondDB().Upgrade = True

LHCbApp().DDDBtag   = "dddb-20131025"
LHCbApp().CondDBtag = "sim-20130830-vc-md100"
CondDB().AllLocalTagsByDataType = ["VP_UVP+RICH_2019+UT_UUT", "FT_StereoAngle5"]


Brunel().MCLinksSequence = ["Unpack", "Tr"]
Brunel().MCCheckSequence = ["Pat"]
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
