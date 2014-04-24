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
  
  if (!lib_rtl_create_lock(0,&m_listenerLock))
    return issueAlarm("Problem creating listener list lock.");

  status = FilePoller::connectToDb();
  
  return status;
}

/// Implementation of IOnlineService::finalize()
StatusCode FilePoller::finalize()  {
  StatusCode statusP = OnlineService::finalize();
  
  if (!lib_rtl_delete_lock(m_listenerLock))
    return issueAlarm("Problem deleting listener list lock.");
  
  int status = sqlite3_close(FilePoller::m_FileInfo);
  if (SQLITE_OK != status) {
      	
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error closing database: " + msg);
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
const StatusCode FilePoller::issueAlarm(const string& msg) 
{
    error(msg);
    return StatusCode::FAILURE;
}

/// Implementation of polling method.
StatusCode FilePoller::poller(string scan_path)
{
  
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
    status = FilePoller::issueAlarm(OnlineService::name() + " calloc memory problem");
    return status; 
  }

  path = strncpy(path,scan_path.c_str(),1023);
  dir = opendir(path);
  
  if ( 0 == dir )  {
     return error("The directory "+scan_path+" is not valid!");
  }
  
  while ((0==readdir_r(dir,&d_entry,&d_res)) && (NULL!=d_res)) {
    
    prev_path = strncpy(prev_path,path,1023);
    if (!prev_path)
        return error("The directory prev_path is not valid!");
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
          status = FilePoller::issueAlarm(OnlineService::name()+": Error opening directory.");
          return status;
      }
      status = FilePoller::poller(n_path);
      if (StatusCode::FAILURE == status) {
         status = FilePoller::issueAlarm(OnlineService::name()+": The poller encountered an error while recursing.");
         return status;
      }
      i = closedir(nested_dir);
      if (-1 == i){
         status = FilePoller::issueAlarm(OnlineService::name()+": Problem closing directory.");
        return status;
      }
    }
    
    path = strncpy(path,prev_path,1023);
    if (!path)
        return error("The directory path is not valid!");
  }
  
  return StatusCode::SUCCESS;
}

/// Implementation of IHandleListenerSvc::addListener.
StatusCode FilePoller::addListener(IAlertSvc* Listener) 
{
  if (lib_rtl_lock(m_listenerLock)) {
      m_fileListeners.push_back(Listener);
  }
  else
      return FilePoller::issueAlarm("Problem acquiring the lock.");
  if (!lib_rtl_unlock(m_listenerLock))
      return FilePoller::issueAlarm("Problem releasing the lock.");
  return StatusCode::SUCCESS;
}

/// Implementation of IHandleListenerSvc::remListener.
StatusCode FilePoller::remListener(IAlertSvc* Listener)
{
  deque<IAlertSvc*>::iterator iter;
  
  if (lib_rtl_lock(m_listenerLock)) {
      
   if (!m_fileListeners.empty())
   {
     iter = find(m_fileListeners.begin(),m_fileListeners.end(),Listener);
     if (*iter == m_fileListeners.front()) 
         m_fileListeners.pop_front();
     else if (iter == m_fileListeners.end())
         return FilePoller::issueAlarm("Logic bug: remove non-existent listener");
     else
         m_fileListeners.erase(iter);
   }
   else 
     info(OnlineService::name()+": No more listeners available.");
   if (!lib_rtl_unlock(m_listenerLock))
       return FilePoller::issueAlarm("Problem releasing the lock.");
   return StatusCode::SUCCESS;
  }
  return FilePoller::issueAlarm("Problem acquiring the lock.");
}


/// Implementation of IHandleListenerSvc::showListeners.
const StatusCode FilePoller::showListeners()
{
  IAlertSvc* Listener; 
  deque<IAlertSvc*>::iterator iter;
  
  
  if (!m_fileListeners.empty())
  {
   
    for(iter = m_fileListeners.begin(); iter != m_fileListeners.end(); ++iter)
    {
      Listener = *iter;
      if (!Listener)
        return FilePoller::issueAlarm(OnlineService::name()+": Error retrieving service");
        cout << Listener->getSvcName() << endl;
    }
    
    
    return StatusCode::SUCCESS;
  }
  FilePoller::issueAlarm("No listeners at the moment.");
  return StatusCode::FAILURE;
  
}


void FilePoller::addTofileNames(const string& newFile)
{
  m_fileNames.push_back(newFile);
}


string FilePoller::remFromfileNames()
{
  if (!m_fileNames.empty())
  {
    string path_file = m_fileNames.front();
    m_fileNames.pop_front();
    return path_file;
  }
  else 
  {
    issueAlarm("No available files.");
    return "";
  }
}

  
/// Implementation of IHandleListenerSvc::statusReport.
StatusCode FilePoller::statusReport(StatusCode status, const string file)
{
  if (StatusCode::FAILURE == status)
  {
    // no book-keeping
    info(OnlineService::name()+": The recipient encountered a problem while reseting the criteria.");
  }
  else if (StatusCode::SUCCESS == status)
  {
    // book-keep the file
    const char* pattern = "/[0-9]*_[0-9]*.raw";  
    StatusCode sc = markBookKept(getRunFileNumber(file,pattern),0);
  }
  
  return status;

}


/// Implementation of DimTimer::timerHandler.
void FilePoller::timerHandler()
{
  StatusCode sc;
  string path_name;
  deque<string> empty_buffer; 
  const char* pattern = "/[0-9]*_[0-9]*.raw"; 
  DimTimer::stop();
  StatusCode poll_st = FilePoller::poller(FilePoller::m_scanDirectory);
  poll_st = showListeners();
  poll_st = printDB();

  while (!m_fileListeners.empty() && !m_fileNames.empty()) {
    
      path_name = remFromfileNames();
      //check if it is book-kept
      sc = isBookKept(getRunFileNumber(path_name,pattern));
      //if not/yes+StFlag==0, alertSvc(), if yes, dismiss and pick next file
      if (StatusCode::SUCCESS == sc) {
	 if (StatusCode::SUCCESS == isProcessed(getRunFileNumber(path_name,pattern)))
	     continue;	  
      }
      else {
        sc = ((IAlertSvc*)m_fileListeners.front())->alertSvc(path_name);
        sc = remListener(m_fileListeners.front());
      }

  }
  
  // Empty the buffer containing the scanned file names.
  m_fileNames.swap(empty_buffer);
    
  FilePoller::start();

} 






/// Implementation of IOnlineBookkeep::getRunFileNumber.
string FilePoller::getRunFileNumber(const string file_path, const char* pattern) {
 
  regex_t regex;
  regmatch_t matchptr;
  
  if (!file_path.empty()) {
  	int status = regcomp(&regex, pattern, 0); //check flags
        if (status) { 
           issueAlarm("Could not compile regex\n");
      	   return "failure in regex!";		 
	}
	status = regexec(&regex, file_path.c_str(), 1, &matchptr, 0);
	regfree(&regex);
	int outOfBounds1 = ( ((matchptr.rm_so)<0)||((matchptr.rm_so)>file_path.length()) );
	int outOfBounds2 = ( ((matchptr.rm_eo)<0)||((matchptr.rm_eo)>file_path.length()) );
	if ( outOfBounds1 || outOfBounds2) { 
           issueAlarm("No match of regexp\n");
      	   return "failure in regex!";		 
	}
	return file_path.substr(matchptr.rm_so+1,matchptr.rm_eo-matchptr.rm_so-1);
  }
  else
	return file_path;
  
}

/// Implementation of IOnlineBookkeep::markBookKept.
StatusCode FilePoller::markBookKept(const std::string file, const int eventCnt) {

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
      issueAlarm("Error finalising insert query: " + msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_text(pstatement,1,file.c_str(),-1,0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising insert query: " + msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  
  run_no = strtol(getRunFileNumber("/"+file,patternR).c_str(),NULL,10);  // "/" to support the regexp and not cut off the run number
  if (ERANGE == errno) {
	issueAlarm("Run number was wrongfully converted.");
	return StatusCode::FAILURE;
  }
  
  status = sqlite3_bind_int(pstatement,2,run_no); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising insert query: " + msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,3,eventCnt);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising insert query: " + msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }

  status = sqlite3_bind_int(pstatement,4,0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising insert query: " + msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    issueAlarm("Error inserting record into book-keeping database: " + msg);
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising insert query: " + msg);
      return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
  
}


/// Implementation of IOnlineBookkeep::isBookKept.  
StatusCode FilePoller::isBookKept(const std::string file) {
  
  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE FileName = '" +file+"'; ";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    issueAlarm("Error querying book-keeping database: " + msg);
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        issueAlarm("Error finalising isBookKept query: " +  msg);
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        issueAlarm("Error finalising isBookKept query: " +  msg);
        return StatusCode::FAILURE;
     }


  return StatusCode::SUCCESS;
  
}



/// Implementation of IOnlineBookkeep::isProcessed.  
StatusCode FilePoller::isProcessed(const std::string file) {

  int status;
  sqlite3_stmt *pstatement;
  string query = "SELECT * from FileRecords WHERE FileName = '" +file+"' AND StatusFlag = 1 ;";
  const char *c_query = query.c_str();
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    issueAlarm("Error querying book-keeping database: " + msg);
    return StatusCode::FAILURE;
  }

  if (SQLITE_ROW != status) {  
     status = sqlite3_finalize(pstatement);
     if (SQLITE_OK != status) {
        string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
        issueAlarm("Error finalising isBookKept query: " +  msg);
        return StatusCode::FAILURE;
     }
     return StatusCode::FAILURE;
  }


  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
       issueAlarm("Error finalising isBookKept query: " +  msg);
       return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}






/// Implementation of IOnlineBookkeep::connectToDb.  
StatusCode FilePoller::connectToDb() {

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
    issueAlarm("Error opening book-keeping database: " + msg);
    sqlite3_close(FilePoller::m_FileInfo);
    return StatusCode::FAILURE;
  }
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0);
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    return StatusCode::FAILURE;
  }

  status = sqlite3_step(pstatement); 
  if (SQLITE_DONE != status) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    issueAlarm("Error creating table: " + msg);
  }
	
  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising create table query: " + msg);
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

  int status;
  const char* pattern = "/[0-9]*_[0-9]*.raw";  
  sqlite3_stmt *pstatement;
  string query = "UPDATE FileRecords SET StatusFlag = 1, TotalEvents = ? WHERE FileName = '" + getRunFileNumber(file,pattern) +"';";
  const char *c_query = query.c_str();
  
  
  status = sqlite3_prepare_v2(FilePoller::m_FileInfo, c_query, -1, &pstatement, 0); 
  
  
  
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
       string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
       issueAlarm("Error finalising update query: " + msg);
       return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  

  status = sqlite3_bind_int(pstatement,1,eventCnt); 
  if (SQLITE_OK != status) {
    status = sqlite3_finalize(pstatement);
    if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising update query: " + msg);
      return StatusCode::FAILURE;
    }
    return StatusCode::FAILURE;
  }
  
  
  status = sqlite3_step(pstatement); 
  if ((SQLITE_DONE != status) && (SQLITE_ROW != status)) {
    string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
    issueAlarm("Error updating book-keeping database: " + msg);
    return StatusCode::FAILURE;
  }

  status = sqlite3_finalize(pstatement);
  if (SQLITE_OK != status) {
      string msg = sqlite3_errmsg(FilePoller::m_FileInfo);
      issueAlarm("Error finalising update query: " + msg);
      return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
  
}




