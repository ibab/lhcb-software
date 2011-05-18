//
// Include files
// local
#include "Kernel/Particle2UnsignedInts.h"

// anonymous namespace to hide local symbols:
namespace 
{
   
  // ===========================================================================
  /** @var CLID_Candidate 
   *  Class ID definition 
   */
  static const CLID CLID_Particle2UnsignedInts = 882;
  
  // ==========================================================================
}
// ============================================================================
// Class ID 
// ============================================================================
const CLID& DaVinci::Map::Particle2UnsignedInts::classID() 
{
  return CLID_Particle2UnsignedInts;
}
//-----------------------------------------------------------------------------
