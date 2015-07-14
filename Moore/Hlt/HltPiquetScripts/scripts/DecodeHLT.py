# General configuration
import os, re
from GaudiConf import IOHelper
from Gaudi.Configuration import *
from Configurables import LHCbApp
app = LHCbApp()
app.EvtMax = -1
app.DataType = '2015'
app.CondDBtag = 'cond-20150625'
app.DDDBtag = 'dddb-20150526'
# app.Simulation = True

from Configurables import CondDB
CondDB().Simulation = False
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = True
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

import sys
try:
    import AllHlt1
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import AllHlt1

import AllHlt1
CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

from Configurables import EventClockSvc
EventClockSvc().EventTimeDecoder = "OdinTimeDecoder"

from Configurables import EventSelector
EventSelector().PrintFreq = 1000

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input = test_file_db['MC2015_MinBias_SPD_lt_420_md_4xKee_L0Filtered']
# input.run(configurable = app)

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input.run(configurable = app)

base_dir = '/daqarea/lhcb/data/2015/RAW/TURBO/LHCb/COLLISION15EM'
# base_dir = '/localdisk/hlt1/data2015/0506NB'
# base_dir = '/net/hlta0405/localdisk/Alignment/BWDivision'
runnrs = [157397]
re_file = re.compile(r"(Run_)?(0*%s)_.*\.(mdf|raw)" % ('|'.join((str(r) for r in runnrs))))
files = sorted([os.path.join(base_dir, str(run), f)  for run in runnrs for f in os.listdir(os.path.join(base_dir, str(run))) if re_file.match(f)])
IOHelper("MDF", "MDF").inputFiles(files)

# IOHelper("MDF", "MDF").inputFiles(['Moore_Hlt2.mdf'])

# Top level sequence
topSeq = GaudiSequencer( "TopSequence" )
from Configurables import createODIN
topSeq.Members = [createODIN()]

# Only non-exclusive lumi
from Configurables import HltRoutingBitsFilter
rbFilter = HltRoutingBitsFilter( "PhysFilter" )
rbFilter.RequireMask = [0x0, 0x4, 0x0]
topSeq.Members += [rbFilter]

from Gaudi.Configuration import importOptions
importOptions("$L0TCK/L0DUConfig.opts")

from DAQSys.Decoders import DecoderDB
for dec in ("HltDecReportsDecoder/Hlt1DecReportsDecoder",
            "HltSelReportsDecoder/Hlt1SelReportsDecoder",
            "HltDecReportsDecoder/Hlt2DecReportsDecoder",
            "HltSelReportsDecoder/Hlt2SelReportsDecoder",
            "HltLumiSummaryDecoder",
            "HltVertexReportsDecoder/Hlt1VertexReportsDecoder",
            "HltVertexReportsDecoder/Hlt2VertexReportsDecoder",
            "L0DUFromRawAlg/L0DUFromRaw"):
    topSeq.Members.append(DecoderDB[dec].setup())

from Configurables import TCKANNSvc
TCKANNSvc().AdditionalIDs = {"Hlt2SelectionID" : {"Hlt2Global" : 2}}
# "ProtoPV3D" : 10117}}

ApplicationMgr().TopAlg = [topSeq]

# Timing table to make sure things work as intended
from Configurables import AuditorSvc, LHCbTimingAuditor
ApplicationMgr().AuditAlgorithms = 1
if 'AuditorSvc' not in ApplicationMgr().ExtSvc:
    ApplicationMgr().ExtSvc.append('AuditorSvc')
AuditorSvc().Auditors.append(LHCbTimingAuditor(Enable = True))

from Configurables import LoKiSvc
LoKiSvc().Welcome = False
ApplicationMgr().ExtSvc = ['LoKiSvc']

# ApplicationMgr().ExtSvc += ['HltMonitorSvc']
# from Configurables import HltMonitorSvc
# HltMonitorSvc().OutputConnection = "ipc:///tmp/hlt2mon/1"
# HltMonitorSvc().HltDecReportsLocation = 'Hlt1/DecReports'

## Algo for checking beam spot
from LoKiAlgo.decorators import Algo
from GaudiPython import SUCCESS

class BeamSpotAlgo(Algo):
    """
    Algorithm to get the beamspot position
    """
    ## initialize the algorithm
    def initialize ( self ) :
        """
        Initialize the algorithm
        """

        ## initialize the base
        sc = Algo.initialize ( self ) ## initialize the base
        if sc.isFailure() : return sc
        from LoKiPhys.functions import BEAMSPOT
        from GaudiKernel.SystemOfUnits import mm
        self._beamspot = BEAMSPOT( 1 * mm )

        return SUCCESS

    ## The standard method for analysis
    def analyse ( self ) :
        self.setFilterPassed ( True )
        return SUCCESS

    def x(self):
        return self._beamspot.x()

    def y(self):
        return self._beamspot.y()

    def beamspot(self):
        return self._beamspot

    def finalize( self ):
        del self._beamspot
        return Algo.finalize(self)

from GaudiPython import AppMgr
gaudi = AppMgr()
bs_algo = BeamSpotAlgo("BeamspotAlgo")

# gaudi.addAlgorithm(bs_algo)

gaudi.initialize()

TES = gaudi.evtSvc()

from collections import defaultdict
events = defaultdict(int)

# n = 0
# while True:
#     r = gaudi.run(1)
#     if not TES['/Event']:
#         break

#     selReports = TES['Hlt2/SelReports']
#     odin = TES['DAQ/ODIN']
#     events[(odin.runNumber(), odin.eventNumber())] += 1

#     # print odin.runNumber(), odin.eventNumber()
#     # print selReports
#     # print '\n\n\n'
#     n += 1
from GaudiPython.Bindings import gbl
dummy = gbl.IConfigTreeEditor

while True:
    r = gaudi.run(1)
    if not TES['/Event']:
        print 'done'
    rawEvent = TES['DAQ/RawEvent']
    rbs = set([rb for rb in gbl.Hlt.firedRoutingBits(rawEvent)])
    if 34 not in rbs:
        continue
    reports = TES['Hlt2/SelReports']
    if reports:
        break
