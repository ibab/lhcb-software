#ifndef _MBM_PRODUCER_H
#define _MBM_PRODUCER_H

#include "MBM/Client.h"

/*
 *  MBM namespace declaration
 */
namespace MBM {

  class Producer : public Client  {
  protected:
    /// Static Ast to be called on event receival (call virtual getEventAst)
    static int spaceAst(void* param);

    /// Static action to be called on event receival
    static int spaceAction(unsigned int facility, void* param);

    /// Static event receival rearm
    static int spaceRearm(unsigned int facility, void* param);

  public:
    /// Initializing constructor
    Producer(const std::string& buffer_name, const std::string& client_name, int partition_id);

    /// Initializing constructor
    Producer(BMID bmid, const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Producer();

    /// Access to event descriptor
    EventDesc& event() {      return m_event;    }

    /// Access to minial buffer allocation size
    int minAlloc();

    /// Switch to non-blocking asynchronous execution mode using WT
    virtual void setNonBlocking(int facility, bool subscribe);

    /// Cancel pending request
    virtual int cancel();

    /// Ast to be called on event receival (may be overloaded by clients)
    virtual int spaceAst();

    /// Action to be called on event receival
    virtual int spaceAction();

    /// Event receival rearm
    virtual int spaceRearm(int new_length);

    /// Declare event after space receival
    virtual int declareEvent();

    /// Declare event after space receival
    virtual int declareEventTry();

    /// Declare event on space receival with possibility to declare multiple events
    int declareEvent(void** fadd, int* flen);

    /// send event space to consumers
    virtual int sendSpace();

    /// Get space call to fill event data
    virtual int getSpace(int len);

    /// Get space call to fill event data
    virtual int getSpaceTry(int len);

    /// send and declare event to consumers
    virtual int sendEvent();
  };
}
#endif  // _MBM_CONSUMER_H
