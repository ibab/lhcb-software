#ifndef ICHALKIA_IALARMHANDLER_H 
#define ICHALKIA_IALARMHANDLER_H 1

// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/IInterface.h"

using namespace std;


static const InterfaceID IID_IAlarmHandler ( "IAlarmHandler",1, 0 );

/** @class IAlarmHandler IAlarmHandler.h ichalkia/IAlarmHandler.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-24
 */

class IAlarmHandler : virtual public IInterface {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlarmHandler; }

  virtual const StatusCode issueAlarm(const string& msg) = 0;
  

};
#endif // ICHALKIA_IALARMHANDLER_H
