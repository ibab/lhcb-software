// $Id: DumpOMAlarms.cpp,v 1.8 2010-09-21 10:39:50 ggiacomo Exp $
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <map>
#include <cstdlib>
//    #include <stdlib.h>

#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OMAMessage.h"
#include "OMAlib/OMAMsgInterface.h"
#include <dis.hxx>
using namespace std;

std::string AnaTask = "any";
std::string System = "any";
int Anaid = -1;
int Msgid = -1;
std::string DB = OnlineHistDBEnv_constants::DB;
std::string DBuser = OnlineHistDBEnv_constants::ACCOUNT;
std::string DBpw = OnlineHistDBEnv_constants::PASSWORD;
bool clear=false;
bool AllAlarms=false;

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
      else if('a' == opt) {
        AllAlarms = true;
      }
      else {
        setoption( *(argv[iarg]+1), argv[iarg+1] );
      }
    }
    iarg++;
  }
  
  if(clear) {
    std::string dimDNS=getenv("DIM_DNS_NODE");
    if (dimDNS != "mona08") {
      cout << "can't remove any alarm without checking the Alarm screen" <<endl;
      cout << "please change your DIM_DNS_NODE to mona08"<<endl;
      return 0;
    }

  }

  if(clear && DBuser == "HIST_READER") {
    DBuser ="HIST_WRITER";
#ifdef _WIN32
    cout << "Enter the HIST_WRITER password on "<<DB<<" for clearing alarms:";
    cin >> DBpw;
    cout<<endl;
#else
    char* pass =getpass("Enter the HIST_WRITER password for disabling alarms:");   
    DBpw=pass;
#endif
  }
  // connect to DB
  HistDB = new OnlineHistDB(DBpw,DBuser,DB);
  
  if (!HistDB) {
    cout << "ERROR: failed to connect to HistDB" << endl;
    return 1;
  }

  std::vector<int> messID;
  std::vector<OMAMessage*> fltMessages;
  std::vector<OMAMessage*>::iterator iM;

  int ntot=HistDB->getMessages(messID,AnaTask);
  int nactive=0;
  int nFltMsg=0;
  for (std::vector<int>::iterator im= messID.begin() ; im !=  messID.end() ; im++) {
    OMAMessage* message = new OMAMessage(*im, *HistDB);
    if (message->isactive() && 
        (System == "any" || std::string(message->concernedSystem()) == System) )  nactive++;
    bool dump=true;
    if ( !message->isactive() && !AllAlarms) dump=false;
    if ( Anaid >0 && (int) message->anaId() != Anaid) dump=false;
    if ( Msgid >0 && (int) message->id() != Msgid) dump=false;
    if ( System != "any" && std::string(message->concernedSystem()) != System) dump=false;
    if(dump) {
      fltMessages.push_back(message);
      nFltMsg++;
    }      
    else {
      delete message;
    }
  }

  std::cout <<" You have "<< nactive
            <<" active messages ( + "<< (ntot-nactive) <<
    " archived messages) from ";
  if (AnaTask == "any") 
    std::cout << "all Online Monitoring Analysis";
  else 
    std::cout << "Analysis "<< AnaTask;
  std::cout <<std::endl<< nFltMsg  <<" messages filtered according to your request:" <<std::endl<<std::endl;

  for (iM=fltMessages.begin(); iM != fltMessages.end(); iM++) {
    (*iM)->dump(&(std::cout));
  }

  if (clear && nFltMsg ) {
    std::string answer;
    cout << "Are you sure you want to disable these alarms? (Y/N)";
    cin >> answer;
    int nClrMsg=0;
    if (answer == "Y") {
      for (iM=fltMessages.begin(); iM != fltMessages.end(); iM++) {
        OMAMessage* message = *iM;
        cout << "disabling message "<< message->id() <<endl;
        nClrMsg++;
        message->disable();
        message->store(true);
        // inform ECS in case analysis task is not running
        OMAMsgInterface* msgIntface= new OMAMsgInterface(HistDB, "DBDrivenAnalysisTask/LHCb");
        if(msgIntface->startMessagePublishing()) { // when true, DIM service for alarm communication is not already being used
          cout << "telling ECS to clear alarm "<< message->id() <<endl;
          msgIntface->unpublishMessage(message);
          sleep(2);
        }
        else {
          cout << "Analysis task is running and will clear alarm from ECS screen  when the first analysis on a physics saveset will be performed"<<endl;
        }
        delete message;
      }

      cout << nClrMsg << " messages have been disabled (kept in archive in non-active state)"<<endl;
      HistDB->commit();
    }
    else {
      cout << "******  warning: Messages NOT disabled (to disable them, answer 'Y' next time) *********** " <<endl;
    }
  }
  else {
    for (iM=fltMessages.begin(); iM != fltMessages.end(); iM++) {
      OMAMessage* message = *iM;
      if(message) delete message;
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
  case 'i':
    Anaid = atoi(value);
    break;
  case 'm':
    Msgid = atoi(value);
    break;
  case 's':
    System = value;
    break;
  }
}


void usage() 
{
  cout << "DumpOMAlarms <options>" <<endl;
  cout << "   where options are:" <<endl;
  cout << "      -t  <AnalysisTaskName> \t to filter by analysys task name (default is 'any') " <<endl;
  cout << "      -c          \t\t to disable the filtered alarms " <<endl;
  cout << "      -a          \t\t to show also archived (inactive) messages " <<endl;
  cout << "      -m  <mID>   \t\t filter messages by message ID" <<endl;
  cout << "      -i  <anaID> \t\t filter messages by analysis ID" <<endl;
  cout << "      -s  <system>\t\t filter messages by concerned system (in uppercase, e.g. BEAM, DAQ, HLT, CALO, RICH ...)" <<endl;
  cout << "      -d  <DB>    \t\t choose database (default is the production DB at the pit)" <<endl;
  cout << "      -u  <DBuser>\t\t database login user (default is HIST_READER)" <<endl;
  cout << "      -p  <DBpw>  \t\t database login password (default is read-only password) " <<endl;
}
