from Configurables import LHCbApp
from Gaudi.Configuration import *

from Configurables import IODataManager
IODataManager().DisablePFNWarning = True

app = LHCbApp(DataType = '2015')
app.EvtMax = 100

from Configurables import HltOutputSvc, HltOutputSequence
outputSvc = HltOutputSvc()
ApplicationMgr().ExtSvc += [outputSvc]

from Configurables import GaudiSequencer as Sequence
from Configurables import LHCb__RawEventCopy as RawEventCopy
from Configurables import DeterministicPrescaler

from PRConfig.TestFileDB import test_file_db
test_file_db['2015HLTValidationData_L0filtered_0x0050'].run(configurable = app)

from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool, AuditorSvc
from Configurables import SequencerTimerTool
SequencerTimerTool().OutputLevel = 4
ApplicationMgr().ExtSvc.append( 'AuditorSvc' )
ta = LHCbTimingAuditor('TIMER', Enable = True)
ta.addTool(LHCbSequencerTimerTool, name = 'TIMER')
AuditorSvc().Auditors.append(ta)

sequence = GaudiSequencer("TopSequence", IgnoreFilterPassed = True)

def groupName(subs):
    return sorted(subs)[0]

groups = {("Full", "TurboFull") : 0.5, ("Turbo",) : 0.2}
filters = {}
for substreams, fraction in groups.iteritems():

    gn = sorted(substreams)[0]
    scaler = DeterministicPrescaler(gn + "Scaler", AcceptFraction = fraction)
    for substream in substreams:
        filters[substream] = [scaler.getFullName()]

    rawCopy = RawEventCopy(gn + "RawEventCopy", DeepCopy = True,
                           Destination = '/Event/DAQ/%sRawEvent' % gn)
    subSequence = GaudiSequencer(gn + "Sequence", Members = [scaler, rawCopy])

    sequence.Members += [subSequence]

ApplicationMgr().TopAlg = [sequence]
ApplicationMgr().AuditAlgorithms = True

# Configure HltOutputSvc
outputSvc.StreamGroups = {groupName(subs) : list(subs) for subs in groups.iterkeys()}
outputSvc.Filters = {s : filters[groupName(subs)] for subs in groups.iterkeys() for s in subs}

# Configure HltOutputStream
outStream = HltOutputSequence()
outStream.WriterType = "LHCb::MDFWriter"
outStream.WriterProperties = {"Compress" : "0"}
outStream.GroupProperties = {"BankLocation" : "/Event/DAQ/%sRawEvent"}
outStream.StreamProperties = {"Connection"   : "file://%sOutput.mdf"}
ApplicationMgr().OutStream = [outStream]
