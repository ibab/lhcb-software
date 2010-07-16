
// local
#include "Event/ProcStatus.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProcStatus
//
// 2010-06-25 : Chris Jones
//-----------------------------------------------------------------------------

void LHCb::ProcStatus::addAlgorithmStatus(const std::string& name,
                                          int status)
{
  bool found = false;
  for ( LHCb::ProcStatus::AlgStatusVector::iterator iS = m_algs.begin();
        iS != m_algs.end(); ++iS )
  {
    if ( iS->first == name )
    {
      iS->second = status;
      found = true;
      break;
    }
  }
  if ( !found ) { m_algs.push_back( AlgStatus(name,status) ); }
}

void LHCb::ProcStatus::addAlgorithmStatus(const std::string& algName,
                                          const std::string& subsystem,
                                          const std::string& reason,
                                          const int status,
                                          const bool eventAborted)
{
  // Add abort or OK string
  std::string message = ( eventAborted ? "ABORTED:" : "OK:" );

  // Add sub-system
  message += subsystem + ":";

  // Add reason for the message
  message += reason + ":";

  // Finally, the algorithm name
  message += algName;

  // Set the full message in the algs list
  addAlgorithmStatus(message,status);

  // if event was aborted, set this as well.
  // (Don't set if false, since this will overwrite pre-existing aborts
  if (eventAborted) setAborted(true);
}

int LHCb::ProcStatus::algorithmStatus(const std::string& name) const
{
  int status = 0;
  for ( LHCb::ProcStatus::AlgStatusVector::const_iterator iS = m_algs.begin();
        iS != m_algs.end(); ++iS )
  {
    if ( iS->first == name )
    {
      status = iS->second;
      break;
    }
  }
  return status;
}

bool LHCb::ProcStatus::subSystemAbort(const std::string& subsystem) const
{
  bool isaborted = false;
  if ( aborted() ) 
  {
    // Loop over reports
    for ( LHCb::ProcStatus::AlgStatusVector::const_iterator iS = m_algs.begin();
          iS != m_algs.end(); ++iS )
    {
      // Is this report an ABORT or OK ?
      int colon = (*iS).first.find_first_of(":");
      if ( colon > 0 && (*iS).first.substr(0,colon) == "ABORTED" )
      {
        // This is an abort, but is it the correct subsystem ?
        const std::string tmpS = (*iS).first.substr(colon+1);
        colon = tmpS.find_first_of(":");
        if ( colon > 0 && tmpS.substr(0,colon) == subsystem )
        {
          isaborted = true;
          break;
        }
      }
    }
  }
  return isaborted;
}
