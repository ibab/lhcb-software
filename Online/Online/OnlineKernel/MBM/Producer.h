#ifndef _MBM_CONSUMER_H
#define _MBM_CONSUMER_H

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

    /// The event descriptor
    EventDesc  m_event;

  public:
    /// Initializing constructor
    Producer(const std::string& buffer_name, const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Producer();

    /// Access to event descriptor
    const EventDesc& event() const {
      return m_event;
    }

    /// Ast to be called on event receival (may be overloaded by clients)
    virtual int spaceAst();

    /// Action to be called on event receival
    virtual int spaceAction();

    /// Event receival rearm
    virtual int spaceRearm(int new_length);
  };
}
#endif  // _MBM_CONSUMER_H
