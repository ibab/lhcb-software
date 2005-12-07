#include "MBM/MepProducer.h"
#include "WT/wtdef.h"
#include <stdexcept>

// Initializing constructor
MEP::Producer::Producer(const std::string& client_name, int partition_id)
: MBM::Producer(MBM_INV_DESC, client_name, partition_id), m_flags(0) 
{
}

// Standard destructor
MEP::Producer::~Producer()    {
  exclude();
}

/// Include MBM client into buffer
int MEP::Producer::include()    {
  if ( m_mepID != MEP_INV_DESC )  {
    m_mepID = ::mep_include(m_name.c_str(),m_partID, m_flags);
    if ( m_mepID == MEP_INV_DESC ) {
      throw std::runtime_error("Failed to include into MEP buffers.");
    }
  }
  return MBM_NORMAL;
}

/// Exclude MBM client into buffer
int MEP::Producer::exclude()    {
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
