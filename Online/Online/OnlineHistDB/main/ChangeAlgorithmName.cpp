// $Id: ChangeAlgorithmName.cpp,v 1.1 2008-08-12 10:37:31 ggiacomo Exp $
#include <iostream>
#include <OnlineHistDB/OnlineHistDB.h>
using namespace std;

void usage() {
  cout << "Usage:" <<endl;
  cout << "ChangeAlgorithmName taskName oldName newName" <<endl;
}

int main(int argc, char* argv[]) {
  if(argc<4) {
    usage();
    return 0;
  }
  string passwd;
  string taskName=argv[1];
  string oldName=argv[2];
  string newName=argv[3];

  cout << "Enter the HIST_WRITER account's password: ";
  cin >> passwd;
  cout<<endl;
  //  OnlineHistDB *HistDB = new OnlineHistDB(passwd,"HIST_WRITER","HISTDB");
  OnlineHistDB *HistDB = new OnlineHistDB(passwd,"LHCB_MON_GIACOMO","devdb10");
  OnlineHistTask* task= HistDB->getTask(taskName);
  if(! task) {
    cout << "Task "<<taskName<<" does not exist!"<<endl;
  }
  else {
    if(task->renameAlgo(oldName,newName)) {
      cout <<"Algorithm "<<oldName<<" renamed to "<<newName<<
        " for task "<<  taskName<<endl;
      HistDB->commit();    
    }
  }
  delete HistDB;
}
