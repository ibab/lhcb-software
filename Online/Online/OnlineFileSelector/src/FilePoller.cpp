// $Header:
//  ====================================================================
//  
//  --------------------------------------------------------------------
//
//  Author    : Ioannis Chalkiadakis
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "FilePoller.h"
#include <string>
#include <dirent>
#include <vector>

#include "dimcpp.cxx"

using namespace std;
using namespace LHCb;
using namespace queue;


class FilePoller : public OnlineService, virtual public IDirectoryScanner, virtual public IAlarmHandler
{
  

public:

/// Implementation of IService::initialize()
StatusCode LHCb::OnlineService::initialize()  {


}

/// Implementation of IService::finalize()
StatusCode LHCb::OnlineService::finalize()  {
 

}


FilePoller::FilePoller(string directory,
                       int time
                       )
{
  setScanDirectory(directory);
  setAlrmTime(time);
  m_timerUtil = Dtimer::DTimer();
}

FilePoller::FilePoller(const FilePoller& fileP)
{
  fileP.setScanDirecory(this.m_scanDirectory);
  fileP.setAlrmTime(this.m_alrmTime);
  fileP.m_timerUtil = this.m_timerUtil;
}
  
virtual ~FilePoller::FilePoller() {}


StatusCode start()
{
  timerUtil.start(m_alrmTime);
  return StatusCode::SUCCESS;
}

StatusCode stop()
{
  timerUtil.stop();
  //find a way to stop the execution of the poller -- maybe if there are no more listeners waiting for a datastream
  return StatusCode::SUCCESS;
}

virtual StatusCode poller()
{
  struct dirent d_entry;                  	
	struct dirent *d_res;
	int i;	
	DIR *dir,*nested_dir;
	string path,prev_path;
  StatusCode status;
  
  path = scanDirectory;
  dir = opendir(scanDirectory);
  
  while ((0==readdir_r(dir,&d_entry,&d_res)) && (NULL!=d_res)) {
	
    prev_path = path; //keep the previous path for the recursion
    if (d_entry.d_name.compare(".") || d_entry.d_name.compare(".."))
            	  	continue;	
	    
    fileNames.push_back(d_entry.d_name);  //simple, just sth to begin with
    //search for idle listeners to wake them up
    if (d_entry.d_type == DT_DIR) {
      path = path + "/" + d_entry.d_name;
      nested_dir = opendir(path);	
		  if (!nested_dir) {
        status = issueAlarm(poller.name(),"Error opening directory.");
      }
      status = FilePoller::poller(path);
      if (StatusCode::FAILURE == status)
      {
        status = issueAlarm(poller.name(),"The poller encountered an error.");
      }
      i = closedir(nested_dir);
      if (-1 == i)
      {
        status = issueAlarm(poller.name(),"Problem closing directory.");
      }
      
    }
    path = prev_path;
  }

  return StatusCode::SUCCESS
}


virtual StatusCode addListener(const ISvcLocator* Listener)
{
  m_fileListeners.push(Listener);
  return StatusCode::SUCCESS;
}


virtual StatusCode remListener()
{
  if (!m_fileListeners.empty())
  {
    m_fileListeners.pop();
    return StatusCode::SUCCESS;
  }
  else
    return issueAlarm(poller.name(),"No listeners available.");    
}
  

virtual const StatusCode printListeners()
{
  ISvcLocator* Listener;
  
  while(!m_fileListeners.empty())
  {
    *Listener = *(m_fileListeners.front()); 
    info(Listener.name()); //inherited from Service.h
  }
  return StatusCode::SUCCESS;
}
  


void setScanDirectory(string directory)
{
    m_scanDirectory = directory;
}

void setAlrmTime(int seconds)
{
    m_alrmTime = seconds;
}


private:

  const string m_scanDirectory;
  const int m_alrmTime;
  DTimer m_timerUtil;
  vector<string> m_fileNames;
  queue<ISvcLocator*> m_fileListeners;
  

  class DTimer : public DimTimer 
  {
    public:
      DTimer::DTimer():DimTimer();
    
      void timerHandler()
      {
        FilePoller::timerUtil.stop();
        StatusCode poll_st = FilePoller::poller();
        //If m_fileNames not emprty: invoke dataReady() from a Listener
        FilePoller::start();
      }
  }
    

}
  

 
