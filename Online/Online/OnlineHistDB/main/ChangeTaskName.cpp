// $Id: ChangeTaskName.cpp,v 1.1 2008-05-14 15:00:56 ggiacomo Exp $
#include <iostream>
#include <OnlineHistDB/OnlineHistDB.h>
using namespace std;

void usage() {
  cout << "Usage:" <<endl;
  cout << "ChangeTaskName oldName newName" <<endl;
}

int main(int argc, char* argv[]) {
  if(argc<3) {
    usage();
    return 0;
  }
  string passwd;
  string oldName=argv[1];
  string newName=argv[2];

  cout << "Enter the HIST_WRITER account's password: ";
  cin >> passwd;
  cout<<endl;
  OnlineHistDB *HistDB = new OnlineHistDB(passwd,"HIST_WRITER","HISTDB");
  OnlineHistTask* task= HistDB->getTask(oldName);
  if(! task) {
    cout << "Task "<<oldName<<" does not exist!"<<endl;
  }
  else {
    if(task->rename(newName)) {
      cout <<"Task "<<oldName<<" renamed to "<<newName<<endl;
      HistDB->commit();    
    }
  }
  delete HistDB;
}
