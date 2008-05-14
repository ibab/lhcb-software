// $Id: FixDoubleAlgName.cpp,v 1.1 2008-05-14 15:00:56 ggiacomo Exp $
#include <iostream>
#include <OnlineHistDB/OnlineHistDB.h>
using namespace std;

void usage() {
  cout << "Usage:" <<endl;
  cout << "FixDoubleAlgName -t TaskName [-f]" <<endl;
  cout << "removes a leading \"AlgName/\" from all histogram names" <<endl;
  cout << "   of task TaskName." <<endl;
  cout << "If -f is specified, any \"string/\" will be removed even" <<endl;
  cout << "   if it's not the algorithm name"<<endl;
}


int main(int argc, char* argv[]) {
  bool force=false;
  string task="_none_"; 
  string passwd;
  // read options
  int iarg=1;
  while(iarg < argc) {
    if(string(argv[iarg]) == "-f") {
      force = true;
    }
    else if (string(argv[iarg]) == "-t") {
      if( ++iarg < argc ) {
        task=argv[iarg];
      }
    } 
    else if (string(argv[iarg]) == "-h") {
      usage();
      return 0;
    }
    iarg++;
  }
  if (task == "_none_") {
    usage();
    return 0;
  }
  cout << "Enter the HIST_WRITER account's password: ";
  cin >> passwd;
  cout<<endl;
  OnlineHistDB *HistDB = new OnlineHistDB(passwd,"HIST_WRITER","HISTDB");
  vector<OnlineHistogram*> list;
  HistDB->getHistogramsByTask(task,&list);
  vector<OnlineHistogram*>::iterator ht;
  string algo,name;
  //cout <<"Please be patient"<<flush;
  int nh=0,nhc=0;
  string message="";
  for (ht=list.begin() ; ht != list.end() ; ht++) {
    
    name= (*ht)->hname();
    algo= (*ht)->algorithm()+"/";
    
    if(force) { // remove any "string/" from name
      int poslash= (int)name.find("/");
      if( poslash >0 ) {
        message += "removing "+name.substr(0,poslash+1)+" from "+(*ht)->hname()+"\n";
        name.erase(0,poslash+1);
        (*ht)->rename((*ht)->task(), (*ht)->algorithm(),name);
        nhc++;
      }
    }
    else {
      if(0 == (int)name.find(algo)) {
        name.erase(0,algo.size());
        message += "removing \""+algo+"\" from \""+(*ht)->hname()+"\"\n";
        (*ht)->rename((*ht)->task(), (*ht)->algorithm(),name);
        nhc++;
      }
    }

    nh++;
    //if(nh%100 == 0) cout<<"."<<flush;
  }
  cout <<endl;
  if(nhc>0) {
    cout <<"Removing double alg names from "<<nhc<<
      " histograms of task "<<task<<endl<<endl;
    cout << message <<endl;
    cout << "Do you want to proceed? (Y/N)";
    string answ;
    cin >> answ;
    if (answ == "Y") {
      HistDB->commit();
      cout << "changes committed"<<endl;
    }
    else {
      cout << "changes aborted"<<endl;
    }
  }
  else{ 
    cout <<"Nothing to do for task "<<task<<endl;
  }
  delete HistDB;
}
