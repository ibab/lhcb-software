#include <sstream>

#include <time.h>

#include "TimeStamps.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TimeStamps
//
// 2008-08-21 : Kurt Rinnert
//-----------------------------------------------------------------------------

namespace Velo
{
  
  DECLARE_TOOL_FACTORY( TimeStamps )
  
  //=============================================================================
  // Standard constructor
  //=============================================================================
  TimeStamps::TimeStamps( const std::string& type,
      const std::string& name,
      const IInterface* parent )
    : GaudiTool ( type, name , parent )
    {
      declareInterface<Velo::ITimeStampProvider>(this);
    }

  //=============================================================================
  // Destructor
  //=============================================================================
  TimeStamps::~TimeStamps() {}

  //=============================================================================
  // Initializes init time stamp
  //=============================================================================
  StatusCode TimeStamps::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if ( !sc ) return sc;

    m_initTime = now();  

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  StatusCode TimeStamps::finalize()
  {
    return GaudiTool::finalize();
  }

  //=============================================================================
  // Access to initialization time stamp
  //=============================================================================
  std::string TimeStamps::initTime()
  {
    return m_initTime;
  }

  //=============================================================================
  // Access to current time stamps
  //=============================================================================
  std::string TimeStamps::now()
  {
    time_t currentTime = time(0);
    char buf[32];
    strftime(buf,32,"%Y-%m-%d_%H.%M.%S",localtime(&currentTime));

    return std::string(buf);
  }
}
