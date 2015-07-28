#ifndef __THREADQUEUE_H_LOADED__
#define __THREADQUEUE_H_LOADED__

#include "RTL/NoMemberAlignment.h"
#include "RTL/rtl.h"
#ifdef _VMS
#define  remqhi lib$remqhi
#define  remqti lib$remqti
#define  insqhi lib$insqhi
#define  insqti lib$insqti
#elif defined(_OSK) || defined(_WIN32) || defined(__linux)
#include <cstdio>
#include "RTL/que.h"
#endif

/// Definition of queueable objects 
/**
   {\Large{\bf Class BasicQueue}}
   Basic object for queueable objects.
   VMS: The object uses atomic queuing functions to manipulate
   the Queue, which are not interruptable and thread safe.
*/
template <class Type> class BasicQueue : public qentry_t {

public:
  //@Man: Class specific typedefs and enumerations
  /// Indicate status of the Queue
  enum BasicQueueStatus {
    BASICQUEUE_ERROR = 0, 
    BASICQUEUE_SUCCESS = 1,
    BASICQUEUE_ONE_ENTRY = 1409049
  };

public:
  //@Man: Public member functions
  /// Default Constructor: Initialize members
  BasicQueue () : qentry_t(0,0)  {  }

  /// Default Destructor
  virtual ~BasicQueue() {  }

  /// Return pointer to previous element of the Queue
  Type *Previous() {
    return prev;
  }

  /// Return pointer to next element of the Queue
  Type *Next() {
    return next;
  }

  /*// Remove the entry itself from the queue it's in
  // This is not allowed in a multi threaded environment,
  // since there the previous entry might change during 
  // execution!
  int   RemoverEntry()  {
  Type *to_remove = (Type*)this;
  Type *entry, *head = (Type*)((int)to_remove+(int)to_remove->Previous());
  int status = remqhi( head, (void**)&entry );
  if ( SUCCESS(status) ) {
  entry->_pType_Next = entry->_pType_Previous = 0;
  return BASICQUEUE_SUCCESS;
  }
  return status;
  }
  */

  /// Remove (and return) entry from the Head of the Queue
  virtual Type* RemoveHead()  {
    Type *entry = 0;
    int status = remqhi( this, (qentry_t**)&entry);
    if ( status ) {
      return entry;
    }
    return 0;
  }

  /// Remove (and return) entry from the Tail of the Queue
  virtual Type* RemoveTail()  {
    Type *entry = 0;
    int status = remqti( this, (qentry_t**)&entry);
    if ( status ) {
      return entry;
    }
    return 0;
  }

  /// Add entry at the head of the queue
  virtual int AddHead ( Type* entry )  {
    int status = insqhi( entry, this );
    if ( !lib_rtl_is_success(status) )  {
      lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in inqhi. status=%s",status);
    }
    return status;
  }

  /// Add entry at the tail of the queue
  virtual int AddTail( Type* entry )  {
    int status = insqti( entry, this );
    if ( !lib_rtl_is_success(status) )  {
      lib_rtl_signal_message(LIB_RTL_DEFAULT,"Error in inqti. status=%s",status);
    }
    return status;
  }
};

#include "RTL/RestoreMemberAlignment.h"
#endif
