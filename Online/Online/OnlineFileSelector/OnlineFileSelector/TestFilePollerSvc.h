#ifndef ONLINEFILESELECTOR_TESTFILEPOLLERSVC_H 
#define ONLINEFILESELECTOR_TESTFILEPOLLERSVC_H 1

// Include files
#include "GaudiOnline/OnlineService.h"
#include "OnlineFileSelector/IAlertSvc.h"
#include "OnlineFileSelector/IHandleListenerSvc.h"
#include <string>
/** @class TestFilePollerSvc TestFilePollerSvc.h OnlineFileSelector/TestFilePollerSvc.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-02-06
 */

namespace LHCb 
{
  
  class TestFilePollerSvc : public OnlineService,
                            virtual public IAlertSvc {
  public: 
    /// Standard constructor
    TestFilePollerSvc(const std::string& name, ISvcLocator*svc );
    
    virtual ~TestFilePollerSvc( ); ///< Destructor
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    
    // Subscribe to FilePoller , i.e. get pointer to it.
    virtual StatusCode initialize();
    
    // Unsubscribe from FilePoller.
    virtual StatusCode finalize();

    // IAlertSvc implementation.
    virtual StatusCode alertSvc(const std::string&);
    
    virtual void setSvcName(const std::string&);
  
    virtual std::string getSvcName();
  
    IHandleListenerSvc* m_filePoller;
    
  };

}

#endif // ONLINEFILESELECTOR_TESTFILEPOLLERSVC_H
