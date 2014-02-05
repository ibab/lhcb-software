// $Header:
//  ====================================================================
//  
//  --------------------------------------------------------------------
//
//  Author    : Ioannis Chalkiadakis
//
//  ====================================================================
#include "OnlineFileSelector/FilePoller.h"
#include "GaudiKernel/ISvcLocator.h"
#include <algorithm>
#include <string>
#include <dirent.h>
#include <stdlib.h>

using namespace std;
using namespace LHCb;


FilePoller::FilePoller(const string& nam,ISvcLocator* svc)
                           : OnlineService::OnlineService(nam,svc),
                           m_scanDirectory(),
                           m_alrmTime()
{
  declareProperty("scanDirectory",m_scanDirectory);
  declareProperty("alarmTime",m_alrmTime);
}

FilePoller::~FilePoller() {}


/// Implementation of IInterface::queryInterface()
StatusCode FilePoller::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IHandleListenerSvc::interfaceID().versionMatch(riid))
    *ppvIF = (IHandleListenerSvc*)this;
  else if(IAlarmHandler::interfaceID().versionMatch(riid))
    *ppvIF = (IAlarmHandler*)this;
  else
    return OnlineService::queryInterface(riid, ppvIF);
  addRef();
  return StatusCode::SUCCESS;
}



/// Implementation of IOnlineService::initialize()
StatusCode FilePoller::initialize()  {
  StatusCode status = OnlineService::initialize();
  if (!status.isSuccess()) {
      return status;
  }
  cout << m_alrmTime << endl;
  cout << m_scanDirectory << endl;
  
  return status;
}

/// Implementation of IOnlineService::finalize()
StatusCode FilePoller::finalize()  {
  StatusCode status = OnlineService::finalize();
  if (!status.isSuccess()) {
      return status;
  }
  return status;
}


StatusCode FilePoller::start()
{
  StatusCode status = OnlineService::start();
  DimTimer::start(FilePoller::m_alrmTime);
  return status;
}

StatusCode FilePoller::stop()
{
  StatusCode status = OnlineService::stop();
  DimTimer::stop();
  return status;
}


const StatusCode FilePoller::issueAlarm(const string& msg) 
{
    cout << msg << endl;
    return StatusCode::FAILURE;
}


StatusCode FilePoller::poller(string scan_path)
{
  struct dirent d_entry;                  	
	struct dirent *d_res;
	int i;	
	DIR *dir,*nested_dir;
	char *path,*prev_path;
  StatusCode status;
  string n_path,dname;
  
  path = (char*)calloc(sizeof(char),256);
  prev_path = (char*)calloc(sizeof(char),256);
  if ((!path)||(!prev_path)){
    status = FilePoller::issueAlarm(OnlineService::name() + "calloc memory problem");
    return status; 
  }

  path = strncpy(path,scan_path.c_str(),255); //size of scan_path should be < 256
  dir = opendir(path);
  while ((0==readdir_r(dir,&d_entry,&d_res)) && (NULL!=d_res)) {
    
    prev_path = strncpy(prev_path,path,256);
    if (!strcmp(d_entry.d_name,".") || !strcmp(d_entry.d_name,".."))
        continue;
    
    dname = string(d_entry.d_name);
    
    FilePoller::addTofileNames(dname);  //simple, just sth to begin with
    
    //search for idle listeners to wake them up
    
    if (d_entry.d_type == DT_DIR) {

      n_path = string(path);
      n_path = n_path + "/" + dname;
      nested_dir = opendir(n_path.c_str());
      
		  if (!nested_dir) {
          status = FilePoller::issueAlarm(OnlineService::name()+"Error opening directory.");
          return status;
      }
      status = FilePoller::poller(n_path);
      if (StatusCode::FAILURE == status) {
        status = FilePoller::issueAlarm(OnlineService::name()+"The poller encountered an error.");
         return status;
      }
      i = closedir(nested_dir);
      if (-1 == i){
         status = FilePoller::issueAlarm(OnlineService::name()+"Problem closing directory.");
        return status;
      }
    }
    path = strncpy(path,prev_path,255);
  }
  
  return StatusCode::SUCCESS;
}


StatusCode FilePoller::addListener(IAlertSvc* Listener) 
{
  m_fileListeners.push_back(Listener);
  return StatusCode::SUCCESS;
}


StatusCode FilePoller::remListener(IAlertSvc* Listener)
{
   if (!m_fileListeners.empty())
   {
     vector<IAlertSvc*>::iterator del_l = FilePoller::findListener(Listener);
     if (del_l != m_fileListeners.end())
     {
       m_fileListeners.erase(del_l);
       return StatusCode::SUCCESS;
     }
   }
   return FilePoller::issueAlarm(OnlineService::name()+"No listeners available.");
}

vector<IAlertSvc*>::iterator FilePoller::findListener(IAlertSvc* Listener)
{
  vector<IAlertSvc*>::iterator iter;
  iter = find(m_fileListeners.begin(),m_fileListeners.end(),Listener);
  return iter;
}


const StatusCode FilePoller::showListeners()
{
  IAlertSvc* Listener; 
  vector<IAlertSvc*>::iterator iter;
  
  if (!m_fileListeners.empty())
  {
    for(iter = m_fileListeners.begin(); iter != m_fileListeners.end(); ++iter)
    {
      Listener = *iter;
      if (!Listener)
        return FilePoller::issueAlarm(OnlineService::name()+"Error retrieving service");
        cout << Listener->getSvcName() << endl;
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
  
}



  
void FilePoller::addTofileNames(const string& newFile)
{
  m_fileNames.push_back(newFile);
}

void FilePoller::remFromfileNames()
{
  m_fileNames.pop_back();
}


void FilePoller::timerHandler()
{
  DimTimer::stop();
  StatusCode poll_st = FilePoller::poller(FilePoller::m_scanDirectory);
  //If m_fileNames not emprty: invoke dataReady() from a Listener
  FilePoller::start();
} 




  
 
