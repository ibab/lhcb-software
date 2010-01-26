// $Id: DumpOMAlarms.cpp,v 1.4 2010-01-26 14:25:37 ggiacomo Exp $
#include <iostream>
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OMAMessage.h"

using namespace std;

std::string AnaTask = "any";
std::string DB = OnlineHistDBEnv_constants::DB;
std::string DBuser = "HIST_READER";
std::string DBpw = "READER";
bool clear=false;

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
      else if('c' == opt) {
        clear = true;
      }
      else {
        setoption( *(argv[iarg]+1), argv[iarg+1] );
      }
    }
    iarg++;
  }
  
  if(clear && DBuser == "HIST_READER") {
    DBuser ="HIST_READER";
    cout << "Enter the " << DBuser <<" password on "<<DB<<" for clearing alarms:";
    cin >> DBpw;
    cout<<endl;
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

  if (clear) {
    std::string answer;
    cout << "Are you sure you want to clear these alarms? (Y/N)";
    cin >> answer;
    if (answer == "Y") {
      for (std::vector<int>::iterator im= messID.begin() ; im !=  messID.end() ; im++) {
        OMAMessage* message = new OMAMessage(*im, *HistDB);
        message->remove();
        delete message;
      }
      cout << messID.size() << " messages have been cleared"<<endl;
    }
    else {
      cout << "Message NOT cleared"<<endl;
    }
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
