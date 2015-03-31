
// Event
#include "Event/CaloClusterEntry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloClusterEntry
//
// 2015-03-31 : Chris Jones
//-----------------------------------------------------------------------------

LHCb::CaloClusterEntry* LHCb::CaloClusterEntry::clone() const 
{
  return new LHCb::CaloClusterEntry( *this );
}
