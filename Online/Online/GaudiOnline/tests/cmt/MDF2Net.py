import os, sys
opts = """
ApplicationMgr.HistogramPersistency  = "NONE";
ApplicationMgr.EvtMax                = 200000;
Runable.MEPManager                   = "";
EventDataSvc.RootCLID                = 1;
EventDataSvc.EnableFaultHandler      = true;
EventPersistencySvc.CnvServices      = {"LHCb::RawDataCnvSvc/RawDataCnvSvc"};
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/Sender"};
Sender.Compress                      = 0;
Sender.AllowSuspend                  = false;
EventSelector.PrintFreq              = 5000;
#pragma print off
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_0"};
SND_0.DataSink                       = "LHCb_storerecv01_lbhlt00_HLT";
SND_0.Compress                       = 0;
SND_0.AllowSuspend                   = false;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_1"};
SND_1.DataSink                       = "LHCb_storerecv01_lbhlt01_HLT";
SND_1.Compress                       = @SND_0.Compress;
SND_1.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_2"};
SND_2.DataSink                       = "LHCb_storerecv01_lbhlt02_HLT";
SND_2.Compress                       = @SND_0.Compress;
SND_2.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_3"};
SND_3.DataSink                       = "LHCb_storerecv01_lbhlt03_HLT";
SND_3.Compress                       = @SND_0.Compress;
SND_3.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_4"};
SND_4.DataSink                       = "LHCb_storerecv01_lbhlt04_HLT";
SND_4.Compress                       = @SND_0.Compress;
SND_4.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_5"};
SND_5.DataSink                       = "LHCb_storerecv01_lbhlt05_HLT";
SND_5.Compress                       = @SND_0.Compress;
SND_5.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_6"};
SND_6.DataSink                       = "LHCb_storerecv01_lbhlt06_HLT";
SND_6.Compress                       = @SND_0.Compress;
SND_6.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_7"};
SND_7.DataSink                       = "LHCb_storerecv01_lbhlt07_HLT";
SND_7.Compress                       = @SND_0.Compress;
SND_7.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_8"};
SND_8.DataSink                       = "LHCb_storerecv01_lbhlt08_HLT";
SND_8.Compress                       = @SND_0.Compress;
SND_8.AllowSuspend                   = @SND_0.AllowSuspend;
ApplicationMgr.TopAlg               += {"LHCb::DimDataSender/SND_9"};
SND_9.DataSink                       = "LHCb_storerecv01_lbhlt09_HLT";
SND_9.Compress                       = @SND_0.Compress;
SND_9.AllowSuspend                   = @SND_0.AllowSuspend;

#include "data/data.opts"
MessageSvc.OutputLevel = 3;
"""

print opts
def startTask(i,options):
  f = open("/tmp/MDF2Net_%02d.opts"%(i,),"w")
  print >>f, o
  f.close()
  os.environ['UTGID'] = 'PROD%02d'%i
  cmd = os.environ['gaudi_exe']+" -opt=/tmp/MDF2Net_%2d.opts -msgsvc=$MSGSVC -main=$GAUDIONLINEROOT/options/Main.opts &"%i
  os.system(cmd)

host = os.environ['HOSTNAME']
if host=='storerecv01':
  opts = opts.replace(
  for j in xrange(5):
    startTask(j,opts)
if os.environ['HOSTNAME']=='storerecv02':
  for j in xrange(5):
    startTask(j,opts)
