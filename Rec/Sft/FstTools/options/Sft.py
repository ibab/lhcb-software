# Shared options for VP, VL and data
#
from Configurables import GaudiSequencer, RecMoniConf, TrackSys, PrDebugTrackingLosses, FastVeloTracking
from Configurables import PrForwardTracking, PrForwardTool, PrForwardFromPointTool, ToolSvc, PrFTHitManager
from Configurables import InputCopyStream
from Configurables import MessageSvc
from FstTools.Configuration import FstConf

from Configurables import Brunel
import Gaudi.Configuration as GC


Brunel().RecoSequence = ["L0", "HLT", "Writer"]
RecMoniConf().MoniSequence = []
Brunel().OutputType = "NONE"

MessageSvc().Format = '% F%50W%S%7W%R%T %0W%M'
#MessageSvc().setVerbose += ['FTRawBankDecoder']
#MessageSvc().setVerbose += ['HltPVsPV3D', 'FstVPDecoding']
#MessageSvc().setVerbose += ['FastFit', 'DstWriter2', 'STOnlinePosition']
#MessageSvc().setDebug += ['SimplifiedMaterialLocator']
#MessageSvc().setVerbose = ['FstForward']
#MessageSvc().setVerbose += ['ToolSvc.PatTStationHitManager']
#MessageSvc().setDebug = ['FstVeloTracking']
#MessageSvc().setDebug = ['HltPVsPV3D']#.PVOfflineTool']
#MessageSvc().setVerbose = ['SelectFwd1']

#FstConf().TCK = '0x0094003d'
FstConf().TTType = "none"
FstConf().TrackFit = "HltFit"
FstConf().MaxIP = 9999
FstConf().MinPt = 1700. #MeV
# Apply after the track fit
FstConf().MinIPChi2 = 16
FstConf().MaxChi2Ndf = 1.5
FstConf().FastDecoding = False
FstConf().Compare = False

# DST writer which copies the input file
# and adds the TES locations to which the
# trigger emulation writes
writer = InputCopyStream('DstWriter2')
writer.ItemList += ["Fst#999"]
#GaudiSequencer("RecoWriterSeq").Members += [writer]
