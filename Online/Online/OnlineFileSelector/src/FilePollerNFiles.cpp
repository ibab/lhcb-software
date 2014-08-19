// $Header:
//  ====================================================================
//  
//  --------------------------------------------------------------------
//
//  Author    : Ioannis Chalkiadakis
//
//  ====================================================================
#include "OnlineFileSelector/FilePollerNFiles.h"
#include "GaudiKernel/ISvcLocator.h"  
#include "GaudiKernel/SvcFactory.h"
#include <string>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FilePollerNFiles)

using namespace std;
using namespace LHCb;
using namespace boost;

FilePollerNFiles::FilePollerNFiles(const string& nam,ISvcLocator* svc)
                           : OnlineService::OnlineService(nam,svc),
                           m_scanDirectory(),
                           m_alrmTime(),
			   m_CurRun("")
{
  declareProperty("scanDirectory",m_scanDirectory);
  declareProperty("alarmTime",m_alrmTime);
  declareProperty("DbName",m_nameOfDb);
  declareProperty("MinimumFileNum",m_minFileNum);  
}

FilePollerNFiles::~FilePollerNFiles() {}


/// Implementation of IInterface::queryInterface()
StatusCode FilePollerNFiles::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IHandleListenerSvc::interfaceID().versionMatch(riid))
    *ppvIF = (IHandleListenerSvc*)this;
  else if(IAlarmHandler::interfaceID().versionMatch(riid))
    *ppvIF = (IAlarmHandler*)this;
  else if(IOnlineBookkeepNFiles::interfaceID().versionMatch(riid))
    *ppvIF = (IOnlineBookkeepNFiles*)this;
  else
    return OnlineService::queryInterface(riid, ppvIF);
  addRef();
  return StatusCode::SUCCESS;
}



/// Implementation of IOnlineService::initialize()
StatusCode FilePollerNFiles::initialize()  {

  StatusCode status = OnlineService::initialize();
  MsgStream log(msgSvc(), name());
  
  status = FilePollerNFiles::connectToDb();
  
  return status;
}

/// Implementation of IOnlineService::finalize()
StatusCode FilePollerNFiles::finalize()  {
  StatusCode statusP = OnlineService::finalize();
  MsgStream log(msgSvc(), name());
  
  int status = sqlite3_close(FilePollerNFiles::m_FileInfo);
  if (SQLITE_OK != status) {
      	
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error closing database" + msg << endmsg;    
	
      return StatusCode::FAILURE;
  }

  return statusP;
}


/// Implementation of IOnlineService::start()
StatusCode FilePollerNFiles::start()
{
  StatusCode status = OnlineService::start();
  DimTimer::start(FilePollerNFiles::m_alrmTime);
  return status;
}

/// Implementation of IOnlineService::stop()
StatusCode FilePollerNFiles::stop()
{
  StatusCode status = OnlineService::stop();
  DimTimer::stop();
  return status;
}


/// Implementation of IAlarmHandler.
const StatusCode FilePollerNFiles::issueAlarm(const string& file) 
{//file is the actual file name, not whole path
  MsgStream log(msgSvc(), name());
  const char* patternR = "/[0-9]*_";
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "INSERT INTO FileRecords (FileName,RunNumber,TotalEvents,StatusFlag) VALUES (?,?,-1,-1);";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  run_no = strtol(getRunFileNumber("/"+file,patternR).c_str(),NULL,10);  // "/" to support the regexp and not cut off the run number
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,2,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error inserting record into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  string run = getRunFileNumber("/"+file,patternR);
  status = decreaseCounter(run);
  int cnt = getCounter(run);
  if (0 == cnt)
      status = clearRun(run);	
  
  
  return StatusCode::SUCCESS;	    
	

}






/// Implementation of polling method.
StatusCode FilePollerNFiles::poller(string scan_path)
{
  MsgStream log(msgSvc(), name());
  struct dirent d_entry;                  	
  struct dirent *d_res;
  int i;	
  DIR *dir,*nested_dir;
  char *path,*prev_path;
  StatusCode status;
  string n_path,dname;
  const char* pattern = "/[0-9]*_[0-9]*.raw";  
  const char* patternR = "/[0-9]*_";

  path = (char*)calloc(sizeof(char),1024);
  prev_path = (char*)calloc(sizeof(char),1024);
  if ((!path)||(!prev_path)){
    log << MSG::ERROR << "Calloc memory problem." << endmsg; 
    return StatusCode::FAILURE; 
  }

  path = strncpy(path,scan_path.c_str(),1023);
  dir = opendir(path);
  
  if ( 0 == dir )  {
     log << MSG::ERROR << "The directory "+scan_path+" is not valid!" << endmsg;
     return StatusCode::FAILURE; 
  }
  
  while ((0==readdir_r(dir,&d_entry,&d_res)) && (NULL!=d_res)) {
    
    prev_path = strncpy(prev_path,path,1023);
    if (!prev_path) {
	   log << MSG::ERROR << "The directory prev_path is not valid!" << endmsg; 
	   return StatusCode::FAILURE;
    }
    if (!strcmp(d_entry.d_name,".") || !strcmp(d_entry.d_name,".."))
        continue;
    
    dname = string(d_entry.d_name);
    n_path = string(path);
    
    if (d_entry.d_type != DT_DIR) {
      
      //Place path of the file in buffer.
      if (find( m_fileNames.begin(),  m_fileNames.end(),n_path) ==  m_fileNames.end()) 
      {
        string file = n_path + "/" + dname;
        FilePollerNFiles::addTofileNames(file);
        const string file_regexp = getRunFileNumber(file,pattern);
        
        if (StatusCode::FAILURE == isBookKept(file_regexp))
        {
	  
          StatusCode sc = markBookKept(file_regexp,0); //setting statusFlag = 0
          string run = getRunFileNumber(file,patternR);
	  if (StatusCode::FAILURE == existsRun(run)){
	    if (StatusCode::FAILURE == insertRun(run)){
	       log << MSG::ERROR << "Error inserting new run in database." << endmsg; 
	       return StatusCode::FAILURE;
	    }
          }
	  else if (-1 != getCounter(run)) {
		
	       	if (StatusCode::FAILURE == increaseCounter(run)){
	       		log << MSG::ERROR << "Error updating run counter in database." << endmsg; 
	       		return StatusCode::FAILURE;
	    	}
	    
	  }
	}       
      }
      
    }
    else {

      n_path = n_path + "/" + dname;
      nested_dir = opendir(n_path.c_str());
      
      if (!nested_dir) {
          log << MSG::ERROR << "Error opening nested directory." << endmsg;
          return StatusCode::FAILURE;
      }
      status = FilePollerNFiles::poller(n_path);
      if (StatusCode::FAILURE == status) {
         log << MSG::ERROR << "The poller encountered an error while recursing." << endmsg;
         return StatusCode::FAILURE;
      }
      i = closedir(nested_dir);
      if (-1 == i){
        log << MSG::ERROR << "Problem closing directory." << endmsg;
        return StatusCode::FAILURE;
      }
    }
    
    path = strncpy(path,prev_path,1023);
    if (!path){
        log << MSG::ERROR << "The directory path is not valid!" << endmsg;
	return StatusCode::FAILURE;
    }
  }
  
  return StatusCode::SUCCESS;
}

/// Implementation of IHandleListenerSvc::addListener.
StatusCode FilePollerNFiles::addListener() 
{
  MsgStream log(msgSvc(), name());
   
  StatusCode sc = Service::service("EventSelector",m_EvtSelector,true);
  if ( !sc.isSuccess() )  {
    log << MSG::ERROR << "Cannot register EventSelector." << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/// Implementation of IHandleListenerSvc::remListener.
StatusCode FilePollerNFiles::remListener()
{
  MsgStream log(msgSvc(), name());
  
  if (m_EvtSelector){
	m_EvtSelector = 0;
        info(OnlineService::name()+": Listener removed.");
  	return StatusCode::SUCCESS;
  }
  else 
	info(OnlineService::name()+": No listener available.");
  
  return StatusCode::FAILURE;
}


/// Implementation of IHandleListenerSvc::showListeners.
const StatusCode FilePollerNFiles::showListeners()
{
  MsgStream log(msgSvc(), name());
  
  if (m_EvtSelector)
  {
     log << MSG::INFO  << m_EvtSelector->getSvcName() << endmsg;
     return StatusCode::SUCCESS;
  }
  
  log << MSG::ERROR << "No listener at the moment." << endmsg;
  return StatusCode::FAILURE;
  
}


void FilePollerNFiles::addTofileNames(const string& newFile)
{
  m_fileNames.push_back(newFile);
}


string FilePollerNFiles::remFromfileNames()
{
  MsgStream log(msgSvc(), name());
  if (!m_fileNames.empty())
  {
    string path_file = m_fileNames.front();
    m_fileNames.pop_front();
    return path_file;
  }
  else 
  {
    log << MSG::ERROR << "No available files." << endmsg;
    return "";
  }
}

  
/// Implementation of IHandleListenerSvc::statusReport.
StatusCode FilePollerNFiles::statusReport(StatusCode status, const string file)
{//arg is the whole path name

  MsgStream log(msgSvc(), name());
  const char* pattern = "/[0-9]*_[0-9]*.raw";  
  const string file_regexp = getRunFileNumber(file,pattern);
  

  if (StatusCode::FAILURE == status)
  {
    // no book-keeping
    log << MSG::ERROR << "The recipient encountered a problem while reseting the criteria - check database for error entry." << endmsg;
    StatusCode sc = issueAlarm(file_regexp);
  }
  else if (StatusCode::SUCCESS == status)
  {
    // book-keep the file as "being processed" -> stFlag = 2
    StatusCode sc = SetInProcess(file_regexp,2);
  }
  
  return status;

}


/// Implementation of DimTimer::timerHandler.
void FilePollerNFiles::timerHandler()
{
  StatusCode sc1,sc2,sc3;
  string path_name;
  deque<string> empty_buffer; 
  const char* patternR = "/[0-9]*_";
  const char* pattern = "/[0-9]*_[0-9]*.raw";  
  MsgStream log(msgSvc(), name());
  //DimTimer::stop();
  StatusCode poll_st = FilePollerNFiles::poller(FilePollerNFiles::m_scanDirectory);
  poll_st = showListeners();
  poll_st = printDB();
  
  while (m_EvtSelector && !m_fileNames.empty()) {
      
      path_name = remFromfileNames();
      
      string run = getRunFileNumber(path_name,patternR);
      int counter = getCounter(run);
      //Check if we do not need to read files of this run anymore.
      if (-1 == counter){
    	 sc1 = clearRun(run);
	 continue;
      }
	
      //Set the counter for the remaining files.
      if (0 == m_remainingFiles)
	m_remainingFiles = m_minFileNum;
	
      const string file = getRunFileNumber(path_name,pattern);
      //check if it is faulty, already processed or in process
      sc1 = isDefect(file);
      sc2 = isProcessed(file);
      sc3 = inProcess(file);
      if (sc1 || sc2 || sc3)
            continue;	 

      
      if (counter >= m_minFileNum) {
  	  sc1 = setReady(run);
      }
      
      if (StatusCode::SUCCESS == isReady(run))
          sc1 = distributeFile(run,path_name);
      else
          continue;
           
  }
  
  // Empty the buffer containing the scanned file names.
  m_fileNames.swap(empty_buffer);
    
  FilePollerNFiles::start();


} 


/// Helper function for making the EventSelector available again.
StatusCode FilePollerNFiles::clearRun(const string run) {


    //Make the EventSelector available again.
    if (0 == m_CurRun.compare(run))
    	m_CurRun = "";

    return StatusCode::SUCCESS;
	
}


/// Helper function for the distribution of a file to the listener.
StatusCode FilePollerNFiles::distributeFile(const string run, const string path_name) {

	MsgStream log(msgSvc(), name());
		
	if ((0==(m_CurRun).compare("")) || (0==(m_CurRun).compare(run))) {
		StatusCode sc = ((IAlertSvc*)m_EvtSelector)->alertSvc(path_name);
       		sc = remListener();
		return sc;		
        }
	else 
		info(OnlineService::name()+": The EventSelector is occupied with a different run.");

	return StatusCode::SUCCESS;		 
		
}


/// Implementation of IOnlineBookkeepNFiles::getRunFileNumber.
string FilePollerNFiles::getRunFileNumber(const string file_path, const char* pattern) {
 
  regex_t regex;
  regmatch_t matchptr;
  MsgStream log(msgSvc(), name());

  if (!file_path.empty()) {
  	int status = regcomp(&regex, pattern, 0); 
        if (status) { 
           log << MSG::ERROR << "Could not compile regex." << endmsg;
      	   return "failure in regex!";		 
	}
	status = regexec(&regex, file_path.c_str(), 1, &matchptr, 0);
	regfree(&regex);
	int outOfBounds1 = ( ((matchptr.rm_so)<0)||((matchptr.rm_so)>file_path.length()) );
	int outOfBounds2 = ( ((matchptr.rm_eo)<0)||((matchptr.rm_eo)>file_path.length()) );
	if ( outOfBounds1 || outOfBounds2) { 
           log << MSG::ERROR << "No match of regex." << endmsg;
      	   return "failure in regex!";		 
	}
	return file_path.substr(matchptr.rm_so+1,matchptr.rm_eo-matchptr.rm_so-1);
  }
  else
	return file_path;
  
}

/// Implementation of IOnlineBookkeepNFiles::markBookKept.
StatusCode FilePollerNFiles::markBookKept(const std::string file, const int eventCnt) { //file is just the file name

  MsgStream log(msgSvc(), name());
  const char* patternR = "/[0-9]*_";
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "INSERT INTO FileRecords (FileName,RunNumber,TotalEvents,StatusFlag) VALUES (?,?,?,?);";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  run_no = strtol(getRunFileNumber("/"+file,patternR).c_str(),NULL,10);  // "/" to support the regexp and not cut off the run number
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,2,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,3,eventCnt);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,4,0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error inserting record into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeepNFiles::isBookKept.  
StatusCode FilePollerNFiles::isBookKept(const std::string file) {//file is just the file name
  
  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE FileName = ?; ";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising isBookKept query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising isBookKept query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising isBookKept query" + msg << endmsg;
        return StatusCode::FAILURE;
     }


  return StatusCode::SUCCESS;
  
}



/// Implementation of IOnlineBookkeepNFiles::isProcessed.  
StatusCode FilePollerNFiles::isProcessed(const std::string file) {//file is just the file name

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE FileName = ? AND StatusFlag = 1 ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising isProcessed query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising isProcessed query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising isProcessed query" + msg << endmsg;
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeepNFiles::inProcess.  
StatusCode FilePollerNFiles::inProcess(const std::string file) {//file is just the file name

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE FileName = ? AND StatusFlag = 2 ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising inProcess query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising inProcess query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising inProcess query" + msg << endmsg;
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}





/// Implementation of IOnlineBookkeepNFiles::isDefect.  
StatusCode FilePollerNFiles::isDefect(const std::string file) {
//arg is the actual file name, not whole path
  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE FileName = ? AND StatusFlag = -1 ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising isDefect query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising isDefect query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising isDefect query" + msg << endmsg;
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}




/// Implementation of IOnlineBookkeepNFiles::connectToDb.  
StatusCode FilePollerNFiles::connectToDb() {

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query1 = "CREATE TABLE IF NOT EXISTS FileRecords ( "\ 
                       "  FileName    TEXT    PRIMARY KEY NOT NULL,"\
                       "  RunNumber   INTEGER             NOT NULL,"\
                       "  TotalEvents INTEGER            NOT NULL,"\                
		       "  StatusFlag  INTEGER             NOT NULL);";
  

  const char *c_query1 = query1.c_str();
  
  string query2 = "CREATE TABLE IF NOT EXISTS RunRecords ( " \
                       "  RunNumber INTEGER  PRIMARY KEY NOT NULL,"\
                       "  FilesNum  INTEGER,"\      
		       "  Ready	    INTEGER			  );";
  

  const char *c_query2 = query2.c_str();


  status = sqlite3_open(FilePollerNFiles::m_nameOfDb.c_str(), &(FilePollerNFiles::m_FileInfo));
  if (!FilePollerNFiles::m_FileInfo) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error opening book-keeping database:" + msg << endmsg;
    sqlite3_close(FilePollerNFiles::m_FileInfo);
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query1, -1, &pstatement, 0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finishing the creation of the database." + msg << endmsg;
        return StatusCode::FAILURE;
     }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error creating table:" + msg << endmsg;
    return StatusCode::FAILURE;
  }
	
  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising create table query:" + msg << endmsg;
      return StatusCode::FAILURE;
  }


  // Creation of the second table.
status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query2, -1, &pstatement, 0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finishing the creation of the database." + msg << endmsg;
        return StatusCode::FAILURE;
     }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error creating table:" + msg << endmsg;
    return StatusCode::FAILURE;
  }
	
  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising create table query:" + msg << endmsg;
      return StatusCode::FAILURE;
  }



  
  return StatusCode::SUCCESS;
}



/// Auxiliary function for printing the contents of the DB.
int FilePollerNFiles::print_aux(void* /*data*/, int columnNum, char **argv, char** /*ColName*/) {
   
   int i;   
   for(i=0; i<columnNum; i++){
      cout << (argv[i] ? argv[i] : "NULL") << "  ----  ";
      
   }
   cout << endl;

   return 0;
}

  
 
/// Implementation of IOnlineBookkeepNFiles::PrintDB()
StatusCode FilePollerNFiles::printDB() {

  if (!FilePollerNFiles::m_FileInfo)
	return StatusCode::FAILURE;
  
  string query = "SELECT * FROM FileRecords;";
  const char* c_query = query.c_str();	  
  const char* data = "FileName                     Run         EventCnt  StatusFlag"; 
 
  cout << data << endl; 	
  
  int status = sqlite3_exec(FilePollerNFiles::m_FileInfo, c_query, FilePollerNFiles::print_aux, 0, 0);
  if (SQLITE_OK != status) 
	return StatusCode::FAILURE;
	

  string query2 = "SELECT * FROM RunRecords;";
  const char* c_query2 = query2.c_str();	  
  const char* data2 = "Run      FilesNum     Ready"; 
 
  cout << data2 << endl; 	
  
  status = sqlite3_exec(FilePollerNFiles::m_FileInfo, c_query2, FilePollerNFiles::print_aux, 0, 0);
  if (SQLITE_OK != status) 
	return StatusCode::FAILURE;


  return StatusCode::SUCCESS; 

}


/// Implementation of IOnlineBookkeepNFiles::updateStatus.
StatusCode FilePollerNFiles::updateStatus(const string file, int eventCnt) { //file is the whole file path

  MsgStream log(msgSvc(), name());
  const char* patternR = "/[0-9]*_";
  const char* pattern = "/[0-9]*_[0-9]*.raw";  
  int status;
  sqlite3_stmt *pstatement;
  string query = "UPDATE FileRecords SET StatusFlag = 1,TotalEvents = ?  WHERE FileName = ?;";
  const char *c_query = query.c_str();
  
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  
  
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
       return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  status = sqlite3_bind_int(pstatement,1,eventCnt); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,2,getRunFileNumber(file,pattern).c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error updating book-keeping database: "+ msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
  }

  string run = getRunFileNumber(file,patternR);
  status = decreaseCounter(run);
  if (0 == getCounter(run))
	status = decreaseCounter(run);

  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeepNFiles::SetInProcess.
StatusCode FilePollerNFiles::SetInProcess(const string file, const int flag) { //file arg is just file name, not whole path

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "UPDATE FileRecords SET StatusFlag = ? WHERE FileName = ? ;";
  const char *c_query = query.c_str();
  
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  
  
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising inProcess query" + msg << endmsg;
       return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  status = sqlite3_bind_int(pstatement,1,flag); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising inProcess query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,2,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error updating book-keeping database: "+ msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising inProcess query" + msg << endmsg;
      return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
  
}





/// Implementation of IOnlineBookkeepNFiles::increaseCounter. 
StatusCode FilePollerNFiles::increaseCounter(const string file) {//arg is the run

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "UPDATE RunRecords SET FilesNum = ? WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  
  int counter = getCounter(file);
  counter++;
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  
  
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising increaseCounter query" + msg << endmsg;
       return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  long run_no = strtol(file.c_str(),NULL,10);  
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,2,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising increaseCounter query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }


  status = sqlite3_bind_int(pstatement,1,counter); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising increaseCounter query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }


  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error updating RunRecords database: "+ msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeepNFiles::decreaseCounter.
StatusCode FilePollerNFiles::decreaseCounter(const string file) {//arg is the run

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "UPDATE RunRecords SET FilesNum = ? WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  
  int counter = getCounter(file);
  if (-2 == counter )
	return StatusCode::FAILURE;
  counter--;
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  
  
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising decreaseCounter query" + msg << endmsg;
       return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  long run_no = strtol(file.c_str(),NULL,10);  
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,2,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising decreaseCounter query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }


  status = sqlite3_bind_int(pstatement,1,counter); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising decreaseCounter query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }


  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error updating RunRecords database: "+ msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeepNFiles::getCounter.  
int FilePollerNFiles::getCounter(const std::string file) { //arg is the run number

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from RunRecords WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  int cnt = -2;
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return -2;
  }

  long run_no = strtol(file.c_str(),NULL,10);  
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return -2;
  }
  
  status = sqlite3_bind_int(pstatement,1,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising getCounter query" + msg << endmsg;
      return -2;
    }
    return -2;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying RunRecords database:" + msg << endmsg;
    return -2;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising getCounter query" + msg << endmsg;
        return -2;
     }
     return -2;
  }

  cnt = sqlite3_column_int(pstatement, 1);
  

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising getCounter query" + msg << endmsg;
       return -2;
  }


  return cnt;
  
}


/// Implementation of IOnlineBookkeepNFiles::insertRun.
StatusCode FilePollerNFiles::insertRun(const std::string run) {

  MsgStream log(msgSvc(), name());
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "INSERT INTO RunRecords (RunNumber,FilesNum,Ready) VALUES (?,?,?);";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  run_no = strtol(run.c_str(),NULL,10); 
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,1,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,2,1);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,3,0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error inserting record into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}




/// Implementation of IOnlineBookkeepNFiles::existsRun.
StatusCode FilePollerNFiles::existsRun(const std::string run) { //arg is the run number

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from RunRecords WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }

  long run_no = strtol(run.c_str(),NULL,10);  
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,1,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising existsRun query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying RunRecords database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising existsRun query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising existsRun query" + msg << endmsg;
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}  
  
/// Implementation of IOnlineBookkeepNFiles::isReady.
StatusCode FilePollerNFiles::isReady(const std::string run) { //arg is the run number

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from RunRecords WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }

  long run_no = strtol(run.c_str(),NULL,10);  
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,1,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising isReady query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error querying RunRecords database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
        log << MSG::ERROR << "Error finalising isReady query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }

  int flag = sqlite3_column_int(pstatement, 2);

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
       log << MSG::ERROR << "Error finalising isReady query" + msg << endmsg;
       return StatusCode::FAILURE;
  }
  
  
  if (0 == flag)
	return StatusCode::FAILURE;
  else
  	return StatusCode::SUCCESS;
  
}  
  

/// Implementation of IOnlineBookkeepNFiles::setReady.
StatusCode FilePollerNFiles::setReady(const std::string run) {

  MsgStream log(msgSvc(), name());
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "UPDATE RunRecords SET Ready = ? WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  run_no = strtol(run.c_str(),NULL,10); 
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,2,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,1,1);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error updating ready flag into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeepNFiles::setFinishedRun.
StatusCode FilePollerNFiles::setFinishedRun(const std::string run) {

  MsgStream log(msgSvc(), name());
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "UPDATE RunRecords SET FilesNum = -1 WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePollerNFiles::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  run_no = strtol(run.c_str(),NULL,10); 
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted." << endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,1,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
    log << MSG::ERROR << "Error updating ready flag into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePollerNFiles::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}
