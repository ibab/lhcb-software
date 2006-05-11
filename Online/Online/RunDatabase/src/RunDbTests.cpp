#include "RunDatabase.h"
#include <string>
#include <ctime>
using namespace PyRPC;
typedef Pair P;
typedef Param Q;

extern "C" int rundb_test_args(int, char**)  {
  Tuple tup(1,2.5,"Hello",5.9,"Bla",Tuple('a','b',1,4,5.8));
  List list(1,2.5,"Hello",5.9,tup);
  Dict dic(Pair(1,tup),P(2,'a'),P(99,Tuple(3.6,'a',"hrkfhd",List(1,2,3,4,5,6,7))),P(9,"Hello"));
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
  MethodCall c3("fileParams",Args(Q("File", Dict(P("RunNumber",32),P("Name","/castor/cern.ch/lhcb/Run.00000032/signal.mdf"))),
                                  Q("Param",Dict(P("Name",Tuple("LIKE","'%'"))))));

  std::cout << "Tuple:" << tup << std::endl;
  std::cout << "List: " << list << std::endl;
  std::cout << "Dict: " << dic << std::endl;
  std::cout << "Call: " << c1 << std::endl;
  std::cout << "Call: " << c2 << std::endl;
  std::cout << "Call: " << c3 << std::endl;
  return 1;
}

extern "C" int rundb_test_interface(int, char**)  {
  RunDatabase db;
  int run = 123456;
  char buff[128];
  sprintf(buff,"/castor/cern.ch/lhcb/Run.%08d/", run);
  std::string dir = buff;

  db.existsRun(run);
  db.createRun(run,1000, 103, time(0), 1.5e9, 1e9);
  db.modifyRun(run,Args(Param("EndDate",time(0)),Param("EndLumi",1e5)));
  db.addRunParam(run,"OtherParam2","OtherValue 2");
  db.addRunParam(run,"OtherParam3","OtherValue 3");
  db.runParam(run,"OtherParam2");
  db.runParams(run);
  db.runParams(run,Args("Typ",Tuple("LIKE","'Some%'")));
  db.addFile(run,dir+"somefile_0.mdf","BB inclusive",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFileParam(run,PyRPC::Dict("File",dir+"somefile_0.mdf"),"Option",123456789,"INTEGER");
  db.addFile(run,dir+"somefile_0.mif","BB inclusive",Args(Q("Type","MIF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"somefile_1.mdf","J/Psi to Leptons",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"somefile_1.mif","J/Psi to Leptons",Args(Q("Type","MIF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"signal.mdf","Exclusive Signals",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"Junk_0.mdf","2 kHz Junk",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES"),Q("Reconstruct","NO")));
  db.addFile(run,dir+"Junk_1.mdf","2 kHz Junk",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES"),Q("Reconstruct","NO")));
  db.addFile(run,dir+"Junk_2.mdf","2 kHz Junk",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES"),Q("Reconstruct","NO")));
  // add same file again: should give an error
  db.modifyRun(run, Args(Q("EndLumi",0.8e9)));
  db.modifyRun(run, Args(Q("EndDate",PyRPC::Time(time(0)+2000))));
  return 1;
}
