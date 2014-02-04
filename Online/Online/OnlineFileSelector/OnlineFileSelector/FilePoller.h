#ifndef ICHALKIA_FILEPOLLER_H 
#define ICHALKIA_FILEPOLLER_H 1

// Include files
#include "GaudiOnline/OnlineService.h"
#include "IDirectoryScanner.h"
#include "IAlarmHandler.h"
#include <vector>
#include <queue>
#include "dim.hxx"
/** @class FilePoller FilePoller.h ichalkia/FilePoller.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-23
 */
using namespace std;
using namespace LHCb;


class FilePoller :  public DimTimer,
                    public OnlineService,
                    virtual public IDirectoryScanner,
                    virtual public IAlarmHandler

{

public:

  ///Standard constructor.
  FilePoller(  /*const string& nam,
                 ISvcLocator* svc,*/
             string directory,
             int time
               );
  
  ///Copy constructor.
  FilePoller(const FilePoller&);

  ///Standard destructor.
  ~FilePoller();

  //IInterface, queryInterface implementation.
  //virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  
  ///IOnlineService, start the timer.
  /*virtual*/ StatusCode start();

  ///IOnlineService, stop the timer.
  /*virtual*/ StatusCode stop();
  
  ///IOnlineService initialize.
  virtual StatusCode initialize();

  ///IOnlineService finalize.
  virtual StatusCode finalize();
  
  ///Accessor methods.
  void setScanDirectory(string );  //keep it??
  void setAlrmTime(int );          //keep it??
  void addTofileNames(const string& );
  void remFromfileNames();

  ///IDimTimer timer handler method.
  virtual void timerHandler();

  ///IDirectoryScanner poll method.
  /*virtual*/ StatusCode poller(const string scan_path);
  /*
 ///IDirectoryScanner add a listener to the queue.
 virtual StatusCode addListener(IService* Listener);
  
 ///IDirectoryScanner remove a listener from the queue.
 virtual StatusCode remListener();

 ///IDirectoryScanner show the listeners waiting.
 virtual const StatusCode printListeners();  
 
 ///IAlarmHandler error response.
 virtual const StatusCode issueAlarm(const string& msg);
  */
private:


  ///Service's name.
  string m_name;//"OnlineFileSelSvc"  //needed ??

  ///Pointer to service locator;
  SmartIF<ISvcLocator>  m_pSvcLocator;  // needed ??

  ///The directory to be scanned.
  string m_scanDirectory;

  ///The period of the scanning process.
  int m_alrmTime;

  ///The names of the files found in the scanned directory.
  vector<string>  m_fileNames;

  ///The listeners waiting for the files.
  queue<ISvcLocator*> m_fileListeners;

};

#endif // ICHALKIA_FILEPOLLER_H
