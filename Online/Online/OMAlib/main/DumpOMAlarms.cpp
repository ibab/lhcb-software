// $Id: DumpOMAlarms.cpp,v 1.2 2009-06-16 17:39:49 ggiacomo Exp $
#include <iostream>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OMAMessage.h"

using namespace std;

std::string AnaTask = "any";
std::string DB = "devdb10";
std::string DBuser = "lhcb_devmon_giacomo";
std::string DBpw = "testhist2009";

OnlineHistDB *HistDB= NULL;

void setoption(char opt, char* value);
void usage();

int main(int narg,char **argv ) {
  
  // get options
  int iarg =1;
  while(iarg < narg) {
    if ( '-' == *(argv[iarg]) ) {
      char opt=*(argv[iarg]+1);
      if ('h' == opt) {
        usage();
        return 0;
      }
      else {
        setoption( *(argv[iarg]+1), argv[iarg+1] );
      }
    }
    iarg++;
  }
  
  // connect to DB
  HistDB = new OnlineHistDB(DBpw,DBuser,DB);

  if (!HistDB) {
    cout << "ERROR: failed to connect to HistDB" << endl;
    return 1;
  }

  std::vector<int> messID;
 
  std::cout <<" You have "<< HistDB->getMessages(messID,AnaTask)
            <<" messages from ";
  if (AnaTask == "any") 
    std::cout << "all Online Monitoring Analysis";
  else 
    std::cout << "Analysis "<< AnaTask;
  std::cout <<std::endl<<std::endl;
  
  for (std::vector<int>::iterator im= messID.begin() ; im !=  messID.end() ; im++) {
    OMAMessage* message = new OMAMessage(*im, *HistDB);
    message->dump(&(std::cout));
    delete message;
  }

  delete HistDB;
  return 0;
}

void setoption(char opt, char* value) 
{
  switch(opt) 
  {
  case 't':
    AnaTask = value;
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
  cout << "DumpOMAlarms <options>" <<endl;
  cout << "   where options are:" <<endl;
  cout << "      -t  <AnalysisTaskName> \t (default is 'any') " <<endl;
  cout << "      -d  <DB>    \t choose database (default is the production DB at the pit)" <<endl;
  cout << "      -u  <DBuser>\t database login user (default is HIST_READER)" <<endl;
  cout << "      -p  <DBpw>  \t database login password (default is read-only password) " <<endl;
}
