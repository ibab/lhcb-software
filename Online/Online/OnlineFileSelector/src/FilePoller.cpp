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
#include <queue>
#include <string>
#include <dirent.h>
#include <stdlib.h>

using namespace std;
using namespace LHCb;


FilePoller::FilePoller(/*const string& nam,
                         ISvcLocator* svc,*/
                       string directory,
                       int time
                       ) : /*OnlineService::OnlineService(nam,svc),
                           m_name(nam),
                           m_pSvcLocator(svc),*/ //needed? how are they initialized?
                           m_scanDirectory(directory),
                           m_alrmTime(time)
{
  cout << "object instantiated" << endl;
}

FilePoller::FilePoller(const FilePoller& fileP)
                      :DimTimer::DimTimer(),
                       /*OnlineService(m_name,m_pSvcLocator),
                       m_name(fileP.m_name),
                       m_pSvcLocator(fileP.m_pSvcLocator),*/
                       m_scanDirectory(fileP.m_scanDirectory),
                       m_alrmTime(fileP.m_alrmTime)
{}
 
 
FilePoller::~FilePoller() {

 cout << "object destroyed" << endl;

}

/*
///Implementation of IInterface::queryInterface()
StatusCode FilePoller::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IDirectoryScanner::interfaceID().versionMatch(riid))
    *ppvIF = (IDirectoryScanner*)this;
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
*/

StatusCode FilePoller::start()
{
  StatusCode status = StatusCode::SUCCESS;//OnlineService::start();
  DimTimer::start(FilePoller::m_alrmTime);
  return status;
}

StatusCode FilePoller::stop()
{
  StatusCode status =  StatusCode::SUCCESS;//OnlineService::stop();
  DimTimer::stop();
  //find a way to stop the execution of the poller -- maybe if there are no more listeners waiting for a datastream
  return status;
}


/*
const StatusCode FilePoller::issueAlarm(const string& msg) 
{
    cout << msg << endl;
    return StatusCode::FAILURE;
}

*/
StatusCode FilePoller::poller(string scan_path)
{
  struct dirent d_entry;                  	
	struct dirent *d_res;
	int i;	
	DIR *dir,*nested_dir;
	char *path,*prev_path;
  StatusCode status = StatusCode::SUCCESS; //for debugging
  string n_path,dname;
  
  path = (char*)calloc(sizeof(char),256);
  prev_path = (char*)calloc(sizeof(char),256);
  if ((!path)||(!prev_path))
  {  cout << "open error" <<endl;///////////////
    
    // status = FilePoller::issueAlarm(OnlineService::name() + "calloc memory problem");
      return status; 
  }

  path = strncpy(path,scan_path.c_str(),255); //size of scan_path should be < 256
  dir = opendir(path);
  while ((0==readdir_r(dir,&d_entry,&d_res)) && (NULL!=d_res)) {
    
    prev_path = strncpy(prev_path,path,256); //keep the previous path for the recursion
    if (!strcmp(d_entry.d_name,".") || !strcmp(d_entry.d_name,".."))
        continue;
    
    dname = string(d_entry.d_name);
    cout << d_entry.d_name << endl; /////////////
    
    FilePoller::addTofileNames(dname);  //simple, just sth to begin with
    //search for idle listeners to wake them up
    if (d_entry.d_type == DT_DIR) {

      n_path = string(path);
      n_path = n_path + "/" + dname;
      nested_dir = opendir(n_path.c_str());
      
		  if (!nested_dir) {
        cout << "nested open error" <<endl;/////////////
        //status = FilePoller::issueAlarm(OnlineService::name()+"Error opening directory.");
          return status;
          
      }
      status = FilePoller::poller(n_path);
      if (StatusCode::FAILURE == status)
      {
        cout << "open error 2" <<endl;
        // status = FilePoller::issueAlarm(OnlineService::name()+"The poller encountered an error.");
        return status;
        
      }
      i = closedir(nested_dir);
      if (-1 == i)
      {
        cout << "open error 3" << endl;
        // status = FilePoller::issueAlarm(OnlineService::name()+"Problem closing directory.");
        return status;
      }
    }
    path = strncpy(path,prev_path,255);
  }
  
  return StatusCode::SUCCESS;
}

/*
StatusCode FilePoller::addListener(IService* Listener)
{
  FilePoller::m_fileListeners.push(Listener);
  return StatusCode::SUCCESS;
}


StatusCode FilePoller::remListener()
{
   if (!FilePoller::m_fileListeners.empty())
   {
     FilePoller::m_fileListeners.pop();
     return StatusCode::SUCCESS;
   }
   FilePoller::issueAlarm(OnlineService::name()+"No listeners available.");    
  return StatusCode::FAILURE;
  
}


const StatusCode FilePoller::printListeners()
{
  ISvcLocator* Listener; //or ISvcLocator* or OnlineService* ??
  

  while(!FilePoller::m_fileListeners.empty())
  {
    Listener = FilePoller::m_fileListeners.front();
    
    SmartIF<IService> pol_list(Listener);
    if (!pol_list.isValid()) {
    //  return FilePoller::issueAlarm(OnlineService::name()+"Error retrieving service");
    }
    cout << pol_list->name() << endl;
    
    
  }
  return StatusCode::SUCCESS;
}

*/

  
void FilePoller::addTofileNames(const string& newFile)
{
  m_fileNames.push_back(newFile);
}

void FilePoller::remFromfileNames()
{
  m_fileNames.pop_back();
}

void FilePoller::setScanDirectory(string directory)
{
  m_scanDirectory = directory;
}

void FilePoller::setAlrmTime(int seconds)
{
  m_alrmTime = seconds;
}


void FilePoller::timerHandler()
{
  DimTimer::stop();
  StatusCode poll_st = FilePoller::poller(FilePoller::m_scanDirectory);
  //If m_fileNames not emprty: invoke dataReady() from a Listener
  /*cout <<"from vector" <<endl;
  for (int i=0; i<m_fileNames.size(); i++)
  cout << m_fileNames[i]<<endl;*/
  FilePoller::start();
} 




  
 
