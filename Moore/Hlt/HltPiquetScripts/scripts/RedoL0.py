import os
from GaudiConf import IOHelper
from Gaudi.Configuration import *
from Configurables import L0App

# General configuration
app = L0App()
app.ReplaceL0Banks = True
app.outputFile = ""
app.EvtMax = 50000
app.TCK = '0x024E'
app.DataType = '2015'

app.CondDBtag = 'cond-20150828'
app.DDDBtag = 'dddb-20150724'
# app.Simulation = True

from Configurables import CondDB
CondDB().Simulation = False
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = False
CondDB().Tags["ONLINE"] = 'fake'
CondDB().Online = True

from Configurables import EventSelector
EventSelector().PrintFreq = 1000

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input = test_file_db['MC2015_MinBias_SPD_lt_420_md_4xKee_L0Filtered']
# input.run(configurable = app)

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input.run(configurable = app)

base_dir = '/daqarea/lhcb/data/2015/RAW/TURBO/LHCb/COLLISION15/164699'
runnrs = [164699]
files = [os.path.join(base_dir, f) for f in os.listdir(base_dir) if any((str(r) in f for r in runnrs))]
IOHelper("MDF", "MDF").inputFiles(files[10:20])

# Output writers
# L0 filter
from Configurables import L0DUFromRawAlg
l0du_alg = L0DUFromRawAlg()
from Configurables import LoKiSvc
LoKiSvc().Welcome = False
from Configurables import LoKi__L0Filter
filter_code = "|".join(["L0_CHANNEL('%s')" % chan for chan in ['Electron','Photon','Hadron','Muon','DiMuon']])
l0_filter = LoKi__L0Filter('L0Filter', Code = filter_code)

from Configurables import bankKiller
hlt_banks = [ 'HltDecReports', 'HltRoutingBits', 'HltSelReports', 'HltVertexReports', 'HltLumiSummary', 'HltTrackReports' ]
killer = bankKiller( 'RemoveInputHltRawBanks',  BankTypes = hlt_banks )

from Configurables import HltRoutingBitsFilter
rb_filter = HltRoutingBitsFilter( "RoutingBitsFilter" )
rb_filter.RequireMask = [0x0, 0x4, 0x0]
from Gaudi.Configuration import ApplicationMgr
ApplicationMgr().TopAlg = [rb_filter, killer]

# InputCopyStream
filename = 'RedoL0_' + app.TCK
from Configurables import LHCb__MDFWriter as MDFWriter
mdf_writer = MDFWriter('MDFWriter', Compress = 0, ChecksumType = 1, GenerateMD5 = True,
                       Connection = 'file://%s.mdf' % filename)

# Sequence
from Configurables import GaudiSequencer
writer_seq = GaudiSequencer('WriterSeq')
# writer_seq.Members = [l0du_alg, l0_filter, mdf_writer]
writer_seq.Members = [mdf_writer]
ApplicationMgr().OutStream = [writer_seq]

# Timing table to make sure things work as intended
from Configurables import AuditorSvc, LHCbTimingAuditor
ApplicationMgr().AuditAlgorithms = 1
if 'AuditorSvc' not in ApplicationMgr().ExtSvc:
    ApplicationMgr().ExtSvc.append('AuditorSvc')
AuditorSvc().Auditors.append(LHCbTimingAuditor(Enable = True))
