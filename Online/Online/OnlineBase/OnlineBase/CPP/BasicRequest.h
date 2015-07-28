#ifndef __BASIC_REQUEST_LOADED_H
#define __BASIC_REQUEST_LOADED_H
#include "RTL/NoMemberAlignment.h"
#include "CPP/BasicQueue.h"

class Worker;

/// Basic interface definition of objects containing a request, which can be served by threads
/** @class BasicRequest
 *  virtual BasicRequest class:
 *  - Handle() function known how to process the request.
 *
 *  @author  M.Frank
 *
 */
class BasicRequest : public BasicQueue<BasicRequest> {
  //@Man: Friends
  /// Worker class needs eventually access to internal structure
  friend class Worker;
protected:
  //@Man: Protected member variables
  /// Request type
  int __RequestType;
public:
  //@Man: Class specific enumerations
  /// Request type: User request should have an ID >= USER_REQUEST
  enum RequestType {
    CLEANUP_REQUEST,
    EXIT_REQUEST,
    WORK_REQUEST,
    ADD_REQUEST,
    REMOVE_REQUEST,
    IDLE_REQUEST,
    ACTIVE_REQUEST,
    USER_REQUEST
  };

  //@Man: Public member functions
  ///  Standard Constructor: Returns reference to BasicRequest instance
  explicit BasicRequest( int typ = WORK_REQUEST ) : __RequestType(typ) {
  }
  ///  Standard Destructor
  virtual ~BasicRequest() {
  }
  /// Virtual template function handle: called to work down the request
  virtual void Handle() {
    delete this;
  }
  /// Return the type of the request
  int Type(void) const {
    return __RequestType;
  }
  /// Virtual template function: Return pointer to request data
  virtual unsigned char *Buffer() {
    return  0;
  }
  /// Virtual template function: Maximal buffer size
  virtual unsigned int  MaxBuffSize() const {
    return  0;
  }
  /// Virtual template function: Return current buffer size
  virtual unsigned int  BuffSize() const {
    return  0;
  }
  /// Virtual template function: Shrink artificially buffer size
  virtual unsigned int  SetBuffSize(unsigned int /* len */ = 0xFFFFFFF) {
    return  0;
  }
};
#include "RTL/RestoreMemberAlignment.h"
#endif
