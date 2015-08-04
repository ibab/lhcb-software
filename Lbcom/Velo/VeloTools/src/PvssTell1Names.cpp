#include <sstream>

#include <time.h>

#include "PvssTell1Names.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PvssTell1Names
//
// 2008-09-08 : Kurt Rinnert
//-----------------------------------------------------------------------------

namespace Velo
{
  
  DECLARE_TOOL_FACTORY( PvssTell1Names )
  
  //=============================================================================
  // Standard constructor
  //=============================================================================
  PvssTell1Names::PvssTell1Names( const std::string& type,
      const std::string& name,
      const IInterface* parent )
    : GaudiTool ( type, name , parent )
    {
      declareInterface<Velo::IPvssTell1Names>(this);
    }

  //=============================================================================
  // Destructor
  //=============================================================================
  PvssTell1Names::~PvssTell1Names() {}

  //=============================================================================
  // Initializes init time stamp
  //=============================================================================
  StatusCode PvssTell1Names::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if ( !sc ) return sc;

    return StatusCode::SUCCESS;
  }

  //=============================================================================
  // Finalize
  //=============================================================================
  StatusCode PvssTell1Names::finalize()
  {
    return GaudiTool::finalize();
  }

  //=============================================================================
  // Access to PVSS TELL1 name by sensor number
  //=============================================================================
  const std::string& PvssTell1Names::pvssName( unsigned int sensorNumber )
  {
    return m_pvssNames.pvssName(sensorNumber);
  }

}
