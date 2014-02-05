#ifndef ICHALKIA_FILEPOLLER_H 
#define ICHALKIA_FILEPOLLER_H 1

// Include files
#include "GaudiOnline/OnlineService.h"
#include "IHandleListenerSvc.h"
#include "IAlarmHandler.h"
#include <vector>
#include "dim.hxx"

/** @class FilePoller FilePoller.h ichalkia/FilePoller.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-23
 */
using namespace std;
using namespace LHCb;


class FilePoller :  virtual public DimTimer,
                    public OnlineService,
                    virtual public IHandleListenerSvc,
                    virtual public IAlarmHandler

{

public:

  /// Standard constructor.
  FilePoller(const string& nam,ISvcLocator* svc);
  
  /// Standard destructor.
  ~FilePoller();

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
  void addTofileNames(const string& );
  void remFromfileNames();
 
  /// IDimTimer timer handler method.
  virtual void timerHandler();

  /// Poll method.
  StatusCode poller(const string scan_path);

  /// IHandleListenerSvc add a listener to the queue.
 virtual StatusCode addListener(IAlertSvc* Listener);

 /// IHandleListenerSvc find a specific listener.
 virtual vector<IAlertSvc*>::iterator findListener(IAlertSvc* Listener);
 
 /// IHandleListenerSvc remove a listener from the queue.
 virtual StatusCode remListener(IAlertSvc* Listener);

 /// IHandleListenerSvc show the listeners waiting.
 virtual const StatusCode showListeners();  

 /// IAlarmHandler error response.
 virtual const StatusCode issueAlarm(const string& msg);
  

private:


 ///The directory to be scanned.
  string m_scanDirectory;

  ///The period of the scanning process.
  int m_alrmTime;

  ///The names of the files found in the scanned directory.
  vector<string>  m_fileNames;

  ///The listeners waiting for the files.
  vector<IAlertSvc*> m_fileListeners; ////change ISVCLocator

};

#endif // ICHALKIA_FILEPOLLER_H
