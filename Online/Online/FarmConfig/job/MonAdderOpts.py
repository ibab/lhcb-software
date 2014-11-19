import sys
import OnlineEnvBase
from xml.dom import minidom

def TaskListfromFile(arch, tasklist):
  f = open(arch,"r")
  t = f.readlines()
  f.close()
  for i in range(len(t)):
    t[i] = t[i].replace('\n','')
    tasklist.append(t[i])
def TaskListfromArch(arch, tasklist):
    xmldoc = minidom.parse(arch)
    itemlist = xmldoc.getElementsByTagName('task')
#    tasklist = []
    for s in itemlist:
        tasklist.append(s.attributes['name'].value)
def openFile(ofile):
    f = open(ofile,'w')
    return f
def writePreamble(f):
    f.write("""#include "$INFO_OPTIONS"
ApplicationMgr.EventLoop             = "LHCb::OnlineRunable/EmptyEventLoop";
ApplicationMgr.Runable               = "LHCb::OnlineRunable/Runable";
ApplicationMgr.HistogramPersistency  = "NONE";
ApplicationMgr.EvtSel                = "NONE";

Runable.Wait                         = 3;  // 1 of running as daemon (Class1 task)

MessageSvc.fifoPath                  = "$LOGFIFO";
MessageSvc.OutputLevel               = @OnlineEnv.OutputLevel;
MonitorSvc.OutputLevel               = @OnlineEnv.OutputLevel;
HistogramPersistencySvc.Warnings     = false;
""")
def OptionsfromTasks(tasklist,f):
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

def CalibOptionsfromTasks(tasklist,f):
    histsvc = []
    cntsvc = []
    for s in tasklist:
      if 'NodeAdder' in s:
        continue
      f.write("ApplicationMgr.ExtSvc               += {\"EvtsPub/"+s+"\"};\n")
      f.write(s+".myName = \""+s+"\";\n")
      f.write(s+".PartitionName = @OnlineEnv.PartitionName;\n")
      f.write(s+".TaskPattern = \"CalibHistos_0\";\n")
      f.write(s+".ServicePattern = \"MON_(.*)"+s+"/Counter/\";\n")
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
f=openFile(ofile)
writePreamble(f)
OptionsfromTasks(tasklist,f)
tasklist = []
arch = "/group/online/dataflow/architectures/lbDataflowArch_Calib.xml"
TaskListfromArch(arch,tasklist)
CalibOptionsfromTasks(tasklist,f)
