import os, re

# General configuration
from Configurables import LHCbApp

app = LHCbApp()
app.EvtMax = 10000
app.DataType = '2015'
app.CondDBtag = 'cond-20150601'
app.DDDBtag = 'dddb-20150526'
# app.Simulation = True

from Configurables import CondDB
CondDB().Simulation = False
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = False
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

from Configurables import EventSelector
EventSelector().PrintFreq = 1000

from GaudiConf import IOHelper
# base_dir = '/localdisk/hlt1_KEEP'
base_dir = '/daqarea/lhcb/data/2015/RAW/FULL/LHCb/COLLISION15EM'
runnrs = [156920]
re_file = re.compile(r"(Run_)?(0*%s)_.*\.(mdf|raw)" % ('|'.join((str(r) for r in runnrs))))
files = sorted([os.path.join(base_dir, str(run), f)  for run in runnrs for f in os.listdir(os.path.join(base_dir, str(run))) if re_file.match(f)])
IOHelper("MDF", "MDF").inputFiles(files)

# Output writers
# L0 filter
from Configurables import bankKiller
hlt_banks = [ 'HltDecReports','HltRoutingBits','HltSelReports','HltVertexReports','HltLumiSummary','HltTrackReports' ]
killer = bankKiller( 'RemoveInputHltRawBanks',  BankTypes = hlt_banks )

from Configurables import HltRoutingBitsFilter
rb_filter = HltRoutingBitsFilter( "PhysFilter" )
rb_filter.RequireMask = [0x0, 0x4, 0x0]

# InputCopyStream
filename = 'StripHLT_156920'
from Configurables import LHCb__MDFWriter as MDFWriter
mdf_writer = MDFWriter('MDFWriter', Compress = 0, ChecksumType = 1, GenerateMD5 = True,
                       Connection = 'file://%s.raw' % filename)

# Sequence
from Configurables import GaudiSequencer, ApplicationMgr
writer_seq = GaudiSequencer('WriterSeq')
# writer_seq.Members = [l0du_alg, l0_filter, mdf_writer]
writer_seq.Members = [rb_filter, killer, mdf_writer]
ApplicationMgr().OutStream = [writer_seq]

# Timing table to make sure things work as intended
from Configurables import AuditorSvc, LHCbTimingAuditor
ApplicationMgr().AuditAlgorithms = 1
if 'AuditorSvc' not in ApplicationMgr().ExtSvc:
    ApplicationMgr().ExtSvc.append('AuditorSvc')
AuditorSvc().Auditors.append(LHCbTimingAuditor(Enable = True))
