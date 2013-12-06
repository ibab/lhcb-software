from xml.dom import minidom
import sys
import OnlineEnvBase

def TaskListfromArch(arch, tasklist):
    xmldoc = minidom.parse(arch)
    itemlist = xmldoc.getElementsByTagName('task')
#    tasklist = []
    for s in itemlist:
        tasklist.append(s.attributes['name'].value)
def OptionsfromTasks(tasklist,level,ofile):
    f = open(ofile,'w')
    f.write("//  Adder Level "+level+"\n")
    if level == "1":
        f.write("""#include "$INFO_OPTIONS"

ApplicationMgr.ExtSvc               += {"MonitorSvc","BusySvc"};

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
BusySvc.BogusMips                    = 0.0;
MonitorSvc.CounterUpdateInterval     = 5;
""")
        histsvc = []
        cntsvc = []
        for s in tasklist:
            if 'NodeAdder' in s:
                continue
            hsvc = s#+"HistAdder"
            f.write("ApplicationMgr.ExtSvc               += {\"AdderSvc/"+hsvc+"HistAdder\"};\n")
            csvc = s#+"CountAdder"
            f.write("ApplicationMgr.ExtSvc               += {\"AdderSvc/"+csvc+"CountAdder\"};\n")
            histsvc.append(hsvc)
            cntsvc.append(csvc)
        f.write("\n")

        for s in histsvc:
            svc = s+"HistAdder"
            f.write(svc+".PartitionName  = @OnlineEnv.PartitionName;\n");
            f.write(svc+".MyName  = \"<part>_<node>_"+s+"\";\n");
            f.write(svc+".TaskPattern = \"<part>_<node>_"+s+"_\";\n");
            f.write(svc+".ServicePattern  = \"MON_<part>_<node>_"+s+"_(.*)/Histos/\";\n");
            f.write(svc+".AdderClass  = \"hists\";\n");
            f.write(svc+".ReceiveTimeout = 3;\n")
            f.write("\n")

        for s in cntsvc:
            svc = s+"CountAdder"
            f.write(svc+".PartitionName  = @OnlineEnv.PartitionName;\n");
            f.write(svc+".MyName  = \"<part>_<node>_"+s+"\";\n");
            f.write(svc+".TaskPattern = \"<part>_<node>_"+s+"_\";\n");
            f.write(svc+".ServicePattern  = \"MON_<part>_<node>_"+s+"_(.*)/Counter/\";\n");
            f.write(svc+".AdderClass  = \"Counter\";\n");
            f.write(svc+".ReceiveTimeout = 2;\n")
            f.write("\n")
    elif level == "2":
        f.write("""#include "$INFO_OPTIONS"

ApplicationMgr.ExtSvc               += {"MonitorSvc","AdderSvc/BusyAdder"};

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
BusySvc.BogusMips                    = 0.0;
MonitorSvc.CounterUpdateInterval     = 5;

BusyAdder.MyName                = "<part>_<node>_Busy";
BusyAdder.PartitionName         = @OnlineEnv.PartitionName;
BusyAdder.TaskPattern           = "<part>_<node>[0-9][0-9]_NodeAdder_0";
BusyAdder.ServicePattern        = "MON_<part>_<node>[0-9][0-9]_NodeAdder_0/Counter/";
BusyAdder.AdderClass            = "Counter";
BusyAdder.InDNS                 = "<node>";
BusyAdder.OutDNS                = "hlt01";
BusyAdder.ReceiveTimeout          = 3;
""")
        histsvc = []
        cntsvc = []
        for s in tasklist:
            if 'SubFarmAdder' in s:
                continue
            hsvc = s#+"HistAdder"
            f.write("ApplicationMgr.ExtSvc               += {\"AdderSvc/"+hsvc+"HistAdder\"};\n")
            csvc = s#+"CountAdder"
            f.write("ApplicationMgr.ExtSvc               += {\"AdderSvc/"+csvc+"CountAdder\"};\n")
            histsvc.append(hsvc)
            cntsvc.append(csvc)
        f.write("\n")

        for s in histsvc:
            svc = s+"HistAdder"
            f.write(svc+".PartitionName  = @OnlineEnv.PartitionName;\n")
            f.write(svc+".MyName  = \"<part>_<node>_"+s+"\";\n");
            f.write(svc+".TaskPattern = \"<part>_<node>[0-9][0-9]_NodeAdder_0\";\n")
            f.write(svc+".ServicePattern  = \"MON_<part>_<node>[0-9][0-9]_"+s+"/Histos/\";\n")
            f.write(svc+".AdderClass  = \"hists\";\n")
            f.write(svc+".ReceiveTimeout = 6;\n")
            f.write(svc+".InDNS = \"<node>\";\n")
            f.write(svc+".OutDNS = \"hlt01\";\n")
            f.write("\n")
            

        for s in cntsvc:
            svc = s+"CountAdder"
            f.write(svc+".PartitionName  = @OnlineEnv.PartitionName;\n")
            f.write(svc+".MyName  = \"<part>_<node>_"+s+"\";\n")
            f.write(svc+".TaskPattern = \"<part>_<node>[0-9][0-9]_NodeAdder_0\";\n")
            f.write(svc+".ServicePattern  = \"MON_<part>_<node>[0-9][0-9]_"+s+"/Counter/\";\n")
            f.write(svc+".ReceiveTimeout = 6;\n")
            f.write(svc+".AdderClass = \"Counter\";\n")
            f.write(svc+".InDNS = \"<node>\";\n")
            f.write(svc+".OutDNS = \"hlt01\";\n")
            f.write("\n")
    elif level == "3":
        f.write("""#include "$INFO_OPTIONS"

ApplicationMgr.ExtSvc               += {"MonitorSvc","AdderSvc/BusyAdder"};

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
BusySvc.BogusMips                    = 0.0;
MonitorSvc.CounterUpdateInterval     = 5;

""")
        histsvc = []
        cntsvc = []
        histsvc.append("Adder")
        cntsvc.append("Busy")
        for s in tasklist:
            hsvc = s#+"HistAdder"
            f.write("ApplicationMgr.ExtSvc               += {\"AdderSvc/"+hsvc+"HistAdder\"};\n")
            csvc = s#+"CountAdder"
            f.write("ApplicationMgr.ExtSvc               += {\"AdderSvc/"+csvc+"CountAdder\"};\n")
            histsvc.append(hsvc)
            cntsvc.append(csvc)
        f.write("\n")

        for s in histsvc:
            svc = s+"HistAdder"
            f.write(svc+".MyName  = \"<part>_<node>_"+s+"_00\";\n");
            f.write(svc+".TaskPattern = \"HLT[a-z][0-9][0-9]_Adder_(.*);\n");
            f.write(svc+".ServicePattern  = \"MON_<part>_hlt[a-z][0-9][0-9]_"+s+"/Histos/\";\n");
            f.write(svc+".AdderClass  = \"hists\";\n");
            f.write(svc+".ReceiveTimeout = 12;\n")
            f.write(svc+".InDNS = \"hlt01\";\n")
            f.write("\n")

        for s in cntsvc:
            svc = s+"CountAdder"
            f.write(svc+".MyName  = \"<part>_<node>_"+s+"_00\";\n");
            f.write(svc+".TaskPattern = \"HLT[a-z][0-9][0-9]_Adder_(.*);\n");
            f.write(svc+".ServicePattern  = \"MON_<part>_hlt[a-z][0-9][0-9]_"+s+"/Counter/\";\n");
            f.write(svc+".AdderClass  = \"Counter\";\n");
            f.write(svc+".ReceiveTimeout = 12;\n")
            f.write(svc+".InDNS = \"hlt01\";\n")
            f.write("\n")

tasklist = []
arch = OnlineEnvBase.HltArchitecture
arch = "/group/online/dataflow/architectures/lbDataflowArch_"+arch+".xml"
level = sys.argv[1]
TaskListfromArch(arch, tasklist)
if len(sys.argv) >= 3:
    ofile = sys.argv[2]
else:
    ofile = "/tmp/AdderOptions.opts"
OptionsfromTasks(tasklist,level,ofile)
