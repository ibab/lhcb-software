// $Id: DaVinciKernel_load.cpp,v 1.3 2006-01-17 12:40:08 pkoppenb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// Associator tool
#include "Kernel/Particle2VertexAsct.h"
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

DECLARE_TOOL_FACTORY( Particle2VertexAsct);
DECLARE_DATAOBJECT_FACTORY( Particle2VertexTable);

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( DaVinciKernel ) {

  DECLARE_TOOL( PhysDesktop ); 
  
}

