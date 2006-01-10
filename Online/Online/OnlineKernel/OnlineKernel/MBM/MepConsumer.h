#ifndef _MEP_CONSUMER_H
#define _MEP_CONSUMER_H

#include "MBM/Consumer.h"
#include "MBM/mepdef.h"

/*
 *  MBM namespace declaration
 */
namespace MEP {

  class Consumer : public MBM::Consumer  {
  protected:
    /// Event dexcriptor definition
    typedef MBM::EventDesc EventDesc;

    /// MEP buffer include identifier
    MEPID      m_mepID;
    /// Flags to indicate include strategy
    int        m_flags;
  public:
    /// Initializing constructor
    Consumer(const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Consumer();

    /// Include MBM client into buffer
    virtual int include();

    /// Exclude MBM client into buffer
    virtual int exclude();
  };
}
#endif  // _MEP_CONSUMER_H
