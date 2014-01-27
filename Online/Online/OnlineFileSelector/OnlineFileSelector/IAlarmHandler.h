#ifndef ICHALKIA_IALARMHANDLER_H 
#define ICHALKIA_IALARMHANDLER_H 1

// Include files
// from STL
#include <string>
#include <cstdarg>
#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/MsgStream.h"

static const InterfaceID IID_IAlarmHandler ( "IAlarmHandler", 1, 0 );

/** @class IAlarmHandler IAlarmHandler.h ichalkia/IAlarmHandler.h
 *  
 *
 *  @author Ioannis Chalkiadakis
 *  @date   2014-01-24
 */

using namespace std

class IAlarmHandler : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlarmHandler; }

  virtual const StatusCode issueAlarm(const char* source,
                                      const char* message,...)
  {
      if ( err.isActive() )  {
        va_list args;
        va_start(args, source);
        char buff[1024];
        size_t nSize = vsnprintf(buff, sizeof(buff), source, args);
        err << buff << endmsg;
        if ( nSize == sizeof(buff) )  {
          err << MSG::FATAL << "Incomplete message - buffer overrun." << endreq;
        }
      }
      return StatusCode::FAILURE;
      
  }
  

};
#endif // ICHALKIA_IALARMHANDLER_H
