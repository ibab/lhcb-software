#include "RTL/rtl.h"
#include "PyRPC.h"
#include <string>
#include <ctime>
using namespace PyRPC;
using namespace std;
typedef Pair P;
typedef Param Q;

static void check_method(const MethodCall& c) {
  size_t idx;
  string s;
  cout << "===================================================================" << endl;
  cout << "-->Call:     " << (s=c.str()) << endl;
  while((idx=s.find("methodCall>"))!=string::npos) 
    s.replace(idx,idx+strlen("methodCall>"),"methodResponse>");
  MethodResponse r(s);
  cout << "-->Response: " << endl << r.str() << endl;
  cout << "-->ASCII:    " << endl;
  r.print();
}

extern "C" int test_pyrpc(int, char**)  {
  /// result = self.createRun(RunNumber=12345,FillNumber=1000,StartLumi=9999,StartDate="2006-12-12 10:12:13",ToCastor="YES")
  MethodCall c1("createRun",Args(Q("RunNumber",12345),
				 Q("FillNumber",1000),
				 Q("PartitionID",103),
				 Q("StartLumi",9999),
				 Q("StartDate","2006-12-12 10:12:13"),
				 Q("BeamEnergy",0.0),
				 Q("ToCastor","YES")));
  /// result = self.addFile(12,"Some_junk_file.dat","Other crap",Q("Type","MDF"),Status='Transferred',Q("ToCastor","YES"),Reconstruct='YES')
  MethodCall c2("addFile",Args(12, "Some_junk_file.dat","Other crap",
			       Q("Type","MDF"),
			       Q("Status","Transferred"),
			       Q("ToCastor","YES"),
			       Q("Reconstruct","YES")));
  /// pars = self.fileParams(File={'RunNumber':RunNumber,'Name':Name},Param={'Name':('LIKE',"'%'")})
  MethodCall c3("fileParams",Args(Q("File",Dict(P("RunNumber",32),
						P("Name","/castor/cern.ch/lhcb/Run.32/raw.mdf"))),
                                  Q("Param",Dict(P("Name",Tuple("LIKE","'%'"))))));
  Tuple tup(1,2.5,"Hello",5.9,"Bla",Tuple('a','b',1,4,5.8));
  cout << "Tuple:" << tup << endl;
  cout << "List: " << List(1,2.5,"Hello",5.9,tup) << endl;
  cout << "Dict: " << Dict(P(1,tup),
			   P(2,'a'),
			   P(99,Tuple(3.6,'a',"hrkfhd",List(1,2,3,4,5,6,7))),
			   P(9,"Hello"))
       << endl;
  check_method(c1);
  check_method(c2);
  check_method(c3);
  check_method(MethodCall("call4",Args('a','b',"C",Tuple(1,2.5,"Hello",5.9,"Bla"))));
  check_method(MethodCall("call5",Args(1,2,3,List(1,2.5,"Hello",5.9,"Bla"))));
  check_method(MethodCall("call6",Args(1,2,3,Dict(P("A",Tuple(1,2.5,"Hello",5.9,"Bla"))))));
  return 1;
}

extern "C" int test_pyrpc2(int, char**)  {
  MethodCall c6("call",Args(1,2,3,Dict(P("A",Tuple(1,2.5,"Hello",5.9,"Bla")))));
  check_method(c6);
  return 1;
}
void help_ping() {
  cout << "usage: test_ping -opt [-opt] " << endl
	    << "      -server=<server-name>       Name of the server of origine." << endl
	    << "      -node=<node-name>           Name of the node to ping." << endl
	    << "      -count=<number>             Number of packtes to send." << endl
	    << "      -length=<number>            Packet size in bytes." << endl;
}

extern "C" int test_pyping(int argc, char** argv)  {
  RTL::CLI cli(argc,argv,help_ping);
  string node="storerecv01", server="storectl01";
  int count=1, length=0, port=8088;
  cli.getopt("port",  1,port);
  cli.getopt("node",  1,node);
  cli.getopt("server",1,server);
  cli.getopt("count", 1,count);
  cli.getopt("length",1,length);
  if ( cli.getopt("debug",1) != 0 ) setDebug(true);
  MethodCall c("ping",Args(node,count,length));
  cout << "--------------------------- " << RTL::nodeName() << "::" << RTL::processName() << endl;
  c.print();
  MethodResponse r(Server(server,port)(c));
  cout << r.str() << endl;
  cout << "-----------------------------------------------------------------------------" << endl;
  r.print();
  if ( r.hasError() )  {
    Error err = r.error();
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Error:" << err.first << " errmsg:" << err.second << endl;
  }
  cout << "-----------------------------------------------------------------------------" << endl;
  cout << " XML-RPC call succeeded without errors." << endl;
  return 1;
}
