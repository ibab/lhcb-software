// $Id: DaVinciAssociators_load.cpp,v 1.8 2003-01-07 09:29:29 ranjard Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ObjectFactory.h"

// local
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"

// LHCbKernel
#include "Relations/RelationMACROs.h"
#include "Relations/AssociatorMACROs.h"

DECLARE_TOOL_FACTORY( Particle2MCWeightedAsct);
DECLARE_TOOL_FACTORY( Particle2MCAsct);
DECLARE_TOOL_FACTORY( ProtoParticle2MCAsct);

DECLARE_DATAOBJECT_FACTORY( Particle2MCWeightedTable);
DECLARE_DATAOBJECT_FACTORY( Particle2MCTable);
DECLARE_DATAOBJECT_FACTORY( ProtoParticle2MCTable);


// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject )
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES( PhysAssociators ) {

  DECLARE_TOOL( Particle2MCWeightedAsct );
  DECLARE_TOOL( Particle2MCAsct );
  DECLARE_TOOL( ProtoParticle2MCAsct );
 
  DECLARE_DATAOBJECT( Particle2MCWeightedTable );
  DECLARE_DATAOBJECT( Particle2MCTable );
  DECLARE_DATAOBJECT( ProtoParticle2MCTable );
 
  DECLARE_ALGORITHM( Particle2MCWithChi2 );
  DECLARE_ALGORITHM( Particle2MCChi2 );
  DECLARE_ALGORITHM( Particle2MCLinks );
  DECLARE_ALGORITHM( CompositeParticle2MCLinks );

  DECLARE_ALGORITHM( ChargedPP2MC );
  DECLARE_ALGORITHM( NeutralPP2MC ) ;

  DECLARE_ALGORITHM( TestAssociators ) ;
  
}
