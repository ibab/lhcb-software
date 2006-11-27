if ( -f /tmp/CreateMEP.opts ) then
  rm -f /tmp/CreateMEP.opts
endif
cat >/tmp/CreateMEP.opts <<END-OF-OPTS
ApplicationMgr.DLLs       = { "MDF" };
ApplicationMgr.TopAlg     = { "LHCb::RawEventTestCreator/RawEventGen", "StoreExplorerAlg" };
ApplicationMgr.EvtMax     = 2000;
ApplicationMgr.EvtSel     = "NONE";
StoreExplorerAlg.Load     = 1;
StoreExplorerAlg.PrintFreq= 0.01;
EventDataSvc.ForceLeaves  = 1;
EventPersistencySvc.CnvServices += { "LHCb::RawDataCnvSvc" };

// To create a MEP file:
ApplicationMgr.OutStream  = { "LHCb::MEPWriter/Writer_0", "LHCb::MEPWriter/Writer_1" };
Writer_0.Compress       = 0;
Writer_0.ChecksumType   = 0;
Writer_0.PackingFactor  = 10;
Writer_0.Connection     = "file://mepData_0.dat";
Writer_1.Compress       = 0;
Writer_1.ChecksumType   = 0;
Writer_1.PackingFactor  = 10;
Writer_1.Connection     = "file://mepData_1.dat";

END-OF-OPTS
#
cat /tmp/CreateMEP.opts
$GAUDIKERNELROOT/$CMTCONFIG/Gaudi.exe /tmp/CreateMEP.opts
rm /tmp/CreateMEP.opts
