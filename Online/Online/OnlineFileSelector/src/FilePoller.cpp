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
#include "GaudiKernel/SvcFactory.h"
#include <string>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FilePoller)

using namespace std;
using namespace LHCb;
using namespace boost;

FilePoller::FilePoller(const string& nam,ISvcLocator* svc)
                           : OnlineService::OnlineService(nam,svc),
                           m_scanDirectory(),
                           m_alrmTime()
{
  declareProperty("scanDirectory",m_scanDirectory);
  declareProperty("alarmTime",m_alrmTime);
  declareProperty("DbName",m_nameOfDb);

  
}

FilePoller::~FilePoller() {}


/// Implementation of IInterface::queryInterface()
StatusCode FilePoller::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if(IHandleListenerSvc::interfaceID().versionMatch(riid))
    *ppvIF = (IHandleListenerSvc*)this;
  else if(IAlarmHandler::interfaceID().versionMatch(riid))
    *ppvIF = (IAlarmHandler*)this;
  else if(IOnlineBookkeep::interfaceID().versionMatch(riid))
    *ppvIF = (IOnlineBookkeep*)this;
  else
    return OnlineService::queryInterface(riid, ppvIF);
  addRef();
  return StatusCode::SUCCESS;
}



/// Implementation of IOnlineService::initialize()
StatusCode FilePoller::initialize()  {

  StatusCode status = OnlineService::initialize();
  MsgStream log(msgSvc(), name());
  
  status = FilePoller::connectToDb();
  
  return status;
}

/// Implementation of IOnlineService::finalize()
StatusCode FilePoller::finalize()  {
  StatusCode statusP = OnlineService::finalize();
  MsgStream log(msgSvc(), name());
      
  
  int status = sqlite3_close(FilePoller::m_FileInfo);
  if (SQLITE_OK != status) {
      	
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error closing database" + msg << endmsg;    
	
      return StatusCode::FAILURE;
  }

  return statusP;
}


/// Implementation of IOnlineService::start()
StatusCode FilePoller::start()
{
  StatusCode status = OnlineService::start();
  DimTimer::start(FilePoller::m_alrmTime);
  return status;
}

/// Implementation of IOnlineService::stop()
StatusCode FilePoller::stop()
{
  StatusCode status = OnlineService::stop();
  DimTimer::stop();
  return status;
}


/// Implementation of IAlarmHandler.
const StatusCode FilePoller::issueAlarm(const string& file) 
{//file is the actual file name, not whole path
  MsgStream log(msgSvc(), name());
  const char* patternR = "/[0-9]*_";
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "INSERT INTO FileRecords (FileName,RunNumber,TotalEvents,StatusFlag) VALUES (?,?,-1,-1);";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
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
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error inserting record into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;	    
	

}


/// Implementation of polling method.
StatusCode FilePoller::poller(string scan_path)
{
  MsgStream log(msgSvc(), name());
  struct dirent d_entry;                  	
  struct dirent *d_res;
  int i;	
  DIR *dir,*nested_dir;
  char *path,*prev_path;
  StatusCode status;
  string n_path,dname;
  
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
        FilePoller::addTofileNames(n_path + "/" + dname);
	
      }
      
    }
    else {

      n_path = n_path + "/" + dname;
      nested_dir = opendir(n_path.c_str());
      
      if (!nested_dir) {
          log << MSG::ERROR << "Error opening nested directory." << endmsg;
          return StatusCode::FAILURE;
      }
      status = FilePoller::poller(n_path);
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
StatusCode FilePoller::addListener() 
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
StatusCode FilePoller::remListener()
{
  MsgStream log(msgSvc(), name());
  
  if (m_EvtSelector){
	m_EvtSelector = 0;
        info(OnlineService::name()+": Listener removed.");
        return StatusCode::SUCCESS;
  }
  else {
	info(OnlineService::name()+": No listener available.");
        return StatusCode::FAILURE;
  }

}


/// Implementation of IHandleListenerSvc::showListeners.
const StatusCode FilePoller::showListeners()
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


void FilePoller::addTofileNames(const string& newFile)
{
  m_fileNames.push_back(newFile);
}


string FilePoller::remFromfileNames()
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
StatusCode FilePoller::statusReport(StatusCode status, const string file)
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
    // book-keep the file
    StatusCode sc = markBookKept(file_regexp,0);
  }
  
  return status;

}


/// Implementation of DimTimer::timerHandler.
void FilePoller::timerHandler()
{
  StatusCode sc,sc1,sc2;
  string path_name;
  deque<string> empty_buffer; 
  const char* patternR = "/[0-9]*_"; 

  //DimTimer::stop();
  StatusCode poll_st = FilePoller::poller(FilePoller::m_scanDirectory);
  poll_st = showListeners();
  poll_st = printDB();
  
  while (m_EvtSelector && !m_fileNames.empty()) {
    
      path_name = remFromfileNames();
      string run = getRunFileNumber(path_name,patternR);
      //check if it is faulty, has already been processed or is currently being processed
      sc = isDefect(run);
      sc1 = isProcessed(run);
      sc2 = isBookKept(run); //is currently being processed
      if (sc || sc1 || sc2)	
	 continue;

	

      sc = ((IAlertSvc*)m_EvtSelector)->alertSvc(path_name);
      sc = remListener();
      
  }
  
  // Empty the buffer containing the scanned file names.
  m_fileNames.swap(empty_buffer);
    
  FilePoller::start();

} 






/// Implementation of IOnlineBookkeep::getRunFileNumber.
string FilePoller::getRunFileNumber(const string file_path, const char* pattern) {
 
  regex_t regex;
  regmatch_t matchptr;
  MsgStream log(msgSvc(), name());

  if (!file_path.empty()) {
  	int status = regcomp(&regex, pattern, 0); //check flags
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

/// Implementation of IOnlineBookkeep::markBookKept.
StatusCode FilePoller::markBookKept(const std::string file, const int eventCnt) { //file is just the file name


  MsgStream log(msgSvc(), name());
  const char* patternR = "/[0-9]*_";
  long run_no = 0;
  int status;
  sqlite3_stmt *pstatement;
  string query = "INSERT INTO FileRecords (FileName,RunNumber,TotalEvents,StatusFlag) VALUES (?,?,?,?);";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0);
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
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
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,3,eventCnt);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,4,0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error inserting record into book-keeping database: " + msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising insert query" + msg << endmsg;
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeep::isBookKept.  
StatusCode FilePoller::isBookKept(const std::string run) {
  
  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE RunNumber = ? ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }
  

  long run_no = strtol(run.c_str(),NULL,10);  
  if (ERANGE == errno) {
	log << MSG::ERROR << "Run number was wrongfully converted."<< endmsg;
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,1,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising isBookKept query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }


  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        log << MSG::ERROR << "Error finalising isBookKept query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        log << MSG::ERROR << "Error finalising isBookKept query" + msg << endmsg;
        return StatusCode::FAILURE;
     }


  return StatusCode::SUCCESS;
  
}



/// Implementation of IOnlineBookkeep::isProcessed.  
StatusCode FilePoller::isProcessed(const std::string run) {

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE RunNumber = ? AND StatusFlag = 1 ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
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
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising isProcessed query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        log << MSG::ERROR << "Error finalising isProcessed query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
       log << MSG::ERROR << "Error finalising isProcessed query" + msg << endmsg;
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}



/// Implementation of IOnlineBookkeep::isDefect.  
StatusCode FilePoller::isDefect(const std::string run) {

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE RunNumber = ? AND StatusFlag = -1 ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
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
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising isDefect query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error querying book-keeping database:" + msg << endmsg;
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        log << MSG::ERROR << "Error finalising isDefect query" + msg << endmsg;
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
       log << MSG::ERROR << "Error finalising isDefect query" + msg << endmsg;
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}




/// Implementation of IOnlineBookkeep::connectToDb.  
StatusCode FilePoller::connectToDb() {

  MsgStream log(msgSvc(), name());
  int status;
  sqlite3_stmt *pstatement;
  string query = "CREATE TABLE IF NOT EXISTS FileRecords ( " \
                       "  FileName    TEXT    PRIMARY KEY NOT NULL," \
                       "  RunNumber   INTEGER             NOT NULL," \
                       "  TotalEvents INTEGER             NOT NULL," \
                       "  StatusFlag  INTEGER             NOT NULL);";
  

  const char *c_query = query.c_str();
  

  status = sqlite3_open(FilePoller::m_nameOfDb.c_str(), &(FilePoller::m_FileInfo));
  if (!FilePoller::m_FileInfo) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error opening book-keeping database:" + msg << endmsg;
    sqlite3_close(FilePoller::m_FileInfo);
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        log << MSG::ERROR << "Error finishing the creation of the database." + msg << endmsg;
        return StatusCode::FAILURE;
     }
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error creating table:" + msg << endmsg;
    return StatusCode::FAILURE;
  }
	
  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising create table query:" + msg << endmsg;
      return StatusCode::FAILURE;
  }

  
  return StatusCode::SUCCESS;
}



/// Auxiliary function for printing the contents of the DB.
int FilePoller::print_aux(void* /*data*/, int columnNum, char **argv, char** /*ColName*/) {
   
   int i;   
   for(i=0; i<columnNum; i++){
      cout << (argv[i] ? argv[i] : "NULL") << "  ----  ";
      
   }
   cout << endl;

   return 0;
}

  
 
/// Implementation of IOnlineBookkeep::PrintDB()
StatusCode FilePoller::printDB() {

  if (!FilePoller::m_FileInfo)
	return StatusCode::FAILURE;
  
  string query = "SELECT * FROM FileRecords;";
  const char* c_query = query.c_str();	  
  const char* data = "FileName                     Run         EventCnt  StatusFlag"; 
 
  cout << data << endl; 	
  
  int status = sqlite3_exec(FilePoller::m_FileInfo, c_query, FilePoller::print_aux, 0, 0);
  if (SQLITE_OK != status) 
	return StatusCode::FAILURE;
	

  return StatusCode::SUCCESS; 

}


StatusCode FilePoller::updateStatus(const string file, int eventCnt) {
//file the whole file path
  MsgStream log(msgSvc(), name());
  int status;
  const char* pattern = "/[0-9]*_[0-9]*.raw";  
  sqlite3_stmt *pstatement;
  string query = "UPDATE FileRecords SET StatusFlag = 1, TotalEvents = ? WHERE FileName = ? ;";
  const char *c_query = query.c_str();
  
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
    
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
       log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
       return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  status = sqlite3_bind_int(pstatement,1,eventCnt); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,2,getRunFileNumber(file,pattern).c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    log << MSG::ERROR << "Error updating book-keeping database: "+ msg << endmsg;
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      log << MSG::ERROR << "Error finalising update query" + msg << endmsg;
      return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}




