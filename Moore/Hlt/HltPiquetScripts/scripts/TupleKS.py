import os
# General configuration
from Gaudi.Configuration import *
from Configurables import NTupleSvc, ToolSvc, ApplicationMgr
from Configurables import LHCbApp
app = LHCbApp()
app.EvtMax = -1

from Configurables import EventSelector
EventSelector().PrintFreq = 1000

base_dir = "/daqarea/lhcb/data/2015/RAW/FULL/LHCb/SMOG15"
files = []
for entry in os.walk(base_dir):
    if entry[0].split('/')[-1] not in ['152177', '152178']:
        continue
    files += [os.path.join(entry[0], f) for f in entry[2]]
from GaudiConf import IOHelper

IOHelper("MDF", "MDF").inputFiles(sorted(files))

# CondDB
from Configurables import CondDB
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = False
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

# Sequence
from Configurables import GaudiSequencer
from Gaudi.Configuration import ApplicationMgr

importOptions("$L0TCK/L0DUConfig.opts")
dec_seq = GaudiSequencer('DecoderSeq')
from DAQSys.Decoders import DecoderDB
for n in ["createODIN", "L0DUFromRawAlg/L0DUFromRaw",
          "HltDecReportsDecoder/Hlt1DecReportsDecoder",
          "HltSelReportsDecoder/Hlt1SelReportsDecoder",
          "HltDecReportsDecoder/Hlt2DecReportsDecoder",
          "HltSelReportsDecoder/Hlt2SelReportsDecoder"]:
    dec_seq.Members.append(DecoderDB[n].setup())

ApplicationMgr().TopAlg = [dec_seq]
ApplicationMgr().HistogramPersistency = 'ROOT'

# Timing table to make sure things work as intended
from Configurables import AuditorSvc, LHCbTimingAuditor
ApplicationMgr().AuditAlgorithms = 1
if 'AuditorSvc' not in ApplicationMgr().ExtSvc:
    ApplicationMgr().ExtSvc.append('AuditorSvc')
AuditorSvc().Auditors.append(LHCbTimingAuditor(Enable = True))

tupleSvc = NTupleSvc(Output = ["LUN1 DATAFILE='TupleKS.root' OPT='NEW'"])
import GaudiPython.TupleUtils as TupleUtils

from GaudiPython.Bindings import AppMgr
gaudi = AppMgr()
gaudi.initialize()

decisions = {'DD' : 'Hlt2KS0_DDTurboDecision', 'LL' : 'Hlt2KS0_LLTurboDecision'}

tuples = {}
for t in decisions.iterkeys():
    tuples[t] = TupleUtils.nTuple("TupleKS_" + t         , ## the path
                                  "KS_%s Turbo Tuple" % t, ## the title
                                  LUN = 'LUN1')            ## logical unit

TES = gaudi.evtSvc()

n = 0

while True:
    gaudi.run(1)
    n += 1

    if not TES['/Event']:
        break
    odin = TES['DAQ/ODIN']

    decReports = TES['Hlt2/DecReports']
    if not decReports:
        continue
    selReports = TES['Hlt2/SelReports']
    for t, dec in decisions.iteritems():
        if not decReports.decReport(dec).decision():
            continue
        selReport = selReports.selReport(dec)
        candidates = selReport.substructure()
        candidate = candidates[0].data()
        tuples[t].column(t + '_Mass', candidate.numericalInfo()['1#Particle.measuredMass'])
        tuples[t].column('run', odin.runNumber())
        tuples[t].column('event', odin.eventNumber())
        tuples[t].write()

TupleUtils.releaseTuples()
