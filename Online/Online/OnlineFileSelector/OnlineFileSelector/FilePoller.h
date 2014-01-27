#ifndef ICHALKIA_FILEPOLLER_H 
#define ICHALKIA_FILEPOLLER_H 1

// Include files
#include "IDirectoryScanner.h"
#include "IAlarmHandler.h"
/** @class FilePoller FilePoller.h ichalkia/FilePoller.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-23
 */

///Register to various services (e.g.AlarmHandler,EventSelector etc)
StatusCode LHCb::OnlineService::initialize();

///Unregister from the subscribed services.
StatusCode LHCb::OnlineService::finalize();

///Constructor.
FilePoller::FilePoller(string ,
                       int
                       )

///Copy constructor.
FilePoller::FilePoller(const FilePoller& );

///Destructor.
virtual ~FilePoller::FilePoller();

///Start the timer.
StatusCode start();

///Stop the timer.
StatusCode stop();

///Accessor methods to directory name and alarm time.
void setScanDirectory(string );
void setAlrmTime(int );

class DTimer;
const string m_scanDirectory;
const int m_alrmTime;
DTimer m_timerUtil;
vector<string> m_fileNames;
queue<OnlineService*> m_fileListeners;


#endif // ICHALKIA_FILEPOLLER_H
