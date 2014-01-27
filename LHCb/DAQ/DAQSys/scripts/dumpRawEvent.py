


ApplicationMgr.TopAlg += { "DAQEventTests::ByteStreamTests",
                           "DAQEventTests::DummyRawEventCreator",
                           "RawEventDump" };
ApplicationMgr.EvtMax = 1;
ApplicationMgr.EvtSel = "NONE";
EventDataSvc.ForceLeaves = 1;
RawEventDump.DumpData = true;
