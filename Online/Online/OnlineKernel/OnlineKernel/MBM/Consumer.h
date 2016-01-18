// $Header: $
// ====================================================================
//
// --------------------------------------------------------------------
//
// Author    : Markus Frank
//
// ====================================================================
#ifndef ONLINE_ONLINEKERNEL_MBM_CONSUMER_H
#define ONLINE_ONLINEKERNEL_MBM_CONSUMER_H

// Framework include files
#include "MBM/Client.h"

///  MBM (Multi-Buffer-Manager) namespace declaration
namespace MBM {

  // Forward declarations 
  class Requirement;

  /// Class describing a MBM Consumer 
  /** 
   *   Every readout function is separated into independent tasks running 
   *   asynchronously in order to derandomize the flow of events. The producer 
   *   task is responsible for reading/assembling data from data sources. 
   *
   *   As soon as the read operation is finished, the data block is declared 
   *   to a managed shared memory area, the buffer manager (BM) and the 
   *   producer is ready for the next read operation.
   *
   *   The consumer task is activated each time an event is declared in the BM. 
   *   The consumer is responsible for releasing the space occupied as 
   *   soon as the consumer has finished processing. Thus, the two activities 
   *   of reading and processing can proceed asynchronously, provided there is 
   *   always sufficient space in the buffer to accommodate at least one read 
   *   operation. 
   *
   *   A BM is managed by a dedicated task executing on each node, which creates 
   *   and initializes the shared memory area. The synchronization between the 
   *   various participating processes has been realized using shared semaphores. 
   *   Any task using the BM registers itself with a name and a partition identifier.
   *   Consumers will only receive data declared by producers with the same 
   *   partition identifier and a boolean accept and veto mask of a width of 128 bits, 
   *   which allows consumers to be specific in the type of data they wish to 
   *   receive. Consumers may request data from the BM in three different ways:
   *   \li The consumer sees all data declared to the BM according to the request mask.
   *   \li A group of consumers see the data exactly once. This mode allows 
   *       executing multiple instances of an application and taking advantage 
   *       of multiple CPU cores.
   *   \li A consumer is served on a best effort basis. In this mode producers 
   *       are always able to declare new data. Possibly pending data, which 
   *       have to be released to satisfy the space requests, are not seen by 
   *       these consumers.
   *
   *   \author  M.Frank
   *   \version 1.0
   *   \date    12/1/2006
   */
  class Consumer : public Client  {
  protected:

    /// Static Ast to be called on event receival (call virtual getEventAst)
    static int eventAst(void* param);

    /// Static action to be called on event receival
    static int eventAction(unsigned int facility, void* param);

    /// Static event receival rearm
    static int eventRearm(unsigned int facility, void* param);

  public:
    /// Initializing constructor
    Consumer(const std::string& buffer_name, 
	     const std::string& client_name, 
	     int partition_id, 
	     int communication_type=BM_COM_FIFO);

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
    void addRequest(const Requirement& req);
    
    /// Add request to event buffer
    void addRequest(int evtype, const unsigned int trmask[4], const unsigned int vetomask[4], int masktype, int usertype, int freqmode, float freq);
    
    /// Delete request to event buffer
    void delRequest(const Requirement& req);

    /// Delete request to event buffer
    void delRequest(int evtype, const unsigned int trmask[4], const unsigned int vetomask[4], int masktype, int usertype);

    /// Get synchronously event
    int getEvent();

    /// Get event asynchronously
    int getEventAsync();

    /// Release event access
    int freeEvent();

    /// Pause event requests
    int pause();

    /// Cancel pending request
    virtual int cancel();

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
#endif  // ONLINE_ONLINEKERNEL_MBM_CONSUMER_H
