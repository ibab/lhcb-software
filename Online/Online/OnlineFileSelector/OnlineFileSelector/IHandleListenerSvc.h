#ifndef ONLINEFILESELECTOR_IHANDLELISTENERSVC_H 
#define ONLINEFILESELECTOR_IHANDLELISTENERSVC_H 1

// Include files
// from STL
#include <string>
#include <vector>
// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "OnlineFileSelector/IAlertSvc.h"


static const InterfaceID IID_IHandleListenerSvc ( "IHandleListenerSvc", 1, 0 );

/** @class IHandleListenerSvc IHandleListenerSvc.h OnlineFileSelector/IHandleListenerSvc.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-02-05
 */
class IHandleListenerSvc : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IHandleListenerSvc; }
  
  virtual StatusCode addListener(IAlertSvc* ) = 0;
  virtual StatusCode remListener(IAlertSvc*) = 0;
  virtual const StatusCode showListeners() = 0;
  virtual StatusCode statusReport(StatusCode , std::string ) = 0;
  
  
  
  
};
#endif // ONLINEFILESELECTOR_IHANDLELISTENERSVC_H
