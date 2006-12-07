#include "MBM/MepConsumer.h"
#include "WT/wtdef.h"
#include <stdexcept>

// Initializing constructor
MEP::Consumer::Consumer(const std::string& client_name, int partition_id)
: MBM::Consumer(MBM_INV_DESC, client_name, partition_id), m_mepID(MEP_INV_DESC), m_flags(0)
{
}

// Standard destructor
MEP::Consumer::~Consumer()    {
  exclude();
}

/// Include MBM client into buffer
int MEP::Consumer::include()    {
  if ( m_mepID == MEP_INV_DESC )  {
    m_mepID = ::mep_include(m_name.c_str(),m_partID, m_flags);
    if ( m_mepID == MEP_INV_DESC ) {
      throw std::runtime_error("Failed to include into MEP buffers.");
    }
  }
  return MBM_NORMAL;
}

/// Exclude MBM client into buffer
int MEP::Consumer::exclude()    {
  if ( m_mepID != MEP_INV_DESC )  {
    int status = ::mep_exclude(m_mepID);
    if ( status != MBM_NORMAL )  {
      throw std::runtime_error("Failed to exclude from MBM buffers.");
    }
    m_mepID = MEP_INV_DESC;
    m_bmid = MBM_INV_DESC;
  }
  return MBM_NORMAL;
}

/// Pause event requests
int MEP::Consumer::pause()  {
  if ( m_mepID != (BMID)-1 ) {
    int status = ::mep_pause(m_mepID);
    if ( status == MBM_NORMAL )  {
      return MBM_NORMAL;
    }
    throw std::runtime_error("Failed to pause access to MEP buffers [Internal Error]");
  }
  throw std::runtime_error("Failed to pause access to MEP buffers [Buffer not connected]");
}
