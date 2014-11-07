// $Id: ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
//====================================================================
//
//  Package    : OnlineBase
//  Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
#ifndef ONLINEBASE_MBM_BUFFERINFO
#define ONLINEBASE_MBM_BUFFERINFO

// Framework include files
#include "MBM/Manager.h"

namespace MBM {

  // Forward declarations
  struct Manager;

  /// API to access buffer manager information for monitoring purposes.
  /*
   * @author  M.Frank
   * @version 1.0
   * @date    10/01/2006
   */
  class BufferInfo {
    MBM::Manager* m_mgr;
  protected:
    /// Internal helper function
    template <typename FUNCTOR> long loop_users(FUNCTOR& fcn, int cons, int prod, int other) const;

  public:
    /// Default constructor
    BufferInfo();
    /// Constructor taking already mapped buffers
    BufferInfo(ServerBMID bm);
    /// Standard destructor
    virtual ~BufferInfo();
    /// Buffer name
    std::string name() const;
    /// Attach to buffer
    int attach(const char* name);
    /// Detach from buffer
    int detach();
    /// Accessor for the total number of events produces
    long tot_produced()  const;
    /// Accessor for the total number of events actual
    long tot_actual()  const;
    /// Accessor for the total number of events seen
    long tot_seen()  const;
    /// Accessor for the number of event slots
    long max_events() const;
    /// Accessor for the number of user slots
    long max_users() const;
    /// Accessor for the buffer space in bytes
    long max_space() const;
    /// Accessor for the number of the event slots used
    long num_events() const;
    /// Accessor for the number of the user slots used
    long num_users() const;
    /// Accessor for the number of the free space in bytes
    long num_space() const;
    /// Count the number of identified consumers in this buffer
    long num_consumers()  const;
    /// Count the number of identified consumers in this buffer with requests pending of type 'ev_type'
    long num_consumers_evtype(int ev_type)  const;
    /// Count the number of identified consumers in this buffer with requests pending
    long num_consumers_partid(int partid)  const;
    /// Count the number of identified consumers in this buffer with requests pending
    long num_consumers_partid_evtype(int partid, int ev_type)  const;
    /// Count the number of identified producers in this buffer
    long num_producers()  const;
    /// Count the number of identified producers in this buffer
    long num_producers_partid(int partid)  const;

  };
}
#endif // ONLINEBASE_MBM_BUFFERINFO
