##############################################################################
# File for running Brunel testing the trigger
#
# Syntax is:
#   gaudirun.py SftWithVP.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/MC-WithTruth.py")

from Configurables import Brunel
Brunel().DataType  = "2011"
Brunel().Simulation = True
Brunel().EvtMax = 1000

from Configurables import DDDBConf, CondDB

CondDB().Upgrade = True
CondDB().AllLocalTagsByDataType=["VL+UT","FT"] 
Brunel().DDDBtag   = "dddb-20121018"

from Configurables import GaudiSequencer, RecMoniConf, TrackSys, PrDebugTrackingLosses, FastVeloTracking
from Configurables import PrForwardTracking, PrForwardTool, PrForwardFromPointTool, ToolSvc, PrFTHitManager

Brunel().RecoSequence = [ "HLT" ]
RecMoniConf().MoniSequence = [ ]
Brunel().MCLinksSequence = [ "Unpack", "Tr" ]
Brunel().MCCheckSequence = [ "Pat" ]
Brunel().OutputType = "NONE"

from FstTools.Configuration import *
FstConf().TStationType = "FT"
FstConf().VeloType = "VL"
FstConf().TTType = "none"
FstConf().TrackFit = "none"

from Gaudi.Configuration import *
from GaudiConf import IOHelper

######## VL + UT + FT, b->phi phi, nu=6.8
IOHelper().inputFiles([
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000001_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000004_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000005_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000006_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000007_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000009_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000010_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000011_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000014_2.xdigi?svcClass=default',
   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022621/0000/00022621_00000015_2.xdigi?svcClass=default'
   ])


######## VL + UT + FT, b->phi phi, nu=3.4
#IOHelper().inputFiles([
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000001_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000002_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000006_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000008_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000010_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000017_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000026_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000027_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000034_2.xdigi?svcClass=default',
#   'PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/Upgrade/XDIGI/00022623/0000/00022623_00000040_2.xdigi?svcClass=default'
#   ])


from Gaudi.Configuration import *
def doMyChanges():
   from Configurables import GaudiSequencer, PrTrackAssociator, PrChecker
   GaudiSequencer("CaloBanksHandler").Members = []
   GaudiSequencer("DecodeTriggerSeq").Members = []
   GaudiSequencer("MCLinksTrSeq").Members = [ "PrLHCbID2MCParticle", "PrTrackAssociator"]
   PrTrackAssociator().RootOfContainers = "/Event/Fst/Track"
   GaudiSequencer("CheckPatSeq" ).Members = [ "PrChecker" ]
   PrChecker().VeloTracks = "/Event/Fst/Track/Velo"
   PrChecker().ForwardTracks = "/Event/Fst/Track/Forward"
appendPostConfigAction( doMyChanges )

