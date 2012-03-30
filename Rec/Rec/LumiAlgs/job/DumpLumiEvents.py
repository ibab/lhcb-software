#!/usr/bin/env gaudirun.py
from Gaudi.Configuration import ApplicationMgr, EventSelector, FileCatalog, DEBUG
from GaudiConf import IOHelper
from Configurables import createODIN, HltLumiSummaryDecoder
from Configurables import DumpLumiEvents

FileCatalog().Catalogs = ["xmlcatalog_file:MyCatalog.xml"]

inputs = ['lfn:lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/81430/081430_0000000113.raw']

IOHelper(None, None).setupServices()

dumpLumiEvents = DumpLumiEvents()
dumpLumiEvents.OutputLevel = DEBUG
dumpLumiEvents.OutputFileName = "testDumpLumiEvents.bz2"
dumpLumiEvents.NEventsHint = 100

ApplicationMgr().TopAlg = [createODIN(), HltLumiSummaryDecoder(), dumpLumiEvents]
ApplicationMgr().HistogramPersistency = 'NONE'
ApplicationMgr().EvtMax = 100
IOHelper(Input='MDF').inputFiles(inputs, clear=True)
EventSelector().PrintFreq = 1
