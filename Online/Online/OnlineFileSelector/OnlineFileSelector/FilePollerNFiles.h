#ifndef ICHALKIA_FILEPOLLERNFILES_H 
#define ICHALKIA_FILEPOLLERNFILES_H 1

// Include files
#include "GaudiOnline/OnlineService.h"
#include "IHandleListenerSvc.h"
#include "IAlarmHandler.h"
#include "IOnlineBookkeepNFiles.h"
#include "RTL/rtl.h"
#include <deque>
#include <map>
#include "dim.hxx"
#include <regex.h>
#include <sqlite3.h>    


namespace LHCb  {
  
  /** @class FilePoller FilePollerNFiles.h ichalkia/FilePollerNFiles.h
   *  
   *
   *  @author Ioannis Chalkiadakis
   *  @date   2014-01-23
   */
  class FilePollerNFiles :  virtual public DimTimer,
                      public OnlineService,
                      virtual public IHandleListenerSvc,
                      virtual public IAlarmHandler,
                      virtual public IOnlineBookkeepNFiles

  {

  public:

    /// Standard constructor.
    FilePollerNFiles(const std::string& nam, ISvcLocator* svc);
  
    /// Standard destructor.
    ~FilePollerNFiles();

    // IInterface, queryInterface implementation.
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
    /// IOnlineService, start the timer.
    virtual StatusCode start();

    /// IOnlineService, stop the timer.
    virtual StatusCode stop();
  
    ///IOnlineService initialize.
    virtual StatusCode initialize();

    ///IOnlineService finalize.
    virtual StatusCode finalize();
  
    /// Accessor methods.
    void addTofileNames(const std::string& );
    std::string remFromfileNames();
 
    /// IDimTimer timer handler method.
    virtual void timerHandler();

    /// Poll method.
    StatusCode poller(const std::string scan_path);

    /// Helper function for the distribution of a file to the listeners.
    StatusCode distributeFile(const std::string run, const std::string path_name);

    /// Helper function for making the EventSelector available again.
    StatusCode clearRun(const std::string );
	

    /// IHandleListenerSvc add a listener.
    virtual StatusCode addListener();

    /// IHandleListenerSvc remove a listener from the queue.
    virtual StatusCode remListener();

    /// IHandleListenerSvc show the listeners waiting.
    virtual const StatusCode showListeners();  
   
    /// IHandlerListenerSvc statusReport.
    virtual StatusCode statusReport(StatusCode status,const std::string file);
    
    /// IAlarmHandler error response.
    virtual const StatusCode issueAlarm(const std::string& msg);
  
    /// IOnlineBookkeep get the run number from file path implementation
    virtual std::string getRunFileNumber(const std::string file_path, const char* pattern);

    /// IOnlineBookkeep book-keep an incoming file.
    virtual StatusCode markBookKept(const std::string file, const int eventCnt);

    /// IOnlineBookkeep check if a file is being processed.
    virtual StatusCode isBookKept(const std::string file);

    /// IOnlineBookkeep update the status flag of and the events read from a file.
    virtual StatusCode updateStatus(const std::string, int );

    /// IOnlineBookkeep connect to the database.
    virtual StatusCode connectToDb();
    
    /// IOnlineBookkeep print the database.
    virtual StatusCode printDB();

    /// Auxiliary function for printing DB.
    static int print_aux(void *data, int argc, char **argv, char **ColName);

    /// IOnlineBookkeep check if a file has been processed.
    virtual StatusCode isProcessed(const std::string );
    
    /// IOnlineBookkeep check if a file is considered erroneous.
    virtual StatusCode isDefect(const std::string );  

    /// IOnlineBookkeep increase the file counter of a run.
    virtual StatusCode increaseCounter(const std::string );  

    /// IOnlineBookkeep decrease the file counter of a run.
    virtual StatusCode decreaseCounter(const std::string );  

    /// IOnlineBookkeep get the file counter of a run.
    virtual int getCounter(const std::string ); 
  
    /// IOnlineBookkeep insert the run in the database and initialize file counter to 0.
    virtual StatusCode insertRun(const std::string );
    
    /// IOnlineBookkeep check if the run exists in the database.
    virtual StatusCode existsRun(const std::string );
  
    /// IOnlineBookkeep set a file as being processed.
    virtual StatusCode SetInProcess(const std::string , const int);
   
    /// IOnlineBookkeep check if a file is being processed.  
    virtual StatusCode inProcess(const std::string );
   
    /// IOnlineBookkeep check if we have the necessary number of files to process the run.
    virtual StatusCode isReady(const std::string );

    /// Implementation of IOnlineBookkeep::setReady.
    virtual StatusCode setReady(const std::string );

    /// Implementation of IOnlineBookkeepNFiles::setFinishedRun.
    virtual StatusCode setFinishedRun(const std::string ); 


    /// The listener waiting for the files.
    mutable IAlertSvc* m_EvtSelector;    


 private:


    /// The directory to be scanned.
    std::string m_scanDirectory;

    /// The period of the scanning process.
    int m_alrmTime;

    /// The names of the files found in the scanned directory.
    std::deque<std::string>  m_fileNames;

    /// The current run that the EventSelector is processing.
    std::string m_CurRun;

    /// SQLite database handler.
    sqlite3* m_FileInfo;

    /// Name of the SQLite database.
    std::string m_nameOfDb;

    /// The minimum number of files needed to start processing the run.
    int m_minFileNum;    

  };
}


#endif // ICHALKIA_FILEPOLLERNFILES_H
