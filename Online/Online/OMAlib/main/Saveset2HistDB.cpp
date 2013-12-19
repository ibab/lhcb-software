// $Id: Saveset2HistDB.cpp,v 1.10 2010-09-15 07:53:45 ggiacomo Exp $
#include <iostream>
#ifndef _WIN32
#include <stdlib.h>
#endif
#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"
// all the rest for the trending tool
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgTool.h"

#include "Trending/ITrendingTool.h"
#include <boost/filesystem.hpp>

using namespace std;

std::string Task = "";
std::string DB = OnlineHistDBEnv_constants::DB;
std::string DBuser = "HIST_WRITER";
std::string DBpw = "";
OnlineHistDB *HistDB= NULL;

class SavesetHistID {
public:
  SavesetHistID(std::string &hAlg,
                std::string &hName,
                OnlineHistDBEnv::HistType &hType) :
    alg(hAlg), name(hName), type(hType), isTrend(false)  { dump();}
  SavesetHistID(std::string &hAlg,
                std::string &hName):
    alg(hAlg), name(hName), type(OnlineHistDBEnv::TRE), isTrend(true)  {dump();}
  ~SavesetHistID() {}
  void dump() {
    if (isTrend)
      cout << "Declaring trend histogram: tag " << name <<" from File "<<alg<<
        "  of type "<< OnlineHistDBEnv_constants::HistTypeName[(int) type]
           <<endl;
    else
      cout << "Declaring histogram " << Task<<"/"<<alg<<
        "/"<<name << "  of type "<< OnlineHistDBEnv_constants::HistTypeName[(int) type]
           <<endl;
  }
  std::string alg;
  std::string name;
  OnlineHistDBEnv::HistType type;
  bool isTrend;
};
std::vector<SavesetHistID*> histos;

int declareDir(TDirectory* dir, std::string algo="",std::string hname="");
void setoption(char opt, char* value);
void usage();
OnlineHistDBEnv::HistType typeFromClass(std::string cl);

int main(int narg,char **argv ) {
  ITrendingTool * trendingTool = NULL;
  if(narg < 2) {
    usage();
    return 0;
  }
  // get options
  int firstfile =1, iarg =1;
  while(iarg < narg) {
    if ( '-' == *(argv[iarg]) ) {
      char opt=*(argv[iarg]+1);
      if ('h' == opt) {
        usage();
        return 0;
      }
      else if ('r' == opt) {
        Task = "TrendHistograms";
        firstfile = iarg+1;
      }
      else {
        setoption( *(argv[iarg]+1), argv[iarg+1] );
        firstfile = iarg+2;
      }
    }
    iarg++;
  }
  if (Task == "") {
    cout << "you must specify a task name"<< endl;
    usage();
    return 1;
  }
  if (firstfile >= narg) {
    cout << "you must specify some savesets "<< endl;
    usage();
    return 1;
  }

  // connect to DB
  if(DBpw == "") {
#ifdef _WIN32
    cout << "Enter the " << DBuser <<" password on "<<DB<<":";
    std::cin >> DBpw;
    std::cout<<endl;
#else
    char* pass =getpass("Enter your password:");
    DBpw=pass;
#endif
  }
  HistDB = new OnlineHistDB(DBpw,DBuser,DB);

  if (!HistDB) {
    cout << "ERROR: failed to connect to HistDB" << endl;
    return 1;
  }

  // loop on input root files and store histograms to be declared
  int nh=0;
  for (iarg = firstfile ; iarg < narg ; iarg++ ) {
    if (Task == "TrendHistograms") {
      // special case of trending histograms
      ISvcLocator * iface = Gaudi::svcLocator() ;
      IService * isvc ;
      iface -> getService( "ToolSvc" , isvc ) ;
      const IInterface *a3( isvc ) ;
      const std::string &name("TrendingTool");
      IAlgTool * intf = AlgTool::Factory::create(name, name, name, a3);

      trendingTool = dynamic_cast<ITrendingTool *>(intf) ;
      boost::filesystem::path filePath(argv[iarg] );
      trendingTool->openRead( filePath.replace_extension("").string() ) ;
      std::vector< std::string > tags ;
      trendingTool->tags( tags );
      std::vector< std::string >::iterator it;
      std::string fileName( argv[iarg] );
      for (it=tags.begin() ; it<tags.end(); it++) {
        nh++;
        std::string& tagname=*it;
        histos.push_back( new SavesetHistID(fileName, tagname ));
      }
    }
    else {
      TFile saveset(argv[iarg],"READ");
      nh += declareDir ((TDirectory*) &saveset);
      saveset.Close();
    }
  }

  // ask for confirmation
  if(nh>0) {
    std::string answ;
    cout << "Commit changes to DB? (Y/N) ";
    cin >> answ;
    cout<<endl;

    std::vector<SavesetHistID*>::iterator ih;
    if ( answ == "Y") {
      cout << "committing"<<flush;
      // declare and commit
      int j=0;
      for ( ih=histos.begin(); ih != histos.end() ; ih++) {
        if((*ih)->isTrend)
          HistDB->declareTrendingHistogram((*ih)->alg, (*ih)->name);
        else
          HistDB->declareHistogram(Task,
                                   (*ih)->alg,
                                   (*ih)->name,
                                   (*ih)->type);
        if ((++j)%50 == 0) cout <<"."<<flush;
      }
      HistDB->commit();
      cout << " OK"<<endl;
    }
    for ( ih=histos.begin(); ih != histos.end() ; ih++) {
      delete (*ih);
    }
  }
  delete HistDB;
  // Release trending tool
  if(trendingTool)
    delete trendingTool ;
  return 0;
}

int declareDir(TDirectory* dir, std::string algo,std::string hpath) {
  int nh=0;
  TIter next(dir->GetListOfKeys());
  TKey *k;
  while ( (k=(TKey*)next()) ) {
    if(std::string(k->GetClassName()) == "TDirectoryFile") {
      std::string passpath=hpath;
      std::string passalgo=algo;
      if (passalgo == "") {
        passalgo= k->GetName();
      }
      else {
        passpath += k->GetName();
        passpath += "/";
      }
      nh += declareDir((TDirectory*) dir->Get(k->GetName()), passalgo , passpath);
    }
    else {
      if (algo == "") {
        cout << "ERROR: unexpected object "<<k->GetName()<<" not inside algorithm directory"<<endl;
      }
      else {
        std::string hname=hpath;
        hname += k->GetName();
        OnlineHistDBEnv::HistType type= typeFromClass(k->GetClassName());
        nh++;
        histos.push_back( new SavesetHistID(algo, hname, type ) );


      }
    }
  }
  return nh;

}

OnlineHistDBEnv::HistType typeFromClass(std::string cl) {
  OnlineHistDBEnv::HistType type= OnlineHistDBEnv::H1D;
  if (cl == "TProfile2D") {
    type= OnlineHistDBEnv::P2D;
  }
  else {
    if (cl == "TProfile") {
      type= OnlineHistDBEnv::P1D;
    }
    else{
      if (cl.substr(0,3) == "TH2")
        type= OnlineHistDBEnv::H2D;
    }
  }
  return type;
}

void setoption(char opt, char* value)
{
  switch(opt)
  {
  case 't':
    Task = value;
    break;
  case 'd':
    DB = value;
    break;
  case 'u':
    DBuser = value;
    break;
  case 'p':
    DBpw = value;
    break;
  }
}


void usage()
{
  cout << "Saveset2HistDB <options> saveset1.root saveset2.root ..." <<endl;
  cout << "   where options are:" <<endl;
  cout << "      -t  <TaskName> \t mandatory, unless -r" <<endl;
  cout << "      -r  source contains trending histograms (equivalent to -t TrendHistograms)" <<endl;
  cout << "      -d  <DB>    \t choose database (default is the production DB at the pit)" <<endl;
  cout << "      -u  <DBuser>\t database login user (default is HIST_WRITER)" <<endl;
  cout << "      -p  <DBpw>  \t database login password " <<endl;
}
