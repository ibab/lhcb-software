import sys
import OnlineEnvBase

def TaskListfromFile(arch, tasklist):
  f = open(arch,"r")
  t = f.readlines()
  f.close()
  for i in range(len(t)):
    t[i] = t[i].replace('\n','')
    tasklist.append(t[i])
def OptionsfromTasks(tasklist,ofile):
    f = open(ofile,'w')
    f.write("""#include "$INFO_OPTIONS"


ApplicationMgr.EventLoop             = "LHCb::OnlineRunable/EmptyEventLoop";
ApplicationMgr.Runable               = "LHCb::OnlineRunable/Runable";
ApplicationMgr.HistogramPersistency  = "NONE";
ApplicationMgr.EvtSel                = "NONE";

Runable.Wait                         = 3;  // 1 of running as daemon (Class1 task)

MessageSvc.fifoPath                  = "$LOGFIFO";
MessageSvc.LoggerOnly                = true;  // Set to false for stdout
MessageSvc.OutputLevel               = @OnlineEnv.OutputLevel;
MonitorSvc.OutputLevel               = @OnlineEnv.OutputLevel;
HistogramPersistencySvc.Warnings     = false;
""")
    histsvc = []
    cntsvc = []
    for s in tasklist:
      if 'NodeAdder' in s:
        continue
      f.write("ApplicationMgr.ExtSvc               += {\"EvtsPub/"+s+"\"};\n")
      f.write(s+".myName = \""+s+"\";\n")
      f.write(s+".PartitionName = @OnlineEnv.PartitionName;\n")
      f.write(s+".TaskPattern = \"" + s + "(.*)\";\n")
      f.write(s+".ServicePattern = \"MON_"+s+"/Counter/\";\n")
      f.write(s+".AdderClass = \"Counter\";\n")
      f.write(s+".InDNS = \"mona08\";\n")
      f.write(s+".OutDNS = \"ecs03\";\n")
      f.write("\n")
    f.write("\n");


tasklist = []
part = OnlineEnvBase.PartitionName
arch ="/group/online/dataflow/options/"+part+"/MonitoringTasks.txt"
TaskListfromFile(arch, tasklist)
print tasklist
if len(sys.argv) >= 2:
    ofile = sys.argv[1]
else:
    ofile = "/tmp/AdderOptions.opts"
OptionsfromTasks(tasklist,ofile)
