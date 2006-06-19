#include "RunDatabase.h"
#include "PyRPC.h"
#include <string>
#include <ctime>
using namespace PyRPC;
using namespace LHCb;
typedef Pair P;
typedef Param Q;

void dumpParams(IRunDatabaseTypes::Params& p)  {
  typedef IRunDatabaseTypes::Params _P;
  std::cout << "Status=" << p.status() << std::endl;
  for(_P::DATA::const_iterator i=p.data.begin(); i!=p.data.end(); ++i)  {
    std::cout << (*i)->ID << "\t" << (*i)->Name << "[" << (*i)->Type << "]\t=\t" << (*i)->Value << std::endl;
  }
}
extern "C" int test_rundb(int argc, char** argv)    {
   RunDatabase rundb("RunDbSrv/RunDb");
   IRunDatabaseReader& db = rundb;
   IRunDatabaseTypes::Params p;
   for(int i=0; i<200;++i)  {
     p = db.runParams(i);
     dumpParams(p);
   }
   return p.status();
}

extern "C" int test_rundb_args(int, char**)  {
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

extern "C" int test_rundb_writer(int, char**)  {
  RunDatabase rundb("RunDbSrv/RunDb");
  IRunDatabaseWriter& db = rundb;
  RunDatabase::Status sc;
  RunDatabase::Param  par;
  RunDatabase::Params pars;
  int run = 123456;
  char buff[128];

  sc = db.existsRun(run);
  RunDatabase::Result<int> runEntry = db.createRun(1000, 103, "Physics", time(0), "Moore", "v3r6");
  run = runEntry.data;
  sprintf(buff,"/castor/cern.ch/lhcb/Run.%08d/", run);
  std::string dir = buff;

  sc = db.modifyRun(run,Args(Param("EndDate",time(0)),Param("IntegratedLumi",1e5)));
  sc = db.addRunParam(run,"OtherParam2","OtherValue 2");
  sc = db.addRunParam(run,"OtherParam3","OtherValue 3");
  par  = db.runParam(run,"OtherParam2");
  pars = db.runParams(run);
  pars = db.runParams(run,Args(Param("Typ",Tuple("LIKE","'Some%'"))));
  sc = db.addFile(run,dir+"somefile_0.mdf","BB inclusive",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES")));
  sc = db.addFileParam(run,Args(Param("FileName",dir+"somefile_0.mdf")),"Option",123456789,"INTEGER");
  db.addFile(run,dir+"somefile_0.mif","BB inclusive",Args(Q("Type","MIF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"somefile_1.mdf","J/Psi to Leptons",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"somefile_1.mif","J/Psi to Leptons",Args(Q("Type","MIF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"signal.mdf","Exclusive Signals",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES")));
  db.addFile(run,dir+"Junk_0.mdf","2 kHz Junk",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES"),Q("Reconstruct","NO")));
  db.addFile(run,dir+"Junk_1.mdf","2 kHz Junk",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES"),Q("Reconstruct","NO")));
  db.addFile(run,dir+"Junk_2.mdf","2 kHz Junk",Args(Q("Type","MDF"),Q("Status","Opened"),Q("ToCastor","YES"),Q("Reconstruct","NO")));
  // add same file again: should give an error
  db.modifyRun(run, Args(Q("IntegratedLumi",0.8e9)));
  db.modifyRun(run, Args(Q("EndDate",time(0)+2000)));
  sc = db.finalizeRun(run,time(0),3e5);
  return 1;
}

extern "C" int test_rundb_reader(int, char**)  {
  RunDatabase rundb("RunDbSrv/RunDb");
  IRunDatabaseReader& db = rundb;
  RunDatabase::Status sc;
  RunDatabase::Params pars;
  RunDatabase::Files files;
  RunDatabase::Runs rset;
  RunDatabase::Run  r;

  int run_no = 10;
  std::cout << "db.existsRun(run_no):      " << (sc=db.existsRun(run_no)).str()           << std::endl;
  std::cout << "db.run(run_no):            " << (r=db.run(run_no)).str()                  << std::endl;
  std::cout << "db.runParam(run_no,Othe..):" << db.runParam(run_no,"OtherParam2").str()   << std::endl;
  std::cout << "db.runParams(run_no):      " << (pars=db.runParams(run_no)).str()         << std::endl;
  files=db.files(run_no);
  std::cout << "db.files(run_no):          " << files.str()             << std::endl;
  for(RunDatabase::Files::DATA::const_iterator f=files.data.begin(); f != files.data.end(); ++f)  {
    std::cout << "db.fileParams(FileName): " << db.fileParams((*f)->FileName).str()       << std::endl;
    std::cout << "db.fileParams(FileID):   " << db.fileParams((*f)->ID).str()             << std::endl;
    break;
  }
  std::cout << "db.runs(run_no, run_no+10):" << (rset = db.runs(run_no, run_no+10)).str() << std::endl;
  std::cout << "db.runsForFill(fill_no):   " << (rset = db.runsForFill(6988)).str()       << std::endl;
  rset = db.runsForUTC(992785491,992820291);
  std::cout << "db.runsForUTC(from,to):    " << rset.str()                                << std::endl;
  return 1;
}
