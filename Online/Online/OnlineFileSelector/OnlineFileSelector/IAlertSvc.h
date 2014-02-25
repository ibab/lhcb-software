#ifndef ONLINEFILESELECTOR_IALERTSVC_H 
#define ONLINEFILESELECTOR_IALERTSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

static const InterfaceID IID_IAlertSvc ( "IAlertSvc", 1, 0 );

/** @class IAlertSvc IAlertSvc.h OnlineFileSelector/IAlertSvc.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-02-05
 */
class IAlertSvc : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlertSvc; }

  virtual StatusCode alertSvc(const std::string&) = 0;
 
  // Set/Get the service's name so that the "alerter" can print it.
  virtual void setSvcName(const std::string& svc_name ) = 0;
  
  virtual std::string getSvcName() = 0;

protected:
  std::string m_listener_name;
  

};
#endif // ONLINEFILESELECTOR_IALERTSVC_H
