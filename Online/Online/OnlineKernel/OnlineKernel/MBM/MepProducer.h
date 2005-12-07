#ifndef _MEP_PRODUCER_H
#define _MEP_PRODUCER_H

#include "MBM/Producer.h"
#include "MBM/mepdef.h"

/*
 *  MBM namespace declaration
 */
namespace MEP {

  class Producer : public MBM::Producer  {
    typedef MBM::EventDesc EventDesc;
  protected:
    /// MEP buffer include identifier
    MEPID      m_mepID;
    /// Flags to indicate include strategy
    int        m_flags;
  public:
    /// Initializing constructor
    Producer(const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Producer();

    /// Include MBM client into buffer
    virtual int include();

    /// Exclude MBM client into buffer
    virtual int exclude();
  };
}
#endif  // _MEP_PRODUCER_H
