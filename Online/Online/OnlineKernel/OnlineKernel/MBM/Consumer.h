#ifndef _MBM_CONSUMER_H
#define _MBM_CONSUMER_H

#include "MBM/Client.h"

/*
 *  MBM namespace declaration
 */
namespace MBM {

  class Consumer : public Client  {
  protected:

    /// Static Ast to be called on event receival (call virtual getEventAst)
    static int eventAst(void* param);

    /// Static action to be called on event receival
    static int eventAction(unsigned int facility, void* param);

    /// Static event receival rearm
    static int eventRearm(unsigned int facility, void* param);

    /// The event descriptor
    EventDesc  m_event;

  public:
    /// Initializing constructor
    Consumer(const std::string& buffer_name, const std::string& client_name, int partition_id);

    /// Initializing constructor
    Consumer(BMID bmid, const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Consumer();

    /// Switch to non-blocking asynchronous execution mode using WT
    virtual void setNonBlocking(int facility, bool subscribe);

    /// Access to event descriptor
    const EventDesc& event() const {
      return m_event;
    }

    /// Add request to event buffer
    void addRequest(int evtype, int trmask[4], int vetomask[4], int masktype, int usertype, int freqmode, float freq);

    /// Get synchronously event
    int getEvent();

    /// Get event asynchronously
    int getEventAsync();

    /// Release event access
    int freeEvent();

    /// Ast to be called on event receival (may be overloaded by clients)
    virtual int eventAst();

    /// Action to be called on event receival
    virtual int eventAction();

    /// Event receival rearm
    virtual int eventRearm();

    /// Run the application with synchronously 
    virtual int runSynchronous();
  };
}
#endif  // _MBM_CONSUMER_H
