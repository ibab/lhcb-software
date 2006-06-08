#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

/// @todo Restore Particle2VertexAsct, needed for Particle to PV association
/*
// Associator tool
#include "Kernel/Particle2VertexAsct.h"
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

DECLARE_TOOL_FACTORY( Particle2VertexAsct);
DECLARE_DATAOBJECT_FACTORY( Particle2VertexTable);
*/

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciKernel ) {

  DECLARE_TOOL( PhysDesktop ); 
  DECLARE_TOOL( Particle2VertexIPSAsct );
//  DECLARE_DATAOBJECT( Particle2VertexTable );
  
}

