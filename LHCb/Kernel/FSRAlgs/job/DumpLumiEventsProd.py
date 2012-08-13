#!/usr/bin/env gaudirun.py
from Gaudi.Configuration import ApplicationMgr, EventSelector
from GaudiConf import IOHelper
from Configurables import createODIN, HltLumiSummaryDecoder
from Configurables import DumpLumiEvents

IOHelper(None, None).setupServices()
#IOHelper(Input='MDF').inputFiles(inputs, clear=True)

dumpLumiEvents = DumpLumiEvents()
dumpLumiEvents.OutputFileName = "output.bz2"

ApplicationMgr().TopAlg = [createODIN(), HltLumiSummaryDecoder(), dumpLumiEvents]
ApplicationMgr().HistogramPersistency = 'NONE'
EventSelector().PrintFreq = 100000